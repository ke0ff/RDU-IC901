/********************************************************************
 ************ COPYRIGHT (c) 2015 by ke0ff, Taylor, TX   *************
 *
 *  File name: eeprom.h
 *
 *  Module:    Control
 *
 *  Summary:   defines and global declarations for io.c
 *
 *  Project scope revision history:
 *    03-23-15 jmh:  creation date
 *
 *******************************************************************/

#include "typedef.h"
#include <stdint.h>

#ifndef EEPROM_H
#define EEPROM_H
#endif

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------
#define	EE_FAIL		0xff		// fail return value
#define	EE_OK		0x00		// OK return value
#define	EE_BLOCK	16			// # words/block
#define	EEREG_WAIT_DLY	200		// ms to wait for reg bit(s)
//#define	IPL_EEPERR	0x0001		// eprom error
//#define	IPL_REGWERR	0x0002		// reg wait err

#define	EE_ADJUST_LEV	0		// EE address of level storage word.  tone_level[7:0]
//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Fn prototypes
//-----------------------------------------------------------------------------
U16 eeprom_init(void);
U32 eerd(U16 addr);
U8  eewr(U16 addr, U32 data);

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
