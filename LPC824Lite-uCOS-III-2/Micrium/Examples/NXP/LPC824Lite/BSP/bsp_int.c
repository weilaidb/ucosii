/*
*********************************************************************************************************
*                                     MICIRUM BOARD SUPPORT PACKAGE
*
*                             (c) Copyright 2013; Micrium, Inc.; Weston, FL
*
*                   All rights reserved.  Protected by international copyright laws.
*                   Knowledge of the source code may not be used to write a similar
*                   product.  This file may only be used in accordance with a license
*                   and should not be redistributed in any way.
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
* Filename      : bsp_int.c
* Version       : V1.00
* Programmer(s) : JPB
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#define  BSP_INT_MODULE
#include <bsp.h>

#ifdef  BSP_CFG_OS
#include <bsp_os.h>
#endif


/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/

#define  BSP_INT_SRC_NBR                                 32


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

static  CPU_FNCT_VOID  BSP_IntVectTbl[BSP_INT_SRC_NBR];


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  BSP_IntHandler      (CPU_DATA  int_id);
static  void  BSP_IntHandlerDummy (void);


/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                              BSP_IntClr()
*
* Description : Clear interrupt.
*
* Argument(s) : int_id      Interrupt to clear.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) An interrupt does not need to be cleared within the interrupt controller.
*********************************************************************************************************
*/

void  BSP_IntClr (CPU_DATA  int_id)
{

}


/*
*********************************************************************************************************
*                                              BSP_IntDis()
*
* Description : Disable interrupt.
*
* Argument(s) : int_id      Interrupt to disable.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_IntDis (CPU_DATA  int_id)
{
    if (int_id < BSP_INT_SRC_NBR) {
        CPU_IntSrcDis(int_id + 16);
    }
}


/*
*********************************************************************************************************
*                                           BSP_IntDisAll()
*
* Description : Disable ALL interrupts.
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

void  BSP_IntDisAll (void)
{
    CPU_IntDis();
}


/*
*********************************************************************************************************
*                                               BSP_IntEn()
*
* Description : Enable interrupt.
*
* Argument(s) : int_id      Interrupt to enable.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_IntEn (CPU_DATA  int_id)
{
    if (int_id < BSP_INT_SRC_NBR) {
        CPU_IntSrcEn(int_id + 16);
    }
}


/*
*********************************************************************************************************
*                                            BSP_IntVectSet()
*
* Description : Assign ISR handler.
*
* Argument(s) : int_id      Interrupt for which vector will be set.
*
*               isr         Handler to assign
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_IntVectSet (CPU_DATA       int_id,
                      CPU_FNCT_VOID  isr)
{
    CPU_SR_ALLOC();


    if (int_id < BSP_INT_SRC_NBR) {
        CPU_CRITICAL_ENTER();
        BSP_IntVectTbl[int_id] = isr;
        CPU_CRITICAL_EXIT();
    }
}


/*
*********************************************************************************************************
*                                            BSP_IntPrioSet()
*
* Description : Assign ISR priority.
*
* Argument(s) : int_id      Interrupt for which vector will be set.
*
*               prio        Priority to assign
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_IntPrioSet (CPU_DATA    int_id,
                      CPU_INT08U  prio)
{
    CPU_SR_ALLOC();


    if (int_id < BSP_INT_SRC_NBR) {
        CPU_CRITICAL_ENTER();
        CPU_IntSrcPrioSet(int_id + 16, prio);
        CPU_CRITICAL_EXIT();
    }
}


/*
*********************************************************************************************************
*********************************************************************************************************
*                                           INTERNAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                              BSP_IntInit()
*
* Description : Initialize interrupts:
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

void  BSP_IntInit (void)
{
    CPU_DATA  int_id;


    for (int_id = 0; int_id < BSP_INT_SRC_NBR; int_id++) {
        BSP_IntVectSet(int_id, BSP_IntHandlerDummy);
    }
}


/*
*********************************************************************************************************
*                                        BSP_IntHandler####()
*
* Description : Handle an interrupt.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : This is an ISR.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_IntHandlerSPI0             (void)  { BSP_IntHandler(BSP_INT_ID_SPI0);                     }
void  BSP_IntHandlerSPI1             (void)  { BSP_IntHandler(BSP_INT_ID_SPI1);                     }
void  BSP_IntHandlerRSVD2            (void)  { BSP_IntHandler(BSP_INT_ID_RSVD2);                    }
void  BSP_IntHandlerUART0            (void)  { BSP_IntHandler(BSP_INT_ID_UART0);                    }
void  BSP_IntHandlerUART1            (void)  { BSP_IntHandler(BSP_INT_ID_UART1);                    }
void  BSP_IntHandlerUART2            (void)  { BSP_IntHandler(BSP_INT_ID_UART2);                    }
void  BSP_IntHandlerRSVD6            (void)  { BSP_IntHandler(BSP_INT_ID_RSVD6);                    }
void  BSP_IntHandlerI2C1             (void)  { BSP_IntHandler(BSP_INT_ID_I2C1);                     }
void  BSP_IntHandlerI2C0             (void)  { BSP_IntHandler(BSP_INT_ID_I2C0);                     }
void  BSP_IntHandlerSCT              (void)  { BSP_IntHandler(BSP_INT_ID_SCT);                      }
void  BSP_IntHandlerMRT              (void)  { BSP_IntHandler(BSP_INT_ID_MRT);                      }
void  BSP_IntHandlerCMP              (void)  { BSP_IntHandler(BSP_INT_ID_CMP);                      }
void  BSP_IntHandlerWDT              (void)  { BSP_IntHandler(BSP_INT_ID_WDT);                      }
void  BSP_IntHandlerBOD              (void)  { BSP_IntHandler(BSP_INT_ID_BOD);                      }
void  BSP_IntHandlerFLASH            (void)  { BSP_IntHandler(BSP_INT_ID_FLASH);                    }
void  BSP_IntHandlerWKT              (void)  { BSP_IntHandler(BSP_INT_ID_WKT);                      }
void  BSP_IntHandlerADC_SEQA         (void)  { BSP_IntHandler(BSP_INT_ID_ADC_SEQA);                 }
void  BSP_IntHandlerADC_SEQB         (void)  { BSP_IntHandler(BSP_INT_ID_ADC_SEQB);                 }
void  BSP_IntHandlerADC_THCMP        (void)  { BSP_IntHandler(BSP_INT_ID_ADC_THCMP);                }
void  BSP_IntHandlerADC_OVR          (void)  { BSP_IntHandler(BSP_INT_ID_ADC_OVR);                  }
void  BSP_IntHandlerDMA              (void)  { BSP_IntHandler(BSP_INT_ID_DMA);                      }
void  BSP_IntHandlerI2C2             (void)  { BSP_IntHandler(BSP_INT_ID_I2C2);                     }
void  BSP_IntHandlerI2C3             (void)  { BSP_IntHandler(BSP_INT_ID_I2C3);                     }
void  BSP_IntHandlerRSVD23           (void)  { BSP_IntHandler(BSP_INT_ID_RSVD23);                   }
void  BSP_IntHandlerPIO_INT0         (void)  { BSP_IntHandler(BSP_INT_ID_PIO_INT0);                 }
void  BSP_IntHandlerPIO_INT1         (void)  { BSP_IntHandler(BSP_INT_ID_PIO_INT1);                 }
void  BSP_IntHandlerPIO_INT2         (void)  { BSP_IntHandler(BSP_INT_ID_PIO_INT2);                 }
void  BSP_IntHandlerPIO_INT3         (void)  { BSP_IntHandler(BSP_INT_ID_PIO_INT3);                 }
void  BSP_IntHandlerPIO_INT4         (void)  { BSP_IntHandler(BSP_INT_ID_PIO_INT4);                 }
void  BSP_IntHandlerPIO_INT5         (void)  { BSP_IntHandler(BSP_INT_ID_PIO_INT5);                 }
void  BSP_IntHandlerPIO_INT6         (void)  { BSP_IntHandler(BSP_INT_ID_PIO_INT6);                 }
void  BSP_IntHandlerPIO_INT7         (void)  { BSP_IntHandler(BSP_INT_ID_PIO_INT7);                 }


/*
*********************************************************************************************************
*********************************************************************************************************
*                                           LOCAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                          BSP_IntHandler()
*
* Description : Central interrupt handler.
*
* Argument(s) : int_id          Interrupt that will be handled.
*
* Return(s)   : none.
*
* Caller(s)   : ISR handlers.
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  BSP_IntHandler (CPU_DATA  int_id)
{
    CPU_FNCT_VOID  isr;

#ifdef  BSP_CFG_OS
    CPU_SR_ALLOC();


    CPU_CRITICAL_ENTER();                                       /* Tell the OS that we are starting an ISR            */

    OSIntEnter();

    CPU_CRITICAL_EXIT();
#endif

    if (int_id < BSP_INT_SRC_NBR) {
        isr = BSP_IntVectTbl[int_id];
        if (isr != (CPU_FNCT_VOID)0) {
            isr();
        }
    }

#ifdef  BSP_CFG_OS
    OSIntExit();                                                /* Tell the OS that we are leaving the ISR            */
#endif
}


/*
*********************************************************************************************************
*                                        BSP_IntHandlerDummy()
*
* Description : Dummy interrupt handler.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : BSP_IntHandler().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  BSP_IntHandlerDummy (void)
{

}
