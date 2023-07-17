/********************************************************************
 *********** COPYRIGHT (c) 2015 by NOV, Cedar Park, TX   ************
 *
 *  File name: init.h
 *
 *  Module:    Control
 *
 *  Summary:   defines and global declarations for main.c
 *
 *******************************************************************/

#include "typedef.h"

#ifndef PLL_H
#define PLL_H
#endif

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

#define	PLL_INVALIDSYSCLK	0x1000
#define	PLL_NOLOCK			0x2000
#define	PLL_OK				0x4000

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// Puplic Fn prototypes
//-----------------------------------------------------------------------------

U16 PLL_Init(U32 sys_clk);					// configure the system to get its clock from the PLL
void PIOSC_Init(void);						// configure PIOSC as SYSCLK

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------

//
//
//***********************************************************************************
// Original File Source Header follows:
// PLL.h
// Runs on TM4C1294
// A software function to change the bus frequency using the PLL.
// Daniel Valvano
// March 27, 2014

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
