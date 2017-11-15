/*
*********************************************************************************************************
*                                     MICRIUM BOARD SUPPORT SUPPORT
*
*                          (c) Copyright 2003-2009; Micrium, Inc.; Weston, FL
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
*                                     MICIUM BOARD SUPPORT PACKAGE
*                                       AT24C02 EEPROM DRIVER
*                                                                         
*
* Filename      : bsp_at24.c
* Version       : V1.00
* Programmer(s) : FT
*********************************************************************************************************
* Note(s)       :
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#define  BSP_AT24_MODULE
#include <bsp.h>


/*
*********************************************************************************************************
*                                              LOCAL DEFINES
*********************************************************************************************************
*/

#define BSP_AT24C02_ADDR_MSK                    0x7F
#define BSP_AT24C02_PAGE_SIZE                   8
#define BSP_AT24C02_CHIP_SIZE                   256

#define BSP_AT24C02_ADDR_LEN                    2

/*
*********************************************************************************************************
*                                           LOCAL CONSTANTS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                           LOCAL MACRO'S
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


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*********************************************************************************************************
**                                         GLOBAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                        BSP_AT24_Init ()
*
* Description : Initialize the the AT24C02 EEPROM.
*
* Argument(s) : none.
*
* Return(s)   : DEF_OK     If the AT24C02 EEPROM was initialized
*               DEF_FAIL   If the AT24C02 EEPROM could not be initialized.
*
* Caller(s)   : Application
*
* Note(s)     : none.
*********************************************************************************************************
*/

CPU_BOOLEAN  BSP_AT24_Init (void)
{
    return DEF_OK;
}


/*
*********************************************************************************************************
*                                        BSP_AT24_Read()
*
* Description : Read data from AT24C02 EEPROM.
*
* Return(s)   : DEF_OK     If the AT24C02 EEPROM could be read.
*               DEF_FAIL   If the AT24C02 EEPROM could not be read.
*
* Caller(s)   : Application
*
* Note(s)     : none.
*********************************************************************************************************
*/

CPU_BOOLEAN BSP_AT24_Read   (CPU_INT16U addr,
                             CPU_INT16U length,
                             CPU_INT08U *buf,
                             CPU_INT16U *real_byte)
{
    CPU_INT16U  chip_size = 0;
    CPU_INT08U  addr_buf[BSP_AT24C02_ADDR_LEN];

    chip_size = BSP_AT24C02_CHIP_SIZE;

    if (length > chip_size) {
        return (DEF_FAIL);
    }

    addr_buf[0] = BSP_AT24_I2C_ADDR & BSP_AT24C02_ADDR_MSK;
    addr_buf[0] |= (CPU_INT08U)((addr >> 8) &
                                (~BSP_AT24C02_ADDR_MSK));

    addr_buf[1] = (CPU_INT08U)(addr & 0xFF);

    *real_byte = ((length + addr) > chip_size) ? (chip_size - addr) : length;

    return (BSP_I2C_WrRd(BSP_AT24_I2C_ID,
                         addr_buf[0],
                         &addr_buf[1],
                         1,
                         buf,
                         *real_byte));
}



/*
*********************************************************************************************************
*                                        BSP_AT24_Write()
*
* Description : Write data to AT24C02 EEPROM.
*
* Return(s)   : DEF_OK     If the AT24C02 EEPROM could be written.
*               DEF_FAIL   If the AT24C02 EEPROM could not be written.
*
* Caller(s)   : Application
*
* Note(s)     : none.
*********************************************************************************************************
*/

CPU_BOOLEAN BSP_AT24_Write  (CPU_INT16U addr,
                             CPU_INT16U length,
                             CPU_INT08U *buf,
                             CPU_INT16U *real_byte)
{
    CPU_INT16U  data_len = 0;
    CPU_INT16U  offset = 0;
    CPU_INT16U  page_size = 0;
    CPU_INT16U  chip_size = 0;
    CPU_INT08U  addr_buf[BSP_AT24C02_ADDR_LEN];
    OS_ERR      err;

    page_size = BSP_AT24C02_PAGE_SIZE;
    chip_size = BSP_AT24C02_CHIP_SIZE;

    if (length > chip_size) {
        return (DEF_FAIL);
    }

    addr_buf[0] = BSP_AT24_I2C_ADDR & BSP_AT24C02_ADDR_MSK;

    *real_byte = ((length + addr) > chip_size) ? (chip_size - addr) : length;

    while (offset < (*real_byte))
        {
        /* Compute the number of data ready to write for one write cycle */
        
        data_len = DEF_MIN ((*real_byte) - offset,
                            page_size - ((addr + offset) % page_size));

        addr_buf[0] |= (CPU_INT08U)(((addr + offset) >> 8) &
                                    (~BSP_AT24C02_ADDR_MSK));
        addr_buf[1] = (CPU_INT08U)((addr + offset) & 0xFF);

        if (BSP_I2C_Wr(BSP_AT24_I2C_ID,
                       addr_buf[0],
                       &addr_buf[1],
                       1,
                       (buf + offset),
                       data_len) != DEF_OK) {
            return (DEF_FAIL);
        }

        offset += data_len;

        OSTimeDlyHMSM(0, 0, 0, 10,
                      OS_OPT_TIME_HMSM_STRICT,
                      &err);
        }

    return (DEF_OK);
}

