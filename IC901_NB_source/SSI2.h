/********************************************************************
 ******* Changes COPYRIGHT (c) 2022 by KE0FF, Taylor, TX   **********
 *
 *  File name: SSI2.h
 *
 *  Module:    Control
 *
 *  Summary:
 *  This is the main file for the SSI2 driver (LCD controllers).
 *
 *  Project scope declarations revision history:
 *    08-25-15 jmh:  creation date
 *
 *******************************************************************/

/********************************************************************
 *  File scope declarations revision history:
 *    08-25-15 jmh:  creation date
 *
 *******************************************************************/
#define	LCD_MSG_LEN	7					// # bytes in LCD half-message
#define	LCD_MSG_MAX	(LCD_MSG_LEN * 4)	// # bytes in LCD burst
#define	CB_MAX	10						// circ buff max entries
#define	MS_MAX	4						// msg max words
#define	MSB_MAX	8						// msg max bytes
#define	NOBLINK	1						// LCD no-blink signal
#define	LBLINK	0						// LCD blink signal
#define	LCOPY	0x80					// copy LCD reg signal

void ssi2_init(void);
void lcd_send(uint8_t targ);
void set_seg(uint64_t* sptr, uint8_t targ);
void clear_seg(uint64_t srcval, uint8_t targ, uint8_t indx);
void or_seg(uint64_t srcval, uint8_t targ, uint8_t indx);
void get_seg(uint64_t* tptr, uint8_t src);
uint8_t get_blink(void);

void SSI2_ISR(void);
void Timer2B_ISR(void);
