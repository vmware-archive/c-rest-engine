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

DWORD
VmSockPosixOpenServer(
    PVMREST_HANDLE                   pRESTHandle,
    USHORT                           usPort,
    int                              nWorkerThread,
    VM_SOCK_CREATE_FLAGS             dwFlags,
    PVM_SOCKET*                      ppSocket,
    char*                            sslCert,
    char*                            sslKey
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

    if (!pRESTHandle)
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
    if(dwFlags & VM_SOCK_IS_SSL)
    {
        if (sslCert == NULL && sslKey == NULL)
        {
            /**** Use application provided SSL Context ****/
            pRESTHandle->pSSLInfo->sslContext = pRESTHandle->pRESTConfig->pSSLContext;
        }
        else
        {
            SSL_library_init();
            dwError = VmRESTSecureSocket(
                          pRESTHandle,
                          sslCert,
                          sslKey
                          );
            BAIL_ON_VMREST_ERROR(dwError);
            pthread_mutex_lock(&gGlobalMutex);
            if (gSSLisedInstaceCount == INVALID)
            {
                gSSLisedInstaceCount = 0;
                dwError = VmRESTSSLThreadLockInit();
            }
            gSSLisedInstaceCount++;
            pthread_mutex_unlock(&gGlobalMutex);
            BAIL_ON_VMREST_ERROR(dwError);
        }
        pSSLInfo->isSecure = 1;
    }
    else
    {
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
        servaddr.servaddr_ipv6.sin6_port = htons(usPort);

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
        servaddr.servaddr_ipv4.sin_port = htons(usPort);

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

    if (nWorkerThread <= 0)
    {
        nWorkerThread = VM_SOCK_POSIX_DEFAULT_WORKER_THR_COUNT;
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
    pSocket->wThrCnt = nWorkerThread;
    pSocket->pTimerSocket = NULL;
    pSocket->pIoSocket = NULL;

    *ppSocket = pSocket;

cleanup:

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
    int                              iEventQueueSize,
    PVM_SOCK_EVENT_QUEUE*            ppQueue
    )
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;
    PVM_SOCK_EVENT_QUEUE             pQueue = NULL;

    if (!ppQueue || !pRESTHandle)
    {
        VMREST_LOG_ERROR(pRESTHandle,"Invalid params");
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMREST_ERROR(dwError);
    }

    if (iEventQueueSize <= 0)
    {
        iEventQueueSize = VM_SOCK_POSIX_DEFAULT_QUEUE_SIZE;
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

    pQueue->epollFd = epoll_create1(0);  //pQueue->dwSize);
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

    dwError = VmSockPosixEventQueueAdd_inlock(
                  pQueue,
                  FALSE,
                  pQueue->pSignalReader
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    *ppQueue = pQueue;
    pRESTHandle->pSSLInfo->isQueueInUse = 1;

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

    if (pSocket->type == VM_SOCK_TYPE_LISTENER)
    {
        pQueue->thrCnt = pSocket->wThrCnt;
    }

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
    BOOLEAN                          bDestroyGlobalMutex = FALSE;
    VM_SOCK_EVENT_TYPE               eventType = VM_SOCK_EVENT_TYPE_UNKNOWN;
    PVM_SOCKET                       pSocket = NULL;
    SSL*                             ssl = NULL;
    uint32_t                         cntRty = 0;
    BOOLEAN                          bFreeEventQueue = 0;
    int                              maxTry = 1000;
    uint32_t                         timerMs = 1;
    int                              timeOutSec = 5;

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
                VMREST_LOG_ERROR(pRESTHandle,"Bad socket information");
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
                    ssl = SSL_new(pRESTHandle->pSSLInfo->sslContext);
                    SSL_set_fd(ssl,pSocket->fd);
retry:
                    if ( SSL_accept(ssl) == -1 )
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
                            goto retry;
                        }
                        else
                        {
                            VMREST_LOG_ERROR(pRESTHandle,"SSL accept failed");
                            SSL_shutdown(ssl);
                            SSL_free(ssl);
                            close(pSocket->fd);
                            dwError = VMREST_TRANSPORT_SSL_ACCEPT_FAILED;
                            BAIL_ON_VMREST_ERROR(dwError);
                        }
                    }
                    pSocket->ssl = ssl;    
                }
                else
                {
                    pSocket->ssl = NULL;
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
                    pSocket = pEventSocket->pIoSocket;
                    eventType = VM_SOCK_EVENT_TYPE_CONNECTION_TIMEOUT;
                    /**** Delete timer socket from poller ****/

                    dwError = VmSockPosixEventQueueDelete_inlock(
                                  pQueue,
                                  pEventSocket
                                  );
                    BAIL_ON_VMREST_ERROR(dwError);
                    if (pEventSocket->fd > 0)
                    {
                        close(pEventSocket->fd);
                    }
                }
            }
            else
            {
                /**** Data is available over the socket ****/
                 pSocket = pEventSocket;
                 eventType = VM_SOCK_EVENT_TYPE_DATA_AVAILABLE;

                /**** stop timer on the socket ****/
                dwError = VmSockPosixReArmTimer(
                              pRESTHandle,
                              pSocket->pTimerSocket,
                              0
                              );
                BAIL_ON_VMREST_ERROR(dwError);
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

    /****  This needs to happen after we unlock mutex ****/
    if (dwError == ERROR_SHUTDOWN_IN_PROGRESS && bFreeEventQueue)
    {
        VmSockPosixFreeEventQueue(pQueue);
        pRESTHandle->pSSLInfo->isQueueInUse = 0;

        /**** Free SSL context only when it is allocated and managed by library ****/
        if((pRESTHandle->pSSLInfo->isSecure == 1) && ((pRESTHandle->pSSLInfo->isCertSet == SSL_INFO_FROM_CONFIG_FILE) || (pRESTHandle->pSSLInfo->isCertSet == SSL_INFO_FROM_BUFFER_API)))
        {
            if (pRESTHandle->pSSLInfo->sslContext)
            {
                VmRESTFreeMemory(pRESTHandle->pSSLInfo->sslContext);
                pRESTHandle->pSSLInfo->sslContext = NULL;
            }
            pthread_mutex_lock(&gGlobalMutex);
            gSSLisedInstaceCount--;
            if (gSSLisedInstaceCount == 0)
            {
                VmRESTSSLThreadLockShutdown();
                gSSLThreadLock = NULL;
                bDestroyGlobalMutex = TRUE;
                gSSLisedInstaceCount = INVALID;
            }
            pthread_mutex_unlock(&gGlobalMutex);
            if (bDestroyGlobalMutex)
            {
                pthread_mutex_destroy(&gGlobalMutex);
            }
        }
        else
        {
            /**** Don't free context as it was passed by application ****/
            pRESTHandle->pSSLInfo->sslContext = NULL;
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
        VmSockPosixReleaseSocket(pRESTHandle,pSocket);
    }

    goto cleanup;
}

VOID
VmSockPosixCloseEventQueue(
    PVMREST_HANDLE                  pRESTHandle,
    PVM_SOCK_EVENT_QUEUE             pQueue
    )
{
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

    while(retry < 10)
    {
        if (pRESTHandle->pSSLInfo->isQueueInUse == 0)
        {
           break;
        }
        sleep(1);
        retry++;
    }
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
        VMREST_LOG_ERROR(pRESTHandle,"Invalid params");
        dwError = ERROR_INVALID_PARAMETER;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTLockMutex(pSocket->pMutex);
    BAIL_ON_VMREST_ERROR(dwError);

    bLocked = TRUE;
    if (pSocket->pszBuffer)
    {
        nPrevBuf = pSocket->nBufData - pSocket->nProcessed;
        /**** copy unprocessed data in local buffer ****/

        if (nPrevBuf > 0)
        {
            dwError = VmRESTAllocateMemory(
                          nPrevBuf,
                          (void **)&pszBufPrev
                          );
            BAIL_ON_VMREST_ERROR(dwError);

            memcpy(pszBufPrev, pSocket->pszBuffer, nPrevBuf);
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
    }while(nRead > 0);

    if (errorCode == EAGAIN || errorCode == EWOULDBLOCK)
    {
        dwError = REST_ENGINE_SUCCESS;
    }
    else
    {
        dwError = errorCode;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    pSocket->pszBuffer = pszBufPrev;
    pSocket->nProcessed = 0;
    pSocket->nBufData = nPrevBuf;
    
    *ppszBuffer = pSocket->pszBuffer;
    *nBufLen = pSocket->nBufData;

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
         if (nWritten >= 0)
         {
             nWrittenTotal += nWritten;
             nRemaining -= nWritten;
             VMREST_LOG_DEBUG(pRESTHandle,"\nBytes written this write %d, Total bytes written %u", nWritten, nWrittenTotal);
             nWritten = 0;
         }
         else
         {
             if (errorCode == EAGAIN || errorCode == EWOULDBLOCK)
             {
                 VMREST_LOG_DEBUG(pRESTHandle,"retry write");
                 usleep(1000);
                 nWritten = 0;
                 continue;
             }
             VMREST_LOG_ERROR(pRESTHandle,"Write failed with errorno %u", errorCode);
             dwError = VM_SOCK_POSIX_ERROR_SYS_CALL_FAILED;
             BAIL_ON_VMREST_ERROR(dwError);
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

    if (!pRESTHandle)
    {
        VMREST_LOG_ERROR(pRESTHandle,"Invalid REST Handler");
        dwError = REST_ERROR_INVALID_HANDLER;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTLockMutex(pSocket->pMutex);
    BAIL_ON_VMREST_ERROR(dwError);

    bLocked = TRUE;

    /**** Delete the socket fd explicitely from the poller ****/
    dwError = VmSockPosixEventQueueDelete_inlock(
                  pRESTHandle->pSockContext->pEventQueue,
                  pSocket
                  );
    BAIL_ON_VMREST_ERROR(dwError); 

    if (pRESTHandle->pSSLInfo->isSecure)
    {
        if (pSocket->ssl)
        {
            SSL_shutdown(pSocket->ssl);
            SSL_free(pSocket->ssl);
            pSocket->ssl = NULL;
        }
    }
    else
    {
        if (pSocket->fd >= 0)
        {
            close(pSocket->fd);
            pSocket->fd = -1;
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

    *ppReaderSocket = NULL;
    *ppWriterSocket = NULL;

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

    event.data.ptr = pSocket;
    event.events = EPOLLIN;

    if (bOneShot)
    {
       event.events = event.events | EPOLLET;
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
    pSocket->pRequest = NULL;
    pSocket->pszBuffer = NULL;
    pSocket->pTimerSocket = NULL;
    pSocket->pIoSocket = NULL;

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
    if (pSocket->fd >= 0)
    {
        close(pSocket->fd);
    }
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

    if ((pSocket->nProcessed == nProcessed) && (pSocket->pszBuffer != NULL))
    {
        VmRESTFreeMemory(pSocket->pszBuffer);
        pSocket->pszBuffer = NULL;
    }
    else
    {
        pSocket->nProcessed = nProcessed;
    }

    dwError = VmSockPosixReArmTimer(
                  pRESTHandle,
                  pSocket->pTimerSocket,
                  10000
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    /**** Add back to poller ****/
    if (!bCompleted)
    {
        event.data.ptr = pSocket;
        event.events = EPOLLIN;

        event.events = event.events | EPOLLET;
        event.events = event.events | EPOLLONESHOT;

        if (epoll_ctl(pQueue->epollFd, EPOLL_CTL_MOD, pSocket->fd, &event) < 0)
        {
            dwError = VM_SOCK_POSIX_ERROR_SYS_CALL_FAILED;
            BAIL_ON_VMREST_ERROR(dwError);
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
    size_t                           nRead = 0;
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

    if ((errorCode == EAGAIN || errorCode == EWOULDBLOCK))
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
            }while(errorCode != EAGAIN || errorCode != EWOULDBLOCK);

            if (!bCloseConn)
            {
                /**** Add timer socket back to poller ****/
                event.data.ptr = pTimerSocket;
                event.events = EPOLLIN;
                event.events = event.events | EPOLLET;
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
    int                              mSec = 10000;  //Timer fires after 10000 milliSecond
    PVM_SOCKET                       pTimerSocket = NULL;

    timerFd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
    if (timerFd == INVALID) 
    {
        VMREST_LOG_ERROR(pRESTHandle,"%s","Timer Creation failed");
        dwError = VM_SOCK_POSIX_ERROR_SYS_CALL_FAILED;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmSockPosixSetDescriptorNonBlocking(timerFd);
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTAllocateMemory(
                  sizeof(*pSocket),
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
                  mSec
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

    goto cleanup;
    
}
