/********************************************************************
 ************ COPYRIGHT (c) 2021 by ke0ff, Taylor, TX   *************
 *
 *  File name: serial.c
 *
 *  Module:    Control
 *
 *  Summary:   This is the LCD driver source file for the IC-900 LCD
 *  			Functions pertaining to the IC-900 Remote Controller LCD
 *  			interface are the focus of this source file.
 *
 *******************************************************************/

/********************************************************************
 *  File scope declarations revision history:
 *    08-08-23 jmh:  Coded/debugged voltape() and squtape().  Re-enabled LCD blink timer and moved to Timer3B.
 *    08-06-23 jmh:  Adapted from RDU-900 clone project.  Modified to produce serial output for the LC7582A LCD driver ICs
 *    				 that are present in the IC-901 controller.  A periodic timer ISR handles blinking.  The SW has to
 *    				 maintain the segment memory and a blink array that is ANDed with the segment memory to establish which
 *    				 segments will be turned off on even blink cycles.  SSI2 and Timer2B (both in ssi2.c) drive the SPI
 *    				 and timing interface.
 *    				 The LCD Fns in this file were modified to use the new, direct-segment architecture employed by the
 *    				 LC7582A which differs from the command/argument model used on the IC900 hardware.  The main difference
 *    				 lies in the lack of a BCD mode in the driver IC.  Since there was already a text2segment path coded,
 *    				 this was then employed to send all messages to the 7-seg areas of the LCD (including numbers-only msgs).
 *    				 Modified SMETER to display all 15 bits of SRF data by negating the lower segments of the bar-graph
 *    				 for levels above "7".  When the 3 top segments are all that is lit, the level is "0xF".
 *    06-19-21 jmh:  creation date
 *
 *******************************************************************/

#include <stdint.h>
#include <stdio.h>
#include "inc/tm4c1294ncpdt.h"
#include "typedef.h"
#include "init.h"
#include "stdio.h"
#include "serial.h"
#include "spi.h"
#include "cmd_fn.h"
#include "lcd.h"
#include "radio.h"
#include "uxpll.h"
#include "sio.h"
#include "spi.h"
#include "nvic.h"
#include "ssi2.h"

//------------------------------------------------------------------------------
// Define Statements
//------------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Local Variable Declarations
//-----------------------------------------------------------------------------
// Hard-coded LCD comm messages:
/*U8	lcd_init_1[] = { 0x45, 0x49, CLR_DMEM, CLR_BMEM, DISP_ON, BLINK_FAST };	// CS1 chip init
U8	lcd_init_2[] = { 0x85, 0x49, CLR_DMEM, CLR_BMEM, DISP_ON, BLINK_FAST};	// CS2 chip init
U8	lcd_mfreq_1[] = { 0x43, 0xe9, OR_DMEM, WITH_DECODE };					// set pointer, "5"00 hz = no change, WITH seg decoder
U8	lcd_mfreq_2[] = { 0x41, 0xd4 };											// MDP
U8	lcd_mfreq_3[] = { 0x41, 0xd0 };											// no MDP
U8	lcd_mraw[] = { CS1_MASK | 3, LOAD_PTR | 9, OR_DMEM, WITHOUT_DECODE };	// set pointer, "5"00 hz = no change, WITHOUT seg decoder
U8	lcd_sfreq_1[] = { 0x83, 0xe7, OR_DMEM, WITH_DECODE };					// set pointer, "5"00 hz = no change, WITH seg decoder
U8	lcd_sfreq_2[] = { 0x81, 0xd4 };											// SDP
U8	lcd_sfreq_3[] = { 0x81, 0xd0 };											// no MDP
U8	lcd_sraw[] = { CS2_MASK | 3, LOAD_PTR | 7, OR_DMEM, WITHOUT_DECODE };	// set pointer, "5"00 hz = no change, WITHOUT seg decoder*/
// VOL/SQU low-order ordinal sequence.  A list of segment patterns for the MEM digit during VOL/SQU adjust
U8	lcd_qv_lsd[5][3] = {													// list of 5 7-seg ordinal patterns
					// [ordinal][seg addr]
					{ 0, 4, 0 },
					{ 0, 6, 0 },
					{ 0, 6, 1 },
					{ 0, 7, 1 },
					{ 1, 7, 1 }
};
// TS ordinal sequence.  A list of step sizes [B:A]
U8	ts_order[3] = { 0x21, 0x51, 0x52 };

// List of supported CTCSS tones, in Hz * 10.  1st tone is #1, last is #38:
//					  00	01    02    03    04    05    06    07
U16	tone_list[] = {  670,  719,  744,  770,  797,  825,  854,  885,
//					  08	09    0a    0b    0c    0d    0e    0f
					 915,  948,  974, 1000, 1035, 1072, 1109, 1148,
//					  10	11    12    13    14    15    16    17
					1188, 1230, 1273, 1318, 1365, 1413, 1462, 1514,
//					  18	19    1a    1b    1c    1d    1e    1f
					1567, 1622, 1679, 1738, 1799, 1862, 1928, 2035,
//					  20	21    22    23    24    25
					2107, 2181, 2257, 2336, 2418, 2503 };

// Mem ordinals         0         0123456789012345678901234
char mem_ordinal[] = { "0123456789ABDEFGHJKLMNPRSTUWYZ()[]" };
uint8_t	smetm_map[] = { 0, 0, MS1, MS3, MS5, MS7, MS9, MS10, MS20, MS20, MS30, MS30, MS40, MS40, MS50, MS60 };
uint8_t	smets_map[] = { 0, 0, SS1, SS3, SS5, SS7, SS9, SS10, SS20, SS20, SS30, SS30, SS40, SS40, SS50, SS60 };
uint8_t	digmap_m[] = { 0x80|8, 0x80|16, 0x80|24, 0x80|34, 0x80|42, 0x80|50, 8 };
uint8_t	digmap_s[] = { 44, 36, 28, 0x80|18, 0x80|10, 0x80|2, 0x40|44 };

uint16_t	sql_map[] = { 0, SQL1, SQL2, SQL3, SQL4, SQL5, SQL6, SQL7, SQL8, SQL9 };
#define	SQL_LEN	(sizeof(sql_map)/sizeof(uint16_t))

uint16_t	vol_map[] = { 0, VOL1, VOL2, VOL3, VOL4, VOL5, VOL6, VOL7, VOL8, VOL9 };
#define	VOL_LEN	(sizeof(vol_map)/sizeof(uint16_t))

// 901 LCD maps

U8	maddr;							// mhz digit mode composite digit address and mode flags register
U8	vfo_display;					// display update signal.  This is band (MAIN/SUB) to update or'd with 0x80 to trigger
U8	xmode[ID1200];					// xmode flags track mem/call modes
U8	xmodeq;							// xmodeq tracks transient adjust modes (vol, squ, tone, offs)
U8	xmodez;							// xmodez tracks first-scan enabled flags
U8	chkmode;						// indicates check/rev mode is in effect
U8	chksqu;							// save reg for check squelch
U8	mute_mode;						// main/sub mute status
U8	tsdisplay;						// ts display flag
U8	sys_err;						// system error flags.  0x00 = no errors
U8	ptt_change;						// PTT changed flag (from radio.c)
char mfbuf[16];						// ascii text (freq field) buffer, main
char sfbuf[16];						// ... sub
U32	mscan[ID1200];					// mem scan enable bits (gathered by get_mscan())
U32	mscan_maskm;					// scan enable mask (m/s)
U32	mscan_masks;
U8	scan_switch;					// scan switch event flag
U8	last_cosm[MAX_COS_MEM];			// last cos ban/mem# circ buffers
U8	last_coss[MAX_COS_MEM];			// .. hi-nyb is band, low-nyb is mem#
U8	last_cosm_tp;					// head/tail pointers
U8	last_cosm_hp;
U8	last_coss_tp;
U8	last_coss_hp;
U8	led_status;						// LED on/off status
U8	lock_mode;
U8	sw_stat;						// "sliding switches" memory reg (updated for 901)


//-----------------------------------------------------------------------------
// Local Fn Declarations
//-----------------------------------------------------------------------------
U8	asc27m(char c);
void clear_lcd_buf(void);
void alock(U8 tf);
U32 bin32_bcdp(U32 bin32);
void lamp_test(U8 tf);
void update_lcd(U8 focus, U8 forced_focus);
U8 process_MS(U8 cmd);
void process_VFODISP(U8 focus);
U8 test_for_cancel(U8 key);
U32 process_DIAL(U8 focus);
U8 process_MEM(U8 cmd);
U8 process_SET(U8 cmd);
void ats(U8 tf);
void mset_500hz(U8 cmd);
void sset_500hz(U8 cmd);
void mon_500hz(U8 tf);
void son_500hz(U8 tf);
void mblink_500hz(U8 tf);
void sblink_500hz(U8 tf);
void mmute_action(U8* mute_flag);
void smute_action(U8* mute_flag);
U8 puts_lcd(U8 focus, char *s, U8 dp_tf);
void togg_tsab(U8 focus);
U8 copy6str(char* sptr, char* dptr, U8 cidx);
U8 nxtscan(U8 focus, U8 adder);
U32 mem2bit(U8 memnum);
void get_mscan(U8 focus);
void set_last_cos(U8 focus);
U8 get_last_cos(U8 focus);
void set_ledl(U8 ledid, U8 ledon);

//-----------------------------------------------------------------------------
// init_lcd() initializes lcd resources
//	The LCD chips (uPD7225) use an SPO=1/SPH=1 transfer format (clock idles high,
//	and data changes on clk FE, stable at clk RE).  However, the clock and data
//	are inverted for this design, so we need SPO=0/SPH=1 transfer format.
//
// The max SSI clock rate per the uPD7225 datasheet is about 1.1 MHz.
//-----------------------------------------------------------------------------
void init_lcd(void){
	volatile uint32_t ui32Loop;

	reset_lcd();														// reset the LCD chipset
	wait(2);
	ssi2_init();
	mhz_time(0xff);														// clear MHZ timer
	return;
}

//****************
// reset_lcd performs a hardware LCD reset
void reset_lcd(void)
{
	wait(3);
	GPIO_PORTD_AHB_DATA_R &= ~(LCD_CE1 | LCD_CE2);						// close all SPI /CS
	wait(3);
	GPIO_PORTL_DATA_R |= LCD_RSTN;										// activate reset
	wait(20);
	GPIO_PORTL_DATA_R &= ~LCD_RSTN;										// de-activate reset
	wait(20);
	return;
}

//-----------------------------------------------------------------------------
// process_UI() updates LCD based on SIN change flags
//	processes key inputs, dial changes, and CCMD inputs
//-----------------------------------------------------------------------------
void process_UI(U8 cmd){
//			U8	i;			// temp
			U8	mode_rtn;
	static	U8	mode;		// mode

	//**************************************
	// process IPL (Initial Program Load) init
	if(cmd == 0xff){
		sw_stat = ~GPIO_PORTK_DATA_R & (LOCK);							// force update of slide switch status
		mode = MAIN_MODE;												// init process variables
		chkmode = 0;
		vfo_display = 0;
		update_lcd(MAIN, MAIN);
		update_lcd(MAIN, SUB);
		process_MS(0xff);												// trigger main/sub process IPL init
		is_mic_updn(1, 0, 0);											// init mic u/d repeat
		ptt_change = 0;
		wait(100);
		mute_radio(0);
		sputs_lcd("  UI OK  ", 2);
		lcd_send(1);
	}else{
		//**************************************
		// process the UI for each of the different modes:
		switch(mode){
		default:
			mode = MAIN_MODE;											// fault trap... force mode to MAIN_MODE
		// MAIN/SUB "normal" process
		case MAIN_MODE:
		case SUB_MODE:
 			mode_rtn = process_MS(mode);
			break;

		// SET configuration loop process
		case SET_MODE:
//			mode_rtn = process_SET(mode);
			break;

		// Memory mode process
//		case MEM_MODE:
//			mode_rtn = process_MEM(mode);
//			break;
		}
		//**************************************
		// update the display when there are mode changes
		if(mode_rtn != mode){
			switch(mode_rtn){
			case MAIN_MODE:
//				init_lcd_main();
				break;

			case SUB_MODE:
//				init_lcd_sub();
				break;

			case SET_MODE:
//				init_lcd_set();
				break;

//			case MEM_MODE:
//				init_lcd_mem();
//				break;
			}
			mode = mode_rtn;
		}
		//**************************************
		// process slide switches (DIM and LOCK)
		if(lock_mode != sw_stat){										// if changes..
			if(lock_mode){
				alock(1);												// process lock mode
			}else{
				alock(0);												// process unlock mode
			}
			sw_stat = lock_mode;										// update GPIO memory (used to trap changes)
		}
	}
	return;
}	// end process_UI()

//-----------------------------------------------------------------------------
// update_lcd() forces update of LCD values
//-----------------------------------------------------------------------------
void update_lcd(U8 focus, U8 forced_focus){
	U8	i;

	GPIO_PORTB_AHB_DATA_R |= SPARE_PB0;				// !!! debug GPIO
	//**************************************
	// update low/hi power display icon
	i = get_lohi(focus, 0xff);
	if(i){
		alow(1);
	}else{
		alow(0);
	}
	if(forced_focus == MAIN){
		//**************************************
		// update Duplex display, main
		switch(read_dplx(MAIN) & (DPLX_MASK)){
		default:
		case DPLX_S:
			mdupa('S');
			break;

		case DPLX_P:
			mdupa('+');
			break;

		case DPLX_M:
			mdupa('-');
			break;
		}
		// update flag cluster
		ats(read_dplx(MAIN) & TSA_F);										// TS (f-step) display
		mtonea(adjust_toneon(MAIN, 0xff));									// TONE display
		if((xmode[get_band_index(MAIN)] & MC_XFLAG) == MEM_XFLAG){			// main band...
			mema(MAIN, 1);													// turn on "M"
			mskpa(MAIN, get_scanmem(MAIN));									// update "skp" annunc.
		}else{
			msmet(0xff, 0);													// init SRF/MEM
			mema(MAIN, 0);													// turn off "M"
			mskpa(MAIN, 1);													// "skp" annunc = off
		}
//		mfreq(get_freq(MAIN), 0);											// Frequency display
		if(ptt_change & PTT_KEYED) i = MAIN | VMODE_ISTX;
		else i = MAIN;
		mfreq(get_freq(i), 0);							// update main freq display from vfo or vfotr
	}else{
		//**************************************
		// update Duplex display, sub
		switch(read_dplx(SUB) & (DPLX_MASK)){
		default:
		case DPLX_S:
			sdupa('S');
			break;

		case DPLX_P:
			sdupa('+');
			break;

		case DPLX_M:
			sdupa('-');
			break;
		}
		// update flag cluster
		stonea(adjust_toneon(SUB, 0xff));
		ssmet(0xff, 0);														// init SRF/MEM
		if((xmode[get_band_index(SUB)] & MC_XFLAG) == MEM_XFLAG){			// sub band...
			mema(SUB, 1);													// turn on "M"
			mskpa(SUB, get_scanmem(SUB));									// update "skp" annunc.
		}else{
			mema(SUB, 0);													// turn off "M"
			mskpa(SUB, 1);													// "skp" annunc = off
		}
		if(!(xmodeq & TEXTS_SLIDE)){
			sfreq(get_freq(SUB), 0);
		}
	}
	GPIO_PORTB_AHB_DATA_R &= ~SPARE_PB0;				// !!! debug GPIO
	return;
}	// end update_lcd()

//-----------------------------------------------------------------------------
// process_MS() MAIN/SUB mode: updates LCD based on SIN change flags
//	processes key inputs, dial changes, and CCMD inputs
//	returns mode changes
//-----------------------------------------------------------------------------
U8 process_MS(U8 mode){
	static	 U32	iflags;
	static	 U8		hflag;
	U8	b;					// key beep counter
	U8	i;					// temp
//	U8	j;					// temp
	U8	k;					// temp
	U32	ii;
	volatile U32	sin_a1;
	volatile U32	sin_a2;
	U8	band_focus = mode;	// band focus of keys/dial
	S32	sii;

//	char dgbuf[30];	// !!!!debug

	GPIO_PORTB_AHB_DATA_R |= SPARE_PB1;				// !!! debug GPIO
	//**************************************
	// process IPL init
	if(mode == 0xff){
		lock_mode = 0xff;												// init lock "switch"
		maddr = MHZ_OFF;												// MHz/thumbwheel mode init
		xmodeq = 0;														// x-modes
		mute_mode = 0;													// smute = off
		iflags = 0;														// SIN change flags storage init
		hflag = 0;														// hold key flag
		tsdisplay = 0;													// clear TS adj display mode
		vfo_display = MAIN|SUB_D;										// force update of main/sub freq
	}else{
		//**************************************
		// process SIN changes
		k = get_band_index(band_focus);
		iflags = read_sin_flags(CLEARALL_FLAGS);							// merge with radio.c variable
		if(iflags){
			// got changes...
			if(ptt_change & PTT_EDGE){
				iflags |= SIN_SRFM_CF;									// force update of MSRF
				ptt_change &= ~PTT_EDGE;
//				read_sin_flags(SIN_SEND_CF);							// clear changes flag
			}
			sin_a1 = fetch_sin(1);										// update data
			// check if squelch adjust
//			if(!(xmodeq & SQU_XFLAG)){
				if(iflags & SIN_SRFM_CF){
					// update SRF
					ii = sin_a1 >> SIN1_SRF_MAIN_B;						// isolate main SRF
					i = (U8)(ii & 0x0f);
					msmet(i, 0);										// update glass
					read_sin_flags(SIN_SRFM_CF);						// clear changes flag
					if(xmode[get_band_index(MAIN)] & CALL_XFLAG){
						mmem(get_callnum(MAIN, 0));						// update call#
					}else{
						mmem(get_memnum(MAIN, 0));						// update mem#
					}
				}
//			}
			// check if vol adjust
//			if(!(xmodeq & VOL_XFLAG)){
				if(iflags & SIN_SRFS_CF){
					// update SRF
					ii = sin_a1 >> SIN1_SRF_SUB_B;						// isolate sub SRF
					i = (U8)(ii & 0x0f);
					ssmet(i, 0);										// update glass
					read_sin_flags(SIN_SRFS_CF);						// clear changes flag
					if(xmode[get_band_index(SUB)] & CALL_XFLAG){
						smem(get_callnum(SUB, 0));						// update call#
					}else{
						smem(get_memnum(SUB, 0));						// update mem#
					}
				}
//			}
			if(iflags & (SIN_SQSM_CF|SIN_SQSS_CF)){						// LED updates (MRX, MTX, SRX)
				// update RX LEDs
				if((sin_a1 & SIN_SQSA) && !(sys_err & (NO_B_PRSNT|NO_MUX_PRSNT))){
					set_ledl(LED_RXM, 1);								// main led = on
					scan_time(MAIN, 2);									// reset scan timer
				}else{
					set_ledl(LED_RXM, 0);								// main led = off
				}
				if(sin_a1 & SIN_SQSB && !(sys_err & (NO_B_PRSNT|NO_SUX_PRSNT))){
					// sub led
					set_ledl(LED_RXS, 1);								// sub led = on
					if(scan_switch){
						if(!scan_time(SUB, 0)){
//							scan_time(SUB, 1);							// reset scan timer after band edge (don't advance mem#)
							scan_switch = 0;							// disable edge flag
						}
					}else{
						scan_time(SUB, 2);								// reset scan timer
					}
				}else{
					set_ledl(LED_RXS, 0);								// sub led = off
					set_last_cos(SUB);									// store last COS
				}
				read_sin_flags(SIN_SQSM_CF|SIN_SQSS_CF);				// clear changes flag
			}
			if(iflags & SIN_SEND_CF){
				sin_a2 = fetch_sin(2);
				// update TX LED
				if(sin_a2 & SIN_SEND){
					// main led
					GPIO_PORTF_AHB_DATA_R |= LED_TX;
				}else{
					GPIO_PORTF_AHB_DATA_R &= ~LED_TX;
				}
				read_sin_flags(SIN_SEND_CF);							// clear changes flag
			}
		}
		//**************************************
		// process vfo display update signal
		process_VFODISP(band_focus);
		//**************************************
		// process dial and mic up/dn changes
		iflags |= process_DIAL(band_focus);
		//**************************************
		// process timeouts (MHz, xflag and SUB)
		if(!mhz_time(0) && (maddr < MHZ_OFF)){							// process mhz digit timeout:
			do_2beep();													// timeoute alert beep (Morse "I")
			if(band_focus == MAIN) digblink(MAIN_CS|maddr,0);
			else digblink(maddr,0);
			maddr = MHZ_OFF;
			copy_vfot(band_focus);
			vfo_change(band_focus);
		}
		if((xmodeq & VOL_XFLAG) && (!v_time(0))){						// if vol x-mode timeout:
			xmodeq &= ~VOL_XFLAG;
//			iflags |= SIN_SRFS_CF;										// update sub SRFs
//			ssmet(0xff, 0);												// update glass
		}
		if((xmodeq & SQU_XFLAG) && (!q_time(0))){						// if squ x-mode timeout:
			xmodeq &= ~SQU_XFLAG;
//			iflags |= SIN_SRFM_CF;										// update main SRFs
//			msmet(0xff, 0);												// update glass
		}
		if((xmodeq & OFFS_XFLAG) && (!offs_time(0))){					// if offs x-mode timeout:
			do_2beep();													// timeoute alert beep (Morse "I")
			if(band_focus == MAIN) digblink(MAIN_CS|(maddr&(~MHZ_OFFS)),0);
			else digblink(maddr,0);
			mhz_time(0xff);												// clear timers
			offs_time(0xff);
			maddr = MHZ_OFF;
			xmodeq &= ~OFFS_XFLAG;
			if(band_focus == MAIN) vfo_display |= MAIN;					// update main VFO
			else vfo_display |= SUB_D;									// update sub VFO
			// if tx
			force_push();												// force update to NVRAM
		}
		if(band_focus == SUB){
			if(!sub_time(0)){											// if SUB timeout, return to main focus
				do_3beep();												// timeoute alert beep (Morse "S")
				band_focus = MAIN;
				asub(0);
				ats(read_dplx(MAIN) & TSA_F);
				i = get_lohi(band_focus, 0xff);
				if(i){													// HILO updates based on focus
					alow(1);
				}else{
					alow(0);
				}
				k = get_band_index(band_focus);							// reset focus pointer
			}
		}
		if(tsdisplay){
			if(!ts_time(0)){											// if SUB timeout, return to main focus
				do_2beep();												// timeoute alert beep (Morse "S")
				tsdisplay = 0;
				if(band_focus == MAIN) vfo_display |= MAIN;				// update main VFO
				else vfo_display |= SUB_D;								// update sub VFO
			}
		}
		//**************************************
		// process keys
		b = 0;
		if(got_key()){													// only run through this branch if there are keys to input
			if(band_focus == SUB){
				sub_time(1);											// reset timeout
			}
			i = get_key();												// pull key in from buffer space
			// scan cancel
			switch(i){													// process scan cancel key presses
			case BANDchr: //VFOchr:
			case VMchr: //MRchr:
			case MSchr:
			case CALLchr:
			case Tchr: //TONEchr:
			case MODEchr: //DUPchr:
			case SETchr:
				set_slide(band_focus, 0);
			case MHZchr:
				doscan(band_focus, 0);
				break;

			default:
				break;
			}
			i = test_for_cancel(i);										// check to see if keycode qualifies for "cancel" (substitutes "cancel" key in place of the actual key)
			b = 1;
			switch(i){													// dispatch to key-specific code segments...
			case MODEchr: //DUPchr:												// duplex button, initial press
				if(band_focus == MAIN_MODE){
					switch(inc_dplx(MAIN) & (DPLX_MASK)){				// advance the duplex (function returns changed status)
					default:
					case DPLX_S:
						mdupa('S');
						break;

					case DPLX_P:
						mdupa('+');
						break;

					case DPLX_M:
						mdupa('-');
						break;
					}
				}else{
					switch(inc_dplx(SUB) & (DPLX_MASK)){
					default:
					case DPLX_S:
						sdupa('S');
						break;

					case DPLX_P:
						sdupa('+');
						break;

					case DPLX_M:
						sdupa('-');
						break;
					}
				}
				// if tx
				force_push();											// force update to NVRAM
				break;

			case HILOchr:												// HILO button, initial press
				i = get_lohi(band_focus, 0xff);
				if(i){
					i = 0;
					alow(0);
				}else{
					i = 1;
					alow(1);
				}
				get_lohi(band_focus, i);
				if(band_focus == MAIN){
					add_vfo(band_focus, 0, MHZ_ONE);
				}
				// if tx
				force_push();											// force update to NVRAM
				break;

			case LOCKchr_H:												// LOCK button, hold-press to unlock
				lock_mode = 0;
				break;

			case LOCKchr:												// LOCK button, initial press to lock
				if(lock_mode) b = 0;
				lock_mode = 1;
				break;

			case SUBchr:												// SUB button, initial press
				if(!(sys_err & (NO_B_PRSNT|NO_MUX_PRSNT|NO_SUX_PRSNT))){ // only allow sub button if there are no errors
					if(band_focus == MAIN_MODE){
						band_focus = SUB_MODE;
						asub(1);
						ats(read_dplx(SUB) & TSA_F);					// TS flag updates based on focus
						sub_time(1);									// set sub timer
					}else{
						band_focus = MAIN_MODE;
						asub(0);
						ats(read_dplx(MAIN) & TSA_F);
						sub_time(0xff);									// clear sub timer
					}
					k = get_band_index(band_focus);						// reset focus pointer
					i = get_lohi(band_focus, 0xff);
					update_lcd(band_focus, SUB);
					if(xmodeq & TEXTS_SLIDE){
						update_radio_all(SUB_VQ);						// don't update freq if slider operating
					}else{
						update_radio_all(SUB_ALL);						// update all
					}
/*					if(i){												// HILO updates based on focus
						alow(1);
					}else{
						alow(0);
					}*/
				}else{
					b = 0;	// no beep
				}
				break;

			case MHZchr:												// MHZ button 1st press
				if(!(xmode[k] & MC_XFLAG)){								// if in mem/call mode, go to string slide mode check...
					if(!mhz_time(0)){									// if timer is not zero, one of the MHz modes is active
						if(maddr == MHZ_OFF){							// this means that the thumbwheel mode isn't active and MHz mode is off
							maddr = MHZ_ONE;							// set MHz mode
							amhz(1);									// turn on mhz icon
						}else{
							maddr = MHZ_OFF;							// if any other MHz mode active, turn it off
							amhz(0);									// turn off mhz icon
						}
					}else{
						sii = 1;
						for(i=0; i<maddr ; i++){						// construct the multiplier for the currently selected digit
							sii *= 10;
						}
						if(maddr == 0) sii = 5;							// lowest digit can only be 0 or 5 (these are all 5KHz stepped radios, except for the UX129 which is a 10 KHz step)
						set_mhz_step(sii);								// store the step mulitplier
//						else set_mhz_step(sii / 10L);
//						i = set_mhz_addr(0xff);
						if(band_focus == MAIN_MODE) digblink(MAIN_CS|maddr,0); // un-blink the old digit (m/s)
						else digblink(maddr,0);
						if(--maddr == 0xff){							// move the digit and process roll-under
							if(get_band_index(band_focus) == ID1200_IDX){
								maddr = 5;
								set_mhz_step(100000L);
							}else{
								maddr = 4;
								set_mhz_step(10000L);
							}
						}
						if(band_focus == MAIN_MODE) digblink(MAIN_CS|maddr,1); // blink the new digit (m/s)
						else digblink(maddr,1);
					}
				}else{
					// toggle mem string disp mode
					set_slide(band_focus, 0xff);
/*					if(band_focus == MAIN){
						if(xmodeq & TEXTM_SLIDE) xmodeq &= ~TEXTM_SLIDE;
						else xmodeq |= TEXTM_SLIDE;
						vfo_display |= MAIN;
					}else{
						if(xmodeq & TEXTS_SLIDE) xmodeq &= ~TEXTS_SLIDE;
						else xmodeq |= TEXTS_SLIDE;
						vfo_display |= SUB_D;
					}*/
				}
				break;

			case MHZchr_H:												// MHZ button, hold (this enters thumbwheel mode)
				if(!(xmode[k] & MC_XFLAG)){								// mem/call, ignore
					amhz(0);											// turn off mhz icon
//					set_mhz_addr(0);
					if(!mhz_time(0)){									// not in thumbwheel mode
						mhz_time(1);									// start timer
						set_mhz_step(100000L);							// set start step
						if(get_band_index(band_focus) == ID1200_IDX){
							maddr = 5;
						}else{
							maddr = 4;
						}
						if(band_focus == MAIN_MODE) digblink(MAIN_CS|maddr,1); // blink the 1st digit (m/s)
						else digblink(maddr,1);
						temp_vfo(band_focus);							// copy vfo -> vfot
					}else{												// already in thumbwheel mode (this will cancel the thumbwheel mode)
//						i = set_mhz_addr(0xff);
						if(xmodeq & OFFS_XFLAG){						// offset is differentiated from VFO frequency
							if(band_focus == MAIN_MODE) digblink(MAIN_CS|(maddr&(~MHZ_OFFS)),0); // blkin digit off
							else digblink(maddr&(~MHZ_OFFS),0);
							mhz_time(0xff);								// clear timers
							offs_time(0xff);
							maddr = MHZ_OFF;							// turn off thumbwheel mode
						}else{
							if(band_focus == MAIN_MODE) digblink(MAIN_CS|(maddr&(~MHZ_OFFS)),0);
							else digblink(maddr&(~MHZ_OFFS),0);
							copy_vfot(band_focus);						// copy updated temp vfo to normal vfo
							set_mhz_step(5L);
							mhz_time(0xff);								// clear timers
							offs_time(0xff);
							maddr = MHZ_OFF;
						}
						force_push();									// force update to NVRAM
					}
					b = 2;	// 2beeps
				}
				break;

			case Vupchr:												// volume increase, initial press... VOL/SQU commandeer the SRF meters and mem ch digit to display level graphics
				v_time(1);
				xmodeq |= VOL_XFLAG;
				adjust_vol(band_focus, 1);
				voltape(adjust_vol(band_focus, 0), 0);					// signal a volume update
				force_push();											// force update to NVRAM
				break;

			case Vdnchr_H:												// VOL-, hold mutes the audio
				adjust_vol(band_focus, 0x81);							// set vol = 1 (then to 0 below...)
			case Vupchr_H:												// VOL+ hold displays the current level without change
				b = 2;	// 2beeps
			case Vdnchr:												// VOL-, initial press decreases the VOL level 1 step
				v_time(1);
				xmodeq |= VOL_XFLAG;
				adjust_vol(band_focus, -1);
				voltape(adjust_vol(band_focus, 0), 0);
				force_push();											// force update to NVRAM
				break;

			case Qupchr_H:												// SQU+, hold sets max squelch
				adjust_squ(band_focus, 0x80 | (LEVEL_MAX - 1));			// set squ = max-1 (then to max below...)
			case Qdnchr_H:												// SQU- hold displays the current level without change
				b = 2;	// 2beeps
			case Qupchr:												// SQU+, initial press increases the SQU level 1 step
				q_time(1);
				xmodeq |= SQU_XFLAG;
				adjust_squ(band_focus, 1);
				squtape(adjust_squ(band_focus, 0), 0);
				force_push();											// force update to NVRAM
				break;

			case Qdnchr:												// SQU-, initial press decreases the SQU level 1 step
				q_time(1);
				xmodeq |= SQU_XFLAG;
				adjust_squ(band_focus, -1);
				msmet(adjust_squ(band_focus, 0), 0);
				force_push();											// force update to NVRAM
				break;

			case SMUTEchr_H:											// SMUTE hold sets mute of main-band audio if sub muted (or no action)
				mmute_action(&mute_mode);
				if(!(mute_mode & SUB_MUTE)){
					smute_action(&mute_mode);
				}
				b = 2;	// 2beeps
				break;

			case SMUTEchr:												// SMUTE initial press toggles mute of sub-band audio (unmutes main if previously muted)
				if(mute_mode & MS_MUTE){
					mmute_action(&mute_mode);
				}
				smute_action(&mute_mode);
				break;

			case Tchr: //TONEchr:												// TONE, initial: toggle tone on/off or cancel adj mode (uses xmode to display tone freq in VFO space)
				if(xmodeq & TONE_XFLAG){									// tone mode already on, cancel it
					if(band_focus == MAIN){
						vfo_display |= MAIN;							// update main VFO
					}else{
						vfo_display |= SUB_D;							// update sub VFO
					}
					xmodeq &= ~TONE_XFLAG;
				}else{
					i = adjust_toneon(band_focus, 0xff);				// tone mode off, turn it on...
					if(i) i = 0;
					else i = 1;
					adjust_toneon(band_focus, i);
					if(band_focus == MAIN){								// display tone freq
						mtonea(i);
					}else{
						stonea(i);
					}
					force_push();										// force update to NVRAM
				}
				break;

			case Tchr_H: //TONEchr_H:												// TONE hold, adjust tone
				vfo_display |= VMODE_TDISP;								// force tone freq to display
				xmodeq |= TONE_XFLAG;									// enable tone xmode
				i = adjust_toneon(band_focus, 0xff);					// undo tone toggle that got us here...
				if(i) i = 0;
				else i = 1;
				adjust_toneon(band_focus, i);
				if(band_focus == MAIN){
					mtonea(i);
				}else{
					stonea(i);
				}
				force_push();											// force update to NVRAM
				b = 2;	// 2beeps
				break;

			case TSchr_H:												// TS hold, set-mode for TS_A/B (this is preliminary, uses beeps as feedback)
				if(read_tsab(band_focus, TSB_SEL) == TS_10){
//					set_tsab(band_focus, TSB_SEL, TS_25);
					tsdisplay = 1;
				}else{
					if(read_tsab(band_focus, TSA_SEL) == TS_5){
//						set_tsab(band_focus, TSA_SEL, TS_10);
						tsdisplay = 2;
					}else{
//						set_tsab(band_focus, TSA_SEL, TS_5);
//						set_tsab(band_focus, TSB_SEL, TS_10);
						tsdisplay = 3;
					}
				}
				ts_time(1);												// set timeout
				vfo_display |= VMODE_TSDISP;							// set display
				togg_tsab(band_focus);									// undo the toogle from initial press
				b = 2;	// 2beeps
				break;

			case TSchr:													// TS, initial press: freq step mode toggle
				if(tsdisplay){
					ts_time(0xff);										// turn off TS adj mode
				}else{
					togg_tsab(band_focus);
				}
				break;

			case MODEchr_H: //DUPchr_H:												// duplex button HOLD (offset adjust)
				// undo the duplex press
				if(band_focus == MAIN_MODE){
					inc_dplx(MAIN);
					inc_dplx(MAIN);
					mfreq((U32)get_offs(band_focus), LEAD0);
				}else{
					inc_dplx(SUB);
					inc_dplx(SUB);
					sfreq((U32)get_offs(band_focus), LEAD0);
				}
				mhz_time(1);
				offs_time(1);
				set_mhz_step(100000L);
				if(get_band_index(band_focus) == ID1200_IDX){
					maddr = MHZ_OFFS|5;
				}else{
					maddr = MHZ_OFFS|4;
				}
				if(band_focus == MAIN_MODE) digblink(MAIN_CS|(maddr&(~MHZ_OFFS)),1);
				else digblink(maddr&(~MHZ_OFFS),1);
				xmodeq |= OFFS_XFLAG;
//				update_lcd();
				b = 2;	// 2beeps
				break;

			case BANDchr: //VFOchr:												// VFO, initial press: cycles selected band modules (this is the BAND button on the IC-901)
				if(!(sys_err & (NO_B_PRSNT|NO_MUX_PRSNT|NO_SUX_PRSNT))){ // only allow sub button if there are no errors
					set_next_band(band_focus);
					update_lcd(band_focus, band_focus);
					if(band_focus == MAIN) i = MAIN_ALL;
					else i = SUB_ALL;
					update_radio_all(i);
					force_push();										// force update to NVRAM
					set_bandnv();
				}else{
					b = 0;	// no beep
				}
				break;

			case MSchr:
				if(!(sys_err & (NO_B_PRSNT|NO_MUX_PRSNT|NO_SUX_PRSNT))){ // only allow sub button if there are no errors
//					mute_radio(0);										// mute radio during band-swap
					set_swap_band();
					if(mute_mode & SUB_MUTE){
						adjust_vol(MAIN, 0);							// restore main vol
					}
					update_lcd(band_focus, MAIN);
					update_lcd(band_focus, SUB);
					update_radio_all(UPDATE_ALL);
					force_push();										// force update to NVRAM
				}else{
					b = 0;	// no beep
				}
				break;

			case CHKchr:												// CHECK, initial press: this is the reverse mode (no squ change)
				if(!chkmode){
					// if chkmode == 0, then initiate chk/rev
					rev_vfo(band_focus);
					chkmode = REV_FLAG;
					if(band_focus == MAIN) mset_500hz(2);				// blink chk/rev flasher
					else sset_500hz(2);
				}else{
					// if chkmode == 1, then cancel chk/rev
					rev_vfo(band_focus);
					chkmode = 0;
					if(band_focus == MAIN) mset_500hz(0);				// un-blink chk/rev flasher
					else sset_500hz(0);
				}
				break;

			case CHKchr_H:												// CHECK hold: this opens the SQU
				// if chkmode active, this is the hold from that initiation, so go to SQU open state (else, ignore)
				if(chkmode){
					chkmode |= REV_SQU_FLAG;
					// !!! open squelch
					chksqu = adjust_squ(band_focus, 0);
					adjust_squ(band_focus, 0x80);						// open up squ (set to zero)
					b = 2;	// 2beeps
				}
				break;

			case CHKchr_R:												// CHECK, release: closes out chk/rev mode
				if(chkmode & REV_SQU_FLAG){
					// if squ mode: cancel check/rev
					adjust_squ(band_focus, chksqu | 0x80);				// restore squelch
					rev_vfo(band_focus);								// return VFO to normal
					chkmode = 0;										// clear check flag
					if(band_focus == MAIN) mset_500hz(0);				// un-blink chk/rev flasher
					else sset_500hz(0);
				}else{
					// if chkmode (not squ) && duplex == S: cancel check/rev
					if(chkmode && ((read_dplx(band_focus) & DPLX_MASK) == DPLX_S)){
						rev_vfo(band_focus);
						chkmode = 0;
						if(band_focus == MAIN) mset_500hz(0);			// if simplex, cancel reverse mode
						else sset_500hz(0);
					}
				}
				b = 0;	// no beeps
				break;

			case VMchr_H: //MRchr_H:												// Mem-mode recall last cos active band/mem
				// if !mem mode, abort
				if(!(xmode[k] & MEM_XFLAG)){							// enter mem mode if off
					if((xmode[k] & (MC_XFLAG)) == 0){					// copy vfo to temp
						copy_vfo2temp(band_focus);
					}
					mema(band_focus, 1);								// turn on "M"
					mskpa(band_focus, get_scanmem(band_focus));			// update "skp" annunc.
					xmode[k] |= MEM_XFLAG;
				}
				i = get_last_cos(band_focus);
//				j = i >> 5;
				i &= 0x1f;
				read_mem(band_focus, get_memnum(band_focus, 0));
				write_xmode(band_focus);
				update_lcd(band_focus, band_focus);
				if(band_focus == MAIN) i = MAIN_ALL;
				else i = SUB_ALL;
				update_radio_all(i);
				break;

			case VMchr: //MRchr:													// Mem-mode toggle: this toggles between mem and vfo mode
				// if mem mode, turn off
				if(xmode[k] & CALL_XFLAG) xmode[k] &= ~(MC_XFLAG);		// turn off call, force M on
				if(xmode[k] & MEM_XFLAG){
					mema(band_focus, 0);								// turn off "M"
					xmode[k] &= ~MEM_XFLAG;
					if((xmode[k] & (MC_XFLAG)) == 0){
						copy_temp2vfo(band_focus);
					}
				}else{
					if((xmode[k] & (MC_XFLAG)) == 0){					// copy vfo to temp
						copy_vfo2temp(band_focus);
					}
					mema(band_focus, 1);								// turn on "M"
					mskpa(band_focus, get_scanmem(band_focus));			// update "skp" annunc.
					xmode[k] |= MEM_XFLAG;
					read_mem(band_focus, get_memnum(band_focus, 0));
				}
				write_xmode(band_focus);
				update_lcd(band_focus, band_focus);
				if(band_focus == MAIN) i = MAIN_ALL;
				else i = SUB_ALL;
				update_radio_all(i);
				break;

			case CALLchr:												// call-mode toggle: this toggles between call mode
				if(xmode[k] & CALL_XFLAG){
					// call mode going off...
					xmode[k] &= ~CALL_XFLAG;							// turn off call mode
					if(xmode[k] & MEM_XFLAG){							// if mem active, go to mem mode..
						mema(band_focus, 1);							// turn on "M" indicator
						read_mem(band_focus, get_memnum(band_focus, 0));
					}
					if((xmode[k] & (MC_XFLAG)) == 0){
						copy_temp2vfo(band_focus);						// if no mem mode, recall VFO
					}
				}else{
					// call mode coming on...
					mema(band_focus, 0);								// turn off "M" indicator
					if((xmode[k] & (MC_XFLAG)) == 0){					// copy vfo state to temp only if not done already
						copy_vfo2temp(band_focus);
					}
					xmode[k] |= CALL_XFLAG;								// turn on call mode
					read_mem(band_focus, get_callnum(band_focus, 0));	// recall VFO state
				}
				if(band_focus == MAIN){
					iflags |= SIN_SRFM_CF;								// force update of MSRF
				}else{
					iflags |= SIN_SRFS_CF;								// force update of SSRF
				}
				write_xmode(band_focus);
				update_lcd(band_focus, band_focus);						// update display & radio
				if(band_focus == MAIN) i = MAIN_ALL;
				else i = SUB_ALL;
				update_radio_all(i);
				break;

			case CALLchr_H:												// call-mode hold: write VFO to call mem
				if(xmode[k] & CALL_XFLAG){
					copy_temp2vfo(band_focus);
					write_mem(band_focus, get_callnum(band_focus, 0));
					if(band_focus == MAIN){
						iflags |= SIN_SRFM_CF;							// force update of MSRF
					}else{
						iflags |= SIN_SRFS_CF;							// force update of SSRF
					}
					update_lcd(band_focus, band_focus);
					if(band_focus == MAIN) i = MAIN_ALL;
					else i = SUB_ALL;
					update_radio_all(i);
					b = 2;	// 2beeps
				}
				break;

			case MWchr:													// Mem-write: arm for skip select
				if(xmode[k] & (MEM_XFLAG)){
					b = 1;	// 1beeps
					hflag = MWchr;
				}
				break;

			case MWchr_R:												// Mem-write: if armed for skip select, toggle skip status
				if(hflag == MWchr){
					i = togg_scanmem(band_focus);
					mskpa(band_focus, i);								// update "skp" annunc.
					if(xmodeq & (MSCANM_XFLAG|MSCANS_XFLAG)){
						get_mscan(band_focus);
					}
					b = 1;	// 1beeps
					hflag = 0;
				}
				break;

			case MWchr_H:												// Mem-write: write VFO to mem in VFO mode; if mem mode, exits with mem in VFO (copy mem to VFO)
				if(xmode[k] & (MC_XFLAG)){
					xmode[k] &= ~(MC_XFLAG);							// turn off call/mem, no VFO coppy-back
					mema(band_focus, 0);								// turn off "M"
					if(band_focus == MAIN){
						iflags |= SIN_SRFM_CF;							// force update of MSRF
					}else{
						iflags |= SIN_SRFS_CF;							// force update of SSRF
					}
					update_lcd(band_focus, band_focus);
				}else{
					write_mem(band_focus, get_memnum(band_focus, 0));
				}
				b = 2;	// 2beeps
				hflag = 0;
				break;

			default:
				b = 0;	// no beeps
				break;
			}
			// process beeps
			switch(b){
			case 1:
				do_1beep();
				break;

			case 2:
				do_2beep();
				break;

			default:
				break;
			}
		}
	}
	GPIO_PORTB_AHB_DATA_R &= ~SPARE_PB1;				// !!! debug GPIO
	return band_focus;
}	// end process_MS()


//-----------------------------------------------------------------------------
// process_VFODISP() process freq display options
//-----------------------------------------------------------------------------
void process_VFODISP(U8 focus){
	U8	i;			// temps
	U8	j = 0;
	U16	ii;
	char tbuf[8];	// ascii freq field buffer

	if(vfo_display){
		// calculate display mode for switch() frame:				// This makes it easier to maintain the VFO display modes (now and future)
		if(xmodeq & (TONE_XFLAG|OFFS_XFLAG)){
			j = TONE_DISP;
		}else{
			if(tsdisplay){
				j = TS_DISP;
			}else{
				if(focus == MAIN) i = TEXTM_SLIDE;
				else i = TEXTS_SLIDE;
				if(!(xmodeq & i)) j = NORM_DISP;
			}
		}
		// dispatch switch() - each "case" is a display mode
		switch(j){
		case TONE_DISP:
			if(xmodeq & TONE_XFLAG){
				// TONE adjust mode
				if(vfo_display & VMODE_TDISP){						// if triggered
					i = adjust_tone(focus, 0) & CTCSS_MASK;
					if((i > TONE_MAX) || (i == 0)){
						i = 1;										// error trap
					}
					ii = tone_list[i - 1];							// tone list is "0" origin, tones values are "1" origin
					sprintf(tbuf,"%4d  ", ii);						// convert number to display string
					puts_lcd(focus, tbuf, 1);
					vfo_display &= ~VMODE_TDISP;					// clear update trigger
					force_push();									// force update to NVRAM
				}
//			}else{
				// offset adjust mode

			}
			break;

		case TS_DISP:
			switch(tsdisplay){
			case 1:
				puts_lcd(focus, "A 5B10", 1);
				break;

			case 2:
				puts_lcd(focus, "A 5B25", 1);
				break;

			case 3:
				puts_lcd(focus, "A10B25", 1);
				break;

			default:
				puts_lcd(focus, "A--B--", 1);
				break;
			}
			vfo_display &= ~(VMODE_TSDISP);
			break;

		case NORM_DISP:
			// normal VFO display
//				sprintf(dgbuf,"vfodisp: %02x",vfo_display); //!!!
//				putsQ(dgbuf);
			if(vfo_display & MAIN){
				if(ptt_change & PTT_KEYED) i = MAIN | VMODE_ISTX;
				else i = MAIN;
				mfreq(get_freq(i), 0);							// update main freq display from vfo or vfotr
				vfo_display &= ~(VMODE_ISTX | MAIN);

/*				if(vfo_display & VMODE_ISTX){
					mfreq(get_freq(MAIN | VMODE_ISTX), 0);		// update main freq display from vfotr
//						sprintf(dgbuf,"vfofrqT: %d",get_freq(focus | 0x80)); //!!!
//						putsQ(dgbuf);
				}else{
					mfreq(get_freq(MAIN), 0);					// update main freq display
//						sprintf(dgbuf,"vfofrqR: %d",get_freq(focus)); //!!!
//						putsQ(dgbuf);
				}
				vfo_display &= ~(VMODE_ISTX | MAIN);*/
			}else{
				if(vfo_display & SUB_D){
					if(!(xmodeq & TEXTS_SLIDE)){
						sfreq(get_freq(SUB), 0);
					}
//					sfreq(get_freq(SUB), 0);					// update sub freq display
					vfo_display &= ~(SUB_D);
				}
			}
			break;

		case TXTSLID_DISP:
			break;

		default:
			break;
		}
	}
	if(xmodeq & (TEXTM_SLIDE | TEXTS_SLIDE)){					// update main/sub slider displays
		// text slider display mode - always runs
		if(!slide_time(0)){
			if(xmodeq & TEXTM_SLIDE){
				i = puts_slide(MAIN, get_nameptr(MAIN), 0);
			}
			if(xmodeq & TEXTS_SLIDE){
				i = puts_slide(SUB, get_nameptr(SUB), 0);
			}
			slide_time(1);
		}
	}
	return;
}

//-----------------------------------------------------------------------------
// test_for_cancel() looks for cancel keys based on mode registers
//	called from top of key dispatch tree
//-----------------------------------------------------------------------------
U8 test_for_cancel(U8 key){
	U8	i = key;		// temp

	if((maddr & ~MHZ_OFFS) < MHZ_OFF){					//	force the MHZ-hold key-code (this clears by-digit mode)
		switch(key){									// process thumbwheel key-cancel
		case Vupchr:									// don't cancel on these codes:
		case Vdnchr:									// VOLu/d, SQUu/d, MHz, DUP(release)
		case Vupchr_H:
		case Vdnchr_H:
		case Vdnchr_R:
		case Vupchr_R:
		case Qupchr:
		case Qdnchr:
		case Qupchr_H:
		case Qdnchr_H:
		case Qupchr_R:
		case Qdnchr_R:
		case MHZchr:
		case MHZchr_H:
		case MHZchr_R:
		case MODEchr_R: //DUPchr_R:
			break;

		default:										// for all other keys, modify to abort MHz digit mode cancel
			i = MHZchr_H;
			break;
		}
	}else{
		if(chkmode){									// for check mode, CHKchr is cancel
			switch(key){								// process thumbwheel key-cancel
			case Vupchr:								// don't cancel on these codes:
			case Vdnchr:								// VOLu/d, SQUu/d, CHECK
			case Vupchr_H:
			case Vdnchr_H:
			case Vdnchr_R:
			case Vupchr_R:
			case Qupchr:
			case Qdnchr:
			case Qupchr_H:
			case Qdnchr_H:
			case Qupchr_R:
			case Qdnchr_R:
			case CHKchr:
			case CHKchr_H:
			case CHKchr_R:
				break;

			default:									// for all other keys, modify to chk/rev cancel
				i = CHKchr;
				break;
			}
		}else{
			if(xmodeq & TONE_XFLAG){
				switch(key){								// process TONE adjust key-cancel
				case Vupchr:								// don't cancel on these codes:
				case Vdnchr:								// VOLu/d, SQUu/d, TONE
				case Vupchr_H:
				case Vdnchr_H:
				case Vdnchr_R:
				case Vupchr_R:
				case Qupchr:
				case Qdnchr:
				case Qupchr_H:
				case Qdnchr_H:
				case Qupchr_R:
				case Qdnchr_R:
				case Tchr: //TONEchr:
				case Tchr_H: //TONEchr_H:
				case Tchr_R: //TONEchr_R:
					break;

				default:									// for all other keys, modify to tone cancel
					i = Tchr; //TONEchr;
					break;
				}
			}else{
				if(xmodeq & (VOL_XFLAG|SQU_XFLAG)){
					switch(key){							// process V/Q adjust key-cancel
					case SUBchr:
					case MSchr:
					case BANDchr: //VFOchr:
					case VMchr: //MRchr:
					case CALLchr:
					case SUBchr_H:
					case MSchr_H:
					case BANDchr_H: //VFOchr_H:
					case VMchr_H: //MRchr_H:
					case CALLchr_H:
					case SUBchr_R:
					case MSchr_R:
					case BANDchr_R: //VFOchr_R:
					case VMchr_R: //MRchr_R:
					case CALLchr_R:
						v_time(0xff);						// cancel vol/squ
						q_time(0xff);
						break;

					default:								// for all other keys, modify to tone cancel
						break;
					}
				}else{
					if(tsdisplay){
						switch(key){						// process TS adjust key-cancel
						case SUBchr:						// these key codes cancel TS adj mode
						case MSchr:
						case BANDchr: //VFOchr:
						case VMchr: //MRchr:
						case CALLchr:
						case Tchr_H: //TONEchr_H:
						case MHZchr:
						case SETchr:
						case MODEchr_H: //DUPchr_H:
							ts_time(0xff);					// cancel TS adj
							break;

						default:							// for all other keys, modify to tone cancel
							break;
						}
					}
				}
			}
		}
	}
	return i;
}	// test_for_cancel()

//-----------------------------------------------------------------------------
// mmute_action() toggles mute of the main-band
// smute_action() toggles mute of the sub-band
// band mute is signalled by flashing the freq display
//-----------------------------------------------------------------------------
void mmute_action(U8* mute_flag){
//	U8	i;		// temp

	if(*mute_flag & MS_MUTE){
		*mute_flag &= ~MS_MUTE;
		// unblink main VFO
		digblink(MAIN_CS|ALL_DIGS, 0);
	}else{
		*mute_flag |= MS_MUTE;
		// blink main VFO
		digblink(MAIN_CS|ALL_DIGS, 1);
	}
	mute_radio(*mute_flag);
	return;
}	// end mmute_action()

void smute_action(U8* mute_flag){
//	U8	i;		// temp

	if(*mute_flag & SUB_MUTE){
		*mute_flag &= ~SUB_MUTE;
		// unblink sub VFO
		digblink(ALL_DIGS, 0);
	}else{
		*mute_flag |= SUB_MUTE;
		// blink sub VFO
		digblink(ALL_DIGS, 1);
	}
	mute_radio(*mute_flag);
	return;
}	// end smute_action()

//-----------------------------------------------------------------------------
// process_DIAL() handles dial changes
//-----------------------------------------------------------------------------
U32 process_DIAL(U8 focus){
	U8	i;					// temp
	U8	j;
	U8	k;					// band index
	U8	m;
	U32	rflags = 0;			// return flags

	k = get_band_index(focus);
	// process dial
	j = is_mic_updn(0, focus, xmodeq) + get_dial(1);
	if(j){
		if((xmodeq & MSCANM_XFLAG) && (focus == MAIN)){
			doscan(MAIN, 0);
			set_slide(focus, 0);
			return rflags;
		}
		if((xmodeq & MSCANS_XFLAG) && (focus == SUB)){
			doscan(SUB, 0);
			set_slide(focus, 0);
			return rflags;
		}
		if(focus == SUB){
			sub_time(1);								// reset timeout
		}
	}
	if(xmodeq & TONE_XFLAG){
		if(j){
			adjust_tone(focus, j);
			vfo_display |= VMODE_TDISP;
		}
		j = 0;											// allow sub-scan to operate
	}else{
		if(tsdisplay){
			// freq step select mode
			if(j){
				if(read_tsab(focus, TSB_SEL) == TS_10){
					set_tsab(focus, TSB_SEL, TS_25);
					tsdisplay = 2;
				}else{
					if(read_tsab(focus, TSA_SEL) == TS_5){
						set_tsab(focus, TSA_SEL, TS_10);
						tsdisplay = 3;
					}else{
						set_tsab(focus, TSA_SEL, TS_5);
						set_tsab(focus, TSB_SEL, TS_10);
						tsdisplay = 1;
					}
				}
				vfo_display |= VMODE_TSDISP;
				ts_time(1);
			}
		}else{
			if(j){
				// if focus and mem mode == active:
				if(xmode[k] & (MC_XFLAG)){
					// mem/call# inc/dec
					if(focus == MAIN){
						if(xmode[k] & CALL_XFLAG){
							get_callnum(focus, j);
						}else{
							get_memnum(focus, j);
						}
						rflags = SIN_SRFM_CF;							// force update of MSRF
					}else{
						if(xmode[k] & CALL_XFLAG){
							get_callnum(focus, j);
						}else{
							get_memnum(focus, j);
						}
						rflags = SIN_SRFS_CF;							// force update of MSRF
					}
					if(xmode[k] & CALL_XFLAG){
						read_mem(focus, get_callnum(focus, 0));			// read new call
					}else{
						read_mem(focus, get_memnum(focus, 0));			// read new mem
					}
					rflags |= SIN_SRFS_CF;								// force update of SSRF
					save_mc(focus);										// save changes to NVRAM
					update_lcd(focus, focus);
					if(focus == MAIN) i = MAIN_ALL;
					else i = SUB_ALL;
					update_radio_all(i);
				}else{
					// main freq update
					if((maddr == MHZ_OFF) || (maddr == MHZ_ONE)){
						// MHZ (No thumbwheel) mode:
						i = add_vfo(focus, j, maddr);					// update vfo
						if(i && (focus == MAIN)){
							mfreq(get_freq(MAIN), 0);					// update display
						}
						if(i && (focus == SUB)){
							sfreq(get_freq(SUB), 0);
						}
						vfo_change(focus);
					}else{
						// digit-by-digit (thumbwheel) mode
						m = maddr & (~MHZ_OFFS);						// mask offset mode flag
						if(m == 0) j = (j & 0x01) * 5;					// pick the odd 5KHz
						i = add_vfo(focus, j, maddr);					// update vfo
						if(mhz_time(0)){
							mhz_time(1);								// reset timer
							if(maddr & MHZ_OFFS){
								offs_time(1);
							}
						}
						else{
							mhz_time(0xff);								// exit MHz mode
							offs_time(0xff);
						}
						if(i && (focus == MAIN)){
							mfreq(get_vfot(), LEAD0);					// update main display
						}
						if(i && (focus == SUB)){
							sfreq(get_vfot(), LEAD0);					// update sub display
						}
						vfo_change(focus);
					}
				}
			}
		}
	}
	if(j == 0){
		// main mem scan
		if(xmodeq & MSCANM_XFLAG){
			if(led_status & LED_TX){									// PTT active
				doscan(MAIN, 0);										// shut down scan
				set_slide(MAIN, 0);
				do_1beep();
			}else{
				if(!(led_status & LED_RXM) || (xmodez & MSCANM1_XFLAG)){ // main COS inactive or 1st scan
					if(!scan_time(MAIN, 0) || (xmodez & MSCANM1_XFLAG)){
						// process if scan timer == 0
						xmodez &= ~MSCANM1_XFLAG;
						if(!nxtscan(MAIN, 1)){
							doscan(MAIN, 0);							// shut down scan, no mems enabled
							set_slide(MAIN, 0);
							do_3beep();
						}else{
							scan_time(MAIN, 1);							// reset scan timer
							read_mem(MAIN, get_memnum(MAIN, 0)); 		// read new mem
							rflags |= SIN_SRFS_CF;						// force update of SSRF
							save_mc(MAIN);								// save changes to NVRAM
							update_lcd(focus, MAIN);
							update_radio_all(MAIN_ALL);
						}
					}
				}
			}
		}
		// sub mem scan...
		if(xmodeq & MSCANS_XFLAG){
			if(!(led_status & LED_RXS) || (xmodez & MSCANS1_XFLAG)){ // sub COS inactive
				if(!scan_time(SUB, 0) || (xmodez & MSCANS1_XFLAG)){
					// process if scan timer == 0
					xmodez &= ~MSCANS1_XFLAG;
					if(scan_switch){
						scan_time(SUB, 4);							// reset scan timer after band edge (don't advance mem#)
						scan_switch = 0;							// disable edge flag
					}else{
						i = nxtscan(SUB, 1);						// advance mem#
						if(!i){
							doscan(SUB, 0);							// shut down scan, no mems enabled
							set_slide(SUB, 0);
							do_3beep();
						}else{
							if(i & 0x80){							// band switch
								force_push();						// force update to NVRAM
								set_bandnv();
								scan_switch = 1;					// set switch event flag
								scan_time(SUB, 3);					// reset scan timer for band edge event
							}else{
								scan_time(SUB, 1);					// reset scan timer
							}
							read_mem(SUB, get_memnum(SUB, 0));		// read new mem
							rflags |= SIN_SRFS_CF;					// force update of SSRF
							save_mc(SUB);							// save changes to NVRAM
							update_lcd(focus, SUB);
							update_radio_all(SUB_ALL);
						}
					}
				}
			}
		}
	}
	return rflags;
}	// end process_DIAL()

//-----------------------------------------------------------------------------
// digfreq() sets the frequency by digit addr.
//	if tr = TRUE, turn on blink
//	if tf = FALSE, turn off blink
//
//	digaddr is the digit address index:
//	1234.567 89			(digits "1" and "9" are fixed at 1 and 5, respectively)
//	 ^       ^-- addr 6
//	 |-------- addr 0
//
//	0x80 bit set selects main-band, else sub-band
//	0x40 bit set selects blink ALL digits in selected band (in this case, the value of digaddr is ignored)
//-----------------------------------------------------------------------------
void digblink(U8 digaddr, U8 tf){
	uint8_t		a = digaddr & (~MAIN_CS_MASK);
	uint8_t		m;
	uint8_t		start;					// start/stop digit
	uint8_t		stop;
	uint64_t	mask[3];

	mask[0] = 0;										// pre-fill mask array (blank)
	mask[1] = 0;
	mask[2] = 0;

	if(digaddr & ALL_DIGS){
		start = 0;
		stop = 6;
		if(digaddr & MAIN_CS){
			mask[MGHZ_WORD] = (uint64_t)1<<MGHZ_POS;
			mask[M50HZ_WORD] = (uint64_t)1<<M50HZ_POS;
		}else{
			mask[SGHZ_WORD] = (uint64_t)1<<SGHZ_POS;
			mask[1] = (uint64_t)1<<S50HZ_POS;
		}
	}else{
		start = a;
		stop = a;
	}
	do{
		if(digaddr & MAIN_CS){
			// do main
			m = digmap_m[start];
			if(start == 5){
					mask[1] |= (uint64_t)(ASC278M & 0x3f) << (m & 0x3f);
					mask[0] |= ((uint64_t)0x10);
			}else{
				if(m & 0x80){
					mask[1] |= (uint64_t)ASC278M << (m & 0x3f);
				}else{
					mask[0] |= ((uint64_t)ASC278M << m);
				}
			}
			if(tf){
				clear_seg(mask[0], LBLINK, 0);
				clear_seg(mask[1], LBLINK, 1);
			}else{
				or_seg(mask[0], LBLINK, 0);
				or_seg(mask[1], LBLINK, 1);
			}
		}else{
			// do sub
			m = digmap_s[start];
			if(start == 5){
					mask[2] |= (uint64_t)(ASC278S & 0xfe) << (m & 0x3f);
					mask[1] |= ((uint64_t)0x40000000000000);
			}else{
				if(m & 0x80){
					mask[2] |= (uint64_t)ASC278S << (m & 0x3f);
				}else{
					if(m & 0x40){
						mask[1] |= ((uint64_t)ASC278S << (m & 0x3f));
					}else{
						mask[0] |= ((uint64_t)ASC278S << m);
					}
				}
			}
			if(tf){
				clear_seg(mask[0], LBLINK, 0);
				clear_seg(mask[1], LBLINK, 3);
				clear_seg(mask[2], LBLINK, 2);
			}else{
				or_seg(mask[0], LBLINK, 0);
				or_seg(mask[1], LBLINK, 3);
				or_seg(mask[2], LBLINK, 2);
			}
		}
		start++;
	}while(stop >= start);
	lcd_send(get_blink());
	return;
}	// end digblink()

//-----------------------------------------------------------------------------
// mfreq() sets the main frequency.  binfreq = binary KHz
//	set lzero to display leading zeros
//-----------------------------------------------------------------------------
void mfreq(U32 binfreq, U8 lzero){
//	U8	i;
//	U8	k = 0;
	char bbuf[11];
	U32	ii;
//	U32	jj;

	if(sys_err & (NO_B_PRSNT|NO_MUX_PRSNT)){			// trap "off" and "off-line" error states
		wait(50);
		if(sys_err & NO_B_PRSNT){
			// disp "----" (off-line)
			mputs_lcd(" NOBASE ", 0);
		}else{
			if(sys_err & NO_MUX_PRSNT){
				// disp "OFF"
				mputs_lcd(" OFF--- ", 0);
			}
		}
	}else{
		ii = binfreq/10L;
		if(lzero){
			sprintf(bbuf,"%09ld", ii);					// keep leading zeros (lzero == 1)
		}else{
			sprintf(bbuf,"%9ld", ii);					// suppress leading zeros (if lzero == 0)
		}
		if(bbuf[8] > '5') bbuf[8] = '5';
		mputs_lcd(bbuf,DP1);
	}
	return;
}	// end mfreq()

//-----------------------------------------------------------------------------
// sfreq() sets the sub frequency.  binfreq = binary KHz
//	set lzero to display leading zeros
//-----------------------------------------------------------------------------
void sfreq(U32 binfreq, U8 lzero){
//	U8	i;
//	U8	k = 0;
	char bbuf[11];
	U32	ii;
//	U32	jj;

	if(sys_err & (NO_B_PRSNT|NO_SUX_PRSNT)){										// trap "off" and "off-line" error states
		if(sys_err & NO_B_PRSNT){
			// disp "----" (off-line)
			sputs_lcd(" --------", 0);
		}else{
			if(sys_err & NO_SUX_PRSNT){
				// disp "OFF"
				sputs_lcd("-OFF-", 0);
			}
		}
	}else{
		ii = binfreq/10L;
		if(lzero){
			sprintf(bbuf,"%09ld", ii);					// keep leading zeros (lzero == 1)
		}else{
			sprintf(bbuf,"%9ld", ii);					// suppress leading zeros (if lzero == 0)
		}
		if(bbuf[8] > '5') bbuf[8] = '5';
		sputs_lcd(bbuf,DP1);
	}
	return;
}	// end sfreq()

//-----------------------------------------------------------------------------
// sets the S-meter: srf = 0-6 or VOL/SQU: srf = 0-34 (with hi-bit set)
// msmet() MAIN
// ssmet() SUB
//			seg		seg
//	seg		addr	data
//	6		0x06	0x4
//	3		0x07	0x1
//	4		0x07	0x2
//	5		0x07	0x4
//	0		0x08	0x1
//	1		0x08	0x2
//	2		0x08	0x4
//
// if hi-bit of srf set, disp v/q level (affects mem-ch digit also).
//	SRF met forms most significant slider of level.  Each segment is 5 units.
//		All 7 segments plus the "4" indication in the MEM digit = MAX level.
//	MEM digit forms least sig slider (modulo 5):
//		blank =				"0"
//		bot bar =			"1"
//		bot+mid bar =		"2"
//		bot+mid+top bar =	"3"
//		"o" + top bar =		"4"
//
//	Total level = (number of SRF segments * 5) + mem value
//
// if srf == 0xff, the Fn clears the SRF, MEM, and MEM blink.
//
//-----------------------------------------------------------------------------

void msmet(U8 srf, U8 blink){
	U8	i;

	if(srf > MAX_SRF) i = MAX_SRF;
	else i = srf;
	clear_seg((uint64_t)MS_CLEAR << MS_POS, NOBLINK, MS_WORD);			// clear meter
	or_seg((uint64_t)smetm_map[i] << MS_POS, NOBLINK, MS_WORD); 		// copy new meter to seg reg
	if(blink){
		clear_seg((uint64_t)MS_CLEAR << MS_POS, LBLINK, MS_WORD);
	}else{
		or_seg((uint64_t)MS_CLEAR << MS_POS, LBLINK, MS_WORD);
	}
	lcd_send(get_blink());
	return;
}

//-----------------------------------------------------------------------------
// ssmet()
//-----------------------------------------------------------------------------
void ssmet(U8 srf, U8 blink){
	U8	i;

	if(srf > MAX_SRF) i = MAX_SRF;
	else i = srf;
	clear_seg((uint64_t)SS_CLEAR << SS_POS, NOBLINK, SS_WORD);			// clear meter
	or_seg((uint64_t)smets_map[i] << SS_POS, NOBLINK, SS_WORD); 		// copy new meter to seg reg
	if(blink){
		clear_seg((uint64_t)SS_CLEAR << SS_POS, LBLINK, SS_WORD);
	}else{
		or_seg((uint64_t)SS_CLEAR << SS_POS, LBLINK, SS_WORD);
	}
	lcd_send(get_blink());
	return;
}

//-----------------------------------------------------------------------------
// sets the SQU tape: squ = 0-34
//			seg		seg
//	seg		addr	data
//	6		0x06	0x4
//	3		0x07	0x1
//	4		0x07	0x2
//	5		0x07	0x4
//	0		0x08	0x1
//	1		0x08	0x2
//	2		0x08	0x4
//
// if hi-bit of srf set, disp v/q level (affects mem-ch digit also).
//	SRF met forms most significant slider of level.  Each segment is 4 units.
//
//	Total level = (number of SRF segments * 4)
//
// if squ == 0xff, the Fn clears the tape.
//
//-----------------------------------------------------------------------------

void squtape(U8 squ, U8 blink){
	U8	i;
#define	MAX_VSQ	34

	if(squ == (MAX_VSQ - 1)){
		squ -= 1;
	}
	if(squ >= MAX_VSQ){
		i = SQL_LEN - 1;
	}else{
		if(squ == 0){
			i = 0;
		}else{
			i = ((squ-1)/4) + 1;
		}
	}
	clear_seg((uint64_t)SQL_CLEAR << SQL_POS, NOBLINK, SQLD_WORD);		// clear squ tape
	or_seg((uint64_t)sql_map[i] << SQL_POS, NOBLINK, SQLD_WORD); 		// copy new tape to seg reg
	if(blink){
		clear_seg((uint64_t)SQL_CLEAR << SQL_POS, LBLINK, SQLD_WORD);
	}else{
		or_seg((uint64_t)SQL_CLEAR << SQL_POS, LBLINK, SQLD_WORD);
	}
	lcd_send(get_blink());
	return;
}

void voltape(U8 vol, U8 blink){
	U8	i;

	if(vol == (MAX_VSQ - 1)){
		vol -= 1;
	}
	if(vol >= MAX_VSQ){
		i = VOL_LEN - 1;
	}else{
		if(vol == 0){
			i = 0;
		}else{
			i = ((vol-1)/4) + 1;
		}
	}
	clear_seg((uint64_t)VOL_CLEAR << VOL_POS, NOBLINK, VOL_WORD);		// clear vol tape
	or_seg((uint64_t)vol_map[i] << VOL_POS, NOBLINK, VOL_WORD); 		// copy new tape to seg reg
	if(blink){
		clear_seg((uint64_t)VOL_CLEAR << VOL_POS, LBLINK, VOL_WORD);
	}else{
		or_seg((uint64_t)VOL_CLEAR << VOL_POS, LBLINK, VOL_WORD);
	}
	lcd_send(get_blink());
	return;
}

//-----------------------------------------------------------------------------
// asc27m() takes the ASCII param and returns the LC7582A bitmaps for MAIN
// asc27s() takes the ASCII param and returns the LC7582A bitmaps for SUB
//-----------------------------------------------------------------------------

// LUTs to convert (ASCII - 0x20) to LC7582A bitmaps
//	starts at ASCII 0x20 (space).  0x38 ('!') is "no-corresponding-code" representation
//	(3 horiz bars).  Alternate invalid chr is 0x09 (segments "ab",character '#').

U8 asc7segm[] = {										// main freq LUT
	0x00, 0x2A, 0x14, 0x2A, 0x2A, 0x31, 0x2A, 0x04,		// <spc>  !  "  #  $  %  &  '
	0x23, 0x62, 0x2A, 0x25, 0x2A, 0x20, 0x2A, 0x31,		//     (  )  *  +  ,  -  .  /
	0x5F, 0x50, 0x3B, 0x7A, 0x74, 0x6E, 0x6F, 0x58,		//     0  1  2  3  4  5  6  7
	0x7F, 0x7E, 0x2A, 0x2A, 0x23, 0x22, 0x62, 0x39,		//     8  9  :  ;  <  =  >  ?
	0x7B, 0x7D, 0x67, 0x23, 0x73, 0x2F, 0x2D, 0x4F,		//     @  A  B  C  D  E  F  G
	0x65, 0x40, 0x53, 0x25, 0x07, 0x69, 0x61, 0x63,		//     H  I  J  K  L  M  N  O
	0x3D, 0x7C, 0x21, 0x4e, 0x27, 0x43, 0x57, 0x4B,		//     P  Q  R  S  T  U  V  W
	0x75, 0x76, 0x1B, 0x0F, 0x64, 0x5A, 0x1C, 0x02		//     X  Y  Z  [  \  ]  ^  _
	};

U8 asc7segs[] = {										// sub freq LUT
	0x00, 0xA8, 0x41, 0xA8, 0xA8, 0x19, 0xA8, 0x40, 	// <spc>  !  "  #  $  %  &  '
	0x38, 0x2C, 0xA8, 0x58, 0xA8, 0x08, 0xA8, 0x19, 	//     (  )  *  +  ,  -  .  /
	0xF5, 0x05, 0xB9, 0xAD, 0x4D, 0xEC, 0xFC, 0x85, 	//     0  1  2  3  4  5  6  7
	0xFD, 0xED, 0xA8, 0xA8, 0x38, 0x28, 0x2C, 0x99, 	//     8  9  :  ;  <  =  >  ?
	0xBD, 0xDD, 0x7C, 0x38, 0x3D, 0xF8, 0xD8, 0xF4,		//     @  A  B  C  D  E  F  G
	0x5C, 0x04, 0x35, 0x58, 0x70, 0x9C, 0x1C, 0x3C, 	//     H  I  J  K  L  M  N  O
	0xD9, 0xCD, 0x18, 0xE4, 0x78, 0x34, 0x75, 0xB4, 	//     P  Q  R  S  T  U  V  W
	0x5D, 0x6D, 0xB1, 0xF0, 0x4C, 0xA5, 0xC1, 0x20		//     X  Y  Z  [  \  ]  ^  _
	};

U8	asc27m(char c){
	U8	i;			// temps
	U8	rtn;

	if(c >= ' ') i = (U8)(c - ' ');						// convert char to 7-seg code LUT index
	else i = (U8)('!' - ' ');							// set invalid char
	if(i > sizeof(asc7segm)){
		i = (U8)('!' - ' ');							// set invalid char
	}
	rtn = asc7segm[i];									// return seg code
	return rtn;
}

U8	asc27s(char c){
	U8	i;			// temps
	U8	rtn;

	if(c >= ' ') i = (U8)(c - ' ');						// convert char to 7-seg code LUT index
	else i = (U8)('!' - ' ');							// set invalid char
	if(i > sizeof(asc7segs)){
		i = (U8)('!' - ' ');							// set invalid char
	}
	rtn = asc7segs[i];									// return seg code
	return rtn;
}

//-----------------------------------------------------------------------------
// mputs_lcd() translates an ASCII string to 7-seg for main digits
//	returns #chars written to display
//-----------------------------------------------------------------------------

U8	mputs_lcd(char *s, U8 dp_tf){
	U8	i;					// temps
	U8	d;
	U8	m;
//	U8	msk = asc27m('8');
	uint64_t	temp[2];
	uint64_t	mask[2];

	temp[0] = 0;											// pre-fill LCD segment array (blank)
	temp[1] = 0;
	mask[0] = 0;
	mask[1] = 0;
	if(*s++ == '1') temp[1] |= 0x40;						// 1000 MHz
	else mask[1] |= 0x40;
	i = 0;
	while(*s && (i < 7)){
		d = asc27m(*s++);
		m = digmap_m[i];
		if(i == 5){
				temp[1] |= (uint64_t)(d & 0x3f) << (m & 0x3f);
				if(d & 0x40){
					temp[0] |= ((uint64_t)0x10);
				}
				mask[1] |= (uint64_t)(ASC278M & 0x3f) << (m & 0x3f);
				mask[0] |= ((uint64_t)0x10);
		}else{
			if(m & 0x80){
				temp[1] |= (uint64_t)d << (m & 0x3f);
				mask[1] |= (uint64_t)ASC278M << (m & 0x3f);
			}else{
				temp[0] |= ((uint64_t)d << m);
				mask[0] |= ((uint64_t)ASC278M << m);
			}
		}
		i++;
	}
	if(*s == '5') temp[0] |= 0x8000;						// 50 Hz
	mask[0] |= 0x8000;
	if(dp_tf & DP1) temp[1] |= ((uint64_t)0x01 << 32);		// DP1
	else mask[1] |= ((uint64_t)0x01 << 32);
	if(dp_tf & DP2) temp[0] |= 0x40;						// DP2
	else mask[0] |= 0x40;
	clear_seg(mask[0], NOBLINK, 0);
	clear_seg(mask[1], NOBLINK, 1);
	or_seg(temp[0], NOBLINK, 0);
	or_seg(temp[1], NOBLINK, 1);
	lcd_send(get_blink());
	return (i-1);
}

//-----------------------------------------------------------------------------
// sputs_lcd() translates an ASCII string to 7-seg for sub digits
//	returns #chars written to display
//-----------------------------------------------------------------------------

U8	sputs_lcd(char *s, U8 dp_tf){
	U8	i;					// temps
	U8	d;
	U8	m;
//	U8	msk = asc27s('8');
	uint64_t	temp[3];
	uint64_t	mask[3];

	temp[0] = 0;											// pre-fill LCD segment array (blank)
	temp[1] = 0;
	temp[2] = 0;
	mask[0] = 0;
	mask[1] = 0;
	mask[2] = 0;
							//   54443322211
							//   28406284062840
	if(*s++ == '1') temp[0] |= 0x10000000000000;			// 1000 MHz
	mask[0] |= 0x10000000000000;
	i = 0;
	while(*s && (i < 7)){
		d = asc27s(*s++);
		m = digmap_s[i];
		if(i == 5){
				temp[2] |= (uint64_t)(d & 0xfe) << (m & 0x3f);
				if(d & 0x01){
					temp[1] |= ((uint64_t)0x40000000000000);
				}
				mask[2] |= (uint64_t)(ASC278S & 0xfe) << (m & 0x3f);
				mask[1] |= ((uint64_t)0x40000000000000);
		}else{
			if(m & 0x80){
				temp[2] |= (uint64_t)d << (m & 0x3f);
				mask[2] |= (uint64_t)ASC278S << (m & 0x3f);
			}else{
				if(m & 0x40){
					temp[1] |= ((uint64_t)d << (m & 0x3f));
					mask[1] |= ((uint64_t)ASC278S << (m & 0x3f));
				}else{
					temp[0] |= ((uint64_t)d << m);
					mask[0] |= ((uint64_t)ASC278S << m);
				}
			}
		}
		i++;				   // 54443322211
	}						   // 28406284062840
	if(*s == '5') temp[1] |=    0x00200000000000;			// 50 Hz
	mask[1] |=    0x00200000000000;
	if(dp_tf & DP1) temp[2] |= 0x00000004000000;			// DP1
	mask[2] |= 0x00000004000000;
	if(dp_tf & DP2) temp[1] |= 0x10000000000000;			// DP2
	mask[1] |= 0x10000000000000;
	clear_seg(mask[0], NOBLINK, 0);
	clear_seg(mask[1], NOBLINK, 3);
	clear_seg(mask[2], NOBLINK, 2);
	or_seg(temp[0], NOBLINK, 0);
	or_seg(temp[1], NOBLINK, 3);
	or_seg(temp[2], NOBLINK, 2);
	lcd_send(get_blink());
	return (i-1);
}

//-----------------------------------------------------------------------------
// mchan_lcd() writes 2ch string to mem channel (main)
//-----------------------------------------------------------------------------

void mchan_lcd(char *s){
	uint64_t	temp;		// temps
	uint64_t	mask;

	temp = 0;											// pre-fill LCD segment array (blank)
	mask = 0;
	if(*s++ == '1') temp |= MCH10;						// 1000 MHz
	else mask |= MCH10;
	temp |= (uint64_t)asc27m(*s) << MCH_BPOS;
	mask |= (uint64_t)ASC278M << MCH_BPOS;
	clear_seg(mask, NOBLINK, MCH_WORD);
	or_seg(temp, NOBLINK, MCH_WORD);
	lcd_send(get_blink());
	return;
}

//-----------------------------------------------------------------------------
// schan_lcd() writes 2ch string to mem channel (main)
//-----------------------------------------------------------------------------

void schan_lcd(char *s){
	uint64_t	temp;		// temps
	uint64_t	mask;

	temp = 0;											// pre-fill LCD segment array (blank)
	mask = 0;
	if(*s++ == '1') temp |= SCH10;						// 1000 MHz
	else mask |= SCH10;
	temp |= (uint64_t)asc27s(*s) << SCH_BPOS;
	mask |= (uint64_t)ASC278S << SCH_BPOS;
	clear_seg(mask, NOBLINK, SCH_WORD);
	or_seg(temp, NOBLINK, SCH_WORD);
	lcd_send(get_blink());
	return;
}

//-----------------------------------------------------------------------------
// puts_lcd() translates an ASCII string to 7-seg for sub digits
//	returns #chars written to display
//-----------------------------------------------------------------------------
U8	puts_lcd(U8 focus, char *s, U8 dp_tf){
	U8	i;		// temp

	if(focus == MAIN){
		i = mputs_lcd(s, dp_tf);
	}else{
		i = sputs_lcd(s, dp_tf);
	}
	return (i);
}

//-----------------------------------------------------------------------------
// mmem() displays main mem#.  Input is ASCII char.
//-----------------------------------------------------------------------------
void mmem(U8 mn){
	U8		i;		// temp
	char	mchr[2];

	if(mn >= (NUM_MEMS*2)) mn = 0;
	i = mn/NUM_MEMS;
	if(i) mchr[0] = '1';
	else mchr[0] = 0;
	mchr[1] = mem_ordinal[mn%NUM_MEMS];
	mchan_lcd(mchr);
	return;
}

//-----------------------------------------------------------------------------
// smem() displays sub mem#.  Input is ASCII.
//-----------------------------------------------------------------------------
void smem(U8 mn){
	U8		i;		// temp
	char	mchr[2];

	if(mn >= (NUM_MEMS*2)) mn = 0;
	i = mn/NUM_MEMS;
	if(i) mchr[0] = '1';
	else mchr[0] = 0;
	mchr[1] = mem_ordinal[mn%NUM_MEMS];
	schan_lcd(mchr);
	return;
}

//-----------------------------------------------------------------------------
// mtonea() sets/clears MAIN TONE annunc.  Input is T/F
// stonea() sets/clears SUB TONE annunc.  Input is T/F
//-----------------------------------------------------------------------------
void mtonea(U8 tf){

	if(tf){
		or_seg(MTENC, NOBLINK, MTENC_WORD);
	}else{
		clear_seg(MTENC, NOBLINK, MTENC_WORD);
	}
	lcd_send(get_blink());
	return;
}

void stonea(U8 tf){

	if(tf){
		or_seg(STENC, NOBLINK, STENC_WORD);
	}else{
		clear_seg(STENC, NOBLINK, STENC_WORD);
	}
	lcd_send(get_blink());
	return;
}

//-----------------------------------------------------------------------------
// mema() sets/clears MAIN/SUB mem annunc.  Input is M/S focus, T/F
//-----------------------------------------------------------------------------
void mema(U8 focus, U8 tf){

	if(focus == MAIN){
		if(tf){
			or_seg(MMEM, NOBLINK, MMEM_WORD);
		}else{
			clear_seg(MMEM, NOBLINK, MMEM_WORD);
		}
	}else{
		if(tf){
			or_seg(SMEM, NOBLINK, SMEM_WORD);
		}else{
			clear_seg(SMEM, NOBLINK, SMEM_WORD);
		}
	}
	lcd_send(get_blink());
	return;
}

//-----------------------------------------------------------------------------
// mskpa() sets/clears MAIN/SUB skip annunc.  Input is M/S focus, T/F
//-----------------------------------------------------------------------------
void mskpa(U8 focus, U8 tf){

	if(focus == MAIN){
		if(tf){
			or_seg(MSKP, NOBLINK, MSKP_WORD);
		}else{
			clear_seg(MSKP, NOBLINK, MSKP_WORD);
		}
	}else{
		if(tf){
			or_seg(SSKP, NOBLINK, SSKP_WORD);
		}else{
			clear_seg(SSKP, NOBLINK, SSKP_WORD);
		}
	}
	lcd_send(get_blink());
	return;
}

//-----------------------------------------------------------------------------
// mdupa() sets/clears MAIN DUP annunc.  Input is ASCII, S, +, -
// sdupa() sets/clears SUB DUP annunc.  Input is ASCII, S, +, -
//-----------------------------------------------------------------------------
void mdupa(char dplx){

	switch(dplx){
	default:
	case 'S':
		clear_seg(MDUP_MASK, NOBLINK, MDUP_WORD);		// turn off "DUP" & "-"
		break;

	case '+':
		clear_seg(MDUPM, NOBLINK, MDUP_WORD);			// turn off "-"
		or_seg(MDUPP, NOBLINK, MDUP_WORD);				// turn on "DUP"
		break;

	case '-':
		or_seg(MDUP_MASK, NOBLINK, MDUP_WORD);			// turn on "DUP" & "-"
		break;
	}
	lcd_send(get_blink());
	return;
}

void sdupa(char dplx){

	switch(dplx){
	default:
	case 'S':
		clear_seg(SDUP_MASK, NOBLINK, SDUP_WORD);		// turn off "DUP" & "-"
		break;

	case '+':
		clear_seg(SDUPM, NOBLINK, SDUP_WORD);			// turn off "-"
		or_seg(SDUPP, NOBLINK, SDUP_WORD);				// turn on "DUP"
		break;

	case '-':
		or_seg(SDUP_MASK, NOBLINK, SDUP_WORD);			// turn on "DUP" & "-"
		break;
	}
	lcd_send(get_blink());
	return;
}

//-----------------------------------------------------------------------------
// mdupa_blink() blinks/unblinks MAIN DUP annunc.  Input is tf
// sdupa_blink() blinks/unblinks SUB DUP annunc.  Input is tf
//-----------------------------------------------------------------------------
void mdupa_blink(U8 tf){

	if(tf){
		or_seg(MDUP_MASK, LBLINK, MDUP_WORD);			// blink "DUP" & "-"
	}else{
		clear_seg(MDUP_MASK, LBLINK, MDUP_WORD);		// unblink "DUP" & "-"
	}
	lcd_send(get_blink());
	return;
}

void sdupa_blink(U8 tf){

	if(tf){
		or_seg(SDUP_MASK, LBLINK, SDUP_WORD);			// blink "DUP" & "-"
	}else{
		clear_seg(SDUP_MASK, LBLINK, SDUP_WORD);		// unblink "DUP" & "-"
	}
	lcd_send(get_blink());
	return;
}

//-----------------------------------------------------------------------------
// mset_500hz() turns on 500 hz icon.  cmd = 0 is off, 1 is on, 2 is blink
// sset_500hz()
//-----------------------------------------------------------------------------
void mset_500hz(U8 cmd){

/*	switch(cmd){
	default:
	case 0:
		mblink_500hz(0);
		mon_500hz(0);
		break;

	case 1:
		mon_500hz(1);
		break;

	case 2:
		mon_500hz(1);
		mblink_500hz(1);
		break;
	}*/
	return;
}

void sset_500hz(U8 cmd){

/*	switch(cmd){
	default:
	case 0:
		sblink_500hz(0);
		son_500hz(0);
		break;

	case 1:
		son_500hz(1);
		break;

	case 2:
		son_500hz(1);
		sblink_500hz(1);
		break;
	}*/
	return;
}

//-----------------------------------------------------------------------------
// mon_500hz() turns on 500 hz icon.  Input is tf
// son_500hz()
//-----------------------------------------------------------------------------
void mon_500hz(U8 tf){

/*	lcd_buf[1] = LOAD_PTR | M00_ADDR;
	if(tf){
		lcd_buf[2] = WR_DMEM | M00;						// set 500hz icon
	}else{
		lcd_buf[2] = WR_DMEM;							// clear...
	}
	lcd_buf[0] = CS1_MASK | 2;
	put_spi(lcd_buf, CS_OPENCLOSE);*/
	return;
}

void son_500hz(U8 tf){

/*	lcd_buf[1] = LOAD_PTR | S00_ADDR;
	if(tf){
		lcd_buf[2] = WR_DMEM | S00;						// set 500hz icon
	}else{
		lcd_buf[2] = WR_DMEM;							// clear...
	}
	lcd_buf[0] = CS2_MASK | 2;
	put_spi(lcd_buf, CS_OPENCLOSE);*/
	return;
}

//-----------------------------------------------------------------------------
// mblink_500hz() turns on 500 hz icon.  Input is tf
// sblink_500hz()
//-----------------------------------------------------------------------------
void mblink_500hz(U8 tf){

/*	lcd_buf[1] = LOAD_PTR | M00_ADDR;
	if(tf){
		lcd_buf[2] = WR_BMEM | M00;						// blink 500hz icon
	}else{
		lcd_buf[2] = WR_BMEM;							// clear...
	}
	lcd_buf[0] = CS1_MASK | 2;
	put_spi(lcd_buf, CS_OPENCLOSE);*/
	return;
}

void sblink_500hz(U8 tf){

/*	lcd_buf[1] = LOAD_PTR | S00_ADDR;
	if(tf){
		lcd_buf[2] = WR_BMEM | S00;						// blink 500hz icon
	}else{
		lcd_buf[2] = WR_BMEM;							// clear...
	}
	lcd_buf[0] = CS2_MASK | 2;
	put_spi(lcd_buf, CS_OPENCLOSE);*/
	return;
}

//-----------------------------------------------------------------------------
// alow() sets/clears LOW annunc.  Input is TF
//-----------------------------------------------------------------------------
void alow(U8 tf){

	if(tf){
		or_seg(MLOW, NOBLINK, MLOW_WORD);				// LOW on
	}else{
		clear_seg(MLOW, NOBLINK, MLOW_WORD);			// LOW off
	}
	lcd_send(get_blink());
	return;
}

//-----------------------------------------------------------------------------
// alock() sets/clears LOCK annunc.  Input is TF
//-----------------------------------------------------------------------------
void alock(U8 tf){

	if(tf){
		or_seg(LOCKD, NOBLINK, LOCKD_WORD);				// LOCK on
	}else{
		clear_seg(LOCKD, NOBLINK, LOCKD_WORD);			// LOCK off
	}
	lcd_send(get_blink());
	return;
}

//-----------------------------------------------------------------------------
// amhz() sets/clears MHZ annunc.  Input is TF
//-----------------------------------------------------------------------------
void amhz(U8 tf){

	if(tf){
		or_seg(MHZD, NOBLINK, MHZD_WORD);				// MHz on
	}else{
		clear_seg(MHZD, NOBLINK, MHZD_WORD);			// MHz off
	}
	lcd_send(get_blink());
	return;
}

//-----------------------------------------------------------------------------
// abnd() sets/clears BAND annunc.  Input is TF
//-----------------------------------------------------------------------------
void abnd(U8 tf){

	if(tf){
		or_seg(BANDD, NOBLINK, BANDD_WORD);				// BAND on
	}else{
		clear_seg(BANDD, NOBLINK, BANDD_WORD);			// BAND off
	}
	lcd_send(get_blink());
	return;
}

//-----------------------------------------------------------------------------
// asub() sets/clears LOW annunc.  Input is TF
//-----------------------------------------------------------------------------
void asub(U8 tf){

	if(tf){
		or_seg(SUBD, NOBLINK, SUBD_WORD);				// SUB on
	}else{
		clear_seg(SUBD, NOBLINK, SUBD_WORD);			// SUB off
	}
	lcd_send(get_blink());
	return;
}

//-----------------------------------------------------------------------------
// ats() sets/clears TS annunc.  Input is TF
//-----------------------------------------------------------------------------
void ats(U8 tf){

	if(tf){
		or_seg(TSD, NOBLINK, TSD_WORD);					// TS on
	}else{
		clear_seg(TSD, NOBLINK, TSD_WORD);				// TS off
	}
	lcd_send(get_blink());
	return;
}

//-----------------------------------------------------------------------------
// bin32_bcdp() converts a U32 to a packed BCD value.
//-----------------------------------------------------------------------------
U32 bin32_bcdp(U32 bin32){
	U32	jj = 0;		// return temp
	U32	ii;			// temps
	U32	kk;
	U32	ll = bin32;

	for(ii=1000000L; ii!=0; ii/=10){
		jj <<= 4;
		kk = ll/ii;
		if(kk != 0){
			jj |= kk;
			ll = ll - (kk * ii);
		}
	}
	return jj;
}

//-----------------------------------------------------------------------------
// bin32_bcds() converts a U32 to an un-packed BCD string.
//-----------------------------------------------------------------------------
void bin32_bcds(U32 bin32, U8* sptr){
	U32	ii;			// temps
	U32	kk;
	U32	ll = bin32;

	for(ii=1000000L; ii!=0; ii/=10){
		kk = ll/ii;
		*sptr++ = kk;
		if(kk != 0){
			ll = ll - (kk * ii);
		}
	}
	return;
}

//-----------------------------------------------------------------------------
// bcds_bin32() converts an un-packed BCD string to U32 bin.
//-----------------------------------------------------------------------------
U32 bcds_bin32(U8* sptr){
	U32	ii;			// temps
	U32	kk = 0;
//	U32	ll = bin32;

	for(ii=1000000L; ii!=0; ii/=10){
		kk += (U32)(*sptr++) * ii;
	}
	return kk;
}

//-----------------------------------------------------------------------------
// add_bcds() adds n to unpacked BCD string at display addr
//-----------------------------------------------------------------------------
void add_bcds(U8* sptr, S8 adder, U8 addr, U8 max, U8 min){
	U8	i;			// string index

	if(adder > 9) adder = 0;							// error trap
	i = 6 - addr;
	sptr[i] += adder;
	if((sptr[i] > 0x7f) || (sptr[i] < min)){
		sptr[i] = sptr[i] + (max - min + 1);			// roll-under
	}
	if(sptr[i] > max){
		sptr[i] = sptr[i] - (max - min + 1);			// roll-over
	}
	return;
}

//*****************************************************************************
//
//		LED ANNUCIATOR Fns
//		While not LCD related, this is the best place for these FNs
//
//-----------------------------------------------------------------------------
// amtx() sets/clears the main TX LED.  Input is TF
//-----------------------------------------------------------------------------
void amtx(U8 tf){

	set_ledl(LED_TX, tf);
	return;
}

//-----------------------------------------------------------------------------
// amrx() sets/clears the main RX LED.  Input is TF
//-----------------------------------------------------------------------------
void amrx(U8 tf){

	set_ledl(LED_RXM, tf);								// main led = on/off
	return;
}

//-----------------------------------------------------------------------------
// asrx() sets/clears the sub RX LED.  Input is TF
//-----------------------------------------------------------------------------
void asrx(U8 tf){

	set_ledl(LED_RXS, tf);
	return;
}

//-----------------------------------------------------------------------------
// lamp_test() writes all "1"s to LCD segment memory and activates RX/TX LEDs
//	True-param sets test
//	false-param clears test
//-----------------------------------------------------------------------------
void lamp_test(U8 tf){
//	U8	i;
//	U8	j;
	U8	k;

	if(tf){
//		j = WR_DMEM | 0xf;
		set_ledl(LED_RXM | LED_RXS | LED_TX, 1);			// all on
	}else{
//		j = WR_DMEM;
		set_ledl(LED_RXM | LED_RXS | LED_TX, 0);			// all off
	}
	for(k=0; k<2; k++){
		wait2(10);
	}
	return;
}

//-----------------------------------------------------------------------------
// set_vfo_display() sets the vfo display signal
//-----------------------------------------------------------------------------
void set_vfo_display(U8	sig){

	vfo_display |= sig;
	return;
}

//-----------------------------------------------------------------------------
// rev_vfo() reverses the T/R on focused VFO
//-----------------------------------------------------------------------------
void rev_vfo(U8	focus){
	U8	i;		// temp

	if(inv_vfo(focus)){
		i = inv_duplex(focus);
		switch(i){
		default:
		case DPLX_S:
			i = 'S';
			break;

		case DPLX_M:
			i = '-';
			break;

		case DPLX_P:
			i = '+';
			break;
		}
		if(focus == MAIN) mdupa(i);
		else sdupa(i);
		vfo_change(focus);
		if(focus == MAIN) vfo_display |= MAIN;
		else vfo_display |= SUB_D;
	}
	return;
}

//-----------------------------------------------------------------------------
// force_push() sets system to push the VFO structure to HIB RAM if not check mode
//-----------------------------------------------------------------------------
void  force_push(void){

	if(!chkmode) force_push_radio();
	return;
}

//-----------------------------------------------------------------------------
// get_mutefl() returns the mute_mode contents
//-----------------------------------------------------------------------------
U8  get_mutefl(void){

	return mute_mode;
}

//-----------------------------------------------------------------------------
// togg_tsab() returns the mute_mode contents
//-----------------------------------------------------------------------------
void togg_tsab(U8 focus){

	if(read_dplx(focus) & TSA_F){
		set_ab(focus, 0);
		ats(0);
	}else{
		set_ab(focus, 1);
		ats(1);
	}
	force_push();											// force update to NVRAM
	return;
}

///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
// get_xmode() returns xmode[] value
//-----------------------------------------------------------------------------
U8 get_xmode(U8 b_id){

	return xmode[b_id];
}

///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
// write_xmode() writes the xmode state to the nv memory space
//-----------------------------------------------------------------------------
void write_xmode(U8 main){
	U32	addr;		// temps
	U8	i = get_band_index(main);

	addr = XMODET_0 + (i * sizeof(U8));
	rw8_nvr(addr, xmode[i], CS_WRITE|CS_OPENCLOSE);
	return;
}

///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
// read_xmode() reads the xmode state array from the nv memory space
//-----------------------------------------------------------------------------
void read_xmode(void){
	U32	addr;		// temps
	U8	i;
	U8	j;

	j = CS_OPEN;
	addr = XMODET_0;
	for(i=0; i<ID1200; i++){
		if(i == (ID1200 - 1)) j = CS_CLOSE;
		xmode[i] = rw8_nvr(addr, 0, CS_READ|j);
		j = CS_READ;
	}
	return;
}

///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
// clear_xmode() clears the xmode state to the nv memory space
//-----------------------------------------------------------------------------
void clear_xmode(void){
	U32	addr;		// temps
	U8	i;
	U8	j;

	j = CS_WRITE|CS_OPEN;
	addr = XMODET_0;
	for(i=0; i<ID1200; i++){
		if(i == (ID1200 - 1)) j = CS_WRITE|CS_CLOSE;
		xmode[i] = 0;
		rw8_nvr(addr, xmode[i], j);
		j = CS_WRITE;
	}
	return;
}

///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
// set_sys_err() transferrs sys err status to LCD module
//-----------------------------------------------------------------------------
void set_sys_err(U8 err){

	sys_err |= err;
	return;
}

///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
// clr_sys_err() transferrs sys err status to LCD module
//-----------------------------------------------------------------------------
void clr_sys_err(U8 err){

	sys_err &= ~err;
	return;
}

///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
// puts_slide() handles display of long text strings
//	focus is m/s
//	sptr is pointer to base string
//	cmd is 0xff to init, 0x01 to start, 0x00 to slide string 1 chr (left only)
//	return TRUE if entire slide sequence is complete
//-----------------------------------------------------------------------------
U8 puts_slide(U8 focus, char* sptr, U8 cmd){
	static U8		moffs_idx;		// offset start of display string, main
	static U8		soffs_idx;		// ... sub
		   U8*		offsptr;
		   char*	tptr;
		   U8		i = 0;			// temp
#define	MAX_SLIDE 18

	if(focus == MAIN){									// use pointers to diff main vs. sub
		offsptr = &moffs_idx;
		tptr = mfbuf;
	}else{
		offsptr = &soffs_idx;
		tptr = sfbuf;
	}
	switch(cmd){
	default:
	case 0xff:											// IPL init
		moffs_idx = 0;
		soffs_idx = 0;
		slide_time(0xff);
		break;

	case 0x01:											// start slide
		*offsptr = 0;
		copy6str(sptr, tptr, *offsptr);
		puts_lcd(focus, tptr, 0);
		break;

	case 0x00:											// run slide
		*offsptr += 1;
		i = copy6str(sptr, tptr, *offsptr);				// if i == true, end of slide is reached
		if(!i){
			puts_lcd(focus, tptr, 0);
		}
		if(*offsptr > MAX_SLIDE) *offsptr = 0;
		break;
	}
	return i;
}

///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
// copy6str() copies 6 chrs from sptr to dptr, plus a null at dptr
//	sptr is pointer to source string
//	dptr is pointer to dest string
//	return TRUE if source string is empty
//-----------------------------------------------------------------------------
U8 copy6str(char* sptr, char* dptr, U8 cidx){
	U8	i = 0;
	U8	j = cidx;
	U8	ccnt = 6;

	do{
		if(j++ < 6){
			*dptr++ = ' ';					// copy spaces to start
			*dptr = '\0';
		}else{
			*dptr = *(sptr+cidx-6);			// copy..
			if(!(*dptr++)) i = 1;				// if source empty, set true return
		}
	}while((*(++sptr)) && (--ccnt));		// up to 6 chrs
	dptr = '\0';
	return i;
}

///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
// doscan() set/clear the scan function
//-----------------------------------------------------------------------------
U8 doscan(U8 focus, U8 tf){

/*	if(tf & ((xmode[get_band_index(focus)] & MC_XFLAG) != MEM_XFLAG)){ // if not mem, exit with error
		return 0;
	}
	if(focus == MAIN){
		if(tf){
			xmodeq |= MSCANM_XFLAG;
			xmodez |= MSCANM1_XFLAG;
			get_mscan(focus);
		}else{
			xmodeq &= ~MSCANM_XFLAG;
		}
		lcd_buf[0] = CS1_MASK | 0x02;
		lcd_buf[1] = LOAD_PTR | MM_ADDR;
		if(tf){
			lcd_buf[2] = WR_BMEM | MM;
		}else{
			lcd_buf[2] = WR_BMEM;
		}
		put_spi(lcd_buf, CS_OPENCLOSE);
	}else{
		if(tf){
			xmodeq |= MSCANS_XFLAG;
			xmodez |= MSCANS1_XFLAG;
			get_mscan(focus);
		}else{
			xmodeq &= ~MSCANS_XFLAG;
		}
		lcd_buf[0] = CS2_MASK | 0x02;
		lcd_buf[1] = LOAD_PTR | SM_ADDR;
		if(tf){
			lcd_buf[2] = WR_BMEM | SM;
		}else{
			lcd_buf[2] = WR_BMEM;
		}
		put_spi(lcd_buf, CS_OPENCLOSE);
	}*/
	return 1;
}

///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
// nxtscan() steps to the next scannable mem
// SUB-scan will skip to the next band when it reaches the end of the mem list
//	The algorithm is not terribly simple:
//	  1) the band must be installed.  If not, skip it entirely (set_next_band() accomplishes this)
//	  2) Use the same algorithm as for the main-band until the end of the mem list is reached.
//	  3) If we end up back at the origin band and origin mem#, then there are no scanable memories (abort).
//	  4) the function needs to pass a signal that a band change was performed.
//
//	returns 0 if no enabled mems in band
//	returns 1 if new mem is ready
//	returns 0x81 if band-switch (sub only)
//-----------------------------------------------------------------------------
U8 nxtscan(U8 focus, U8 adder){
	U8	memstrt;
	U8	j;
	U8	breakout = 0;
	U8	scanen = 0;
	U8	bid;			// band id
	U8	bids;			// band id start
	U32	memask;

	memstrt = get_memnum(focus, 0);						// set start mem#
	memask = mem2bit(memstrt);
	bids = get_band_index(focus);						// set start band id
	if(focus == MAIN){
		j = memstrt;
		do{
			j += 1;										// advance mem#
			memask <<= 1;
			if(j >= MAX_MEM){
				j = 0;
				memask = 0x01L;
			}
			if(memask & mscan[bids]){
				scanen = TRUE;							// k is true if scan enabled
				set_memnum(bids, j);
			}

/*			j = get_memnum(focus, adder);				// get next memnum
//			eol = j & 0x80;								// isolate end of list..
			j &= 0x7f;									// isolate mem#
			if(mem2bit(j) & mscan[bids]){
				scanen = TRUE;							// k is true if scan enabled
			}*/
		}while((j != memstrt) && !scanen);
		if(j == memstrt) scanen = 0;					// if 1 or no channels enabled, "no scan"
	}else{
		bid = bids;										// set band id to start
		j = memstrt;									// set running mem# = start mem#
		do{
			j += 1;										// advance mem#
			memask <<= 1;
			if(j >= MAX_MEM){							// if end of list...
				j = 0;									// rollover mem# and mask
				memask = 0x01L;
				bid = set_next_band(focus);				// go to next available band
				if(((bid == bids) && (j == memstrt)) || (bid == BAND_ERROR)){ // if back to starting point or error, set exit
					breakout = 1;						// end of lookup
				}else{
					if(mscan[bid] == 0){				// if no channels enabled,
						j = MAX_MEM;					// no channels enabled, skip to next band
					}
				}
			}
			if(mem2bit(j) & mscan[bid]){
				scanen = 1;								// if scan enabled set end of lookup
			}
		}while(!scanen & !breakout);
		if(breakout) scanen = 0;
		else{
			if(scanen && (bid != bids)) scanen = 0x81; // band change exit
			set_memnum(bid, j);
		}
	}
	return scanen;
}

///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
// get_mscan() gathers scan enable bits for all mem locations.
//	main just does main band, sub does all bands
//-----------------------------------------------------------------------------
void get_mscan(U8 focus){
	U8	bid;
	U8	j;
	U8	k;
	U32	mask;

	if(focus == MAIN){								// process main band
		bid = get_band_index(focus);
		mask = 0x20000000L;							// set start at mem#29
		mscan[bid] = 0;
		j = MAX_MEM-1;
		do{
			k = get_scanen(bid, j);					// k is true if scan enabled
			if(k){
				mscan[bid] |= mask;
			}
			j--;
			mask >>= 1;
		}while(mask);
	}else{											// process all bands
		for(bid=0; bid<ID1200; bid++){
			mscan[bid] = 0;
			mask = 0x20000000L;						// set start at mem#30
			j = MAX_MEM-1;
			do{
				k = get_scanen(bid, j);				// k is true if scan enabled
				if(k){
					mscan[bid] |= mask;
				}
				j--;
				mask >>= 1;
			}while(mask);
		}
	}
	return;
}

///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
// mem2bit() returns bitmask for corresponding mem#
//-----------------------------------------------------------------------------
U32 mem2bit(U8 memnum){
	U32	ii = 0x01L;		// temp
	U8	i = memnum;

	while(i){
		ii <<= 1;
		i--;
	}
	return ii;
}

///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
// get_xmodeq() returns xmodeq
//-----------------------------------------------------------------------------
U8 get_xmodeq(void){

	return xmodeq;
}

///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
// get_xmodeq() returns xmodeq
//-----------------------------------------------------------------------------
void set_ptt(U8 pttstat){

	ptt_change = pttstat | PTT_EDGE;
	return;
}

///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
// set_slide() sets slide mode or toggles
//	0 = turn off, 1 = turn on, 0xff = toggle
//-----------------------------------------------------------------------------
void set_slide(U8 focus, U8 tf){

	// toggle mem string disp mode
	if(focus == MAIN){
		switch(tf){
		default:
		case 0:
			xmodeq &= ~TEXTM_SLIDE;
			break;

		case 1:
			xmodeq |= TEXTM_SLIDE;
			break;

		case 0xff:
			if(xmodeq & TEXTM_SLIDE) xmodeq &= ~TEXTM_SLIDE;
			else xmodeq |= TEXTM_SLIDE;
			break;
		}
		vfo_display |= MAIN;
	}else{
		switch(tf){
		default:
		case 0:
			xmodeq &= ~TEXTS_SLIDE;
			break;

		case 1:
			xmodeq |= TEXTS_SLIDE;
			break;

		case 0xff:
			if(xmodeq & TEXTS_SLIDE) xmodeq &= ~TEXTS_SLIDE;
			else xmodeq |= TEXTS_SLIDE;
			break;
		}
		vfo_display |= SUB_D;
	}
}

//-----------------------------------------------------------------------------
// mem2ordinal() returns ordinal of ASCII mem#.
//-----------------------------------------------------------------------------
U8 mem2ordinal(char cm){
	U8	i;		// temp

	i=0;
	while((i<sizeof(mem_ordinal)) && (cm != mem_ordinal[i])){
		i++;
	}
	if(i >= sizeof(mem_ordinal)) i = 0xff;				// err exit
	return i;
}

//-----------------------------------------------------------------------------
// ordinal2mem() returns ASCII mem# of ordinal.
//-----------------------------------------------------------------------------
char ordinal2mem(U8 memnum){

	if(memnum >= NUM_MEMS) return '!';					// err exit
	return mem_ordinal[memnum];
}

//-----------------------------------------------------------------------------
// lookup_pl() returns U8 CTCSS# from U16 frequency (PL tone * 10).
//	truncates 1's digit of both LUT and parameter.  Thus, the decimal portion
//	of the PL tone is not needed to get a match.
//	returns 0xff if tone not found
//-----------------------------------------------------------------------------
U8 lookup_pl(U16 ctcss){
	U8	i;	// temps
	U16	ii;

	ii = (ctcss / 10) * 10;								// truncate param
	i = 0;
	while((ii != (tone_list[i]/10)*10) && (i < sizeof(tone_list))){
		i++;
	}
	if(i >= sizeof(tone_list)) i = 0xff;
	return i;
}

//-----------------------------------------------------------------------------
// set_last_cos() stores bid and mem# in next location of last_cosx[] array.
//-----------------------------------------------------------------------------
void set_last_cos(U8 focus){
	U8	i;	// temps
	U8	j;

	i = get_memnum(focus, 0);									// construct composite of mem# and bid
	j = get_band_index(focus);
	i |= j << 5;
	if(focus == MAIN){
		last_cosm[last_cosm_hp] = i;							// store to next buffer position
		if(++last_cosm_hp >= MAX_COS_MEM) last_cosm_hp = 0;		// advance head pointer
		if(last_cosm_hp == last_cosm_tp){						// if h == t, advance tail (lose oldest data)
			if(++last_cosm_tp >= MAX_COS_MEM) last_cosm_tp = 0;
		}
	}else{
		last_coss[last_coss_hp] = i;							// store to next buffer position
		if(++last_coss_hp >= MAX_COS_MEM) last_coss_hp = 0;		// advance head pointer
		if(last_coss_hp == last_coss_tp){						// if h == t, advance tail (lose oldest data)
			if(++last_coss_tp >= MAX_COS_MEM) last_coss_tp = 0;
		}
	}
	return;
}

//-----------------------------------------------------------------------------
// get_last_cos() recalls bid and mem# from previous location of last_cosx[] array.
//	returns composite of bid/mem#.
//	A return value of 0xff means that there are no COS entries to recall.
//-----------------------------------------------------------------------------
U8 get_last_cos(U8 focus){
	U8	i = 0xff;	// temps

	if(focus == MAIN){
		if(last_cosm_hp != last_cosm_tp){
			if(--last_cosm_hp == 0xff) last_cosm_hp = MAX_COS_MEM - 1;
			i = last_cosm[last_cosm_hp];
		}
	}else{
		if(last_coss_hp != last_coss_tp){
			if(--last_coss_hp == 0xff) last_coss_hp = MAX_COS_MEM - 1;
			i = last_coss[last_coss_hp];
		}
	}
	return i;
}

//-----------------------------------------------------------------------------
// set_ledl() updates the LED on/off status using PWM settings
//-----------------------------------------------------------------------------
void set_ledl(U8 ledid, U8 ledon){

	if(ledon){
		led_status |= ledid;								// led = on
	}else{
		led_status &= ~ledid;								// led = off
	}
	return;
}
