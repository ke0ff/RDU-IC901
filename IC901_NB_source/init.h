/********************************************************************
 ************ COPYRIGHT (c) 2022 by KE0FF, Taylor, TX   *************
 *
 *  File name: init.h
 *
 *  Module:    Control
 *
 *  Summary:   defines and global declarations for main.c
 *
 *******************************************************************/

#include <stdint.h>
#include "typedef.h"

#ifndef INIT_H
#define INIT_H
#endif

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------
// ccmd source ID (different for each LRU)
#define	SOURCE_ID	'A'

// Process cmd defines
#define	INIT_PROCESS	0xff			// initialize process
#define	RUN_PROCESS		0x00			// normal process run


#define OSC_LF 4            		// osc clock selects
#define OSC_HF 0
#define OSC_EXT 1

#define SEC10MS    10           	// timer constants (ms)
#define SEC33MS    33
#define SEC50MS    50
#define SEC75MS    74
#define SEC100MS  100
#define SEC250MS  250
#define SEC300MS  300
#define SEC400MS  400
#define SEC500MS  500
#define SEC750MS  750
#define SEC1     1000
#define ONESEC   SEC1
#define MS1500   1500
#define SEC2     2000
#define SEC3     3000
#define SEC5     5000
#define SEC10   10000
#define SEC15   15000
#define SEC30   30000
#define SEC60   60000
#define SEC300 300000L
#define	ONEMIN	(SEC60)
#define	REG_WAIT_DLY 200			// 200ms wait limit for register action
#define RESP_SAMP SEC100MS			// sets resp rate
#define	SOUT_PACE_TIME	9			// 33/4800 s for one SOUT msg
#define	SIN_PACE_TIME	50			// wait a little over 2 word times (2 x 32/4800 s for one SIN word)

#define	CLI_BUFLEN	80						// CLI buffer length
#define	RE_BUFLEN	40						// buffer mem lengths

// timerA/B interrupt masks
#define TIMER_MIS_BMASK	(TIMER_MIS_DMABMIS | TIMER_MIS_TBMMIS | TIMER_MIS_CBEMIS | TIMER_MIS_CBMMIS | TIMER_MIS_TBTOMIS)

#define TIMER_MIS_AMASK	(TIMER_MIS_DMAAMIS | TIMER_MIS_TAMMIS | TIMER_MIS_CAEMIS | TIMER_MIS_CAMMIS | TIMER_MIS_TATOMIS)
#define TIMERA_MIS_MASK	TIMER_MIS_AMASK
#define TIMERB_MIS_MASK	TIMER_MIS_BMASK

#define	DIAL_DEBOUNCE	10			// 10 ms dial debounce

// ***********************
// GPIO PORT BITMAPS
// PORTQ
#define PORTQ SYSCTL_RCGCGPIO_R14
#define	NU_PQ4			0x10			    // PQ4 = n/c			(i) GPIO
#define	DATA2			0x08				// PQ3 = DATA2 in		(i) GPIO (i) / SSI3
#define	NU_PQ2			0x04				// PQ2 = n/c			(i) GPIO
#define	NU_PQ1			0x02				// PQ1 = n/c			(i) GPIO
#define	NU_PQ0			0x01				// PQ0 = n/c			(i) GPIO
#define	PORTQ_INIT		(0)
#define PORTQ_DEN		(NU_PQ4|DATA2|NU_PQ2|NU_PQ1|NU_PQ0)
#define PORTQ_DIR		(0)
#define PORTQ_PUR		(NU_PQ4|NU_PQ2|NU_PQ1|NU_PQ0)

#define	PTT_TIME		1000				// 1sec PTT timeout

// PORTP
#define PORTP SYSCTL_RCGCGPIO_R13
#define	NU_PP5			0x20			    // PP5 = n/c			(i) GPIO
#define	NU_PP4			0x10			    // PP4 = n/c			(i) GPIO
#define	NU_PP3			0x08			    // PP3 = n/c			(i) GPIO
#define	DBLED			0x04			    // PP2 = n/c			(i) GPIO
#define	NU_PP1			0x02			    // PP1 = n/c			(i) GPIO
#define	NU_PP0			0x01			    // PP0 = n/c			(i) GPIO
#define	PORTP_INIT	(DBLED)
#define PORTP_DEN	(NU_PP0|NU_PP1|DBLED|NU_PP3|NU_PP4|NU_PP5)
#define PORTP_DIR	(DBLED)
#define PORTP_PUR	(NU_PP0|NU_PP1|NU_PP3|NU_PP4|NU_PP5)

// PORTN
#define PORTN SYSCTL_RCGCGPIO_R12
#define	NU_PN5			0x20			    // PN5 = n/c			(i) GPIO
#define	NU_PN4			0x10			    // PN4 = n/c			(i) GPIO
#define	NU_PN3			0x08			    // PN3 = n/c			(i) GPIO
#define	NU_PN2			0x04			    // PN2 = n/c			(i) GPIO
#define	NU_PN1			0x02			    // PN1 = n/c			(i) GPIO
#define	NU_PN0			0x01			    // PN0 = n/c			(i) GPIO
#define	PORTN_INIT	(0)
#define PORTN_DEN	(NU_PN0|NU_PN1|NU_PN2|NU_PN3|NU_PN4|NU_PN5)
#define PORTN_DIR	(0)
#define PORTN_PUR	(NU_PN0|NU_PN1|NU_PN2|NU_PN3|NU_PN4|NU_PN5)

// PORTM
#define PORTM SYSCTL_RCGCGPIO_R11
#define	ROW3			0x80		    	// PM7 = button in		(i) GPIO
#define	ROW2			0x40		    	// PM6 = button in		(i) GPIO
#define	ROW1			0x20		    	// PM5 = button in		(i) GPIO
#define	ROW0			0x10		    	// PM4 = button in		(i) GPIO
#define	ROW7			0x08		    	// PM3 = button in		(i) GPIO
#define	ROW6			0x04		    	// PM2 = button in		(i) GPIO
#define	ROW5			0x02		    	// PM1 = button in		(i) GPIO
#define	ROW4			0x01		    	// PM0 = button in		(i) GPIO
#define	PORTM_INIT	(0)
#define PORTM_DEN	(ROW7|ROW6|ROW5|ROW4|ROW3|ROW2|ROW1|ROW0)
#define PORTM_DIR	(0)
#define PORTM_PUR	(0) //(ROW7|ROW6|ROW5|ROW4|ROW3|ROW2|ROW1|ROW0)

// PORTL
#define PORTL SYSCTL_RCGCGPIO_R10
#define	SPARE_PL7		0x80		    	// PL7 = spare				(i) USBP
#define	SPARE_PL6		0x40		    	// PL6 = spare				(i) USBP
#define	EX766_PTT		0x20				// PL5 = PTT in EX766 mode	(i) GPIO
#define	BEEP			0x10				// PL4 = beep out			(o) T0CCP0
#define	C1O				0x08				// PL3 = comp1 out			(o) CMP1
#define	C0O				0x04			    // PL2 = comp0 out			(o) CMP0
#define	LCD_RSTN		0x02				// PL1 = LCD reset out		(o) GPIO
#define	NU_PL0			0x01			    // PL0 = n/c				(i) GPIO
#define	PORTL_INIT	(0)
#define PORTL_DEN	(SPARE_PL7|SPARE_PL6|EX766_PTT|BEEP|C1O|C0O|LCD_RSTN|NU_PL0)
#define PORTL_DIR	(BEEP|C1O|C0O|LCD_RSTN)
#define PORTL_PUR	(SPARE_PL7|SPARE_PL6|EX766_PTT|NU_PL0)
// IC-900 SM defines the beep frequency as 1/525us = 1905 Hz.  However, tests indicate that 3375 Hz
//	has a greater resonance and thus sounds much louder.
#define	BEEP_DURATION	75L								// #ms for beep
#define	BEEP_SHORT		20L								// #ms for beep
#define	BEEP_FREQ		3375L							// 3.3KHz beep frequency
#define	BEEP_COUNT		(BEEP_FREQ * BEEP_DURATION / 1000L)		// 0.075s beep pulse
#define	BEEP2_COUNT		(110)							// ~~2x beep pulse delay
#define	DIAL_BEEP_COUNT	(BEEP_FREQ * BEEP_SHORT / 1000L)
#define	BEEP_GAP		(BEEP_DURATION * 2)

// PORTK
#define PORTK SYSCTL_RCGCGPIO_R9
#define	LOCK			0x80		    	// PK7 = lock btn			 (i) GPIO
#define	SMUTE			0x40				// PK6 = smute btn			 (i) GPIO
#define	COL0			0x20				// PK5 = button col0		 (o) GPIO
#define	COL1			0x10				// PK4 = button col1		 (o) GPIO
#define	NU_PK3			0x08				// PK3 = n/c				 (i)
#define	NU_PK2			0x04				// PK2 = n/c				 (i)
#define	NU_PK1			0x02				// PK1 = n/c				 (i)
#define	RMT_RXD			0x01				// PK0 = UART4 RXD			 (i) UART4
#define	PORTK_INIT	(0)
#define PORTK_DEN	(LOCK|SMUTE|COL0|COL1|NU_PK3|NU_PK2|NU_PK1|RMT_RXD)
#define PORTK_DIR	(COL0|COL1)
#define PORTK_PUR	(NU_PK3|NU_PK2|NU_PK1|RMT_RXD)

// keypad defines
// [xduL mkrr rrrr rrcc] ==> bitmap of composite reg

#define	KB_COL_M		(COL1|COL0)	// mask for kb col 1of2
#define	KB_NOKEYM		(0xFF)							// key-mask semaphore (port M, rows)
#define	KB_NOKEYG		(CHECK)							// key-mask semaphore (port G)
#define	KB_NOKEYK		(SMUTE|LOCK)					// key-mask semaphore (port K)
#define	KB_NOKEYCOL		(COL1|COL0)						// key-mask semaphore (Port K, col bits)
#define	KB_NOKEY		(0x7FFCL)						// no keypressed semaphore (composite)
#define	KB_HIKEY		(0x7FFCL)						// upper key bit mask (composite)
#define	KB_UPKEY		(0x2000)						// MU_D2 "up" key-mask
#define	KB_DNKEY		(0x4000)						// MU_D2 "dn" key-mask
#define	KB_SCNKEY		(0x4000>>2)						// scan start value (1st key bit in keycode, shifted by 2 to account for the COL bits being shifted out)
#define	KB_SCNCNT		(13)							// scan count (# of key entries in each col of LUT)
#define	COL0_BIT		(0x02)							// composite key-bit locations
#define	COL1_BIT		(0x01)
#define	S4_BIT			(0x1000)
#define	S4_FLAG			(0x02)

#define	KP_DEBOUNCE_DN	2			// 20 ms of keypad debounce
#define	KP_DEBOUNCE_UP	3			// 30 ms of keypad debounce

// PORTJ
#define PORTJ SYSCTL_RCGCGPIO_R8
#define	NU_PJ1			0x02			    // PJ1 = LPC /CS		GPIO (o)
#define	NU_PJ0			0x01			    // PJ0 = LPC R/W		GPIO (o)
#define	PORTJ_INIT	(0)
#define PORTJ_DEN	(NU_PJ0|NU_PJ1)
#define PORTJ_DIR	(0)
#define PORTJ_PUR	(NU_PJ0|NU_PJ1)

// PORTH
#define PORTH SYSCTL_RCGCGPIO_R7
#define	BT_RESETN		0x08				// PH3 = BT reset out		GPIO (o)		// controls direction of 3V<->5V translator (PORTK)
#define	NU_PH2			0x04				// PH2 = n/c				(i)
#define	NU_PH1			0x02				// PH1 = n/c				(i)
#define	NU_PH0			0x01				// PH0 = n/c				(i)
#define	PORTH_INIT	(BT_RESETN) // init LPC bus, set vidsw to IC7K
#define PORTH_DEN	(BT_RESETN|NU_PH2|NU_PH1|NU_PH0)
#define PORTH_DIR	(BT_RESETN)
#define PORTH_PUR	NU_PH2|NU_PH1|NU_PH0

// PORTG
#define PORTG SYSCTL_RCGCGPIO_R6
#define	CHECK			0x02		    	// PG1 = button in			GPIO (i)
#define	ENC_UP			0x01				// PG0 = edge det			GPIO (i)
#define	PORTG_INIT	(0)
#define PORTG_DEN	(CHECK|ENC_UP)
#define PORTG_DIR	(0)
#define PORTG_PUR	(0)

// PORTF
#define PORTF SYSCTL_RCGCGPIO_R5
#define	ENC_DN			0x10				// PF4 = edge det			GPIO (i)
#define	DIMMER			0x08				// PF3 = BL LEDs			M0PWM3 (o)		// heartbeat LED
#define	LED_RXS			0x04				// PF2 = RX sub				M0PWM2 (o)
#define	LED_RXM			0x02				// PF1 = RX main			M0PWM1 (o)
#define	LED_TX			0x01				// PF0 = TX					M0PWM0 (o)		// TIVA D4
#define	PORTF_INIT	(0)
#define PORTF_DEN	(ENC_DN|DIMMER|LED_RXS|LED_RXM|LED_TX)
#define PORTF_DIR	(DIMMER|LED_RXS|LED_RXM|LED_TX)
#define PORTF_PUR	0

// PORTE
#define PORTE SYSCTL_RCGCGPIO_R4
#define	NU_PE5			0x20		    	// PE5 = n/c				(i)
#define	DATA1			0x10		    	// PE4 = MOSI1				SSI1 (o)		(radio SSI data)
#define	SPARE_PE3		0x08		    	// PE3 = spare pad			(i)
#define	SPARE_PE2		0x04		    	// PE2 = spare pad			(i)
#define	NU_PE1			0x02				// PE1 = n/c				(i)
#define	NU_PE0			0x01				// PE0 = n/c				(i)
#define	PORTE_INIT	(0)
#define PORTE_DEN	(NU_PE5|DATA1|SPARE_PE3|SPARE_PE2|NU_PE1|NU_PE0)
#define PORTE_DIR	DATA1
#define PORTE_PUR	(NU_PE5|SPARE_PE3|SPARE_PE2|NU_PE1|NU_PE0)

// PORTD
#define PORTD SYSCTL_RCGCGPIO_R3
#define	SPARE_PD7		0x80		    	// PD7 = spare pad			 (i)
#define	SPARE_PD6		0x40				// PD6 = spare pad			 (i)
#define	NU_PD5			0x20				// PD5 = dtmf tx enable	GPIO (o)
#define	NU_PD4			0x10				// PD4 = MANON sense	GPIO (o)
#define	LCD_CLK			0x08				// PD3 = SPI2 CLK			 (o)
#define	LCD_CE2			0x04				// PD2 = GPIO (o)			 (o)
#define	LCD_DATA		0x02				// PD1 = SPI2 TX			 (o)
#define	LCD_CE1			0x01				// PD0 = GPIO (o)			 (o)
#define	PORTD_INIT	(0)
#define PORTD_DEN	(SPARE_PD7|SPARE_PD6|NU_PD5|NU_PD4|LCD_CLK|LCD_CE2|LCD_DATA|LCD_CE1)
#define PORTD_DIR	(LCD_CLK|LCD_CE2|LCD_DATA|LCD_CE1)
#define PORTD_PUR	(SPARE_PD7|SPARE_PD6|NU_PD5|NU_PD4)

// PORTC
#define PORTC SYSCTL_RCGCGPIO_R2
#define	C0M_MUD2		0x80		    	// PC7 = comparator0 in-	(a)
#define	C0P				0x40		    	// PC6 = comparator0 in+	(a)
#define	C1P				0x20		    	// PC5 = comparator1 in+	(a)
#define	C1M_MUD2		0x10		    	// PC4 = comparator1 in-	(a)
#define	PORTC_INIT	(0)
#define PORTC_DEN	0x0f
#define PORTC_DIR	0x0a
#define PORTC_PUR	0x00
#define PORTC_AMSEL	(C0M_MUD2|C0P|C1P|C1M_MUD2)

// PORTB
#define PORTB SYSCTL_RCGCGPIO_R1
#define	CLK_DATA1		0x20		    	// PB5 = SPCK1				SSI1 (o)	(radio SSI clock)
#define	NU_PB4			0x10		    	// PB4 = n/c				(i)
#define	SPARE_PB3		0x08		    	// PB3 = spare pad			(i)
#define	SPARE_PB2		0x04		    	// PB2 = spare pad			(i)
#define	SPARE_PB1		0x02		    	// PB1 = spare pad			(i)
#define	SPARE_PB0		0x01		    	// PB0 = spare pad			(i)
#define	PORTB_INIT	(0)
#define PORTB_DEN	0xff
#define PORTB_DIR	(CLK_DATA1 | SPARE_PB0 | SPARE_PB1)
#define PORTB_PUR	(CLK_DATA1|SPARE_PB2|SPARE_PB3|NU_PB4)

// PORTA
#define PORTA SYSCTL_RCGCGPIO_R0
#define	TXD2		0x80		    	// PA7 = TXD2 (BT)		UART2
#define	RXD2		0x40		    	// PA6 = RXD2			UART2
#define	NVMISO		0x20		    	// PA5 = NVMISO, NVRAM
#define	NVMOSI		0x10		    	// PA4 = NVMOSI, NVRAM
#define	NVCS_N		0x08		    	// PA3 = NVCS_N, NVRAM
#define	NVCLK		0x04		    	// PA2 = SSI0CLK, NVRAM
#define	TXD0		0x02		    	// PA1 = TXD0 (CLI)		UART0
#define	RXD0		0x01		    	// PA0 = RXD0			UART0
#define	PORTA_INIT	(NVCS_N)
#define PORTA_DEN	(TXD2|RXD2|NVMISO|NVMOSI|NVCS_N|NVCLK|TXD0|RXD0)
#define PORTA_DIR	(TXD2|NVMOSI|NVCS_N|NVCLK|TXD0)
#define PORTA_PUR	(NVMISO)

// ***********************

#define BAND_28		0x01
#define BAND_52		0x02
#define BAND_144	0x04
#define BAND_220	0x08
#define BAND_440	0x10
#define BAND_1200	0x20

// ***********************

#define	TIMERISR	1			// set to 1 to enable ISR based timers
#define	READ_TIMER_CMD	0xffff	// cmd to read timer

//#define USE_QSPI	0xff
/*#define	USE_BIT_BANG_SPI	0xff

#define	LED_OFF		0x00		// LED commands
#define	LED_ON		0x01
#define	LED_BLINK	0x02
#define	LED_NOP		0xff

#define SEC10MS    10           // timer constants (ms)
#define SEC50MS    50           // timer constants (ms)
#define SEC75MS    74
#define SEC100MS  100
#define SEC250MS  250
#define SEC500MS  500
#define SEC750MS  750
#define SEC1     1000
#define ONESEC   SEC1
#define SEC2     2000
#define SEC3     3000
#define SEC5     5000
#define SEC10   10000
#define SEC11   11000
#define SEC15   15000
#define SEC16   16000
#define SEC30   30000
#define SEC60   60000
#define SEC300 300000L
#define	ONEMIN	(SEC60 * 60)
#define	KEY_TIME	SEC10
#define	READ_TIME	0xFFFF
#define	POWERON_DB		50
#define	POWERON_WAIT	SEC500MS
#define	POWEROFF_WAIT	SEC1
#define	LED_TIME	(8 * SEC100MS)
*/
// timer definitions.  Uses EXTXTAL #def to select between ext crystal and int osc
//  for normal mode.
// SYSCLK value in Hz

#define	MASTER_4800	4857

#define	SSI0_BR		3000000									// SSI0, NVRAM tx/rx, bit rate (3MHz)
#define	SSI0_SCR	19										// SSI0 SCR value
#define	SSI0_DVSR	(SYSCLK / ((1 + SSI0_SCR) * SSI0_BR))	// SSI0 SCR value

#define	SSI1_BR		MASTER_4800								// SSI1, DATA1 tx, bit rate (4800 baud, 4857 as measured)
#define	SSI1_SCR	99										// SSI1 SCR value
#define	SSI1_DVSR	(SYSCLK / ((1 + SSI1_SCR) * SSI1_BR))	// SSI1 SCR value

#define	SSI2_BR		200000 //100000 									// SSI2, LCD tx, bit rate (100 KHz)
#define	SSI2_SCR	15										// SSI2 SCR value
#define	SSI2_DVSR	(SYSCLK / ((1 + SSI2_SCR) * SSI2_BR))	// SSI2 SCR value

#define	SSI3_BR		4800								// SSI3, DATA2 rx, bit rate (4800 baud)
#define	SSI3_SCR	99										// SSI3 SCR value
#define	SSI3_DVSR	(SYSCLK / ((1 + SSI3_SCR) * SSI3_BR))	// SSI3 SCR value

#define OSC_LF		4            							// osc clock selects
#define OSC_HF		0
#define OSC_EXT		1
#define FXTAL		25000000								// fixed, this crystal is soldered to the Connected Launchpad
#define FPIOSC		16000000								// fixed, this is hard coded in the chip Si

#define SYSCLKL 10000L

// SIN bit time defines... assumes PS = 0 (SYSCLK/1 is used for timer)
#define SIO_BAUD	MASTER_4800
#define TIMER2A_PS 31
#define	SIN_BIT_TIME		(SYSCLK/(SIO_BAUD * (TIMER2A_PS + 1)))
#define	SIN_START_BIT_TIME	(SIN_BIT_TIME/2)					// 1/2 bit time delay to align SSI clock with mid-point of sin async data stream
#define	SIN_EOT_TIME		(18*SIN_BIT_TIME)					// 1 sin word time + 2 stop bits
// timer definitions
#define TIMER1_PS 31				// prescale value for timer1
#define	TIMER1_FREQ	9600			// timer1 intr freq
//#define TIMER3_ILR 0xffff			// timer 3 interval (24 bit)
//#define TIMER3_PS 0xff
//#define TIMER1_PS 32
#define TIMER3_ILR 0xffff			// timer 3 interval (24 bit)
#define TIMER3_PS 32

#define TIMER1B_PS 		1			// prescale value for timer1B
#define	BBSPICLK_FREQ	600000L		// edge freq for BB SPI clk

#define	BBSPICLK_FREQ	600000L		// edge freq for BB SPI clk

#define	TPULSE	(100L)				// in usec
#define TMIN	(((SYSCLK / 100) * TPULSE)/10000L)	// minimum pulse width



#define	PLLCLK								// enable pll config

#ifdef PLLCLK								// if defined, init PLL
	//    SysClk = fVCO / (PSYSDIV + 1) <== this is calculated in PLL_Init()
	//#define SYSCLK ((FXTAL/(Q+1)/(N+1))*(MINT+MFRAC/1024)/(PSYSDIV+1))
	// valid SYSCLKs
	#define SYSCLK 120000000
//	#define SYSCLK 96000000
//	#define SYSCLK 80000000
//	#define SYSCLK 60000000
//	#define SYSCLK 48000000
//	#define SYSCLK 30000000
//	#define SYSCLK 24000000
//	#define SYSCLK 16000000
	// The #define statement PSYSDIV initializes
	// the PLL to the desired frequency.
	// bus frequency is 480MHz/(PSYSDIV+1)
	#define PSYSDIV ((480000000/SYSCLK) - 1)
	// IMPORTANT: See Step 6) of PLL_Init().  If you change something, change 480 MHz.
	// IMPORTANT: You can use the 10-bit PSYSDIV value to generate an extremely slow
	// clock, but this may cause the debugger to be unable to connect to the processor
	// and return it to full speed.  If this happens, you will need to erase the whole
	// flash or somehow slow down the debugger.  You should also insert some mechanism
	// to return the processor to full speed such as a key wakeup or a button pressed
	// during reset.  The debugger starts to have problems connecting when the system
	// clock is about 600,000 Hz or slower; although inserting a failsafe mechanism
	// will completely prevent this problem.
	// IMPORTANT: Several peripherals (including but not necessarily limited to ADC,
	// USB, Ethernet, etc.) require a particular minimum clock speed.  You may need to
	// use the 16 MHz PIOSC as the clock for these modules instead of the PLL.
	#define Q            0
	#define N            4  // chosen for reference frequency within 4 to 30 MHz
	#define MINT        96  // 480,000,000 = (25,000,000/(0 + 1)/(4 + 1))*(96 + (0/1,024))
	#define MFRAC        0  // zero to reduce jitter

	#define TIMER0_PS 31					// prescale value for timer0
	#define	TIMER0_FREQ	1000				// timer0 intr freq
	#define TIMER1_PS 31					// prescale value for timer1
//	#define	TIMER1_FREQ	10000				// timer1 intr freq
//	#define TIMER1B_PS 31					// prescale value for timer1
	#define	TIMER1B_FREQ 100				// timer1B intr freq (57Hz is lowest possible)
	#define TIMER2B_PS 31					// prescale value for timer2B
	#define	TIMER2B_FREQ 100				// timer2B freq
	#define WAIT2_MUL 8
	#define WAIT3_MUL 16000					// no timerisr delay const
	#define	TIMER2_PS	1					// timer2 prescaler value
	#define	TIMER2_FREQ	(250000/2)			// timer2 output freq for TLC04		//350000	// timer2 output freq for TLC14
	#define	TIMER4_FREQ	FSAMP				// timer4 sample freq for DDS pwm
	#define	TIMER4_PS 0						// timer4 prescaler value - runs at the Full Monty SYSCLK rate
#else
	#define Q            0
	#define N            4  // chosen for reference frequency within 4 to 30 MHz
	#define MINT        96  // 480,000,000 = (25,000,000/(0 + 1)/(4 + 1))*(96 + (0/1,024))
	#define MFRAC        0  // zero to reduce jitter
	#define SYSCLK (FPIOSC)					// if no PLL, the PIOSC drives SYSCLK
	#define TIMER0_PS 31					// prescale value for timer0 (actual divisor is TIMERx_PS + 1)
	#define	TIMER0_FREQ	1000				// timer0 intr freq
	#define WAIT2_MUL 1
	#define WAIT3_MUL 2133					// no timerisr delay const
	#define	TIMER2_PS	1					// timer2 prescaler value (actual divisor is TIMERx_PS + 1)
	#define	TIMER2_FREQ	350000				// timer2 output freq for TLC14
	#define	TIMER4_FREQ	FSAMP				// timer4 sample freq for DDS pwm
	#define	TIMER4_PS 0						// timer4 prescaler value - runs at the Full Monty SYSCLK rate
#endif


//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------

#ifndef MAIN_C
extern U16	app_timer1ms;		// app timer
extern U16	xm_timer;			// xmodem timer
extern U16	ccmd_timer;			// ccmd timer
extern char	bchar;				// global bchar storage
extern char	swcmd;				// global swcmd storage
extern S8	handshake;			// xon/xoff enable
extern S8	xoffsent;			// xoff sent
#endif

// system error flag defines

#define	NOP				FALSE			// no-operation flag.  Passed to functions that have the option to perform an action or return a status
#define	CLR				TRUE			// perform read-and-clear operation flag.  Passed to functions that have the option to perform read status and clear
#define	KEY_NULL		'~'				// null chr for HMD keycode LUT
#define KEYP_IDLE		0x00			// keypad ISR state machine state IDs
#define KEYP_DBDN		0x01
#define KEYP_PRESSED	0x02
#define KEYP_HOLD		0x03
#define KEYP_DBUP		0x04
#define	KEY_PR_FL		0x01			// key-pressed bit field
#define	KEY_HOLD_FL		0x02			// key-hold bit field
#define	KEY_PRESCALE	10				// sets COL hold time (in ms +1)
#define KEY_HOLD_TIME	(SEC1/KEY_PRESCALE) // keypad hold timer value (~~ 1 sec)
#define	KEY_HOLD_KEY	0x80000000L		// set hi bit of key buffer entry to signal hold
#define	KEY_RELEASE_KEY	0x40000000L		// key release keycode
#define	KHOLD_FLAG		0x80			// flag bit for key hold character
#define	KREL_FLAG		0x40			// flag bit for key release character
#define	KEY_RELEASE_CHR	0x3e			// key release character
// key press character defines
#define	SUBchr			33
#define	LOCKchr			34
#define	CALLchr			35
#define	MWchr			36
#define	TSchr			37
#define	Tchr			38
#define	Vupchr			39
#define	Vdnchr			40
#define	HILOchr			41
#define	CHKchr			42
#define	errorchr		43

#define	MSchr			44
#define	BANDchr			45
#define	MODEchr			46
#define	VMchr			47
#define	MHZchr			48
#define	SETchr			49
#define	Qupchr			50
#define	Qdnchr			51
#define	SMUTEchr		52

#define	MUP2chr			53
#define	MDN2chr			54

// key hold chr codes
#define	SUBchr_H		(33 | KHOLD_FLAG)
#define	LOCKchr_H		(34 | KHOLD_FLAG)
#define	CALLchr_H		(35 | KHOLD_FLAG)
#define	MWchr_H			(36 | KHOLD_FLAG)
#define	TSchr_H			(37 | KHOLD_FLAG)
#define	Tchr_H			(38 | KHOLD_FLAG)
#define	Vupchr_H		(39 | KHOLD_FLAG)
#define	Vdnchr_H		(40 | KHOLD_FLAG)
#define	HILOchr_H		(41 | KHOLD_FLAG)
#define	CHKchr_H		(42 | KHOLD_FLAG)
#define	errorchr_H		(43 | KHOLD_FLAG)

#define	MSchr_H			(44 | KHOLD_FLAG)
#define	BANDchr_H		(45 | KHOLD_FLAG)
#define	MODEchr_H		(46 | KHOLD_FLAG)
#define	VMchr_H			(47 | KHOLD_FLAG)
#define	MHZchr_H		(48 | KHOLD_FLAG)
#define	SETchr_H		(49 | KHOLD_FLAG)
#define	Qupchr_H		(50 | KHOLD_FLAG)
#define	Qdnchr_H		(51 | KHOLD_FLAG)
#define	SMUTEchr_H		(52 | KHOLD_FLAG)

#define	MUP2chr_H		(53 | KHOLD_FLAG)
#define	MDN2chr_H		(54 | KHOLD_FLAG)

// key release chr codes
#define	SUBchr_R		(33 | KREL_FLAG)
#define	LOCKchr_R		(34 | KREL_FLAG)
#define	CALLchr_R		(35 | KREL_FLAG)
#define	MWchr_R			(36 | KREL_FLAG)
#define	TSchr_R			(37 | KREL_FLAG)
#define	Tchr_R			(38 | KREL_FLAG)
#define	Vupchr_R		(39 | KREL_FLAG)
#define	Vdnchr_R		(40 | KREL_FLAG)
#define	HILOchr_R		(41 | KREL_FLAG)
#define	CHKchr_R		(42 | KREL_FLAG)
#define	errorchr_R		(43 | KREL_FLAG)

#define	MSchr_R			(44 | KREL_FLAG)
#define	BANDchr_R		(45 | KREL_FLAG)
#define	MODEchr_R		(46 | KREL_FLAG)
#define	VMchr_R			(47 | KREL_FLAG)
#define	MHZchr_R		(48 | KREL_FLAG)
#define	SETchr_R		(49 | KREL_FLAG)
#define	Qupchr_R		(50 | KREL_FLAG)
#define	Qdnchr_R		(51 | KREL_FLAG)
#define	SMUTEchr_R		(52 | KREL_FLAG)

#define	MUP2chr_R		(53 | KREL_FLAG)
#define	MDN2chr_R		(54 | KREL_FLAG)

#define	HIB_APPL	0					// HIB is being accessed by application
#define	HIB_INTR	1					// HIB may be accessed by intrpt

#define	GPIO_PORTA_DATA_R		GPIO_PORTA_AHB_DATA_R
#define	GPIO_PORTA_AFSEL_R		GPIO_PORTA_AHB_AFSEL_R
#define	GPIO_PORTA_PCTL_R		GPIO_PORTA_AHB_PCTL_R
#define	GPIO_PORTA_DIR_R		GPIO_PORTA_AHB_DIR_R
#define	GPIO_PORTA_DEN_R		GPIO_PORTA_AHB_DEN_R

#define	GPIO_PORTB_DATA_R		GPIO_PORTB_AHB_DATA_R
#define	GPIO_PORTB_AFSEL_R		GPIO_PORTB_AHB_AFSEL_R
#define	GPIO_PORTB_PCTL_R		GPIO_PORTB_AHB_PCTL_R
#define	GPIO_PORTB_DIR_R		GPIO_PORTB_AHB_DIR_R
#define	GPIO_PORTB_DEN_R		GPIO_PORTB_AHB_DEN_R

#define	GPIO_PORTC_DATA_R		GPIO_PORTC_AHB_DATA_R
#define	GPIO_PORTC_AFSEL_R		GPIO_PORTC_AHB_AFSEL_R
#define	GPIO_PORTC_PCTL_R		GPIO_PORTC_AHB_PCTL_R
#define	GPIO_PORTC_DIR_R		GPIO_PORTC_AHB_DIR_R
#define	GPIO_PORTC_DEN_R		GPIO_PORTC_AHB_DEN_R
#define	GPIO_PORTC_MIS_R		GPIO_PORTC_AHB_MIS_R
#define	GPIO_PORTC_IM_R			GPIO_PORTC_AHB_IM_R
#define	GPIO_PORTC_ICR_R		GPIO_PORTC_AHB_ICR_R

#define	GPIO_PORTD_DATA_R		GPIO_PORTD_AHB_DATA_R
#define	GPIO_PORTD_AFSEL_R		GPIO_PORTD_AHB_AFSEL_R
#define	GPIO_PORTD_PCTL_R		GPIO_PORTD_AHB_PCTL_R
#define	GPIO_PORTD_DIR_R		GPIO_PORTD_AHB_DIR_R
#define	GPIO_PORTD_DEN_R		GPIO_PORTD_AHB_DEN_R
#define	GPIO_PORTD_PUR_R		GPIO_PORTD_AHB_PUR_R
#define	GPIO_PORTD_AMSEL_R		GPIO_PORTD_AHB_AMSEL_R

#define	GPIO_PORTE_DATA_R		GPIO_PORTE_AHB_DATA_R
#define	GPIO_PORTE_AFSEL_R		GPIO_PORTE_AHB_AFSEL_R
#define	GPIO_PORTE_PCTL_R		GPIO_PORTE_AHB_PCTL_R
#define	GPIO_PORTE_DIR_R		GPIO_PORTE_AHB_DIR_R
#define	GPIO_PORTE_DEN_R		GPIO_PORTE_AHB_DEN_R

#define	GPIO_PORTF_DATA_R		GPIO_PORTF_AHB_DATA_R
#define	GPIO_PORTF_AFSEL_R		GPIO_PORTF_AHB_AFSEL_R
#define	GPIO_PORTF_PCTL_R		GPIO_PORTF_AHB_PCTL_R
#define	GPIO_PORTF_DIR_R		GPIO_PORTF_AHB_DIR_R
#define	GPIO_PORTF_DEN_R		GPIO_PORTF_AHB_DEN_R
#define	GPIO_PORTF_MIS_R		GPIO_PORTF_AHB_MIS_R
#define	GPIO_PORTF_IM_R			GPIO_PORTF_AHB_IM_R
#define	GPIO_PORTF_ICR_R		GPIO_PORTF_AHB_ICR_R

#define	GPIO_PORTG_DATA_R		GPIO_PORTG_AHB_DATA_R
#define	GPIO_PORTG_AFSEL_R		GPIO_PORTG_AHB_AFSEL_R
#define	GPIO_PORTG_PCTL_R		GPIO_PORTG_AHB_PCTL_R
#define	GPIO_PORTG_DIR_R		GPIO_PORTG_AHB_DIR_R
#define	GPIO_PORTG_DEN_R		GPIO_PORTG_AHB_DEN_R
#define	GPIO_PORTG_MIS_R		GPIO_PORTG_AHB_MIS_R
#define	GPIO_PORTG_IM_R			GPIO_PORTG_AHB_IM_R
#define	GPIO_PORTG_ICR_R		GPIO_PORTG_AHB_ICR_R

#define	GPIO_PORTH_DATA_R		GPIO_PORTH_AHB_DATA_R
#define	GPIO_PORTH_AFSEL_R		GPIO_PORTH_AHB_AFSEL_R
#define	GPIO_PORTH_PCTL_R		GPIO_PORTH_AHB_PCTL_R
#define	GPIO_PORTH_DIR_R		GPIO_PORTH_AHB_DIR_R
#define	GPIO_PORTH_DEN_R		GPIO_PORTH_AHB_DEN_R

#define	GPIO_PORTJ_DATA_R		GPIO_PORTJ_AHB_DATA_R
#define	GPIO_PORTJ_AFSEL_R		GPIO_PORTJ_AHB_AFSEL_R
#define	GPIO_PORTJ_PCTL_R		GPIO_PORTJ_AHB_PCTL_R
#define	GPIO_PORTJ_DIR_R		GPIO_PORTJ_AHB_DIR_R
#define	GPIO_PORTJ_DEN_R		GPIO_PORTJ_AHB_DEN_R


//-----------------------------------------------------------------------------
// main.c Fn prototypes
//-----------------------------------------------------------------------------

void Init_Device(void);
char process_IO(U8 flag);
void  process_chg(U8 chg, char* buf, U8* key_mem, U8* keyh_mem);
U8 get_status(U8* keyh_mem);
U8 set_pttmode(U8 value);
void set_pwm(U8 pwmnum, U8 percent);
void set_led(U8 lednum, U8 value);

void wrwhib_ram(U32 data, volatile U8 addr);
void wrhib_ram(uint8_t data, U8 addr);
uint32_t rdwhib_ram(U8 addr);
uint8_t rdhib_ram(U8 addr);

void waitpio(U16 waitms);
void wait(U16 waitms);
void set_wait(U16 waitms);
U8 is_wait(void);
void wait2(U16 waitms);
U16 getipl(void);
U32 get_syserr(U8 opr);
U8 wait_busy0(U16 delay);
U8 wait_busy1(U16 delay);
U8 wait_reg0(volatile uint32_t *regptr, uint32_t clrmask, U16 delay);
U8 wait_reg1(volatile uint32_t *regptr, uint32_t setmask, U16 delay);

U8 got_key(void);
//U8 not_key(U8 flag);
char get_key(void);

void warm_reset(void);
U32 free_run(void);
void set_dial(S8 value);
S8 get_dial(U8 tf);
void do_dial_beep(void);
void do_1beep(void);
void do_2beep(void);
void do_3beep(void);
U8 sin_time(U8 cmd);
U8 sout_time(U8 cmd);
U8 mhz_time(U8 tf);
U8 v_time(U8 tf);
U8 q_time(U8 tf);
U8 set_time(U8 tf);
U8 offs_time(U8 tf);
U8 sub_time(U8 tf);
U8 mic_time(U8 tf);
U8 micdb_time(U8 tf);
U8 mute_time(U8 tf);
U8 ts_time(U8 tf);
U8 slide_time(U8 tf);
U8 scan_time(U8 focus, U8 tf);
U32 get_free(void);

void set_beep(U16 beep_frq, U16 b_count);
void do_beep(U16 beep_cycles);
void Timer0A_ISR(void);

void gpiof_isr(void);
void gpiog_isr(void);
void Timer0B_ISR(void);

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
