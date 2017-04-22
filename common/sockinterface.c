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
    PVM_SOCK_IO_BUFFER               pIoBuffer,
    DWORD                            dwError
    );

static
DWORD
VmRESTOnNewConnection(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket,
    PVM_SOCK_IO_BUFFER               pIoBuffer
    );

static
VOID
VmRESTOnDisconnect(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket,
    PVM_SOCK_IO_BUFFER               pIoBuffer
    );

static
DWORD
VmRESTOnDataAvailable(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket,
    PVM_SOCK_IO_BUFFER               pIoBuffer
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
    PVM_SOCKET                       pSocket
    );

static
DWORD
VmRESTReceiveData(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket,
    PVM_SOCK_IO_BUFFER               pIoBuffer
    );

static
DWORD
VmRESTTcpReceiveData(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket,
    PVM_SOCK_IO_BUFFER               pIoBuffer
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

    if (! pRESTHandle || !( pRESTHandle->pRESTConfig))
    {
        VMREST_LOG_ERROR(pRESTHandle,"%s","Invalid REST config");
        dwError = REST_ENGINE_INVALID_HANDLER;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    pSockContext =  pRESTHandle->pSockContext;

    dwError = VmRESTAllocateMutex(&pSockContext->pMutex);
    BAIL_ON_VMREST_ERROR(dwError);

    /**** Init SSL if configured ****/
    if (( pRESTHandle->pRESTConfig->server_port != NULL) && (strlen( pRESTHandle->pRESTConfig->server_port) == 0))
    {
        VMREST_LOG_ERROR(pRESTHandle,"%s","REST Engine config server port missing");
        dwError = 111;  /** Fix this **/
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
        if (strlen( pRESTHandle->pRESTConfig->ssl_certificate) == 0 || strlen( pRESTHandle->pRESTConfig->ssl_key) == 0)
        {
            VMREST_LOG_ERROR(pRESTHandle,"%s", "Invalid SSL params");
            dwError =  112; /** Fix this **/
        }
        BAIL_ON_VMREST_ERROR(dwError);
        dwFlags = dwFlags | VM_SOCK_IS_SSL;
        sslCert =  pRESTHandle->pRESTConfig->ssl_certificate;
        sslKey =   pRESTHandle->pRESTConfig->ssl_key;
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
    BAIL_ON_VMREST_ERROR(dwError);
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
    dwError = VmwSockEventQueueAdd(
                   pRESTHandle,
                  pSockContext->pEventQueue,
                  pSockContext->pListenerTCP6
                  );
    BAIL_ON_VMREST_ERROR(dwError);
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

    goto cleanup;
}

VOID
VmRESTShutdownProtocolServer(
    PVMREST_HANDLE                   pRESTHandle
    )
{
    if ( pRESTHandle)
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
    PVMREST_HANDLE                   pRESTHandle = pWorkerData-> pRESTHandle;
    PVMREST_SOCK_CONTEXT             pSockContext = pWorkerData->pSockContext;
    PVM_SOCKET                       pSocket = NULL;
    PVM_SOCK_IO_BUFFER               pIoBuffer = NULL;

    if (pWorkerData != NULL)
    {
        VmRESTFreeMemory(pWorkerData);
        pWorkerData = NULL;
    }

    for(;;)
    {
        VM_SOCK_EVENT_TYPE eventType = VM_SOCK_EVENT_TYPE_UNKNOWN;

        dwError = VmwSockWaitForEvent(
                         pRESTHandle,
                        pSockContext->pEventQueue,
                        -1,
                        &pSocket,
                        &eventType,
                        &pIoBuffer);

        if (dwError == ERROR_SHUTDOWN_IN_PROGRESS)
        {
            break;
        }
        dwError = VmRESTHandleSocketEvent(
                         pRESTHandle,
                        pSocket,
                        eventType,
                        pIoBuffer,
                        dwError);

        if (dwError == ERROR_SUCCESS ||
            dwError == ERROR_IO_PENDING)
        { 
            dwError = ERROR_SUCCESS;
        }
        else
        {
            pSocket = NULL;
            pIoBuffer = NULL;
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
    PVM_SOCK_IO_BUFFER               pIoBuffer,
    DWORD                            dwError
    )
{
    if (dwError == ERROR_SUCCESS)
    {
        switch (sockEvent)
        {
        case VM_SOCK_EVENT_TYPE_TCP_NEW_CONNECTION:

            dwError = VmRESTOnNewConnection( pRESTHandle, pSocket, pIoBuffer);
            BAIL_ON_VMREST_ERROR(dwError);
            break;
#ifndef WIN32
        case VM_SOCK_EVENT_TYPE_DATA_AVAILABLE:
            dwError = VmRESTOnDataAvailable( pRESTHandle,pSocket, pIoBuffer);
            BAIL_ON_VMREST_ERROR(dwError);
            break;

        case VM_SOCK_EVENT_TYPE_CONNECTION_CLOSED:
            VmRESTOnDisconnect( pRESTHandle, pSocket, pIoBuffer);
            break;

        case VM_SOCK_EVENT_TYPE_UNKNOWN:
             dwError = ERROR_INVALID_STATE;
             break;

        default:
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
VmRESTOnNewConnection(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket,
    PVM_SOCK_IO_BUFFER               pIoBuffer
    )
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;
    if (!pSocket)
    {
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMREST_ERROR(dwError);
    }

#ifdef WIN32
    dwError = VmRESTTcpReceiveNewData( pRESTHandle, pSocket);
    BAIL_ON_VMREST_ERROR(dwError);
#endif
cleanup:

    return dwError;

error:

    goto cleanup;
}

static
VOID
VmRESTOnDisconnect(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket,
    PVM_SOCK_IO_BUFFER               pIoBuffer
    )
{
    if (pSocket)
    {
        VmwSockClose( pRESTHandle, pSocket);
    }

    if (pIoBuffer)
    {
        VmwSockReleaseIoBuffer( pRESTHandle, pIoBuffer);
    }
}

static
DWORD
VmRESTOnDataAvailable(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket,
    PVM_SOCK_IO_BUFFER               pIoBuffer
    )
{
    DWORD dwError = REST_ENGINE_SUCCESS;

    if (!pSocket)
    {
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMREST_ERROR(dwError);
    }

    dwError = VmRESTReceiveData( pRESTHandle,pSocket, pIoBuffer);
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:
    if (pIoBuffer)
    {
        VmwSockReleaseIoBuffer( pRESTHandle, pIoBuffer);
    }

    return dwError;

error:

    goto cleanup;
}

static
DWORD
VmRESTReceiveData(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket,
    PVM_SOCK_IO_BUFFER               pIoBuffer
    )
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;

    if (!pSocket)
    {
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMREST_ERROR(dwError);
    }

    dwError = VmRESTTcpReceiveData( pRESTHandle,pSocket, pIoBuffer);
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:
    if (pIoBuffer)
    {
        VmwSockReleaseIoBuffer( pRESTHandle, pIoBuffer);
    }

    return dwError;

error:

    goto cleanup;

}

static
DWORD
VmRESTTcpReceiveData(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket,
    PVM_SOCK_IO_BUFFER               pIoBuffer
    )
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;

    if (!pSocket)
    {
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMREST_ERROR(dwError);
    }

    if (!pIoBuffer)
    {
        dwError = VmRESTTcpReceiveNewData( pRESTHandle,pSocket);
        BAIL_ON_VMREST_ERROR(dwError);
    }

cleanup:
    if (pIoBuffer)
    {
        VmwSockReleaseIoBuffer( pRESTHandle, pIoBuffer);
    }

    return dwError;

error:

    goto cleanup;
}

static
DWORD
VmRESTTcpReceiveNewData(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket
    )
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;
    char                             appBuffer[MAX_DATA_BUFFER_LEN] = {0};
    uint32_t                         bytesRead = 0;

    dwError = VmsockPosixGetXBytes(
                   pRESTHandle,
                  MAX_DATA_BUFFER_LEN,
                  appBuffer,
                  pSocket,
                  &bytesRead,
                  0
                  );
     BAIL_ON_VMREST_ERROR(dwError);


     if (bytesRead > 0)
     {
         VMREST_LOG_DEBUG(pRESTHandle,"%s","Starting HTTP Parsing.");
         dwError = VmRESTProcessIncomingData(
                        pRESTHandle,
                       appBuffer,
                       bytesRead,
                       pSocket
                       );
         BAIL_ON_VMREST_ERROR(dwError);
     }


cleanup:
    VMREST_LOG_DEBUG(pRESTHandle,"%s","Calling closed connection....");
    VmRESTDisconnectClient( pRESTHandle, pSocket);

    return dwError;

error:

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
VmsockPosixGetXBytes(
    PVMREST_HANDLE                   pRESTHandle,
    uint32_t                         bytesRequested,
    char*                            appBuffer,
    PVM_SOCKET                       pSocket,
    uint32_t*                        bytesRead,
    uint8_t                          shouldBlock
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    uint32_t                         dataIndex = 0;
    uint32_t                         remainingBytes = 0;
    uint32_t                         dataAvailableInCache = 0;
    PVM_SOCK_IO_BUFFER               pIoBuffer = NULL;
    PVM_STREAM_BUFFER                pStreamBuffer = NULL;

    if (bytesRequested > MAX_DATA_BUFFER_LEN || appBuffer == NULL || bytesRead == NULL)
    {
       VMREST_LOG_DEBUG(pRESTHandle,"%s","Bytes to be read %u Large or appBuffer: %s",
                         bytesRequested, appBuffer);
        dwError = VMREST_TRANSPORT_INVALID_PARAM;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    if (sizeof(appBuffer) > MAX_DATA_BUFFER_LEN)
    {
        VMREST_LOG_DEBUG(pRESTHandle,"%s","ERROR: Application buffer size too large");
        dwError = VMREST_TRANSPORT_INVALID_PARAM;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    VmwSockGetStreamBuffer( pRESTHandle, pSocket, &pStreamBuffer);

    if (!pStreamBuffer)
    {
        dwError = 500;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    dataIndex = pStreamBuffer->dataProcessed;

    dataAvailableInCache = pStreamBuffer->dataRead - pStreamBuffer->dataProcessed;

    if (dataAvailableInCache >= bytesRequested)
    {
        /**** Enough data available in stream cache buffer ****/
        memcpy(appBuffer,
               &(pStreamBuffer->pData[dataIndex]),
               bytesRequested
               );
        pStreamBuffer->dataProcessed += bytesRequested;
        *bytesRead = bytesRequested;
    }
    else if(dataAvailableInCache < bytesRequested)
    {
        /**** Copy all remaining client Stream bytes and perform read ****/
        if (dataAvailableInCache > 0)
        {
            memcpy(appBuffer,
                   &(pStreamBuffer->pData[dataIndex]),
                   dataAvailableInCache
                   );
            pStreamBuffer->dataProcessed += dataAvailableInCache;
            /**** This will be overwritten in case of success ****/
            *bytesRead = dataAvailableInCache;
        }

        dwError = VmwSockAllocateIoBuffer(
                         pRESTHandle,
                        VM_SOCK_EVENT_TYPE_TCP_REQUEST_DATA_READ,
                        MAX_DATA_BUFFER_LEN,
                        &pIoBuffer
                        );
        BAIL_ON_VMREST_ERROR(dwError);
        dwError = VmwSockRead(
                             pRESTHandle,
                            pSocket,
                            pIoBuffer);
		VMREST_LOG_DEBUG(pRESTHandle,"SockRead(), dwError = %u, dataRead %u", dwError, pIoBuffer->dwBytesTransferred);
        if (dwError == ERROR_SUCCESS)
        {
            memset(pStreamBuffer->pData, '\0', MAX_DATA_BUFFER_LEN);
            memcpy(pStreamBuffer->pData, pIoBuffer->pData,pIoBuffer->dwBytesTransferred);
			pStreamBuffer->dataProcessed = 0;
			pStreamBuffer->dataRead = pIoBuffer->dwBytesTransferred;
			BAIL_ON_VMREST_ERROR(dwError);
        }
        else if (dwError == ERROR_IO_PENDING)
        {
            // fail for linux?
#ifndef WIN32
            pIoBuffer = NULL;
#endif 
        }
        else
        {
            BAIL_ON_VMREST_ERROR(dwError);
        }

        remainingBytes = bytesRequested - dataAvailableInCache;
        dataIndex = 0;

        if (remainingBytes > pStreamBuffer->dataRead)
        {
            remainingBytes = pStreamBuffer->dataRead;
            VMREST_LOG_DEBUG(pRESTHandle,"WARNING: Requested %u bytes, available only %u bytes", bytesRequested,(dataAvailableInCache + remainingBytes));
        }

        memcpy((appBuffer + dataAvailableInCache),
              &(pStreamBuffer->pData[dataIndex]),
              remainingBytes);
        pStreamBuffer->dataProcessed = remainingBytes;
        *bytesRead = dataAvailableInCache + remainingBytes;

        VMREST_LOG_DEBUG(pRESTHandle,"dataAvailableInCache %u, remainingBytes %u, appBuffersize %u", dataAvailableInCache, remainingBytes, strlen(appBuffer));
    }

    VmwSockSetStreamBuffer( pRESTHandle, pSocket, pStreamBuffer);

cleanup:
    if (pIoBuffer)
    {
        VmwSockReleaseIoBuffer( pRESTHandle, pIoBuffer);
    }

    return dwError;

error:

    goto cleanup;

}

uint32_t
VmSockPosixAdjustProcessedBytes(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket,
    uint32_t                         dataSeen
)
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    PVM_STREAM_BUFFER                pStreamBuffer = NULL;

    if (dataSeen > MAX_DATA_BUFFER_LEN)
    {
       VMREST_LOG_DEBUG(pRESTHandle,"%s","Invalid new Processed Data Index %u", dataSeen);
        dwError = VMREST_TRANSPORT_INVALID_PARAM;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    VmwSockGetStreamBuffer( pRESTHandle, pSocket, &pStreamBuffer);

    if (!pStreamBuffer)
    {
        dwError = 500;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    pStreamBuffer->dataProcessed = dataSeen;

    VmwSockSetStreamBuffer( pRESTHandle, pSocket, pStreamBuffer);

cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VmsockPosixWriteDataAtOnce(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket,
    char*                            buffer,
    uint32_t                         bytes
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    PVM_SOCK_IO_BUFFER               pIoNewBuffer = NULL;
    
    dwError = VmwSockAllocateIoBuffer(
                   pRESTHandle,
                  VM_SOCK_EVENT_TYPE_TCP_RESPONSE_DATA_WRITE,
                  bytes,
                  &pIoNewBuffer);
    BAIL_ON_VMREST_ERROR(dwError);

    memcpy(pIoNewBuffer->pData, buffer, bytes);

    dwError = VmwSockWrite(
                   pRESTHandle,
                  pSocket,
                  NULL,
                  0,
                  pIoNewBuffer
                  );
    if (dwError == ERROR_SUCCESS)
    {
        dwError = REST_ENGINE_SUCCESS;
        BAIL_ON_VMREST_ERROR(dwError);
    }
    else if (dwError == ERROR_IO_PENDING)
    {
        pIoNewBuffer = NULL;
        BAIL_ON_VMREST_ERROR(dwError);
    }

cleanup:
    if (pIoNewBuffer)
    {
        VmwSockReleaseIoBuffer( pRESTHandle, pIoNewBuffer);
    }
    return dwError;
error:
    goto cleanup;
}
