/********************************************************************
 ******* Changes COPYRIGHT (c) 2015 by KE0FF, Taylor, TX   **********
 *
 *  File name: SSI2.c
 *
 *  Module:    Control
 *
 *  Summary:
 *  This is the main file for the SSI2 driver (LCD data output).
 *  The SSI runs as a 16b Freescale master, MOSI & SPCK only (no MISO or FSS).
 *
 *  Project scope declarations revision history:
 *    03-22-22 jmh:  creation date
 *
 *******************************************************************/

/********************************************************************
 *  File scope declarations revision history:
 *    03-22-22 jmh:  creation date
 *
 *******************************************************************/

// NOT-USED: SSI3CLK (SCLK, pin 5) connected to PQ0
// NOT-USED: SSI3FSS (/CS, pin 6) connected to PQ1
// NOT-USED: SSI3TX (MOSI, pin 11) connected to PQ2
// SSI3RX (MISO, pin 27) connected to PQ3

#include <stdint.h>
#include <stdio.h>
#include "inc/tm4c1294ncpdt.h"
#include "inc/tm4c1294ncpdt_PCTL.h"
#include "nvic.h"
#include "SSI2.h"
#include "init.h"
#include "lcd.h"

union	LCDREG	lcdsegs[4];			// lcd bitmap
union	LCDREG	blinksegs[4];		// lcd blinkmap
union	LCDREG	lcdcbbuf[5][4];		// lcd ssi2 circ buffer, 5 entries of 4 elements each
U8	cbh;							// circ buff head index
U8	cbt;							// circ buff tail index
U8	mst;							// msg tail index
U8	ipl_t2b;						// timer 2 ipl flag
U8	blinky_lcd;						// LCD blink counter

//******** SSI2_Init *****************
// Initialize SSI2, 16b, 1.875MHz clk

void ssi2_init(void){

	lcdsegs[0].bigt = (((uint64_t)(LCD_CE1))<<56) | 0x4;	// init lcd segment registers
	lcdsegs[1].bigt = (((uint64_t)(LCD_CE1))<<56) | 0x1;
	lcdsegs[2].bigt = (((uint64_t)(LCD_CE2))<<56) | 0x4;
	lcdsegs[3].bigt = (((uint64_t)(LCD_CE2))<<56) | 0x1;
	NVIC_DIS1_R = NVIC_EN1_SSI2;
	ipl_t2b = 1;								// trigger ipl reset of ISR
	cbh = 0;
	cbt = 0;
	mst = 0;
	SYSCTL_RCGCSSI_R |= SYSCTL_RCGCSSI_R2;		// activate SSI2
	GPIO_PORTD_AHB_AFSEL_R |= LCD_CLK | LCD_DATA; // enable alt funct on PD
	GPIO_PORTD_AHB_PCTL_R = GPIO_PCTL_PD3_SSI2CLK|GPIO_PCTL_PD1_SSI2XDAT0;
	GPIO_PORTD_AHB_AMSEL_R &= ~(LCD_CLK | LCD_DATA); // disable analog functionality on PD
												// BR for data2 = 100000
												// SSI3CLK = SYSCLK / (CPSDVSR * (1 + SCR)) { 2 <= CPSDVSR <= 254, even only)
	SSI2_CR1_R = 0x00000000;					// disable SSI, master mode
	SSI2_CPSR_R = (SYSCLK / ((1 + SSI2_SCR) * SSI2_BR));
	SSI2_CR0_R = (SSI2_SCR << 8) | SSI_CR0_DSS_8;		// SCR = [15:8], SPH[7] = 0, SPO[6] = 0 Freescale, DSS = 8-bit data
	SSI2_IM_R = 0;								// disable ISR
	SSI2_CR1_R |= SSI_CR1_SSE;					// enable SSI
	SSI2_ICR_R = SSI_ICR_EOTIC;
	NVIC_EN1_R = NVIC_EN1_SSI2;

	wait(2);
	GPIO_PORTL_DATA_R = LCD_RSTN;				// release driver IC inhibit (need to wait, elsewhere, 200ms for LCD reset to clear)
	return;
}

//******** set segments *****************
// copies source array into target
//	sptr = source pointer
//	targ = targ selector, 1=lcd, else blink

void set_seg(uint64_t* sptr, uint8_t targ){
	uint8_t		i;
	union LCDREG*		tptr;

	if(targ==NOBLINK) tptr = lcdsegs;
	else tptr = blinksegs;
	for(i=0; i<MS_MAX; i++){
		tptr[i].bigt = sptr[i];
	}
	return;
}

//******** set segments *****************
// source array bits that are set get cleared in the target
//	sptr = source pointer
//	targ = targ selector, 1=lcd, else blink
// Just clears one word

void clear_seg(uint64_t srcval, uint8_t targ, uint8_t indx){
	union LCDREG*		tptr;

	if(targ==NOBLINK) tptr = lcdsegs;
	else tptr = blinksegs;
	tptr[indx].bigt &= ~srcval;
	return;
}

//******** set segments *****************
// source array bits that are set get OR'd with the target
//	sptr = source pointer
//	targ = targ selector, 1=lcd, else blink

void or_seg(uint64_t srcval, uint8_t targ, uint8_t indx){
	union LCDREG*		tptr;

	if(targ==NOBLINK) tptr = lcdsegs;
	else tptr = blinksegs;
	tptr[indx].bigt |= srcval;
	return;
}

//******** set segments *****************
// copies source array to target
//	tptr = targ pointer
//	src = source selector, 1=lcd, else blink
//			if hi-bit of src low, only copy byte7 & low nybble

void get_seg(uint64_t* tptr, uint8_t src){
	uint8_t		i;
	union LCDREG*		sptr;

	i = src & 0x01;
	if(i==NOBLINK) sptr = lcdsegs;
	else sptr = blinksegs;
	if(src&0x80){
		for(i=0; i<MS_MAX; i++){
			tptr[i] = sptr[i].bigt;
		}
	}else{
		for(i=0; i<MS_MAX; i++){
			tptr[i] = sptr[i].bigt & 0x00ff00000000000f;
		}
	}
	return;
}

// xor_seg()



//******** get blink *****************
// returns low bit of blink counter

uint8_t get_blink(void){

	return (blinky_lcd & 0x01);
}

//******** lcd_send *****************
// start ssi, fill ssi fifo
//	targ = targ selector, 1=lcd, else blink

void lcd_send(uint8_t targ){
	uint8_t		i;		// temps
	uint8_t		cb;

	SSI2_IM_R = 0;								// disable end of tx ISR
	cb = cbh+1;									// advance circbuff# (as a temp var)
	if(cb >= CB_MAX){
		cb = 0;
	}
	while(cb == cbt);							// wait for buffer to clear
	if(targ){
		for(i=0; i<4; i++){
			lcdcbbuf[cbh][i].bigt = lcdsegs[i].bigt;
		}
	}else{
		for(i=0; i<4; i++){
			lcdcbbuf[cbh][i].bigt = (lcdsegs[i].bigt & blinksegs[i].bigt);
		}
	}
	if(cbh == cbt){								// kick-start ssi
		// preset head/tail
		mst = 1;
		// set CE
		GPIO_PORTD_AHB_DATA_R |= lcdcbbuf[cbh][0].b[MSB_MAX-1];
		// send data
		for(i=MSB_MAX-2; i<MSB_MAX; i--){
			while(!(SSI2_SR_R & SSI_SR_TNF));	// SSI Transmit FIFO Not Full (failsafe trap)
			SSI2_DR_R = lcdcbbuf[cbt][0].b[i];  // send data out
		}
	}
	// copy temp head to real one
	cbh = cb;
	SSI2_IM_R = SSI_IM_EOTIM;					// enable end of tx ISR
/*
	SSI2_IM_R = SSI_IM_EOTIM					// enable ISR
	NVIC_EN1_R = NVIC_EN1_SSI2;
 */
	return;
}

// ***************************************************************************
//*****************************************************************************
// ***************************************************************************

//-----------------------------------------------------------------------------
// SSI2_ISR
// Sends next datastream msg to LCD chips.  Datastream message is defined by
//	the LCDREG union type.  The high byte (i[0]) holds the CE settings.
//	cbh is the array head pointer, cbt is the tail.
//	This scheme depends on the FIFO being at least 7 bytes long.  The FIFO is
//	filled all at once within this interrupt.
//-----------------------------------------------------------------------------
void SSI2_ISR(void){
	U8	i;

	GPIO_PORTD_AHB_DATA_R &= ~(LCD_CE2 | LCD_CE1);			// CE = low
	for(i=0;i<10;i++);
	SSI2_ICR_R = SSI_ICR_EOTIC;
	if(cbh != cbt){											// still data to send
		// set CE
		GPIO_PORTD_AHB_DATA_R |= lcdcbbuf[cbt][mst].b[MSB_MAX-1];
		// send data
		for(i=MSB_MAX-2; i<MSB_MAX; i--){
			while(!(SSI2_SR_R & SSI_SR_TNF));				// SSI Transmit FIFO Not Full (failsafe trap)
			SSI2_DR_R = lcdcbbuf[cbt][mst].b[i];        	// send data out
		}
		// advance word tail
		mst += 1;
		if(mst >= MS_MAX){
			mst = 0;
			// advance word tail
			cbt += 1;
			if(cbt == CB_MAX){
				cbt = 0;
			}
		}
	}
	return;
}

//-----------------------------------------------------------------------------
// Timer2B_ISR
//	Paces LCD messages and does 1 sec blink cycle
//-----------------------------------------------------------------------------
#define	PRESCALE2B	25					// 100hz * (100/200)(sec) = 50: number of prescale cycles in 1/2 sec

void Timer2B_ISR(void){
	static	U16	ps2b;

	if(ipl_t2b){
		ipl_t2b = 0;
		ps2b = PRESCALE2B;
		blinky_lcd = 0;
	}
	if(TIMER2_MIS_R & TIMER_MIS_TBTOMIS){
		if(--ps2b == 0){
			ps2b = PRESCALE2B;
			// brocess blink function
			blinky_lcd += 1;
			lcd_send(blinky_lcd & 0x01);
		}
	}
	TIMER2_ICR_R = TIMERB_MIS_MASK;								// clear all 2B-intr
	return;
}

// EOF
