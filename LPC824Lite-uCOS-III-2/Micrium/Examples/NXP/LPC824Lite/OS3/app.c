/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                          (c) Copyright 2003-2014; Micrium, Inc.; Weston, FL
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
*                                            EXAMPLE CODE
*
*                                           NXP LPC824Lite
*                                          Evaluation Board
*
* Filename      : app.c
* Version       : V1.00
* Programmer(s) : JPB
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include  <app_cfg.h>
#include  <includes.h>

/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

static  OS_TCB        App_TaskStartTCB;
static  CPU_STK_SIZE  App_TaskStartStk[APP_CFG_TASK_START_STK_SIZE];

static  OS_TCB        App_TaskLEDTCB;
static  CPU_STK_SIZE  App_TaskLEDStk[APP_CFG_TASK_LED_STK_SIZE];

/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  App_ObjCreate     (void);
static  void  App_TaskCreate    (void);

static  void  App_TaskStart     (void *p_arg);
static  void  App_TaskLED       (void *p_arg);

/*
*********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.  It is assumed that your code will call
*               main() once you have performed all necessary initialization.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : This the main standard entry point.
*
* Note(s)     : none.
*********************************************************************************************************
*/

int  main (void)
{
    OS_ERR  err;

#if (CPU_CFG_NAME_EN == DEF_ENABLED)
    CPU_ERR     cpu_err;
#endif


    CPU_Init();                                                 /* Initialize the CPU abstraction layer.                */

    Mem_Init();                                                 /* Initialize the Memory Management Module.             */

    Math_Init();                                                /* Initialize the Mathematical Module.                  */

#if (CPU_CFG_NAME_EN == DEF_ENABLED)
    CPU_NameSet((CPU_CHAR *)"LPC824Lite",
                (CPU_ERR  *)&cpu_err);
#endif

    BSP_IntDisAll();                                            /* Disable all interrupts.                              */

#if (defined(TRACE_CFG_EN) && (TRACE_CFG_EN > 0u))
    TRACE_INIT();                                               /* Initialize the µC/Trace recorder.                    */
    TRACE_START();                                              /* Start recording.                                     */
#endif

    OSInit(&err);                                               /* Initialize "uC/OS-III, The Real-Time Kernel".        */

    OSTaskCreate((OS_TCB     *)&App_TaskStartTCB,               /* Create the startup task.                             */
                 (CPU_CHAR   *)"Startup Task",
                 (OS_TASK_PTR ) App_TaskStart,
                 (void       *) 0,
                 (OS_PRIO     ) APP_CFG_TASK_START_PRIO,
                 (CPU_STK    *)&App_TaskStartStk[0],
                 (CPU_STK     )(APP_CFG_TASK_START_STK_SIZE / 10u),
                 (CPU_STK_SIZE) APP_CFG_TASK_START_STK_SIZE,
                 (OS_MSG_QTY  ) 0,
                 (OS_TICK     ) 0,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);

    OSStart(&err);                                              /* Start multitasking (i.e. give control to uC/OS-III). */

    while(DEF_ON) {                                             /* Should Never Get Here                                */
    };
}


/*
*********************************************************************************************************
*                                          App_TaskStart()
*
* Description : This is an example of a startup task.  As mentioned in the book's text, you MUST
*               initialize the ticker only once multitasking has started.
*
* Argument(s) : p_arg   is the argument passed to 'App_TaskStart()' by 'OSTaskCreate()'.
*
* Return(s)   : none.
*
* Caller(s)   : This is a task.
*
* Notes       : (1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                   used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/

static  void  App_TaskStart (void *p_arg)
{
    OS_ERR  os_err;
    CPU_INT08U  wdata[4] = {0x5a, 0xa5, 0x55, 0xaa};
    CPU_INT08U  rdata[4] = {0};
    CPU_INT16U  cnts;

    (void)p_arg;                                                /* See Note #1.                                         */

    BSP_Init();                                                 /* Start BSP and tick initialization.                   */

    BSP_Tick_Init();                                            /* Start Tick Initialization.                           */

#if OS_CFG_STAT_TASK_EN > 0u
    OSStatTaskCPUUsageInit(&os_err);                            /* Compute CPU capacity with no task running            */
#endif

#ifdef  CPU_CFG_INT_DIS_MEAS_EN
    CPU_IntDisMeasMaxCurReset();
#endif

    APP_TRACE_INFO(("Creating Application Events...\n\r"));
    App_ObjCreate();                                            /* Create Applicaton kernel objects.                    */

    APP_TRACE_INFO(("Creating Application Tasks...\n\r"));
    App_TaskCreate();                                           /* Create Application tasks.                            */

    BSP_AT24_Write(EEPROM_POST_ADDR, 4, wdata, &cnts);
    BSP_AT24_Read(EEPROM_POST_ADDR, 4, rdata, &cnts);
    if (Mem_Cmp(wdata, rdata, 4) != DEF_YES) {
        APP_TRACE_INFO(("EEPROM POST data: 0x%02x-0x%02x-0x%02x-0x%02x\n\r",
                        rdata[0], rdata[1], rdata[2], rdata[3]));
    } else {
        APP_TRACE_INFO(("EEPROM Test .............................. PASS\n\r"));
    }

    while (DEF_TRUE) {                                          /* Task body, always written as an infinite loop.       */
        OSTimeDlyHMSM(0, 0, 0, 100,
                      OS_OPT_TIME_HMSM_STRICT, &os_err);
    }
}


/*
*********************************************************************************************************
*                                      App_ObjCreate()
*
* Description:  Creates the application kernel objects.
*
* Argument(s) :  none.
*
* Return(s)   :  none.
*
* Caller(s)   :  App_TaskStart().
*
* Note(s)     :  none.
*********************************************************************************************************
*/

static  void  App_ObjCreate (void)
{
}


/*
*********************************************************************************************************
*                                      App_TaskCreate()
*
* Description :  This function creates the application tasks.
*
* Argument(s) :  none.
*
* Return(s)   :  none.
*
* Caller(s)   :  App_TaskStart().
*
* Note(s)     :  none.
*********************************************************************************************************
*/

static  void  App_TaskCreate (void)
{
    OS_ERR  err;


    OSTaskCreate((OS_TCB     *)&App_TaskLEDTCB,            /* Create the RGB LED task.                             */
                 (CPU_CHAR   *)"LED Task",
                 (OS_TASK_PTR ) App_TaskLED,
                 (void       *) 0,
                 (OS_PRIO     ) APP_CFG_TASK_LED_PRIO,
                 (CPU_STK    *)&App_TaskLEDStk[0],
                 (CPU_STK     )(APP_CFG_TASK_LED_STK_SIZE / 10u),
                 (CPU_STK_SIZE) APP_CFG_TASK_LED_STK_SIZE,
                 (OS_MSG_QTY  ) 0,
                 (OS_TICK     ) 0,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);
}


/*
*********************************************************************************************************
*                                          App_TaskLED()
*
* Description : This is an example of an application task.
*
* Argument(s) : p_arg   is the argument passed to 'App_TaskLED()' by 'OSTaskCreate()'.
*
* Return(s)   : none.
*
* Caller(s)   : This is a task.
*
* Notes       : (1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                   used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/

static  void  App_TaskLED (void *p_arg)
{
     OS_ERR      os_err;


    (void)p_arg;                                                /* See Note #1                                              */

    while (DEF_TRUE) {                                          /* Task body, always written as an infinite loop.           */

        BSP_LED_Toggle(BSP_LED1);                               /* Set the color of the RGB LED.                            */

        OSTimeDlyHMSM(0, 0, 0, 500,
                      OS_OPT_TIME_HMSM_STRICT, &os_err);        /* Suspend the task execution for 50 milliseconds.          */
    }
}

