/********************************************************************
 ******* Changes COPYRIGHT (c) 2022 by KE0FF, Taylor, TX   **********
 *
 *  File name: tm4c1294ncpdt_PCTL.h
 *
 *  Module:    Control
 *
 *  Summary:
 *  This is the TM4C1294NCPDT PCTL bitmap Definitions
 *
 *******************************************************************/

/********************************************************************
 *  File scope declarations revision history:
 *    03-09-22 jmh:  creation date
 *
 *******************************************************************/


#define	GPIO_PCTL_PA0_M			(15<<0)
#define	GPIO_PCTL_PA0_CAN0RX	(7<<0)
#define	GPIO_PCTL_PA0_I2C9SCL	(2<<0)
#define	GPIO_PCTL_PA0_T0CCP0	(3<<0)
#define	GPIO_PCTL_PA0_U0RX		(1<<0)

#define	GPIO_PCTL_PA1_M			(15<<4)
#define	GPIO_PCTL_PA1_CAN0TX	(7<<4)
#define	GPIO_PCTL_PA1_T0CCP1	(3<<4)
#define	GPIO_PCTL_PA1_I2C9SDA	(2<<4)
#define	GPIO_PCTL_PA1_U0TX		(1<<4)

#define	GPIO_PCTL_PA2_M			(15<<8)
#define	GPIO_PCTL_PA2_I2C8SCL	(2<<8)
#define	GPIO_PCTL_PA2_SSI0CLK	(15<<8)
#define	GPIO_PCTL_PA2_T1CCP0	(3<<8)
#define	GPIO_PCTL_PA2_U4RX		(1<<8)

#define	GPIO_PCTL_PA3_M			(15<<12)
#define	GPIO_PCTL_PA3_I2C8SDA	(2<<12)
#define	GPIO_PCTL_PA3_SSI0FSS	(15<<12)
#define	GPIO_PCTL_PA3_T1CCP1	(3<<12)
#define	GPIO_PCTL_PA3_U4TX		(1<<12)

#define	GPIO_PCTL_PA4_M			(15<<16)
#define	GPIO_PCTL_PA4_I2C7SCL	(2<<16)
#define	GPIO_PCTL_PA4_SSI0XDAT0	(15<<16)
#define	GPIO_PCTL_PA4_T2CCP0	(3<<16)
#define	GPIO_PCTL_PA4_U3RX		(1<<16)

#define	GPIO_PCTL_PA5_M			(15<<20)
#define	GPIO_PCTL_PA5_I2C7SDA	(2<<20)
#define	GPIO_PCTL_PA5_SSI0XDAT1	(15<<20)
#define	GPIO_PCTL_PA5_T2CCP1	(3<<20)
#define	GPIO_PCTL_PA5_U3TX		(1<<20)

#define	GPIO_PCTL_PA6_M			(15<<24)
#define	GPIO_PCTL_PA6_EPI0S8	(15<<24)
#define	GPIO_PCTL_PA6_I2C6SCL	(2<<24)
#define	GPIO_PCTL_PA6_SSI0XDAT2	(13<<24)
#define	GPIO_PCTL_PA6_T3CCP0	(3<<24)
#define	GPIO_PCTL_PA6_U2RX		(1<<24)
#define	GPIO_PCTL_PA6_USB0EPEN	(5<<24)

#define	GPIO_PCTL_PA7_M			(15<<28)
#define	GPIO_PCTL_PA7_EPI0S9	(15<<28)
#define	GPIO_PCTL_PA7_I2C6SDA	(2<<28)
#define	GPIO_PCTL_PA7_SSI0XDAT3	(13<<28)
#define	GPIO_PCTL_PA7_T3CCP1	(3<<28)
#define	GPIO_PCTL_PA7_U2TX		(1<<28)
#define	GPIO_PCTL_PA7_USB0EPEN	(11<<28)
#define	GPIO_PCTL_PA7_USB0PFLT	(5<<28)

#define	GPIO_PCTL_PB0_M			(15<<0)
#define	GPIO_PCTL_PB0_CAN1RX	(7<<0)
#define	GPIO_PCTL_PB0_I2C5SCL	(2<<0)
#define	GPIO_PCTL_PB0_T4CCP0	(3<<0)
#define	GPIO_PCTL_PB0_U1RX		(1<<0)

#define	GPIO_PCTL_PB1_M			(15<<4)
#define	GPIO_PCTL_PB1_CAN1TX	(7<<4)
#define	GPIO_PCTL_PB1_I2C5SDA	(2<<4)
#define	GPIO_PCTL_PB1_T4CCP1	(3<<4)
#define	GPIO_PCTL_PB1_U1TX		(1<<4)

#define	GPIO_PCTL_PB2_M			(15<<8)
#define	GPIO_PCTL_PB2_EPI0S27	(15<<8)
#define	GPIO_PCTL_PB2_I2C0SCL	(2<<8)
#define	GPIO_PCTL_PB2_T5CCP0	(3<<8)
#define	GPIO_PCTL_PB2_USB0STP	(14<<8)

#define	GPIO_PCTL_PB3_M			(15<<12)
#define	GPIO_PCTL_PB3_EPI0S28	(15<<12)
#define	GPIO_PCTL_PB3_I2C0SDA	(2<<12)
#define	GPIO_PCTL_PB3_T5CCP1	(3<<12)
#define	GPIO_PCTL_PB3_USB0CLK	(14<<12)

#define	GPIO_PCTL_PB4_M			(15<<16)
#define	GPIO_PCTL_PB4_I2C5SCL	(2<<16)
#define	GPIO_PCTL_PB4_SSI1FSS	(15<<16)
#define	GPIO_PCTL_PB4_U0CTS		(1<<16)

#define	GPIO_PCTL_PB5_M			(15<<20)
#define	GPIO_PCTL_PB5_I2C5SDA	(2<<20)
#define	GPIO_PCTL_PB5_SSI1CLK	(15<<20)
#define	GPIO_PCTL_PB5_U0RTS		(1<<20)

//#define	GPIO_PCTL_PC0_M			(15<<0)
//#define	GPIO_PCTL_PC0_TCK		(1<<0)
//#define	GPIO_PCTL_PC1_TMS		(1<<4)
//#define	GPIO_PCTL_PC2_TDI		(1<<8)
//#define	GPIO_PCTL_PC3_SWO		(2<<12)
//#define	GPIO_PCTL_PC3_TDO		(1<<12)

#define	GPIO_PCTL_PC4_M			(15<<16)
#define	GPIO_PCTL_PC4_EPI0S7	(15<<16)
#define	GPIO_PCTL_PC4_U7RX		(1<<16)

#define	GPIO_PCTL_PC5_M			(15<<20)
#define	GPIO_PCTL_PC5_EPI0S6	(15<<20)
#define	GPIO_PCTL_PC5_RTCCLK	(7<<20)
#define	GPIO_PCTL_PC5_U7TX		(1<<20)

#define	GPIO_PCTL_PC6_M			(15<<24)
#define	GPIO_PCTL_PC6_EPI0S5	(15<<24)
#define	GPIO_PCTL_PC6_U5RX		(1<<24)

#define	GPIO_PCTL_PC7_M			(15<<28)
#define	GPIO_PCTL_PC7_EPI0S4	(15<<28)
#define	GPIO_PCTL_PC7_U5TX		(1<<28)

#define	GPIO_PCTL_PD0_M			(15<<0)
#define	GPIO_PCTL_PD0_C0O		(5<<0)
#define	GPIO_PCTL_PD0_I2C7SCL	(2<<0)
#define	GPIO_PCTL_PD0_SSI2XDAT1	(15<<0)
#define	GPIO_PCTL_PD0_T0CCP0	(3<<0)

#define	GPIO_PCTL_PD1_M			(15<<4)
#define	GPIO_PCTL_PD1_C1O		(5<<4)
#define	GPIO_PCTL_PD1_I2C7SDA	(2<<4)
#define	GPIO_PCTL_PD1_SSI2XDAT0	(15<<4)
#define	GPIO_PCTL_PD1_T0CCP1	(3<<4)

#define	GPIO_PCTL_PD2_M			(15<<8)
#define	GPIO_PCTL_PD2_C2O		(5<<8)
#define	GPIO_PCTL_PD2_I2C8SCL	(2<<8)
#define	GPIO_PCTL_PD2_SSI2FSS	(15<<8)
#define	GPIO_PCTL_PD2_T1CCP0	(3<<8)

#define	GPIO_PCTL_PD3_M			(15<<12)
#define	GPIO_PCTL_PD3_I2C8SDA	(2<<12)
#define	GPIO_PCTL_PD3_SSI2CLK	(15<<12)
#define	GPIO_PCTL_PD3_T1CCP1	(3<<12)

#define	GPIO_PCTL_PD4_M			(15<<16)
#define	GPIO_PCTL_PD4_SSI1XDAT2	(15<<16)
#define	GPIO_PCTL_PD4_T3CCP0	(3<<16)
#define	GPIO_PCTL_PD4_U2RX		(1<<16)

#define	GPIO_PCTL_PD5_M			(15<<20)
#define	GPIO_PCTL_PD5_SSI1XDAT3	(15<<20)
#define	GPIO_PCTL_PD5_T3CCP1	(3<<20)
#define	GPIO_PCTL_PD5_U2TX		(1<<20)

#define	GPIO_PCTL_PD6_M			(15<<24)
#define	GPIO_PCTL_PD6_SSI2XDAT3	(15<<24)
#define	GPIO_PCTL_PD6_T4CCP0	(3<<24)
#define	GPIO_PCTL_PD6_U2RTS		(1<<24)
#define	GPIO_PCTL_PD6_USB0EPEN	(5<<24)

#define	GPIO_PCTL_PD7_M			(15<<28)
#define	GPIO_PCTL_PD7_NMI		(8<<28)
#define	GPIO_PCTL_PD7_SSI2XDAT2	(15<<28)
#define	GPIO_PCTL_PD7_T4CCP1	(3<<28)
#define	GPIO_PCTL_PD7_U2CTS		(1<<28)
#define	GPIO_PCTL_PD7_USB0PFLT	(5<<28)

#define	GPIO_PCTL_PE0_M			(15<<0)
#define	GPIO_PCTL_PE0_U1RTS		(1<<0)

#define	GPIO_PCTL_PE1_M			(15<<4)
#define	GPIO_PCTL_PE1_U1DSR		(1<<4)

#define	GPIO_PCTL_PE2_M			(15<<8)
#define	GPIO_PCTL_PE2_U1DCD		(1<<8)

#define	GPIO_PCTL_PE3_M			(15<<12)
#define	GPIO_PCTL_PE3_U1DTR		(1<<12)

#define	GPIO_PCTL_PE4_M			(15<<16)
#define	GPIO_PCTL_PE4_SSI1XDAT0	(15<<16)
#define	GPIO_PCTL_PE4_U1RI		(1<<16)

#define	GPIO_PCTL_PE5_M			(15<<20)
#define	GPIO_PCTL_PE5_SSI1XDAT1	(15<<20)

#define	GPIO_PCTL_PF0_M			(15<<0)
#define	GPIO_PCTL_PF0_EN0LED0	(5<<0)
#define	GPIO_PCTL_PF0_M0PWM0	(6<<0)
#define	GPIO_PCTL_PF0_SSI3XDAT1	(14<<0)
#define	GPIO_PCTL_PF0_TRD2		(15<<0)

#define	GPIO_PCTL_PF1_M			(15<<4)
#define	GPIO_PCTL_PF1_EN0LED2	(5<<4)
#define	GPIO_PCTL_PF1_M0PWM1	(6<<4)
#define	GPIO_PCTL_PF1_SSI3XDAT0	(14<<4)
#define	GPIO_PCTL_PF1_TRD1		(15<<4)

#define	GPIO_PCTL_PF2_M			(15<<8)
#define	GPIO_PCTL_PF2_M0PWM2	(6<<8)
#define	GPIO_PCTL_PF2_SSI3FSS	(14<<8)
#define	GPIO_PCTL_PF2_TRD0		(15<<8)

#define	GPIO_PCTL_PF3_M			(15<<12)
#define	GPIO_PCTL_PF3_M0PWM3	(6<<12)
#define	GPIO_PCTL_PF3_SSI3CLK	(14<<12)
#define	GPIO_PCTL_PF3_TRCLK		(15<<12)

#define	GPIO_PCTL_PF4_M			(15<<16)
#define	GPIO_PCTL_PF4_EN0LED1	(5<<16)
#define	GPIO_PCTL_PF4_M0FAULT0	(6<<16)
#define	GPIO_PCTL_PF4_SSI3XDAT2	(14<<16)
#define	GPIO_PCTL_PF4_TRD3		(15<<16)

#define	GPIO_PCTL_PG0_M			(15<<0)
#define	GPIO_PCTL_PG0_EN0PPS	(5<<0)
#define	GPIO_PCTL_PG0_EPI0S11	(15<<0)
#define	GPIO_PCTL_PG0_I2C1SCL	(2<<0)
#define	GPIO_PCTL_PG0_M0PWM4	(6<<0)

#define	GPIO_PCTL_PG1_M			(15<<4)
#define	GPIO_PCTL_PG1_EPI0S10	(15<<4)
#define	GPIO_PCTL_PG1_I2C1SDA	(2<<4)
#define	GPIO_PCTL_PG1_M0PWM5	(6<<4)

#define	GPIO_PCTL_PH0_M			(15<<0)
#define	GPIO_PCTL_PH0_EPI0S0	(15<<0)
#define	GPIO_PCTL_PH0_U0RTS		(1<<0)

#define	GPIO_PCTL_PH1_M			(15<<4)
#define	GPIO_PCTL_PH1_EPI0S1	(15<<4)
#define	GPIO_PCTL_PH1_U0CTS		(1<<4)

#define	GPIO_PCTL_PH2_M			(15<<8)
#define	GPIO_PCTL_PH2_EPI0S2	(15<<8)
#define	GPIO_PCTL_PH2_U0DCD		(1<<8)

#define	GPIO_PCTL_PH3_M			(15<<12)
#define	GPIO_PCTL_PH3_EPI0S3	(15<<12)
#define	GPIO_PCTL_PH3_U0DSR		(1<<12)

#define	GPIO_PCTL_PJ0_M			(15<<0)
#define	GPIO_PCTL_PJ0_EN0PPS	(5<<0)
#define	GPIO_PCTL_PJ0_U3RX		(1<<0)

#define	GPIO_PCTL_PJ1_M			(15<<4)
#define	GPIO_PCTL_PJ1_U3TX		(1<<4)

#define	GPIO_PCTL_PK0_M			(15<<0)
#define	GPIO_PCTL_PK0_EPI0S0	(15<<0)
#define	GPIO_PCTL_PK0_U4RX		(1<<0)

#define	GPIO_PCTL_PK1_M			(15<<4)
#define	GPIO_PCTL_PK1_EPI0S1	(15<<4)
#define	GPIO_PCTL_PK1_U4TX		(1<<4)

#define	GPIO_PCTL_PK2_M			(15<<8)
#define	GPIO_PCTL_PK2_EPI0S2	(15<<8)
#define	GPIO_PCTL_PK2_U4RTS		(1<<8)

#define	GPIO_PCTL_PK3_M			(15<<12)
#define	GPIO_PCTL_PK3_EPI0S3	(15<<12)
#define	GPIO_PCTL_PK3_U4CTS		(1<<12)

#define	GPIO_PCTL_PK4_M			(15<<16)
#define	GPIO_PCTL_PK4_EN0LED0	(5<<16)
#define	GPIO_PCTL_PK4_EPI0S32	(15<<16)
#define	GPIO_PCTL_PK4_I2C3SCL	(2<<16)
#define	GPIO_PCTL_PK4_M0PWM6	(6<<16)

#define	GPIO_PCTL_PK5_M			(15<<20)
#define	GPIO_PCTL_PK5_EN0LED2	(5<<20)
#define	GPIO_PCTL_PK5_EPI0S31	(15<<20)
#define	GPIO_PCTL_PK5_I2C3SDA	(2<<20)
#define	GPIO_PCTL_PK5_M0PWM7	(6<<20)

#define	GPIO_PCTL_PK6_M			(15<<24)
#define	GPIO_PCTL_PK6_EN0LED1	(5<<24)
#define	GPIO_PCTL_PK6_EPI0S25	(15<<24)
#define	GPIO_PCTL_PK6_I2C4SCL	(2<<24)
#define	GPIO_PCTL_PK6_M0FAULT1	(6<<24)

#define	GPIO_PCTL_PK7_M			(15<<28)
#define	GPIO_PCTL_PK7_EPI0S24	(15<<28)
#define	GPIO_PCTL_PK7_I2C4SDA	(2<<28)
#define	GPIO_PCTL_PK7_M0FAULT2	(6<<28)
#define	GPIO_PCTL_PK7_RTCCLK	(5<<28)
#define	GPIO_PCTL_PK7_U0RI		(1<<28)

#define	GPIO_PCTL_PL0_M			(15<<0)
#define	GPIO_PCTL_PL0_EPI0S16	(15<<0)
#define	GPIO_PCTL_PL0_I2C2SDA	(2<<0)
#define	GPIO_PCTL_PL0_M0FAULT3	(6<<0)
#define	GPIO_PCTL_PL0_USB0D0	(14<<0)

#define	GPIO_PCTL_PL1_M			(15<<4)
#define	GPIO_PCTL_PL1_EPI0S17	(15<<4)
#define	GPIO_PCTL_PL1_I2C2SCL	(2<<4)
#define	GPIO_PCTL_PL1_PhA0		(6<<4)
#define	GPIO_PCTL_PL1_USB0D1	(14<<4

#define	GPIO_PCTL_PL2_M			(15<<8)
#define	GPIO_PCTL_PL2_C0O		(5<<8)
#define	GPIO_PCTL_PL2_EPI0S18	(15<<8)
#define	GPIO_PCTL_PL2_PhB0		(6<<8)
#define	GPIO_PCTL_PL2_USB0D2	(14<<8)

#define	GPIO_PCTL_PL3_M			(15<<12)
#define	GPIO_PCTL_PL3_C1O		(5<<12)
#define	GPIO_PCTL_PL3_EPI0S19	(15<<12)
#define	GPIO_PCTL_PL3_IDX0		(6<<12)
#define	GPIO_PCTL_PL3_USB0D3	(14<<12)

#define	GPIO_PCTL_PL4_M			(15<<16)
#define	GPIO_PCTL_PL4_EPI0S26	(15<<16)
#define	GPIO_PCTL_PL4_T0CCP0	(3<<16)
#define	GPIO_PCTL_PL4_USB0D4	(14<<16)

#define	GPIO_PCTL_PL5_M			(15<<20)
#define	GPIO_PCTL_PL5_EPI0S33	(15<<20)
#define	GPIO_PCTL_PL5_T0CCP1	(3<<20)
#define	GPIO_PCTL_PL5_USB0D5	(14<<20)

#define	GPIO_PCTL_PL6_M			(15<<24)
#define	GPIO_PCTL_PL6_T1CCP0	(3<<24)

#define	GPIO_PCTL_PL7_M			(15<<28)
#define	GPIO_PCTL_PL7_T1CCP1	(3<<28)

#define	GPIO_PCTL_PM0_M			(15<<0)
#define	GPIO_PCTL_PM0_EPI0S15	(15<<0)
#define	GPIO_PCTL_PM0_T2CCP0	(3<<0)

#define	GPIO_PCTL_PM1_M			(15<<4)
#define	GPIO_PCTL_PM1_EPI0S14	(15<<4)
#define	GPIO_PCTL_PM1_T2CCP1	(3<<4)

#define	GPIO_PCTL_PM2_M			(15<<8)
#define	GPIO_PCTL_PM2_EPI0S13	(15<<8)
#define	GPIO_PCTL_PM2_T3CCP0	(3<<8)

#define	GPIO_PCTL_PM3_M			(15<<12)
#define	GPIO_PCTL_PM3_EPI0S12	(15<<12)
#define	GPIO_PCTL_PM3_T3CCP1	(3<<12)

#define	GPIO_PCTL_PM4_M			(15<<16)
#define	GPIO_PCTL_PM4_T4CCP0	(3<<16)
#define	GPIO_PCTL_PM4_U0CTS		(1<<16)

#define	GPIO_PCTL_PM5_M			(15<<20)
#define	GPIO_PCTL_PM5_T4CCP1	(3<<20)
#define	GPIO_PCTL_PM5_U0DCD		(1<<20)

#define	GPIO_PCTL_PM6_M			(15<<24)
#define	GPIO_PCTL_PM6_T5CCP0	(3<<24)
#define	GPIO_PCTL_PM6_U0DSR		(1<<24)

#define	GPIO_PCTL_PM7_M			(15<<28)
#define	GPIO_PCTL_PM7_T5CCP1	(3<<28)
#define	GPIO_PCTL_PM7_U0RI		(1<<28)

#define	GPIO_PCTL_PN0_M			(15<<0)
#define	GPIO_PCTL_PN0_U1RTS		(1<<0)

#define	GPIO_PCTL_PN1_M			(15<<4)
#define	GPIO_PCTL_PN1_U1CTS		(1<<4)

#define	GPIO_PCTL_PN2_M			(15<<8)
#define	GPIO_PCTL_PN2_EPI0S29	(15<<8)
#define	GPIO_PCTL_PN2_U1DCD		(1<<8)
#define	GPIO_PCTL_PN2_U2RTS		(2<<8)

#define	GPIO_PCTL_PN3_M			(15<<12)
#define	GPIO_PCTL_PN3_EPI0S30	(15<<12)
#define	GPIO_PCTL_PN3_U1DSR		(1<<12)
#define	GPIO_PCTL_PN3_U2CTS		(2<<12)

#define	GPIO_PCTL_PN4_M			(15<<16)
#define	GPIO_PCTL_PN4_EPI0S34	(15<<16)
#define	GPIO_PCTL_PN4_I2C2SDA	(3<<16)
#define	GPIO_PCTL_PN4_U1DTR		(1<<16)
#define	GPIO_PCTL_PN4_U3RTS		(2<<16)

#define	GPIO_PCTL_PN5_M			(15<<20)
#define	GPIO_PCTL_PN5_EPI0S35	(15<<20)
#define	GPIO_PCTL_PN5_I2C2SCL	(3<<20)
#define	GPIO_PCTL_PN5_U1RI		(1<<20)
#define	GPIO_PCTL_PN5_U3CTS		(2<<20)

#define	GPIO_PCTL_PP0_M			(15<<0)
#define	GPIO_PCTL_PP0_SSI3XDAT2	(15<<0)
#define	GPIO_PCTL_PP0_U6RX		(1<<0)

#define	GPIO_PCTL_PP1_M			(15<<4)
#define	GPIO_PCTL_PP1_SSI3XDAT3	(15<<4)
#define	GPIO_PCTL_PP1_U6TX		(1<<4)

#define	GPIO_PCTL_PP2_M			(15<<8)
#define	GPIO_PCTL_PP2_EPI0S29	(15<<8)
#define	GPIO_PCTL_PP2_U0DTR		(1<<8)
#define	GPIO_PCTL_PP2_USB0NXT	(14<<8)

#define	GPIO_PCTL_PP3_M			(15<<12)
#define	GPIO_PCTL_PP3_EPI0S30	(15<<12)
#define	GPIO_PCTL_PP3_RTCCLK	(7<<12)
#define	GPIO_PCTL_PP3_U0DCD		(2<<12)
#define	GPIO_PCTL_PP3_U1CTS		(1<<12)
#define	GPIO_PCTL_PP3_USB0DIR	(14<<12)

#define	GPIO_PCTL_PP4_M			(15<<16)
#define	GPIO_PCTL_PP4_U0DSR		(2<<16)
#define	GPIO_PCTL_PP4_U3RTS		(1<<16)
#define	GPIO_PCTL_PP4_USB0D7	(14<<16)

#define	GPIO_PCTL_PP5_M			(15<<20)
#define	GPIO_PCTL_PP5_I2C2SCL	(2<<20)
#define	GPIO_PCTL_PP5_U3CTS		(1<<20)
#define	GPIO_PCTL_PP5_USB0D6	(14<<20)

#define	GPIO_PCTL_PQ0_M			(15<<0)
#define	GPIO_PCTL_PQ0_EPI0S20	(15<<0)
#define	GPIO_PCTL_PQ0_SSI3CLK	(14<<0)

#define	GPIO_PCTL_PQ1_M			(15<<0)
#define	GPIO_PCTL_PQ1_EPI0S21	(15<<4)
#define	GPIO_PCTL_PQ1_SSI3FSS	(14<<4)

#define	GPIO_PCTL_PQ2_M			(15<<8)
#define	GPIO_PCTL_PQ2_EPI0S22	(15<<8)
#define	GPIO_PCTL_PQ2_SSI3XDAT0	(14<<8)

#define	GPIO_PCTL_PQ3_M			(15<<12)
#define	GPIO_PCTL_PQ3_EPI0S23	(15<<12)
#define	GPIO_PCTL_PQ3_SSI3XDAT1	(14<<12)

#define	GPIO_PCTL_PQ4_M			(15<<16)
#define	GPIO_PCTL_PQ4_DIVSCLK	(7<<16)
#define	GPIO_PCTL_PQ4_U1RX		(1<<16)
