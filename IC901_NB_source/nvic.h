/********************************************************************
 ************ COPYRIGHT (c) 2015 by KE0FF, Taylor, TX   *************
 *
 *  File name: nvic.h
 *
 *  Module:    Control
 *
 *  Summary:   bit-map defines for Tiva NVIC registers
 *
 *******************************************************************/

#ifndef NVIC_H_
#define NVIC_H_
												// NVIC_EN0
#define	NVIC_EN0_GPIO_PORTA			0x00000001	// 00{	GPIO Port A
#define	NVIC_EN0_GPIO_PORTB			0x00000002	// 01	GPIO Port B
#define	NVIC_EN0_GPIO_PORTC			0x00000004	// 02	GPIO Port C
#define	NVIC_EN0_GPIO_PORTD			0x00000008	// 03	GPIO Port D
#define	NVIC_EN0_GPIO_PORTE			0x00000010	// 04{	GPIO Port E
#define	NVIC_EN0_UART0				0x00000020	// 05	UART0 Rx and Tx
#define	NVIC_EN0_UART1				0x00000040	// 06	UART1 Rx and Tx
#define	NVIC_EN0_SSI0				0x00000080	// 07	SSI0 Rx and Tx
#define	NVIC_EN0_I2C0				0x00000100	// 08{	I2C0 Master and Slave
#define	NVIC_EN0_PWMFAULT			0x00000200	// 09	PWM Fault
#define	NVIC_EN0_PWM0				0x00000400	// 10	PWM Generator 0
#define	NVIC_EN0_PWM1				0x00000800	// 11	PWM Generator 1
#define	NVIC_EN0_PWM2				0x00001000	// 12{	PWM Generator 2
#define	NVIC_EN0_QENC0				0x00002000	// 13	Quadrature Encoder 0
#define	NVIC_EN0_ADC0SEQ0			0x00004000	// 14	ADC0 Sequence 0
#define	NVIC_EN0_ADC0SEQ1			0x00008000	// 15	ADC0 Sequence 1
#define	NVIC_EN0_ADC0SEQ2			0x00010000	// 16{	ADC0 Sequence 2
#define	NVIC_EN0_ADC0SEQ3			0x00020000	// 17	ADC0 Sequence 3
#define	NVIC_EN0_WDOG				0x00040000	// 18	Watchdog timer
#define	NVIC_EN0_TIMER0A			0x00080000	// 19	Timer 0 subtimer A
#define	NVIC_EN0_TIMER0B			0x00100000	// 20{	Timer 0 subtimer B
#define	NVIC_EN0_TIMER1A			0x00200000	// 21	Timer 1 subtimer A
#define	NVIC_EN0_TIMER1B			0x00400000	// 22	Timer 1 subtimer B
#define	NVIC_EN0_TIMER2A			0x00800000	// 23	Timer 2 subtimer A
#define	NVIC_EN0_TIMER2B			0x01000000	// 24{	Timer 2 subtimer B
#define	NVIC_EN0_COMP0				0x02000000	// 25	Analog Comparator 0
#define	NVIC_EN0_COMP1				0x04000000	// 26	Analog Comparator 1
#define	NVIC_EN0_COMP2				0x08000000	// 27	Analog Comparator 2
#define	NVIC_EN0_SYSCNTL			0x10000000	// 28{	System Control (PLL, OSC, BO)
#define	NVIC_EN0_FLASH				0x20000000	// 29	FLASH Control
#define	NVIC_EN0_GPIO_PORTF			0x40000000	// 30	GPIO Port F
#define	NVIC_EN0_GPIO_PORTG			0x80000000	// 31	GPIO Port G
												// NVIC_EN1
#define	NVIC_EN1_GPIO_PORTH			0x00000001	// 00{	GPIO Port H
#define	NVIC_EN1_UART2				0x00000002	// 01	UART2 Rx and Tx
#define	NVIC_EN1_SSI1				0x00000004	// 02	SSI1 Rx and Tx
#define	NVIC_EN1_TIMER3A			0x00000008	// 03	Timer 3 subtimer A
#define	NVIC_EN1_TIMER3B			0x00000010	// 04{	Timer 3 subtimer B
#define	NVIC_EN1_I2C1				0x00000020	// 05	I2C1 Master and Slave
#define	NVIC_EN1_CAN0				0x00000040	// 06	CAN0
#define	NVIC_EN1_CAN1				0x00000080	// 07	CAN1
#define	NVIC_EN1_ETHERNET			0x00000100	// 08{	Ethernet
#define	NVIC_EN1_HIB				0x00000200	// 09	Hibernate
#define	NVIC_EN1_USB0				0x00000400	// 10	USB0
#define	NVIC_EN1_PWM3				0x00000800	// 11	PWM Generator 3
#define	NVIC_EN1_DMAXFR				0x00001000	// 12{	uDMA Software Transfer
#define	NVIC_EN1_DMAERR				0x00002000	// 13	uDMA Error
#define	NVIC_EN1_ADC0SEQ0			0x00004000	// 14	ADC1 Sequence 0
#define	NVIC_EN1_ADC0SEQ1			0x00008000	// 15	ADC1 Sequence 1
#define	NVIC_EN1_ADC0SEQ2			0x00010000	// 16{	ADC1 Sequence 2
#define	NVIC_EN1_ADC0SEQ3			0x00020000	// 17	ADC1 Sequence 3
#define	NVIC_EN1_EBI0				0x00040000	// 18	External Bus Interface 0
#define	NVIC_EN1_GPIO_PORTJ			0x00080000	// 19	GPIO Port J
#define	NVIC_EN1_GPIO_PORTK			0x00100000	// 20{	GPIO Port K
#define	NVIC_EN1_GPIO_PORTL			0x00200000	// 21	GPIO Port L
#define	NVIC_EN1_SSI2				0x00400000	// 22	SSI2 Rx and Tx
#define	NVIC_EN1_SSI3				0x00800000	// 23	SSI3 Rx and Tx
#define	NVIC_EN1_UART3				0x01000000	// 24{	UART3 Rx and Tx
#define	NVIC_EN1_UART4				0x02000000	// 25	UART4 Rx and Tx
#define	NVIC_EN1_UART5				0x04000000	// 26	UART5 Rx and Tx
#define	NVIC_EN1_UART6				0x08000000	// 27	UART6 Rx and Tx
#define	NVIC_EN1_UART7				0x10000000	// 28{	UART7 Rx and Tx
#define	NVIC_EN1_I2C2				0x20000000	// 29	I2C2 Master and Slave
#define	NVIC_EN1_I2C3				0x40000000	// 30	I2C3 Master and Slave
#define	NVIC_EN1_TIMER4A			0x80000000	// 31	Timer 4 subtimer A
												// NVIC_EN2
#define	NVIC_EN2_TIMER4B			0x00000001	// 00{	Timer 4 subtimer B
#define	NVIC_EN2_TIMER5A			0x00000002	// 01	Timer 5 subtimer A
#define	NVIC_EN2_TIMER5B			0x00000004	// 02	Timer 5 subtimer B
#define	NVIC_EN2_FPU				0x00000008	// 03	FPU
//									0x00000010	// 04{	Reserved
//									0x00000020	// 05	Reserved
#define	NVIC_EN2_I2C4				0x00000040	// 06	I2C4 Master and Slave
#define	NVIC_EN2_I2C5				0x00000080	// 07	I2C5 Master and Slave
#define	NVIC_EN2_GPIO_PORTM			0x00000100	// 08{	GPIO Port M
#define	NVIC_EN2_GPIO_PORTN			0x00000200	// 09	GPIO Port N
//									0x00000400	// 10	Reserved
#define	NVIC_EN2_TAMPER				0x00000800	// 11	Tamper
#define	NVIC_EN2_GPIO_PORTP			0x00001000	// 12{	GPIO Port P (Summary or P0)
#define	NVIC_EN2_GPIO_PORTP1		0x00002000	// 13	GPIO Port P1
#define	NVIC_EN2_GPIO_PORTP2		0x00004000	// 14	GPIO Port P2
#define	NVIC_EN2_GPIO_PORTP3		0x00008000	// 15	GPIO Port P3
#define	NVIC_EN2_GPIO_PORTP4		0x00010000	// 16{	GPIO Port P4
#define	NVIC_EN2_GPIO_PORTP5		0x00020000	// 17	GPIO Port P5
#define	NVIC_EN2_GPIO_PORTP6		0x00040000	// 18	GPIO Port P6
#define	NVIC_EN2_GPIO_PORTP7		0x00080000	// 19	GPIO Port P7
#define	NVIC_EN2_GPIO_PORTQ			0x00100000	// 20{	GPIO Port Q (Summary or Q0)
#define	NVIC_EN2_GPIO_PORTQ1		0x00200000	// 21	GPIO Port Q1
#define	NVIC_EN2_GPIO_PORTQ2		0x00400000	// 22	GPIO Port Q2
#define	NVIC_EN2_GPIO_PORTQ3		0x00800000	// 23	GPIO Port Q3
#define	NVIC_EN2_GPIO_PORTQ4		0x01000000	// 24{	GPIO Port Q4
#define	NVIC_EN2_GPIO_PORTQ5		0x02000000	// 25	GPIO Port Q5
#define	NVIC_EN2_GPIO_PORTQ6		0x04000000	// 26	GPIO Port Q6
#define	NVIC_EN2_GPIO_PORTQ7		0x08000000	// 27	GPIO Port Q7
#define	NVIC_EN2_GPIO_PORTR			0x10000000	// 28{	GPIO Port R
#define	NVIC_EN2_GPIO_PORTS			0x20000000	// 29	GPIO Port S
#define	NVIC_EN2_SHA_MD5			0x40000000	// 30	SHA/MD5 0
#define	NVIC_EN2_AES				0x80000000	// 31	AES 0
												// NVIC_EN3
#define	NVIC_EN3_DES				0x00000001	// 00{	DES3DES 0
#define	NVIC_EN3_LCD0				0x00000002	// 01	LCD Controller 0
#define	NVIC_EN3_TIMER6A			0x00000004	// 02	Timer 6 subtimer A
#define	NVIC_EN3_TIMER6B			0x00000008	// 03	Timer 6 subtimer B
#define	NVIC_EN3_TIMER7A			0x00000010	// 04{	Timer 7 subtimer A
#define	NVIC_EN3_TIMER7B			0x00000020	// 05	Timer 7 subtimer B
#define	NVIC_EN3_I2C6				0x00000040	// 06	I2C6 Master and Slave
#define	NVIC_EN3_I2C7				0x00000080	// 07	I2C7 Master and Slave
#define	NVIC_EN3_HIMKEY				0x00000100	// 08{	HIM Scan Matrix Keyboard 0
#define	NVIC_EN3_1WIRE0				0x00000200	// 09	One Wire 0
#define	NVIC_EN3_HIMPS2				0x00000400	// 10	HIM PS/2 0
#define	NVIC_EN3_HIMLED				0x00000800	// 11	HIM LED Sequencer 0
#define	NVIC_EN3_HIMIR				0x00001000	// 12{	HIM Consumer IR 0
#define	NVIC_EN3_I2C8				0x00002000	// 13	I2C8 Master and Slave
#define	NVIC_EN3_I2C9				0x00004000	// 14	I2C9 Master and Slave
#define	NVIC_EN3_GPIO_PORTT			0x00008000	// 15	GPIO Port T
#define	NVIC_EN3_FAN1				0x00010000	// 16{	Fan 1

#endif /* NVIC_H_ */
