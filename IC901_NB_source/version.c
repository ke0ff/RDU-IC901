/********************************************************************
 ************ COPYRIGHT (c) 2022 by KE0FF, Taylor, TX   *************
 *
 *  File name: serial.c
 *
 *  Module:    Control
 *
 *  Summary:   This is the serial I/O module for the FF-PGMR11
 *             protocol converter.
 *
 *******************************************************************/


/********************************************************************
 *  File scope declarations revision history:
 *    07-30-14 jmh:  creation date
 *
 *******************************************************************/

#include "inc/tm4c1294ncpdt.h"
#include "typedef.h"
#define VERSOURCE
#include "version.h"
#include "stdio.h"
#include "serial.h"

//------------------------------------------------------------------------------
// Define Statements
//------------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Local Variable Declarations
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Local Fn Declarations
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// dispSWvers() displays SW version to console
//-----------------------------------------------------------------------------
void dispSWvers(char* s){

	puts0("\n\nIC-901F Controller, de KE0FF");
    sprintf(s,"Vers: %s   Date: %s",version_number,date_code);
    puts0(s);
    puts0("Enter \"?\" for help.\n");
}

//-----------------------------------------------------------------------------
// rtrnSWvers() puts truncated SW version into string
//-----------------------------------------------------------------------------
void rtrnSWvers(char* s){

    sprintf(s,"%s",version_number);
}

//-----------------------------------------------------------------------------
// rtrnSWvers() puts truncated SW version into string
//-----------------------------------------------------------------------------
void rtrnSWdate(char* s){

    sprintf(s,"%s",date_code);
}

//-----------------------------------------------------------------------------
// ccmdSWvers() returns SW version for ccmds
//-----------------------------------------------------------------------------
void ccmdSWvers(char* sbuf){

    sprintf(sbuf,"IC-901,IPL:%04x,Vers:%s,%s",getipl(),version_number,date_code);
}

//-----------------------------------------------------------------------------
// nvram_sn() returns NVRAM sernum
//	this number is used to validate NVRAM revision.  If this number differs
//	from that stored on the NVRAM, the system must execute nvram_fix() to update
//	or re-init the SRAM contents.
//-----------------------------------------------------------------------------
U16 nvram_sn(void){

    return 0x0001;
}
