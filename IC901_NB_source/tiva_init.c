/********************************************************************
 ************ COPYRIGHT (c) 2015 by KE0FF, Taylor, TX   *************
 *
 *  File name: tiva_init.c
 *
 *  Module:    Control
 *
 *  Summary:
 *  This file holds the IPL code for processor & peripheral initialization
 *
 *  Project scope declarations revision history:
 *    10-22-14 jmh:  creation date
 *
 *******************************************************************/

/********************************************************************
 *  File scope declarations revision history:
 *    10-22-14 jmh:  creation date
 *
 *******************************************************************/

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
// compile defines

#define TIVA_INIT_C
#include <stdint.h>
#include "inc/tm4c1294ncpdt.h"
#include "inc/tm4c1294ncpdt_PCTL.h"
#include <stdio.h>
#include <string.h>
#include "init.h"
#include "typedef.h"
#include "version.h"
#include "serial.h"
#include "cmd_fn.h"
#include "tiva_init.h"
//#include "ADCSWTrigger.h"
#include "SSI0.h"
#include "SSI1.h"
#include "SSI2.h"
#include "SSI3.h"
#include "sio.h"
#include "nvic.h"

//-----------------------------------------------------------------------------
// Local Definitions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Local Variables
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Local variables in this file
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Local Prototypes
//-----------------------------------------------------------------------------



//*****************************************************************************
// proc_init()
//  initializes the processor I/O peripherals
//	returns bitmapped initialize result status as U16
//
//*****************************************************************************
U16 proc_init(U32 sys_clk)
{
	volatile uint32_t ui32Loop;
	U16	ipl = 0;					// initialize response value

	// init GPIO
	SYSCTL_RCGCGPIO_R = PORTQ|PORTP|PORTN|PORTM|PORTL|PORTK|PORTJ|PORTH|PORTG|PORTF|PORTE|PORTD|PORTC|PORTB|PORTA;
    ui32Loop = SYSCTL_RCGCGPIO_R;    				// Do a dummy read to allow the enable to propogate
//    while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R4) == 0){};

#if PORTQ != 0
    GPIO_PORTQ_DIR_R = PORTQ_DIR;
    GPIO_PORTQ_PUR_R = PORTQ_PUR;
    GPIO_PORTQ_DEN_R = PORTQ_DEN;
    GPIO_PORTQ_DATA_R = PORTQ_INIT;
#endif

#if PORTP != 0
    GPIO_PORTP_DIR_R = PORTP_DIR;
    GPIO_PORTP_DEN_R = PORTP_DEN;
    GPIO_PORTP_PUR_R = PORTP_PUR;
    GPIO_PORTP_DATA_R = PORTP_INIT;
#endif

#if PORTN != 0
    GPIO_PORTN_DIR_R = PORTN_DIR;
    GPIO_PORTN_DEN_R = PORTN_DEN;
    GPIO_PORTN_PUR_R = PORTN_PUR;
    GPIO_PORTN_DATA_R = PORTN_INIT;
#endif

#if PORTM != 0
    GPIO_PORTM_DIR_R = PORTM_DIR;
    GPIO_PORTM_PUR_R = PORTM_PUR;
    GPIO_PORTM_DEN_R = PORTM_DEN;
    GPIO_PORTM_DATA_R = PORTM_INIT;
#endif

#if PORTL != 0
    GPIO_PORTL_DIR_R = PORTL_DIR;
    GPIO_PORTL_DEN_R = PORTL_DEN;
    GPIO_PORTL_PUR_R = PORTL_PUR;
    GPIO_PORTL_DATA_R = PORTL_INIT;
#endif

#if PORTK != 0
    GPIO_PORTK_DIR_R = PORTK_DIR;
    GPIO_PORTK_PUR_R = PORTK_PUR;
    GPIO_PORTK_DEN_R = PORTK_DEN;
    GPIO_PORTK_DATA_R = PORTK_INIT;
#endif

#if PORTJ != 0
    GPIO_PORTJ_AHB_DIR_R = PORTJ_DIR;
    GPIO_PORTJ_AHB_DEN_R = PORTJ_DEN;
    GPIO_PORTJ_AHB_PUR_R = PORTJ_PUR;
    GPIO_PORTJ_AHB_DATA_R = PORTJ_INIT;
#endif

#if PORTJ != 0
    GPIO_PORTH_AHB_DIR_R = PORTH_DIR;
    GPIO_PORTH_AHB_DEN_R = PORTH_DEN;
    GPIO_PORTH_AHB_PUR_R = PORTH_PUR;
    GPIO_PORTH_AHB_DATA_R = PORTH_INIT;
#endif

#if PORTG != 0
    GPIO_PORTG_AHB_DIR_R = PORTG_DIR;
    GPIO_PORTG_AHB_DEN_R = PORTG_DEN;
    GPIO_PORTG_AHB_PUR_R = PORTG_PUR;
    GPIO_PORTG_AHB_DATA_R = PORTG_INIT;
#endif

#if PORTF != 0
    GPIO_PORTF_AHB_DIR_R = PORTF_DIR;
    GPIO_PORTF_AHB_DEN_R = PORTF_DEN;
    GPIO_PORTF_AHB_PUR_R = PORTF_PUR;
    GPIO_PORTF_AHB_DATA_R = PORTF_INIT;
#endif

#if PORTE != 0
    GPIO_PORTE_AHB_DIR_R = PORTE_DIR;
    GPIO_PORTE_AHB_DEN_R = PORTE_DEN;
    GPIO_PORTE_AHB_PUR_R = PORTE_PUR;
    GPIO_PORTE_AHB_DATA_R = PORTE_INIT;
#endif

#if PORTD != 0
    GPIO_PORTD_AHB_DIR_R = PORTD_DIR;
    GPIO_PORTD_AHB_DEN_R = PORTD_DEN;
    GPIO_PORTD_AHB_PUR_R = PORTD_PUR;
    GPIO_PORTD_AHB_DATA_R = PORTD_INIT;
#endif

#if PORTC != 0
    GPIO_PORTC_AHB_DIR_R = PORTC_DIR;
    GPIO_PORTC_AHB_DEN_R = PORTC_DEN;
    GPIO_PORTC_AHB_PUR_R = PORTC_PUR;
    GPIO_PORTC_AHB_DATA_R = PORTC_INIT;
#endif

#if PORTB != 0
    GPIO_PORTB_AHB_DIR_R = PORTB_DIR;
    GPIO_PORTB_AHB_DEN_R = PORTB_DEN;
    GPIO_PORTB_AHB_PUR_R = PORTB_PUR;
    GPIO_PORTB_AHB_DATA_R = PORTB_INIT;
#endif

#if PORTA != 0
	GPIO_PORTA_AHB_DIR_R = PORTA_DIR;
    GPIO_PORTA_AHB_DEN_R = PORTA_DEN;
    GPIO_PORTA_AHB_PUR_R = PORTA_PUR;
	GPIO_PORTA_AHB_DATA_R = PORTA_INIT;
#endif

    // init comparator 0/1 for MU/D2 input
	SYSCTL_RCGCACMP_R = SYSCTL_RCGCACMP_R0;
	GPIO_PORTC_AHB_AMSEL_R |= PORTC_AMSEL;			// enable comparator inputs as analog functionality on PC
	GPIO_PORTL_AFSEL_R |= C0O | C1O;				// enable alt funct on PL for comparator outputs
	GPIO_PORTL_PCTL_R = GPIO_PCTL_PL2_C0O|GPIO_PCTL_PL3_C1O;
	COMP_ACREFCTL_R = 0;
	COMP_ACCTL0_R = 0;
	COMP_ACCTL1_R = 0;
//	COMP_ACSTAT0_R & COMP_ACSTAT_OVAL				// bit isolation mask for COMP output status

	// encoder dial GPIO edge ISR config
	// unlock PORTF
//	GPIO_PORTF_LOCK_R = 0x4C4F434B;
//	GPIO_PORTF_CR_R = 0xff;
	GPIO_PORTF_AHB_IM_R = 0x00;							// config PF4 edge interrupt
	GPIO_PORTF_AHB_IEV_R &= ~ENC_DN;					// falling edge
	GPIO_PORTF_AHB_IBE_R &= ~ENC_DN;					// one edge
	GPIO_PORTF_AHB_IS_R &= ~ENC_DN;						// edge ints
	GPIO_PORTF_AHB_ICR_R = 0xff;						// clear int flags
	GPIO_PORTF_AHB_IM_R |= (ENC_DN);					// enable ENCdn edge intr

	GPIO_PORTG_AHB_IM_R = 0x00;							// config PG0 edge interrupt
	GPIO_PORTG_AHB_IEV_R &= ~ENC_UP;					// falling edge
	GPIO_PORTG_AHB_IBE_R &= ~ENC_UP;					// one edge
	GPIO_PORTG_AHB_IS_R &= ~ENC_UP;						// edge ints
	GPIO_PORTG_AHB_ICR_R = 0xff;						// clear int flags
	GPIO_PORTG_AHB_IM_R |= (ENC_UP);					// enable ENCup edge intr

	NVIC_EN0_R = NVIC_EN0_GPIO_PORTF;					// enable ISRs
	NVIC_EN0_R = NVIC_EN0_GPIO_PORTG;

	// init timer0A for interrupts
	timer0A_init(sys_clk);								// init main.c ms app timer isr
	timer0B_init(sys_clk);								// beep output/ISR
	timer1A_init(sys_clk);								// init serial pacing isr
//	timer1B_init(sys_clk);								// KPU long baseline timer intr
	// enable timer2 clock domain
	SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R2;
	ui32Loop = SYSCTL_RCGCGPIO_R;
	NVIC_EN0_R = NVIC_EN0_TIMER2A;						// enable timer2A intr

	ssi0_init();										// init SSI modules
	init_sio(sys_clk, INIT_SIN);						// init DATA1 & DATA2 SSI modules (SSI1 & SSI3)
	ssi2_init();										// LCD ssi init
	timer3B_init(sys_clk);								// LCD blink timer

	// init LED PWMs (M0) on PF0 - PF3
	SYSCTL_RCGCPWM_R |= SYSCTL_RCGCPWM_R0;
	ui32Loop = SYSCTL_RCGCPWM_R;										// delay a few cycles
	GPIO_PORTF_AFSEL_R |= DIMMER|LED_RXS|LED_RXM|LED_TX;				// enable alt fn, PF0-3
	GPIO_PORTF_PCTL_R &= ~(GPIO_PCTL_PF0_M|GPIO_PCTL_PF1_M|GPIO_PCTL_PF2_M|GPIO_PCTL_PF3_M);
	GPIO_PORTF_PCTL_R |= (GPIO_PCTL_PF0_M0PWM0|GPIO_PCTL_PF1_M0PWM1|GPIO_PCTL_PF2_M0PWM2|GPIO_PCTL_PF3_M0PWM3);
	PWM0_CTL_R = 0;
	PWM0_INVERT_R = LED_RXS|LED_RXM|LED_TX;
	PWM0_CC_R = (PWM0_CC_R & ~PWM_CC_PWMDIV_M) | (PWM_DIV << 17) | PWM_CC_USEPWM;
	PWM0_0_CTL_R = 0;
	PWM0_0_GENA_R = PWM_0_GENA_ACTCMPAD_ZERO|PWM_0_GENA_ACTLOAD_ONE;	// M0PWM0
	PWM0_0_GENB_R = PWM_0_GENB_ACTCMPBD_ZERO|PWM_0_GENB_ACTLOAD_ONE;	// M0PWM1
	PWM0_0_LOAD_R = PWM_ZERO;
	PWM0_0_CMPA_R = PWM_ZERO - 1;
	PWM0_0_CMPB_R = PWM_ZERO - 1;
	PWM0_1_CTL_R = 0;
	PWM0_1_GENA_R = PWM_1_GENA_ACTCMPAD_ZERO|PWM_1_GENA_ACTLOAD_ONE;	// M0PWM2
	PWM0_1_GENB_R = PWM_1_GENB_ACTCMPBD_ZERO|PWM_1_GENB_ACTLOAD_ONE;	// M0PWM3
	PWM0_1_LOAD_R = PWM_ZERO;
	PWM0_1_CMPA_R = PWM_ZERO - 1;
	PWM0_1_CMPB_R = PWM_ZERO - 1;
	PWM0_0_CTL_R = PWM_1_CTL_ENABLE;
	PWM0_1_CTL_R = PWM_2_CTL_ENABLE;
	PWM0_ENABLE_R = PWM_ENABLE_PWM0EN|PWM_ENABLE_PWM1EN|PWM_ENABLE_PWM2EN|PWM_ENABLE_PWM3EN;
	ipl |= IPL_PWM0INIT;


	initserial();										// init UARTs

//    init_uart2(115200);
//    init_uart3(115200);
//    init_uart4(115200);
//    init_uart5(115200);
//    init_uart6(115200);
//    init_uart7(115200);								// init ccmd UART
	return ipl;
}

//*****************************************************************************
// adc_chan_select()
//  sets the ADC channel number (for a single-channel sequencer model)
//
//*****************************************************************************
void adc_chan_select(U8 chnum){

/*	ADC0_ACTSS_R &= ~(ADC_ACTSS_ASEN3);				// turn off sequencer 3
	ADC0_EMUX_R	= (ADC0_EMUX_R&~ADC_EMUX_EM3_M)|ADC_EMUX_EM3_ALWAYS; //(ADC_EMUX_EM3_ALWAYS|ADC_EMUX_EM2_NEVER|ADC_EMUX_EM1_NEVER|ADC_EMUX_EM0_NEVER);	// set SS3 to sample continuously
	ADC0_SSOP3_R &= ~ADC_SSOP3_S0DCOP;
	ADC0_SSMUX3_R &= ~(0x0f);						// config the input source..
	ADC0_SSMUX3_R = (ADC0_SSMUX3_R & ~0x0f)|(chnum & 0x0f);					// config the input source..
	ADC0_SSEMUX3_R = (ADC0_SSEMUX3_R & ~(0x01)) | (chnum & 0x10) >> 4;
	ADC0_SSCTL3_R = 0x06; //ADC_SSCTL3_END0;				// set end of sequence.
	ADC0_ACTSS_R |= ADC_ACTSS_ASEN3;				// turn on sequencer 3
	ADC0_IM_R &= ~ADC_IM_MASK3;
	ADC0_PSSI_R = ADC_PSSI_SS3;						// initiate sampling*/
}

//*****************************************************************************
//	Timer0A drives the main.c 1ms interrupt that services the application timer stacks
//*****************************************************************************
void timer0A_init(U32 sys_clk){
	volatile U32	ui32Loop;

	// init timer0A (piezo beep)
	SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R0;
	ui32Loop = SYSCTL_RCGCGPIO_R;
	// unlock PORTF
//	GPIO_PORTF_LOCK_R = 0x4C4F434B;
//	GPIO_PORTF_CR_R = 0xff;
	///////////////
	GPIO_PORTL_PCTL_R &= ~(GPIO_PCTL_PL4_M);
	GPIO_PORTL_PCTL_R |= (GPIO_PCTL_PL4_T0CCP0);
	GPIO_PORTL_AFSEL_R |= BEEP;
	TIMER0_CTL_R &= ~(TIMER_CTL_TAEN);										// disable timer
	TIMER0_CTL_R |= (TIMER_CTL_TAEVENT_NEG);								// enable FE PWM intr
	TIMER0_CFG_R = TIMER_CFG_16_BIT; //0x4; //0;
	TIMER0_TAMR_R = TIMER_TAMR_TAMR_PERIOD | TIMER_TAMR_TAAMS | TIMER_TAMR_TAPWMIE | TIMER_TAMR_TAMIE;
	TIMER0_TAPR_R = 0;
	set_beep(BEEP_FREQ, BEEP_COUNT);										// init timer regs to default beep freq/duration
	TIMER0_IMR_R = TIMER_IMR_CAEIM;											// enable timer intr
	TIMER0_ICR_R = TIMER0_MIS_R;											// clear any flagged ints
	NVIC_EN0_R = NVIC_EN0_TIMER0A;											// enable timer0A intr in the NVIC_EN regs

	// init timer0A for interrupts
/*	SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R0;
	ui32Loop = SYSCTL_RCGCGPIO_R;
	TIMER0_CTL_R &= ~(TIMER_CTL_TAEN);				// disable timer
	TIMER0_CFG_R = TIMER_CFG_16_BIT; //0x4; //0;
	TIMER0_TAMR_R = TIMER_TAMR_TAMR_PERIOD;
	TIMER0_TAPR_R = TIMER0_PS;
	TIMER0_TAILR_R = (uint16_t)(sys_clk/(TIMER0_FREQ * (TIMER0_PS + 1)));
	TIMER0_IMR_R = TIMER_IMR_TATOIM;				// enable timer intr
	TIMER0_CTL_R |= (TIMER_CTL_TAEN);				// enable timer
	TIMER0_ICR_R = TIMER0_MIS_R;					// clear any flagged ints
	NVIC_PRI4_R &= ~NVIC_PRI4_INT19_M;				// reset to highest prio
	NVIC_EN0_R = NVIC_EN0_TIMER0A;					// enable timer0A intr (int19) in the NVIC_EN regs*/
}

//*****************************************************************************
//	Timer0B drives the BEEP ISR system
//*****************************************************************************
void timer0B_init(U32 sys_clk){
	volatile U32	ui32Loop;

	SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R0;		// enable timer0B module
	ui32Loop = SYSCTL_RCGCGPIO_R;
	TIMER0_CTL_R &= ~(TIMER_CTL_TBEN);				// disable timer
	TIMER0_CFG_R |= TIMER_CFG_16_BIT; //0x4; //0;
	TIMER0_TBMR_R = TIMER_TBMR_TBMR_PERIOD;
	TIMER0_TBPR_R = TIMER0_PS;
	TIMER0_TBILR_R = (uint16_t)(sys_clk/(TIMER0_FREQ * (TIMER0_PS + 1)));
	TIMER0_IMR_R |= TIMER_IMR_TBTOIM;				// enable timer intr
	TIMER0_CTL_R |= (TIMER_CTL_TBEN);				// enable timer
	TIMER0_ICR_R |= TIMER0_MIS_R;					// clear any flagged ints
	NVIC_PRI4_R &= ~NVIC_PRI5_INT20_M;				// reset to highest prio
	NVIC_EN0_R = NVIC_EN0_TIMER0B;					// enable timer0B intr (int20) in the NVIC_EN regs

/*	// init timer0B for interrupts
	SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R0;		// assumes timer0A enabled already
	ui32Loop = SYSCTL_RCGCGPIO_R;
	TIMER0_CTL_R &= ~(TIMER_CTL_TBEN);				// disable timer
	//TIMER0_CFG_R = TIMER_CFG_16_BIT; //0x4; //0;
	TIMER0_TBMR_R &= ~(TIMER_TBMR_TBMR_M);
	TIMER0_TBMR_R |= TIMER_TBMR_TBMR_PERIOD;
	TIMER0_TBPR_R = 1;								// timer 0B prescale
	TIMER0_TBILR_R = 600;							// interval = ILR/6 (*100ns)
	TIMER0_IMR_R |= TIMER_IMR_TBTOIM;				// enable timer intr
	TIMER0_CTL_R &= ~(TIMER_CTL_TBEN);				// disable timer
	NVIC_DIS0_R = NVIC_EN0_TIMER0B;					// disable timer0B intr (int20) in the NVIC_EN regs*/
}

//*****************************************************************************
//	Timer1A drives Serial (UART) pacing timer
//*****************************************************************************
void timer1A_init(U32 sys_clk){
	volatile U32	ui32Loop;

	// init timer1A for interrupts
	SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R1;
	ui32Loop = SYSCTL_RCGCGPIO_R;
	TIMER1_CTL_R &= ~(TIMER_CTL_TAEN);				// disable timer
	TIMER1_CFG_R = TIMER_CFG_16_BIT; //0x4; //0;
	TIMER1_TAMR_R = TIMER_TAMR_TAMR_PERIOD;
	TIMER1_TAPR_R = TIMER1_PS;
	TIMER1_TAILR_R = (uint16_t)(sys_clk/(TIMER1_FREQ * (TIMER1_PS + 1)));
	TIMER1_IMR_R = TIMER_IMR_TATOIM;				// enable timer intr
//	TIMER1_CTL_R |= (TIMER_CTL_TAEN);				// enable timer
	TIMER1_ICR_R = TIMER1_MIS_R;					// clear any flagged ints
	NVIC_EN0_R = NVIC_EN0_TIMER1A;					// enable timer1A intr in the NVIC_EN regs
}

//*****************************************************************************
//	???? Timer1B drives the 100ms/tval ISR that services the long baseline timers in kpu.c
//*****************************************************************************
void timer1B_init(U32 sys_clk){
	volatile U32	ui32Loop;

	// init timer0B for interrupts
//	SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R0;		// assumes timer1A enabled already
//	ui32Loop = SYSCTL_RCGCGPIO_R;
	TIMER1_CTL_R &= ~(TIMER_CTL_TBEN);				// disable timer
	//TIMER0_CFG_R = TIMER_CFG_16_BIT; //0x4; //0;
	TIMER1_TBMR_R &= ~(TIMER_TBMR_TBMR_M);
	TIMER1_TBMR_R |= TIMER_TBMR_TBMR_PERIOD;
	TIMER1_TBPR_R = TIMER1B_PS;						// timer 1B prescale
	TIMER1_TBILR_R = (uint16_t)(sys_clk/(TIMER1B_FREQ * (TIMER1B_PS + 1)));
	TIMER1_IMR_R |= TIMER_IMR_TBTOIM;				// enable timer intr
	TIMER1_CTL_R |= TIMER_CTL_TBEN;					// enable timer
	NVIC_EN0_R = NVIC_EN0_TIMER1B;					// enable timer1B intr (int21) in the NVIC_EN regs
}

//*****************************************************************************
//	DATA2 1/2bit or EOT_WAIT timer
//	bit_start is "1" for start bit or "0" for EOT_WAIT
//*****************************************************************************
void timer2A_init(U32 sys_clk, U8 bit_start){
	volatile U32	ui32Loop;

	// timer2 clock domain enabled in proc_init()
	// init Timer2A (Count dn, periodic -- inputs IC-900 async data)
	TIMER2_CTL_R &= ~(TIMER_CTL_TAEN);					// disable timer
//	TIMER2_CFG_R = 0;
	TIMER2_TAMR_R = TIMER_TAMR_TAMR_PERIOD;
	TIMER2_CFG_R = TIMER_CFG_16_BIT;
	TIMER2_TAPR_R = TIMER2A_PS;
	if(bit_start){
		TIMER2_TAILR_R = (uint16_t)(SIN_START_BIT_TIME);
	}else{
		TIMER2_TAILR_R = (uint16_t)(SIN_EOT_TIME);
	}
	TIMER2_IMR_R = TIMER_IMR_TATOIM;					// enable timer intr
	TIMER2_CTL_R |= (TIMER_CTL_TAEN);					// enable timer
	TIMER2_ICR_R = TIMER2_MIS_R;						// clear any flagged ints
}

//*****************************************************************************
//	Timer2B drives the 0.25s LCB blink system
//*****************************************************************************
void timer3B_init(U32 sys_clk){
	volatile U32	ui32Loop;

	// init timer3B for interrupts
	SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R3;			// enable timer 2 clock domain
	ui32Loop = SYSCTL_RCGCGPIO_R;
	TIMER3_CTL_R &= ~(TIMER_CTL_TBEN);					// disable timer
	TIMER3_CFG_R = TIMER_CFG_16_BIT; //0x4; //0;
	TIMER3_TBMR_R &= ~(TIMER_TBMR_TBMR_M);
	TIMER3_TBMR_R |= TIMER_TBMR_TBMR_PERIOD;
	TIMER3_TBPR_R = TIMER2B_PS;							// timer 1B prescale
	TIMER3_TBILR_R = (uint16_t)(sys_clk/(TIMER2B_FREQ * (TIMER2B_PS + 1)));
	TIMER3_IMR_R |= TIMER_IMR_TBTOIM;					// enable timer intr
	TIMER3_CTL_R |= TIMER_CTL_TBEN;						// enable timer
	NVIC_EN1_R = NVIC_EN1_TIMER3B;						// enable timer3B intr in the NVIC_EN regs
}

//*****************************************************************************
// hib_init()
//  initializes the HIB peripheral
//	init_switch == 0, turns on HIB osc only
//	else, remainder of HIB init (only allowed if timer interrupts enabled)
//
//*****************************************************************************
U16 hib_init(U8 init_switch){
	volatile uint32_t ui32Loop;
	U16	ipl = 0;

	if(!init_switch){
	    SYSCTL_RCGCHIB_R |= 0x01;
	    ui32Loop = HIB_CTL_R;							// dummy read
		if(!(ui32Loop & HIB_CTL_CLK32EN)){				// if CLK32EN == 0, this is the first POC, turn on HIB osc
		    HIB_IM_R = HIB_IM_WC;						// enable WC mask
		    if((ui32Loop & HIB_CTL_WRC) == 0){
		    	ipl = IPL_HIBERR;						// if WC set, then there is an HIB error
		    }else{
				HIB_CTL_R = HIB_CTL_CLK32EN;			// enable HIB clock
		    }
		}else{
			ipl = IPL_HIBINIT;							// init was completed on previous power cycle
		}
//	}else{
//		wait(1500);										// wait 1500ms for HIB osc to start
//		while(!(HIB_CTL_R & HIB_CTL_WRC));				// make sure WC is clear
//		HIB_CTL_R = HIB_CTL_PINWEN | HIB_CTL_CLK32EN | HIB_CTL_VBATSEL_2_1V;	// enable /WAKE control
//		while(!(HIB_CTL_R & HIB_CTL_WRC));				// wait for WC to clear
//		ipl = IPL_HIBINIT;								// set init complete
	}
	return ipl;
}
