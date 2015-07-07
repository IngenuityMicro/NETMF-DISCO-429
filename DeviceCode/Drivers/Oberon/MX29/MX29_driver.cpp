////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
//
//  Copyright (c) Microsoft Corporation. All rights reserved.
//  Implementation for MX29: Copyright (c) Oberon microsystems, Inc.
//
//  *** MX29 Flash Driver ***
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <tinyhal.h>
#include "MX29.h"


//--//
BOOL __section(SectionForFlashOperations) MX29_Flash_Driver::ChipInitialize( void* context )
{
    return TRUE;
}


BOOL __section(SectionForFlashOperations) MX29_Flash_Driver::ChipUnInitialize( void* context )
{
    return TRUE;
}


const BlockDeviceInfo* __section(SectionForFlashOperations) MX29_Flash_Driver::GetDeviceInfo( void* context )
{
    return ((MEMORY_MAPPED_NOR_BLOCK_CONFIG*)context)->BlockConfig.BlockDeviceInformation;
}


BOOL __section(SectionForFlashOperations) MX29_Flash_Driver::IsBlockErased( void* context, ByteAddress address, UINT32 blockLength )
{
    NATIVE_PROFILE_HAL_DRIVERS_FLASH();

    address = CPU_GetUncachableAddress(address);

    volatile CHIP_WORD * ChipAddress = (volatile CHIP_WORD *) address;

    CHIP_WORD * EndAddress = (CHIP_WORD*)(address + blockLength);
    
    while(ChipAddress < EndAddress)
    {
        if( (*ChipAddress ) != 0xFFFF)
        {
            return FALSE;   
        }
        ChipAddress ++;
    }
    return TRUE;
}

BOOL __section(SectionForFlashOperations) MX29_Flash_Driver::EraseBlock( void* context, ByteAddress address )
{
    NATIVE_PROFILE_HAL_DRIVERS_FLASH();
    
    BOOL result;
    CHIP_WORD * ChipAddress;
    
    UINT32 iRegion, iRange;

    MEMORY_MAPPED_NOR_BLOCK_CONFIG* config = (MEMORY_MAPPED_NOR_BLOCK_CONFIG*)context;
    const BlockDeviceInfo *    deviceInfo = config->BlockConfig.BlockDeviceInformation;

    if (deviceInfo->Attribute.WriteProtected) return FALSE;
    
    if (!deviceInfo->FindRegionFromAddress(address, iRegion, iRange)) return FALSE;
    
    UINT32 blockSize = deviceInfo->Regions[iRegion].BytesPerBlock;

    if (IsBlockErased(context, address, blockSize)) return TRUE;

    address -= (address % blockSize);

    ChipAddress = (CHIP_WORD *) address;
    
    result =  Action_EraseSector( context, ChipAddress );

    return result;
}


BOOL __section(SectionForFlashOperations) MX29_Flash_Driver::Read(void* context, ByteAddress address, UINT32 numBytes, BYTE * pSectorBuff)
{
    NATIVE_PROFILE_HAL_DRIVERS_FLASH();

    CHIP_WORD* ChipAddress, *EndAddress;


#if defined(_DEBUG)    
    UINT32 iRegion, iRange;

    MEMORY_MAPPED_NOR_BLOCK_CONFIG* config = (MEMORY_MAPPED_NOR_BLOCK_CONFIG*)context;
    const BlockDeviceInfo *    deviceInfo = config->BlockConfig.BlockDeviceInformation;
    if(!deviceInfo->FindRegionFromAddress(address, iRegion, iRange)) return FALSE;
#endif

    address = CPU_GetUncachableAddress(address);

    ChipAddress = (CHIP_WORD *) address;
    EndAddress  = (CHIP_WORD *)(address + numBytes);

    CHIP_WORD *pBuf = (CHIP_WORD *)pSectorBuff;

    while(ChipAddress < EndAddress)
    {
        *pBuf++ = *ChipAddress++;
    }

    return TRUE;
}

BOOL __section(SectionForFlashOperations) MX29_Flash_Driver::Write(void* context, ByteAddress Address, UINT32 NumBytes, BYTE * pSectorBuff, BOOL ReadModifyWrite)
{

    NATIVE_PROFILE_PAL_FLASH();

    BYTE * pData;
    BYTE * pBuf = NULL;
    MEMORY_MAPPED_NOR_BLOCK_CONFIG* config = (MEMORY_MAPPED_NOR_BLOCK_CONFIG*)context;
    const BlockDeviceInfo * deviceInfo = config->BlockConfig.BlockDeviceInformation;
    
    UINT32 region, range;

    if(ReadModifyWrite) 
    {
        BOOL fRet = TRUE;
        
        if(!deviceInfo->FindRegionFromAddress(Address, region, range)) return FALSE;

        UINT32      bytesPerBlock   = deviceInfo->Regions[region].BytesPerBlock;
        UINT32      regionEnd       = deviceInfo->Regions[region].Start + deviceInfo->Regions[region].Size();
        UINT32      offset          = Address % bytesPerBlock;
        ByteAddress addr            = Address;
        ByteAddress addrEnd         = Address + NumBytes;
        UINT32      index           = 0;

        pBuf = (BYTE*)private_malloc(bytesPerBlock);

        if(pBuf == NULL)
        {
 
            return FALSE;
        }

        while(fRet && addr < addrEnd)
        {
            ByteAddress sectAddr = (addr - offset);
             
            if(offset == 0 && NumBytes >= bytesPerBlock)
            {
                pData = &pSectorBuff[index];
            }
            else
            {
                int bytes = __min(bytesPerBlock - offset, NumBytes); 
                
                memcpy( &pBuf[0]     , (void*)sectAddr    , bytesPerBlock );
                memcpy( &pBuf[offset], &pSectorBuff[index], bytes         );

                pData = pBuf;
            }

            if(!EraseBlock( context, sectAddr ))
            {
                fRet = FALSE;
                break;
            }

            fRet = WriteX(context, sectAddr, bytesPerBlock, pData, ReadModifyWrite, TRUE);

            NumBytes -= bytesPerBlock - offset;
            addr     += bytesPerBlock - offset;
            index    += bytesPerBlock - offset;
            offset    = 0;

            if(NumBytes > 0 && addr >= regionEnd)
            {
                region++;

                if(region >= deviceInfo->NumRegions)
                {
                    fRet = FALSE;
                }
                else
                {
                    regionEnd       = deviceInfo->Regions[region].Start + deviceInfo->Regions[region].Size();
                    bytesPerBlock   = deviceInfo->Regions[region].BytesPerBlock;

                    private_free(pBuf);

                    pBuf = (BYTE*)private_malloc(bytesPerBlock);

                    if(pBuf == NULL)
                    {
                        fRet = FALSE;
                    }
                }
            }
                
        }

        if(pBuf != NULL)
        {
            private_free(pBuf);
        }

        return fRet;            
    }
    else
    {
        return WriteX(context, Address, NumBytes, pSectorBuff, ReadModifyWrite, TRUE);
    }
    
}

BOOL __section(SectionForFlashOperations) MX29_Flash_Driver::Memset(void* context, ByteAddress address, UINT8 data, UINT32 numBytes)
{
    NATIVE_PROFILE_PAL_FLASH();

    CHIP_WORD chipData;

    memset(&chipData, data, sizeof(CHIP_WORD));

    return WriteX(context, address, numBytes, (BYTE*)&chipData, TRUE, FALSE);
}


BOOL __section(SectionForFlashOperations) MX29_Flash_Driver::WriteX(void* context, ByteAddress address, UINT32 numBytes, BYTE * pSectorBuff, BOOL ReadModifyWrite, BOOL fIncrementDataPtr)
{
    NATIVE_PROFILE_PAL_FLASH();

    MEMORY_MAPPED_NOR_BLOCK_CONFIG* config = (MEMORY_MAPPED_NOR_BLOCK_CONFIG*)context;
    const BlockDeviceInfo *    deviceInfo = config->BlockConfig.BlockDeviceInformation;

    CHIP_WORD* ChipAddress;
    CHIP_WORD* EndAddress, *pData;
    BOOL result = TRUE;


    if (deviceInfo->Attribute.WriteProtected) return FALSE;

    address = CPU_GetUncachableAddress(address);

    ChipAddress = (CHIP_WORD *)address;
    EndAddress  = (CHIP_WORD *)(address + numBytes); 
    pData       = (CHIP_WORD *)pSectorBuff;

   
    while(ChipAddress < EndAddress)
    {
       // if same, nothing to do, continue nextword.
       if(*ChipAddress != *pData) 
       {
            // check for having to move bits from 0->1, a failure case for a write
            if(0 != (*pData  & ~(*ChipAddress)))
            {
                debug_printf( "Write X erase failure: 0x%08x=0x%04x\r\n", (size_t)ChipAddress, *ChipAddress );
                ASSERT(0);
                result =FALSE;
                break;
            }

            Action_WriteWord( config, ChipAddress, *pData );

            
            if (*ChipAddress != *pData)
            {
                debug_printf( "Flash_WriteToSector failure @ 0x%08x, wrote 0x%08x, read 0x%08x\r\n", (UINT32)ChipAddress, *pData, *ChipAddress );
                result = FALSE;
                break;
            }
        }

        ChipAddress++;
        if(fIncrementDataPtr) pData++;    
    }

    return result;
}

BOOL __section(SectionForFlashOperations) MX29_Flash_Driver::GetSectorMetadata(void* context, ByteAddress SectorStart, SectorMetadata* pSectorMetadata)
{
    return TRUE;
}

BOOL __section(SectionForFlashOperations) MX29_Flash_Driver::SetSectorMetadata(void* context, ByteAddress SectorStart, SectorMetadata* pSectorMetadata)
{
    return TRUE;
}


void  __section(SectionForFlashOperations) MX29_Flash_Driver::SetPowerState(void* context, UINT32 State )
{
    // our flash driver is always ON
    return ;
}


UINT32 __section(SectionForFlashOperations) MX29_Flash_Driver::MaxSectorWrite_uSec( void* context )
{
    NATIVE_PROFILE_PAL_FLASH();

    MEMORY_MAPPED_NOR_BLOCK_CONFIG* config = (MEMORY_MAPPED_NOR_BLOCK_CONFIG*)context;
    
    return config->BlockConfig.BlockDeviceInformation->MaxSectorWrite_uSec;
}


UINT32 __section(SectionForFlashOperations) MX29_Flash_Driver::MaxBlockErase_uSec( void* context )
{
    NATIVE_PROFILE_PAL_FLASH();

    MEMORY_MAPPED_NOR_BLOCK_CONFIG* config = (MEMORY_MAPPED_NOR_BLOCK_CONFIG*)context;

    return config->BlockConfig.BlockDeviceInformation->MaxBlockErase_uSec;
}

//--//

BOOL __section(SectionForFlashOperations) MX29_Flash_Driver::Action_EraseSector( void* context, volatile CHIP_WORD* SectorStart )
{
    NATIVE_PROFILE_HAL_DRIVERS_FLASH();

    MEMORY_MAPPED_NOR_BLOCK_CONFIG* config = (MEMORY_MAPPED_NOR_BLOCK_CONFIG*)context;
    
    volatile CHIP_WORD* BaseAddress = (CHIP_WORD*)config->BlockConfig.BlockDeviceInformation->Regions[0].Start;

    // Erase command sequence
    BaseAddress[0x0555] = 0x00AA;
    BaseAddress[0x02AA] = 0x0055;
    BaseAddress[0x0555] = 0x0080;
    BaseAddress[0x0555] = 0x00AA;
    BaseAddress[0x02AA] = 0x0055;

    *SectorStart = 0x0030; // start erase
    
    // wait for device to signal completion
    while (TRUE) {
        // read twice to asure Q7 cannot be older than Q5
        CHIP_WORD status1 = *SectorStart; // get status for Q5
        CHIP_WORD status2 = *SectorStart; // get status for Q7
        if (status2 & 0x0080) { // done
            return TRUE;
        }
        if (status1 & 0x0020) { // timeout
            BaseAddress[0x0] = 0x00F0; // reset mode
            return FALSE;
        }
    }
}

void __section(SectionForFlashOperations) MX29_Flash_Driver::Action_WriteWord( void* context, volatile CHIP_WORD* Address, CHIP_WORD Data )
{
    NATIVE_PROFILE_HAL_DRIVERS_FLASH();

    MEMORY_MAPPED_NOR_BLOCK_CONFIG* config = (MEMORY_MAPPED_NOR_BLOCK_CONFIG*)context;
    
    volatile CHIP_WORD* BaseAddress = (CHIP_WORD *)config->BlockConfig.BlockDeviceInformation->Regions[0].Start;
     
    // Program command sequence
    BaseAddress[0x0555] = 0x00AA;
    BaseAddress[0x02AA] = 0x0055;
    BaseAddress[0x0555] = 0x00A0;

    *Address = Data; // start data write

    // wait for device to signal completion
    while (TRUE) {
        // read twice to asure Q7 cannot be older than Q5
        CHIP_WORD status1 = *Address; // get status for Q5
        CHIP_WORD status2 = *Address; // get status for Q7
        if ((status2 & 0x0080) == (Data & 0x0080)) { // done
            return;
        }
        if (status1 & 0x0020) { // timeout
            BaseAddress[0x0] = 0x00F0; // reset mode
            return;
        }
    }
}

#if defined(ADS_LINKER_BUG__NOT_ALL_UNUSED_VARIABLES_ARE_REMOVED)
#pragma arm section rodata = "g_MX29_BS_DeviceTable"
#endif

struct IBlockStorageDevice g_MX29_BS_DeviceTable = 
{
    &MX29_Flash_Driver::ChipInitialize,
    &MX29_Flash_Driver::ChipUnInitialize,
    &MX29_Flash_Driver::GetDeviceInfo,
    &MX29_Flash_Driver::Read,
    &MX29_Flash_Driver::Write,
    &MX29_Flash_Driver::Memset,
    &MX29_Flash_Driver::GetSectorMetadata,
    &MX29_Flash_Driver::SetSectorMetadata,
    &MX29_Flash_Driver::IsBlockErased,
    &MX29_Flash_Driver::EraseBlock,
    &MX29_Flash_Driver::SetPowerState,
    &MX29_Flash_Driver::MaxSectorWrite_uSec,
    &MX29_Flash_Driver::MaxBlockErase_uSec,     
};

#if defined(arm) || defined(__arm)
#pragma arm section rodata 
#endif

