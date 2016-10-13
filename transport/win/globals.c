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

// For now only client socket part is portable

VM_SOCK_PACKAGE gVmSockWinPackage =
{
    VMREST_SF_INIT(.pfnOpenClientSocket, &VmSockWinOpenClient),
    VMREST_SF_INIT(.pfnOpenServerSocket, &VmSockWinOpenServer),
	VMREST_SF_INIT(.pfnStartListening, &VmSockWinStartListening),
    VMREST_SF_INIT(.pfnCreateEventQueue, &VmSockWinCreateEventQueue),
    VMREST_SF_INIT(.pfnAddEventQueue, &VmSockWinEventQueueAdd),
    VMREST_SF_INIT(.pfnWaitForEvent, &VmSockWinWaitForEvent),
    VMREST_SF_INIT(.pfnCloseEventQueue, &VmSockWinCloseEventQueue),
    VMREST_SF_INIT(.pfnSetNonBlocking, &VmSockWinSetNonBlocking),
    VMREST_SF_INIT(.pfnGetProtocol, &VmSockWinGetProtocol),
    VMREST_SF_INIT(.pfnSetData, &VmSockWinSetData),
    VMREST_SF_INIT(.pfnGetData, &VmSockWinGetData),
    VMREST_SF_INIT(.pfnRead, &VmSockWinRead),
	VMREST_SF_INIT(.pfnWrite, &VmSockWinWrite),
    VMREST_SF_INIT(.pfnAcquireSocket, &VmSockWinAcquire),
    VMREST_SF_INIT(.pfnReleaseSocket, &VmSockWinRelease),
    VMREST_SF_INIT(.pfnCloseSocket, &VmSockWinClose),
    VMREST_SF_INIT(.pfnGetAddress, &VmSockWinGetAddress),
    VMREST_SF_INIT(.pfnAllocateIoBuffer, &VmSockWinAllocateIoBuffer),
    VMREST_SF_INIT(.pfnReleaseIoBuffer, &VmSockWinFreeIoBuffer),
    VMREST_SF_INIT(.pfnGetStreamBuffer, &VmSockPosixGetStreamBuffer),
    VMREST_SF_INIT(.pfnSetStreamBuffer, &VmSockPosixSetStreamBuffer)
};

PVM_SOCK_PACKAGE gpVmWinSockPackage = &gVmSockWinPackage;
