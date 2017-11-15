/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                              (c) Copyright 2013; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                      APPLICATION CONFIGURATION
*
*                                           NXP LPC824Lite
*                                          Evaluation Board
*
* Filename      : bsp_cfg.h
* Version       : V1.00
* Programmer(s) : SB
*********************************************************************************************************
*/

#ifndef  BSP_CFG_MODULE_PRESENT
#define  BSP_CFG_MODULE_PRESENT

/*
*********************************************************************************************************
*                                       ENABLE/DISABLE uCOS BSP CODE
*********************************************************************************************************
*/

#ifndef  BSP_CFG_OS_ENABLED
#define  BSP_CFG_OS_ENABLED                         1u
#endif

#if  (BSP_CFG_OS_ENABLED == 1u)
#define  BSP_CFG_OS
#else
#define  BSP_CFG_BAREMETAL
#endif

#endif
