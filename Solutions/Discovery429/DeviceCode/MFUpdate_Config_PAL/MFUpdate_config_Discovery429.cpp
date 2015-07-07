////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
//
//  Copyright (c) Oberon microsystems, Inc.
//
//  *** MFUpdate Configuration ***
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <tinyhal.h>
#include <mfupdate_decl.h>
#include <Drivers\MFUpdate\MicroBooterUpdate.h>

static const IUpdatePackage s_UpdatePackages[] =
{
    {
        "NetMF",
        &g_MicroBooterUpdateProvider,
        &g_CrcUpdateValidationProvider,
        &g_BlockStorageUpdateProvider,
        NULL,
    }, 
};

const IUpdatePackage* g_UpdatePackages     = s_UpdatePackages;
const INT32           g_UpdatePackageCount = ARRAYSIZE(s_UpdatePackages);


