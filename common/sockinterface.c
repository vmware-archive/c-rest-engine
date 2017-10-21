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

static
VOID
VmRESTSockContextFree(
    PVMREST_HANDLE                   pRESTHandle,
    PVMREST_SOCK_CONTEXT             pSockInterface
    );

static
DWORD
VmRESTHandleSocketEvent(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket,
    VM_SOCK_EVENT_TYPE               sockEvent,
    PVM_SOCK_EVENT_QUEUE             pQueue,
    DWORD                            dwError
    );

static
VOID
VmRESTOnDisconnect(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket
    );

static
PVOID
VmRESTSockWorkerThreadProc(
    PVOID                            pData
    );

static
DWORD
VmRESTTcpReceiveNewData(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket,
    PVM_SOCK_EVENT_QUEUE             pQueue
    );

static
DWORD
VmRESTDisconnectClient(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket
    );

static
PVOID
VmRESTSockWorkerThreadProc(
    PVOID                            pData
    );

static
DWORD
VmRESTOnConnectionTimeout(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket
    );

DWORD
VmRESTInitProtocolServer(
    PVMREST_HANDLE                   pRESTHandle
    )
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;
    PVMREST_SOCK_CONTEXT             pSockContext = NULL;
    DWORD                            dwFlags = VM_SOCK_CREATE_FLAGS_REUSE_ADDR |
                                               VM_SOCK_CREATE_FLAGS_NON_BLOCK;
    DWORD                            iThr = 0;
    char                             lastPortChar = '\0';
    char*                            sslCert = NULL;
    char*                            sslKey = NULL;
    char*                            temp = NULL;
    PVM_WORKER_THREAD_DATA           pThreadData = NULL;
    BOOLEAN                          bNoIpV6 = FALSE;

    if (! pRESTHandle || !( pRESTHandle->pRESTConfig))
    {
        VMREST_LOG_ERROR(pRESTHandle,"%s","Invalid REST config");
        dwError = REST_ERROR_INVALID_HANDLER;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    pSockContext =  pRESTHandle->pSockContext;

    dwError = VmRESTAllocateMutex(&pSockContext->pMutex);
    BAIL_ON_VMREST_ERROR(dwError);

    /**** Init SSL if configured ****/
    if (strlen( pRESTHandle->pRESTConfig->server_port) == 0)
    {
        VMREST_LOG_ERROR(pRESTHandle,"%s","REST Engine config server port missing");
        dwError = REST_ERROR_INVALID_CONFIG_PORT;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    lastPortChar = VmRESTUtilsGetLastChar(
                        pRESTHandle->pRESTConfig->server_port
                       );

    if (lastPortChar == 'p' || lastPortChar == 'P')
    {
        VMREST_LOG_DEBUG(pRESTHandle,"%s","Server initing in plain text wire connection mode");
        temp =  pRESTHandle->pRESTConfig->server_port;
        while(temp != NULL)
        {
            if (*temp == 'p' || *temp == 'P')
            {
                *temp = '\0';
                break;
            }
            temp++;
        }
    }
    else
    {
        VMREST_LOG_DEBUG(pRESTHandle,"%s","Server initing in encrypted wire connection mode");
        if ((pRESTHandle->pRESTConfig->pSSLContext == NULL)  && ((IsNullOrEmptyString(pRESTHandle->pRESTConfig->ssl_certificate)) || (IsNullOrEmptyString(pRESTHandle->pRESTConfig->ssl_key))))
        {
            VMREST_LOG_ERROR(pRESTHandle,"%s", "Invalid SSL params");
            dwError =  REST_ERROR_INVALID_CONFIG;
        }
        BAIL_ON_VMREST_ERROR(dwError);
        dwFlags = dwFlags | VM_SOCK_IS_SSL;

        if (pRESTHandle->pRESTConfig->pSSLContext != NULL)
        {
            sslCert = NULL;
            sslKey = NULL;
        }
        else
        {
            sslCert =  pRESTHandle->pRESTConfig->ssl_certificate;
            sslKey =   pRESTHandle->pRESTConfig->ssl_key;
        }
    }

    /**** Handle IPv4 case ****/

    dwError = VmwSockOpenServer(
                         pRESTHandle,
                        ((unsigned short)atoi( pRESTHandle->pRESTConfig->server_port)),
                        ((int)atoi( pRESTHandle->pRESTConfig->worker_thread_count)),
                        dwFlags | VM_SOCK_CREATE_FLAGS_TCP |
                                  VM_SOCK_CREATE_FLAGS_IPV4,
                        &pSockContext->pListenerTCP,
                        sslCert,
                        sslKey
                        );
    BAIL_ON_VMREST_ERROR(dwError);

#ifdef AF_INET6
    /**** Handle IPv6 case ****/

    dwError = VmwSockOpenServer(
                   pRESTHandle,
                  ((unsigned short)atoi( pRESTHandle->pRESTConfig->server_port)),
                  ((int)atoi( pRESTHandle->pRESTConfig->worker_thread_count)),
                  dwFlags | VM_SOCK_CREATE_FLAGS_TCP |
                          VM_SOCK_CREATE_FLAGS_IPV6,
                  &pSockContext->pListenerTCP6,
                  sslCert,
                  sslKey
                  );
    if (dwError != REST_ENGINE_SUCCESS)
    {
        VMREST_LOG_DEBUG(pRESTHandle,"%s","Problem in IpV6 configuation.. Server listening ONLY on IPv4 Address !!");
        bNoIpV6 = TRUE;
    }
#endif

    dwError = VmwSockCreateEventQueue(
                   pRESTHandle,
                  -1,
                  &pSockContext->pEventQueue
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmwSockEventQueueAdd(
                   pRESTHandle,
                  pSockContext->pEventQueue,
                  pSockContext->pListenerTCP
                  );
    BAIL_ON_VMREST_ERROR(dwError);

#ifdef AF_INET6
    if (!bNoIpV6)
    {
        dwError = VmwSockEventQueueAdd(
                      pRESTHandle,
                      pSockContext->pEventQueue,
                      pSockContext->pListenerTCP6
                      );
        BAIL_ON_VMREST_ERROR(dwError);
    }
#endif

    dwError = VmRESTAllocateMemory(
                  sizeof(PVMREST_THREAD) * (((int)atoi( pRESTHandle->pRESTConfig->worker_thread_count))) ,
                  (PVOID*)&pSockContext->pWorkerThreads
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    pSockContext->dwNumThreads = ((int)atoi( pRESTHandle->pRESTConfig->worker_thread_count));

    for (; iThr < pSockContext->dwNumThreads; iThr++)
    {
        dwError = VmRESTAllocateMemory(
                  sizeof(VM_WORKER_THREAD_DATA) ,
                  (PVOID*)&pThreadData
                  );
        BAIL_ON_VMREST_ERROR(dwError);
        pThreadData->pSockContext = pSockContext;
        pThreadData-> pRESTHandle =  pRESTHandle;

        dwError = VmRESTAllocateMemory(
                      sizeof(VMREST_THREAD),
                      (void **)&pSockContext->pWorkerThreads[iThr]
                      );
        BAIL_ON_VMREST_ERROR(dwError);

        dwError = VmRESTCreateThread(
                      pSockContext->pWorkerThreads[iThr],
                      TRUE,
                      (PVMREST_START_ROUTINE)&VmRESTSockWorkerThreadProc,
                      pThreadData
                      );
        BAIL_ON_VMREST_ERROR(dwError);

        pThreadData = NULL;
    }

     pRESTHandle->pSockContext = pSockContext;

cleanup:

    return dwError;

error:
    if (pThreadData != NULL)
    {
        VmRESTFreeMemory(pThreadData);
        pThreadData = NULL;
    }

    goto cleanup;
}

VOID
VmRESTShutdownProtocolServer(
    PVMREST_HANDLE                   pRESTHandle
    )
{
    if (pRESTHandle && pRESTHandle->pSockContext)
    {
         VmRESTSockContextFree( pRESTHandle,  pRESTHandle->pSockContext);
    }
}

static
PVOID
VmRESTSockWorkerThreadProc(
    PVOID                            pData
    )
{
    DWORD                            dwError = 0;
    PVM_WORKER_THREAD_DATA           pWorkerData = (PVM_WORKER_THREAD_DATA)pData;
    PVMREST_HANDLE                   pRESTHandle = NULL;
    PVMREST_SOCK_CONTEXT             pSockContext = NULL;
    PVM_SOCKET                       pSocket = NULL;

    if (pWorkerData != NULL)
    {
        pRESTHandle = pWorkerData-> pRESTHandle;
        pSockContext = pWorkerData->pSockContext;
        VmRESTFreeMemory(pWorkerData);
        pWorkerData = NULL;
    }
    else
    {
        return NULL;
    }

    for(;;)
    {
        VM_SOCK_EVENT_TYPE eventType = VM_SOCK_EVENT_TYPE_UNKNOWN;

        dwError = VmwSockWaitForEvent(
                        pRESTHandle,
                        pSockContext->pEventQueue,
                        -1,
                        &pSocket,
                        &eventType);

        if (dwError == ERROR_SHUTDOWN_IN_PROGRESS)
        {
            break;
        }
        dwError = VmRESTHandleSocketEvent(
                         pRESTHandle,
                        pSocket,
                        eventType,
                        pSockContext->pEventQueue,
                        dwError);

        if (dwError == ERROR_SUCCESS ||
            dwError == ERROR_IO_PENDING)
        { 
            dwError = ERROR_SUCCESS;
        }
        else
        {
            pSocket = NULL;
            dwError = 0;
        }
        BAIL_ON_VMREST_ERROR(dwError);
   
    }
error:
#ifndef WIN32
    if (pSocket)
    {
        VmwSockRelease( pRESTHandle, pSocket);
    }
#endif

    return NULL;
}



static
DWORD
VmRESTHandleSocketEvent(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket,
    VM_SOCK_EVENT_TYPE               sockEvent,
    PVM_SOCK_EVENT_QUEUE             pQueue,
    DWORD                            dwError
    )
{
    if (dwError == ERROR_SUCCESS)
    {
        switch (sockEvent)
        {
        case VM_SOCK_EVENT_TYPE_TCP_NEW_CONNECTION:
            VMREST_LOG_DEBUG(pRESTHandle,"%s","EVENT-HANDLER: Accepted new connection.");
            break;
#ifndef WIN32
        case VM_SOCK_EVENT_TYPE_DATA_AVAILABLE:
            VMREST_LOG_DEBUG(pRESTHandle,"%s","EVENT-HANDLER: Data available on socket");
            dwError = VmRESTTcpReceiveNewData(pRESTHandle,pSocket,pQueue);
            BAIL_ON_VMREST_ERROR(dwError);
            break;

        case VM_SOCK_EVENT_TYPE_CONNECTION_CLOSED:
            VMREST_LOG_DEBUG(pRESTHandle,"%s","EVENT-HANDLER: Broken pipe or remote conn closed");
            VmRESTOnDisconnect( pRESTHandle, pSocket);
            break;

        case VM_SOCK_EVENT_TYPE_CONNECTION_TIMEOUT:
            /**** Free Associated memory ****/
            VMREST_LOG_DEBUG(pRESTHandle,"%s","EVENT-HANDLER: Connection timeout happened..Disconnecting client ...");
            dwError = VmRESTOnConnectionTimeout(
                          pRESTHandle,
                          pSocket
                          );
            BAIL_ON_VMREST_ERROR(dwError);
            break;

        case VM_SOCK_EVENT_TYPE_UNKNOWN:
             VMREST_LOG_DEBUG(pRESTHandle,"%s","EVENT-HANDLER: Unknown Socket Event, do nothing");
             break;

        default:
            VMREST_LOG_DEBUG(pRESTHandle,"%s","EVENT-HANDLER: Invalid socket event.");
            dwError = ERROR_INVALID_MESSAGE;
            break;
#endif
        }
    }

cleanup:

    return dwError;

error :
    goto cleanup;
}

static
DWORD
VmRESTOnConnectionTimeout(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket
    )
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;
    PREST_REQUEST                    pRequest = NULL;
    uint32_t                         errCodeTimeOut = 408;

    dwError = VmwSockGetRequestHandle(
                  pRESTHandle,
                  pSocket,
                  &pRequest
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    if (pRequest == NULL)
    {
        dwError = VmRESTGetRequestHandle(
                      pRESTHandle,
                      pSocket,
                      &pRequest
                      );
        BAIL_ON_VMREST_ERROR(dwError);
    }

    VMREST_LOG_DEBUG(pRESTHandle,"%s","Connection Timeout..Closing conn..");

    dwError = VmRESTSendFailureResponse(
                  pRESTHandle,
                  errCodeTimeOut,
                  pRequest
                  );
    if (dwError != REST_ENGINE_SUCCESS)
    {
        VMREST_LOG_ERROR(pRESTHandle,"%s","Double Failure case detected ....");
    }
    dwError = REST_ENGINE_SUCCESS;


    dwError = VmRESTDisconnectClient(
                     pRESTHandle,
                     pSocket
                     );
    BAIL_ON_VMREST_ERROR(dwError);

    if (pRequest)
    {
        VmRESTFreeRequestHandle(
            pRESTHandle,
            pRequest
            );
        pRequest = NULL;
    }

cleanup:

    return dwError;

error :
    goto cleanup;

}
     
static
VOID
VmRESTOnDisconnect(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket
    )
{
    if (pSocket)
    {
        VmwSockClose( pRESTHandle, pSocket);
    }
}

static
DWORD
VmRESTTcpReceiveNewData(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket,
    PVM_SOCK_EVENT_QUEUE             pQueue
    )
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;
    PREST_REQUEST                    pRequest = NULL;
    PREST_REQUEST                    pSetReq = NULL;
    char*                            pszBuffer = NULL;
    uint32_t                         nProcessed = 0;
    uint32_t                         nBufLen = 0;
    BOOLEAN                          bNextIO = FALSE;

    if (!pSocket || !pRESTHandle || !pQueue)
    {
        dwError = ERROR_INVALID_PARAMETER;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmwSockGetRequestHandle(
                  pRESTHandle,
                  pSocket,
                  &pRequest
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    if (pRequest == NULL)
    {
        /**** This is the first IO for data on socket ****/ 
        dwError = VmRESTGetRequestHandle(
                      pRESTHandle,
                      pSocket,
                      &pRequest
                      );
        BAIL_ON_VMREST_ERROR(dwError);
    }

    /**** Do socket read ****/
    dwError = VmwSockRead(
                  pRESTHandle,
                  pSocket,
                  &pszBuffer,
                  &nBufLen
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    if (nBufLen > 0)
    {
        VMREST_LOG_DEBUG(pRESTHandle,"%s","Starting HTTP Parsing");
        dwError = VmRESTProcessBuffer(
                      pRESTHandle,
                      pszBuffer,
                      nBufLen,
                      pRequest,
                      &nProcessed
                      );
        if (dwError == REST_ENGINE_MORE_IO_REQUIRED)
        {
            bNextIO = TRUE;
            dwError = REST_ENGINE_SUCCESS;
        }
        BAIL_ON_VMREST_ERROR(dwError);
    }
    else if (nBufLen == 0)
    {
        bNextIO = TRUE;
    }

    if (bNextIO)
    {
        pSetReq = pRequest;
    }

    /**** Save state of request processing in socket context ****/
    dwError = VmwSockSetRequestHandle(
                  pRESTHandle,
                  pSocket,
                  pSetReq,
                  nProcessed,
                  pQueue
                  );
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:

   if (!bNextIO)
   {
       VMREST_LOG_DEBUG(pRESTHandle,"%s","Calling closed connection....");
       /**** Close connection ****/ 
       dwError = VmRESTDisconnectClient(
                     pRESTHandle,
                     pSocket
                     );
       BAIL_ON_VMREST_ERROR(dwError);

       /****  free request object memory ****/
       if (pRequest)
       {
           VmRESTFreeRequestHandle(
               pRESTHandle,
               pRequest
               );
           pRequest = NULL;
       }
   }

    return dwError;

error:

    VMREST_LOG_DEBUG(pRESTHandle,"ERROR code %u", dwError);
    goto cleanup;
}

static
VOID
VmRESTSockContextFree(
    PVMREST_HANDLE                   pRESTHandle,
    PVMREST_SOCK_CONTEXT             pSockContext
    )
{
    if (pSockContext->pEventQueue)
    {
        VmwSockCloseEventQueue( pRESTHandle, pSockContext->pEventQueue);
    }
    if (pSockContext->pListenerTCP)
    {
        VmwSockRelease( pRESTHandle, pSockContext->pListenerTCP);
    }
    if (pSockContext->pListenerTCP6)
    {
        VmwSockRelease( pRESTHandle, pSockContext->pListenerTCP6);
    }
    if (pSockContext->pWorkerThreads)
    {
        DWORD iThr = 0;

        for (; iThr < pSockContext->dwNumThreads; iThr++)
        {
            PVMREST_THREAD pThread = pSockContext->pWorkerThreads[iThr];

            if (pThread)
            {
                VmRESTFreeThread(pThread);
            }
        }

        VmRESTFreeMemory(pSockContext->pWorkerThreads);
    }
    if (pSockContext->pMutex)
    {
        VmRESTFreeMutex(pSockContext->pMutex);
    }
}

static
DWORD
VmRESTDisconnectClient(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket
    )
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;

    if (!pSocket)
    {
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMREST_ERROR(dwError);
    }

    VmwSockClose( pRESTHandle, pSocket);
    VmwSockRelease( pRESTHandle, pSocket);

cleanup:

    return dwError;

error:

    goto cleanup;
}

uint32_t
VmRESTCommonWriteDataAtOnce(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket,
    char*                            pszBuffer,
    uint32_t                         nBytes
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;

    dwError = VmwSockWrite(
                  pRESTHandle,
                  pSocket,
                  pszBuffer,
                  nBytes
                  );
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:

    return dwError;

error:

    goto cleanup;
}

uint32_t
VmRESTCommonGetPeerInfo(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket,
    char*                            pIpAddress,
    uint32_t                         nLen,
    int*                             pPortNo
    )
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;

    dwError = VmwSockGetPeerInfo(
                  pRESTHandle,
                  pSocket,
                  pIpAddress,
                  nLen,
                  pPortNo
                  );
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:
    
    return dwError;

error:

    goto cleanup;
}

