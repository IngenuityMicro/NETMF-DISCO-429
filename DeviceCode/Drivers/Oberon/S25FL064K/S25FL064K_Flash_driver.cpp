////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
//
//  Copyright (c) Microsoft Corporation. All rights reserved.
//  Implementation for Mountaineer board (STM32): Copyright (c) Oberon microsystems, Inc.
//
//  *** S25FL064K Flash Driver ***
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <tinyhal.h>
#include "S25FL064K_Flash.h"


#define WrEn_Instr  0x06  // write enable
#define RdSt_Instr  0x05  // read status
#define Read_Instr  0x0B  // fast read
#define Write_Instr 0x02  // page program
#define Erase_Instr 0x20  // sector erase (4k)

#define S25FL064K_BLOCK_SIZE 4096


const SPI_CONFIGURATION g_S25FL064K_spi = {
    (GPIO_PIN)32,         // DeviceCS       // CS on pin PC0
    FALSE,                // CS_Active      // low active
    TRUE,                 // MSK_IDLE       // idle high
    TRUE,                 // MSK_SampleEdge // sample on rising
    FALSE,                // MD_16bits      // 8 bits
    //80000,                // Clock_RateKHz  // 80MHz
    21000,                // Clock_RateKHz  // 21MHz
    0,                    // CS_Setup_usec  // no setup delay
    0,                    // CS_Hold_usec   // no hold delay
    0,                    // SPI_mod        // device on SPI1
    {GPIO_PIN_NONE,FALSE} // BusyPin        // none
};


BOOL S25FL064K_EnableWrite()
{
    NATIVE_PROFILE_HAL_DRIVERS_FLASH();
    
    BYTE en_cmd = WrEn_Instr;
    return CPU_SPI_nWrite8_nRead8(g_S25FL064K_spi, &en_cmd, 1, NULL, 0, 0);
}
    
BOOL S25FL064K_WaitReady()
{
    NATIVE_PROFILE_HAL_DRIVERS_FLASH();
    
    BYTE rd_cmd = RdSt_Instr;
    BYTE status;
    
    do {
        if (!CPU_SPI_nWrite8_nRead8(g_S25FL064K_spi, &rd_cmd, 1, &status, 1, 1)) return FALSE;
    } while (status & 1); // Write_In_Progress bit set
    
    return TRUE;
}
    

//--//

    /////////////////////////////////////////////////////////
    // Description:
    //    Initializes a given block device for use
    // 
    // Input:
    //
    // Returns:
    //   true if succesful; false if not
    //
    // Remarks:
    //    No other functions in this interface may be called
    //    until after Init returns.
    //
BOOL S25FL064K_Flash_Driver::ChipInitialize( void* context )
{
    NATIVE_PROFILE_HAL_DRIVERS_FLASH();

    return TRUE;
}

    /////////////////////////////////////////////////////////
    // Description:
    //    Initializes a given block device for use
    // 
    // Returns:
    //   true if succesful; false if not
    //
    // Remarks:
    //   De initializes the device when no longer needed
    //
BOOL S25FL064K_Flash_Driver::ChipUnInitialize( void* context )
{
    NATIVE_PROFILE_HAL_DRIVERS_FLASH();

    return TRUE;
}

    /////////////////////////////////////////////////////////
    // Description:
    //    Gets the information describing the device
    //
const BlockDeviceInfo* S25FL064K_Flash_Driver::GetDeviceInfo( void* context )
{
    BLOCK_CONFIG* config = (BLOCK_CONFIG*)context;
    
    return config->BlockDeviceInformation;    
}

    /////////////////////////////////////////////////////////
    // Description:
    //    Reads data from a set of sectors
    //
    // Input:
    //    StartSector - Starting Sector for the read
    //    NumSectors  - Number of sectors to read
    //    pSectorBuff - pointer to buffer to read the data into.
    //                  Must be large enough to hold all of the data
    //                  being read.
    //
    // Returns:
    //   true if succesful; false if not
    //
    // Remarks:
    //   This function reads the number of sectors specified from the device.
    //   
BOOL  S25FL064K_Flash_Driver::Read( void* context, ByteAddress StartSector, UINT32 NumBytes, BYTE * pSectorBuff)
{
    NATIVE_PROFILE_HAL_DRIVERS_FLASH();

    if (pSectorBuff == NULL) return FALSE;
    
    BYTE rd_cmd[4] = {Read_Instr,
                      (BYTE)(StartSector >> 16),
                      (BYTE)(StartSector >> 8),
                      (BYTE)StartSector};

    S25FL064K_WaitReady();
    return CPU_SPI_nWrite8_nRead8(g_S25FL064K_spi, rd_cmd, 4, pSectorBuff, NumBytes, 5);
}


BOOL S25FL064K_Flash_Driver::WriteX( void* context, ByteAddress Address, UINT32 NumBytes, BYTE * pSectorBuff, BOOL increment )
{
    NATIVE_PROFILE_PAL_FLASH();
    
    BYTE wr_cmd[260];
    wr_cmd[0] = Write_Instr;
    
    UINT32 len = 256 - (Address & 0xFF); // rest of first page
    
    while(NumBytes > 0) {
        wr_cmd[1] = (BYTE)(Address >> 16);
        wr_cmd[2] = (BYTE)(Address >> 8);
        wr_cmd[3] = (BYTE)Address;
        if (len > NumBytes) len = NumBytes;
        for (int i = 4; i < len + 4; i++) {
            wr_cmd[i] = *pSectorBuff;
            if (increment) pSectorBuff++;
        }
        // ready?
        if (!S25FL064K_EnableWrite()) return FALSE;
        // write sector
        S25FL064K_WaitReady();        
        if (!CPU_SPI_nWrite8_nRead8(g_S25FL064K_spi, wr_cmd, len + 4, NULL, 0, 0)) return FALSE;
        Address += len;
        NumBytes -= len;
        len = 256;
        if (!S25FL064K_WaitReady()) return FALSE;
    }
    
    return TRUE;
}

    /////////////////////////////////////////////////////////
    // Description:
    //    Writes data to a set of sectors
    //
    // Input:
    //    StartSector - Starting Sector for the write
    //    NumSectors  - Number of sectors to write
    //    pSectorBuff - pointer to data to write.
    //                  Must be large enough to hold complete sectors
    //                  for the number of sectors being written.
    //
    // Returns:
    //   true if succesful; false if not
    //
    // Remarks:
    //   This function reads the number of sectors specified from the device.
    //   
BOOL S25FL064K_Flash_Driver::Write(void* context, ByteAddress Address, UINT32 NumBytes, BYTE * pSectorBuff, BOOL ReadModifyWrite)
{
    NATIVE_PROFILE_PAL_FLASH();
    
    if (ReadModifyWrite) {
        ByteAddress block = (ByteAddress)(Address & ~(S25FL064K_BLOCK_SIZE - 1));
        ByteAddress end = (ByteAddress)(Address + NumBytes);
        do {
            if (!EraseBlock(context, block)) return FALSE;
            block += S25FL064K_BLOCK_SIZE;
        } while (block < end);
    }
    
    return WriteX(context, Address, NumBytes, pSectorBuff, TRUE);
}

BOOL S25FL064K_Flash_Driver::Memset(void* context, ByteAddress Address, UINT8 Data, UINT32 NumBytes)
{
    NATIVE_PROFILE_PAL_FLASH();

    return WriteX(context, Address, NumBytes, &Data, FALSE);
}


BOOL S25FL064K_Flash_Driver::GetSectorMetadata(void* context, ByteAddress SectorStart, SectorMetadata* pSectorMetadata)
{
    // no metadata
    return FALSE;
}

BOOL S25FL064K_Flash_Driver::SetSectorMetadata(void* context, ByteAddress SectorStart, SectorMetadata* pSectorMetadata)
{
    // no metadata
    return FALSE;
}


    /////////////////////////////////////////////////////////
    // Description:
    //    Check a block is erased or not.
    // 
    // Input:
    //    BlockStartAddress - Logical Sector Address
    //
    // Returns:
    //   true if it is erassed, otherwise false
    //
    // Remarks:
    //    Check  the block containing the sector address specified.
    //    
BOOL S25FL064K_Flash_Driver::IsBlockErased( void* context, ByteAddress BlockStart, UINT32 BlockLength )
{
    NATIVE_PROFILE_HAL_DRIVERS_FLASH();

    BYTE buffer[256];
    
    while(BlockLength > 0)
    {
        int len = BlockLength;
        if (len > 256) len = 256;

        if (!Read(context, BlockStart, len, buffer)) return FALSE;
        
        for (int i = 0; i < len; i++) {
            if (buffer[i] != 0xFF) return FALSE;
        }
        BlockStart += len;
        BlockLength -= len;
    }
    
    return TRUE;
}


    /////////////////////////////////////////////////////////
    // Description:
    //    Erases a block
    // 
    // Input:
    //    Address - Logical Sector Address
    //
    // Returns:
    //   true if succesful; false if not
    //
    // Remarks:
    //    Erases the block containing the sector address specified.
    //    
BOOL S25FL064K_Flash_Driver::EraseBlock( void* context, ByteAddress Sector )
{
    NATIVE_PROFILE_HAL_DRIVERS_FLASH();
    
    if (IsBlockErased(context, Sector, S25FL064K_BLOCK_SIZE)) return TRUE;

    if (!S25FL064K_EnableWrite()) return FALSE;

    // erase block
    BYTE er_cmd[4] = {Erase_Instr,
                      (BYTE)(Sector >> 16),
                      (BYTE)(Sector >> 8),
                      (BYTE)Sector};
                    
    S25FL064K_WaitReady();
    if (!CPU_SPI_nWrite8_nRead8(g_S25FL064K_spi, er_cmd, 4, NULL, 0, 0)) return FALSE;
    return S25FL064K_WaitReady();
}



    /////////////////////////////////////////////////////////
    // Description:
    //   Changes the power state of the device
    // 
    // Input:
    //    State   - true= power on; false = power off
    //
    // Remarks:
    //   This function allows systems to conserve power by 
    //   shutting down the hardware when the system is 
    //   going into low power states.
    //
void  S25FL064K_Flash_Driver::SetPowerState( void* context, UINT32 State )
{
    // no setable power state
}


//--// ---------------------------------------------------

#pragma arm section code = "SectionForFlashOperations"

UINT32 S25FL064K_Flash_Driver::MaxSectorWrite_uSec( void* context )
{
    NATIVE_PROFILE_PAL_FLASH();

    BLOCK_CONFIG* config = (BLOCK_CONFIG*)context;
    
    return config->BlockDeviceInformation->MaxSectorWrite_uSec;
}


UINT32 S25FL064K_Flash_Driver::MaxBlockErase_uSec( void* context )
{
    NATIVE_PROFILE_PAL_FLASH();
    
    BLOCK_CONFIG* config = (BLOCK_CONFIG*)context;

    return config->BlockDeviceInformation->MaxBlockErase_uSec;
    
}


#if defined(ADS_LINKER_BUG__NOT_ALL_UNUSED_VARIABLES_ARE_REMOVED)
#pragma arm section rodata = "g_S25FL064K_Flash_DeviceTable"
#endif

struct IBlockStorageDevice g_S25FL064K_Flash_DeviceTable = 
{                          
    &S25FL064K_Flash_Driver::ChipInitialize,
    &S25FL064K_Flash_Driver::ChipUnInitialize,
    &S25FL064K_Flash_Driver::GetDeviceInfo,
    &S25FL064K_Flash_Driver::Read,
    &S25FL064K_Flash_Driver::Write,
    &S25FL064K_Flash_Driver::Memset,    
    &S25FL064K_Flash_Driver::GetSectorMetadata,
    &S25FL064K_Flash_Driver::SetSectorMetadata,
    &S25FL064K_Flash_Driver::IsBlockErased,
    &S25FL064K_Flash_Driver::EraseBlock,
    &S25FL064K_Flash_Driver::SetPowerState,
    &S25FL064K_Flash_Driver::MaxSectorWrite_uSec,
    &S25FL064K_Flash_Driver::MaxBlockErase_uSec,    
};

#if defined(ADS_LINKER_BUG__NOT_ALL_UNUSED_VARIABLES_ARE_REMOVED)
#pragma arm section rodata 
#endif 






