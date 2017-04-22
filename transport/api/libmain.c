/* C-REST-Engine
*
* Copyright (c) 2017 VMware, Inc. All Rights Reserved. 
*
* This product is licensed to you under the Apache 2.0 license (the "License").
* You may not use this product except in compliance with the Apache 2.0 License.  
*
* This product may include a number of subcomponents with separate copyright 
* notices and license terms. Your use of these subcomponents is subject to the 
* terms and conditions of the subcomponent's license, as noted in the LICENSE file. 
*
*/

#include "includes.h"

DWORD
VmwSockInitialize(
    PVMREST_HANDLE                    pRESTHandle
    )
{
    DWORD dwError = 0;

    if (pRESTHandle)
    {
#ifdef _WIN32
        dwError = VmWinSockInitialize(&(pRESTHandle->pPackage));
#else
        dwError = VmSockPosixInitialize(&(pRESTHandle->pPackage));
#endif
    }

    return dwError;
}

VOID
VmwSockShutdown(
    PVMREST_HANDLE                    pRESTHandle
    )
{
    if (pRESTHandle && pRESTHandle->pPackage)
    {
#ifdef _WIN32
        VmWinSockShutdown(pRESTHandle->pPackage);
#else
        VmSockPosixShutdown(pRESTHandle->pPackage);
#endif
    }
}
