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

/*    dwError = pRESTHandler->pPackage->pfnOpenClientSocket(
                                    pszHost,
                                    usPort,
                                    dwFlags,
                                    ppSocket);
*/
error:

    return dwError;
}

DWORD
VmwSockOpenServer(
    PVMREST_HANDLER                    pRESTHandler,
    USHORT                           usPort,
    int                              iListenQueueSize,
    VM_SOCK_CREATE_FLAGS             dwFlags,
    PVM_SOCKET*                      ppSocket,
    char*                            sslCert,
    char*                            sslKey
    )
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;

    if (!usPort || !ppSocket || !pRESTHandler)
    {
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMSOCK_ERROR(dwError);
    }

    dwError = pRESTHandler->pPackage->pfnOpenServerSocket(
                                      pRESTHandler,
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
    PVMREST_HANDLER                    pRESTHandler,
    int                              iEventQueueSize,
    PVM_SOCK_EVENT_QUEUE*            ppQueue
    )
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;

    if (!iEventQueueSize || !ppQueue || !pRESTHandler)
    {
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMSOCK_ERROR(dwError);
    }

    dwError = pRESTHandler->pPackage->pfnCreateEventQueue(pRESTHandler,iEventQueueSize, ppQueue);

error:

    return dwError;
}

DWORD
VmwSockEventQueueAdd(
    PVMREST_HANDLER                    pRESTHandler,
    PVM_SOCK_EVENT_QUEUE             pQueue,
    PVM_SOCKET                       pSocket
    )
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;

    if (!pQueue || !pSocket || !pRESTHandler)
    {
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMSOCK_ERROR(dwError);
    }

    dwError = pRESTHandler->pPackage->pfnAddEventQueue(pRESTHandler,pQueue, pSocket);

error:

    return dwError;
}

DWORD
VmwSockWaitForEvent(
    PVMREST_HANDLER                    pRESTHandler,
    PVM_SOCK_EVENT_QUEUE             pQueue,
    int                              iTimeoutMS,
    PVM_SOCKET*                      ppSocket,
    PVM_SOCK_EVENT_TYPE              pEventType,
    PVM_SOCK_IO_BUFFER*              ppIoEvent
    )
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;

    if (!pQueue || !ppSocket || !pEventType || !pRESTHandler)
    {
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMSOCK_ERROR(dwError);
    }

    dwError = pRESTHandler->pPackage->pfnWaitForEvent(
                                    pRESTHandler,
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
    PVMREST_HANDLER                    pRESTHandler,
    PVM_SOCK_EVENT_QUEUE             pQueue
    )
{
    if (!pRESTHandler)
    {
        return;
    }

    if (pQueue)
    {
        pRESTHandler->pPackage->pfnCloseEventQueue(pRESTHandler,pQueue);
    }
}

DWORD
VmwSockSetNonBlocking(
    PVMREST_HANDLER                  pRESTHandler,
    PVM_SOCKET                       pSocket
    )
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;

    if (!pSocket || !pRESTHandler)
    {
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMSOCK_ERROR(dwError);
    }

    dwError = pRESTHandler->pPackage->pfnSetNonBlocking(pRESTHandler,pSocket);

error:

    return dwError;
}

DWORD
VmwSockGetProtocol(
    PVMREST_HANDLER                    pRESTHandler,
    PVM_SOCKET                       pSocket,
    PDWORD                           pdwProtocol
    )
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;

    if (!pSocket || !pRESTHandler)
    {
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMSOCK_ERROR(dwError);
    }

    dwError = pRESTHandler->pPackage->pfnGetProtocol(pRESTHandler, pSocket, pdwProtocol);

error:

    return dwError;
}

DWORD
VmwSockSetData(
    PVMREST_HANDLER                    pRESTHandler,
    PVM_SOCKET                       pSocket,
    PVOID                            pData,
    PVOID*                           ppOldData
    )
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;

    if (!pSocket || !pRESTHandler)
    {
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMSOCK_ERROR(dwError);
    }

    dwError = pRESTHandler->pPackage->pfnSetData(pRESTHandler, pSocket, pData, ppOldData);

error:

    return dwError;
}

DWORD
VmwSockGetData(
    PVMREST_HANDLER                    pRESTHandler,
    PVM_SOCKET                       pSocket,
    PVOID*                           ppData
    )
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;

    if (!pSocket || !ppData || !pRESTHandler)
    {
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMSOCK_ERROR(dwError);
    }

    dwError = pRESTHandler->pPackage->pfnGetData(pRESTHandler,pSocket, ppData);

error:

    return dwError;
}

DWORD
VmwSockRead(
    PVMREST_HANDLER                    pRESTHandler,
    PVM_SOCKET                       pSocket,
    PVM_SOCK_IO_BUFFER               pIoBuffer
    )
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;

    if (!pSocket || !pIoBuffer || !pRESTHandler)
    {
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMSOCK_ERROR(dwError);
    }

    dwError = pRESTHandler->pPackage->pfnRead(
                            pRESTHandler,
                            pSocket,
                            pIoBuffer);

error:

    return dwError;
}

DWORD
VmwSockWrite(
    PVMREST_HANDLER                    pRESTHandler,
    PVM_SOCKET                       pSocket,
    const struct sockaddr*           pClientAddress,
    socklen_t                        addrLength,
    PVM_SOCK_IO_BUFFER               pIoBuffer
)
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;

    if (!pSocket || !pIoBuffer || !pRESTHandler)
    {
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMSOCK_ERROR(dwError);
    }

    dwError = pRESTHandler->pPackage->pfnWrite(
                            pRESTHandler,
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
    PVMREST_HANDLER                    pRESTHandler,
    PVM_SOCKET                       pSocket
    )
{
    return pSocket ? pRESTHandler->pPackage->pfnAcquireSocket(pRESTHandler,pSocket) : NULL;
}

VOID
VmwSockRelease(
    PVMREST_HANDLER                    pRESTHandler,
    PVM_SOCKET                       pSocket
    )
{
    if (pSocket)
    {
        pRESTHandler->pPackage->pfnReleaseSocket(pRESTHandler, pSocket);
    }
}

DWORD
VmwSockClose(
    PVMREST_HANDLER                    pRESTHandler,
    PVM_SOCKET                       pSocket
    )
{
    DWORD dwError = 0;
    
    dwError = pRESTHandler->pPackage->pfnCloseSocket(pRESTHandler, pSocket);
    BAIL_ON_VMSOCK_ERROR(dwError);

error:

    return dwError;
}

BOOLEAN
VmwSockIsValidIPAddress(
    PVMREST_HANDLER                    pRESTHandler,
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
    PVMREST_HANDLER                    pRESTHandler,
    PVM_SOCKET                       pSocket,
    struct sockaddr_storage*         pAddress,
    socklen_t*                       addresLen
    )
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;

    if (!pSocket || !pRESTHandler)
    {
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMSOCK_ERROR(dwError);
    }

    dwError = pRESTHandler->pPackage->pfnGetAddress(pRESTHandler, pSocket, pAddress, addresLen);
    BAIL_ON_VMSOCK_ERROR(dwError);

error:

    return dwError;
}

DWORD
VmwSockStartListening(
    PVMREST_HANDLER                    pRESTHandler,
    PVM_SOCKET                       pSocket,
    int                              iListenQueueSize
    )
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;

    if (!pSocket || !pRESTHandler)
    {
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMSOCK_ERROR(dwError);
    }

    dwError = pRESTHandler->pPackage->pfnStartListening(pRESTHandler, pSocket, iListenQueueSize);
    BAIL_ON_VMSOCK_ERROR(dwError);

error:

    return dwError;
}

DWORD
VmwSockAllocateIoBuffer(
    PVMREST_HANDLER                    pRESTHandler,
    VM_SOCK_EVENT_TYPE               eventType,
    DWORD                            dwSize,
    PVM_SOCK_IO_BUFFER*              ppIoBuffer
    )
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;

    if (!ppIoBuffer || !pRESTHandler)
    {
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMSOCK_ERROR(dwError);
    }

    dwError = pRESTHandler->pPackage->pfnAllocateIoBuffer(pRESTHandler, eventType, dwSize, ppIoBuffer);
    BAIL_ON_VMSOCK_ERROR(dwError);

error:

    return dwError;
}

DWORD
VmwSockReleaseIoBuffer(
    PVMREST_HANDLER                    pRESTHandler,
    PVM_SOCK_IO_BUFFER               pIoBuffer
    )
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;

    if (!pIoBuffer || !pRESTHandler)
    {
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMSOCK_ERROR(dwError);
    }

    pRESTHandler->pPackage->pfnReleaseIoBuffer(pRESTHandler, pIoBuffer);

error:

    return dwError;

}

VOID
VmwSockGetStreamBuffer(
    PVMREST_HANDLER                    pRESTHandler,
    PVM_SOCKET                       pSocket,
    PVM_STREAM_BUFFER*               ppStreamBuffer
    )
{
    pRESTHandler->pPackage->pfnGetStreamBuffer(pRESTHandler,pSocket,ppStreamBuffer);
}

VOID
VmwSockSetStreamBuffer(
    PVMREST_HANDLER                    pRESTHandler,
    PVM_SOCKET                       pSocket,
    PVM_STREAM_BUFFER                pStreamBuffer
    )
{
     pRESTHandler->pPackage->pfnSetStreamBuffer(pRESTHandler,pSocket,pStreamBuffer);
}
