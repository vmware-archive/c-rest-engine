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

PVMREST_SOCK_CONTEXT gpSockContext = NULL;
PVM_REST_CONFIG      gpRESTConfig = NULL;

static
VOID
VmRESTSockContextFree(
    PVMREST_SOCK_CONTEXT             pSockInterface
    );

static
DWORD
VmRESTHandleSocketEvent(
    PVM_SOCKET                       pSocket,
    VM_SOCK_EVENT_TYPE               sockEvent,
    PVM_SOCK_IO_BUFFER               pIoBuffer,
    DWORD                            dwError
    );

static
DWORD
VmRESTOnNewConnection(
    PVM_SOCKET                       pSocket,
    PVM_SOCK_IO_BUFFER               pIoBuffer
    );

static
VOID
VmRESTOnDisconnect(
    PVM_SOCKET                       pSocket,
    PVM_SOCK_IO_BUFFER               pIoBuffer
    );

static
DWORD
VmRESTOnDataAvailable(
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
    PVM_SOCKET                       pSocket
    );

static
DWORD
VmRESTReceiveData(
    PVM_SOCKET                       pSocket,
    PVM_SOCK_IO_BUFFER               pIoBuffer
    );

static
DWORD
VmRESTTcpReceiveData(
    PVM_SOCKET                       pSocket,
    PVM_SOCK_IO_BUFFER               pIoBuffer
    );

static
DWORD
VmRESTDisconnectClient(
    PVM_SOCKET                       pSocket
    );

static
PVOID
VmRESTSockWorkerThreadProc(
    PVOID                            pData
    );

DWORD
VmRESTInitProtocolServer(
    VOID
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

    if (!gpRESTConfig)
    {
        VMREST_LOG_ERROR("REST Engine configuration missing");
        dwError = 110;  /** Fix this **/
    }
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTAllocateMemory(
                  sizeof(*pSockContext),
                  (PVOID*)&pSockContext
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTAllocateMutex(&pSockContext->pMutex);
    BAIL_ON_VMREST_ERROR(dwError);

    /**** Init SSL if configured ****/

    if (!(gpRESTConfig->server_port))
    {
        VMREST_LOG_ERROR("REST Engine config server port missing");
        dwError = 111;  /** Fix this **/
    }
    BAIL_ON_VMREST_ERROR(dwError);

    lastPortChar = VmRESTUtilsGetLastChar(
                       gpRESTConfig->server_port
                       );
    if (lastPortChar == 's' || lastPortChar == 'S')
    {
        if (strlen(gpRESTConfig->ssl_certificate) == 0 || strlen(gpRESTConfig->ssl_key) == 0)
        {
            VMREST_LOG_ERROR("Invalid SSL params");
            dwError =  112; /** Fix this **/
        }
        dwFlags = dwFlags | VM_SOCK_IS_SSL;
        sslCert = gpRESTConfig->ssl_certificate;
        sslKey = gpRESTConfig->ssl_key;
        temp = gpRESTConfig->server_port;
        while(temp != NULL)
        {
            if (*temp == 's' || *temp == 'S')
            {
                *temp = '\0';
                break;
            }
            temp++;
        }
    }

    /**** Handle IPv4 case ****/

    dwError = VmwSockOpenServer(
                        ((unsigned short)atoi(gpRESTConfig->server_port)),
                        ((int)atoi(gpRESTConfig->worker_thread_count)),
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
                  ((unsigned short)atoi(gpRESTConfig->server_port)),
                  ((int)atoi(gpRESTConfig->worker_thread_count)),
                  dwFlags | VM_SOCK_CREATE_FLAGS_TCP |
                          VM_SOCK_CREATE_FLAGS_IPV6,
                  &pSockContext->pListenerTCP6,
                  sslCert,
                  sslKey
                  );
    BAIL_ON_VMREST_ERROR(dwError);
#endif

    dwError = VmwSockCreateEventQueue(
                  -1, 
                  &pSockContext->pEventQueue
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmwSockEventQueueAdd(
                  pSockContext->pEventQueue,
                  pSockContext->pListenerTCP
                  );
    BAIL_ON_VMREST_ERROR(dwError);

#ifdef AF_INET6
    dwError = VmwSockEventQueueAdd(
                  pSockContext->pEventQueue,
                  pSockContext->pListenerTCP6
                  );
    BAIL_ON_VMREST_ERROR(dwError);
#endif

#ifdef WIN32
    dwError = VmwSockStartListening(
                  pSockContext->pListenerTCP,
                  ((int)atoi(gpRESTConfig->worker_thread_count))
                  );
    BAIL_ON_VMREST_ERROR(dwError);
#endif

    dwError = VmRESTAllocateMemory(
                  sizeof(PVMREST_THREAD) * (((int)atoi(gpRESTConfig->worker_thread_count))) ,
                  (PVOID*)&pSockContext->pWorkerThreads
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    pSockContext->dwNumThreads = ((int)atoi(gpRESTConfig->worker_thread_count));

    for (; iThr < pSockContext->dwNumThreads; iThr++)
    {
        dwError = VmRESTAllocateMemory(
                      sizeof(VMREST_THREAD),
                      (void **)&pSockContext->pWorkerThreads[iThr]
                      );
        BAIL_ON_VMREST_ERROR(dwError);

        dwError = VmRESTCreateThread(
                      pSockContext->pWorkerThreads[iThr],
                      TRUE,
                      (PVMREST_START_ROUTINE)&VmRESTSockWorkerThreadProc,
                      pSockContext
                      );
        BAIL_ON_VMREST_ERROR(dwError);
    }

    gpSockContext = pSockContext;

cleanup:

    return dwError;

error:

    if (pSockContext)
    {
        VmRESTSockContextFree(pSockContext);
    }

    goto cleanup;
}

VOID
VmRESTShutdownProtocolServer(
    VOID
    )
{
    if (gpSockContext)
    {
        VmRESTSockContextFree(gpSockContext);
        gpSockContext = NULL;
    }
}

static
PVOID
VmRESTSockWorkerThreadProc(
    PVOID pData
    )
{
    DWORD dwError = 0;
    PVMREST_SOCK_CONTEXT pSockContext = (PVMREST_SOCK_CONTEXT)pData;
    PVM_SOCKET pSocket = NULL;
    PVM_SOCK_IO_BUFFER pIoBuffer = NULL;


    for(;;)
    {
        VM_SOCK_EVENT_TYPE eventType = VM_SOCK_EVENT_TYPE_UNKNOWN;

        dwError = VmwSockWaitForEvent(
                        pSockContext->pEventQueue,
                        -1,
                        &pSocket,
                        &eventType,
                        &pIoBuffer);


        if (dwError == ERROR_SHUTDOWN_IN_PROGRESS)
        {
            VMREST_LOG_INFO(
                "%s shutdown in progress, exit sock worker loop.",
                __FUNCTION__);
            break;
        }

        dwError = VmRESTHandleSocketEvent(
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
            if (pSocket != pSockContext->pListenerUDP)
            {
                VmRESTDisconnectClient(pSocket);
                pSocket = NULL;
            }
            pIoBuffer = NULL;
            dwError = 0;
        }
        BAIL_ON_VMREST_ERROR(dwError);
    }
error:

    if (pSocket)
    {
        VmwSockRelease(pSocket);
    }
    return NULL;
}



static
DWORD
VmRESTHandleSocketEvent(
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

            dwError = VmRESTOnNewConnection(pSocket, pIoBuffer);
            BAIL_ON_VMREST_ERROR(dwError);
            break;
#ifndef WIN32
        case VM_SOCK_EVENT_TYPE_DATA_AVAILABLE:
            dwError = VmRESTOnDataAvailable(pSocket, pIoBuffer);
            BAIL_ON_VMREST_ERROR(dwError);
            break;

        case VM_SOCK_EVENT_TYPE_CONNECTION_CLOSED:

            VmRESTOnDisconnect(pSocket, pIoBuffer);
            break;
        case VM_SOCK_EVENT_TYPE_UNKNOWN:
             /**** This is for case when hot fd is returned twice as process and read on fd is not protected under mutex ****/
             /**** DO NOTHING ****/
             break;

        default:
            dwError = ERROR_INVALID_MESSAGE;
            break;
#endif
        }
    }

cleanup:

    if (dwError != ERROR_IO_PENDING && pIoBuffer)
    {
        //VmwSockReleaseIoBuffer(pIoBuffer);
    }

    return dwError;

error :
    goto cleanup;
}

static
DWORD
VmRESTOnNewConnection(
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
    dwError = VmRESTTcpReceiveNewData(pSocket);
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
    PVM_SOCKET                       pSocket,
    PVM_SOCK_IO_BUFFER               pIoBuffer
    )
{
    if (pSocket)
    {
        VmwSockClose(pSocket);
    }

    if (pIoBuffer)
    {
        VmwSockReleaseIoBuffer(pIoBuffer);
    }
}

static
DWORD
VmRESTOnDataAvailable(
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

    dwError = VmRESTReceiveData(pSocket, pIoBuffer);
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:
    if (pIoBuffer)
    {
        VmwSockReleaseIoBuffer(pIoBuffer);
    }

    return dwError;

error:

    goto cleanup;
}

static
DWORD
VmRESTReceiveData(
    PVM_SOCKET                       pSocket,
    PVM_SOCK_IO_BUFFER               pIoBuffer
    )
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;
    DWORD                            dwProtocol = 0;

    if (!pSocket)
    {
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMREST_ERROR(dwError);
    }

    dwError = VmwSockGetProtocol(pSocket, &dwProtocol);
    BAIL_ON_VMREST_ERROR(dwError);

    if (dwProtocol == SOCK_STREAM)
    {
        dwError = VmRESTTcpReceiveData(pSocket, pIoBuffer);
        BAIL_ON_VMREST_ERROR(dwError);
    }
    else
    {
       VMREST_LOG_DEBUG("%s","%s", "Not a TCP socket");
    }

cleanup:
    if (pIoBuffer)
    {
        VmwSockReleaseIoBuffer(pIoBuffer);
    }

    return dwError;

error:

    goto cleanup;

}

static
DWORD
VmRESTTcpReceiveData(
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
        dwError = VmRESTTcpReceiveNewData(pSocket);
        BAIL_ON_VMREST_ERROR(dwError);
    }

cleanup:
    if (pIoBuffer)
    {
        VmwSockReleaseIoBuffer(pIoBuffer);
    }

    return dwError;

error:

    goto cleanup;
}

static
DWORD
VmRESTTcpReceiveNewData(
    PVM_SOCKET                       pSocket
    )
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;
    char                             appBuffer[MAX_DATA_BUFFER_LEN] = {0};
    uint32_t                         bytesRead = 0;

    dwError = VmsockPosixGetXBytes(
                  MAX_DATA_BUFFER_LEN,
                  appBuffer,
                  pSocket,
                  &bytesRead,
                  0
                  );
     BAIL_ON_VMREST_ERROR(dwError);


     if (bytesRead > 0)
     {
         VMREST_LOG_DEBUG("%s","Starting HTTP Parsing.");
         dwError = VmRESTProcessIncomingData(
                       appBuffer,
                       bytesRead,
                       pSocket
                       );
         BAIL_ON_VMREST_ERROR(dwError);
     }
     VmwSockRelease(pSocket);
     VmRESTDisconnectClient(pSocket);

     VMREST_LOG_DEBUG("%s","Calling closed connection....");

cleanup:

    return dwError;

error:

    goto cleanup;
}

static
VOID
VmRESTSockContextFree(
    PVMREST_SOCK_CONTEXT             pSockContext
    )
{
    if (pSockContext->pEventQueue)
    {
        VmwSockCloseEventQueue(pSockContext->pEventQueue);
    }
    if (pSockContext->pListenerTCP)
    {
        VmwSockRelease(pSockContext->pListenerTCP);
    }
    if (pSockContext->pListenerUDP)
    {
        VmwSockClose(pSockContext->pListenerUDP);
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
    VmRESTFreeMemory(pSockContext);
}

static
DWORD
VmRESTDisconnectClient(
    PVM_SOCKET                       pSocket
    )
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;

    if (!pSocket)
    {
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMREST_ERROR(dwError);
    }

    VmwSockClose(pSocket);
    VmwSockRelease(pSocket);

cleanup:

    return dwError;

error:

    goto cleanup;
}

uint32_t
VmsockPosixGetXBytes(
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
       VMREST_LOG_DEBUG("%s","Bytes to be read %u Large or appBuffer: %s",
                         bytesRequested, appBuffer);
        dwError = VMREST_TRANSPORT_INVALID_PARAM;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    if (sizeof(appBuffer) > MAX_DATA_BUFFER_LEN)
    {
      VMREST_LOG_DEBUG("%s","ERROR: Application buffer size too large");
        dwError = VMREST_TRANSPORT_INVALID_PARAM;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    VmwSockGetStreamBuffer(pSocket, &pStreamBuffer);

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
        }
//readAgain:

        dwError = VmwSockAllocateIoBuffer(
                        VM_SOCK_EVENT_TYPE_TCP_REQUEST_DATA_READ,
                        MAX_DATA_BUFFER_LEN,
                        &pIoBuffer
                        );
        BAIL_ON_VMREST_ERROR(dwError);

        dwError = VmwSockRead(
                            pSocket,
                            pIoBuffer);

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

        /*** TODO:: Implement thread blocking if required ****/
        /*
        if ((pSocket->pStreamBuffer->dataRead == 0) && (shouldBlock == 1))
        {
           VMREST_LOG_DEBUG("%s","DEBUG: Read returned 0 or -1, will block client with index %d unless data arrives", clientIndex);
            pthread_mutex_lock(&(gServerSocketInfo.clients[clientIndex].mtxWaitForData));
            pthread_cond_wait(&(gServerSocketInfo.clients[clientIndex].condDataAvaialble), &(gServerSocketInfo.clients[clientIndex].mtxWaitForData));
            pthread_mutex_unlock(&(gServerSocketInfo.clients[clientIndex].mtxWaitForData));
            goto readAgain;
        }
        */

        remainingBytes = bytesRequested - dataAvailableInCache;
        dataIndex = 0;

        if (remainingBytes > pStreamBuffer->dataRead)
        {
            remainingBytes = pStreamBuffer->dataRead;
            VMREST_LOG_DEBUG("WARNING: Requested %u bytes, available only %u bytes", bytesRequested,(dataAvailableInCache + remainingBytes));
        }

        memcpy((appBuffer + dataAvailableInCache),
              &(pStreamBuffer->pData[dataIndex]),
              remainingBytes);
        pStreamBuffer->dataProcessed = remainingBytes;
        *bytesRead = dataAvailableInCache + remainingBytes;
    }

    VmwSockSetStreamBuffer(pSocket, pStreamBuffer);

cleanup:
    if (pIoBuffer)
    {
        VmwSockReleaseIoBuffer(pIoBuffer);
    }

    return dwError;

error:

    goto cleanup;

}

uint32_t
VmSockPosixAdjustProcessedBytes(
    PVM_SOCKET                       pSocket,
    uint32_t                         dataSeen
)
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    PVM_STREAM_BUFFER                pStreamBuffer = NULL;

    if (dataSeen > MAX_DATA_BUFFER_LEN)
    {
       VMREST_LOG_DEBUG("%s","Invalid new Processed Data Index %u", dataSeen);
        dwError = VMREST_TRANSPORT_INVALID_PARAM;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    VmwSockGetStreamBuffer(pSocket, &pStreamBuffer);

    if (!pStreamBuffer)
    {
        dwError = 500;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    pStreamBuffer->dataProcessed = dataSeen;

    VmwSockSetStreamBuffer(pSocket, pStreamBuffer);

cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VmsockPosixWriteDataAtOnce(
    PVM_SOCKET                       pSocket,
    char*                            buffer,
    uint32_t                         bytes
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    PVM_SOCK_IO_BUFFER               pIoNewBuffer = NULL;
    
    dwError = VmwSockAllocateIoBuffer(
                  VM_SOCK_EVENT_TYPE_TCP_RESPONSE_DATA_WRITE,
                  bytes,
                  &pIoNewBuffer);
    BAIL_ON_VMREST_ERROR(dwError);

    memcpy(pIoNewBuffer->pData, buffer, bytes);

    dwError = VmwSockWrite(
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
        VmwSockReleaseIoBuffer(pIoNewBuffer);
    }
    return dwError;
error:
    goto cleanup;
}

void
VmRESTSetConfig(
   PVM_REST_CONFIG                   pRESTConfig
   )
{
    gpRESTConfig = pRESTConfig;
}

void
VmRESTUnSetConfig(
    void
    )
{
    gpRESTConfig = NULL;
}
