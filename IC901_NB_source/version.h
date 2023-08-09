/********************************************************************
 ************ COPYRIGHT (c) 2022 by KE0FF, Taylor, TX   *************
 *
 *  File name: version.h
 *
 *  Module:    Control
 *
 *  Summary:   This header contains the software version number
 *             as a character string.
 *
 *******************************************************************/

#include <stdint.h>
#include "typedef.h"

/********************************************************************
 *  File scope declarations revision history:
 *    07-30-14 jmh:  creation date
 *
 *******************************************************************/

#ifdef VERSOURCE
const S8    version_number[] = {"0.0"};
const S8    date_code[]      = {"08-Aug-2023"};
#endif

//-----------------------------------------------------------------------------
// Public Fn Declarations
//-----------------------------------------------------------------------------
void dispSWvers(char* s);
void rtrnSWvers(char* s);
void rtrnSWdate(char* s);
U16 nvram_sn(void);
void ccmdSWvers(char* sbuf);

#define VERSION_INCLUDED
