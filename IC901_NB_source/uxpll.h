/********************************************************************
 ************ COPYRIGHT (c) 2021 by KE0FF, Taylor, TX   *************
 *
 *  File name: uxpll.h
 *
 *  Module:    Control
 *
 *  Summary:   defines and global declarations for radio.c
 *
 *******************************************************************/

#include "typedef.h"
#include <stdint.h>


// PLL calculation defines
// UX band module defines
#define BANDOFF		0x00				// base reset address
// ordinal ID numbers
#define ID10M		0x01				// UX-29
#define ID6M		0x02				// UX-59
#define ID2M		0x03				// UX-29 (not UX, remaps to IC901 base)
#define ID220		0x04				// UX-39
#define ID440		0x05				// UX-49 (not UX, remaps to IC901 base)
#define ID1200		0x06				// UX-129
#define IDWBRX		0x07				// R91 WBRX
#define ID2MSSB		0x08				// S92 2m SSB
#define ID2MSSBD	0x09				// 2mSSB "dials"
#define IDBUNIT		0x0a				// vol/squ/tone
#define	IDPAGE		0x0b				// paging
#define IDMAX		ID2MSSB				// max number of radio modules

#define B6_STUFF	0xffffffdfL			// bit 6 stuffing mask
#define	BITREV18	0x20000L			// 18 bit wide bit reverse start

#define	MAX_BAND		(IDWBRX + 2)	// top index points to "band-err" locations
#define	MERR_BAND		ID2M	//(MAX_BAND-1)
#define	SERR_BAND		ID440	//(MAX_BAND)
#define BAND_ERROR		(0xf0)
#define BAND_ERROR_F	(0xff)
#define	NUM_VFOS		(IDMAX * 2)		// 2 for each UX band
#define ID10M_IDX		0x00			// ordinal index values
#define ID6M_IDX		0x01
#define ID2M_IDX		0x02
#define ID220_IDX		0x03
#define ID440_IDX		0x04
#define ID1200_IDX		0x05
#define IDR91_IDX		0x06
#define IDS92_IDX		0x07

#define IDMAX_IDX		0x07

#define ID10M_B			0x01			// bitmapped ID numbers
#define ID6M_B			0x02
#define ID2M_B			0x04			// UX-29, ID reused here for IC901
#define ID220_B			0x08
#define ID440_B			0x10			// UX-49, ID reused here for IC901
#define ID1200_B		0x20
#define IDR91_B			0x40			// UX-R91
#define IDS92_B			0x80			// UX-S92


#define IDALL_B		(ID10M_B | ID6M_B | ID2M_B | ID220_B | ID440_B | ID1200_B | IDR91_B | IDS92_B)

// Module bitmap bases
//0x000000001F	// Base reset trigger
//0x730000001F	// Base peripheral clear
#define UX19_BASE	0x090000001FLL	// UX-19
#define UX59_BASE	0x120000001FLL	// UX-59
//0x1B0000001F	// <UX-29, not accessed by the IC-901 controller>
#define UX39_BASE	0x240000001FLL	// UX-39
#define UX129_BASE	0x360000001FLL	// UX-129
#define VHF_BASE	0x380000001FLL	// 2m (base)
#define UHF_BASE	0x400000001FLL	// 440 (base)
//0x480000001F	// <unknown, possibly 430 SSB>
#define UX92_BASEW	0x500000001FLL	// UX-S92
//0x580000001F	// <unknown, possibly 1200 SSB>
#define UX91_BASEH	0x6000000000000000LL	// UX-R91 upper
#define UX91_BASEL	0x007FLL			// UX-R91 lower

// init PLL frames
#define INIT_UX19	0x090000001FLL	// UX-19
#define INIT_UX59	0x120000001FLL	// UX-59
#define INIT_UX39	0x240000001FLL	// UX-39
//#define UX129	0x360000001FLL	// UX-129
#define INIT_VHF	0x380002803FLL	// 2m (base)
#define INIT_UHFP	0x400000001FLL	// 440 PLL (base)!!!
#define INIT_UHFR	0x4000000A3FLL	// 440 REF (base)!!!
#define INIT_UHFH	0x400000001FLL	// 440 HL (base)!!!
#define INIT_UHFG	0x400000007FLL	// 440 GPIO (base)!!!
//#define UX92	// UX-S92
//#define UX91	// UX-R91 upper

// RX base freqs (KHz)
#define BASE_RX_10M		28000000L
#define BASE_RX_6M		40000000L
#define BASE_RX_2M		136000000L			// bit7 skip
#define BASE_RX_220		220000000L			// bit7 skip
#define BASE_RX_440		400000000L
#define BASE_RX_1200	1200000000L			// bit7 skip
#define BASE_RX_R91		400000L				// TBD
#define BASE_RX_S92		140000000L			// TBD
// TX offset freqs (5KHz channels)
#define BASE_TX_10M		(10695L/5L)			// (-)
#define BASE_TX_6M		(13990L/5L)			// (-)
#define BASE_TX_2M		(17200L/5L)			// (+)
#define BASE_TX_220		(17200L/5L)			// (+)
#define BASE_TX_440		(0x181FL)			// (+)
#define BASE_TX_1200	(136600L/10L)		// (+) 1.2G is 10KHz channel
#define BASE_TX_S92		(0L)				// (*) ssb
// PLL base bitmaps
#define INIT_PLL_6M		0x01325L			// PLL init pattern for 6M & 10M
#define INIT_PLL_10M	0x01325L
#define PLL_10M			0x01e3bL			// must <<1 after adding freq
#define PLL_6M			0x02a2eL			// must <<1 after adding freq
#define PLL_2M			0x05CD0L
#define PLL_220			0x09e70L
#define PLL_440			0x12061L
//#define	PLL_BITLEN		0x14000000L		// PLL xfer bit length
//#define	PLL_1200BITLEN	0x04000000L		// 1200 init frames PLL xfer bit length
// the TC9181 PLL IC takes LSB 1st, which requires all of the PLL streams to be bit reversed and MSB aligned
// prior to transmission with the msb-first drivers.
#define INIT_PLL_1200	0x360000001FLL		// PLL blank pattern for 1200 (16 bits, bit reversed sets ref osc)
#define INIT_REF_1200	0x360000143FLL		// ref init pattern for 1200 (16 bits, bit reversed sets ref osc)
#define INIT_HL_1200	0x360000001FLL		// "HL" config
#define INIT_GPR_1200	0x360000007FLL		// "GPIO" preset config
#define INIT_GPI_1200	0x36000001FFLL		// "GPIO" idle
#define INIT_GPU_1200	0x36000000FFLL		// "GPIO" up
#define INIT_GPD_1200	0x360000017FLL		// "GPIO" dn

//#define INIT_PLL_1202	0x00000003L		// "GPIO" reset config, 4 bits only
//#define INIT_PLL_1203	0x0000000fL		// "GPIO" config, 4 bits only
#define PLL_1200_N		0x00040000L			// "code" bits for N frames
#define PLL_1200_MASK	0x0003ffffL			// mask for N frames
#define PLL_1200		0x19f64L			// must bit reverse
#define UX_XIT_MASK		0xf0000000000LL		// XIT code mask
#define	UX_XIT			0x80000000000LL		// XIT code
#define	UX_XIT_UP		0x00000000010LL		// XIT up code
#define	UX_XIT_COUNT	0x0000000000fLL		// XIT count mask
#define	UX_XIT_CKUP		0x00000000007LL		// XIT clk up code (1st half-clk)
#define	UX_XIT_CKDN		0x0000000000bLL		// XIT clk dn code (1st half-clk)
#define	UX_XIT_CK0		0x0000000000fLL		// XIT, 2nd half-clk
#define	REG_XIT_UP		0x10				// XIT/RIT up bit
#define	REG_XIT_CNT		0x0f				// count value

#define	UX_MUTE			0xff000fffffffe000LL		// UX module mute semaphore

#define TX_IDX			5
#define	DATA_ARRAY_LEN	11
#define	BND_IDX			10

//#define	BND_IDX8		40
#define BNDID_MASK8		0xf0
#define BND_MASK		0xffffff00
#define BNDID_MASK		0xfffffff0
#define BNDID_FMASK		0x0000000f
#define	BNDID_HIPWR		0x00000020
#define	BNDID_WBIF		0x00000010
#define	BNDID_SOVRD		0x00000040
#define	BNDID_Q			0x00000080
#define BND_SHIFT		0

// SOUT defines
#define	TONE_NUL		0x74C000181FLL				// tone nul msg, after each VFO update
#define	ADDR_VQT		0x700000001FLL				// vol, squ, tone addr
#define	OPT12_SOUT		0x0400000000LL				// Selects [OPT1 = main, OPT2 = sub] if "1", else [OPT1 = sub, OPT2 = main] if "0"
#define	VOL_SOUT		0x0200000000LL				// if "1", vol level
#define	SQU_SOUT		0x0100000000LL				// if "1", SQU level
#define	TONE_SOUT		0x0080000000LL				// if "1", tone code
#define	XFR12_SOUT		0x0040000000LL				// "1" = OPT1 is serial xfr dest, else OPT2
#define	AFM1_SOUT		0x0020000000LL				// AFM main
#define	AFM2_SOUT		0x0010000000LL				// AFM sub
#define	RPTXB_SOUT		0x0008000000LL				// xband rpt
#define	MAINLEV_SOUT	0x0000000200LL				// main level \___ can't be "11"
#define	SUBLEV_SOUT		0x0000000100LL				// sub level  /

#define	ATTEN_FINE_LIM		5
#define	ATTEN_FINE			0x000010
#define	ATTEN_COARSE_LIM	7
#define	ATTEN_COARSE		0x000800
#define	LEV_SHFT		11							// #bits to left shift ATTEN data to align
#define	LEVEL_MAX 		((ATTEN_FINE_LIM * ATTEN_COARSE_LIM) - 1)
#define	ATTEN_MAIN		0x00000200					// main select
#define	ATTEN_SUB		0x00000100					// sub select

#define	VOL_ADDR		(ADDR_VQT|VOL_SOUT)			// sout addr for vol
#define	SQU_ADDR		(ADDR_VQT|SQU_SOUT)			// sout addr for squ
#define	TONE_ADDR		(ADDR_VQT|TONE_SOUT)		// sout addr for tone enc
#define	TONE_SHFT		5							// #bits to left shift tone data to align

///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
// external function prototypes
//-----------------------------------------------------------------------------

