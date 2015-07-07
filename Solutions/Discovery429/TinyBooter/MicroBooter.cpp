////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Microsoft Corporation.  All rights reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <MicroBooter_decl.h>
#include <MFUpdate_decl.h>
#include <Drivers\MFUpdate\Storage\BlockStorageUpdate.h>


static UINT8 s_WriteBuffer[512];
static INT32 s_WriteBufferIndex = 0;

static UINT8* s_ReadBuffer = NULL;

static BlockStorageStream            s_MemoryStreamDst;
static const IUpdateStorageProvider* s_MemoryStreamSrc;
static INT32                         s_MemoryStreamSrcOffset = 0;
static INT32                         s_MemoryStreamSrcHandle = -1;
static UINT32                        s_MemoryStreamSrcEraseSize = 0;


static const IUpdateStorageProvider* s_UpdateStorageList[] =
{
    &g_BlockStorageUpdateProvider
};

const IUpdateStorageProvider** g_MicroBooter_UpdateStorageList      = s_UpdateStorageList;
UINT32                         g_MicroBooter_UpdateStorageListCount = ARRAYSIZE(s_UpdateStorageList);


// so far we are assuming a one byte read/write in sequential order (as required by compression/decompression)
static BOOL Memory_Write( UINT32 address, UINT32 length, const BYTE* data )
{
    ASSERT(length == 1);

    s_WriteBuffer[s_WriteBufferIndex] = *data;

    s_WriteBufferIndex += length;
    
    if(s_WriteBufferIndex < sizeof(s_WriteBuffer))
    {
        return TRUE;
    }

    s_WriteBufferIndex = 0;

    if(s_MemoryStreamDst.CurrentUsage == BlockUsage::BOOTSTRAP) return FALSE;

    if(0 == (s_MemoryStreamDst.CurrentIndex % s_MemoryStreamDst.BlockLength))
    {
        s_MemoryStreamDst.Erase( s_MemoryStreamDst.BlockLength );
    }

    return s_MemoryStreamDst.Write( (BYTE*)s_WriteBuffer, sizeof(s_WriteBuffer) );
}

static BOOL Memory_Flush()
{
    if(s_WriteBufferIndex > 0)
    {
        BOOL fRet = s_MemoryStreamDst.Write( (BYTE*)s_WriteBuffer, s_WriteBufferIndex );

        s_WriteBufferIndex = 0;

        return fRet;
    }

    return TRUE;
}

static BOOL Memory_Read( UINT32 address, UINT32 length, BYTE* data )
{
    INT32 readBufferSize = 512;
    INT32 index = (address % readBufferSize);

    ASSERT(length == 1);
    
    if(s_ReadBuffer == NULL)
    {
        s_ReadBuffer = (UINT8*)private_malloc(readBufferSize);

        ASSERT(s_ReadBuffer != NULL);
        if(s_ReadBuffer == NULL) return FALSE;

        if(0 != index)
        {
            s_MemoryStreamSrc->Read( s_MemoryStreamSrcHandle, s_MemoryStreamSrcOffset - index, s_ReadBuffer, readBufferSize );

            s_MemoryStreamSrcOffset += readBufferSize - index;
        }
    }

    if(s_ReadBuffer != NULL)
    {
        if(0 == index)
        {
            if(readBufferSize <= ((UINT32)&HeapEnd - (UINT32)&HeapBegin))
            {
                s_MemoryStreamSrc->Read( s_MemoryStreamSrcHandle, s_MemoryStreamSrcOffset, s_ReadBuffer, readBufferSize );

                s_MemoryStreamSrcOffset += readBufferSize;
            }
            else
            {
                return s_MemoryStreamSrc->Read( s_MemoryStreamSrcHandle, s_MemoryStreamSrcOffset++, data, length );
            }
        } 

        *data = s_ReadBuffer[index];

        return TRUE;
    }
    
    return s_MemoryStreamSrc->Read(s_MemoryStreamSrcHandle, s_MemoryStreamSrcOffset++, data, length );
}

BOOL MemStreamSeekBlockAddress( BlockStorageStream &stream, UINT32 address )
{
    if(!stream.Initialize( BlockUsage::ANY )) return FALSE;

    while(!(stream.BaseAddress <= address && address < (stream.BaseAddress + stream.Length)))
    {
        if(!stream.NextStream()) return FALSE;
    }

    while(!(stream.CurrentAddress() <= address && address < (stream.CurrentAddress() + stream.BlockLength)))
    {
        if(!stream.Seek( BlockStorageStream::STREAM_SEEK_NEXT_BLOCK, BlockStorageStream::SeekCurrent ))
        {
            return FALSE;
        }
    }

    return TRUE;
}

BOOL MicroBooter_PerformSigCheck(MFUpdateHeader& header, INT32 storageHandle, HAL_UPDATE_CONFIG cfg)
{
    if(cfg.UpdateSignType == HAL_UPDATE_CONFIG_SIGN_TYPE__CRC)
    {
        UINT32 offset = 0;
        UINT8 buf[512];
        INT32 len = sizeof(buf);
        UINT32 crc = 0;
        
        while(offset < header.UpdateSize)
        {
            if((offset + len) > header.UpdateSize)
            {
                len = header.UpdateSize - offset;
            }

            s_MemoryStreamSrc->Read( storageHandle, offset, buf, len );

            crc = SUPPORT_ComputeCRC(buf, len, crc);

            offset += len;
        }

        return crc == *(UINT32*)cfg.UpdateSignature;
    }
    
    return TRUE;
}

BOOL MicroBooter_Install(HAL_UPDATE_CONFIG updateCfg)
{
    MFUpdateHeader header;
    UINT32 src = 0, srcEnd;
    UINT32 dest, destEnd;
    CompressedImage_Header hdr;

    for(INT32 i=0; i<g_MicroBooter_UpdateStorageListCount; i++)
    {
        s_MemoryStreamSrc = g_MicroBooter_UpdateStorageList[i];
    
        s_MemoryStreamSrcHandle = s_MemoryStreamSrc->Open(updateCfg.UpdateID, MFUPDATE_UPDATETYPE_FIRMWARE, MFUPDATE_UPDATESUBTYPE_ANY);

        if(s_MemoryStreamSrcHandle != -1) break;
    }

    if(s_MemoryStreamSrcHandle == -1) return FALSE;

    s_MemoryStreamSrcOffset = sizeof(CompressedImage_Header);

    s_MemoryStreamSrcEraseSize = s_MemoryStreamSrc->GetEraseSize( s_MemoryStreamSrcHandle );
    
    if(!s_MemoryStreamSrc->GetHeader(s_MemoryStreamSrcHandle, &header)) return FALSE;

    if(!MicroBooter_PerformSigCheck(header, s_MemoryStreamSrcHandle, updateCfg)) return FALSE;
    
    if (header.UpdateSubType == MFUPDATE_UPDATESUBTYPE_USER_DEFIND) {
        // erase Deployment region
        BlockStorageStream stream;
        if (stream.Initialize(BlockUsage::DEPLOYMENT)) {
            do stream.Erase(stream.Length); while(stream.NextStream());
        }
    }

    while(src < header.UpdateSize)
    {
        if(!s_MemoryStreamSrc->Read(s_MemoryStreamSrcHandle, src, (UINT8*)&hdr, sizeof(hdr))) return FALSE;

        srcEnd = hdr.Compressed;
        dest = 0; 
        destEnd = hdr.Uncompressed;

        if(!MemStreamSeekBlockAddress( s_MemoryStreamDst, (UINT32)hdr.Destination )) return FALSE;
        
        if( -1 == LZ77_Decompress((UINT8*)src, srcEnd, (UINT8*)dest, destEnd, Memory_Write, Memory_Read)) return FALSE;

        Memory_Flush();

        src += hdr.Compressed + sizeof(hdr);

        if(0 != (src % 4))
        {
            src += (4 - (src % 4));
        }
    }

    return TRUE;
}


