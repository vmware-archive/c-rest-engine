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

DWORD
VmSockPosixOpenServer(
    PVMREST_HANDLE                   pRESTHandle,
    USHORT                           usPort,
    int                              iListenQueueSize,
    VM_SOCK_CREATE_FLAGS             dwFlags,
    PVM_SOCKET*                      ppSocket,
    char*                            sslCert,
    char*                            sslKey
    );

DWORD
VmSockPosixCreateEventQueue(
    PVMREST_HANDLE                   pRESTHandle,
    int                              iEventQueueSize,
    PVM_SOCK_EVENT_QUEUE*            ppQueue
    );

DWORD
VmSockPosixEventQueueAdd(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCK_EVENT_QUEUE             pQueue,
    PVM_SOCKET                       pSocket
    );

DWORD
VmSockPosixWaitForEvent(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCK_EVENT_QUEUE             pQueue,
    int                              iTimeoutMS,
    PVM_SOCKET*                      ppSocket,
    PVM_SOCK_EVENT_TYPE              pEventType,
    PVM_SOCK_IO_BUFFER*              ppIoBuffer
    );

VOID
VmSockPosixCloseEventQueue(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCK_EVENT_QUEUE             pQueue
    );

DWORD
VmSockPosixSetNonBlocking(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket
    );

DWORD
VmSockPosixRead(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket,
    PVM_SOCK_IO_BUFFER               pIoBuffer
    );

DWORD
VmSockPosixWrite(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket,
    const struct sockaddr*           pClientAddress,
    socklen_t                        addrLength,
    PVM_SOCK_IO_BUFFER               pIoBuffer
    );


PVM_SOCKET
VmSockPosixAcquireSocket(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket
    );

VOID
VmSockPosixReleaseSocket(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket
    );

DWORD
VmSockPosixCloseSocket(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket
    );

DWORD
VmSockPosixAllocateIoBuffer(
    PVMREST_HANDLE                   pRESTHandle,
    VM_SOCK_EVENT_TYPE               eventType,
    DWORD                            dwSize,
    PVM_SOCK_IO_BUFFER*              ppIoBuffer
    );

VOID
VmSockPosixFreeIoBuffer(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCK_IO_BUFFER               pIoBuffer
    );

VOID
VmSockPosixGetStreamBuffer(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket,
    PVM_STREAM_BUFFER*               ppStreamBuffer
    );

VOID
VmSockPosixSetStreamBuffer(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket,
    PVM_STREAM_BUFFER                pStreamBuffer
    );

uint32_t
VmRESTGetSockPackagePosix(
     PVM_SOCK_PACKAGE*               ppSockPackagePosix
     );

VOID
VmRESTFreeSockPackagePosix(
    PVM_SOCK_PACKAGE                 pSockPackagePosix
    );

