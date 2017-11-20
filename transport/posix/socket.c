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
DWORD
VmSockPosixCreateSignalSockets(
    PVM_SOCKET*                      ppReaderSocket,
    PVM_SOCKET*                      ppWriterSocket
    );

static
DWORD
VmSockPosixEventQueueAdd_inlock(
    PVM_SOCK_EVENT_QUEUE             pQueue,
    BOOLEAN                          bOneShot,
    PVM_SOCKET                       pSocket
    );

static
DWORD
VmSockPosixEventQueueDelete_inlock(
    PVM_SOCK_EVENT_QUEUE             pQueue,
    PVM_SOCKET                       pSocket
    );

static
DWORD
VmSockPosixAcceptConnection(
    PVM_SOCKET                       pListener,
    PVM_SOCKET*                      ppSocket
    );

static
DWORD
VmSockPosixSetDescriptorNonBlocking(
    int                              fd
    );

static
DWORD
VmSockPosixSetReuseAddress(
    int                              fd
    );

static
VOID
VmSockPosixFreeEventQueue(
    PVM_SOCK_EVENT_QUEUE             pQueue
    );

static
VOID
VmSockPosixFreeSocket(
    PVM_SOCKET                       pSocket
    );

static
uint32_t
VmSockPosixCreateTimer(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket
    );

static
uint32_t
VmSockPosixReArmTimer(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pTimerSocket,
    int                              milliSec
    );

static
BOOLEAN
VmSockPosixIsSafeToCloseConnOnTimeOut(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pTimerSocket
    );

static
uint32_t
VmRESTAcceptSSLContext(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket,
    BOOLEAN                          bWatched
    );

static
uint32_t
VmRESTCreateSSLObject(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket
    );



DWORD
VmSockPosixOpenServer(
    PVMREST_HANDLE                   pRESTHandle,
    VM_SOCK_CREATE_FLAGS             dwFlags,
    PVM_SOCKET*                      ppSocket
    )
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;
    union
    {
#ifdef AF_INET6
        struct sockaddr_in6          servaddr_ipv6;
#endif
        struct sockaddr_in           servaddr_ipv4;
    } servaddr;
    struct
    {
        int                          domain;
        int                          type;
        int                          protocol;
    } socketParams;
    struct sockaddr*                 pSockAddr = NULL;
    socklen_t                        addrLen = 0;
    int                              fd = -1;
    PVM_SOCKET                       pSocket = NULL;
    PVM_SOCK_SSL_INFO                pSSLInfo = NULL;
    BOOLEAN                          bLocked = FALSE;

    if (!pRESTHandle || !pRESTHandle->pSSLInfo || !pRESTHandle->pRESTConfig)
    {
        dwError = REST_ERROR_INVALID_HANDLER;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    if (dwFlags & VM_SOCK_CREATE_FLAGS_IPV6)
    {
#ifdef AF_INET6
        socketParams.domain = AF_INET6;
#else
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_VMREST_ERROR(dwError);
#endif
    }
    else
    {
        socketParams.domain = AF_INET;
    }

    if (dwFlags & VM_SOCK_CREATE_FLAGS_UDP)
    {
        socketParams.type = SOCK_DGRAM;
    }
    else
    {
        socketParams.type = SOCK_STREAM;
    }

    socketParams.protocol = 0;

    pSSLInfo = pRESTHandle->pSSLInfo;

    /**** Check if connection is over SSL ****/
    if (pRESTHandle->pRESTConfig->isSecure)
    {
        VMREST_LOG_DEBUG(pRESTHandle,"%s","Server initing in encrypted wire connection mode");

        if (pRESTHandle->pRESTConfig->pSSLContext != NULL)
        {
            VMREST_LOG_INFO(pRESTHandle,"%s","Using Application provided SSL Context");
            pRESTHandle->pSSLInfo->sslContext = pRESTHandle->pRESTConfig->pSSLContext;
        }
        else
        {
            if ((IsNullOrEmptyString(pRESTHandle->pRESTConfig->pszSSLCertificate)) || (IsNullOrEmptyString(pRESTHandle->pRESTConfig->pszSSLKey)))
            {
                VMREST_LOG_ERROR(pRESTHandle,"%s", "Invalid SSL params");
                dwError =  REST_ERROR_INVALID_CONFIG;
            }
            BAIL_ON_VMREST_ERROR(dwError);

            pthread_mutex_lock(&gGlobalMutex);
            bLocked = TRUE;

            if (gSSLisedInstaceCount == INVALID)
            {
                SSL_library_init();
                dwError = VmRESTSecureSocket(
                              pRESTHandle,
                              pRESTHandle->pRESTConfig->pszSSLCertificate,
                              pRESTHandle->pRESTConfig->pszSSLKey
                              );
                BAIL_ON_VMREST_ERROR(dwError);

                gSSLisedInstaceCount = 0;
                dwError = VmRESTSSLThreadLockInit();
                BAIL_ON_VMREST_ERROR(dwError);
                gpSSLCTX = pRESTHandle->pSSLInfo->sslContext;
            }
            else
            {
                pRESTHandle->pSSLInfo->sslContext = gpSSLCTX;
            }
            gSSLisedInstaceCount++;
            pthread_mutex_unlock(&gGlobalMutex);
            bLocked = FALSE;

            VMREST_LOG_DEBUG(pRESTHandle,"%s","Using REST Engine Generated SSL Context");
        }
        pSSLInfo->isSecure = 1;
    }
    else
    {
        VMREST_LOG_WARNING(pRESTHandle,"%s","Server initing in plain text wire connection mode");
        pSSLInfo->isSecure = 0;
    }

    fd = socket(socketParams.domain, socketParams.type, socketParams.protocol);
    if (fd < 0)
    {
        VMREST_LOG_ERROR(pRESTHandle,"Socket call failed with Error code %d", errno);
        dwError = VM_SOCK_POSIX_ERROR_SYS_CALL_FAILED;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    if (dwFlags & VM_SOCK_CREATE_FLAGS_REUSE_ADDR)
    {
        dwError = VmSockPosixSetReuseAddress(fd);
        BAIL_ON_VMREST_ERROR(dwError);
    }

    memset(&servaddr, 0, sizeof(servaddr));

    if (dwFlags & VM_SOCK_CREATE_FLAGS_IPV6)
    {
#ifdef AF_INET6
        servaddr.servaddr_ipv6.sin6_family = AF_INET6;
        servaddr.servaddr_ipv6.sin6_addr = in6addr_any;
        servaddr.servaddr_ipv6.sin6_port = htons((unsigned short)pRESTHandle->pRESTConfig->serverPort);

        pSockAddr = (struct sockaddr*) &servaddr.servaddr_ipv6;
        addrLen = sizeof(servaddr.servaddr_ipv6);

#if defined(SOL_IPV6) && defined(IPV6_V6ONLY)
        int one = 1;
        int ret = 0;
        ret = setsockopt(fd, SOL_IPV6, IPV6_V6ONLY, (void *) &one, sizeof(one));
        if (ret != 0)
        {
            dwError = ERROR_NOT_SUPPORTED;
            BAIL_ON_VMREST_ERROR(dwError);
        }
#endif

#else
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_VMREST_ERROR(dwError);
#endif
    }
    else
    {
        servaddr.servaddr_ipv4.sin_family = AF_INET;
        servaddr.servaddr_ipv4.sin_addr.s_addr = htonl(INADDR_ANY);
        servaddr.servaddr_ipv4.sin_port = htons((unsigned short)pRESTHandle->pRESTConfig->serverPort);

        pSockAddr = (struct sockaddr*) &servaddr.servaddr_ipv4;
        addrLen = sizeof(servaddr.servaddr_ipv4);
    }

    if (bind(fd, pSockAddr, addrLen) < 0)
    {
        VMREST_LOG_ERROR(pRESTHandle,"bind() call failed with Error code %d", errno);
        dwError = VM_SOCK_POSIX_ERROR_SYS_CALL_FAILED;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    if (dwFlags & VM_SOCK_CREATE_FLAGS_NON_BLOCK)
    {
        dwError = VmSockPosixSetDescriptorNonBlocking(fd);
        BAIL_ON_VMREST_ERROR(dwError);
    }

    if (listen(fd, VM_SOCK_POSIX_DEFAULT_LISTEN_QUEUE_SIZE) < 0)
    {
         VMREST_LOG_ERROR(pRESTHandle,"Listen() on server socket with fd %d failed with Error code %d", fd, errno);
         dwError = VM_SOCK_POSIX_ERROR_SYS_CALL_FAILED;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    VMREST_LOG_DEBUG(pRESTHandle,"Server Listening on socket with fd %d ...", fd);

    dwError = VmRESTAllocateMemory(
                  sizeof(*pSocket),
                  (PVOID*)&pSocket
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTAllocateMutex(&pSocket->pMutex);
    BAIL_ON_VMREST_ERROR(dwError);

    pSocket->type = VM_SOCK_TYPE_LISTENER;
    pSocket->fd = fd;
    pSocket->ssl = NULL;
    pSocket->pTimerSocket = NULL;
    pSocket->pIoSocket = NULL;

    *ppSocket = pSocket;

cleanup:

    if (bLocked)
    {
        pthread_mutex_unlock(&gGlobalMutex);
        bLocked = FALSE;
    }

    return dwError;

error:

    if (ppSocket)
    {
        *ppSocket = NULL;
    }

    if (pSocket)
    {
        VmSockPosixFreeSocket(pSocket);
    }
    if (fd >= 0)
    {
        close(fd);
    }

    goto cleanup;
}

DWORD
VmSockPosixCreateEventQueue(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCK_EVENT_QUEUE*            ppQueue
    )
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;
    PVM_SOCK_EVENT_QUEUE             pQueue = NULL;
    uint32_t                         iEventQueueSize = VM_SOCK_POSIX_DEFAULT_QUEUE_SIZE;

    if (!ppQueue || !pRESTHandle || !pRESTHandle->pRESTConfig)
    {
        VMREST_LOG_ERROR(pRESTHandle,"Invalid params");
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMREST_ERROR(dwError);
    }

    dwError = VmRESTAllocateMemory(
                  sizeof(*pQueue),
                  (PVOID*)&pQueue
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmSockPosixCreateSignalSockets(
                  &pQueue->pSignalReader,
                  &pQueue->pSignalWriter
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTAllocateMutex(&pQueue->pMutex);
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTAllocateMemory(
                  iEventQueueSize * sizeof(*pQueue->pEventArray),
                  (PVOID*)&pQueue->pEventArray
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    pQueue->dwSize = iEventQueueSize;

    pQueue->epollFd = epoll_create1(0);
    if (pQueue->epollFd < 0)
    {
        VMREST_LOG_ERROR(pRESTHandle,"epoll create failed with Error code %d", errno);
        dwError = VM_SOCK_POSIX_ERROR_SYS_CALL_FAILED;
        BAIL_ON_VMREST_ERROR(dwError);
    }

    pQueue->state  = VM_SOCK_POSIX_EVENT_STATE_WAIT;
    pQueue->nReady = -1;
    pQueue->iReady = 0;
    pQueue->bShutdown = 0;
    pQueue->thrCnt = pRESTHandle->pRESTConfig->nWorkerThr;

    dwError = VmSockPosixEventQueueAdd_inlock(
                  pQueue,
                  FALSE,
                  pQueue->pSignalReader
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    *ppQueue = pQueue;
    pRESTHandle->pSSLInfo->bQueueInUse = TRUE;

    VMREST_LOG_DEBUG(pRESTHandle,"Event queue creation successful");

cleanup:

    return dwError;

error:

    if (ppQueue)
    {
        *ppQueue = NULL;
    }

    if (pQueue)
    {
        VmSockPosixFreeEventQueue(pQueue);
    }

    goto cleanup;
}

DWORD
VmSockPosixEventQueueAdd(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCK_EVENT_QUEUE             pQueue,
    PVM_SOCKET                       pSocket
    )
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;
    BOOLEAN                          bLocked = TRUE;

    if (!pQueue || !pSocket)
    {
        VMREST_LOG_ERROR(pRESTHandle,"Invalid params");
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMREST_ERROR(dwError);
    }

    dwError = VmRESTLockMutex(pQueue->pMutex);
    BAIL_ON_VMREST_ERROR(dwError);

    bLocked = TRUE;

    dwError = VmSockPosixEventQueueAdd_inlock(
                  pQueue,
                  FALSE,
                  pSocket
                  );
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:

    if (bLocked && pQueue)
    {
        VmRESTUnlockMutex(pQueue->pMutex);
    }

    return dwError;

error:

    goto cleanup;
}

DWORD
VmSockPosixWaitForEvent(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCK_EVENT_QUEUE             pQueue,
    int                              iTimeoutMS,
    PVM_SOCKET*                      ppSocket,
    PVM_SOCK_EVENT_TYPE              pEventType
    )
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;
    BOOLEAN                          bLocked = FALSE;
    VM_SOCK_EVENT_TYPE               eventType = VM_SOCK_EVENT_TYPE_UNKNOWN;
    PVM_SOCKET                       pSocket = NULL;
    BOOLEAN                          bFreeEventQueue = 0;

    if (!pQueue || !ppSocket || !pEventType)
    {
        VMREST_LOG_ERROR(pRESTHandle,"Invalid params");
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMREST_ERROR(dwError);
    }

    dwError = VmRESTLockMutex(pQueue->pMutex);
    BAIL_ON_VMREST_ERROR(dwError);

    bLocked = TRUE;

    if ((pQueue->state == VM_SOCK_POSIX_EVENT_STATE_PROCESS) &&
        (pQueue->iReady >= pQueue->nReady))
    {
        pQueue->state = VM_SOCK_POSIX_EVENT_STATE_WAIT;
    }

    if (pQueue->state == VM_SOCK_POSIX_EVENT_STATE_WAIT)
    {
        pQueue->iReady = 0;
        pQueue->nReady = -1;

        while (pQueue->nReady < 0)
        {
            pQueue->nReady = epoll_wait(
                                 pQueue->epollFd,
                                 pQueue->pEventArray,
                                 pQueue->dwSize,
                                 iTimeoutMS
                                 );
            if ((pQueue->nReady < 0) && (errno != EINTR))
            {
                VMREST_LOG_ERROR(pRESTHandle,"epoll_wait() failed with Error code %d", errno);
                dwError = VM_SOCK_POSIX_ERROR_SYS_CALL_FAILED;
                BAIL_ON_VMREST_ERROR(dwError);
            }
        }
        pQueue->state = VM_SOCK_POSIX_EVENT_STATE_PROCESS;
    }

    if (pQueue->state == VM_SOCK_POSIX_EVENT_STATE_PROCESS)
    {
        if (pQueue->iReady < pQueue->nReady)
        {
            struct epoll_event* pEvent = &pQueue->pEventArray[pQueue->iReady];
            PVM_SOCKET pEventSocket = (PVM_SOCKET)pEvent->data.ptr;

            if (!pEventSocket)
            {
                VMREST_LOG_ERROR(pRESTHandle,"%s","Bad socket information");
                dwError = ERROR_INVALID_STATE;
                BAIL_ON_VMREST_ERROR(dwError);
            }

            if (pEvent->events & (EPOLLERR | EPOLLHUP))
            {
                eventType = VM_SOCK_EVENT_TYPE_CONNECTION_CLOSED;
                dwError = VmSockPosixEventQueueDelete_inlock(
                              pQueue,
                              pEventSocket
                              );
                BAIL_ON_VMREST_ERROR(dwError);
                pSocket = pEventSocket;
            }
            else if (pEventSocket->type == VM_SOCK_TYPE_LISTENER)
            {
                dwError = VmSockPosixAcceptConnection(
                              pEventSocket,
                              &pSocket);
                BAIL_ON_VMREST_ERROR(dwError);

                dwError = VmSockPosixSetNonBlocking(pRESTHandle,pSocket);
                BAIL_ON_VMREST_ERROR(dwError);

                /**** If conn is over SSL, do the needful ****/
                if (pRESTHandle->pSSLInfo->isSecure)
                {
                    dwError = VmRESTCreateSSLObject(
                                   pRESTHandle,
                                   pSocket
                                   );
                    BAIL_ON_VMREST_ERROR(dwError);

                    dwError  = VmRESTAcceptSSLContext(
                                   pRESTHandle,
                                   pSocket,
                                   FALSE
                                   );
                    BAIL_ON_VMREST_ERROR(dwError);
                
                }

                dwError = VmSockPosixEventQueueAdd_inlock(
                              pQueue,
                              TRUE,
                              pSocket
                              );
                BAIL_ON_VMREST_ERROR(dwError);

                /**** Create and add the timer socket also ****/
                dwError = VmSockPosixCreateTimer(
                              pRESTHandle,
                              pSocket
                              );
                BAIL_ON_VMREST_ERROR(dwError);
                eventType = VM_SOCK_EVENT_TYPE_TCP_NEW_CONNECTION;
            }
            else if (pEventSocket->type == VM_SOCK_TYPE_SIGNAL)
            {
                if (pQueue->bShutdown)
                {
                    pQueue->thrCnt--;
                    if (pQueue->thrCnt == 0)
                    {
                        bFreeEventQueue = TRUE;
                    }

                    dwError = ERROR_SHUTDOWN_IN_PROGRESS;
                    BAIL_ON_VMREST_ERROR(dwError);
                }
                else
                {
                    pSocket = pEventSocket;
                    eventType = VM_SOCK_EVENT_TYPE_DATA_AVAILABLE;
                }
            }
            else if (pEventSocket->type == VM_SOCK_TYPE_TIMER)
            {
                /**** No activity on the socket watched by poller till timeout ****/
                if (VmSockPosixIsSafeToCloseConnOnTimeOut(pRESTHandle, pEventSocket))
                {
                    pSocket = pEventSocket;

                    /**** Delete timer socket from poller ****/
                    dwError = VmSockPosixEventQueueDelete_inlock(
                                  pQueue,
                                  pSocket
                                  );
                    BAIL_ON_VMREST_ERROR(dwError);

                    /**** Delete actual IO socket from poller ****/
                    dwError = VmSockPosixEventQueueDelete_inlock(
                                  pQueue,
                                  pSocket->pIoSocket
                                  );
                    BAIL_ON_VMREST_ERROR(dwError);

                    pSocket = pEventSocket->pIoSocket;
                    eventType = VM_SOCK_EVENT_TYPE_CONNECTION_TIMEOUT;
                }
            }
            else
            {
                /**** Data is available over the socket ****/
                pSocket = pEventSocket;

                /**** If SSL handshake is not yet complete, do the needful ****/
                if ((pRESTHandle->pSSLInfo->isSecure) && (!(pSocket->bSSLHandShakeCompleted)))
                {
                    dwError = VmRESTAcceptSSLContext(
                                  pRESTHandle,
                                  pSocket,
                                  TRUE
                                  );
                    BAIL_ON_VMREST_ERROR(dwError);
                }
                else
                {
                    eventType = VM_SOCK_EVENT_TYPE_DATA_AVAILABLE;

                    /**** Stop timer on the socket ****/
                    dwError = VmSockPosixReArmTimer(
                                   pRESTHandle,
                                   pSocket->pTimerSocket,
                                   0
                                   );
                    BAIL_ON_VMREST_ERROR(dwError);
                }
            }
        }
        pQueue->iReady++;
    }

    *ppSocket = pSocket;
    *pEventType = eventType;

cleanup:

    if (pQueue && bLocked)
    {
        VmRESTUnlockMutex(pQueue->pMutex);
    }

    if (dwError == ERROR_SHUTDOWN_IN_PROGRESS && bFreeEventQueue)
    {
        VmSockPosixFreeEventQueue(pQueue);
        pRESTHandle->pSSLInfo->bQueueInUse = FALSE;

        if (pRESTHandle->pSSLInfo->isSecure == 1)
        {
            VmRESTSecureSocketShutdown(pRESTHandle);
        }
    }

    return dwError;

error:

    if (ppSocket)
    {
        *ppSocket = NULL;
    }
    if (pEventType)
    {
        *pEventType = VM_SOCK_EVENT_TYPE_UNKNOWN;
    }

    if (pSocket)
    {
        VmSockPosixCloseSocket(pRESTHandle,pSocket);
        VmSockPosixReleaseSocket(pRESTHandle,pSocket);
    }

    /**** If we are bailing on error on event, we must mark that event as processed ****/
    if ( pQueue && (pQueue->state == VM_SOCK_POSIX_EVENT_STATE_PROCESS) && (pQueue->iReady < pQueue->nReady))
    {
        pQueue->iReady++;
    }

    goto cleanup;
}

DWORD
VmSockPosixCloseEventQueue(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCK_EVENT_QUEUE             pQueue,
    uint32_t                         waitSecond
    )
{
 
    DWORD                            dwError = REST_ENGINE_SUCCESS;
    uint32_t                         retry = 0;

    if (pQueue)
    {
        if (pQueue->pSignalWriter)
        {
            char szBuf[] = {0};
            pQueue->bShutdown = 1;
            write(pQueue->pSignalWriter->fd, szBuf, sizeof(szBuf));
        }
    }

    /**** Worker threads are detached threads, give them some time for cleanup. Block upto 10 seconds *****/

    while(retry <= waitSecond)
    {
        if (pRESTHandle->pSSLInfo->bQueueInUse == FALSE)
        {
           break;
        }
        sleep(1);
        retry++;
    }

    if (pRESTHandle->pSSLInfo->bQueueInUse == TRUE)
    {
        /**** This is not a clean stop of the server ****/
        dwError = REST_ENGINE_FAILURE;
    }

    return dwError;
}

DWORD
VmSockPosixSetNonBlocking(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket
    )
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;
    BOOLEAN                          bLocked = FALSE;

    dwError = VmRESTLockMutex(pSocket->pMutex);
    BAIL_ON_VMREST_ERROR(dwError);

    bLocked = TRUE;

    dwError = VmSockPosixSetDescriptorNonBlocking(pSocket->fd);
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:

    if (bLocked)
    {
        VmRESTUnlockMutex(pSocket->pMutex);
    }

    return dwError;

error:

    goto cleanup;
}


DWORD
VmSockPosixRead(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket,
    char**                           ppszBuffer,
    uint32_t*                        nBufLen
    )
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;
    BOOLEAN                          bLocked = FALSE;
    ssize_t                          nRead   = 0;
    uint32_t                         errorCode = 0;
    char*                            pszBufPrev = NULL;
    uint32_t                         nPrevBuf = 0;

    if (!pSocket || !ppszBuffer || !nBufLen || !pRESTHandle)
    {
        VMREST_LOG_ERROR(pRESTHandle,"%s","Invalid params");
        dwError = ERROR_INVALID_PARAMETER;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTLockMutex(pSocket->pMutex);
    BAIL_ON_VMREST_ERROR(dwError);

    bLocked = TRUE;
    if (pSocket->pszBuffer)
    {
        nPrevBuf = pSocket->nBufData - pSocket->nProcessed;
        VMREST_LOG_DEBUG(pRESTHandle,"Data from prev read %u", nPrevBuf);
        /**** copy unprocessed data in local buffer ****/

        if (nPrevBuf > 0)
        {
            dwError = VmRESTAllocateMemory(
                          nPrevBuf,
                          (void **)&pszBufPrev
                          );
            BAIL_ON_VMREST_ERROR(dwError);

            memcpy(pszBufPrev, (pSocket->pszBuffer + pSocket->nProcessed), nPrevBuf);
        }
        VmRESTFreeMemory(pSocket->pszBuffer);
        pSocket->pszBuffer = NULL;
        pSocket->nBufData = 0;
        pSocket->nProcessed = 0;
    }

    dwError = VmRESTReallocateMemory(
                  (void*)pszBufPrev,
                  (void **)&pszBufPrev,
                  (nPrevBuf + MAX_DATA_BUFFER_LEN)
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    memset((pszBufPrev + nPrevBuf), '\0', MAX_DATA_BUFFER_LEN);

    do
    {
        nRead = 0;
        if (pRESTHandle->pSSLInfo->isSecure && (pSocket->ssl != NULL))
        {
            nRead = SSL_read(pSocket->ssl, (pszBufPrev + nPrevBuf), MAX_DATA_BUFFER_LEN);
            errorCode = SSL_get_error(pSocket->ssl, nRead);
        }
        else if (pSocket->fd > 0)
        {
            nRead = read(pSocket->fd, (void*)(pszBufPrev + nPrevBuf), MAX_DATA_BUFFER_LEN);
            errorCode = errno;
        }

        if ((nRead > 0) && (nRead <= MAX_DATA_BUFFER_LEN))
        {
            nPrevBuf += nRead;
            dwError = VmRESTReallocateMemory(
                  (void*)pszBufPrev,
                  (void **)&pszBufPrev,
                  (nPrevBuf + MAX_DATA_BUFFER_LEN)
                  );
            BAIL_ON_VMREST_ERROR(dwError);
            memset((pszBufPrev + nPrevBuf), '\0', MAX_DATA_BUFFER_LEN);
        }
    }while((nRead > 0) && (nPrevBuf < pRESTHandle->pRESTConfig->maxDataPerConnMB));

    if (((pSocket->fd > 0) && (errorCode == EAGAIN || errorCode == EWOULDBLOCK)) || ((pRESTHandle->pSSLInfo->isSecure) && (errorCode == SSL_ERROR_WANT_READ)))
    {
        dwError = REST_ENGINE_SUCCESS;
    }
    else if (nRead < 0)
    {
        VMREST_LOG_ERROR(pRESTHandle, "Socket read failed with error code %u", errorCode);
        dwError = errorCode;
    }
    else if (nRead == 0)
    {
        VMREST_LOG_ERROR(pRESTHandle,"%s", "Socket read failed due to broken pipe");
        dwError = VM_SOCK_POSIX_ERROR_BROKEN_PIPE;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    if (nPrevBuf >= pRESTHandle->pRESTConfig->maxDataPerConnMB)
    {
        /**** Discard the request here itself. This might be the first read IO cycle ****/
        VMREST_LOG_ERROR(pRESTHandle,"Total Data in request %u bytes is over allowed limit of %u bytes, closing connection...", nPrevBuf, pRESTHandle->pRESTConfig->maxDataPerConnMB);
        dwError = REST_ENGINE_FAILURE;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    pSocket->pszBuffer = pszBufPrev;
    pSocket->nProcessed = 0;
    pSocket->nBufData = nPrevBuf;
    
    *ppszBuffer = pSocket->pszBuffer;
    *nBufLen = pSocket->nBufData;

    VMREST_LOG_DEBUG(pRESTHandle,"Read status, total bytes(including prev) %u", *nBufLen);

cleanup:

    if (bLocked)
    {
        VmRESTUnlockMutex(pSocket->pMutex);
    }

    return dwError;

error:

    if (pSocket)
    {
        VMREST_LOG_ERROR(pRESTHandle,"Socket read failed with Socket fd %d, dwError = %u, nRead = %d, errno = %u, errorCode = %u", pSocket->fd, dwError, nRead, errno, errorCode);
    }
    else
    {
        VMREST_LOG_ERROR(pRESTHandle,"Socket read failed with dwError = %u, nRead = %d, errno = %u, errorCode = %u", dwError, nRead, errno, errorCode);
    }

    if (pszBufPrev && pSocket && pRESTHandle->pSockContext)
    {
        /**** Delete the socket from poller ****/
        VmSockPosixEventQueueDelete_inlock(
            pRESTHandle->pSockContext->pEventQueue,
            pSocket
            );

        VmRESTFreeMemory(pszBufPrev);
        pszBufPrev = NULL;
        pSocket->pszBuffer = NULL;
        pSocket->nProcessed = 0;
        pSocket->nBufData = 0;
    }

    if (nBufLen)
    {
        *nBufLen = 0;
    }

    if (ppszBuffer)
    {
        *ppszBuffer = NULL;
    }

    goto cleanup;
}

DWORD
VmSockPosixWrite(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket,
    char*                            pszBuffer,
    uint32_t                         nBufLen
    )
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;
    BOOLEAN                          bLocked  = FALSE;
    ssize_t                          nWritten = 0;
    uint32_t                         nRemaining = 0;
    uint32_t                         nWrittenTotal = 0;
    uint32_t                         errorCode = 0;
    uint32_t                         cntRty = 0;
    int                              maxTry = 1000;
    uint32_t                         timerMs = 1;
    int                              timeOutSec = 5;

    if (!pRESTHandle || !pSocket || !pszBuffer)
    {
        VMREST_LOG_ERROR(pRESTHandle,"Invalid params");
        dwError = ERROR_INVALID_PARAMETER;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    nRemaining = nBufLen;

    dwError = VmRESTLockMutex(pSocket->pMutex);
    BAIL_ON_VMREST_ERROR(dwError);

    bLocked = TRUE;

    while(nWrittenTotal < nBufLen )
    {
         if (pRESTHandle->pSSLInfo->isSecure && (pSocket->ssl != NULL))
         {
             nWritten = SSL_write(pSocket->ssl,(pszBuffer + nWrittenTotal),nRemaining);
             errorCode = SSL_get_error(pSocket->ssl, nWritten);
         }
         else if (pSocket->fd > 0)
         {
             nWritten = write(pSocket->fd, (pszBuffer + nWrittenTotal) ,nRemaining);
             errorCode = errno;
         }
         if (nWritten > 0)
         {
             nWrittenTotal += nWritten;
             nRemaining -= nWritten;
             VMREST_LOG_DEBUG(pRESTHandle,"\nBytes written this write %d, Total bytes written %u", nWritten, nWrittenTotal);
             nWritten = 0;
             /**** reset to original values ****/
             maxTry = 1000;
             timerMs = 1;
             timeOutSec = 5;
         }
         else
         {
             if (errorCode == EAGAIN || errorCode == EWOULDBLOCK || errorCode == SSL_ERROR_WANT_WRITE)
             {
                 if (timeOutSec >= 0)
                 {
                     cntRty++;
                     usleep((timerMs * 1000));
                     if (cntRty >= maxTry)
                     {
                         timerMs = ((timerMs >= 1000) ? 1000 : (timerMs*10));
                         maxTry = ((maxTry <= 1) ? 1 : (maxTry/10));
                         timeOutSec--;
                         cntRty = 0;
                     }
                     VMREST_LOG_DEBUG(pRESTHandle,"retry write");
                     nWritten = 0;
                     continue;
                 }
                 else
                 {
                     VMREST_LOG_ERROR(pRESTHandle,"%s", "Exhausted maximum time to write data");
                     dwError = VM_SOCK_POSIX_ERROR_SYS_CALL_FAILED;
                     BAIL_ON_VMREST_ERROR(dwError);
                 }
             }
             else
             {
                 dwError = VM_SOCK_POSIX_ERROR_SYS_CALL_FAILED;
                 VMREST_LOG_ERROR(pRESTHandle,"Socket write failed with error code %u, dwError %u, nWritten %d", errorCode, dwError, nWritten);
                 BAIL_ON_VMREST_ERROR(dwError);
             }
        }
    }
    VMREST_LOG_DEBUG(pRESTHandle,"\nWrite Status on Socket with fd = %d\nRequested: %d nBufLen\nWritten %d bytes\n", pSocket->fd, nBufLen, nWrittenTotal);

cleanup:

    if (bLocked)
    {
        VmRESTUnlockMutex(pSocket->pMutex);
    }

    return dwError;

error:

    goto cleanup;
}

VOID
VmSockPosixReleaseSocket(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket
    )
{
    if (pSocket)
    {
        if (pSocket->pTimerSocket)
        {
             VmSockPosixFreeSocket(pSocket->pTimerSocket);
        }
        VmSockPosixFreeSocket(pSocket);
    }
}

DWORD
VmSockPosixCloseSocket(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket
    )
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;
    BOOLEAN                          bLocked = FALSE;

    if (!pRESTHandle || !pSocket )
    {
        VMREST_LOG_ERROR(pRESTHandle,"%s","Invalid Params..");
        dwError = ERROR_INVALID_PARAMETER;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTLockMutex(pSocket->pMutex);
    BAIL_ON_VMREST_ERROR(dwError);

    bLocked = TRUE;

    if (pSocket->pTimerSocket && (pSocket->pTimerSocket->fd > 0))
    {
        close(pSocket->pTimerSocket->fd);
        pSocket->pTimerSocket->fd = -1;
    }

    if (pSocket->fd >= 0)
    {
        close(pSocket->fd);
        pSocket->fd = -1;
    }

    if (pRESTHandle->pSSLInfo->isSecure && pSocket->ssl)
    {
        SSL_shutdown(pSocket->ssl);
        SSL_free(pSocket->ssl);
        pSocket->ssl = NULL;
    }

cleanup:

    if (bLocked)
    {
        VmRESTUnlockMutex(pSocket->pMutex);
    }

    return dwError;

error:

    goto cleanup;
}

static
DWORD
VmSockPosixCreateSignalSockets(
    PVM_SOCKET*                      ppReaderSocket,
    PVM_SOCKET*                      ppWriterSocket
    )
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;
    PVM_SOCKET                       pReaderSocket = NULL;
    PVM_SOCKET                       pWriterSocket = NULL;
    PVM_SOCKET*                      sockets[] = { &pReaderSocket, &pWriterSocket };
    int                              fdPair[] = { -1, -1 };
    DWORD                            iSock = 0;

    if (!ppReaderSocket || !ppWriterSocket)
    {
        dwError = ERROR_INVALID_PARAMETER;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    if (socketpair(AF_LOCAL, SOCK_STREAM, 0, fdPair) < 0)
    {
        dwError = VM_SOCK_POSIX_ERROR_SYS_CALL_FAILED;
        BAIL_ON_VMREST_ERROR(dwError);
    }

    for (; iSock < sizeof(sockets)/sizeof(sockets[0]); iSock++)
    {
        PVM_SOCKET pSocket = NULL;

        dwError = VmRESTAllocateMemory(
                      sizeof(VM_SOCKET),
                      (PVOID*)sockets[iSock]
                      );
        BAIL_ON_VMREST_ERROR(dwError);

        pSocket = *sockets[iSock];

        dwError = VmRESTAllocateMutex(&pSocket->pMutex);
        BAIL_ON_VMREST_ERROR(dwError);

        pSocket->type = VM_SOCK_TYPE_SIGNAL;
        pSocket->fd = fdPair[iSock];

        fdPair[iSock] = -1;
    }

    *ppReaderSocket = pReaderSocket;
    *ppWriterSocket = pWriterSocket;

cleanup:

    return dwError;

error:

    if (ppReaderSocket)
    {
        *ppReaderSocket = NULL;
    }
    if (ppWriterSocket)
    {
        *ppWriterSocket = NULL;
    }

    if (pReaderSocket)
    {
        VmSockPosixFreeSocket(pReaderSocket);
    }
    if (pWriterSocket)
    {
        VmSockPosixFreeSocket(pWriterSocket);
    }
    for (iSock = 0; iSock < sizeof(fdPair)/sizeof(fdPair[0]); iSock++)
    {
        if (fdPair[iSock] >= 0)
        {
            close(fdPair[iSock]);
        }
    }

    goto cleanup;
}

static
DWORD
VmSockPosixEventQueueAdd_inlock(
    PVM_SOCK_EVENT_QUEUE             pQueue,
    BOOLEAN                          bOneShot,
    PVM_SOCKET                       pSocket
    )
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;
    struct                           epoll_event event = {0};

    if (!pSocket || !pQueue)
    {
        dwError = REST_ERROR_INVALID_HANDLER;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    event.data.ptr = pSocket;
    event.events = EPOLLIN;

    if (bOneShot)
    {
       event.events = event.events | EPOLLONESHOT;
    }

    if (epoll_ctl(pQueue->epollFd, EPOLL_CTL_ADD, pSocket->fd, &event) < 0)
    {
        dwError = VM_SOCK_POSIX_ERROR_SYS_CALL_FAILED;
        BAIL_ON_VMREST_ERROR(dwError);
    }

error:

    return dwError;
}

static
DWORD
VmSockPosixEventQueueDelete_inlock(
    PVM_SOCK_EVENT_QUEUE             pQueue,
    PVM_SOCKET                       pSocket
    )
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;
    struct                           epoll_event event = {0};

    if (epoll_ctl(pQueue->epollFd, EPOLL_CTL_DEL, pSocket->fd, &event) < 0)
    {
        dwError = VM_SOCK_POSIX_ERROR_SYS_CALL_FAILED;
        BAIL_ON_VMREST_ERROR(dwError);
    }

error:

    return dwError;
}

static
DWORD
VmSockPosixAcceptConnection(
    PVM_SOCKET                       pListener,
    PVM_SOCKET*                      ppSocket
    )
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;
    PVM_SOCKET                       pSocket = NULL;
    int                              fd = 0;

    dwError = VmRESTAllocateMemory(
                  sizeof(*pSocket),
                  (PVOID*)&pSocket
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTAllocateMutex(&pSocket->pMutex);
    BAIL_ON_VMREST_ERROR(dwError);

    pSocket->type = VM_SOCK_TYPE_SERVER;

    fd = accept(pListener->fd, &pSocket->addr, &pSocket->addrLen);
    if (fd < 0)
    {
        dwError = VM_SOCK_POSIX_ERROR_SYS_CALL_FAILED;
        BAIL_ON_VMREST_ERROR(dwError);
    }

    pSocket->fd = fd;
    pSocket->ssl = NULL;
    pSocket->pRequest = NULL;
    pSocket->pszBuffer = NULL;
    pSocket->pTimerSocket = NULL;
    pSocket->pIoSocket = NULL;
    pSocket->bSSLHandShakeCompleted = FALSE;

    *ppSocket = pSocket;

cleanup:

    return dwError;

error:

    *ppSocket = NULL;

    if (pSocket)
    {
        VmSockPosixFreeSocket(pSocket);
    }
    if (fd >= 0)
    {
        close(fd);
    }

    goto cleanup;
}

static
DWORD
VmSockPosixSetDescriptorNonBlocking(
    int                              fd
    )
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;
    int                              flags = 0;

    if ((flags = fcntl(fd, F_GETFL, 0)) < 0)
    {
        dwError = VM_SOCK_POSIX_ERROR_SYS_CALL_FAILED;
        BAIL_ON_VMREST_ERROR(dwError);
    }

    flags |= O_NONBLOCK;

    if ((flags = fcntl(fd, F_SETFL, flags)) < 0)
    {
        dwError = VM_SOCK_POSIX_ERROR_SYS_CALL_FAILED;
        BAIL_ON_VMREST_ERROR(dwError);
    }

error:

    return dwError;
}

static
DWORD
VmSockPosixSetReuseAddress(
    int                              fd
    )
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;
    int                              on = 1;

    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
    {
        dwError = VM_SOCK_POSIX_ERROR_SYS_CALL_FAILED;
        BAIL_ON_VMREST_ERROR(dwError);
    }

error:

    return dwError;
}

static
VOID
VmSockPosixFreeEventQueue(
    PVM_SOCK_EVENT_QUEUE             pQueue
    )
{
    if (!pQueue)
    {
        return;
    }
    
    if (pQueue->pSignalReader)
    {   
        VmSockPosixFreeSocket(pQueue->pSignalReader);
        pQueue->pSignalReader = NULL;
    }
    if (pQueue->pSignalWriter)
    {
        VmSockPosixFreeSocket(pQueue->pSignalWriter);
        pQueue->pSignalWriter = NULL;
    }
    if (pQueue->pMutex)
    {
        VmRESTFreeMutex(pQueue->pMutex);
        pQueue->pMutex = NULL;
    }
    if (pQueue->epollFd >= 0)
    {
        close(pQueue->epollFd);
        pQueue->epollFd = -1;
    }
    if (pQueue->pEventArray)
    {
        VmRESTFreeMemory(pQueue->pEventArray);
        pQueue->pEventArray = NULL;
    }
    if(pQueue)
    {
        VmRESTFreeMemory(pQueue);
        pQueue = NULL;
    }
}

static
VOID
VmSockPosixFreeSocket(
    PVM_SOCKET                       pSocket
    )
{
    if (pSocket->pMutex)
    {
        VmRESTFreeMutex(pSocket->pMutex);
    }

    if (pSocket->pszBuffer)
    {
        VmRESTFreeMemory(pSocket->pszBuffer);
        pSocket->pszBuffer = NULL;
    }

    VmRESTFreeMemory(pSocket);
}

DWORD
VmSockPosixGetRequestHandle(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket,
    PREST_REQUEST*                   ppRequest
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    BOOLEAN                          bLocked = FALSE;

    if (!pRESTHandle || !pSocket)
    {
        VMREST_LOG_ERROR(pRESTHandle,"%s","Invalid Params..");
        dwError = ERROR_INVALID_PARAMETER;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTLockMutex(pSocket->pMutex);
    BAIL_ON_VMREST_ERROR(dwError);

    bLocked = TRUE;

    if (pSocket->pRequest)
    {
        *ppRequest = pSocket->pRequest;
    }
    else
    {
        *ppRequest = NULL;
    }

cleanup:

    if (bLocked)
    {
        VmRESTUnlockMutex(pSocket->pMutex);
    }

    return dwError;

error:

    goto cleanup;


}

DWORD
VmSockPosixSetRequestHandle(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket,
    PREST_REQUEST                    pRequest,
    uint32_t                         nProcessed,
    PVM_SOCK_EVENT_QUEUE             pQueue
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    BOOLEAN                          bLocked = FALSE;
    BOOLEAN                          bCompleted = FALSE;
    struct                           epoll_event event = {0};

    if (!pSocket || !pRESTHandle || !pQueue)
    {
        VMREST_LOG_ERROR(pRESTHandle, "%s", "Invalid params ...");
        dwError = ERROR_INVALID_PARAMETER;
    }
    BAIL_ON_VMREST_ERROR(dwError);
    
    dwError = VmRESTLockMutex(pSocket->pMutex);
    BAIL_ON_VMREST_ERROR(dwError);

    bLocked = TRUE;

    if (pRequest)
    {
        pSocket->pRequest = pRequest;
    }
    else
    {
        pSocket->pRequest = NULL;
        bCompleted = TRUE;
    }

    pSocket->nProcessed = nProcessed;

    /**** Rearm timer and add sockfd back to poller ****/
    if (!bCompleted)
    {
        dwError = VmSockPosixReArmTimer(
                      pRESTHandle,
                      pSocket->pTimerSocket,
                      ((pRESTHandle->pRESTConfig->connTimeoutSec) * 1000)
                      );
        BAIL_ON_VMREST_ERROR(dwError);

        event.data.ptr = pSocket;
        event.events = EPOLLIN;

        event.events = event.events | EPOLLONESHOT;

        if (epoll_ctl(pQueue->epollFd, EPOLL_CTL_MOD, pSocket->fd, &event) < 0)
        {
            dwError = VM_SOCK_POSIX_ERROR_SYS_CALL_FAILED;
            BAIL_ON_VMREST_ERROR(dwError);
        }
    }
    else
    {
        /**** Delete timerfd from poller ****/
        dwError = VmSockPosixEventQueueDelete_inlock(
                      pQueue,
                      pSocket->pTimerSocket
                      );
        BAIL_ON_VMREST_ERROR(dwError);

        /**** Delete actual IO socket from poller ****/
        dwError = VmSockPosixEventQueueDelete_inlock(
                      pQueue,
                      pSocket
                      );
        BAIL_ON_VMREST_ERROR(dwError);

        if (pSocket->pTimerSocket->fd > 0)
        {
            close(pSocket->pTimerSocket->fd);
            pSocket->pTimerSocket->fd = -1;
        } 
    }

cleanup:

    if (bLocked)
    {
        VmRESTUnlockMutex(pSocket->pMutex);
    }

    return dwError;

error:

    goto cleanup;
    

}


DWORD
VmSockPosixGetPeerInfo(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket,
    char*                            pIpAddress,
    uint32_t                         nLen,
    int*                             pPortNo
    )
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;
    socklen_t                        len = 0;
    struct sockaddr_storage          addr = {0};
    int                              ret = 0;
    struct sockaddr_in*              pIpV4 = NULL;
    struct sockaddr_in6*             pIpV6 = NULL;

    if (!pRESTHandle || !pSocket || !pIpAddress || !pPortNo || (nLen < INET6_ADDRSTRLEN))
    {
        VMREST_LOG_ERROR(pRESTHandle,"%s","Invalid params");
        dwError = ERROR_INVALID_PARAMETER;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    len = sizeof(addr);

    ret = getpeername(pSocket->fd, (struct sockaddr*)&addr, &len);

    if (ret < 0)
    {
        VMREST_LOG_ERROR(pRESTHandle,"%s","Invalid params");
        dwError = errno;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    if (addr.ss_family == AF_INET) 
    {
        pIpV4 = (struct sockaddr_in *)&addr;
        *pPortNo = ntohs(pIpV4->sin_port);
        inet_ntop(AF_INET, &pIpV4->sin_addr, pIpAddress, INET6_ADDRSTRLEN);
    } 
    else 
    {
        pIpV6 = (struct sockaddr_in6 *)&addr;
        *pPortNo = ntohs(pIpV6->sin6_port);
        inet_ntop(AF_INET6, &pIpV6->sin6_addr, pIpAddress, INET6_ADDRSTRLEN);
    }


cleanup:

    return dwError;

error:
    if (pIpAddress)
    {
        pIpAddress = NULL;
    }
    if (pPortNo)
    {
        pPortNo = NULL;
    }

    goto cleanup;

}

static
uint32_t
VmSockPosixReArmTimer(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pTimerSocket,
    int                              milliSec
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    struct                           itimerspec ts = {0};
    int                              timerFd = -1;
    int                              nSec = 0;
    int                              nNanoSec = 0;

    if (milliSec > 0)
    {
        nSec = milliSec / 1000;
        nNanoSec = (milliSec % 1000) * 1000000;
    }

    timerFd =pTimerSocket->fd;

    ts.it_interval.tv_sec = 0;
    ts.it_interval.tv_nsec = 0;
    ts.it_value.tv_sec = nSec;
    ts.it_value.tv_nsec = nNanoSec;
    
    if (timerfd_settime(timerFd, 0, &ts, NULL) < 0)
    {
        VMREST_LOG_ERROR(pRESTHandle,"%s","Set time failed");
        close(timerFd);
        dwError = VM_SOCK_POSIX_ERROR_SYS_CALL_FAILED;
    }
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:

    return dwError;

error:

    goto cleanup;

}

static
BOOLEAN
VmSockPosixIsSafeToCloseConnOnTimeOut(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pTimerSocket
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    struct                           epoll_event event = {0};
    BOOLEAN                          bCloseConn = FALSE;
    ssize_t                          nRead = 0;
    uint32_t                         errorCode = 0;
    uint64_t                         res = 0;
    char                             pBuf = '\0';
    struct                           itimerspec ts = {0};
    PVM_SOCKET                       pSocket = NULL;

    if (!pRESTHandle || !pTimerSocket)
    {
        VMREST_LOG_ERROR(pRESTHandle,"%s","Invalid params");
        dwError = ERROR_INVALID_PARAMETER;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    pSocket = pTimerSocket->pIoSocket;

    if ((pRESTHandle->pSSLInfo->isSecure) && (pSocket->ssl))
    {
        nRead = SSL_read(pSocket->ssl, &pBuf, 0);
        errorCode = SSL_get_error(pSocket->ssl, nRead);
    }
    else if (pSocket->fd > 0)
    {
        nRead = read(pSocket->fd, &pBuf, 0);
        errorCode = errno;
    }
    VMREST_LOG_DEBUG(pRESTHandle, "IO socket read bytes %d, errorCode %u, at timer expiration", nRead, errorCode);

    if (errorCode == EAGAIN || errorCode == EWOULDBLOCK || errorCode == SSL_ERROR_WANT_READ)
    {
        /**** Check for preceeding reset of timer due to valid IO ****/
        if (timerfd_gettime(pTimerSocket->fd, &ts) == 0)
        {
            if ((ts.it_value.tv_sec == 0) && (ts.it_value.tv_nsec == 0))
            {
                /**** timer is still disarmed ****/
                /**** It's safe to close connection ****/
                bCloseConn = TRUE;
            }

            /**** Do a read on timer socket - dummy read ****/
            do
            {
                errorCode = 0;
                nRead = 0;
                nRead = read(pTimerSocket->fd, &res, sizeof(res));
                errorCode = errno;
                res = 0;
            }while(nRead > 0);

            if (!bCloseConn)
            {
                /**** Add timer socket back to poller ****/
                event.data.ptr = pTimerSocket;
                event.events = EPOLLIN;
                event.events = event.events | EPOLLONESHOT;

                if (epoll_ctl(pRESTHandle->pSockContext->pEventQueue->epollFd, EPOLL_CTL_MOD, pTimerSocket->fd, &event) < 0)
                {
                    dwError = VM_SOCK_POSIX_ERROR_SYS_CALL_FAILED;
                    BAIL_ON_VMREST_ERROR(dwError);
                }
            }
        }
    }

cleanup:

    return bCloseConn;

error:

    bCloseConn = FALSE;

    goto cleanup;

}

static
uint32_t
VmSockPosixCreateTimer(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    int                              timerFd = INVALID;
    PVM_SOCKET                       pTimerSocket = NULL;

    if (!pSocket || !pRESTHandle)
    {
        VMREST_LOG_ERROR(pRESTHandle,"%s","Invalid params");
        dwError = ERROR_INVALID_PARAMETER;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    timerFd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
    if (timerFd == INVALID) 
    {
        VMREST_LOG_ERROR(pRESTHandle,"%s","Timer Creation failed");
        dwError = VM_SOCK_POSIX_ERROR_SYS_CALL_FAILED;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTAllocateMemory(
                  sizeof(*pTimerSocket),
                  (PVOID*)&pTimerSocket
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTAllocateMutex(&pTimerSocket->pMutex);
    BAIL_ON_VMREST_ERROR(dwError);

    pTimerSocket->type = VM_SOCK_TYPE_TIMER;
    pTimerSocket->fd = timerFd;
    pTimerSocket->pIoSocket = pSocket;
    pTimerSocket->pRequest = NULL;
    pTimerSocket->pszBuffer = NULL;
    pTimerSocket->nBufData = 0;
    pTimerSocket->nProcessed = 0;
    pTimerSocket->pTimerSocket = NULL;

    pSocket->pTimerSocket = pTimerSocket;

    dwError = VmSockPosixReArmTimer(
                  pRESTHandle,
                  pTimerSocket,
                  ((pRESTHandle->pRESTConfig->connTimeoutSec) * 1000)
                  );
    BAIL_ON_VMREST_ERROR(dwError);
    
    dwError = VmSockPosixEventQueueAdd_inlock(
                  pRESTHandle->pSockContext->pEventQueue,
                  TRUE,
                  pTimerSocket
                  );
    BAIL_ON_VMREST_ERROR(dwError);     
    

cleanup:

    return dwError;

error:

    if (pTimerSocket)
    {
        VmSockPosixFreeSocket(pTimerSocket);
        pTimerSocket = NULL;
    }

    if (pSocket)
    {
        pSocket->pTimerSocket = NULL;
    }

    goto cleanup;
    
}

static
uint32_t
VmRESTAcceptSSLContext(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket,
    BOOLEAN                          bWatched
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    int                              ret = 0;
    uint32_t                         errorCode = 0;
    BOOLEAN                          bReArm = FALSE;
    struct                           epoll_event event = {0};

    if (!pSocket || !pRESTHandle || !pRESTHandle->pSSLInfo || !pSocket->ssl || !pRESTHandle->pSockContext || !pRESTHandle->pSockContext->pEventQueue)
    {
        VMREST_LOG_ERROR(pRESTHandle,"%s","Invalid params");
        dwError = ERROR_INVALID_PARAMETER;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    ret = SSL_accept(pSocket->ssl);
    errorCode = SSL_get_error(pSocket->ssl, ret);

    if (ret == 1)
    {
        pSocket->bSSLHandShakeCompleted = TRUE;
        bReArm = TRUE;
    }
    else if ((ret == -1) && ((errorCode == SSL_ERROR_WANT_READ) || (errorCode == SSL_ERROR_WANT_WRITE)))
    {
       VMREST_LOG_DEBUG(pRESTHandle,"SSL handshake not completed for socket %d", pSocket->fd);
       pSocket->bSSLHandShakeCompleted = FALSE;
       bReArm = TRUE;
    }
    else
    {
        VMREST_LOG_ERROR(pRESTHandle, "SSL handshake failed...connection will be closed for socket with fd %d", pSocket->fd);
        dwError = VMREST_TRANSPORT_SSL_ACCEPT_FAILED;
        BAIL_ON_VMREST_ERROR(dwError);
    }

     if (bReArm && bWatched)
     {
         dwError = VmSockPosixReArmTimer(
                       pRESTHandle,
                       pSocket->pTimerSocket,
                       ((pRESTHandle->pRESTConfig->connTimeoutSec) * 1000)
                       );
         BAIL_ON_VMREST_ERROR(dwError);

         event.data.ptr = pSocket;
         event.events = EPOLLIN;

         event.events = event.events | EPOLLONESHOT;

         if (epoll_ctl(pRESTHandle->pSockContext->pEventQueue->epollFd, EPOLL_CTL_MOD, pSocket->fd, &event) < 0)
         {
             dwError = VM_SOCK_POSIX_ERROR_SYS_CALL_FAILED;
             BAIL_ON_VMREST_ERROR(dwError);
         }
     }
  
cleanup:

    return dwError;

error:

    if (bWatched && pRESTHandle && pRESTHandle->pSockContext)
    {
        /**** Delete from poller ****/
        VmSockPosixEventQueueDelete_inlock(
            pRESTHandle->pSockContext->pEventQueue,
            pSocket
            );
    }

    goto cleanup;

}

static
uint32_t
VmRESTCreateSSLObject(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    SSL*                             pSSL = NULL;

    if (!pSocket || !pRESTHandle || !pRESTHandle->pSSLInfo)
    {
        VMREST_LOG_ERROR(pRESTHandle,"%s","Invalid params");
        dwError = ERROR_INVALID_PARAMETER;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    pSSL = SSL_new(pRESTHandle->pSSLInfo->sslContext);
    if (!pSSL)
    {
        VMREST_LOG_ERROR(pRESTHandle, "SSL Context creation failed for socket fd %d", pSocket->fd);
        dwError = VMREST_TRANSPORT_SSL_ERROR;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    if (!(SSL_set_fd(pSSL, pSocket->fd)))
    {
        VMREST_LOG_ERROR(pRESTHandle, "Associating SSL CTX with raw socket fd %d failed ...", pSocket->fd);
        dwError = VMREST_TRANSPORT_SSL_ERROR;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    pSocket->ssl = pSSL;
    pSocket->bSSLHandShakeCompleted = FALSE;

cleanup:

    return dwError;

error:

    if (pSSL)
    {
        SSL_free(pSSL);
        pSSL = NULL;
    }

    if (pSocket)
    {
        pSocket->ssl = NULL;
    }

    goto cleanup;

}
