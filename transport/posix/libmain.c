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
VmSockPosixInitialize(
    PVM_SOCK_PACKAGE*                ppPackage
    )
{

    return VmRESTGetSockPackagePosix(ppPackage);

}

VOID
VmSockPosixShutdown(
    PVM_SOCK_PACKAGE                 pPackage
    )
{
    VmRESTFreeSockPackagePosix(pPackage);
}

uint32_t
VmRESTGetSockPackagePosix(
     PVM_SOCK_PACKAGE*               ppSockPackagePosix
     )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    PVM_SOCK_PACKAGE                 pSockPackagePosix = NULL;

    if (!ppSockPackagePosix)
    {
        dwError = REST_ENGINE_NO_MEMORY;
        BAIL_ON_VMREST_ERROR(dwError);
    }

    pSockPackagePosix = *ppSockPackagePosix;

    pSockPackagePosix->pfnOpenServerSocket = &VmSockPosixOpenServer;
    pSockPackagePosix->pfnCreateEventQueue = &VmSockPosixCreateEventQueue;
    pSockPackagePosix->pfnAddEventQueue = &VmSockPosixEventQueueAdd;
    pSockPackagePosix->pfnWaitForEvent = &VmSockPosixWaitForEvent;
    pSockPackagePosix->pfnCloseEventQueue = &VmSockPosixCloseEventQueue;
    pSockPackagePosix->pfnRead = &VmSockPosixRead;
    pSockPackagePosix->pfnWrite = &VmSockPosixWrite;
    pSockPackagePosix->pfnAcquireSocket = &VmSockPosixAcquireSocket;
    pSockPackagePosix->pfnReleaseSocket = &VmSockPosixReleaseSocket;
    pSockPackagePosix->pfnCloseSocket = &VmSockPosixCloseSocket;
    pSockPackagePosix->pfnAllocateIoBuffer = &VmSockPosixAllocateIoBuffer;
    pSockPackagePosix->pfnReleaseIoBuffer = &VmSockPosixFreeIoBuffer;
    pSockPackagePosix->pfnGetStreamBuffer = &VmSockPosixGetStreamBuffer;
    pSockPackagePosix->pfnSetStreamBuffer = &VmSockPosixSetStreamBuffer;

cleanup:

    return dwError;

error:

    goto cleanup;

}

VOID
VmRESTFreeSockPackagePosix(
    PVM_SOCK_PACKAGE                 pSockPackagePosix
    )
{
    if (pSockPackagePosix)
    {
        /* Do nothing */
    }
}

