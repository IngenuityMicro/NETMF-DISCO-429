////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
//
//  Copyright (c) Oberon microsystems, Inc.
//
//  *** STM32F4 Random Number Generator interop ***
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "System_Security_Cryptography_Netmf.h"
#include "System_Security_Cryptography_Netmf_System_Security_Cryptography_Stm32F4Random.h"

using namespace System::Security::Cryptography;

#include <cores\arm\include\cpu.h>
#include "STM32F4_Random_functions.h"

void Stm32F4Random::NativeEnable( HRESULT &hr )
{
    Random_Start();
}

void Stm32F4Random::NativeDisable( HRESULT &hr )
{
    Random_Stop();
}

void Stm32F4Random::NativeGetBytes( CLR_RT_TypedArray_UINT8 param0, HRESULT &hr )
{
    unsigned char* arr = param0.GetBuffer();
    unsigned int length = param0.GetSize();
    unsigned int offset = 0;

    UINT32 rand;

    if (length > sizeof(rand))
    {
        for (offset = 0; offset < length-sizeof(rand); offset += sizeof(rand))
        {
            rand = Random_Generate();
            memcpy(arr+offset, &rand, sizeof(rand));
        }
    }
    if (offset < length)
    {
        unsigned int difference = length - offset;
        rand = Random_Generate();
        memcpy(arr+offset, &rand, difference);
    }
    
}

