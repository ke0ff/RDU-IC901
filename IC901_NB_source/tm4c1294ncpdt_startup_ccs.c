//*****************************************************************************
//
// Startup code for use with TI's Code Composer Studio. {EEPROM}
//
// Copyright (c) 2011-2013 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Software License Agreement
//
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
//
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
//
//*****************************************************************************

#include <stdint.h>
#include "typedef.h"
#include "init.h"
#include "serial.h"
#include "sio.h"
#include "ssi2.h"

//*****************************************************************************
//
// Forward declaration of the default fault handlers.
//
//*****************************************************************************
void ResetISR(void);
static void NmiSR(void);
static void FaultISR(void);
static void UART0Handler(void);
static void UART2Handler(void);
static void UART3Handler(void);
static void UART4Handler(void);
static void UART5Handler(void);
static void UART6Handler(void);
static void UART7Handler(void);
static void IntDefaultHandler(void);
static void TIMER0AHandler(void);
static void TIMER0BHandler(void);
static void TIMER1AHandler(void);
static void TIMER1BHandler(void);
static void TIMER2AHandler(void);
static void TIMER3BHandler(void);
//static void TIMER4AHandler(void);
//static void TIMER4BHandler(void);
//static void GPIO_PL_Handler(void);
//static void GPIO_PM_Handler(void);
//static void GPIO_PP_Handler(void);
static void GPIO_PF_Handler(void);
static void GPIO_PG_Handler(void);
static void GPIO_PQ_Handler(void);
static void SSI2Handler(void);
//static void SSI3Handler(void);

//*****************************************************************************
//
// External declaration for the reset handler that is to be called when the
// processor is started
//
//*****************************************************************************
extern void _c_int00(void);

//*****************************************************************************
//
// Linker variable that marks the top of the stack.
//
//*****************************************************************************
extern uint32_t __STACK_TOP;

//*****************************************************************************
//
// External declarations for the interrupt handlers used by the application.
//
//*****************************************************************************
// To be added by user

//*****************************************************************************
//
// The vector table.  Note that the proper constructs must be placed on this to
// ensure that it ends up at physical address 0x0000.0000 or at the start of
// the program if located at a start address other than 0.
//
//*****************************************************************************
#pragma DATA_SECTION(g_pfnVectors, ".intvecs")
void (* const g_pfnVectors[])(void) =
{
    (void (*)(void))((uint32_t)&__STACK_TOP),
                                            // The initial stack pointer
    ResetISR,                               // The reset handler
    NmiSR,                                  // The NMI handler
    FaultISR,                               // The hard fault handler
    IntDefaultHandler,                      // The MPU fault handler
    IntDefaultHandler,                      // The bus fault handler
    IntDefaultHandler,                      // The usage fault handler
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    IntDefaultHandler,                      // SVCall handler
    IntDefaultHandler,                      // Debug monitor handler
    0,                                      // Reserved
    IntDefaultHandler,                      // The PendSV handler
    IntDefaultHandler,                      // The SysTick handler
    										// NVIC_EN0
    IntDefaultHandler,                      // 00{	GPIO Port A
    IntDefaultHandler,                      // 01	GPIO Port B
    IntDefaultHandler,                      // 02	GPIO Port C
    IntDefaultHandler,                      // 03	GPIO Port D
    IntDefaultHandler,                      // 04{	GPIO Port E
    UART0Handler,                      		// 05	UART0 Rx and Tx
    IntDefaultHandler,                      // 06	UART1 Rx and Tx
    IntDefaultHandler,                      // 07	SSI0 Rx and Tx
    IntDefaultHandler,                      // 08{	I2C0 Master and Slave
    IntDefaultHandler,                      // 09	PWM Fault
    IntDefaultHandler,                      // 10	PWM Generator 0
    IntDefaultHandler,                      // 11	PWM Generator 1
    IntDefaultHandler,                      // 12{	PWM Generator 2
    IntDefaultHandler,                      // 13	Quadrature Encoder 0
    IntDefaultHandler,                      // 14	ADC Sequence 0
    IntDefaultHandler,                      // 15	ADC Sequence 1
    IntDefaultHandler,                      // 16{	ADC Sequence 2
    IntDefaultHandler,                      // 17	ADC Sequence 3
    IntDefaultHandler,                      // 18	Watchdog timer
    TIMER0AHandler,                      	// 19	Timer 0 subtimer A
	TIMER0BHandler,                      	// 20{	Timer 0 subtimer B
	TIMER1AHandler,                      	// 21	Timer 1 subtimer A
	TIMER1BHandler,                      	// 22	Timer 1 subtimer B
	TIMER2AHandler,                      	// 23	Timer 2 subtimer A
	IntDefaultHandler,                      // 24{	Timer 2 subtimer B
    IntDefaultHandler,                      // 25	Analog Comparator 0
    IntDefaultHandler,                      // 26	Analog Comparator 1
    IntDefaultHandler,                      // 27	Analog Comparator 2
    IntDefaultHandler,                      // 28{	System Control (PLL, OSC, BO)
    IntDefaultHandler,                      // 29	FLASH Control
	GPIO_PF_Handler,                      	// 30	GPIO Port F
	GPIO_PG_Handler,                      	// 31	GPIO Port G
											// NVIC_EN1
    IntDefaultHandler,                      // 00{	GPIO Port H
    UART2Handler,		                    // 01	UART2 Rx and Tx
    IntDefaultHandler,                      // 02	SSI1 Rx and Tx
    IntDefaultHandler,                      // 03	Timer 3 subtimer A
	TIMER3BHandler,                      	// 04{	Timer 3 subtimer B
    IntDefaultHandler,                      // 05	I2C1 Master and Slave
    IntDefaultHandler,                      // 06	CAN0
    IntDefaultHandler,                      // 07	CAN1
    IntDefaultHandler,                      // 08{	Ethernet
    IntDefaultHandler,                      // 09	Hibernate
    IntDefaultHandler,                      // 10	USB0
    IntDefaultHandler,                      // 11	PWM Generator 3
    IntDefaultHandler,                      // 12{	uDMA Software Transfer
    IntDefaultHandler,                      // 13	uDMA Error
    IntDefaultHandler,                      // 14	ADC1 Sequence 0
    IntDefaultHandler,                      // 15	ADC1 Sequence 1
    IntDefaultHandler,                      // 16{	ADC1 Sequence 2
    IntDefaultHandler,                      // 17	ADC1 Sequence 3
    IntDefaultHandler,                      // 18	External Bus Interface 0
    IntDefaultHandler,                      // 19	GPIO Port J
    IntDefaultHandler,                      // 20{	GPIO Port K
	IntDefaultHandler,                      // 21	GPIO Port L
	SSI2Handler,                      		// 22	SSI2 Rx and Tx
	IntDefaultHandler,                     	// 23	SSI3 Rx and Tx
    UART3Handler,                      		// 24{	UART3 Rx and Tx
    UART4Handler,                     	 	// 25	UART4 Rx and Tx
    UART5Handler,                      		// 26	UART5 Rx and Tx
    UART6Handler,                      		// 27	UART6 Rx and Tx
    UART7Handler,                      		// 28{	UART7 Rx and Tx
    IntDefaultHandler,                      // 29	I2C2 Master and Slave
    IntDefaultHandler,                      // 30	I2C3 Master and Slave
	IntDefaultHandler,                      // 31	Timer 4 subtimer A
											// NVIC_EN2
	IntDefaultHandler,                     	// 00{	Timer 4 subtimer B
    IntDefaultHandler,                      // 01	Timer 5 subtimer A
    IntDefaultHandler,                      // 02	Timer 5 subtimer B
    IntDefaultHandler,                      // 03	FPU
    0,                                      // 04{	Reserved
    0,                                      // 05	Reserved
    IntDefaultHandler,                      // 06	I2C4 Master and Slave
    IntDefaultHandler,                      // 07	I2C5 Master and Slave
	IntDefaultHandler,                      // 08{	GPIO Port M
    IntDefaultHandler,                      // 09	GPIO Port N
    0,                                      // 10	Reserved
    IntDefaultHandler,                      // 11	Tamper
	IntDefaultHandler,                      // 12{	GPIO Port P (Summary or P0)
    IntDefaultHandler,                      // 13	GPIO Port P1
    IntDefaultHandler,                      // 14	GPIO Port P2
    IntDefaultHandler,                      // 15	GPIO Port P3
	IntDefaultHandler,                      // 16{	GPIO Port P4
	IntDefaultHandler,                      // 17	GPIO Port P5
    IntDefaultHandler,                      // 18	GPIO Port P6
    IntDefaultHandler,                      // 19	GPIO Port P7
	GPIO_PQ_Handler,                        // 20{	GPIO Port Q (Summary or Q0)
    IntDefaultHandler,                      // 21	GPIO Port Q1
    IntDefaultHandler,                      // 22	GPIO Port Q2
	IntDefaultHandler,                      // 23	GPIO Port Q3
    IntDefaultHandler,                      // 24{	GPIO Port Q4
    IntDefaultHandler,                      // 25	GPIO Port Q5
    IntDefaultHandler,                      // 26	GPIO Port Q6
    IntDefaultHandler,                      // 27	GPIO Port Q7
    IntDefaultHandler,                      // 28{	GPIO Port R
    IntDefaultHandler,                      // 29	GPIO Port S
    IntDefaultHandler,                      // 30	SHA/MD5 0
    IntDefaultHandler,                      // 31	AES 0
											// NVIC_EN3
    IntDefaultHandler,                      // 00{	DES3DES 0
    IntDefaultHandler,                      // 01	LCD Controller 0
    IntDefaultHandler,                      // 02	Timer 6 subtimer A
    IntDefaultHandler,                      // 03	Timer 6 subtimer B
    IntDefaultHandler,                      // 04{	Timer 7 subtimer A
    IntDefaultHandler,                      // 05	Timer 7 subtimer B
    IntDefaultHandler,                      // 06	I2C6 Master and Slave
    IntDefaultHandler,                      // 07	I2C7 Master and Slave
    IntDefaultHandler,                      // 08{	HIM Scan Matrix Keyboard 0
    IntDefaultHandler,                      // 09	One Wire 0
    IntDefaultHandler,                      // 10	HIM PS/2 0
    IntDefaultHandler,                      // 11	HIM LED Sequencer 0
    IntDefaultHandler,                      // 12{	HIM Consumer IR 0
    IntDefaultHandler,                      // 13	I2C8 Master and Slave
    IntDefaultHandler,                      // 14	I2C9 Master and Slave
    IntDefaultHandler,                      // 15	GPIO Port T
    IntDefaultHandler,                      // 16{	Fan 1
    0,                                      // 17	Reserved
};

//*****************************************************************************
//
// This is the code that gets called when the processor first starts execution
// following a reset event.  Only the absolutely necessary set is performed,
// after which the application supplied entry() routine is called.  Any fancy
// actions (such as making decisions based on the reset cause register, and
// resetting the bits in that register) are left solely in the hands of the
// application.
//
//*****************************************************************************
void
ResetISR(void)
{
    //
    // Jump to the CCS C initialization routine.  This will enable the
    // floating-point unit as well, so that does not need to be done here.
    //
     __asm("    .global _c_int00\n"
          "    b.w     _c_int00");
}

//*****************************************************************************
//
// This is the code that gets called when the processor receives a NMI.  This
// simply enters an infinite loop, preserving the system state for examination
// by a debugger.
//
//*****************************************************************************
static void
NmiSR(void)
{
    //
    // Enter an infinite loop.
    //
    while(1)
    {
    }
}

//*****************************************************************************
//
// This is the code that gets called when the processor receives a fault
// interrupt.  This simply enters an infinite loop, preserving the system state
// for examination by a debugger.
//
//*****************************************************************************
static void
FaultISR(void)
{
	uint32_t	i = 1;
    //
    // Enter an infinite loop.
    //
    while(i)
    {
    }
}
//*****************************************************************************
//
// This is the code that gets called when the processor receives a UART0 intr.
//
//*****************************************************************************
static void
UART0Handler(void)
{
	rxd_intr();						// process UART rcv interrupt
}

static void
UART2Handler(void)
{
#ifdef ENAB_UART2
	rxd2_intr();						// process UART rcv interrupt
#else
    while(0)
    {
    }
#endif
}

static void
UART3Handler(void)
{
#ifdef ENAB_UART3
	rxd3_intr();						// process UART rcv interrupt
#else
    while(0)
    {
    }
#endif
}

static void
UART4Handler(void)
{
#ifdef ENAB_UART4
	rxd4_intr();						// process UART rcv interrupt
#else
    while(0)
    {
    }
#endif
}

static void
UART5Handler(void)
{
#ifdef ENAB_UART5
	rxd5_intr();						// process UART rcv interrupt
#else
    while(0)
    {
    }
#endif
}

static void
UART6Handler(void)
{
#ifdef ENAB_UART6
	rxd6_intr();						// process UART rcv interrupt
#else
    while(0)
    {
    }
#endif
}

static void
UART7Handler(void)
{
#ifdef ENAB_UART7
	rxd7_intr();						// process UART rcv interrupt
#else
    while(0)
    {
    }
#endif
}

//*****************************************************************************
//
// This is the code that gets called when the processor receives a TIMER0A intr.
//
//*****************************************************************************
static void
TIMER0AHandler(void)
{
	Timer0A_ISR();					// process timer0 interrupt
}

//*****************************************************************************
//
// This is the code that gets called when the processor receives a TIMER0B intr.
//
//*****************************************************************************
static void
TIMER0BHandler(void)
{
	Timer0B_ISR();					// process timer0 interrupt
}

//*****************************************************************************
//
// This is the code that gets called when the processor receives a TIMER1A intr.
//
//*****************************************************************************
static void
TIMER1AHandler(void)
{
	Timer1A_ISR();					// process timer1A interrupt
}

static void
TIMER1BHandler(void)
{
	Timer1A_ISR();					// process timer1B interrupt
}

//*****************************************************************************
//
// This is the code that gets called when the processor receives a TIMER4A intr.
//
//*****************************************************************************
/*static void
TIMER4AHandler(void)
{
	Timer4A_ISR();					// process timer4 interrupt
}*/

//*****************************************************************************
//
// This is the code that gets called when the processor receives a TIMER4A intr.
//
//*****************************************************************************
/*static void
TIMER4BHandler(void)
{
	Timer4B_ISR();					// process timer4 interrupt
}*/

//*****************************************************************************
//
// This is the code that gets called when the processor receives a gpioq intr.
//
//*****************************************************************************
static void
GPIO_PF_Handler(void)
{
	gpiof_isr();					// process gpioq interrupt
}

//*****************************************************************************
//
// This is the code that gets called when the processor receives a gpioq intr.
//
//*****************************************************************************
static void
GPIO_PG_Handler(void)
{
	gpiog_isr();					// process gpioq interrupt
}

//*****************************************************************************
//
// This is the code that gets called when the processor receives a gpioq intr.
//
//*****************************************************************************
static void
GPIO_PQ_Handler(void)
{
	gpioq_isr();					// process gpioq interrupt
}

//*****************************************************************************
//
// This is the code that gets called when the processor receives a TIMER2A intr.
//
//*****************************************************************************
static void
TIMER2AHandler(void)
{
	Timer2A_ISR();					// process timer2A interrupt
}

//*****************************************************************************
//
// This is the code that gets called when the processor receives a TIMER3B intr.
//
//*****************************************************************************
static void
TIMER3BHandler(void)
{
	Timer3B_ISR();					// process timer3B interrupt
}

//*****************************************************************************
//
// This is the code that gets called when the processor receives a ssi3 intr.
//
//*****************************************************************************
static void
SSI2Handler(void)
{
	SSI2_ISR();						// process ssi3 interrupt
}

//*****************************************************************************
//
// This is the code that gets called when the processor receives a ssi3 intr.
//
//*****************************************************************************
/*static void
SSI3Handler(void)
{
	SSI3_ISR();						// process ssi3 interrupt
}*/

//*****************************************************************************
//
// This is the code that gets called when the processor receives an unexpected
// interrupt.  This simply enters an infinite loop, preserving the system state
// for examination by a debugger.
//
//*****************************************************************************
static void
IntDefaultHandler(void)
{
    //
    // Go into an infinite loop.
    //
    while(1)
    {
    }
}
