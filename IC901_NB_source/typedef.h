/********************************************************************
 ************ COPYRIGHT (c) 2015 by KE0FF, Taylor, TX   *************
 *
 *  File name: typedef.h
 *
 *  Module:    Control
 *
 *  Summary:   This header contains the variable declaration definitions
 *       used by all modules of the system.
 *
 *
 *******************************************************************/

/********************************************************************
 *  File scope declarations revision history:
 *    07-30-14 jmh:  creation date
 *
 *******************************************************************/
//#include <stdint.h>

/* shorthand type definitions */

#define U8				uint8_t		//unsigned char
#define S8				int8_t		//signed char
#define U16				uint16_t
#define S16				int16_t
#define U32				uint32_t	//unsigned long
#define S32				int32_t		//signed long
#define U64				uint64_t	//unsigned long-long
#define S64				sint64_t	//signed long-long
#define F32				float
#define F64				double
#define BOOL			uint8_t		//unsigned char

#define TRUE			1
#define FALSE			!TRUE

#define	NAN_BCD8		0xff
#define	NAN_BCD16		0xffff

#define TYPEDEF_INCLUDED
