/********************************************************************
 ************ COPYRIGHT (c) 2015 by KE0FF, Taylor, TX   *************
 *
 *  File name: spi.h
 *
 *  Module:    Control
 *
 *  Summary:   defines and global declarations for spi.c
 *
 *******************************************************************/

#include "typedef.h"
#include <stdint.h>
#ifndef SIO_H_
#define SIO_H_


//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

#define	FLUSH_SIN	1
#define	INIT_SIN	0

// SOUT data word defines
// masks & data bit defn's
#define SOUT_ADDR	0x38000000L	// bits [29:27]

#define SOUT_CNTL	0x07f00000L	// bits [26:20]
#define SOUT_DATA	0x000fffffL	// bits [19:0]
// addr bits
#define SOUT_UX19	0x08000000L
#define SOUT_UX59	0x10000000L
#define SOUT_UX29	0x18000000L
#define SOUT_UX39	0x20000000L
#define SOUT_UX49	0x28000000L
#define SOUT_UX129	0x30000000L

// Front Unit control signals
#define SOUT_MAIN	0x04000000L
#define SOUT_SUB	0x02000000L
#define SOUT_PON	0x01000000L
#define SOUT_LOHI	0x00800000L
#define SOUT_BAND	0x00400000L
#define SOUT_PTT	0x00200000L	// band unit PTT
#define	SOUT_NU0	0x00100000L	// not used bit
#define	SOUT_PLLM	0x000fffffL	// mask - 20 bits of PLL data

// addr == 0x7
#define SOUT_ADDR7	0x38000000L	// B-unit addr
#define SOUT_OPT12	0x04000000L	// if "1" MAIN connects to OPT1
								// if "0" MAIN connects to OPT2
#define SOUT_VOL	0x02000000L	// lower 18 data bits active
#define SOUT_SQL	0x01000000L	// lower 18 data bits active
#define SOUT_TONEA	0x00800000L	// lower 8 data bits active
#define SOUT_DSQL	0x00400000L	// if "1", mutes audio
#define SOUT_SEL12	0x00200000L	// if TONEA == 1 && SEL12 == 1, OPT1 is addressed
								// if TONEA == 1 && SEL12 == 0, OPT2 is addressed

// SIN data word defines
#define SIN_ADDR_MASK	0xC000
#define SIN_ADDR_INIT	(0 << 14)
#define SIN_ADDR_PRIME	(1 << 14)
#define SIN_ADDR_SPRDC	(2 << 14)
#define SIN_ADDR_PAGNG	(3 << 14)

#define SIN_STOP		0x0003			// mask - stop validate bits
//SIN_ADDR = 0 (INIT)
#define SIN0_UX19		0x2000			// UX19 module detected
#define SIN0_UX59		0x1000			// UX59 module detected
#define SIN0_2M			0x0800			// base 2m module "slot" (this bit is never sent in the SIN0 field, but we use it internally
#define SIN0_UX39		0x0400			// UX39 module detected
#define SIN0_70CM		0x0200			// base 440 module "slot" (this bit is never sent in the SIN0 field, but we use it internally
#define SIN0_UX129		0x0100			// UX19 module detected
#define SIN0_UXQ		0x0080			// "new" 1200 module detected
#define SIN0_UXV1		0x0040			// "new" 144 module detected
#define SIN0_UXU1		0x0020			// "new" 440 module detected
#define SIN0_UXR91		0x0010			// WBRX module detected
#define SIN0_UXS92		0x0008			// 144ssb module detected
#define SIN0_UXW		0x0004			// 430ssb module detected
#define SIN0_STOP		0x0003			// stop bits

#define SIN0_ALL		(SIN0_UX19 | SIN0_UX59 | SIN0_UX39 | SIN0_UX129 | SIN0_UXQ | SIN0_UXV1 | SIN0_UXU1 | SIN0_UXR91 | SIN0_UXS92 | SIN0_UXW)

//SIN_ADDR = 1 (PRIME)
#define SIN_BUSY		0x2000			// module selected feedback
#define SIN_SQSA		0x1000			// main COS
#define SIN_SQSB		0x0800			// sub COS
#define SIN_SRFA		0x0780			// mask (main)
#define	SIN1_SRF_MAIN_B	7
#define SIN_SRFB		0x0078			// mask (sub)
#define	SIN1_SRF_SUB_B	3
#define SIN1_DATA		(SIN_BUSY|SIN_SQSA|SIN_SQSB|SIN_SRFA|SIN_SRFB)	// mask

#define SIN1_CF_SHR		9							// bit shift for change flags
#define SIN_BUSY_CF		(0x2000 >> SIN1_CF_SHR)		// module selected feedback
#define SIN_SQSA_CF		(0x1000 >> SIN1_CF_SHR)		// main COS
#define SIN_SQSB_CF		(0x0800 >> SIN1_CF_SHR)		// sub COS
#define SIN_SRFM_CF		(0x0400 >> SIN1_CF_SHR)		// main SRF
#define SIN_SRFS_CF		(0x0200 >> SIN1_CF_SHR)		// sub SRF

// SIN_ADDR = 2 (SPORADIC)
#define SIN_SEND		0x2000						// /PTT active
#define SIN_DSQA		0x1000						// "1" = tone detected
#define SIN_DSQB		0x0800
#define SIN_MUP			0x0400						// MIC u/d button pressed
#define SIN_MDN			0x0200						// MIC u/d button state, "1" = UP
#define SIN_SCAN		0x0100						// ???
#define SIN_TONE		0x0080						// OPT detect
#define SIN_OPT1		0x0040
#define SIN_OPT2		0x0020
#define SIN_OPT3		0x0010
#define SIN2_DATA		(SIN_SEND|SIN_DSQA|SIN_DSQB|SIN_MUP|SIN_MDN|SIN_SCAN|SIN_TONE|SIN_OPT1|SIN_OPT2|SIN_OPT3)	// mask

#define SIN2_CF_SHL		1							// bit shift for change flags
#define SIN_SEND_CF		(0x2000 << SIN2_CF_SHL)		// /PTT active
#define SIN_DSQA_CF		(0x1000 << SIN2_CF_SHL)		// "1" = tone detected
#define SIN_DSQB_CF		(0x0800 << SIN2_CF_SHL)
#define SIN_MUP_CF		(0x0400 << SIN2_CF_SHL)		// MIC u/d button pressed
#define SIN_MDN_CF		(0x0200 << SIN2_CF_SHL)		// MIC u/d button state, "1" = UP
#define SIN_SCAN_CF		(0x0100 << SIN2_CF_SHL)		// ???
#define SIN_TONE_CF		(0x0080 << SIN2_CF_SHL)		// OPT detect
#define SIN_OPT1_CF		(0x0040 << SIN2_CF_SHL)
#define SIN_OPT2_CF		(0x0020 << SIN2_CF_SHL)
#define SIN_OPT3_CF		(0x0010 << SIN2_CF_SHL)

// SIN_ADDR = 3 (PAGING)
#define SIN_AB			0x1000			// a/b
#define SIN_CODE_MASK	0x0FFF			// code mask[11:0]
#define	SIN_CODE_CF		0x80000			// code change flag
#define SIN_AB_CF		0x40000			// a/b change flag
//#define	CHANGE3_ALIGN 11				// right shift of AB and CHANGE to move into change flags word
#define SIN3_DATA		(SIN_AB|SIN_CODE_MASK)	// data mask

// sin algorithm flags
#define	START_BIT		0x00			// sin timer start bit delay mode
#define	EOT_WAIT		0x01			// sin timer EOT wait mode
// sin errors
#define	NORM_SSI_STAT	0x08
#define	SIN_BUFOVR		0x01
#define	SIN_SSIERR		0x02
//-----------------------------------------------------------------------------
// Global Fns
//-----------------------------------------------------------------------------

U32 init_sio(U32 sys_clk, U8 flush);
void send_so(uint64_t data);

U16 get_sin(void);
char got_sin(void);
void flush_sin(void);
U16 get_error(void);
void print_ptr(void);
void gpioq_isr(void);
void Timer2A_ISR(void);
//void SSI3_ISR(void);

#endif /* SPI_H_ */
