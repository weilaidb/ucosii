/*
*********************************************************************************************************
*
*                                    MICRIUM BOARD SUPPORT PACKAGE
*
*                          (c) Copyright 2003-2010; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*
*               This BSP is provided in source form to registered licensees ONLY.  It is
*               illegal to distribute this source code to any third party unless you receive
*               written permission by an authorized Micrium representative.  Knowledge of
*               the source code may NOT be used to develop a similar product.
*
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*
*               You can contact us at www.micrium.com.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                    MICRIUM BOARD SUPPORT PACKAGE
*                                       SERIAL (UART) INTERFACE
*
* Filename      : bsp_ser.c
* Version       : V1.00
* Programmer(s) : EHS
*                 SR
*                 AA
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#define  BSP_SER_MODULE
#include <bsp.h>
#include <bsp_os.h>

/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/


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

#if (APP_CFG_SERIAL_EN == DEF_ENABLED)

static  BSP_OS_SEM   BSP_SerTxWait;
static  BSP_OS_SEM   BSP_SerRxWait;
static  BSP_OS_SEM   BSP_SerLock;
static  CPU_INT08U   BSP_SerRxData;
static  CPU_INT08U   BSP_SerTxData;

#if (BSP_CFG_SER_CMD_HISTORY_LEN > 0u)
static  CPU_CHAR     BSP_SerCmdHistory[BSP_CFG_SER_CMD_HISTORY_LEN];
#endif

/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void        BSP_Ser_WrByteUnlocked  (CPU_INT08U  c);
static  CPU_INT08U  BSP_Ser_RdByteUnlocked  (void);
static  void        BSP_Ser_ISR_Handler     (void);
static  void        BSP_Ser_SetBaud(LPC_USART_T *pUART, CPU_INT32U baudrate);

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
*                                          BSP_Ser_Init()
*
* Description : Initialize a serial port for communication.
*
* Argument(s) : baud_rate           The desire RS232 baud rate.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_Ser_Init (CPU_INT32U  baud_rate)
{
    BSP_OS_SemCreate(&BSP_SerTxWait,   0, "Serial Tx Wait");
    BSP_OS_SemCreate(&BSP_SerRxWait,   0, "Serial Rx Wait");
    BSP_OS_SemCreate(&BSP_SerLock,     1, "Serial Lock");

#if (BSP_CFG_SER_CMD_HISTORY_LEN > 0u)
    BSP_SerCmdHistory[0] = (CPU_CHAR)'\0';
#endif

	/* Enable the clock to the Switch Matrix */
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_SWM);

	/* Connect the TXD_O and RXD_I signals to port pins(P0.4, P0.0) */
	
	Chip_SWM_DisableFixedPin(SWM_FIXED_ACMP_I1);
	Chip_SWM_DisableFixedPin(SWM_FIXED_ADC11);

	/* Enable UART Divider clock, divided by 1 */
	Chip_Clock_SetUARTClockDiv(1);

	/* Divided by 1 */
    Chip_SWM_MovablePinAssign(SWM_U1_TXD_O, 4);
    Chip_SWM_MovablePinAssign(SWM_U1_RXD_I, 0);

	/* Disable the clock to the Switch Matrix to save power */
	Chip_Clock_DisablePeriphClock(SYSCTL_CLOCK_SWM);

	/* Enable USART clock */
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_UART1);

    /* Peripheral reset control to USART1 */
    Chip_SYSCTL_PeriphReset(RESET_USART1);

    /* Configure the UART for 8-bit mode, no parity */
	Chip_UART_ConfigData(BSP_CONSOLE, UART_CFG_DATALEN_8 | UART_CFG_PARITY_NONE | UART_CFG_STOPLEN_1);
	Chip_Clock_SetUSARTNBaseClockRate((baud_rate * 6 * 16), true);
	BSP_Ser_SetBaud(BSP_CONSOLE, baud_rate);

    /* Enable TIE and RIE
     * Transmit Interrupt Enable for TDRE and 
     * Receiver Interrupt Enable for RDRF
     */

    BSP_IntVectSet(BSP_INT_ID_UART1, BSP_Ser_ISR_Handler);
    BSP_IntEn(BSP_INT_ID_UART1);

    /* Enable receiver and transmitter */
	Chip_UART_Enable(BSP_CONSOLE);
	Chip_UART_TXEnable(BSP_CONSOLE);
}

/* Set baud rate for UART */
static void BSP_Ser_SetBaud(LPC_USART_T *pUART, CPU_INT32U baudrate)
{
	CPU_INT32U baudRateGenerator;
	baudRateGenerator = Chip_Clock_GetUSARTNBaseClockRate() / (16 * baudrate);
	pUART->BRG = baudRateGenerator - 1;	/* baud rate */
}

/*
*********************************************************************************************************
*                                         BSP_Ser_ISR_Handler()
*
* Description : Serial ISR.
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

void  BSP_Ser_ISR_Handler (void)
{
    CPU_INT32U  status;
    LPC_USART_T *UART_base = BSP_CONSOLE;

    status = Chip_UART_GetIntStatus(UART_base);
    if (status & UART_STAT_RXRDY) {
        BSP_SerRxData = Chip_UART_ReadByte(UART_base);          /* Read one byte from the receive data register.      */
        BSP_OS_SemPost(&BSP_SerRxWait);                         /* Post to the sempahore                              */
    }

    if ((Chip_UART_GetIntsEnabled(UART_base) & UART_STAT_TXRDY) &&
        (status & UART_STAT_TXRDY)) {
        Chip_UART_SendByte(UART_base, BSP_SerTxData);
        Chip_UART_IntDisable(UART_base, UART_STAT_TXRDY);
        BSP_OS_SemPost(&BSP_SerTxWait);                         /* Post to the semaphore                              */
    }
}


/*
*********************************************************************************************************
*                                           BSP_Ser_Printf()
*
* Description : Print formatted data to the output serial port.
*
* Argument(s) : format      String that contains the text to be written.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) This function output a maximum of BSP_SER_PRINTF_STR_BUF_SIZE number of bytes to the
*                   serial port.  The calling function hence has to make sure the formatted string will
*                   be able fit into this string buffer or hence the output string will be truncated.
*********************************************************************************************************
*/

void  BSP_Ser_Printf (CPU_CHAR  *format, ...)
{
    CPU_CHAR  buf_str[BSP_SER_PRINTF_STR_BUF_SIZE + 1u];
    va_list   v_args;


    va_start(v_args, format);
    (void)vsnprintf((char       *)&buf_str[0],
                    (size_t      )sizeof(buf_str),
                    (char const *)format,
                    v_args);
    va_end(v_args);

    BSP_Ser_WrStr(buf_str);
}


/*
*********************************************************************************************************
*                                                BSP_Ser_RdByte()
*
* Description : Receive a single byte.
*
* Argument(s) : none.
*
* Return(s)   : The received byte.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) This functions blocks until a data is received.
*
*               (2) It can not be called from an ISR.
*********************************************************************************************************
*/

CPU_INT08U  BSP_Ser_RdByte (void)
{
    CPU_INT08U  rx_byte;

    BSP_OS_SemWait(&BSP_SerLock, 0);                            /* Obtain access to the serial interface.             */

    rx_byte = BSP_Ser_RdByteUnlocked();

    BSP_OS_SemPost(&BSP_SerLock);                               /* Release access to the serial interface.            */

    return (rx_byte);
}


/*
*********************************************************************************************************
*                                       BSP_Ser_RdByteUnlocked()
*
* Description : Receive a single byte.
*
* Argument(s) : none.
*
* Return(s)   : The received byte.
*
* Caller(s)   : BSP_Ser_RdByte()
*               BSP_Ser_RdStr()
*
* Note(s)     : none.
*********************************************************************************************************
*/

CPU_INT08U  BSP_Ser_RdByteUnlocked (void)
{
    CPU_INT08U   rx_byte;
    LPC_USART_T *UART_base = BSP_CONSOLE;

    Chip_UART_IntEnable(UART_base, UART_INTEN_RXRDY);           /* Enable the Receive not empty interrupt             */

    BSP_OS_SemWait(&BSP_SerRxWait, 0);                          /* Wait until data is received                        */

    Chip_UART_IntDisable(UART_base, UART_INTEN_RXRDY);          /* Disable the Receive not empty interrupt            */

    rx_byte = BSP_SerRxData;                                    /* Read the data from the temporary register          */

    return (rx_byte);
}

/*
*********************************************************************************************************
*                                                BSP_Ser_RdStr()
*
* Description : This function reads a string from a UART.
*
* Argument(s) : p_str       A pointer to a buffer at which the string can be stored.
*
*               len         The size of the string that will be read.
*
* Return(s)   : none.
*
* Caller(s)   : Application
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_Ser_RdStr (CPU_CHAR    *p_str,
                     CPU_INT16U   len)
{
    CPU_CHAR     *p_char;
    CPU_BOOLEAN   rxd_history_char0;
    CPU_CHAR      rx_data;
    CPU_BOOLEAN   err;


    rxd_history_char0 = DEF_NO;
    p_str[0]          = (CPU_CHAR)'\0';
    p_char            = p_str;

    err = BSP_OS_SemWait(&BSP_SerLock, 0);                      /* Obtain access to the serial interface                */

    if (err != DEF_OK ) {
        return;
    }

    while (DEF_TRUE)
    {
        rx_data = BSP_Ser_RdByteUnlocked();

        if ((rx_data == ASCII_CHAR_CARRIAGE_RETURN) ||          /* Is it '\r' or '\n' character  ?                      */
            (rx_data == ASCII_CHAR_LINE_FEED      )) {

            BSP_Ser_WrByteUnlocked((CPU_INT08U)ASCII_CHAR_LINE_FEED);
            BSP_Ser_WrByteUnlocked((CPU_INT08U)ASCII_CHAR_CARRIAGE_RETURN);
           *p_char = (CPU_CHAR)'\0';                            /* set the null character at the end of the string      */
#if (BSP_CFG_SER_CMD_HISTORY_LEN > 0u)
            Str_Copy(BSP_SerCmdHistory, p_str);
#endif
            break;                                              /* exit the loop                                        */
        }

        if (rx_data == ASCII_CHAR_BACKSPACE) {                  /* Is backspace character                               */
            if (p_char > p_str) {
                BSP_Ser_WrByteUnlocked((CPU_INT08U)ASCII_CHAR_BACKSPACE);
                p_char--;                                       /* Decrement the index                                  */
            }
        }

        if ((ASCII_IsPrint(rx_data)      ) &&
            (rxd_history_char0 == DEF_NO)) {                    /* Is it a printable character ... ?                    */
            BSP_Ser_WrByteUnlocked((CPU_INT08U)rx_data);        /* Echo-back                                            */
           *p_char = rx_data;                                   /* Save the received character in the buffer            */
            p_char++;                                           /* Increment the buffer index                           */
            if (p_char >= &p_str[len]) {
                p_char  = &p_str[len];
            }

        } else if ((rx_data           == ASCII_CHAR_ESCAPE) &&
                   (rxd_history_char0 == DEF_NO           )) {
            rxd_history_char0 = DEF_YES;

#if (BSP_CFG_SER_CMD_HISTORY_LEN > 0u)
        } else if ((rx_data           == ASCII_CHAR_LEFT_SQUARE_BRACKET) &&
                   (rxd_history_char0 == DEF_YES                       )) {

            while (p_char != p_str) {
                BSP_Ser_WrByteUnlocked((CPU_INT08U)ASCII_CHAR_BACKSPACE);
                p_char--;                                       /* Decrement the index                                  */
            }

            Str_Copy(p_str, BSP_SerCmdHistory);

            while (*p_char != '\0') {
                BSP_Ser_WrByteUnlocked(*p_char++);
            }
#endif
        } else {
            rxd_history_char0 = DEF_NO;
        }
    }

    BSP_OS_SemPost(&BSP_SerLock);                               /* Release access to the serial interface               */
}


/*
*********************************************************************************************************
*                                          BSP_Ser_WrByteUnlocked()
*
* Description : Writes a single byte to a serial port.
*
* Argument(s) : c           The character to output.
*
* Return(s)   : none.
*
* Caller(s)   : BSP_Ser_WrByte()
*               BSP_Ser_WrByteUnlocked()
*
* Note(s)     : (1) This function blocks until room is available in the UART for the byte to be sent.
*********************************************************************************************************
*/

void  BSP_Ser_WrByteUnlocked (CPU_INT08U c)
{
    LPC_USART_T *UART_base = BSP_CONSOLE;

    BSP_SerTxData = c;
    Chip_UART_IntEnable(UART_base, UART_INTEN_TXRDY);           /* Enable the Transmit empty interrupt                  */
    BSP_OS_SemWait(&BSP_SerTxWait, 0);
}


/*
*********************************************************************************************************
*                                                BSP_Ser_WrByte()
*
* Description : Writes a single byte to a serial port.
*
* Argument(s) : tx_byte     The character to output.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_Ser_WrByte(CPU_INT08U  c)
{
    BSP_OS_SemWait(&BSP_SerLock, 0);                            /* Obtain access to the serial interface              */

    BSP_Ser_WrByteUnlocked(c);

    BSP_OS_SemPost(&BSP_SerLock);                               /* Release access to the serial interface             */
}


/*
*********************************************************************************************************
*                                                BSP_Ser_WrStr()
*
* Description : Transmits a string.
*
* Argument(s) : p_str       Pointer to the string that will be transmitted.
*
* Caller(s)   : Application.
*
* Return(s)   : none.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_Ser_WrStr (CPU_CHAR  *p_str)
{
    CPU_BOOLEAN  err;


    if (p_str == (CPU_CHAR *)0) {
        return;
    }


    err = BSP_OS_SemWait(&BSP_SerLock, 0);                      /* Obtain access to the serial interface              */
    if (err != DEF_OK ) {
        return;
    }

    while ((*p_str) != (CPU_CHAR )0) {
        if (*p_str == ASCII_CHAR_LINE_FEED) {
            BSP_Ser_WrByteUnlocked(ASCII_CHAR_CARRIAGE_RETURN);
            BSP_Ser_WrByteUnlocked(ASCII_CHAR_LINE_FEED);
            p_str++;
        } else {
            BSP_Ser_WrByteUnlocked(*p_str++);
        }
    }

    BSP_OS_SemPost(&BSP_SerLock);                               /* Release access to the serial interface             */
}

#if (APP_CFG_PRINTF_EN == DEF_ENABLED)

/* For printf usage */

#pragma import(__use_no_semihosting)

struct __FILE
{
    int handle;
};

FILE __stdout;

int _sys_exit (int x)
{
    x = x;
    return 0;
}

int  fputc (int ch, FILE *f)
{
    BSP_OS_SemWait(&BSP_SerLock, 0);                            /* Obtain access to the serial interface              */

    BSP_Ser_WrByteUnlocked((CPU_INT08U)ch);

    BSP_OS_SemPost(&BSP_SerLock);                               /* Release access to the serial interface             */

    return ch;
}
#endif  /* (APP_CFG_PRINTF_EN == DEF_ENABLED) */

#endif  /* (APP_CFG_SERIAL_EN == DEF_ENABLED) */

