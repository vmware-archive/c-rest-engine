/*
 * Copyright © 2012-2015 VMware; Inc.  All Rights Reserved.
 *
 * Licensed under the Apache License; Version 2.0 (the “License”); you may not
 * use this file except in compliance with the License.  You may obtain a copy
 * of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing; software
 * distributed under the License is distributed on an “AS IS” BASIS; without
 * warranties or conditions of any kind; EITHER EXPRESS OR IMPLIED.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */


#include "includes.h"

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

    pSockPackagePosix->pfnOpenClientSocket = &VmSockPosixOpenClient;
    pSockPackagePosix->pfnOpenServerSocket = &VmSockPosixOpenServer;
    pSockPackagePosix->pfnCreateEventQueue = &VmSockPosixCreateEventQueue;
    pSockPackagePosix->pfnAddEventQueue = &VmSockPosixEventQueueAdd;
    pSockPackagePosix->pfnWaitForEvent = &VmSockPosixWaitForEvent;
    pSockPackagePosix->pfnCloseEventQueue = &VmSockPosixCloseEventQueue;
    pSockPackagePosix->pfnSetNonBlocking = &VmSockPosixSetNonBlocking;
    pSockPackagePosix->pfnGetProtocol = &VmSockPosixGetProtocol;
    pSockPackagePosix->pfnSetData = &VmSockPosixSetData;
    pSockPackagePosix->pfnGetData = &VmSockPosixGetData;
    pSockPackagePosix->pfnRead = &VmSockPosixRead;
    pSockPackagePosix->pfnWrite = &VmSockPosixWrite;
    pSockPackagePosix->pfnAcquireSocket = &VmSockPosixAcquireSocket;
    pSockPackagePosix->pfnReleaseSocket = &VmSockPosixReleaseSocket;
    pSockPackagePosix->pfnCloseSocket = &VmSockPosixCloseSocket;
    pSockPackagePosix->pfnGetAddress = &VmSockPosixGetAddress;
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
