/********************************************************************
 ************ COPYRIGHT (c) 2021 by KE0FF, Taylor, TX   *************
 *
 *  File name: lcd.h
 *
 *  Module:    Control
 *
 *  Summary:   defines and global declarations for lcd.c
 *
 *******************************************************************/

#include "typedef.h"
#include <stdint.h>
#include <stdio.h>


//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------
// Processor I/O assignments
// ...see init.h

// ssi2 defines

union LCDREG{
		uint64_t	bigt;		// 64b integer bit field for the LCD segment driver ICs (2 per chip)
		uint8_t		b[8];		// U8 int array for access by SSI processing fns
								//	the 64b int is accessed as 7 bytes (the msB is not used).
								//  ints are lilendian, so i[7] = msB (we will use i[6] thru i[0])
};


// mem scan COS memory defines
#define	MAX_COS_MEM		4		// "last cos" memory

// mode defines
#define	MODE_MASK		0x0f	// main modes mask
#define	FOCUS_MASK		0x01	// focus mask
#define	SUB_MODE		0x00	// "normal" mode, sub focus
#define	MAIN_MODE		0x01	// "normal" mode, main focus
#define	SUB_D			0x02	// disp signal sub flag
#define	SET_MODE		0x04	// set/config loop mode
//#define	???		0x08	// spare
#define	VMODE_MASK		0xf0	// "normal" vfo display mode mask
#define	VMODE_TSDISP	0x10	// display TS adj
#define	VMODE_TDISP		0x20	// display PL tone
#define	VMODE_ISTX		0x40	// tx mode
#define	VMODE_ODISP		0x80	// display offset

// process_VFODISP defines
#define	TONE_DISP		0x01
#define	TS_DISP			0x02
#define	NORM_DISP		0x03
#define	TXTSLID_DISP	0x04

//#define VFOTR_IS_TX		0x40	// flag to signal fetch of vfotr to display TX frequency from uxpll update
//#define VFO_DISP_F		0x80	// flag to signal vfo display update

// Xq-flags
#define	VOL_XFLAG		0x01		// VOL display/edit active
#define	SQU_XFLAG		0x02		// SQU display/edit active
#define	TONE_XFLAG		0x04		// tone display/edit active
#define	OFFS_XFLAG		0x08		// offset display/edit active
#define	TEXTM_SLIDE		0x10		// text slider display active flags (M/S)
#define	TEXTS_SLIDE		0x20
#define	MSCANM_XFLAG	0x40		// mem scan active flags (M/S)
#define	MSCANS_XFLAG	0x80

// Xz-flags
#define	MSCANM1_XFLAG	0x40		// mem scan first active flags (M/S)
#define	MSCANS1_XFLAG	0x80

// XMODE flags
#define	MEM_XFLAG		0x10
#define	CALL_XFLAG		0x20
#define	MC_XFLAG		(MEM_XFLAG|CALL_XFLAG)

// chkmode flags
#define	REV_FLAG		0x01		// reverse mode enabled
#define	REV_SQU_FLAG	0x02		// check mode enabled

// smute flags
#define	MS_MUTE			0x01		// both bands band mute
#define	SUB_MUTE		0x02		// sub band mute

// sys_err (freq display Fn error flags)
#define	NO_MUX_PRSNT	0x01		// no main band present
#define	NO_SUX_PRSNT	0x02		// no sub band present
#define	NO_B_PRSNT		0x04		// no base present

// mfreq/sfreq blink flags
#define	LEAD0		0xff
#define	MAIN_CS		0x80			// sets main lcd chip in digblink()
#define	ALL_DIGS	0x40			// digblink - triggers "ALL DIGITS" to blink/unblink
#define	MAIN_CS_MASK (MAIN_CS|ALL_DIGS)

#define	MHZ_ONE		0x80
#define	MHZ_OFFS	0x40
#define	MHZ_OFF		0x20
#define	MHZ_MASK	0x1f
/*
// LCD chip command masks
#define	CS2_MASK	0x80
#define	CS1_MASK	0x40
#define	DA_CM_MASK	0x20
#define	LEN_MASK	0x1f

// LCD chip commands
#define	MODE_SET	0x49			// /3 time-div, 1/3 bias, 2E-8 fdiv
#define	BLINK_SLOW	0x1A			// low-bit is flash-rate
#define	BLINK_FAST	0x1B			//  "   " ...
#define	BLINK_OFF	0x18
#define	DISP_ON		0x11
#define	DISP_OFF	0x10
#define	WITH_DECODE	0x15
#define	WITHOUT_DECODE	0x14
#define	LOAD_PTR	0xE0			// OR with (0x1f masked address)
#define	WR_DMEM		0xD0			// OR with (0x0f masked data)
#define	OR_DMEM		0xB0			// OR with (0x0f masked data)
#define	AND_DMEM	0x90			// OR with (0x0f masked data)
#define	CLR_DMEM	0x20
#define	WR_BMEM		0xC0			// OR with (0x0f masked data)
#define	OR_BMEM		0xA0			// OR with (0x0f masked data)
#define	AND_BMEM	0x80			// OR with (0x0f masked data)
#define	CLR_BMEM	0x00

#define	MAX_SRF		7				// #srf bargraph segments
#define	MSMET_ADDR	0x06
#define	SSMET_ADDR	0x1b

// LCD segment bit defines (addr and segment map)
//	CS1
#define	MMEM_ADDR		0x01
#define	MDUP		0x1
#define	MDUP_ADDR		0x04
#define	MMIN		0x2
#define	MMIN_ADDR		0x04
//#define	MSKP		0x4
#define	MSKP_ADDR		0x04
#define	MTNE		0x1
#define	MTNE_ADDR		0x05
#define	MM			0x2
#define	MM_ADDR			0x05

#define	MSRF6		0x4
#define	MSRF6_ADDR		0x06
#define	MSRF3		0x1
#define	MSRF4		0x2
#define	MSRF5		0x4
#define	MSRF345_ADDR	0x07
#define	MSRF0		0x1
#define	MSRF1		0x2
#define	MSRF2		0x4
#define	MSRF012_ADDR	0x08
#define	M00			0x2
#define	M00_ADDR	0x09
#define	M0_ADDR		0x0a

#define	AOW			0x1
#define	AOW_ADDR		0x1d
#define	ALOW		0x2
#define	ALOW_ADDR		0x1d

#define	ARIT		0x1
#define	ARIT_ADDR		0x1f
#define	AVXO		0x2
#define	AVXO_ADDR		0x1f
#define	ATS			0x4
#define	ATS_ADDR		0x1f

//	CS2
#define	SMEM_ADDR	0x02
#define	SDUP		0x1
#define	SDUP_ADDR	0x05
#define	SMIN		0x2
#define	SMIN_ADDR	0x05
//#define	SSKP		0x4
#define	SSKP_ADDR	0x05
#define	STNE		0x1
#define	STNE_ADDR	0x06
#define	SM			0x2
#define	SM_ADDR		0x06
#define	S00			0x2
#define	S00_ADDR	0x07

#define	S0_ADDR		0x08
#define	S6			0x1
#define	S6_ADDR		0x1a

#define	SSRF6		0x4
#define	SSRF6_ADDR		0x1b
#define	SSRF3		0x1
#define	SSRF4		0x2
#define	SSRF5		0x4
#define	SSRF345_ADDR	0x1c
#define	SSRF0		0x1
#define	SSRF1		0x2
#define	SSRF2		0x4
#define	SSRF012_ADDR	0x1d

#define	ASUB		0x1
#define	ASUB_ADDR	0x1e
#define	ALCK		0x2
#define	ALCK_ADDR	0x1e

#define	APRG		0x1
#define	APRG_ADDR	0x1f
#define	AMHZ		0x2
#define	AMHZ_ADDR	0x1f
#define	ABND		0x4
#define	ABND_ADDR	0x1f*/


#define	MAX_SRF		7				// #srf bargraph segments
#define	MSMET_ADDR	0x06
#define	SSMET_ADDR	0x1b

//ic901
#define	DP1			0x1
#define	DP2			0x2
#define	ASC278M 	0x7f
#define	ASC278S 	0xfd
#define	MGHZ_WORD	1							// GHz segments
#define	MGHZ_POS	6
#define	SGHZ_WORD	0
#define	SGHZ_POS	52
#define	M50HZ_WORD	0							// 50Hz segments
#define	M50HZ_POS	15
#define	S50HZ_WORD	3
#define	S50HZ_POS	45

#define	MCH_WORD	2
#define	MCH_BPOS	36
#define	MCH10		((uint64_t)1<<34)

#define	SCH_WORD	3
#define	SCH_BPOS	20
#define	SCH10		((uint64_t)1<<28)

#define	MS_WORD		0							// word index
#define	MS_POS		20							// main smet bit position
#define	MS1			(0x01)						// S1
#define	MS3			(0x02 | MS1)
#define	MS5			(0x04 | MS3)
#define	MS7			(0x08 | MS5)
#define	MS9			(0x10 | MS7)				// S9
#define	MS10		(0x20 | MS9)				// S9+10
#define	MS20		(0x40 | MS10)				// S9+20
#define	MS_MASK		0x7f

#define	SS_WORD		3							// word index
#define	SS_POS		34							// sub smet bit position
#define	SS1			(0x40)						// S1
#define	SS3			(0x80 | SS1)
#define	SS5			(0x10 | SS3)
#define	SS7			(0x20 | SS5)
#define	SS9			(0x04 | SS7)				// S9
#define	SS10		(0x08 | SS9)				// S9+10
#define	SS20		(0x01 | SS10)				// S9+20
#define	SS_MASK		0xfd

#define	SQL_WORD	2							// word index
#define	SQL_POS		46							// sql tape bit position
#define	SQL1		(0x001)						// 1 bar setting
#define	SQL2		(0x002 | SQL1)				// 2 bar setting
#define	SQL3		(0x004 | SQL2)				// 3 bar setting
#define	SQL4		(0x008 | SQL3)				// 4 bar setting
#define	SQL5		(0x010 | SQL4)				// 5 bar setting
#define	SQL6		(0x020 | SQL5)				// 6 bar setting
#define	SQL7		(0x040 | SQL6)				// 7 bar setting
#define	SQL8		(0x080 | SQL7)				// 8 bar setting
#define	SQL9		(0x100 | SQL8)				// 9 bar setting
#define	SQL_MASK	0x1ff

#define	SQLD_WORD	2
#define	SQLD		((uint64_t)1<<55)
#define	SQLD_MASK	SQLD

#define	VOLD_WORD	3
#define	VOLD		((uint64_t)1<<11)
#define	VOLD_MASK	VOLD

#define	VOL_WORD	3							// word index
#define	VOL_POS		8							// vol tape bit position
#define	VOL1		(0x010)						// 1 bar setting
#define	VOL2		(0x020 | VOL1)				// 2 bar setting
#define	VOL3		(0x040 | VOL2)				// 3 bar setting
#define	VOL4		(0x080 | VOL3)				// 4 bar setting
#define	VOL5		(0x100 | VOL4)				// 5 bar setting
#define	VOL6		(0x200 | VOL5)				// 6 bar setting
#define	VOL7		(0x001 | VOL6)				// 7 bar setting
#define	VOL8		(0x002 | VOL7)				// 8 bar setting
#define	VOL9		(0x004 | VOL8)				// 9 bar setting
#define	VOL_MASK	0x2f7

#define	MMODE_WORD	1
#define	MMODE_FM	((uint64_t)1<<15)
#define	MMODE_USB	((uint64_t)1<<23)
#define	MMODE_LSB	((uint64_t)1<<33)
#define	MMODE_CW	((uint64_t)1<<49)
#define	MMODE_MASK	(MMODE_FM|MMODE_USB|MMODE_LSB|MMODE_CW)

#define	SMODE_WORD_FM	0
#define	SMODE_WORD_USB	0
#define	SMODE_WORD_LSB	0
#define	SMODE_WORD_CW	2
#define	SMODE_WORD_AM	3

#define	SMODE_FM	((uint64_t)1<<53)
#define	SMODE_USB	((uint64_t)1<<45)
#define	SMODE_LSB	((uint64_t)1<<29)
#define	SMODE_CW	((uint64_t)1<<19)
#define	SMODE_AM	((uint64_t)1<<55)
#define	SMODE_MASK0	(SMODE_FM|SMODE_USB|SMODE_LSB)
#define	SMODE_MASK2	(SMODE_CW)
#define	SMODE_MASK3	(SMODE_AM)

#define	MDUP_WORD	0
#define	MDUPP		((uint64_t)1<<17)
#define	MDUPM		((uint64_t)1<<18)
#define	MDUP_MASK	(MDUPP|MDUPM)

#define	SDUP_WORD	3
#define	SDUPP		((uint64_t)1<<43)
#define	SDUPM		((uint64_t)1<<32)
#define	SDUP_MASK	(SDUPP|SDUPM)

#define	MBELL_WORD	0
#define	MBELL		((uint64_t)1<<5)
#define	MBELL_MASK	MBELL

#define	SBELL_WORD	0
#define	SBELL		((uint64_t)1<<55)
#define	SBELL_MASK	SBELL

#define	MLOW_WORD	0
#define	MLOW		((uint64_t)1<<16)
#define	MLOW_MASK	MLOW

#define	SRCV_WORD	3
#define	SRCV		((uint64_t)1<<42)
#define	SRCV_MASK	SRCV

#define	MTENC_WORD	0
#define	MTENC		((uint64_t)1<<19)
#define	MTENC_MASK	MTENC

#define	STENC_WORD	3
#define	STENC		((uint64_t)1<<35)
#define	STENC_MASK	STENC

#define	MTSQL_WORD	0
#define	MTSQL		((uint64_t)1<<27)
#define	MTSQL_MASK	MTSQL

#define	STSQL_WORD	3
#define	STSQL		((uint64_t)1<<33)
#define	STSQL_MASK	STSQL

#define	MFAST_WORD	2
#define	MFAST		((uint64_t)1<<35)
#define	MFAST_MASK	MFAST

#define	SFAST_WORD	3
#define	SFAST		((uint64_t)1<<29)
#define	SFAST_MASK	SFAST

#define	MMEM_WORD	2
#define	MMEM		((uint64_t)1<<31)
#define	MMEM_MASK	MMEM

#define	SMEM_WORD	3
#define	SMEM		((uint64_t)1<<31)
#define	SMEM_MASK	SMEM

#define	MSKP_WORD	2
#define	MSKP		((uint64_t)1<<30)
#define	MSKP_MASK	MSKP

#define	SSKP_WORD	3
#define	SSKP		((uint64_t)1<<30)
#define	SSKP_MASK	SSKP

#define	TSD_WORD	2
#define	TSD			((uint64_t)1<<44)
#define	TSD_MASK	TSD

#define	SATD_WORD	0
#define	SATD		((uint64_t)1<<54)
#define	SATD_MASK	SATD

#define	MHZD_WORD	2
#define	MHZD		((uint64_t)1<<45)
#define	MHZD_MASK	MHZD

#define	BANDD_WORD	2
#define	BANDD		((uint64_t)1<<43)
#define	BANDD_MASK	BANDD

#define	LOCKD_WORD	3
#define	LOCKD		((uint64_t)1<<7)
#define	LOCKD_MASK	LOCKD

#define	PROGD_WORD	3
#define	PROGD		((uint64_t)1<<6)
#define	PROGD_MASK	PROGD

#define	SUBD_WORD	1
#define	SUBD		((uint64_t)1<<7)
#define	SUBD_MASK	SUBD

#define	OPTD_WORD	0
#define	OPTD		((uint64_t)1<<37)
#define	OPTD_MASK	OPTD

#define	OPTM_WORD	1
#define	OPTM		((uint64_t)1<<31)
#define	OPTM_MASK	OPTM

#define	OPTS_WORD	1
#define	OPTS		((uint64_t)1<<41)
#define	OPTS_MASK	OPTS

#define	NBD_WORD	2
#define	NBD			((uint64_t)1<<11)
#define	NBD_MASK	NBD

#define	NBM_WORD	2
#define	NBM			((uint64_t)1<<27)
#define	NBM_MASK	NBM

#define	NBS_WORD	3
#define	NBS			((uint64_t)1<<53)
#define	NBS_MASK	NBS

#define	RITD_WORD	2
#define	RITD		((uint64_t)1<<29)
#define	RITD_MASK	RITD

#define	RITM_WORD	2
#define	RITM		((uint64_t)1<<28)
#define	RITM_MASK	RITM

#define	RITS_WORD	2
#define	RITS		((uint64_t)1<<33)
#define	RITS_MASK	RITS

#define	RFGD_WORD	3
#define	RFGD		((uint64_t)1<<19)
#define	RFGD_MASK	NBD

#define	RFGM_WORD	3
#define	RFGM		((uint64_t)1<<18)
#define	RFGM_MASK	RFGM

#define	RFGS_WORD	3
#define	RFGS		((uint64_t)1<<21)
#define	RFGS_MASK	RFGS

#define	VXOD_WORD	2
#define	VXOD		((uint64_t)1<<32)
#define	VXOD_MASK	NBD



//-----------------------------------------------------------------------------
// Global Fns
//-----------------------------------------------------------------------------

void init_lcd(void);
void reset_lcd(void);
void process_UI(U8 cmd);
void digblink(U8 digaddr, U8 tf);
void mfreq(U32 dfreq, U8 blink);
void sfreq(U32 dfreq, U8 blink);
void msmet(U8 srf, U8 blink);
void ssmet(U8 srf, U8 blink);
U8 mputs_lcd(char *s, U8 dp_tf);
U8 sputs_lcd(char *s, U8 dp_tf);
void mmem(U8 mn);
void smem(U8 mn);
void mtonea(U8 tf);
void mema(U8 focus, U8 tf);
void stonea(U8 tf);
void mskpa(U8 focus, U8 tf);
void mdupa(char dplx);
void sdupa(char dplx);
void mdupa_blink(U8 tf);
void sdupa_blink(U8 tf);
void alow(U8 tf);
void amhz(U8 tf);
void abnd(U8 tf);
void asub(U8 tf);

void amtx(U8 tf);
void amrx(U8 tf);
void asrx(U8 tf);
void mchan_lcd(char *s);
void schan_lcd(char *s);

void bin32_bcds(U32 bin32, U8* sptr);
U32 bcds_bin32(U8* sptr);
void add_bcds(U8* sptr, S8 adder, U8 addr, U8 max, U8 min);
void set_vfo_display(U8	sig);
void rev_vfo(U8	focus);
void force_push(void);
U8 get_mutefl(void);
U8 get_xmode(U8 b_id);
void write_xmode(U8 main);
void read_xmode(void);
void clear_xmode(void);
void set_sys_err(U8 err);
void clr_sys_err(U8 err);
U8 puts_slide(U8 focus, char* sptr, U8 cmd);
U8 doscan(U8 focus, U8 tf);
U8 get_xmodeq(void);
void set_ptt(U8 pttstat);
void set_slide(U8 focus, U8 tf);
U8 mem2ordinal(char cm);
char ordinal2mem(U8 memnum);
U8 lookup_pl(U16 ctcss);
