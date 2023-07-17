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
// Initialize SSI3, PQ3 edge interrupt, and timer

void ssi3_init(void){

	SYSCTL_RCGCSSI_R |= SYSCTL_RCGCSSI_R3;		// activate SSI3
	SSI3_CR1_R = 0x00000000;					// disable SSI, master mode
	GPIO_PORTQ_AFSEL_R |= DATA2;				// enable alt funct on PQ3
	GPIO_PORTQ_PCTL_R = GPIO_PCTL_PQ3_SSI3XDAT1;
	GPIO_PORTQ_AMSEL_R &= ~(DATA2);				// disable analog functionality on PQ
												// BR for data2 = 4800
												// SSI3CLK = SYSCLK / (CPSDVSR * (1 + SCR)) { 2 <= CPSDVSR <= 254, even only)
	SSI3_CPSR_R = (SYSCLK / ((1 + SSI3_SCR) * SSI3_BR));
	SSI3_CR0_R = (SSI3_SCR << 8) | 0x07;		// SCR = [15:8], SPH[7] = 0, SPO[6] = 0 Freescale, DSS = 8-bit data
	SSI3_IM_R = SSI_IM_EOTIM;					// enable end of tx ISR
	SSI3_CR1_R |= 0x00000002;					// enable SSI
	NVIC_EN1_R = NVIC_EN1_SSI3;
	NVIC_EN2_R = NVIC_EN2_GPIO_PORTQ3;
	return;
}
