/********************************************************************
 ******* Changes COPYRIGHT (c) 2022 by KE0FF, Taylor, TX   **********
 *
 *  File name: SSI0.c
 *
 *  Module:    Control
 *
 *  Summary:
 *  This is the main file for the SSI0 driver (NVSRAM memory).
 *  The ANV32AA1ADK66 T is used to retain the radio state and memories.
 *  It features a shadow SRAM that is linked via an SPI interface.
 *  Fsclk(max) = 66 MHz -- we'll use something in the 3 MHz range
 *
 *******************************************************************/

/********************************************************************
 *  File scope declarations revision history:
 *    03-09-22 jmh:  creation date
 *
 *******************************************************************/

#include <stdint.h>
#include "inc/tm4c1294ncpdt.h"
#include "inc/tm4c1294ncpdt_PCTL.h"
#include "SSI0.h"
#include "init.h"


//******** SSI0_Init *****************
// Initialize NVSRAM interface

void ssi0_init(void){

	SYSCTL_RCGCSSI_R |= SYSCTL_RCGCSSI_R0;				// activate SSI0
	GPIO_PORTA_AHB_AFSEL_R |= NVMISO|NVMOSI|NVCLK;		// enable alt funct on PQ0-3
	GPIO_PORTA_AHB_PCTL_R = GPIO_PCTL_PA2_SSI0CLK | GPIO_PCTL_PA4_SSI0XDAT0 | GPIO_PCTL_PA5_SSI0XDAT1;	//0x0000eeee;
	GPIO_PORTA_AHB_AMSEL_R &= ~(NVMISO|NVMOSI|NVCLK);	// disable analog functionality on PQ
	SSI0_CR1_R = 0;										// disable SSI, master mode
														// BR for NVRAM = 1 MHz
														// SSI0CLK = SYSCLK / (CPSDVSR * (1 + SCR)) { 2 <= CPSDVSR <= 254, even only)
	SSI0_CPSR_R = SSI0_DVSR;
	SSI0_CR0_R = (SSI0_SCR << 8) | SSI_CR0_DSS_8;		// SCR = [15:8], SPH[7] = 0, SPO[6] = 0 Freescale, DSS = 8-bit data
	SSI0_CR1_R |= SSI_CR1_SSE;							// enable SSI
	return;
}

//*************************************
// write_sram_cmd()
//	send WE, WD, WSR, or BE cmds to flash chip

uint8_t shift_SSI0(uint8_t sdata){

	while(!(SSI0_SR_R & SSI_SR_TNF));					// SSI Transmit FIFO Not Full
	SSI0_DR_R = sdata;                					// data out, no reply
	while(!(SSI0_SR_R & SSI_SR_RNE));					// wait for RX flag to catch up (should never have to wait here long)
	return SSI0_DR_R;
}

//*************************************
// read_SSI0()
//	fetches data from SSI buffer

uint8_t read_SSI0(uint8_t dataw){
	volatile uint8_t	i;

	while((SSI0_SR_R & SSI_SR_RNE)){					// failsafe... clear out buffer
		i = SSI0_DR_R;
	}
	i = shift_SSI0(dataw);								// send data (ye must send, that ye may ra-ceive...)
	return i;
}

/* this is kept for reference as a commented section !!! DO NOT UNCOMMENT !!!
//-----------------------------------------------------------------------------
// shift_spi() drives shift engine for bit-bang SSI
//	returns receive data (requires NVRAM CS to be low to receive NVRAM data,
//	else all you get is the lock switch status)
//	Uses ssiflag trigger from Timer1B_ISR to apply the clock period to each edge
//-----------------------------------------------------------------------------
U8 shift_spi(U8 dato){
	U8	i;
	U8	datain = 0;

	for(i=0x80;i;i >>= 1){
		if(i & dato) GPIO_PORTD_DATA_R |= MOSI_N;		// set MOSI
		else GPIO_PORTD_DATA_R &= ~MOSI_N;				// clear MOSI
		GPIO_PORTD_DATA_R |= SCK;						// clr SCK (it is inverted before reaching the slave devices)
		ssiflag = 0;
		while(!ssiflag);								// delay 1/2 bit time
		ssiflag = 0;
		if(GPIO_PORTB_DATA_R & MISO_LOCK) datain |= i;	// capture MISO == 1
		GPIO_PORTD_DATA_R &= ~SCK;						// set SCK
		ssiflag = 0;
		while(!ssiflag);								// delay 1/2 bit time
	}
	ssiflag = 0;
	while(!ssiflag);									// delay 1/2 bit time
	return datain;
}*/

//************************************************************************
// NVRAM support Fns
//************************************************************************

/****************
 * open_nvr starts an spi message by lowering RAMCS_N
 */
void open_nvr(void)
{

//	putchar_bQ('c');
//	TIMER1_CTL_R |= (TIMER_CTL_TBEN);					// enable bit timer
	GPIO_PORTA_AHB_DATA_R &= ~NVCS_N;					// open NVRAM
//	ssiflag = 0;
//	while(!ssiflag);									// sync to bit timer ISR & setup time
	return;
}

/****************
 * close_nvr closes an spi message by raising RAMCS_N
 */
void close_nvr(void)
{
//	uint32_t	i;

	while(SSI0_SR_R & SSI_SR_BSY);
//	for(i=0; i<40000; i++);
	GPIO_PORTA_AHB_DATA_R |= NVCS_N;					// close NVRAM
//	ssiflag = 0;
//	while(!ssiflag);									// hold time (1/2 bit)
//	TIMER1_CTL_R &= ~(TIMER_CTL_TBEN);					// disable timer
//	putchar_bQ('d');
	return;
}

/****************
 * wen_nvr sends write enable cmd to the NVRAM
 */
void wen_nvr(void)
{

	open_nvr();
	shift_SSI0(WREN);
	close_nvr();
	return;
}

/****************
 * storecall_nvr sends store or recall cmd to the NVRAM
 *	tf_fl == 1 is store, else recall
 */
void storecall_nvr(uint8_t tf_fl)
{

	open_nvr();
	if(tf_fl) shift_SSI0(STORE);
	else shift_SSI0(RECALL);
	close_nvr();
	return;
}

/****************
 * rws_nvr sends status-rw cmd to the NVRAM
 *
 *	set hi-bit of mode (CS_WRITE) to signal write
 */
uint8_t rws_nvr(uint8_t dataw, uint8_t mode)
{
	uint8_t	i;		// temp

	if(mode & CS_WRITE) i = WRSR;
	else i = RDSR;
	open_nvr();
	shift_SSI0(i);
	i = read_SSI0(dataw);
	close_nvr();
	return i;
}

/****************
 * rw8_nvr sends byte-rw cmd to the NVRAM
 *
 * Supports repeated write/read based on state of mode flag:
 *	if mode = OPEN, assert CS and send r/w cmd and address
 *	if mode = CLOSE, de-assert CS at end
 *	else, just send/rx data byte
 *	set hi-bit of mode (CS_WRITE) to signal write
 */
uint8_t rw8_nvr(uint32_t addr, uint8_t dataw, uint8_t mode)
{
	uint8_t	i;		// temp

	if(mode & CS_OPEN){
		if(mode & CS_WRITE){
			i = WRITE;
			wen_nvr();									// have to enable writes for every cycle
		}else{
			i = READ;
		}
		open_nvr();
		shift_SSI0(i);
		shift_SSI0((uint8_t)(addr >> 16));
		shift_SSI0((uint8_t)(addr >> 8));
		shift_SSI0((uint8_t)(addr));
	}
	i = read_SSI0(dataw);
	if(mode & CS_CLOSE){
		close_nvr();
	}
	return i;
}

/****************
 * rw16_nvr sends word-rw cmd to the NVRAM
 *
 * Supports same modes as rw8_nvr()
 * Writes data low byte first, high byte last
 */
uint16_t rw16_nvr(uint32_t addr, uint16_t dataw, uint8_t mode)
{
	uint8_t	i = 0;		// temp
	uint16_t	ii;

	if(mode & CS_WRITE){
		i = (uint8_t)dataw;
	}
	ii = (uint16_t)rw8_nvr(addr, i, (mode&(CS_WRITE | CS_OPEN)) );
	if(mode & CS_WRITE) i = (uint8_t)(dataw >> 8);
	ii |= ((uint16_t)rw8_nvr(addr+1, i, mode & CS_CLOSE)) << 8;
	return ii;
}

/****************
 * rw32_nvr sends wword-rw cmd to the NVRAM
 *
 * Supports same modes as rw8_nvr()
 * Writes data low byte first, high byte last
 */
uint32_t rw32_nvr(uint32_t addr, uint32_t dataw, uint8_t mode)
{
	uint8_t	i = 0;		// temp
//	uint8_t	j;
	uint32_t	ii;
	uint32_t	jj = dataw;

	// first (low) byte:
	if(mode & CS_WRITE){
		i = (uint8_t)jj;
	}
	ii = (uint32_t)rw8_nvr(addr, i, (mode&(CS_WRITE | CS_OPEN)) );
	// 2nd byte:
	if(mode & CS_WRITE){
		jj >>= 8;
		i = (uint8_t)jj;
	}
	ii |= (uint32_t)rw8_nvr(addr+1, i, (mode&(CS_WRITE)) ) << 8;
	// 3rd byte:
	if(mode & CS_WRITE){
		jj >>= 8;
		i = (uint8_t)jj;
	}
	ii |= (uint32_t)rw8_nvr(addr+2, i, (mode&(CS_WRITE)) ) << 16;
	// 4th (hi) byte:
	if(mode & CS_WRITE){
		jj >>= 8;
		i = (uint8_t)jj;
	}
	ii |= (uint32_t)rw8_nvr(addr+3, i, (mode&(CS_WRITE | CS_CLOSE)) ) << 24;

	return ii;
}

/****************
 * rwusn_nvr r/w NVRAM user seria#
 *
 */
void rwusn_nvr(uint8_t* dptr, uint8_t mode)
{
	uint8_t	i;
	uint8_t	j;

	if(mode&CS_WRITE) wen_nvr();
	open_nvr();
	if(mode&CS_WRITE) shift_SSI0(WRSNR);
	else shift_SSI0(RDSNR);
	for(i=0; i<16; i++, dptr++){
		j = read_SSI0(*dptr);
		if(!(mode&CS_WRITE)) *dptr = j;
	}
	close_nvr();
	return;
}
