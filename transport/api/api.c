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

#ifdef WIN32
#pragma comment(lib, "Ws2_32.lib")
#define inet_pton(x, y, z) InetPtonA(x, y, z)
#endif

DWORD
VmwSockOpenClient(
    PCSTR                            pszHost,
    USHORT                           usPort,
    VM_SOCK_CREATE_FLAGS             dwFlags,
    PVM_SOCKET*                      ppSocket
    )
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;

    if (!pszHost || !usPort || !ppSocket )
    {
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMSOCK_ERROR(dwError);
    }

    dwError = gpVmSockPackage->pfnOpenClientSocket(
                                    pszHost,
                                    usPort,
                                    dwFlags,
                                    ppSocket);

error:

    return dwError;
}

DWORD
VmwSockOpenServer(
    USHORT                           usPort,
    int                              iListenQueueSize,
    VM_SOCK_CREATE_FLAGS             dwFlags,
    PVM_SOCKET*                      ppSocket
    )
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;

    if (!usPort || !ppSocket)
    {
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMSOCK_ERROR(dwError);
    }

    dwError = gpVmSockPackage->pfnOpenServerSocket(
                                    usPort,
                                    iListenQueueSize,
                                    dwFlags,
                                    ppSocket);
error:

    return dwError;
}

DWORD
VmwSockCreateEventQueue(
    int                              iEventQueueSize,
    PVM_SOCK_EVENT_QUEUE*            ppQueue
    )
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;

    if (!iEventQueueSize || !ppQueue)
    {
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMSOCK_ERROR(dwError);
    }

    dwError = gpVmSockPackage->pfnCreateEventQueue(iEventQueueSize, ppQueue);

error:

    return dwError;
}

DWORD
VmwSockEventQueueAdd(
    PVM_SOCK_EVENT_QUEUE             pQueue,
    PVM_SOCKET                       pSocket
    )
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;

    if (!pQueue || !pSocket)
    {
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMSOCK_ERROR(dwError);
    }

    dwError = gpVmSockPackage->pfnAddEventQueue(pQueue, pSocket);

error:

    return dwError;
}

DWORD
VmwSockWaitForEvent(
    PVM_SOCK_EVENT_QUEUE             pQueue,
    int                              iTimeoutMS,
    PVM_SOCKET*                      ppSocket,
    PVM_SOCK_EVENT_TYPE              pEventType,
    PVM_SOCK_IO_BUFFER*              ppIoEvent
    )
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;

    if (!pQueue || !ppSocket || !pEventType)
    {
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMSOCK_ERROR(dwError);
    }

    dwError = gpVmSockPackage->pfnWaitForEvent(
                                    pQueue,
                                    iTimeoutMS,
                                    ppSocket,
                                    pEventType,
                                    ppIoEvent
                                    );

error:

    return dwError;
}

VOID
VmwSockCloseEventQueue(
    PVM_SOCK_EVENT_QUEUE             pQueue
    )
{
    if (pQueue)
    {
        gpVmSockPackage->pfnCloseEventQueue(pQueue);
    }
}

DWORD
VmwSockSetNonBlocking(
    PVM_SOCKET                       pSocket
    )
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;

    if (!pSocket)
    {
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMSOCK_ERROR(dwError);
    }

    dwError = gpVmSockPackage->pfnSetNonBlocking(pSocket);

error:

    return dwError;
}

DWORD
VmwSockGetProtocol(
    PVM_SOCKET                       pSocket,
    PDWORD                           pdwProtocol
    )
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;

    if (!pSocket)
    {
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMSOCK_ERROR(dwError);
    }

    dwError = gpVmSockPackage->pfnGetProtocol(pSocket, pdwProtocol);

error:

    return dwError;
}

DWORD
VmwSockSetData(
    PVM_SOCKET                       pSocket,
    PVOID                            pData,
    PVOID*                           ppOldData
    )
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;

    if (!pSocket)
    {
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMSOCK_ERROR(dwError);
    }

    dwError = gpVmSockPackage->pfnSetData(pSocket, pData, ppOldData);

error:

    return dwError;
}

DWORD
VmwSockGetData(
    PVM_SOCKET                       pSocket,
    PVOID*                           ppData
    )
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;

    if (!pSocket || !ppData)
    {
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMSOCK_ERROR(dwError);
    }

    dwError = gpVmSockPackage->pfnGetData(pSocket, ppData);

error:

    return dwError;
}

DWORD
VmwSockRead(
    PVM_SOCKET                       pSocket,
    PVM_SOCK_IO_BUFFER               pIoBuffer
    )
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;

    if (!pSocket || !pIoBuffer)
    {
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMSOCK_ERROR(dwError);
    }

    dwError = gpVmSockPackage->pfnRead(
                            pSocket,
                            pIoBuffer);

error:

    return dwError;
}

DWORD
VmwSockWrite(
    PVM_SOCKET                       pSocket,
    const struct sockaddr*           pClientAddress,
    socklen_t                        addrLength,
    PVM_SOCK_IO_BUFFER               pIoBuffer
)
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;

    if (!pSocket || !pIoBuffer)
    {
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMSOCK_ERROR(dwError);
    }

    dwError = gpVmSockPackage->pfnWrite(
                            pSocket,
                            pClientAddress,
                            addrLength,
                            pIoBuffer);
    BAIL_ON_VMSOCK_ERROR(dwError);

error:

    return dwError;
}

PVM_SOCKET
VmwSockAcquire(
    PVM_SOCKET                       pSocket
    )
{
    return pSocket ? gpVmSockPackage->pfnAcquireSocket(pSocket) : NULL;
}

VOID
VmwSockRelease(
    PVM_SOCKET                       pSocket
    )
{
    if (pSocket)
    {
        gpVmSockPackage->pfnReleaseSocket(pSocket);
    }
}

DWORD
VmwSockClose(
    PVM_SOCKET                       pSocket
    )
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;

    if (!pSocket)
    {
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMSOCK_ERROR(dwError);
    }

    dwError = gpVmSockPackage->pfnCloseSocket(pSocket);
    BAIL_ON_VMSOCK_ERROR(dwError);

error:

    return dwError;
}

BOOLEAN
VmwSockIsValidIPAddress(
    PCSTR                            pszAddress
    )
{
    BOOLEAN                          bIsValid = TRUE;

    if (!IsNullOrEmptyString(pszAddress))
    {
        struct sockaddr_storage ipv4Addr;

        bIsValid = (inet_pton(AF_INET, pszAddress, &ipv4Addr) == 1);

#ifdef AF_INET6
        if (!bIsValid)
        {
            struct sockaddr_storage ipv6Addr;

            bIsValid = (inet_pton(AF_INET6, pszAddress, &ipv6Addr) == 1);
        }
#endif
    }

    return bIsValid;
}


DWORD
VmwSockGetAddress(
    PVM_SOCKET                       pSocket,
    struct sockaddr_storage*         pAddress,
    socklen_t*                       addresLen
    )
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;

    if (!pSocket)
    {
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMSOCK_ERROR(dwError);
    }

    dwError = gpVmSockPackage->pfnGetAddress(pSocket, pAddress, addresLen);
    BAIL_ON_VMSOCK_ERROR(dwError);

error:

    return dwError;
}

DWORD
VmwSockStartListening(
    PVM_SOCKET                       pSocket,
    int                              iListenQueueSize
    )
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;

    if (!pSocket)
    {
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMSOCK_ERROR(dwError);
    }

    dwError = gpVmSockPackage->pfnStartListening(pSocket, iListenQueueSize);
    BAIL_ON_VMSOCK_ERROR(dwError);

error:

    return dwError;
}

DWORD
VmwSockAllocateIoBuffer(
    VM_SOCK_EVENT_TYPE               eventType,
    DWORD                            dwSize,
    PVM_SOCK_IO_BUFFER*              ppIoBuffer
    )
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;

    if (!ppIoBuffer)
    {
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMSOCK_ERROR(dwError);
    }

    dwError = gpVmSockPackage->pfnAllocateIoBuffer(eventType, dwSize, ppIoBuffer);
    BAIL_ON_VMSOCK_ERROR(dwError);

error:

    return dwError;
}

DWORD
VmwSockReleaseIoBuffer(
    PVM_SOCK_IO_BUFFER               pIoBuffer
    )
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;

    if (!pIoBuffer)
    {
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMSOCK_ERROR(dwError);
    }

    gpVmSockPackage->pfnReleaseIoBuffer(pIoBuffer);

error:

    return dwError;

}

VOID
VmwSockGetStreamBuffer(
    PVM_SOCKET                       pSocket,
    PVM_STREAM_BUFFER*               ppStreamBuffer
    )
{
    gpVmSockPackage->pfnGetStreamBuffer(pSocket,ppStreamBuffer);
}

VOID
VmwSockSetStreamBuffer(
    PVM_SOCKET                       pSocket,
    PVM_STREAM_BUFFER                pStreamBuffer
    )
{
     gpVmSockPackage->pfnSetStreamBuffer(pSocket,pStreamBuffer);
}
