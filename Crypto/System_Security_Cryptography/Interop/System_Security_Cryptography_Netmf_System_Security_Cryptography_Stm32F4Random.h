//-----------------------------------------------------------------------------
//
//                   ** WARNING! ** 
//    This file was generated automatically by a tool.
//    Re-running the tool will overwrite this file.
//    You should copy this file to a custom location
//    before adding any customization in the copy to
//    prevent loss of your changes when the tool is
//    re-run.
//
//-----------------------------------------------------------------------------


#ifndef _SYSTEM_SECURITY_CRYPTOGRAPHY_NETMF_SYSTEM_SECURITY_CRYPTOGRAPHY_STM32F4RANDOM_H_
#define _SYSTEM_SECURITY_CRYPTOGRAPHY_NETMF_SYSTEM_SECURITY_CRYPTOGRAPHY_STM32F4RANDOM_H_

namespace System
{
    namespace Security
    {
        namespace Cryptography
        {
            struct Stm32F4Random
            {
                // Helper Functions to access fields of managed object
                static INT8& Get__enabled( CLR_RT_HeapBlock* pMngObj )    { return Interop_Marshal_GetField_INT8( pMngObj, Library_System_Security_Cryptography_Netmf_System_Security_Cryptography_Stm32F4Random::FIELD___enabled ); }

                // Declaration of stubs. These functions are implemented by Interop code developers
                static void NativeEnable( HRESULT &hr );
                static void NativeDisable( HRESULT &hr );
                static void NativeGetBytes( CLR_RT_TypedArray_UINT8 param0, HRESULT &hr );
            };
        }
    }
}
#endif  //_SYSTEM_SECURITY_CRYPTOGRAPHY_NETMF_SYSTEM_SECURITY_CRYPTOGRAPHY_STM32F4RANDOM_H_
