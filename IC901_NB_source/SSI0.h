/********************************************************************
 ******* Changes COPYRIGHT (c) 2022 by KE0FF, Taylor, TX   **********
 *
 *  File name: SSI0.c
 *
 *  Module:    Control
 *
 *  Summary:
 *  This is the main file for the SSI0 driver (NVSRAM memory).
 *
 *******************************************************************/

/********************************************************************
 *  File scope declarations revision history:
 *    03-09-22 jmh:  creation date
 *
 *******************************************************************/


// putspi defines
#define	CS_OPEN			1
#define	CS_CLOSE		2
#define	CS_OPENCLOSE	(CS_OPEN | CS_CLOSE)
#define	CS_IDLE			0
#define	CS_READ			0
#define	CS_WRITE		0x80

// ANV32AA1A NVRAM defines
#define	WREN		0x06				// write enable command (1 byte)
#define	WRDI		0x04				// write disable command (1 byte)
// STATUS bitmasks
#define	RDY_N		0x01				// /ready bit
#define	WEN			0x02				// write enable
#define	BP0			0x04				// block protect 0		BP[00] = none protected		BP[01] = 0x18000 - 0x1ffff
#define	BP1			0x08				// block protect 1		BP[10] = 0x10000 - 0x1ffff	BP[11] = 0x00000 - 0x1ffff
#define	SWM_N		0x10				// secure write result
#define	PDIS		0x40				// power store disable

#define	RDSR		0x05				// read status command (1 byte command, followed by 1 byte read)
#define	WRSR		0x01				// write status  (1 byte command, followed by 1 byte write)
#define	READ		0x03				// read memory (1 byte cmd, 3 byte addr write, 1 byte data read .. continuous reads return next memory address)
#define	SREAD		0x13				// secure read memory (see datasheet)
#define	WRITE		0x02				// write memory (1 byte cmd, 3 byte addr write, 1 byte data write .. continuous writes go to next memory address)
#define	SWRITE		0x12				// secure write memory (see datasheet)
#define	STORE		0x08				// store memory to NV space command (1 byte)
#define	RECALL		0x09				// recall memory from NV space command (1 byte)
#define	RDSNR		0xC3				// read user sernum command (1 byte, 2 bytes read)
#define	WRSNR		0xC2				// write user sernum command (1 byte, 2 bytes write)
#define	NVHIBR		0xB9				// NVRAM hibernate command (1 byte)

//-----------------------------------------------------------------------------
// Global Fns
//-----------------------------------------------------------------------------

void ssi0_init(void);
uint8_t shift_SSI0(uint8_t sdata);
uint8_t read_SSI0(uint8_t dataw);
// NVRAM Fns
void close_nvr(void);
void wen_nvr(void);
void storecall_nvr(uint8_t tf_fl);
uint8_t rws_nvr(uint8_t dataw, uint8_t mode);
uint8_t rw8_nvr(uint32_t addr, uint8_t dataw, uint8_t mode);
uint16_t rw16_nvr(uint32_t addr, uint16_t dataw, uint8_t mode);
uint32_t rw32_nvr(uint32_t addr, uint32_t dataw, uint8_t mode);
void rwusn_nvr(uint8_t* dptr, uint8_t mode);
void Timer1B_ISR(void);

void write_sram_cmd(uint8_t fcmd, uint8_t fdata);
uint8_t read_sram_cmd(uint8_t fcmd);
