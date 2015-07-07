@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@
@  Licensed under the Apache License, Version 2.0 (the "License");
@  you may not use this file except in compliance with the License.
@  You may obtain a copy of the License at http:;www.apache.org/licenses/LICENSE-2.0
@
@  Copyright (c) Microsoft Corporation. All rights reserved.
@  Implementation for STM32: Copyright (c) Oberon microsystems, Inc.
@
@  CORTEX-M3 Bootloader Entry Code 
@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    .global  EntryPoint

    .global  ARM_Vectors

    .global StackBottom
    .global StackTop
    .global HeapBegin
    .global HeapEnd
    .global CustomHeapBegin
    .global CustomHeapEnd

    .extern  BootEntry
    .extern  BootstrapCodeMinimal

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

@ these define the region to zero initialize
    .extern  Image$$ER_RAM_RW$$ZI$$Base
    .extern  Image$$ER_RAM_RW$$ZI$$Length
    
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    .section SectionForStackBottom,       "a", %progbits
StackBottom:
    .word   0
    .section SectionForStackTop,          "a", %progbits
StackTop:
    .word   0
    .section SectionForHeapBegin,         "a", %progbits
HeapBegin:
    .word   0
    .section SectionForHeapEnd,           "a", %progbits
HeapEnd:
    .word   0
    .section SectionForCustomHeapBegin,   "a", %progbits
ARM_Vectors:
    .zero   84*4   @ exception vector table (max 84 entries)
CustomHeapBegin:
    .word   0
    .section SectionForCustomHeapEnd,     "a", %progbits
CustomHeapEnd:
    .word   0


    .section i.EntryPoint, "xa", %progbits

    .entry

    .thumb_func
EntryPoint:

@ The first word has several functions:
@ - It is the entry point of the application
@ - it contains a signature word used to identify application blocks
@ - out of reset it contains the initial stack pointer value
@ - it is the first entry of the initial exception handler table
@ The actual word used is 0x2000E00C

    b       Start         @ 0xE00C
    .byte     0x00        @ Booter signature is 0x2000E00C
    .byte     0x20
    .word     Start         @ Reset
    .word     Fault_Handler @ NMI
    .word     Fault_Handler @ Hard Fault
    .word     Fault_Handler @ MMU Fault
    .word     Fault_Handler @ Bus Fault
    .word     Fault_Handler @ Usage Fault

Start:

    ldr     sp,StackTop_Ptr
    bl      BootstrapCodeMinimal

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ clear the Zero Initialized RAM, .bss 
@ do this last just in case there is overlap on the load area
@ and the execution ZI area
@ this is not a failsafe, but helps some situations
@ it would be nice to trap non-workable scatter files somehow - more thought required here @todo
    
    ldr     r0, = Image$$ER_RAM_RW$$ZI$$Length
    cmp     r0, #0
    beq     NoClearZI_ER_RAM
    ldr     r1, = Image$$ER_RAM_RW$$ZI$$Base
    mov     r2, #0

ClearZI_ER_RAM:

    stmia   r1!, { r2 }
    subs    r0, r0, #4                  @ 4 bytes filled per loop 
    bne     ClearZI_ER_RAM

NoClearZI_ER_RAM:


@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ done moving and clearing RAM, so continue on in C 

    b       BootEntry


    .align

StackTop_Ptr:
    DCD     StackTop

Fault_Handler:
    b       Fault_Handler


    .end
