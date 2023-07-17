/********************************************************************
 *********** COPYRIGHT (c) 2015 by NOV, Cedar Park, TX   ************
 *
 *  File name: pll.c
 *
 *  Module:    Control
 *
 *  Summary:
 *  This file holds the PLL code
 *
 *  Project scope declarations revision history:
 *    2-28-15 jmh:  import date (see endo fo file for import source credits)
 *
 *******************************************************************/

/********************************************************************
 *  File scope declarations revision history:
 *    04-08-15 jmh:  Edited formatting for style and added standard file sections
 *    				 Added return codes to flag errors or OK result to caller
 *    				 Moved #defines to init.h to consolodate SYSCLK/PLL declarations
 *    				 Modified PLL_Init() to use desired SYSCLK as parameter.  Valid
 *    				 SYSCLK values are <= 120 MHz.  Actual SYSCLK is determined by
 *    				 SYSCLK = 480/x, where "x" is an integer in the range {3 <= x <= 127}
 *    				 Modified PLL_Init() to return control to PIOSC if error
 *
 *******************************************************************/

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
// compile defines

#define PLL_C
#include <stdint.h>
#include "typedef.h"
#include "PLL.h"
#include "init.h"
#include "inc/tm4c1294ncpdt.h"

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
// PLL_Init() configures the system to use the PLL as the SYSCLK source
// returns status (U16) if error or OK
// The #define statement PSYSDIV in init.h initializes the PLL to the desired frequency.
// bus frequency is 480MHz/(PSYSDIV+1) = 480MHz/(3+1) = 120 MHz
// IMPORTANT: See Step 6) of PLL_Init().  If you change something, change 480 MHz.
// REF the following table:e
//		PSYSDIV  SysClk (Hz)
//			3     120,000,000
//			4      96,000,000
//			5      80,000,000
//			7      60,000,000
//			9      48,000,000
//			15     30,000,000
//			19     24,000,000
//			29     16,000,000
//			39     12,000,000
//			79      6,000,000
//*****************************************************************************
U16 PLL_Init(U32 sys_clk){
	uint32_t	timeout;
	U8			psys_div = 3;		// default PSYSDIV

	if((sys_clk <= 120000000) && (sys_clk > 3720930)){
		psys_div = (((FXTAL/(Q+1)/(N+1)) * (MINT+MFRAC/1024))/sys_clk) - 1;
	}else{
		return PLL_INVALIDSYSCLK;
	}
	// 1) Once POR has completed, the PIOSC is acting as the system clock.  Just in case
	//    this function has been called previously, be sure that the system is not being
	//    clocked from the PLL while the PLL is being reconfigured.
	SYSCTL_RSCLKCFG_R &= ~SYSCTL_RSCLKCFG_USEPLL;
	// 2) Power up the MOSC by clearing the NOXTAL bit in the SYSCTL_MOSCCTL_R register.
	// 3) Since crystal mode is required, clear the PWRDN bit.  The datasheet says to do
	//    these two operations in a single write access to SYSCTL_MOSCCTL_R.
	SYSCTL_MOSCCTL_R &= ~(SYSCTL_MOSCCTL_NOXTAL|SYSCTL_MOSCCTL_PWRDN);
	//    Wait for the MOSCPUPRIS bit to be set in the SYSCTL_RIS_R register, indicating
	//    that MOSC crystal mode is ready.
	while((SYSCTL_RIS_R & SYSCTL_RIS_MOSCPUPRIS) == 0){};
	// 4) Set both the OSCSRC and PLLSRC fields to 0x3 in the SYSCTL_RSCLKCFG_R register
	//    at offset 0x0B0.
	//    Temporarily get run/sleep clock from 25 MHz main oscillator.
	SYSCTL_RSCLKCFG_R = (SYSCTL_RSCLKCFG_R & ~SYSCTL_RSCLKCFG_OSCSRC_M) + SYSCTL_RSCLKCFG_OSCSRC_MOSC;
	//    PLL clock from main oscillator.
	SYSCTL_RSCLKCFG_R = (SYSCTL_RSCLKCFG_R & ~SYSCTL_RSCLKCFG_PLLSRC_M) + SYSCTL_RSCLKCFG_PLLSRC_MOSC;
	// 5) If the application also requires the MOSC to be the deep-sleep clock source,
	//    then program the DSOSCSRC field in the SYSCTL_DSCLKCFG_R register to 0x3.
	//    Get deep-sleep clock from main oscillator (few examples use deep-sleep; optional).
	SYSCTL_DSCLKCFG_R = (SYSCTL_DSCLKCFG_R & ~SYSCTL_DSCLKCFG_DSOSCSRC_M) + SYSCTL_DSCLKCFG_DSOSCSRC_MOSC;
	// 6) Write the SYSCTL_PLLFREQ0_R and SYSCTL_PLLFREQ1_R registers with the values of
	//    Q, N, MINT, and MFRAC to configure the desired VCO frequency setting.
	//    ************
	//    The datasheet implies that the VCO frequency can go as high as 25.575 GHz
	//    with MINT=1023 and a 25 MHz crystal.  This is clearly unreasonable.  For lack
	//    of a recommended VCO frequency, this program sets Q, N, and MINT for a VCO
	//    frequency of 480 MHz with MFRAC=0 to reduce jitter.  To run at a frequency
	//    that is not an integer divisor of 480 MHz, change this section.
	//    fVC0 = (fXTAL/(Q + 1)/(N + 1))*(MINT + (MFRAC/1,024))
	//    fVCO = 480,000,000 Hz (arbitrary, but presumably as small as needed)

	SYSCTL_PLLFREQ0_R = (SYSCTL_PLLFREQ0_R & ~SYSCTL_PLLFREQ0_MFRAC_M)+(MFRAC << SYSCTL_PLLFREQ0_MFRAC_S) |
                        (SYSCTL_PLLFREQ0_R & ~SYSCTL_PLLFREQ0_MINT_M)+(MINT << SYSCTL_PLLFREQ0_MINT_S);
	SYSCTL_PLLFREQ1_R = (SYSCTL_PLLFREQ1_R & ~SYSCTL_PLLFREQ1_Q_M)+(Q << SYSCTL_PLLFREQ1_Q_S) |
                        (SYSCTL_PLLFREQ1_R & ~SYSCTL_PLLFREQ1_N_M)+(N << SYSCTL_PLLFREQ1_N_S);
	SYSCTL_PLLFREQ0_R |= SYSCTL_PLLFREQ0_PLLPWR;       // turn on power to PLL
	SYSCTL_RSCLKCFG_R |= SYSCTL_RSCLKCFG_NEWFREQ;      // lock in register changes
	// 7) Write the SYSCTL_MEMTIM0_R register to correspond to the new clock setting.
	//    ************
	//    Set the timing parameters to the main Flash and EEPROM memories, which
	//    depend on the system clock frequency.  See Table 5-12 in datasheet.
	if(sys_clk < 16000000){
		// FBCHT/EBCHT = 0, FBCE/EBCE = 0, FWS/EWS = 0
		SYSCTL_MEMTIM0_R = (SYSCTL_MEMTIM0_R & ~0x03EF03EF) + (0x0<<22) + (0x0<<21) + (0x0<<16) + (0x0<<6) + (0x0<<5) + (0x0);
	}else if(sys_clk == 16000000){
		// FBCHT/EBCHT = 0, FBCE/EBCE = 1, FWS/EWS = 0
		SYSCTL_MEMTIM0_R = (SYSCTL_MEMTIM0_R & ~0x03EF03EF) + (0x0<<22) + (0x1<<21) + (0x0<<16) + (0x0<<6) + (0x1<<5) + (0x0);
	}else if(sys_clk <= 40000000){
		// FBCHT/EBCHT = 2, FBCE/EBCE = 0, FWS/EWS = 1
		SYSCTL_MEMTIM0_R = (SYSCTL_MEMTIM0_R & ~0x03EF03EF) + (0x2<<22) + (0x0<<21) + (0x1<<16) + (0x2<<6) + (0x0<<5) + (0x1);
	}else if(sys_clk <= 60000000){
		// FBCHT/EBCHT = 3, FBCE/EBCE = 0, FWS/EWS = 2
		SYSCTL_MEMTIM0_R = (SYSCTL_MEMTIM0_R & ~0x03EF03EF) + (0x3<<22) + (0x0<<21) + (0x2<<16) + (0x3<<6) + (0x0<<5) + (0x2);
	}else if(sys_clk <= 80000000){
		// FBCHT/EBCHT = 4, FBCE/EBCE = 0, FWS/EWS = 3
		SYSCTL_MEMTIM0_R = (SYSCTL_MEMTIM0_R & ~0x03EF03EF) + (0x4<<22) + (0x0<<21) + (0x3<<16) + (0x4<<6) + (0x0<<5) + (0x3);
	}else if(sys_clk <= 100000000){
		// FBCHT/EBCHT = 5, FBCE/EBCE = 0, FWS/EWS = 4
		SYSCTL_MEMTIM0_R = (SYSCTL_MEMTIM0_R & ~0x03EF03EF) + (0x5<<22) + (0x0<<21) + (0x4<<16) + (0x5<<6) + (0x0<<5) + (0x4);
	}else if(sys_clk <= 120000000){
		// FBCHT/EBCHT = 6, FBCE/EBCE = 0, FWS/EWS = 5
		SYSCTL_MEMTIM0_R = (SYSCTL_MEMTIM0_R & ~0x03EF03EF) + (0x6<<22) + (0x0<<21) + (0x5<<16) + (0x6<<6) + (0x0<<5) + (0x5);
	}else{
		// A setting is invalid, and the PLL cannot clock the system faster than 120 MHz.
		// Skip the rest of the initialization, leaving the system clocked from the MOSC,
		// which is a 25 MHz crystal.
		// return SYSCLK to PIOSC source:
		SYSCTL_RSCLKCFG_R = 0;
		SYSCTL_MOSCCTL_R = SYSCTL_MOSCCTL_NOXTAL|SYSCTL_MOSCCTL_PWRDN;
		// error exit;
		return PLL_INVALIDSYSCLK;
	}
	// 8) Wait for the SYSCTL_PLLSTAT_R register to indicate that the PLL has reached
	//    lock at the new operating point (or that a timeout period has passed and lock
	//    has failed, in which case an error condition exists and this sequence is
	//    abandoned and error processing is initiated).
	timeout = 0;
	while(((SYSCTL_PLLSTAT_R & SYSCTL_PLLSTAT_LOCK) == 0) && (timeout++ < 0xffff));
	if(timeout >= 0xffff){
		// The PLL never locked or is not powered.
		// return SYSCLK to PIOSC source:
		SYSCTL_RSCLKCFG_R = 0;
		SYSCTL_MOSCCTL_R = SYSCTL_MOSCCTL_NOXTAL|SYSCTL_MOSCCTL_PWRDN;
		// error exit;
		return PLL_NOLOCK;
	}
	// 9)Write the SYSCTL_RSCLKCFG_R register's PSYSDIV value, set the USEPLL bit to
	//   enabled, and set the MEMTIMU bit.
	SYSCTL_RSCLKCFG_R = (SYSCTL_RSCLKCFG_R & ~SYSCTL_RSCLKCFG_PSYSDIV_M)+(psys_div & SYSCTL_RSCLKCFG_PSYSDIV_M) |
                         SYSCTL_RSCLKCFG_MEMTIMU |
                         SYSCTL_RSCLKCFG_USEPLL;
	return PLL_OK;
}

//*****************************************************************************
// PIOSC_Init() configures the system to use the PIOSC
//*****************************************************************************
void PIOSC_Init(void){
	// set SYSCLK to PIOSC source:
	SYSCTL_RSCLKCFG_R = 0;
	SYSCTL_MOSCCTL_R = SYSCTL_MOSCCTL_NOXTAL|SYSCTL_MOSCCTL_PWRDN;
}

//*******************************************************************************************************************
// This file was modified from source obtained from utexas.edu.  Original credit header follows:
//
// PLL.c
// Runs on TM4C1294
// A software function to change the bus frequency using the PLL.
// Daniel Valvano
// April 9, 2014

/* This example accompanies the book
   "Embedded Systems: Introduction to Arm Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2014
   Program 4.6, Section 4.3
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2014
   Program 2.10, Figure 2.37

 Copyright 2014 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */
