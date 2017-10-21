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
    PVM_SOCK_EVENT_TYPE              pEventType
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
                                    pEventType
                                    );

error:

    return dwError;
}

DWORD
VmwSockCloseEventQueue(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCK_EVENT_QUEUE             pQueue,
    uint32_t                         waitSecond
    )
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;

    if (!pRESTHandle)
    {
        dwError = REST_ENGINE_FAILURE;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    if (pQueue)
    {
        dwError = pRESTHandle->pPackage->pfnCloseEventQueue(pRESTHandle,pQueue,waitSecond);
    }
    BAIL_ON_VMREST_ERROR(dwError);

error:

    return dwError;

}

DWORD
VmwSockRead(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket,
    char**                           ppszBuffer,
    uint32_t*                        nBufLen
    )
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;

    if (!pSocket || !ppszBuffer || !pRESTHandle)
    {
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMSOCK_ERROR(dwError);
    }

    dwError = pRESTHandle->pPackage->pfnRead(
                            pRESTHandle,
                            pSocket,
                            ppszBuffer,
                            nBufLen);

error:

    return dwError;
}

DWORD
VmwSockWrite(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket,
    char*                            pszBuffer,
    uint32_t                         nBufLen
)
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;

    if (!pSocket || !pszBuffer || !pRESTHandle)
    {
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMSOCK_ERROR(dwError);
    }

    dwError = pRESTHandle->pPackage->pfnWrite(
                            pRESTHandle,
                            pSocket,
                            pszBuffer,
                            nBufLen);
    BAIL_ON_VMSOCK_ERROR(dwError);

error:

    return dwError;
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
VmwSockGetRequestHandle(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket,
    PREST_REQUEST*                   ppRequest
    )
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;

    dwError =  pRESTHandle->pPackage->pfnGetRequestHandle(pRESTHandle,pSocket, ppRequest);

    return dwError;
}

DWORD
VmwSockSetRequestHandle(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket,
    PREST_REQUEST                    pRequest,
    uint32_t                         nProcessed,
    PVM_SOCK_EVENT_QUEUE             pQueue
    )
{
     DWORD                            dwError = REST_ENGINE_SUCCESS;

     dwError = pRESTHandle->pPackage->pfnSetRequestHandle(pRESTHandle,pSocket,pRequest, nProcessed, pQueue);

     return dwError;
}

DWORD
VmwSockGetPeerInfo(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket,
    char*                            pIpAddress,
    uint32_t                         nLen,
    int*                             pPortNo
    )
{
     DWORD                            dwError = REST_ENGINE_SUCCESS;

     dwError = pRESTHandle->pPackage->pfnGetPeerInfo(pRESTHandle, pSocket, pIpAddress, nLen, pPortNo);

     return dwError;
}


