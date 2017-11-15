;/*****************************************************************************
; * @file:    startup_LPC8xx.s
; * @purpose: CMSIS Cortex-M0+ Core Device Startup File 
; *           for the NXP LPC8xx Device Series 
; * @version: V1.0
; * @date:    16. Aug. 2012
; *------- <<< Use Configuration Wizard in Context Menu >>> ------------------
; *
; * Copyright (C) 2012 ARM Limited. All rights reserved.
; * ARM Limited (ARM) is supplying this software for use with Cortex-M0+ 
; * processor based microcontrollers.  This file can be freely distributed 
; * within development tools that are supporting such ARM based processors. 
; *
; * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
; * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
; * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
; * ARM SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
; * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
; *
; *****************************************************************************/


; <h> Stack Configuration
;   <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>
#include  "bsp_cfg.h"

Stack_Size      EQU     0x00000200

                AREA    STACK, NOINIT, READWRITE, ALIGN=3
Stack_Mem       SPACE   Stack_Size
__initial_sp


; <h> Heap Configuration
;   <o>  Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Heap_Size       EQU     0x00000000

                AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
Heap_Mem        SPACE   Heap_Size
__heap_limit


                PRESERVE8
                THUMB


; Vector Table Mapped to Address 0 at Reset
                IMPORT  BSP_IntHandlerSPI0
                IMPORT  BSP_IntHandlerSPI1
                IMPORT  BSP_IntHandlerRSVD2
                IMPORT  BSP_IntHandlerUART0
                IMPORT  BSP_IntHandlerUART1
                IMPORT  BSP_IntHandlerUART2
                IMPORT  BSP_IntHandlerRSVD6
                IMPORT  BSP_IntHandlerI2C1
                IMPORT  BSP_IntHandlerI2C0
                IMPORT  BSP_IntHandlerSCT
                IMPORT  BSP_IntHandlerMRT
                IMPORT  BSP_IntHandlerCMP
                IMPORT  BSP_IntHandlerWDT
                IMPORT  BSP_IntHandlerBOD
                IMPORT  BSP_IntHandlerFLASH
                IMPORT  BSP_IntHandlerWKT
                IMPORT  BSP_IntHandlerADC_SEQA
                IMPORT  BSP_IntHandlerADC_SEQB
                IMPORT  BSP_IntHandlerADC_THCMP
                IMPORT  BSP_IntHandlerADC_OVR
                IMPORT  BSP_IntHandlerDMA
                IMPORT  BSP_IntHandlerI2C2
                IMPORT  BSP_IntHandlerI2C3
                IMPORT  BSP_IntHandlerRSVD23
                IMPORT  BSP_IntHandlerPIO_INT0
                IMPORT  BSP_IntHandlerPIO_INT1
                IMPORT  BSP_IntHandlerPIO_INT2
                IMPORT  BSP_IntHandlerPIO_INT3
                IMPORT  BSP_IntHandlerPIO_INT4
                IMPORT  BSP_IntHandlerPIO_INT5
                IMPORT  BSP_IntHandlerPIO_INT6
                IMPORT  BSP_IntHandlerPIO_INT7
                
                #ifdef  BSP_CFG_OS
                IMPORT  OS_CPU_PendSVHandler
                IMPORT  OS_CPU_SysTickHandler
                #endif

                AREA    RESET, DATA, READONLY
                EXPORT  __Vectors
                EXPORT  __Vectors_End
                EXPORT  __Vectors_Size

__Vectors       DCD     __initial_sp                            ; 000 Initial stack pointer.
                DCD     App_Reset_ISR                           ; 001 Initial program counter.
                DCD     App_NMI_ISR                             ; 002 Non-maskable interrupt.
                DCD     App_Fault_ISR                           ; 003 Hard fault exception.
                DCD     App_Spurious_ISR                        ; 004 Reserved interrupt 4.
                DCD     App_BusFault_ISR                        ; 005 Bus fault exception.
                DCD     App_UsageFault_ISR                      ; 006 Usage fault exception.
                DCD     App_Spurious_ISR                        ; 007 Reserved interrupt 7.
                DCD     App_Spurious_ISR                        ; 008 Reserved interrupt 8.
                DCD     App_Spurious_ISR                        ; 009 Reserved interrupt 9.
                DCD     App_Spurious_ISR                        ; 010 Reserved interrupt 10.
                DCD     App_Spurious_ISR                        ; 011 A supervisor call exception.
                DCD     App_Spurious_ISR                        ; 012 Debug Monitor.
                DCD     App_Spurious_ISR                        ; 013 Reserved interrupt 13.
                
                #ifdef  BSP_CFG_OS
                DCD     OS_CPU_PendSVHandler                    ; 014 PendSV exception.
                DCD     OS_CPU_SysTickHandler                   ; 015 SysTick Interrupt.
                #else
                DCD     App_Spurious_ISR                        ; 014 Reserved interrupt 14.
                DCD     App_Spurious_ISR                        ; 015 Reserved interrupt 15.
                #endif

                ; External Interrupts
                DCD     BSP_IntHandlerSPI0                      ; 016 IRQ[  0] SPI 0 controller
                DCD     BSP_IntHandlerSPI1                      ; 017 IRQ[  1] SPI 1 controller
                DCD     BSP_IntHandlerRSVD2
                DCD     BSP_IntHandlerUART0                     ; 019 IRQ[  3] UART0 interrupt
                DCD     BSP_IntHandlerUART1                     ; 020 IRQ[  4] UART1 interrupt
                DCD     BSP_IntHandlerUART2                     ; 021 IRQ[  5] UART2 interrupt
                DCD     BSP_IntHandlerRSVD6
                DCD     BSP_IntHandlerI2C1                      ; 023 IRQ[  7] I2C1 interrupt
                DCD     BSP_IntHandlerI2C0                      ; 024 IRQ[  8] I2C0 interrupt
                DCD     BSP_IntHandlerSCT                       ; 025 IRQ[  9] Smart Counter Timer
                DCD     BSP_IntHandlerMRT                       ; 026 IRQ[ 10] Multi-Rate Timer
                DCD     BSP_IntHandlerCMP                       ; 027 IRQ[ 11] Comparator
                DCD     BSP_IntHandlerWDT                       ; 028 IRQ[ 12] WatchDog timer
                DCD     BSP_IntHandlerBOD                       ; 029 IRQ[ 13] Brown Out Detect
                DCD     BSP_IntHandlerFLASH                     ; 030 IRQ[ 14] Non-Volatile Memory Controller
                DCD     BSP_IntHandlerWKT                       ; 031 IRQ[ 15] Wakeup timer
                DCD     BSP_IntHandlerADC_SEQA                  ; 032 IRQ[ 16] ADC Sequence A Completion [Only on LPC82X]
                DCD     BSP_IntHandlerADC_SEQB                  ; 033 IRQ[ 17] ADC Sequence B Completion [Only on LPC82X]
                DCD     BSP_IntHandlerADC_THCMP                 ; 034 IRQ[ 18] ADC Threshold compare [Only on LPC82X]
                DCD     BSP_IntHandlerADC_OVR                   ; 035 IRQ[ 19] ADC Overrun [Only on LPC82X]
                DCD     BSP_IntHandlerDMA                       ; 036 IRQ[ 20] DMA Controller [Only on LPC82X]
                DCD     BSP_IntHandlerI2C2                      ; 037 IRQ[ 21] I2C2 Controller [Only on LPC82X]
                DCD     BSP_IntHandlerI2C3                      ; 038 IRQ[ 22] I2C3 Controller [Only on LPC82X]
                DCD     BSP_IntHandlerRSVD23
                DCD     BSP_IntHandlerPIO_INT0                  ; 040 IRQ[ 24] PIO INT0
                DCD     BSP_IntHandlerPIO_INT1                  ; 041 IRQ[ 25] PIO INT1
                DCD     BSP_IntHandlerPIO_INT2                  ; 042 IRQ[ 26] PIO INT2
                DCD     BSP_IntHandlerPIO_INT3                  ; 043 IRQ[ 27] PIO INT3
                DCD     BSP_IntHandlerPIO_INT4                  ; 044 IRQ[ 28] PIO INT4
                DCD     BSP_IntHandlerPIO_INT5                  ; 044 IRQ[ 29] PIO INT5
                DCD     BSP_IntHandlerPIO_INT6                  ; 046 IRQ[ 30] PIO INT6
                DCD     BSP_IntHandlerPIO_INT7                  ; 047 IRQ[ 31] PIO INT7
__Vectors_End

__Vectors_Size 	EQU     __Vectors_End - __Vectors

; <h> Flash Configuration
;   <i> 16-byte flash configuration field that stores default protection settings (loaded on reset)
;   <i> and security information that allows the MCU to restrict acces to the FTFL module.
;   <h> Backdoor Comparison Key
;     <o0>  Backdoor Key 0  <0x0-0xFF:2>
;     <o1>  Backdoor Key 1  <0x0-0xFF:2>
;     <o2>  Backdoor Key 2  <0x0-0xFF:2>
;     <o3>  Backdoor Key 3  <0x0-0xFF:2>
;     <o4>  Backdoor Key 4  <0x0-0xFF:2>
;     <o5>  Backdoor Key 5  <0x0-0xFF:2>
;     <o6>  Backdoor Key 6  <0x0-0xFF:2>
;     <o7>  Backdoor Key 7  <0x0-0xFF:2>
BackDoorK0      EQU     0xFF
BackDoorK1      EQU     0xFF
BackDoorK2      EQU     0xFF
BackDoorK3      EQU     0xFF
BackDoorK4      EQU     0xFF
BackDoorK5      EQU     0xFF
BackDoorK6      EQU     0xFF
BackDoorK7      EQU     0xFF
;   </h>
;   <h> Program flash protection bytes (FPROT)
;     <i> Each program flash region can be protected from program and erase operation by setting the associated PROT bit.
;     <i> Each bit protects a 1/32 region of the program flash memory.
;     <h> FPROT0
;       <i> Program flash protection bytes
;       <i> 1/32 - 8/32 region
;       <o.0>   FPROT0.0
;       <o.1>   FPROT0.1
;       <o.2>   FPROT0.2
;       <o.3>   FPROT0.3
;       <o.4>   FPROT0.4
;       <o.5>   FPROT0.5
;       <o.6>   FPROT0.6
;       <o.7>   FPROT0.7
nFPROT0         EQU     0x00
FPROT0          EQU     nFPROT0:EOR:0xFF
;     </h>
;     <h> FPROT1
;       <i> Program Flash Region Protect Register 1
;       <i> 9/32 - 16/32 region
;       <o.0>   FPROT1.0
;       <o.1>   FPROT1.1
;       <o.2>   FPROT1.2
;       <o.3>   FPROT1.3
;       <o.4>   FPROT1.4
;       <o.5>   FPROT1.5
;       <o.6>   FPROT1.6
;       <o.7>   FPROT1.7
nFPROT1         EQU     0x00
FPROT1          EQU     nFPROT1:EOR:0xFF
;     </h>
;     <h> FPROT2
;       <i> Program Flash Region Protect Register 2
;       <i> 17/32 - 24/32 region
;       <o.0>   FPROT2.0
;       <o.1>   FPROT2.1
;       <o.2>   FPROT2.2
;       <o.3>   FPROT2.3
;       <o.4>   FPROT2.4
;       <o.5>   FPROT2.5
;       <o.6>   FPROT2.6
;       <o.7>   FPROT2.7
nFPROT2         EQU     0x00
FPROT2          EQU     nFPROT2:EOR:0xFF
;     </h>
;     <h> FPROT3
;       <i> Program Flash Region Protect Register 3
;       <i> 25/32 - 32/32 region
;       <o.0>   FPROT3.0
;       <o.1>   FPROT3.1
;       <o.2>   FPROT3.2
;       <o.3>   FPROT3.3
;       <o.4>   FPROT3.4
;       <o.5>   FPROT3.5
;       <o.6>   FPROT3.6
;       <o.7>   FPROT3.7
nFPROT3         EQU     0x00
FPROT3          EQU     nFPROT3:EOR:0xFF
;     </h>
;   </h>
;   </h>
;   <h> Flash nonvolatile option byte (FOPT)
;     <i> Allows the user to customize the operation of the MCU at boot time.
;     <o.0>  LPBOOT0
;       <0=> Core and system clock divider (OUTDIV1) is 0x7 (divide by 8) or 0x3 (divide by 4)
;       <1=> Core and system clock divider (OUTDIV1) is 0x1 (divide by 2) or 0x0 (divide by 1)
;     <o.4>  LPBOOT1
;       <0=> Core and system clock divider (OUTDIV1) is 0x7 (divide by 8) or 0x1 (divide by 2)
;       <1=> Core and system clock divider (OUTDIV1) is 0x3 (divide by 4) or 0x0 (divide by 1)
;     <o.2>  NMI_DIS
;       <0=> NMI interrupts are always blocked
;       <1=> NMI pin/interrupts reset default to enabled
;     <o.3>  RESET_PIN_CFG
;       <0=> RESET pin is disabled following a POR and cannot be enabled as RESET function
;       <1=> RESET pin is dedicated
;     <o.3>  FAST_INIT
;       <0=> Slower initialization
;       <1=> Fast Initialization
FOPT            EQU     0xFF
;   </h>
;   <h> Flash security byte (FSEC)
;     <i> WARNING: If SEC field is configured as "MCU security status is secure" and MEEN field is configured as "Mass erase is disabled",
;     <i> MCU's security status cannot be set back to unsecure state since Mass erase via the debugger is blocked !!!
;     <o.0..1> SEC
;       <2=> MCU security status is unsecure
;       <3=> MCU security status is secure
;         <i> Flash Security
;         <i> This bits define the security state of the MCU.
;     <o.2..3> FSLACC
;       <2=> Freescale factory access denied
;       <3=> Freescale factory access granted
;         <i> Freescale Failure Analysis Access Code
;         <i> This bits define the security state of the MCU.
;     <o.4..5> MEEN
;       <2=> Mass erase is disabled
;       <3=> Mass erase is enabled
;         <i> Mass Erase Enable Bits
;         <i> Enables and disables mass erase capability of the FTFL module
;     <o.6..7> KEYEN
;       <2=> Backdoor key access enabled
;       <3=> Backdoor key access disabled
;         <i> Backdoor key Security Enable
;         <i> These bits enable and disable backdoor key access to the FTFL module.
FSEC            EQU     0xFE
;   </h>

                IF      :LNOT::DEF:RAM_TARGET
                AREA    |.ARM.__at_0x400|, CODE, READONLY
                DCB     BackDoorK0, BackDoorK1, BackDoorK2, BackDoorK3
                DCB     BackDoorK4, BackDoorK5, BackDoorK6, BackDoorK7
                DCB     FPROT0,     FPROT1,     FPROT2,     FPROT3
                DCB     FSEC,       FOPT,       0xFF,     0xFF
                ENDIF

                AREA    |.text|, CODE, READONLY


; Reset Handler
SIM_COPC    EQU     0x40048100

App_Reset_ISR   PROC
                EXPORT  App_Reset_ISR             [WEAK]
                IMPORT  SystemInit
                IMPORT  __main
                
                LDR		R0, =SystemInit
                BLX		R0
                
                LDR     R0, =__main
                BX      R0
                ENDP


; Dummy Exception Handlers (infinite loops which can be modified)

App_NMI_ISR\
                PROC
                EXPORT  App_NMI_ISR               [WEAK]
                B       .
                ENDP
App_Fault_ISR\
                PROC
                EXPORT  App_Fault_ISR             [WEAK]
                B       .
                ENDP
App_BusFault_ISR\
                PROC
                EXPORT  App_BusFault_ISR          [WEAK]
                B       .
                ENDP
App_UsageFault_ISR\
                PROC
                EXPORT  App_UsageFault_ISR        [WEAK]
                B       .
                ENDP
App_Spurious_ISR\
                PROC
                EXPORT  App_Spurious_ISR          [WEAK]
                B       .
                ENDP


                ALIGN

; User Initial Stack & Heap

                IF      :DEF:__MICROLIB

                EXPORT  __initial_sp
                EXPORT  __heap_base
                EXPORT  __heap_limit

                ELSE

                IMPORT  __use_two_region_memory
                EXPORT  __user_initial_stackheap
__user_initial_stackheap

                LDR     R0, =  Heap_Mem
                LDR     R1, =(Stack_Mem + Stack_Size)
                LDR     R2, = (Heap_Mem +  Heap_Size)
                LDR     R3, = Stack_Mem
                BX      LR

                ALIGN

                ENDIF


                END
