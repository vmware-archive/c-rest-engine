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
VmWinSockInitialize(
    PVM_SOCK_PACKAGE* ppPackage
    )
{
    DWORD dwError = 0;
    WSADATA wsaData = { 0 };

    dwError = WSAStartup(MAKEWORD(2, 2), &wsaData);
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTGetSockPackageWin(ppPackage);
	BAIL_ON_VMREST_ERROR(dwError);

cleanup:

    return dwError;

error:

    *ppPackage = NULL;

    goto cleanup;
}

VOID
VmWinSockShutdown(
    PVM_SOCK_PACKAGE pPackage
    )
{
   // WSACleanup();
}


uint32_t
VmRESTGetSockPackageWin(
     PVM_SOCK_PACKAGE*               ppSockPackageWin
     )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    PVM_SOCK_PACKAGE                 pSockPackageWin = NULL;

    if (!ppSockPackageWin)
    {
        dwError = REST_ENGINE_NO_MEMORY;
        BAIL_ON_VMREST_ERROR(dwError);
    }

    pSockPackageWin = *ppSockPackageWin;

    pSockPackageWin->pfnOpenServerSocket = &VmSockWinOpenServer;
    pSockPackageWin->pfnCreateEventQueue = &VmSockWinCreateEventQueue;
    pSockPackageWin->pfnAddEventQueue = &VmSockWinEventQueueAdd;
    pSockPackageWin->pfnWaitForEvent = &VmSockWinWaitForEvent;
    pSockPackageWin->pfnCloseEventQueue = &VmSockWinCloseEventQueue;
    pSockPackageWin->pfnRead = &VmSockWinRead;
    pSockPackageWin->pfnWrite = &VmSockWinWrite;
    pSockPackageWin->pfnAcquireSocket = &VmSockWinAcquire;
    pSockPackageWin->pfnReleaseSocket = &VmSockWinRelease;
    pSockPackageWin->pfnCloseSocket = &VmSockWinClose;
    pSockPackageWin->pfnAllocateIoBuffer = &VmSockWinAllocateIoBuffer;
    pSockPackageWin->pfnReleaseIoBuffer = &VmSockWinFreeIoBuffer;
    pSockPackageWin->pfnGetStreamBuffer = &VmSockWinGetStreamBuffer;
    pSockPackageWin->pfnSetStreamBuffer = &VmSockWinSetStreamBuffer;

cleanup:

    return dwError;

error:

    goto cleanup;

}

VOID
VmRESTFreeSockPackageWin(
    PVM_SOCK_PACKAGE                 pSockPackageWin
    )
{
    if (pSockPackageWin)
    {
        /* Do nothing */
    }
}
