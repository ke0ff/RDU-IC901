/********************************************************************
 ************ COPYRIGHT (c) 2023 by ke0ff, Taylor, TX   *************
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
 *    08-06-23 jmh:  Reformed for IC-901 data formats and initial validation/debug complete
 *    06-29-21 jmh:  creation date
 *
 *******************************************************************/

#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include "inc/tm4c1294ncpdt.h"		// processor peripheral defines
#include "inc/tm4c1294ncpdt_PCTL.h"	// GPIO alt-func defines
#include "nvic.h"					// peripheral ISR enables
#include "typedef.h"
#include "init.h"					// App-specific SFR Definitions
#include "sio.h"
#include "tiva_init.h"				// init function defines
#include "serial.h"					// serial I/O functions
#include "ssi1.h"					// SOUT
#include "ssi3.h"					// SIN
#include "radio.h"					// SIN

//-----------------------------------------------------------------------------
// local declarations
//-----------------------------------------------------------------------------

U16	sin_perr;						// sin error status register
#define	SIN_MAX	10
U16	sin_buf[SIN_MAX];				// sin buffer array
U16	sinstat_buf[SIN_MAX];			// sin SSI status buffer array
U8	sin_hptr;						// sin circ buffer indexes
U8	sin_tptr;
U8	tmr2a_mode;						// Timer2A mode register

//-----------------------------------------------------------------------------
// ***** START OF CODE *****
//-----------------------------------------------------------------------------

/****************
 * init_sio is called at IPL (or whenever the SIN system needs to be flushed) and initializes the data structures for the sio Fns
 * SSI1 is the 40b sync/async data output master.  Clock is pinned out, but is only used for the base-unit direct connect (not used for the EX-766 connection)
 * SSI3 is the 16b async data input master.  This serial transfer is initiated by a GPIO trigger on PQ3.  This triggers a timer delay (timer 2A) of one bit time
 * 		at which point the Timer2A_ISR triggers an SPI transfer (a single word of 16b).  SSI3_ISR transfers the received data word to the buffer and then re-cycles
 * 		the GPIO trigger to arm for the next transfer.
 * 		Since the start bit is skipped, the received data word has only data bits.  !! This differs from the IC-900 implementation !!
 * flush is a T/F that bypasses SOUT init if true
 */
U32 init_sio(U32 sys_clk, U8 flush)
{
	U32				i;				// temp U32
	U32				iplr = 0;
	volatile U32	ui32Loop;

	// init local variables and buffers
	sin_perr = 0;
	sin_hptr = 0;
	sin_tptr = 0;
//	sin_buf[sin_hptr] = 0;
	for(i=0; i<SIN_MAX; i++){								// clear buffer entries
		sin_buf[i] = 0;
		sinstat_buf[i] = 0;
	}

	// init ssi1 (4800 baud, 8b, async serial out)
	//	drives 40b message output (5, 8bit values burst-written into the FIFO), the SO bitmap is as follows
	//	BIT#		DESCRIPTION
	//	39			start, always cleared to 0
	//	[38:35]		sub-module addr
	//	[34:24]		control data
	//	[23:05]		data
	//	[04:00]		stop, always set to 1 except for WBRX module which features 80bit frames

	// call SOUT config in SSI1.c
	if(!flush){
		ssi1_init();										// init SOUT data output plus clock (master)
	}
	// SIN config
	SYSCTL_RCGCSSI_R |= SYSCTL_RCGCSSI_R3;					// activate SSI3 clock domain
	GPIO_PORTQ_IM_R = 0x00;									// config PQ3 edge interrupt
	GPIO_PORTQ_IEV_R &= ~DATA2;								// falling edge
	GPIO_PORTQ_IBE_R &= ~DATA2;								// one edge
	GPIO_PORTQ_IS_R &= ~DATA2;								// edge ints
	GPIO_PORTQ_ICR_R = 0xff;								// clear int flags
	GPIO_PORTQ_IM_R |= (DATA2);								// enable SIN edge intr
//	GPIO_PORTB_AHB_DATA_R |= SPARE_PB1;					// !!! debug GPIO
	// enable GPIO ISR
	NVIC_EN2_R = NVIC_EN2_GPIO_PORTQ;						// enable GPIOQ edge intr
	iplr = IPL_ASIOINIT;
	return iplr;
}

//**********************************************************************************************//
//																								//
//	Serial Out via SSI1TX to create a 4800 baud, 40b data path (1 start bit, at least 1 stop	//
//	bit). Requires external signal inversion for data to get it to work right since the TIVA	//
//	SPI returns to zero between transfers (data needs to idle high to make it look async).		//
//	SSI1 is configured for 8b xfrs, which means that 5 bytes must be burst filled into the		//
//	SSI FIFO to get a continuous 40b frame.  The frame must be continuous to allow the ASYNC	//
//	case which arises when the EX-766 is used.													//
//																								//
//**********************************************************************************************//

//--------------------------------------------------------------------------------
// send_so() sends 4800 baud SYNC/ASYNC data using SSI1 tx output.  CLOCK is
// sent, but is not used for EX766 interface, only for the direct connect
// interface.  This is configured mechanically (via S3 on IC-901 Controller, per
// the IC-901 operating manual).  Data is 39 bits, justified against b39 (left),
// b40 is the start bit.  send_so() adds the start bit by masking off bit 40,
// and then stores the transfer as 5, 8b values to the SPI data register/FIFO
//--------------------------------------------------------------------------------
void send_so(uint64_t data, U8 r91a){
	uint8_t		dptr[5];
	uint8_t 	jk;

	data = (~data);											// invert data to compensate for inverted hardware path
	for(jk=5; jk!=0; jk--){									// break U64 down into 5 bytes
		dptr[jk-1] = (uint8_t)(data & 0xff);
		data >>= 8;
	}
	if(!r91a){
		dptr[0] |= 0x80;									// set start bit if not UX-R91a 2nd-word
	}
	for(jk=0; jk<5; jk++){
		while(SSI1_SR_R & SSI_SR_BSY);						// wait for fifo to empty (this shouldn't hang up unless UX-R91A messages are being sent)
		SSI1_DR_R = dptr[jk];
	}
	return;
}

//**********************************************************************************************//
//																								//
//	Serial IN via PQ3 & timer2a to create a 4800 baud, 16b async data path (1 start bit,		//
//	1+ stop bit).  Edge detect for PQ3 triggers Timer2A to align to the first bit of data.		//
//	Timer2A then enables SSI3 to transfer 16b of data for reception and resets itself to		//
//	trap the EOT case where the SSI3 data is received and transfered to the signal buffer.		//
//	At this point, Timer2A is disabled, and the GPIO edge ISR is re-config'd which re-arms the	//
//	system for the next data word reception.													//
//																								//
//**********************************************************************************************//

//-----------------------------------------------------------------------------
// get_sin looks for a word in the buffer.  If none, return 0x0000
//	uses circular buffer sin_buf[]/sin_tptr/sin_hptr which is filled in the TIMER2A interrupt
//-----------------------------------------------------------------------------
U16 get_sin(void){
	U16 c;

	if(sin_tptr == sin_hptr){								// if head == tail, there is no data, return NULL
		c = 0;
	}else{													// data is present:
		c = sin_buf[sin_tptr++];							// get chr from circ-buff and update tail ptr
		if(sin_tptr >= SIN_MAX){							// process tail wrap-around
			sin_tptr = 0;
		}
	}
	return c;
}

//-----------------------------------------------------------------------------
// got_sin returns TRUE if there are any words in the buffer.
//-----------------------------------------------------------------------------
char got_sin(void){
	char c;			// return val, default to "no chr"

    if(sin_tptr == sin_hptr){								// if (head != tail) && there is at least 1 msg..
    	c = FALSE;											// .. set buffer empty flag
	}else{
    	c = TRUE;											// .. set data present flag
	}
	return c;
}

//-----------------------------------------------------------------------------
// flush_sin empties the input buffer & reconfigs SIN.
//-----------------------------------------------------------------------------
void flush_sin(void){

	GPIO_PORTQ_IM_R &= ~(DATA2);							// disable SIN edge and timer intr
	TIMER2_CTL_R &= ~(TIMER_CTL_TAEN);
	init_sio(SYSCLK, FLUSH_SIN);
	return;
}

//-----------------------------------------------------------------------------
// get_error() returns and clears framing error status
//-----------------------------------------------------------------------------
U16 get_error(void){
	U16	i;		// temp

	i = sin_perr;
	sin_perr = 0;
	return i;
}

//-----------------------------------------------------------------------------
// print_ptr() prints t/h ptrs (debug)
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
// The SSI3 ISR was deprecated due to issues with the EOT interrupt not working correctly
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
//	GPIO_PORTB_AHB_DATA_R &= ~SPARE_PB1;				// !!! debug GPIO
	GPIO_PORTQ_IM_R = 0;									// disable SIN edge intr
	GPIO_PORTQ_ICR_R = 0xff;								// pre-clear int flags
	NVIC_DIS2_R = NVIC_EN2_GPIO_PORTQ;						// disable GPIOQ edge intr

	tmr2a_mode = START_BIT;									// set start-bit mode for timer
	timer2A_init(SYSCLK, START_BIT_FLAG);
	ssi3_init();											// enable SSI
	return;
}

//-----------------------------------------------------------------------------
// Timer2_ISR
//-----------------------------------------------------------------------------
//
// Called when timer2A overflows:
//	This ISR completes the SSI3 first-bit clock alignment and ersatz EOT events.  At EOT, it gathers data
//	from the completed SSI3 receive cycle to feed into the sin[] signal buffer.
//		"tmr2a_mode = START_BIT" is the start-bit alignment mode.  The ISR sends an SSI word (16b) to trigger
//			reception of the incoming data that has now been aligned to the center of the data
//			bit lanes.  The first bit clocked is the data MSb (the start bit is not captured by the SSI)
//		"tmr2a_mode = EOT_WAIT" signifies the end of the RX cycle.  The ISR pulls in the SSI data and
//			places it into the sin[] circular buffer.  Note: Buffer overflow errors cause the oldest
//			data to be lost.  This mode then disables Timer2A and SSI3 and re-arms/configs the GPIO edge ISR
//
//-----------------------------------------------------------------------------

void Timer2A_ISR(void)
{
	U16	ii;			// temp

	if(tmr2a_mode == EOT_WAIT){
		sinstat_buf[sin_hptr] = SSI3_RIS_R;					// buffer the SSI status
		SSI3_ICR_R = 0xff;									// clear SSI flags
		if(sinstat_buf[sin_hptr] != NORM_SSI_STAT){
			sin_perr |= SIN_SSIERR;							// set SSI error
		}
		ii = SSI3_DR_R;
		sin_buf[sin_hptr++] = ii;							// put rcvd word into buffer
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
		if(!(ii & SIN_ADDR_MASK)){							// special case: save module present status word directly to flag register
			set_present(ii);
		}
		// flip-flop back to GPIO edge detect: disable SSI & timer and enable GPIO edge
		SSI3_CR1_R = 0x00000000;							// disable SSI
		GPIO_PORTQ_IEV_R &= ~DATA2;							// set falling edge
		GPIO_PORTQ_IBE_R &= ~DATA2;							// set one edge
		GPIO_PORTQ_IS_R &= ~DATA2;							// set edge ints

		GPIO_PORTQ_AFSEL_R &= ~DATA2;						// disable alt funct on PQ3 (SSI3)
		GPIO_PORTQ_ICR_R = 0xff;							// pre-clear int flags
//		GPIO_PORTB_AHB_DATA_R |= SPARE_PB1;				// !!! debug GPIO
		GPIO_PORTQ_IM_R |= (DATA2);							// enable SIN edge intr (gpioq) for next message
		NVIC_EN2_R = NVIC_EN2_GPIO_PORTQ;					// enable GPIOQ edge intr
		TIMER2_ICR_R = TIMERA_MIS_MASK;						// clear Timer2A-intr
		TIMER2_CTL_R &= ~(TIMER_CTL_TAEN);					// disable timer
//		GPIO_PORTB_AHB_DATA_R &= ~SPARE_PB0;			// !!! debug GPIO
	}else{
//		GPIO_PORTB_AHB_DATA_R |= SPARE_PB1;				// !!! debug GPIO
		// start bit aligned, trigger SSI transfer & set EOT_WAIT mode
//		GPIO_PORTB_AHB_DATA_R |= SPARE_PB0;				// !!! debug GPIO
		SSI3_DR_R = 0;										// send 16 bits
		timer2A_init(SYSCLK, EOT_WAIT_FLAG);
		tmr2a_mode = EOT_WAIT;
//	GPIO_PORTB_AHB_DATA_R &= ~SPARE_PB1;				// !!! debug GPIO
	}
	return;
}
