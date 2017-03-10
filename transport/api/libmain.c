/*
 * Copyright © 2012-2015 VMware, Inc.  All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the “License”); you may not
 * use this file except in compliance with the License.  You may obtain a copy
 * of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an “AS IS” BASIS, without
 * warranties or conditions of any kind, EITHER EXPRESS OR IMPLIED.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */


#include "includes.h"

DWORD
VmwSockInitialize(
    PVMREST_HANDLER                    pRESTHandler
    )
{
    DWORD dwError = 0;

    if (pRESTHandler)
    {
#ifdef _WIN32
        dwError = VmWinSockInitialize(&gpVmSockPackage);
#else
        dwError = VmSockPosixInitialize(&(pRESTHandler->pPackage));
#endif
    }

    return dwError;
}

VOID
VmwSockShutdown(
    PVMREST_HANDLER                    pRESTHandler
    )
{
    if (pRESTHandler->pPackage)
    {
#ifdef _WIN32
        VmWinSockShutdown(gpVmSockPackage);
#else
        VmSockPosixShutdown(pRESTHandler->pPackage);
#endif
    }
}
