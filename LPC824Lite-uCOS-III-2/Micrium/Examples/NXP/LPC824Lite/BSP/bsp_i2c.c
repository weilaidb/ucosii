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
*                                        I2C DRIVER (MASTER ONLY)
*                                                                         
*
* Filename      : bsp_i2c.c
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

#define  BSP_I2C_MODULE
#include <bsp.h>


/*
*********************************************************************************************************
*                                              LOCAL DEFINES
*********************************************************************************************************
*/

#define BSP_I2C_BUF_SIZE                        0x10
#define BSP_I2C_HDL_MEM_SIZE                    0x20

/*
*********************************************************************************************************
*                                           LOCAL CONSTANTS
*********************************************************************************************************
*/



/*
*********************************************************************************************************
*                                          LOCAL DATA TYPES
*
* Note(s) :  The 'BSP_I2C_DEV' structure defines the status of the current transfer
*
*********************************************************************************************************
*/

typedef  struct bsp_i2c_dev {
    I2C_HANDLE_T *I2CHandle;
    BSP_OS_SEM   SemLock;                                       /* I2C Exclusive access sempahore                       */
    BSP_OS_SEM   SemWait;                                       /* Transfer Complete signal                             */
    CPU_INT08U   TxBuf[BSP_I2C_BUF_SIZE];                       /* The transfer data area                               */
    CPU_INT08U   RxBuf[BSP_I2C_BUF_SIZE];                       /* The receive data area                                */
    CPU_INT32U   I2CHandleMem[BSP_I2C_HDL_MEM_SIZE];
} BSP_I2C_DEV;

/*
*********************************************************************************************************
*                                            LOCAL TABLES
*********************************************************************************************************
*/

static  BSP_I2C_DEV     BSP_I2C_DevTbl[BSP_I2C_NBR_MAX];

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

static void BSP_I2C0_ISR_Handler    (void);
static void BSP_I2C0_Callback (CPU_INT32U err_code, CPU_INT32U n);

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
*                                        BSP_I2C_Init()
*
* Description : Initialize the I2C.
*
* Argument(s) : i2c_id     I2C peripheral ID
*                              BSP_I2C_ID_I2C0
*
*               bit_rate   I2C clock speed. It must be set to a value lower than 100 kHz (Standard Mode) or
*                          400 Khz (Fast mode)
*
* Return(s)   : DEF_OK     If the I2C peripheral was initialized
*               DEF_FAIL   If the I2C peripheral could not be initialized.
*
* Caller(s)   : Application
*
* Note(s)     : none.
*********************************************************************************************************
*/


CPU_BOOLEAN  BSP_I2C_Init (CPU_INT08U  i2c_id,
                           CPU_INT08U  i2c_mode,
                           CPU_INT32U  bit_rate)
                    
{
    CPU_BOOLEAN     err;
    CPU_INT32U      i2c_base;
    BSP_I2C_DEV     *p_i2c_dev;

    switch (i2c_id) {
        case BSP_I2C_ID_I2C0:
            /* Enable I2C clock */
            Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_I2C0);
            /* Peripheral reset control to I2C */
            Chip_SYSCTL_PeriphReset(RESET_I2C0);
            i2c_base = LPC_I2C_BASE;
            p_i2c_dev = (BSP_I2C_DEV *)&BSP_I2C_DevTbl[0];

            break;

        default:
            return (DEF_FAIL);
    }

    BSP_TRACE_INFO(("i2c ram size: 0x%x\n\r", LPC_I2CD_API->i2c_get_mem_size()));
    /* Perform a sanity check on the storage allocation */
    if (LPC_I2CD_API->i2c_get_mem_size() > sizeof(p_i2c_dev->I2CHandleMem)) {
        /* Example only: this should never happen and probably isn't needed for
           most I2C code. */
        return (DEF_FAIL);
    }

    /* Setup the I2C handle */
    p_i2c_dev->I2CHandle = LPC_I2CD_API->i2c_setup(i2c_base, p_i2c_dev->I2CHandleMem);
    if (p_i2c_dev->I2CHandle == NULL) {
        return (DEF_FAIL);
    }

    /* Set I2C bitrate */
    if (LPC_I2CD_API->i2c_set_bitrate(p_i2c_dev->I2CHandle,
                                      Chip_Clock_GetSystemClockRate(),
                                      bit_rate) != LPC_OK) {
        return (DEF_FAIL);
    }
                                                                /* -------------- CREATE OS SEMAPHORES  ------------- */
    err = BSP_OS_SemCreate((BSP_OS_SEM    *)&(p_i2c_dev->SemWait),
                           (BSP_OS_SEM_VAL ) 0, 
                           (CPU_CHAR      *) "I2C Wait");          
    if (err == DEF_FAIL) {
        return (DEF_FAIL);
    }

    err = BSP_OS_SemCreate((BSP_OS_SEM    *)&(p_i2c_dev->SemLock),
                           (BSP_OS_SEM_VAL ) 1, 
                           (CPU_CHAR      *)"I2C Lock");        
    if (err == DEF_FAIL) {
        return (DEF_FAIL);
    }

    switch (i2c_id) {
        case BSP_I2C_ID_I2C0:
            BSP_IntVectSet(BSP_INT_ID_I2C0, BSP_I2C0_ISR_Handler);
            BSP_IntEn(BSP_INT_ID_I2C0);
            break;

        default:
             break;
    }

    return (DEF_OK);
    
}


/*
*********************************************************************************************************
*                                        BSP_I2C_Rd()
*
* Description : Read 'n' bytes from the I2C bus.
*
* Argument(s) : i2c_id       I2C peripheral number
*                                BSP_I2C_ID_I2C0
*
*               i2c_addr     The I2C device address
*
*               p_buf        Pointer to the buffer into which the bytes will be stored.
*
*               nbr_bytes    Number of bytes to be read.
*
* Return(s)   : DEF_OK       If all bytes were read.
*               DEF_FAIL     If all bytes could not be read.
*
* Caller(s)   : Application
*
* Note(s)     : none.
*********************************************************************************************************
*/

CPU_BOOLEAN  BSP_I2C_Rd (CPU_INT08U   i2c_id,
                         CPU_INT08U   i2c_addr,
                         CPU_INT08U  *p_buf,
                         CPU_INT16U   nbr_bytes)
{
    CPU_BOOLEAN     err;
	I2C_PARAM_T     param;
	I2C_RESULT_T    result;
	ErrorCode_t     error_code;
    BSP_I2C_DEV     *p_i2c_dev;

    if (p_buf == (CPU_INT08U *)0) {
        return (DEF_FAIL);
    }
    if ((nbr_bytes < 1) || ((nbr_bytes + 1) > BSP_I2C_BUF_SIZE)) {
        return (DEF_FAIL);
    }
    switch (i2c_id) {
        case BSP_I2C_ID_I2C0:
            p_i2c_dev = (BSP_I2C_DEV *)&BSP_I2C_DevTbl[0];
            break;

        default:
            return (DEF_FAIL);
    }

    err = BSP_OS_SemWait(&(p_i2c_dev->SemLock), 0);             /* Lock the I2C peripheral                              */
    if (err == DEF_FAIL) {
        return (DEF_FAIL);
    }

	p_i2c_dev->RxBuf[0] = ((i2c_addr << 1) | DEF_BIT_00);

	/* Setup I2C parameters for number of bytes with stop - appears as follows on bus:
	   Start - address7 or address10upper - ack
	   (10 bits addressing only) address10lower - ack
	   value 1 - ack
	   value 2 - ack - stop */
	param.num_bytes_send    = 0;
	param.num_bytes_rec     = nbr_bytes + 1;
	param.buffer_ptr_rec    = p_i2c_dev->RxBuf;
	param.stop_flag         = 1;
	param.func_pt           = BSP_I2C0_Callback;

	/* Set timeout (much) greater than the transfer length */
	LPC_I2CD_API->i2c_set_timeout(p_i2c_dev->I2CHandle, 100000);

	/* Do master write transfer */
	/* Function is non-blocking, returned error should be LPC_OK, but isn't checked here */
	error_code = LPC_I2CD_API->i2c_master_receive_intr(p_i2c_dev->I2CHandle, &param, &result);
                                                                /* Wait until the transfer completes                    */
    err = BSP_OS_SemWait(&(p_i2c_dev->SemWait), 500);

    BSP_OS_SemPost(&(p_i2c_dev->SemLock));                      /* Release the I2C Peripheral                           */

    if (error_code != 0) {                                      /* If the transfer is incomplete ...                    */
        err  = DEF_FAIL;                                        /* ... return an errror                                 */
    } else {
        Mem_Copy(p_buf, &p_i2c_dev->RxBuf[1], nbr_bytes);
    }

    return (err);

}


/*
*********************************************************************************************************
*                                        BSP_I2C_Wr()
*
* Description : Write 'n' bytes tothe I2C bus.
*
* Argument(s) : i2c_id      I2C peripheral number
*                                BSP_I2C_ID_I2C0
*
*               i2c_addr     The I2C device address
*
*               p_buf        Pointer to the buffer where the bytes will be transfered.
*
*               nbr_bytes    Number of bytes to be read.
*
* Return(s)   : DEF_OK       If all bytes were written
*               DEF_FAIL     If all bytes could not be written.
*
* Caller(s)   : Application
*
* Note(s)     : none.
*********************************************************************************************************
*/


CPU_BOOLEAN  BSP_I2C_Wr (CPU_INT08U   i2c_id,
                         CPU_INT08U   i2c_addr,
                         CPU_INT08U  *offset_buf,
                         CPU_INT08U   offset_len,
                         CPU_INT08U  *p_buf,
                         CPU_INT16U   nbr_bytes)
{
    CPU_BOOLEAN     err;
	I2C_PARAM_T     param;
	I2C_RESULT_T    result;
	ErrorCode_t     error_code;
    BSP_I2C_DEV     *p_i2c_dev;

    if ((offset_buf == (CPU_INT08U *)0) || (p_buf == (CPU_INT08U *)0)) {
        return (DEF_FAIL);
    }
    if ((nbr_bytes < 1) || ((offset_len + nbr_bytes + 1) > BSP_I2C_BUF_SIZE)) {
        return (DEF_FAIL);
    }
    switch (i2c_id) {
        case BSP_I2C_ID_I2C0:
            p_i2c_dev = (BSP_I2C_DEV *)&BSP_I2C_DevTbl[0];
            break;

        default:
            return (DEF_FAIL);
    }

    err = BSP_OS_SemWait(&(p_i2c_dev->SemLock), 0);             /* Lock the I2C peripheral                              */
    if (err == DEF_FAIL) {
        return (DEF_FAIL);
    }

	p_i2c_dev->TxBuf[0] = ((i2c_addr << 1) & DEF_BIT_FIELD(7, 1));
    Mem_Copy(&p_i2c_dev->TxBuf[1], offset_buf, offset_len);
    Mem_Copy(&p_i2c_dev->TxBuf[offset_len + 1], p_buf, nbr_bytes);

	/* Setup I2C parameters for number of bytes with stop - appears as follows on bus:
	   Start - address7 or address10upper - ack
	   (10 bits addressing only) address10lower - ack
	   value 1 - ack
	   value 2 - ack - stop */
	param.num_bytes_send    = offset_len + nbr_bytes + 1;
	param.buffer_ptr_send   = p_i2c_dev->TxBuf;
	param.num_bytes_rec     = 0;
	param.stop_flag         = 1;
	param.func_pt           = BSP_I2C0_Callback;

	/* Set timeout (much) greater than the transfer length */
	LPC_I2CD_API->i2c_set_timeout(p_i2c_dev->I2CHandle, 100000);

	/* Do master write transfer */
	/* Function is non-blocking, returned error should be LPC_OK, but isn't checked here */
	error_code = LPC_I2CD_API->i2c_master_transmit_intr(p_i2c_dev->I2CHandle, &param, &result);
                                                                /* Wait until the transfer completes                    */
    err = BSP_OS_SemWait(&(p_i2c_dev->SemWait), 500);

    BSP_OS_SemPost(&(p_i2c_dev->SemLock));                      /* Release the I2C Peripheral                           */

    if (error_code != 0) {                                      /* If the transfer is incomplete ...                    */
        err  = DEF_FAIL;                                        /* ... return an errror                                 */
    }

    return (err);
}


/*
*********************************************************************************************************
*                                        BSP_I2C_WrRd()
*
* Description : Perform a write followed by multiples/single read(s)
*
* Argument(s) : i2c_id      I2C peripheral number
*                                BSP_I2C_ID_I2C0
*
*               i2c_addr     The I2C device address
*
*               p_buf        Pointer to the buffer where the bytes will be transfered/received.
*
*               nbr_bytes    Number of bytes to be read.
*
* Return(s)   : DEF_OK       If all bytes were read
*               DEF_FAIL     If all bytes could not be read.
*
* Caller(s)   : Application
*
* Note(s)     : none.
*********************************************************************************************************
*/

CPU_BOOLEAN  BSP_I2C_WrRd (CPU_INT08U   i2c_id,
                           CPU_INT08U   i2c_addr,
                           CPU_INT08U  *offset_buf,
                           CPU_INT08U   offset_len,
                           CPU_INT08U  *p_buf,
                           CPU_INT16U   nbr_bytes)
{
    CPU_BOOLEAN     err;
	I2C_PARAM_T     param;
	I2C_RESULT_T    result;
	ErrorCode_t     error_code;
    BSP_I2C_DEV     *p_i2c_dev;

    if ((offset_buf == (CPU_INT08U *)0) || (p_buf == (CPU_INT08U *)0)) {
        return (DEF_FAIL);
    }
    if ((nbr_bytes < 1) ||
        ((offset_len + 1) > BSP_I2C_BUF_SIZE) ||
        ((nbr_bytes + 1) > BSP_I2C_BUF_SIZE)) {
        return (DEF_FAIL);
    }
    switch (i2c_id) {
        case BSP_I2C_ID_I2C0:
            p_i2c_dev = (BSP_I2C_DEV *)&BSP_I2C_DevTbl[0];
            break;

        default:
            return (DEF_FAIL);
    }

    err = BSP_OS_SemWait(&(p_i2c_dev->SemLock), 0);             /* Lock the I2C peripheral                              */
    if (err == DEF_FAIL) {
        return (DEF_FAIL);
    }

	p_i2c_dev->TxBuf[0] = ((i2c_addr << 1) & DEF_BIT_FIELD(7, 1));
	p_i2c_dev->RxBuf[0] = ((i2c_addr << 1) | DEF_BIT_00);
    Mem_Copy(&p_i2c_dev->TxBuf[1], offset_buf, offset_len);

	/* Setup I2C parameters for number of bytes with stop - appears as follows on bus:
	   Start - address7 or address10upper - ack
	   (10 bits addressing only) address10lower - ack
	   value 1 - ack
	   value 2 - ack - stop */
	param.num_bytes_send    = offset_len + 1;
	param.buffer_ptr_send   = p_i2c_dev->TxBuf;
	param.num_bytes_rec     = nbr_bytes + 1;
	param.buffer_ptr_rec    = p_i2c_dev->RxBuf;
	param.stop_flag         = 1;
	param.func_pt           = BSP_I2C0_Callback;

	/* Set timeout (much) greater than the transfer length */
	LPC_I2CD_API->i2c_set_timeout(p_i2c_dev->I2CHandle, 100000);

	/* Do master write transfer */
	/* Function is non-blocking, returned error should be LPC_OK, but isn't checked here */
	error_code = LPC_I2CD_API->i2c_master_tx_rx_intr(p_i2c_dev->I2CHandle, &param, &result);
                                                                /* Wait until the transfer completes                    */
    err = BSP_OS_SemWait(&(p_i2c_dev->SemWait), 500);

    BSP_OS_SemPost(&(p_i2c_dev->SemLock));                      /* Release the I2C Peripheral                           */

    if (error_code != 0) {                                      /* If the transfer is incomplete ...                    */
        err  = DEF_FAIL;                                        /* ... return an errror                                 */
    } else {
        Mem_Copy(p_buf, &p_i2c_dev->RxBuf[1], nbr_bytes);
    }

    return (err);             
}


/*
*********************************************************************************************************
*                                        BSP_I2C0_ISR_Handler()
*
* Description : I2C0 ISR handlers
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


static  void  BSP_I2C0_ISR_Handler (void)
{
	/* Call I2C ISR function in ROM with the I2C handle */
	LPC_I2CD_API->i2c_isr_handler(BSP_I2C_DevTbl[0].I2CHandle);
}

/*
*********************************************************************************************************
*                                        BSP_I2C0_Callback()
*
* Description : I2C0 callback function
*
* Return(s)   : none.
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  BSP_I2C0_Callback (CPU_INT32U err_code, CPU_INT32U n) 
{
    BSP_OS_SemPost(&(BSP_I2C_DevTbl[0].SemWait));
}

