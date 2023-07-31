/********************************************************************
 ************ COPYRIGHT (c) 2021 by ke0ff, Taylor, TX   *************
 *
 *  File name: sio.c
 *
 *  Module:    Control
 *
 *  Summary:
 *  This is the source file for the IC-900 RDU Clone application
 *  async serial I/O.
 *
 *******************************************************************/

/********************************************************************
 *  File scope declarations revision history:
 *    06-29-21 jmh:  creation date
 *
 *******************************************************************/

#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include "inc/tm4c1294ncpdt.h"
#include "inc/tm4c1294ncpdt_PCTL.h"
#include "nvic.h"
#include "typedef.h"
#include "init.h"						// App-specific SFR Definitions
#include "sio.h"
#include "tiva_init.h"
#include "serial.h"
#include "radio.h"
#include "ssi1.h"
#include "ssi3.h"

//-----------------------------------------------------------------------------
// local declarations
//-----------------------------------------------------------------------------

U32	sin_perr;				// sin status registers
U8	sin_error;
U32	sin_mask;
U8	sin_hptrm1;				// sin head/tail indecies
U8	sin_hptr;
U8	sin_tptr;
#define	SIN_MAX	10
U16	sin_buf[SIN_MAX];		// sin buffer array
U16	sinstat_buf[SIN_MAX];	// sin SSI status buffer array
U16	sin_dr;
U8	tmr_pingpong;			// Timer2A mode register

//-----------------------------------------------------------------------------
// ***** START OF CODE *****
//-----------------------------------------------------------------------------

/****************
 * init_sio is called at IPL and initializes the data structures for the sio Fns
 * SSI1 is the 40b sync/async data output master.  Clock is pinned out, but is only used for the base-unit direct connect (not used for the EX-766 connection)
 * SSI3 is the 16b async dat input master.  This serial transfer is initiated by a GPIO trigger on PQ3.  This triggers a timer delay (timer 2A) of one bit time
 * 		at which point the Timer2A_ISR triggers an SPI transfer (a single word of 16b).  SSI3_ISR transfers the received data word to the buffer and then re-cycles
 * 		the GPIO trigger to arm for the next transfer.
 * 		Since the start bit is skipped, the received data word has only data bits.  !! This differs from the IC-900 implementation !!
 */
U32 init_sio(U32 sys_clk, U8 flush)
{
	U32				i;				// temp U32
	U32				iplr = 0;
	volatile U32	ui32Loop;

	// init local variables and buffers
	sin_error = 0;
	sin_perr = 0;
	sin_mask = 0;
	sin_hptrm1 = 0;
	sin_hptr = 0;
	sin_tptr = 0;
	sin_buf[sin_hptr] = 0;
	for(i=0; i<SIN_MAX; i++){								// clear buffer entries
		sin_buf[i] = 0;
		sinstat_buf[i] = 0;
	}

	// init ssi1 (4800 baud, 8b, async serial out)
	//	drives 40b message output (5, 8bit values written into the FIFO one after the other), the SO bitmap is as follows
	//	BIT#		DESCRIPTION
	//	39			start, always cleared to 0
	//	[38:35]		sub-module addr
	//	[34:24]		control data
	//	[23:05]		data
	//	[04:00]		stop, always set to 1 except for WBRX module

	// SOUT config is in SSI1.c
	if(!flush){
		ssi1_init();										// init SSI1 output plus clock (master)
	}
	// SIN config
	SYSCTL_RCGCSSI_R |= SYSCTL_RCGCSSI_R3;					// activate SSI3 clock domain
	GPIO_PORTQ_IM_R = 0x00;									// config PQ3 edge interrupt
	GPIO_PORTQ_IEV_R &= ~DATA2;								// falling edge
	GPIO_PORTQ_IBE_R &= ~DATA2;								// one edge
	GPIO_PORTQ_IS_R &= ~DATA2;								// edge ints
	GPIO_PORTQ_ICR_R = 0xff;								// clear int flags
	GPIO_PORTQ_IM_R |= (DATA2);								// enable SIN edge intr
	// init timer2A (1-bit timer for DATA2 start-bit)
	timer2A_init(sys_clk);
	// enable ISRs
	NVIC_EN0_R = NVIC_EN0_TIMER2A;							// enable timer2A intr
	NVIC_EN2_R = NVIC_EN2_GPIO_PORTQ;						// enable GPIOQ edge intr
	iplr = IPL_ASIOINIT;
	return iplr;
}

//**********************************************************************************************//
//																								//
//	Serial Out via SSI1TX to create a 4800 baud, 32b data path (1 start bit, 1 stop bit)		//
//	Requires an inverter to get it to work right since the TIVA SPI returns to zero between		//
//	transfers.																					//
//																								//
//**********************************************************************************************//

/****************
 * send_so sends 4800 baud SYNC/ASYNC data using SSI1 tx output.
 *  CLOCK is sent, but is not used for EX766 interface, only for direct connect interface.
 *  This is configured mechanically per the IC-901 operating manual.
 * 	data is 39 bits, justified against bit 39 (left)  -- bit 40 is start bit.
 * send_so() adds the start bit by masking off bit 40, and then stores
 * 	the transfer as 5, 8-bit values to the SPI data register/FIFO
 */
void send_so(uint64_t data){
	uint8_t		dptr[5];
	uint8_t 	jk;

	data = (~data);
	for(jk=5; jk!=0; jk--){
		dptr[jk-1] = (uint8_t)(data & 0xff);
		data >>= 8;
	}
	dptr[0] |= 0x80;										// set start bit
	if(SSI1_SR_R & SSI_SR_BSY){								// wait for previous msg to clear
		wait(!(SSI1_SR_R & SSI_SR_BSY));
//		wait(2);											// add extra time to complete 2nd stop bit
	}
	for(jk=0; jk<5; jk++){
		SSI1_DR_R = dptr[jk];
	}
	return;
}

//**********************************************************************************************//
//																								//
//	Serial IN via PF4 & timer2a to create a 4800 baud, 31b data path (1 start bit, 16 stop bit)	//
//																								//
//**********************************************************************************************//

//-----------------------------------------------------------------------------
// get_sin looks for chr in input buffer.  If none, return '\0'
//	uses circular buffer sin_buf[] which is filled in the TIMER2A interrupt
//-----------------------------------------------------------------------------
U32 get_sin(void){
	U32 c = 0L;

	if(sin_tptr != sin_hptr){								// if head != tail,
		c = sin_buf[sin_tptr++];							// get chr from circ-buff and update tail ptr
		if(sin_tptr >= SIN_MAX){							// process tail wrap-around
			sin_tptr = 0;
		}
	}
	return c;
}

//-----------------------------------------------------------------------------
// got_sin checks if there are any sin chrs in the buffer.
//-----------------------------------------------------------------------------
char got_sin(void){
	char c = FALSE;			// return val, default to "no chr"

    if(sin_tptr != sin_hptr){								// if (head != tail) && there is at least 1 msg..
    	c = TRUE;											// .. set chr ready to get flag
	}
	return c;
}

//-----------------------------------------------------------------------------
// flush_sin empties the input buffer.
//-----------------------------------------------------------------------------
void flush_sin(void){

	GPIO_PORTQ_IM_R &= ~(DATA2);							// disable SIN edge and timer intr
	TIMER2_CTL_R &= ~(TIMER_CTL_TAEN);
	init_sio(SYSCLK, FLUSH_SIN);
	return;
}

//-----------------------------------------------------------------------------
// get_error() returns framing error count
//-----------------------------------------------------------------------------
U32 get_error(void){
	U32	i;		// temp

	i = sin_perr;
	sin_perr = 0;
	return i;
}

//-----------------------------------------------------------------------------
// print_ptr() prints t/h ptrs
//-----------------------------------------------------------------------------
void print_ptr(void){
	char dbuf[25];

	sprintf(dbuf,"h%d,t%d", sin_hptr, sin_tptr);
	putsQ(dbuf);
	return;
}

// **************************************************************************\\
//
// SIN async data reception ISRs
//
//	SIN is received through a combination of GPIO edge and timer interrupts.
//	The data is received by using SSI3 to clock in the async data word (N16-2)
//	Only the RXD pin of the SSI is used, clock and TXD are not enabled.
//
//	The GPIO edge (falling) triggers the start of word reception.  This ISR triggers
//	Timer2A one-half bit later (START_BIT).  The timer ISR has two modes: START_BIT
//	and EOT_WAIT.  In START_BIT mode, the ISR triggers a 16b SSI word reception
//	by sending a 16b word (since there is no TX analog for this resource, the data
//	is DON'T-CARE), then resetting the timer to interrupt 18 bit times later
//	followed by setting the EOT_WAIT mode.
//
//	When the timer EOT_WAIT mode ISR occurs, the received data is ready in the SSI data
//	register, so the Timer2A ISR code reads that data and places it into the receive buffer
//	structure before recycling the system to disable Timer2A & SSI3 and arm the GPIO edge
//	detect for the next word.
//
// The reason for this convolution is due to the nature of the algorithm and the
//	peculiarities of the TI peripherals (some of these are by design, and others
//	are by errata).  Using SSI3 saves processing overhead by limiting the ISR events.
//	Here, there are 3 ISR events per received word: one for the start bit edge, another
//	for the 1st bit alignment, and the 3rd when the reception is complete.  Without SSI3,
//	we would still need the GPIO edge interrupt, then 17 timer interrupts to clock in
//	each data bit of the word (plus the start bit).  It is arguable that the timer ISR
//	would be at least as complicated as it is now which equates to a savings of 15 ISR
//	events per received word.  The received data period is on the order of 6.5ms, which
//	equates to a savings of about 2300 ISR events per second.
//
// The SSI3 ISR was depricated due to issues with the EOT interrupt not working correctly
//	(or not documented clearly).  Timer2A was re-tasked to produce an ersatz EOT ISR event
//	after the SSI word transfer time expires (plus a couple of stop-bit delays).
//
//-----------------------------------------------------------------------------
// gpioq_isr
//-----------------------------------------------------------------------------
//
// GPIO_PORTQ isr, processes the SIN start bit detect
//		* disables the PQ3 GPIO ISR
//		* inits timer mode setting
//		* starts timer2 at 1/2-bit time
//		* enables SSI3 (but doesn't trigger a transfer)

void gpioq_isr(void){
	GPIO_PORTQ_IM_R = 0;									// disable SIN edge intr
	GPIO_PORTQ_ICR_R = 0xff;								// pre-clear int flags
	NVIC_DIS2_R = NVIC_EN2_GPIO_PORTQ;						// disable GPIOQ edge intr

	tmr_pingpong = START_BIT;								// set start-bit mode for timer
	TIMER2_TAILR_R = (uint16_t)(SIN_START_BIT_TIME);		// start-bit time is 1/2 the baud rate to allow the SSI clock to align to the mid-point of the rcv bit frames
	TIMER2_ICR_R = TIMERA_MIS_MASK;							// pre-clear A-intr
	TIMER2_CTL_R |= (TIMER_CTL_TAEN);						// enable timer
	ssi3_init();											// enable SSI
	return;
}

//-----------------------------------------------------------------------------
// Timer2_ISR
//-----------------------------------------------------------------------------
//
// Called when timer2A overflows:
//	This ISR sequences the SSI3 clock alignment and ersatz EOT events.  At EOT, it gathers data
//	from the completed SSI3 receive cycle to feed into the sin[] buffer structure.
//		pingpong = START_BIT is the start-bit alignment mode.  The ISR sends an SSI word to trigger
//			reception of the incoming data that has now been aligned to the center of the data
//			bit lanes.  The first bit clocked is the data MSb (the start bit is not captured by the SSI)
//		pingpong = EOT_WAIT signifies the end of the RX cycle.  The ISR pulls in the SSI data and
//			places it into the sin[] circular buffer.  Note: Buffer overflow errors cause the oldest
//			data to be lost.  This mode then disables Timer2A and SSI3 and re-arms the GPIO edge ISR
//
//-----------------------------------------------------------------------------

void Timer2A_ISR(void)
{

	if(tmr_pingpong == EOT_WAIT){
		sinstat_buf[sin_hptr] = SSI3_RIS_R;					// buffer the SSI status
		SSI3_ICR_R = 0xff;									// clear SSI flags
		if(sinstat_buf[sin_hptr] != NORM_SSI_STAT){
			sin_perr |= SIN_SSIERR;							// set SSI error
		}
		sin_buf[sin_hptr++] = SSI3_DR_R;					// put rcvd word into buffer
		if(sin_hptr >= SIN_MAX){							// process head wrap-around
			sin_hptr = 0;
		}
		if(sin_hptr == sin_tptr){							// trap buffer overflow
			sin_perr |= SIN_BUFOVR;							// set overflow error
			sin_tptr += 1;									// strip oldest entry from buffer
			if(sin_tptr >= SIN_MAX){						// process tail wrap-around
				sin_tptr = 0;
			}
		}
		// flip-flop to GPIO edge detect: disable SSI & timer and enable GPIO edge
		SSI3_CR1_R = 0x00000000;							// disable SSI
		GPIO_PORTQ_IEV_R &= ~DATA2;							// set falling edge
		GPIO_PORTQ_IBE_R &= ~DATA2;							// set one edge
		GPIO_PORTQ_IS_R &= ~DATA2;							// set edge ints

		GPIO_PORTQ_AFSEL_R &= ~DATA2;						// disable alt funct on PQ3 (SSI3)
		GPIO_PORTQ_ICR_R = 0xff;							// pre-clear int flags
		GPIO_PORTQ_IM_R |= (DATA2);							// enable SIN edge intr (gpioq) for next message
		NVIC_EN2_R = NVIC_EN2_GPIO_PORTQ;					// enable GPIOQ edge intr
		TIMER2_ICR_R = TIMERA_MIS_MASK;						// clear Timer2A-intr
		TIMER2_CTL_R &= ~(TIMER_CTL_TAEN);					// disable timer
		GPIO_PORTB_AHB_DATA_R &= ~SPARE_PB0;				// !!! debug GPIO off
	}else{
		// start bit aligned, trigger SSI transfer & set EOT_WAIT mode
		GPIO_PORTB_AHB_DATA_R |= SPARE_PB0;					// !!! debug GPIO on
		SSI3_DR_R = 0;										// send 16 bits
		TIMER2_ICR_R = TIMERA_MIS_MASK;						// clear all A-intr
		TIMER2_TAILR_R = (uint16_t)(SIN_EOT_TIME);
		tmr_pingpong = EOT_WAIT;
	}
	return;
}

//-----------------------------------------------------------------------------
// SSI3_ISR
//-----------------------------------------------------------------------------
//
// EOT ISR for DATA2.
//	gathers bits from FIFO, assembles into 32b word, and places into buffer
//	re-arms GPIOQ edge interrupt
//
//-----------------------------------------------------------------------------
/*
void SSI3_ISR(void)
{
//	U32	i;
//	U32	dr = 0;

	sin_buf[sin_hptr++] = SSI3_DR_R << 1;					// put rcvd word into buffer
	if(sin_hptr >= SIN_MAX){								// process head wrap-around
		sin_hptr = 0;
	}
	if(sin_hptr == sin_tptr){
		sin_tptr += 1;
		if(sin_tptr >= SIN_MAX){
			sin_tptr = 0;
		}
	}
//	SSI3_IM_R &= ~SSI_IM_EOTIM;								// disable end of tx ISR
	SSI3_IM_R = 0;								// disable end of tx ISR
	SSI3_CR1_R = 0x00000000;								// disable SSI
	GPIO_PORTQ_IEV_R &= ~DATA2;								// falling edge
	GPIO_PORTQ_IBE_R &= ~DATA2;								// one edge
	GPIO_PORTQ_IS_R &= ~DATA2;								// edge ints

	GPIO_PORTQ_AFSEL_R &= ~DATA2;							// disable alt funct on PQ3 (SSI3)
	GPIO_PORTQ_ICR_R = 0xff;								// pre-clear int flags
	GPIO_PORTQ_IM_R |= (DATA2);								// enable SIN edge intr (gpioq) for next message
	NVIC_EN2_R = NVIC_EN2_GPIO_PORTQ;						// enable GPIOQ edge intr
	return;
}*/
