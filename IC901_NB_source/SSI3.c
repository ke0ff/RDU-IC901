/********************************************************************
 ******* Changes COPYRIGHT (c) 2015 by KE0FF, Taylor, TX   **********
 *
 *  File name: SSI3.c
 *
 *  Module:    Control
 *
 *  Summary:
 *  This is the main file for the SSI3 driver (DATA2 input).
 *  The SSI runs as a 4b Freescale master, MISO only (no clock, MOSI, or FSS).
 *  A FE GPIO interrupt starts triggers a half-bit timer which triggers an SSI
 *  send of 5 nybbles (send data is don't-care). The 20 bits of RX data is
 *  assembled into a single 32 bit data word (DATA2) and buffered.
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

#include <stdint.h>
#include "inc/tm4c1294ncpdt.h"
#include "inc/tm4c1294ncpdt_PCTL.h"
#include "nvic.h"
#include "SSI3.h"
#include "init.h"


//******** SSI3_Init *****************
// Initialize SSI3, input only on PQ3

void ssi3_init(void){

	GPIO_PORTQ_AFSEL_R |= DATA2;				// enable alt funct on PQ3
	GPIO_PORTQ_PCTL_R = GPIO_PCTL_PQ3_SSI3XDAT1;
	GPIO_PORTQ_AMSEL_R &= ~(DATA2);				// disable analog functionality on PQ
	SSI3_CR1_R = 0x00000000;					// disable SSI to reset
												// BR for data2 = 4800
												// SSI3CLK = SYSCLK / (CPSDVSR * (1 + SCR)) { 2 <= CPSDVSR <= 254, even only)
	SSI3_CPSR_R = (SYSCLK / ((1 + SSI3_SCR) * SSI3_BR));
	SSI3_CR0_R = (SSI3_SCR << SSI_CR0_SCR_S) | SSI_CR0_DSS_16 | SSI_CR0_SPH | SSI_CR0_SPO; // SCR = [15:8], SPH[7] = 1, SPO[6] = 1, DSS = 16-bit data
	SSI3_CR1_R = SSI_CR1_FSSHLDFRM;				// hold fss
//	SSI3_IM_R = SSI_IM_EOTIM|8;					// enable end of tx ISR
	SSI3_ICR_R = 0xff;
//	NVIC_EN1_R = NVIC_EN1_SSI3;					// enable ISRs
	SSI3_CR1_R |= SSI_CR1_SSE;					// enable SSI

	return;
}

// ARCHIVE ****************************************************************
//
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
