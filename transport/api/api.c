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

error:

    return dwError;
}

DWORD
VmwSockOpenServer(
    PVMREST_HANDLE                   pRESTHandle,
    USHORT                           usPort,
    int                              iListenQueueSize,
    VM_SOCK_CREATE_FLAGS             dwFlags,
    PVM_SOCKET*                      ppSocket,
    char*                            sslCert,
    char*                            sslKey
    )
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;

    if (!usPort || !ppSocket || !pRESTHandle)
    {
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMSOCK_ERROR(dwError);
    }

    dwError = pRESTHandle->pPackage->pfnOpenServerSocket(
                                     pRESTHandle,
                                     usPort,
                                     iListenQueueSize,
                                     dwFlags,
                                     ppSocket,
                                     sslCert,
                                     sslKey);
error:

    return dwError;
}

DWORD
VmwSockCreateEventQueue(
    PVMREST_HANDLE                   pRESTHandle,
    int                              iEventQueueSize,
    PVM_SOCK_EVENT_QUEUE*            ppQueue
    )
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;

    if (!iEventQueueSize || !ppQueue || !pRESTHandle)
    {
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMSOCK_ERROR(dwError);
    }

    dwError = pRESTHandle->pPackage->pfnCreateEventQueue(pRESTHandle,iEventQueueSize, ppQueue);

error:

    return dwError;
}

DWORD
VmwSockEventQueueAdd(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCK_EVENT_QUEUE             pQueue,
    PVM_SOCKET                       pSocket
    )
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;

    if (!pQueue || !pSocket || !pRESTHandle)
    {
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMSOCK_ERROR(dwError);
    }

    dwError = pRESTHandle->pPackage->pfnAddEventQueue(pRESTHandle,pQueue, pSocket);

error:

    return dwError;
}

DWORD
VmwSockWaitForEvent(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCK_EVENT_QUEUE             pQueue,
    int                              iTimeoutMS,
    PVM_SOCKET*                      ppSocket,
    PVM_SOCK_EVENT_TYPE              pEventType,
    PVM_SOCK_IO_BUFFER*              ppIoEvent
    )
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;

    if (!pQueue || !ppSocket || !pEventType || !pRESTHandle)
    {
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMSOCK_ERROR(dwError);
    }

    dwError = pRESTHandle->pPackage->pfnWaitForEvent(
                                    pRESTHandle,
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
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCK_EVENT_QUEUE             pQueue
    )
{
    if (!pRESTHandle)
    {
        return;
    }

    if (pQueue)
    {
        pRESTHandle->pPackage->pfnCloseEventQueue(pRESTHandle,pQueue);
    }
}

DWORD
VmwSockSetNonBlocking(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket
    )
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;

    if (!pSocket || !pRESTHandle)
    {
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMSOCK_ERROR(dwError);
    }

    dwError = pRESTHandle->pPackage->pfnSetNonBlocking(pRESTHandle,pSocket);

error:

    return dwError;
}

DWORD
VmwSockGetProtocol(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket,
    PDWORD                           pdwProtocol
    )
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;

    if (!pSocket || !pRESTHandle)
    {
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMSOCK_ERROR(dwError);
    }

    dwError = pRESTHandle->pPackage->pfnGetProtocol(pRESTHandle, pSocket, pdwProtocol);

error:

    return dwError;
}

DWORD
VmwSockSetData(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket,
    PVOID                            pData,
    PVOID*                           ppOldData
    )
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;

    if (!pSocket || !pRESTHandle)
    {
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMSOCK_ERROR(dwError);
    }

    dwError = pRESTHandle->pPackage->pfnSetData(pRESTHandle, pSocket, pData, ppOldData);

error:

    return dwError;
}

DWORD
VmwSockGetData(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket,
    PVOID*                           ppData
    )
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;

    if (!pSocket || !ppData || !pRESTHandle)
    {
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMSOCK_ERROR(dwError);
    }

    dwError = pRESTHandle->pPackage->pfnGetData(pRESTHandle,pSocket, ppData);

error:

    return dwError;
}

DWORD
VmwSockRead(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket,
    PVM_SOCK_IO_BUFFER               pIoBuffer
    )
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;

    if (!pSocket || !pIoBuffer || !pRESTHandle)
    {
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMSOCK_ERROR(dwError);
    }

    dwError = pRESTHandle->pPackage->pfnRead(
                            pRESTHandle,
                            pSocket,
                            pIoBuffer);

error:

    return dwError;
}

DWORD
VmwSockWrite(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket,
    const struct sockaddr*           pClientAddress,
    socklen_t                        addrLength,
    PVM_SOCK_IO_BUFFER               pIoBuffer
)
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;

    if (!pSocket || !pIoBuffer || !pRESTHandle)
    {
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMSOCK_ERROR(dwError);
    }

    dwError = pRESTHandle->pPackage->pfnWrite(
                            pRESTHandle,
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
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket
    )
{
    return pSocket ? pRESTHandle->pPackage->pfnAcquireSocket(pRESTHandle,pSocket) : NULL;
}

VOID
VmwSockRelease(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket
    )
{
    if (pSocket)
    {
        pRESTHandle->pPackage->pfnReleaseSocket(pRESTHandle, pSocket);
    }
}

DWORD
VmwSockClose(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket
    )
{
    DWORD dwError = 0;
    
    dwError = pRESTHandle->pPackage->pfnCloseSocket(pRESTHandle, pSocket);
    BAIL_ON_VMSOCK_ERROR(dwError);

error:

    return dwError;
}

BOOLEAN
VmwSockIsValidIPAddress(
    PVMREST_HANDLE                   pRESTHandle,
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
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket,
    struct sockaddr_storage*         pAddress,
    socklen_t*                       addresLen
    )
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;

    if (!pSocket || !pRESTHandle)
    {
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMSOCK_ERROR(dwError);
    }

    dwError = pRESTHandle->pPackage->pfnGetAddress(pRESTHandle, pSocket, pAddress, addresLen);
    BAIL_ON_VMSOCK_ERROR(dwError);

error:

    return dwError;
}

DWORD
VmwSockStartListening(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket,
    int                              iListenQueueSize
    )
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;

    if (!pSocket || !pRESTHandle)
    {
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMSOCK_ERROR(dwError);
    }

    dwError = pRESTHandle->pPackage->pfnStartListening(pRESTHandle, pSocket, iListenQueueSize);
    BAIL_ON_VMSOCK_ERROR(dwError);

error:

    return dwError;
}

DWORD
VmwSockAllocateIoBuffer(
    PVMREST_HANDLE                   pRESTHandle,
    VM_SOCK_EVENT_TYPE               eventType,
    DWORD                            dwSize,
    PVM_SOCK_IO_BUFFER*              ppIoBuffer
    )
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;

    if (!ppIoBuffer || !pRESTHandle)
    {
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMSOCK_ERROR(dwError);
    }

    dwError = pRESTHandle->pPackage->pfnAllocateIoBuffer(pRESTHandle, eventType, dwSize, ppIoBuffer);
    BAIL_ON_VMSOCK_ERROR(dwError);

error:

    return dwError;
}

DWORD
VmwSockReleaseIoBuffer(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCK_IO_BUFFER               pIoBuffer
    )
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;

    if (!pIoBuffer || !pRESTHandle)
    {
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMSOCK_ERROR(dwError);
    }

    pRESTHandle->pPackage->pfnReleaseIoBuffer(pRESTHandle, pIoBuffer);

error:

    return dwError;

}

VOID
VmwSockGetStreamBuffer(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket,
    PVM_STREAM_BUFFER*               ppStreamBuffer
    )
{
    pRESTHandle->pPackage->pfnGetStreamBuffer(pRESTHandle,pSocket,ppStreamBuffer);
}

VOID
VmwSockSetStreamBuffer(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket,
    PVM_STREAM_BUFFER                pStreamBuffer
    )
{
     pRESTHandle->pPackage->pfnSetStreamBuffer(pRESTHandle,pSocket,pStreamBuffer);
}
