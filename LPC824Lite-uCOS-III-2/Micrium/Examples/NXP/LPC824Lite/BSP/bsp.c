/*
*********************************************************************************************************
*                                     MICIRUM BOARD SUPPORT PACKAGE
*
*                             (c) Copyright 2014; Micrium, Inc.; Weston, FL
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
* Filename      : bsp.c
* Version       : V1.00
* Programmer(s) : JPB
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#define  BSP_MODULE
#include  <bsp.h>

#ifdef  BSP_CFG_OS
#include  <bsp_os.h>
#endif

/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/

#define  BSP_CORE_DIV              (1u)
#define  BSP_BUS_DIV               (2u)
#define  BSP_FLEXBUS_DIV           (3u)
#define  BSP_FLASH_DIV             (6u)


/*
*********************************************************************************************************
*                                           LOCAL CONSTANTS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                          LOCAL DATA TYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            LOCAL TABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

static  CPU_INT32U      BSP_CPU_ClkFreq_MHz;
static const CPU_INT08U BSP_LED_PinMaps[BSP_LED_CNT] = {7, 13, 16, 17, 19, 27, 28, 18};
static const CPU_INT08U BSP_KEY_PinMaps[BSP_KEY_CNT] = {12, 4 , 1};
static const CPU_INT08U BSP_KEY_PioConMaps[BSP_KEY_CNT] = {IOCON_PIO12, IOCON_PIO4, IOCON_PIO1};
/* System oscillator rate and clock rate on the CLKIN pin */
const uint32_t OscRateIn = MAIN_OSC_XTAL_FREQ_HZ;
const uint32_t ExtRateIn = EXT_CLOCK_IN_FREQ_HZ;

/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  BSP_LED_Init      (void);
static  void  BSP_SPI_PinMux    (void);
static  void  BSP_I2C_PinMux    (void);

/*
*********************************************************************************************************
*                                             REGISTERS
*********************************************************************************************************
*/

#define  DBGMCU_CR   *(CPU_REG32 *)0xE0042004

/*
*********************************************************************************************************
*                                            REGISTER BITS
*********************************************************************************************************
*/

#define  DBGMCU_CR_TRACE_IOEN_MASK       0x10
#define  DBGMCU_CR_TRACE_MODE_ASYNC      0x00
#define  DBGMCU_CR_TRACE_MODE_SYNC_01    0x40
#define  DBGMCU_CR_TRACE_MODE_SYNC_02    0x80
#define  DBGMCU_CR_TRACE_MODE_SYNC_04    0xC0
#define  DBGMCU_CR_TRACE_MODE_MASK       0xC0

/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/

#if ((CPU_CFG_TS_TMR_EN          != DEF_ENABLED) && \
     (APP_CFG_PROBE_OS_PLUGIN_EN == DEF_ENABLED) && \
     (OS_PROBE_HOOKS_EN          == 1))
#error  "CPU_CFG_TS_EN                  illegally #define'd in 'cpu.h'"
#error  "                              [MUST be  DEF_ENABLED] when    "
#error  "                               using uC/Probe COM modules    "
#endif


/*
*********************************************************************************************************
*                                               BSP_Init()
*
* Description : Initialize the Board Support Package (BSP).
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) This function SHOULD be called before any other BSP function is called.
*
*               (2) CPU instruction / data tracing requires the use of the following pins :
*                   (a) (1) Aysynchronous     :  PB[3]
*                       (2) Synchronous 1-bit :  PE[3:2]
*                       (3) Synchronous 2-bit :  PE[4:2]
*                       (4) Synchronous 4-bit :  PE[6:2]
*
*                   (b) The application may wish to adjust the trace bus width depending on I/O
*                       requirements.
*********************************************************************************************************
*/

void  BSP_Init (void)
{
    BSP_IntInit();                                              /* Initialize Interrupts.                               */

    SystemCoreClockUpdate();
    
	Chip_GPIO_Init(LPC_GPIO_PORT);                              /* Enable all the port Clocks                           */

    BSP_CPU_ClkFreq_MHz = BSP_CPU_ClkFreq() / (CPU_INT32U)1000000;

    BSP_CPU_ClkFreq_MHz = BSP_CPU_ClkFreq_MHz;                  /* Surpress compiler warning BSP_CPU_ClkFreq_MHz.       */

    BSP_LED_Init();                                             /* Initialize the I/Os for the RGB LED.                 */

#if (APP_CFG_SERIAL_EN == DEF_ENABLED)
    BSP_Ser_Init(115200);
#endif

    BSP_SPI_PinMux();
    BSP_I2C_PinMux();

    BSP_I2C_Init(BSP_I2C_ID_I2C0, BSP_I2C_MODE_STANDARD, 
                 BSP_I2C_MODE_STANDARD_MAX_FREQ_HZ);

#ifdef TRACE_EN                                                 /* See project / compiler preprocessor options.         */
    DBGMCU_CR |=  DBGMCU_CR_TRACE_IOEN_MASK;                    /* Enable tracing (see Note #2).                        */
    DBGMCU_CR &= ~DBGMCU_CR_TRACE_MODE_MASK;                    /* Clr trace mode sel bits.                             */
    DBGMCU_CR |=  DBGMCU_CR_TRACE_MODE_SYNC_04;                 /* Cfg trace mode to synch 4-bit.                       */
#endif
}


/*
*********************************************************************************************************
*                                            BSP_CPU_ClkFreq()
*
* Description : Read CPU registers to determine the CPU clock frequency of the chip.
*
* Argument(s) : none.
*
* Return(s)   : The CPU clock frequency, in Hz.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

CPU_INT32U  BSP_CPU_ClkFreq (void)
{
  return ((CPU_INT32U)Chip_Clock_GetSystemClockRate());         /* The board runs the CPU at 48MHz.                     */
}


/*
*********************************************************************************************************
*                                            BSP_Tick_Init()
*
* Description : Initialize all the peripherals that required OS Tick services (OS initialized)
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/
#ifdef  BSP_CFG_OS
void BSP_Tick_Init (void)
{
    CPU_INT32U  cpu_clk_freq;
    CPU_INT32U  cnts;

    cpu_clk_freq = BSP_CPU_ClkFreq();                           /* Determine SysTick reference freq.                    */

#if (OS_VERSION >= 30000u)
    cnts  = cpu_clk_freq / (CPU_INT32U)OSCfg_TickRate_Hz;       /* Determine nbr SysTick increments.                    */
#else
    cnts  = cpu_clk_freq / (CPU_INT32U)OS_TICKS_PER_SEC;        /* Determine nbr SysTick increments.                    */
#endif

    OS_CPU_SysTickInit(cnts);                                   /* Init uC/OS periodic time src (SysTick).              */
}
#endif


/*
*********************************************************************************************************
*********************************************************************************************************
*                                             RGB LED FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             BSP_LED_Init()
*
* Description : Initialize the I/O for the RGB LED.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : BSP_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  BSP_LED_Init (void)
{
	int i;

	for (i = 0; i < BSP_LED_CNT; i++) {
		Chip_GPIO_PinSetDIR(LPC_GPIO_PORT, 0, BSP_LED_PinMaps[i], 1);
		Chip_GPIO_PinSetState(LPC_GPIO_PORT, 0, BSP_LED_PinMaps[i], true);
	}
}


/*
*********************************************************************************************************
*                                             BSP_LED_Set()
*
* Description : Sets the color by re-configuring the duty cycle of the PWM signals.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_LED_Set (CPU_INT32U led_id, CPU_BOOLEAN led_on)
{
	if (led_id < BSP_LED_CNT) {
		Chip_GPIO_PinSetState(LPC_GPIO_PORT, 0, BSP_LED_PinMaps[led_id], (bool) !led_on);
	}
}

void  BSP_LED_Toggle(CPU_INT32U led_id)
{
	if (led_id < BSP_LED_CNT) {
		Chip_GPIO_PinToggleState(LPC_GPIO_PORT, 0, BSP_LED_PinMaps[led_id]);
	}
}

/*
*********************************************************************************************************
*********************************************************************************************************
*                                          TSI MODULE FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             BSP_TSI_Init()
*
* Description : Initialize the TSI (Touch Sense Input) module.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : BSP_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  BSP_SPI_PinMux (void)
{
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_SWM);
	Chip_SWM_MovablePinAssign(SWM_SPI1_SSEL0_IO, 15);
	Chip_SWM_MovablePinAssign(SWM_SPI1_SCK_IO, 24);
	Chip_SWM_MovablePinAssign(SWM_SPI1_MISO_IO, 25);
	Chip_SWM_MovablePinAssign(SWM_SPI1_MOSI_IO, 26);
	Chip_Clock_DisablePeriphClock(SYSCTL_CLOCK_SWM);
}


/*
*********************************************************************************************************
*                                             BSP_TSI_SelfCal()
*
* Description : Calibrates the TSI (Touch Sense Input) module.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : BSP_TSI_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  BSP_I2C_PinMux (void)
{
	/* Enable the clock to the Switch Matrix */
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_SWM);
	Chip_SWM_EnableFixedPin(SWM_FIXED_I2C0_SDA);
	Chip_SWM_EnableFixedPin(SWM_FIXED_I2C0_SCL);

	/* Enable Fast Mode Plus for I2C pins */
	Chip_IOCON_PinSetI2CMode(LPC_IOCON, IOCON_PIO10, PIN_I2CMODE_FASTPLUS);
	Chip_IOCON_PinSetI2CMode(LPC_IOCON, IOCON_PIO11, PIN_I2CMODE_FASTPLUS);

	/* Disable the clock to the Switch Matrix to save power */
	Chip_Clock_DisablePeriphClock(SYSCTL_CLOCK_SWM);
}

