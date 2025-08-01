/********************************************************************
 ******* Changes COPYRIGHT (c) 2015 by KE0FF, Taylor, TX   **********
 *
 *  File name: SSI1.c
 *
 *  Module:    Control
 *
 *  Summary:
 *  This is the main file for the SSI1 driver (DATA1 output).
 *  The SSI runs as an 8b Freescale master, MOSI & SPCK only (no MISO or FSS).
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
#include "SSI1.h"
#include "init.h"


//******** SSI1_Init *****************
// Initialize SSI1, 8b, 4800 baud clk

void ssi1_init(void){

	SYSCTL_RCGCSSI_R |= SYSCTL_RCGCSSI_R1;		// activate SSI1
	GPIO_PORTB_AHB_AFSEL_R |= CLK_DATA1;		// enable alt funct on PB
	GPIO_PORTB_AHB_PCTL_R = GPIO_PCTL_PB5_SSI1CLK;
	GPIO_PORTB_AHB_AMSEL_R &= ~(CLK_DATA1);		// disable analog functionality on PB
	GPIO_PORTE_AHB_AFSEL_R |= DATA1;			// enable alt funct on PE
	GPIO_PORTE_AHB_PCTL_R = GPIO_PCTL_PE4_SSI1XDAT0;
	GPIO_PORTE_AHB_AMSEL_R &= ~(DATA1);			// disable analog functionality on PE
												// BR for data2 = 4800
												// SSI1CLK = SYSCLK / (CPSDVSR * (1 + SCR)) { 2 <= CPSDVSR <= 254, even only)
	SSI1_CR1_R = 0x00000000;					// disable SSI, master mode
	SSI1_CPSR_R = (SYSCLK / ((1 + SSI1_SCR) * SSI1_BR));
	SSI1_CR0_R = (SSI1_SCR << SSI_CR0_SCR_S) | SSI_CR0_DSS_8 | SSI_CR0_SPH | SSI_CR0_SPO; // SCR = [15:8], SPH[7] = 0, SPO[6] = 1 ti, DSS = 8-bit data,,, SSI_CR0_FRF_TI |
	SSI1_IM_R = SSI_IM_EOTIM;					// enable end of tx ISR
	SSI1_CR1_R = SSI_CR1_FSSHLDFRM;				// hold fss
	SSI1_CR1_R |= SSI_CR1_SSE;					// enable SSI
//	NVIC_EN1_R = NVIC_EN1_SSI1;
	return;
}
