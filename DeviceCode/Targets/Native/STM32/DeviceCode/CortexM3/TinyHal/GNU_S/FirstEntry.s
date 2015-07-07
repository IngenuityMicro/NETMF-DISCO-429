@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@
@  Licensed under the Apache License, Version 2.0 (the "License");
@  you may not use this file except in compliance with the License.
@  You may obtain a copy of the License at http:;www.apache.org/licenses/LICENSE-2.0
@
@  Copyright (c) Microsoft Corporation. All rights reserved.
@  Implementation for STM32: Copyright (c) Oberon microsystems, Inc.
@
@  CORTEX-M3 Standard Entry Code 
@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


    .global  EntryPoint

    .global  PreStackInit_Exit_Pointer
    .global  ARM_Vectors

    .global StackBottom
    .global StackTop
    .global HeapBegin
    .global HeapEnd
    .global CustomHeapBegin
    .global CustomHeapEnd

    .extern  PreStackInit


    .ifdef HAL_REDUCESIZE
        .extern BootEntryLoader
    .else
        .extern BootEntry
    .endif
    .extern  BootstrapCode


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

    @ allow per processor pre-stack initialization

PreStackEntry:
    b       PreStackInit

PreStackInit_Exit_Pointer:

    ldr     sp,StackTop_Ptr
    bl      BootstrapCode

    .ifdef HAL_REDUCESIZE
        b   BootEntryLoader
    .else
        b   BootEntry
    .endif


    .align

StackTop_Ptr:
    .word     StackTop

Fault_Handler:
    b       Fault_Handler


    .end
