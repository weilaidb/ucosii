/*
*********************************************************************************************************
*                                     MICIRUM BOARD SUPPORT PACKAGE
*
*                             (c) Copyright 2013; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                        BOARD SUPPORT PACKAGE
*
*                                           NXP LPC824Lite
*                                          Evaluation Board
*
* Filename      : bsp.h
* Version       : V1.00
* Programmer(s) : JPB
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                                 MODULE
*
* Note(s) : (1) This header file is protected from multiple pre-processor inclusion through use of the
*               BSP present pre-processor macro definition.
*********************************************************************************************************
*/

#ifndef  BSP_PRESENT
#define  BSP_PRESENT


/*
*********************************************************************************************************
*                                                 EXTERNS
*********************************************************************************************************
*/

#ifdef   BSP_MODULE
#define  BSP_EXT
#else
#define  BSP_EXT  extern
#endif


/*
*********************************************************************************************************
*                                              INCLUDE FILES
*********************************************************************************************************
*/

#include  <stdarg.h>
#include  <stdio.h>

#include  <cpu.h>
#include  <cpu_core.h>

#include  <lib_ascii.h>
#include  <lib_def.h>
#include  <lib_mem.h>
#include  <lib_str.h>

#include  <bsp_os.h>
#include  <app_cfg.h>
#include  <bsp_cfg.h>
#if (APP_CFG_SERIAL_EN == DEF_ENABLED)
#include  <bsp_ser.h>
#endif
#include  <bsp_i2c.h>
#include  <bsp_at24.h>

#include  <chip.h>

/*
*********************************************************************************************************
*                                           NAME DEFINITIONS
*********************************************************************************************************
*/

#define BSP_LED1                    0
#define BSP_LED2                    1
#define BSP_LED3                    2
#define BSP_LED4                    3
#define BSP_LED5                    4
#define BSP_LED6                    5
#define BSP_LED7                    6
#define BSP_LED8                    7
#define BSP_LED_CNT                 8

#define BSP_KEY_CNT                 3

#define BSP_CONSOLE                 LPC_USART1

/** Define the frequency in Hz, of the main oscillator (from Xtal) 
 * Note that it only takes effect if main oscillator is selected as clock source
 */
#define MAIN_OSC_XTAL_FREQ_HZ		12000000
/** Define the frequency in Hz, of the external clock input.
 * Note that it only takes effect if external clock is selected as clock source
 */
#define EXT_CLOCK_IN_FREQ_HZ		0

#define BSP_AT24_I2C_ID                 BSP_I2C_ID_I2C0
#define BSP_AT24_I2C_ADDR               ((CPU_INT08U)0x50)

/*
*********************************************************************************************************
*                                               INT DEFINES
*********************************************************************************************************
*/

#define  BSP_INT_ID_SPI0                                  0u    /* SPI 0 controller                                     */
#define  BSP_INT_ID_SPI1                                  1u    /* SPI 1 controller                                     */
#define  BSP_INT_ID_RSVD2                                 2u
#define  BSP_INT_ID_UART0                                 3u    /* UART0 interrupt                                      */
#define  BSP_INT_ID_UART1                                 4u    /* UART1 interrupt                                      */
#define  BSP_INT_ID_UART2                                 5u    /* UART2 interrupt                                      */
#define  BSP_INT_ID_RSVD6                                 6u
#define  BSP_INT_ID_I2C1                                  7u    /* I2C1 interrupt                                       */
#define  BSP_INT_ID_I2C0                                  8u    /* I2C0 interrupt                                       */
#define  BSP_INT_ID_SCT                                   9u    /* Smart Counter Timer                                  */
#define  BSP_INT_ID_MRT                                  10u    /* Multi-Rate Timer                                     */
#define  BSP_INT_ID_CMP                                  11u    /* Comparator                                           */
#define  BSP_INT_ID_WDT                                  12u    /* WatchDog timer                                       */
#define  BSP_INT_ID_BOD                                  13u    /* Brown Out Detect                                     */
#define  BSP_INT_ID_FLASH                                14u    /* Non-Volatile Memory Controller                       */
#define  BSP_INT_ID_WKT                                  15u    /* Wakeup timer                                         */
#define  BSP_INT_ID_ADC_SEQA                             16u    /* ADC Sequence A Completion [Only on LPC82X]           */
#define  BSP_INT_ID_ADC_SEQB                             17u    /* ADC Sequence B Completion [Only on LPC82X]           */
#define  BSP_INT_ID_ADC_THCMP                            18u    /* ADC Threshold compare [Only on LPC82X]               */
#define  BSP_INT_ID_ADC_OVR                              19u    /* ADC Overrun [Only on LPC82X]                         */
#define  BSP_INT_ID_DMA                                  20u    /* DMA Controller [Only on LPC82X]                      */
#define  BSP_INT_ID_I2C2                                 21u    /* I2C2 Controller [Only on LPC82X]                     */
#define  BSP_INT_ID_I2C3                                 22u    /* I2C2 Controller [Only on LPC82X]                     */
#define  BSP_INT_ID_RSVD23                               23u
#define  BSP_INT_ID_PIO_INT0                             24u    /* PIO INT0                                             */
#define  BSP_INT_ID_PIO_INT1                             25u    /* PIO INT1                                             */
#define  BSP_INT_ID_PIO_INT2                             26u    /* PIO INT2                                             */
#define  BSP_INT_ID_PIO_INT3                             27u    /* PIO INT3                                             */
#define  BSP_INT_ID_PIO_INT4                             28u    /* PIO INT4                                             */
#define  BSP_INT_ID_PIO_INT5                             29u    /* PIO INT5                                             */
#define  BSP_INT_ID_PIO_INT6                             30u    /* PIO INT6                                             */
#define  BSP_INT_ID_PIO_INT7                             31u    /* PIO INT7                                             */

/*
*********************************************************************************************************
*                                            GLOBAL VARIABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                                 MACRO'S
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void        BSP_Init        (void);

void        BSP_IntDisAll   (void);

void        BSP_Tick_Init   (void);

CPU_INT32U  BSP_CPU_ClkFreq (void);


/*
*********************************************************************************************************
*                                           INTERRUPT SERVICES
*********************************************************************************************************
*/

void         BSP_IntInit    (void);

void         BSP_IntEn      (CPU_DATA       int_id);

void         BSP_IntDis     (CPU_DATA       int_id);

void         BSP_IntClr     (CPU_DATA       int_id);

void         BSP_IntVectSet (CPU_DATA       int_id,
                             CPU_FNCT_VOID  isr);

void         BSP_IntPrioSet (CPU_DATA       int_id,
                             CPU_INT08U     prio);

void         BSP_IntHandlerSPI0             (void);
void         BSP_IntHandlerSPI1             (void);
void         BSP_IntHandlerRSVD2            (void);
void         BSP_IntHandlerUART0            (void);
void         BSP_IntHandlerUART1            (void);
void         BSP_IntHandlerUART2            (void);
void         BSP_IntHandlerRSVD6            (void);
void         BSP_IntHandlerI2C1             (void);
void         BSP_IntHandlerI2C0             (void);
void         BSP_IntHandlerSCT              (void);
void         BSP_IntHandlerMRT              (void);
void         BSP_IntHandlerCMP              (void);
void         BSP_IntHandlerWDT              (void);
void         BSP_IntHandlerBOD              (void);
void         BSP_IntHandlerFLASH            (void);
void         BSP_IntHandlerWKT              (void);
void         BSP_IntHandlerADC_SEQA         (void);
void         BSP_IntHandlerADC_SEQB         (void);
void         BSP_IntHandlerADC_THCMP        (void);
void         BSP_IntHandlerADC_OVR          (void);
void         BSP_IntHandlerDMA              (void);
void         BSP_IntHandlerI2C2             (void);
void         BSP_IntHandlerI2C3             (void);
void         BSP_IntHandlerRSVD23           (void);
void         BSP_IntHandlerPIO_INT0         (void);
void         BSP_IntHandlerPIO_INT1         (void);
void         BSP_IntHandlerPIO_INT2         (void);
void         BSP_IntHandlerPIO_INT3         (void);
void         BSP_IntHandlerPIO_INT4         (void);
void         BSP_IntHandlerPIO_INT5         (void);
void         BSP_IntHandlerPIO_INT6         (void);
void         BSP_IntHandlerPIO_INT7         (void);

/*
*********************************************************************************************************
*                                              LED SERVICES
*********************************************************************************************************
*/

void        BSP_LED_Set (CPU_INT32U led_id, CPU_BOOLEAN led_on);
void        BSP_LED_Toggle (CPU_INT32U led_id);

/*
*********************************************************************************************************
*                                             MODULE END
*********************************************************************************************************
*/

#endif                                                          /* End of module include.                               */
