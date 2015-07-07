////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
//
//  Copyright (c) Microsoft Corporation. All rights reserved.
//  Implementation for STM32: Copyright (c) Oberon microsystems, Inc.
//
//  CORTEX-M3 Interrupt Disable Handling
//  Compiler independent (CMSIS) version
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <tinyhal.h>

/*
 *  Usage:
 *  constructor { Release Acquire | Probe } destructor
 *
 *  legal states:     m_state    Primask
 *  WAS_DISABLED         1       1 (disabled)
 *  WAS_ENABLED          0       1 (disabled)
 *  RELEASED             0       0 (enabled)
 */


/*
 *  disabled -> WAS_DISABLED
 *  enabled  -> WAS_ENABLED
 */
SmartPtr_IRQ::SmartPtr_IRQ(void* context)
{ 
    m_context = context; // never used
    m_state = __get_PRIMASK();
    __disable_irq();
}

/*
 *  WAS_DISABLED -> disable
 *  WAS_ENABLED  -> enable
 *  RELEASED     -> enable
 */
SmartPtr_IRQ::~SmartPtr_IRQ() 
{ 
    __set_PRIMASK(m_state);
}

BOOL SmartPtr_IRQ::WasDisabled()
{
    // Also check for interrupt state != 0
    return m_state || __get_IPSR() != 0;
}

/*
 *  WAS_DISABLED -> WAS_DISABLED
 *  WAS_ENABLED  -> RELEASED
 *  RELEASED     -> RELEASED
 */
void SmartPtr_IRQ::Release()
{
    __set_PRIMASK(m_state);
}

/*
 *  WAS_DISABLED -> WAS_DISABLED
 *  WAS_ENABLED  -> WAS_ENABLED
 *  RELEASED     -> WAS_ENABLED
 */
void SmartPtr_IRQ::Acquire()
{
    __disable_irq();
}

/*
 *  WAS_DISABLED -> WAS_DISABLED
 *  WAS_ENABLED  -> enabled -> WAS_ENABLED
 *  RELEASED     -> RELEASED
 */
void SmartPtr_IRQ::Probe()
{
    UINT32 m = __get_PRIMASK();
    __set_PRIMASK(m_state);
    __nop;
    __set_PRIMASK(m);
}


// static members

BOOL SmartPtr_IRQ::GetState(void* context)
{
    // Also check for interrupt state == 0
    return __get_PRIMASK() == 0 && __get_IPSR() == 0;
}

BOOL SmartPtr_IRQ::ForceDisabled(void* context)
{
    UINT32 m = __get_PRIMASK();
    __disable_irq();
    return m ^ 1;
}

BOOL SmartPtr_IRQ::ForceEnabled(void* context)
{
    UINT32 m = __get_PRIMASK();
    __enable_irq();
    return m ^ 1;
}


// private members (not used)

void SmartPtr_IRQ::Disable()
{
    m_state = __get_PRIMASK();
    __disable_irq();
}

void SmartPtr_IRQ::Restore()
{
    __set_PRIMASK(m_state);
}


