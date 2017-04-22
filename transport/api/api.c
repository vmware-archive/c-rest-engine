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

#ifdef WIN32
#pragma comment(lib, "Ws2_32.lib")
#define inet_pton(x, y, z) InetPtonA(x, y, z)
#endif

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
