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
void
VmRESTSSLThreadLockCallback(
    int                              mode,
    int                              type,
    char*                            file,
    int                              line
    );

static
unsigned long
VmRESTSSLThreadId(
    void
    );

static
uint32_t
VmRESTSSLThreadLockInit(
    void
    );

static
void
VmRESTSSLThreadLockShutdown(
    void
    );

static
uint32_t
VmRESTSecureSocket(
    PVMREST_HANDLE                   pRESTHandle,
    char*                            certificate,
    char*                            key
    );

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
void
VmRESTSSLThreadLockCallback(
    int                              mode,
    int                              type,
    char*                            file,
    int                              line
    )
{
    (void)line;
    (void)file;
    if(mode & CRYPTO_LOCK)
    {
        pthread_mutex_lock(&(gSSLThreadLock[type]));
    }
    else
    {
        pthread_mutex_unlock(&(gSSLThreadLock[type]));
    }
}

static
unsigned long
VmRESTSSLThreadId(
    void
    )
{
    unsigned long                      ret = 0;

    ret = (unsigned long)pthread_self();
    return ret;
}

static
uint32_t
VmRESTSSLThreadLockInit(
    void
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    int                              i = 0;

    gSSLThreadLock = (pthread_mutex_t *)OPENSSL_malloc(
                                           CRYPTO_num_locks() * sizeof(pthread_mutex_t)
                                           );
    if (gSSLThreadLock == NULL)
    {
        dwError = REST_ERROR_NO_MEMORY;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    for(i = 0; i < CRYPTO_num_locks(); i++) 
    {
        pthread_mutex_init(&(gSSLThreadLock[i]), NULL);
    }

    CRYPTO_set_id_callback((unsigned long (*)())VmRESTSSLThreadId);
    CRYPTO_set_locking_callback((void (*)())VmRESTSSLThreadLockCallback);

cleanup:
    return dwError;
error:
    dwError = VMREST_TRANSPORT_SSL_ERROR;
    goto cleanup;
}

static
void
VmRESTSSLThreadLockShutdown(
    void
    )
{
    int                              i = 0;

    CRYPTO_set_locking_callback(NULL);

    for( i = 0; i < CRYPTO_num_locks(); i++)
    {
        pthread_mutex_destroy(&(gSSLThreadLock[i]));
    }
    OPENSSL_free(gSSLThreadLock);
}

static
uint32_t
VmRESTSecureSocket(
    PVMREST_HANDLE                   pRESTHandle,
    char*                            certificate,
    char*                            key
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    int                              ret = 0;
    long                             options = 0;
    const SSL_METHOD*                method = NULL;
    SSL_CTX*                         context = NULL;

    if (key == NULL || certificate == NULL)
    {
        VMREST_LOG_ERROR(pRESTHandle,"Invalid params");
        dwError = VMREST_TRANSPORT_INVALID_PARAM;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    method = SSLv23_server_method();
    context = SSL_CTX_new(method);
    if ( context == NULL )
    {
        VMREST_LOG_ERROR(pRESTHandle,"SSL context is NULL");
        dwError = VMREST_TRANSPORT_SSL_CONFIG_ERROR;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    options = SSL_CTX_get_options(context);

    options = options | SSL_OP_NO_TLSv1|SSL_OP_NO_SSLv3|SSL_OP_NO_SSLv2;

    options = SSL_CTX_set_options(context, options);

    ret = SSL_CTX_set_cipher_list(context, "!aNULL:kECDH+AESGCM:ECDH+AESGCM:RSA+AESGCM:kECDH+AES:ECDH+AES:RSA+AES");
    if (ret == 0)
    {
        VMREST_LOG_ERROR(pRESTHandle,"SSL_CTX_set_cipher_list() : Cannot apply security approved cipher suites");
        dwError = VMREST_TRANSPORT_SSL_INVALID_CIPHER_SUITES;
    }
    BAIL_ON_VMREST_ERROR(dwError);
 
    ret = SSL_CTX_use_certificate_file(context, certificate, SSL_FILETYPE_PEM);
    if (ret <= 0)
    {
        VMREST_LOG_ERROR(pRESTHandle,"Cannot Use SSL certificate");
        dwError = VMREST_TRANSPORT_SSL_CERTIFICATE_ERROR;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    ret = SSL_CTX_use_PrivateKey_file(context, key, SSL_FILETYPE_PEM);
    if (ret <= 0)
    {
        VMREST_LOG_ERROR(pRESTHandle,"Cannot use private key file");
        dwError = VMREST_TRANSPORT_SSL_PRIVATEKEY_ERROR;
        BAIL_ON_VMREST_ERROR(dwError);
    }
    if (!SSL_CTX_check_private_key(context))
    {
        VMREST_LOG_ERROR(pRESTHandle,"Error in Private Key");
        dwError = VMREST_TRANSPORT_SSL_PRIVATEKEY_CHECK_ERROR;
        BAIL_ON_VMREST_ERROR(dwError);
    }

    pRESTHandle->pSSLInfo->sslContext = context;

cleanup:
    return dwError;

error:
     dwError = VMREST_TRANSPORT_SSL_ERROR;
    goto cleanup;
}

DWORD
VmSockPosixOpenServer(
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

    if (!pRESTHandle)
    {
        VMREST_LOG_DEBUG(pRESTHandle,"Invalid REST Handler");
        dwError = REST_ERROR_INVALID_HANDLER;
    }
    BAIL_ON_VMREST_ERROR(dwError);


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

    if (dwFlags & VM_SOCK_CREATE_FLAGS_IPV6)
    {
#ifdef AF_INET6
        socketParams.domain = AF_INET6;
#else
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_POSIX_SOCK_ERROR(dwError);
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
        if (gSSLisedInstaceCount == INVALID)
        {
            pthread_mutex_init(&gGlobalMutex, NULL);
            gSSLisedInstaceCount = 0;
        }
        SSL_library_init();
        dwError = VmRESTSecureSocket(
                      pRESTHandle,
                      sslCert,
                      sslKey
                      );
        BAIL_ON_POSIX_SOCK_ERROR(dwError);
        pSSLInfo->isSecure = 1;
        pthread_mutex_lock(&gGlobalMutex);
        if (gSSLisedInstaceCount == 0)
        {
            dwError = VmRESTSSLThreadLockInit();
            gSSLisedInstaceCount++;
        }
        pthread_mutex_unlock(&gGlobalMutex);
        BAIL_ON_VMREST_ERROR(dwError);
        
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
    BAIL_ON_POSIX_SOCK_ERROR(dwError);

    if (dwFlags & VM_SOCK_CREATE_FLAGS_REUSE_ADDR)
    {
        dwError = VmSockPosixSetReuseAddress(fd);
        BAIL_ON_POSIX_SOCK_ERROR(dwError);
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
            BAIL_ON_POSIX_SOCK_ERROR(dwError);
        }
#endif

#else
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_POSIX_SOCK_ERROR(dwError);
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
    BAIL_ON_POSIX_SOCK_ERROR(dwError);

    if (dwFlags & VM_SOCK_CREATE_FLAGS_NON_BLOCK)
    {
        dwError = VmSockPosixSetDescriptorNonBlocking(fd);
        BAIL_ON_POSIX_SOCK_ERROR(dwError);
    }

    if (!(dwFlags & VM_SOCK_CREATE_FLAGS_UDP))
    {
        if (iListenQueueSize <= 0)
        {
            iListenQueueSize = VM_SOCK_POSIX_DEFAULT_LISTEN_QUEUE_SIZE;
        }

        if (listen(fd, iListenQueueSize) < 0)
        {
            VMREST_LOG_ERROR(pRESTHandle,"Listen() on server socket with fd %d failed with Error code %d", fd, errno);
            dwError = VM_SOCK_POSIX_ERROR_SYS_CALL_FAILED;
        }
        BAIL_ON_POSIX_SOCK_ERROR(dwError);
    }
    VMREST_LOG_DEBUG(pRESTHandle,"Server Listening on socket with fd %d ...", fd);

    dwError = VmRESTAllocateMemory(
                  sizeof(*pSocket),
                  (PVOID*)&pSocket
                  );
    BAIL_ON_POSIX_SOCK_ERROR(dwError);

    pSocket->refCount = 1;

    dwError = VmRESTAllocateMutex(&pSocket->pMutex);
    BAIL_ON_POSIX_SOCK_ERROR(dwError);

    pSocket->type = VM_SOCK_TYPE_LISTENER;

    if (dwFlags & VM_SOCK_CREATE_FLAGS_UDP)
    {
        pSocket->protocol = VM_SOCK_PROTOCOL_UDP;
    }
    else
    {
        pSocket->protocol = VM_SOCK_PROTOCOL_TCP;
    }

    pSocket->fd = fd;
    pSocket->pStreamBuffer = NULL;
    pSocket->ssl = NULL;
    pSocket->wThrCnt = iListenQueueSize;

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
        BAIL_ON_POSIX_SOCK_ERROR(dwError);
    }

    if (iEventQueueSize <= 0)
    {
        iEventQueueSize = VM_SOCK_POSIX_DEFAULT_QUEUE_SIZE;
    }

    dwError = VmRESTAllocateMemory(
                  sizeof(*pQueue),
                  (PVOID*)&pQueue
                  );
    BAIL_ON_POSIX_SOCK_ERROR(dwError);

    dwError = VmSockPosixCreateSignalSockets(
                  &pQueue->pSignalReader,
                  &pQueue->pSignalWriter
                  );
    BAIL_ON_POSIX_SOCK_ERROR(dwError);

    dwError = VmRESTAllocateMutex(&pQueue->pMutex);
    BAIL_ON_POSIX_SOCK_ERROR(dwError);

    dwError = VmRESTAllocateMemory(
                  iEventQueueSize * sizeof(*pQueue->pEventArray),
                  (PVOID*)&pQueue->pEventArray
                  );
    BAIL_ON_POSIX_SOCK_ERROR(dwError);

    pQueue->dwSize = iEventQueueSize;

    pQueue->epollFd = epoll_create(pQueue->dwSize);
    if (pQueue->epollFd < 0)
    {
        VMREST_LOG_ERROR(pRESTHandle,"epoll create failed with Error code %d", errno);
        dwError = VM_SOCK_POSIX_ERROR_SYS_CALL_FAILED;
        BAIL_ON_POSIX_SOCK_ERROR(dwError);
    }

    pQueue->state  = VM_SOCK_POSIX_EVENT_STATE_WAIT;
    pQueue->nReady = -1;
    pQueue->iReady = 0;
    pQueue->bShutdown = 0;

    dwError = VmSockPosixEventQueueAdd_inlock(
                  pQueue,
                  pQueue->pSignalReader
                  );
    BAIL_ON_POSIX_SOCK_ERROR(dwError);

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
        BAIL_ON_POSIX_SOCK_ERROR(dwError);
    }

    dwError = VmRESTLockMutex(pQueue->pMutex);
    BAIL_ON_POSIX_SOCK_ERROR(dwError);

    bLocked = TRUE;

    if (pSocket->type == VM_SOCK_TYPE_LISTENER && pSocket->protocol == VM_SOCK_PROTOCOL_TCP)
    {
        pQueue->thrCnt = pSocket->wThrCnt;
    }

    dwError = VmSockPosixEventQueueAdd_inlock(
                  pQueue,
                  pSocket
                  );
    BAIL_ON_POSIX_SOCK_ERROR(dwError);

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
    PVM_SOCK_EVENT_TYPE              pEventType,
    PVM_SOCK_IO_BUFFER*              ppIoBuffer
    )
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;
    BOOLEAN                          bLocked = FALSE;
    BOOLEAN                          destroyGlobalMutex = FALSE;
    VM_SOCK_EVENT_TYPE               eventType = VM_SOCK_EVENT_TYPE_UNKNOWN;
    PVM_SOCKET                       pSocket = NULL;
    SSL*                             ssl = NULL;
    uint32_t                         cntRty = 0;
    uint32_t                         freeEventQueue = 0;
    int                              maxTry = 1000;
    uint32_t                         timerMs = 1;
    int                              timeOutSec = 5;

    if (!pQueue || !ppSocket || !pEventType)
    {
        VMREST_LOG_ERROR(pRESTHandle,"Invalid params");
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_POSIX_SOCK_ERROR(dwError);
    }

    dwError = VmRESTLockMutex(pQueue->pMutex);
    BAIL_ON_POSIX_SOCK_ERROR(dwError);

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
                BAIL_ON_POSIX_SOCK_ERROR(dwError);
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
                BAIL_ON_POSIX_SOCK_ERROR(dwError);
            }

            if (pEvent->events & (EPOLLERR | EPOLLHUP))
            {
                eventType = VM_SOCK_EVENT_TYPE_CONNECTION_CLOSED;

                pSocket = pEventSocket;
            }
            else if (pEventSocket->type == VM_SOCK_TYPE_LISTENER)
            {
                switch (pEventSocket->protocol)
                {
                    case VM_SOCK_PROTOCOL_TCP:

                        dwError = VmSockPosixAcceptConnection(
                                      pEventSocket,
                                      &pSocket);
                        BAIL_ON_POSIX_SOCK_ERROR(dwError);
                        pSocket->inUse = 0;

                        dwError = VmSockPosixSetNonBlocking(pRESTHandle,pSocket);
                        BAIL_ON_POSIX_SOCK_ERROR(dwError);

                        

                        /**** If conn is over SSL, do the needful ****/
                        if (pRESTHandle->pSSLInfo->isSecure)
                        {
                             ssl = SSL_new(pRESTHandle->pSSLInfo->sslContext);
                             SSL_set_fd(ssl,pSocket->fd);
retry:
                             if ((SSL_accept(ssl) == -1) && (timeOutSec >= 0))
                             {
                                 if (timeOutSec >= 0)
                                 {
                                 cntRty++;
#ifdef WIN32
                                 Sleep(timerMs);
#else
                                 usleep((timerMs * 1000));
#endif
                                 if (cntRty >= maxTry)
                                 {
                                     timerMs = ((timerMs >= 1000) ? 1000 : (timerMs*10));
                                     maxTry = ((maxTry <= 1) ? 1 : (maxTry/10));
                                     timeOutSec--;
                                     cntRty = 0;
                                 }
                                 goto retry;
                                 }
 
                                 else if(timeOutSec <= 0)
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
                        /**** We dont need to add accepted FD to poller ****/
                        eventType = VM_SOCK_EVENT_TYPE_DATA_AVAILABLE;

                        break;

                    default:

                        dwError = ERROR_INVALID_STATE;
                        BAIL_ON_POSIX_SOCK_ERROR(dwError);
                }
            }
            else if (pEventSocket->type == VM_SOCK_TYPE_SIGNAL)
            {
                if (pQueue->bShutdown)
                {
                    pQueue->thrCnt--;
                    if (pQueue->thrCnt == 0)
                    {
                        freeEventQueue = 1;
                    }

                    dwError = ERROR_SHUTDOWN_IN_PROGRESS;
                    BAIL_ON_POSIX_SOCK_ERROR(dwError);
                }
                else
                {
                    pSocket = VmSockPosixAcquireSocket(pRESTHandle, pEventSocket);
                    eventType = VM_SOCK_EVENT_TYPE_DATA_AVAILABLE;
                }
            }
            else
            {
                /**** Do nothing ****/
            }
        }
        pQueue->iReady++;
    }

    *ppSocket = pSocket;
    *pEventType = eventType;
    if(pSocket)
    {
        *ppIoBuffer = (PVM_SOCK_IO_BUFFER)pSocket->pData;
    }

cleanup:

    if (pQueue && bLocked)
    {
        VmRESTUnlockMutex(pQueue->pMutex);
    }

    if (ppIoBuffer)
    {
        *ppIoBuffer = NULL;
    }

    // This needs to happen after we unlock mutex
    if (dwError == ERROR_SHUTDOWN_IN_PROGRESS && freeEventQueue == 1)
    {
        VmSockPosixFreeEventQueue(pQueue);
        pRESTHandle->pSSLInfo->isQueueInUse = 0;

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
            destroyGlobalMutex = TRUE;
            gSSLisedInstaceCount = INVALID;
        }
        pthread_mutex_unlock(&gGlobalMutex);
        if (destroyGlobalMutex)
        {
            pthread_mutex_destroy(&gGlobalMutex);
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
    BAIL_ON_POSIX_SOCK_ERROR(dwError);

    bLocked = TRUE;

    dwError = VmSockPosixSetDescriptorNonBlocking(pSocket->fd);
    BAIL_ON_POSIX_SOCK_ERROR(dwError);

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
    PVM_SOCK_IO_BUFFER               pIoBuffer
    )
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;
    BOOLEAN                          bLocked = FALSE;
    int                              flags   = 0;
    ssize_t                          nRead   = 0;
    DWORD                            dwBufSize = 0;
    uint32_t                         tryCnt = 0;
    int                              maxTry = 1000;
    uint32_t                         timerMs = 1;
    int                              timeOutSec = 5;
    uint32_t                         errorCode = 0;

    if (!pSocket || !pIoBuffer || !pIoBuffer->pData)
    {
        VMREST_LOG_ERROR(pRESTHandle,"Invalid params");
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_POSIX_SOCK_ERROR(dwError);
    }

    if (pIoBuffer->dwExpectedSize < pIoBuffer->dwCurrentSize)
    {
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_POSIX_SOCK_ERROR(dwError);
    }

    dwBufSize = pIoBuffer->dwExpectedSize - pIoBuffer->dwCurrentSize;
    pIoBuffer->addrLen = sizeof pIoBuffer->clientAddr;

    dwError = VmRESTLockMutex(pSocket->pMutex);
    BAIL_ON_POSIX_SOCK_ERROR(dwError);

    bLocked = TRUE;

tryAgain:
    if (pRESTHandle->pSSLInfo->isSecure && (pSocket->ssl != NULL))
    {
        nRead = SSL_read(pSocket->ssl, pIoBuffer->pData + pIoBuffer->dwCurrentSize, dwBufSize);
        errorCode = SSL_get_error(pSocket->ssl, nRead);
    }
    else if (pSocket->fd > 0)
    {
        nRead = recvfrom(
                pSocket->fd,
                pIoBuffer->pData + pIoBuffer->dwCurrentSize,
                dwBufSize,
                flags,
                (struct sockaddr*)&pIoBuffer->clientAddr,
                &pIoBuffer->addrLen);
        errorCode = errno;
    }

    /**** Make server wait upto 5 seconds before connection timeout ****/

    if (nRead < 0 && (timeOutSec >= 0)  && (errorCode == EAGAIN || errorCode == SSL_ERROR_WANT_READ))
    {
        tryCnt++;
#ifdef WIN32
        Sleep(timerMs);
#else
        usleep((timerMs * 1000));
#endif
        if (tryCnt >= maxTry)
        {
            timerMs = ((timerMs >= 1000) ? 1000 : (timerMs*10));
            maxTry = ((maxTry <= 1) ? 1 : (maxTry/10));
            timeOutSec--;
            tryCnt = 0;
        }
        goto tryAgain;
    }
    else if (nRead < 0)
    {
        VMREST_LOG_ERROR(pRESTHandle,"Socket Read failed");
        dwError = VM_SOCK_POSIX_ERROR_SYS_CALL_FAILED;
        BAIL_ON_POSIX_SOCK_ERROR(dwError);
    }

    pIoBuffer->dwCurrentSize += nRead;
    pIoBuffer->dwBytesTransferred = nRead;

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
    const struct sockaddr*           pClientAddress,
    socklen_t                        addrLength,
    PVM_SOCK_IO_BUFFER               pIoBuffer
    )
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;
    BOOLEAN                          bLocked  = FALSE;
    int                              flags    = MSG_NOSIGNAL;
    ssize_t                          nWritten = 0;
    DWORD                            dwBytesToWrite = 0;
    const struct sockaddr*           pClientAddressLocal = NULL;
    socklen_t                        addrLengthLocal = 0;
    uint32_t                         bytes = 0;
    uint32_t                         bytesLeft = 0;
    uint32_t                         bytesWritten = 0;

    if (!pSocket || !pIoBuffer || !pIoBuffer->pData)
    {
        VMREST_LOG_ERROR(pRESTHandle,"Invalid params");
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_POSIX_SOCK_ERROR(dwError);
    }

    dwBytesToWrite = pIoBuffer->dwExpectedSize;

    bytes = dwBytesToWrite;
    bytesLeft = bytes;

    switch (pSocket->protocol)
    {
        case VM_SOCK_PROTOCOL_TCP:

            pClientAddressLocal = &pSocket->addr;
            addrLengthLocal     = pSocket->addrLen;

            break;

        case VM_SOCK_PROTOCOL_UDP:

            if (!pClientAddress || addrLength <= 0)
            {
                dwError = ERROR_INVALID_PARAMETER;
                BAIL_ON_VMREST_ERROR(dwError);
            }

            memcpy(
                &pIoBuffer->clientAddr,
                pClientAddress,
                addrLength);

            pClientAddressLocal = pClientAddress;
            addrLengthLocal = addrLength;

            break;

        default:

            dwError = ERROR_NOT_SUPPORTED;
            BAIL_ON_POSIX_SOCK_ERROR(dwError);
    }

    dwError = VmRESTLockMutex(pSocket->pMutex);
    BAIL_ON_POSIX_SOCK_ERROR(dwError);

    bLocked = TRUE;

    while(bytesWritten < bytes )
    {
         if (pRESTHandle->pSSLInfo->isSecure && (pSocket->ssl != NULL))
         {
             nWritten = SSL_write(pSocket->ssl,(pIoBuffer->pData + bytesWritten),bytesLeft);
         }
         else if (pSocket->fd > 0)
         {
             nWritten = sendto(
                        pSocket->fd,
                        (pIoBuffer->pData + bytesWritten),
                        bytesLeft,
                        flags,
                        pClientAddressLocal,
                        addrLengthLocal);
         }
         if (nWritten >= 0)
         {
             bytesWritten += nWritten;
             bytesLeft -= nWritten;
             VMREST_LOG_DEBUG(pRESTHandle,"\nBytes written this write %d, Total bytes written %u", nWritten, bytesWritten);
             nWritten = 0;
         }
         else
         {
             if (errno == 11)
             {
                 VMREST_LOG_DEBUG(pRESTHandle,"retry write");
                 usleep(1000);
                 nWritten = 0;
                 continue;
             }
             VMREST_LOG_ERROR(pRESTHandle,"Write failed with errorno %d", errno);
             dwError = VM_SOCK_POSIX_ERROR_SYS_CALL_FAILED;
             BAIL_ON_VMREST_ERROR(dwError);
        }

    }
    VMREST_LOG_DEBUG(pRESTHandle,"\nWrite Status on Socket with fd = %d\nRequested: %d bytes\nWritten %d bytes\n", pSocket->fd, bytes, bytesWritten);

    pIoBuffer->dwCurrentSize += nWritten;
    pIoBuffer->dwBytesTransferred = nWritten;

cleanup:

    if (bLocked)
    {
        VmRESTUnlockMutex(pSocket->pMutex);
    }

    return dwError;

error:

    goto cleanup;
}

PVM_SOCKET
VmSockPosixAcquireSocket(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket
    )
{
    if (pSocket)
    {
        pSocket->refCount++;
    }
    return pSocket;
}

VOID
VmSockPosixReleaseSocket(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket
    )
{
    if (pSocket)
    {
        if (--(pSocket->refCount) == 0)
        {
            VmSockPosixFreeSocket(pSocket);
        }
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
    BAIL_ON_POSIX_SOCK_ERROR(dwError);

    dwError = VmRESTLockMutex(pSocket->pMutex);
    BAIL_ON_POSIX_SOCK_ERROR(dwError);

    bLocked = TRUE;

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
        BAIL_ON_POSIX_SOCK_ERROR(dwError);
    }

    for (; iSock < sizeof(sockets)/sizeof(sockets[0]); iSock++)
    {
        PVM_SOCKET pSocket = NULL;

        dwError = VmRESTAllocateMemory(
                      sizeof(VM_SOCKET),
                      (PVOID*)sockets[iSock]
                      );
        BAIL_ON_POSIX_SOCK_ERROR(dwError);

        pSocket = *sockets[iSock];

        pSocket->refCount = 1;

        dwError = VmRESTAllocateMutex(&pSocket->pMutex);
        BAIL_ON_POSIX_SOCK_ERROR(dwError);

        pSocket->protocol = VM_SOCK_PROTOCOL_TCP;
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
    PVM_SOCKET                       pSocket
    )
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;
    struct                           epoll_event event = {0};

    event.data.ptr = pSocket;
    event.events = EPOLLIN;

    if (epoll_ctl(pQueue->epollFd, EPOLL_CTL_ADD, pSocket->fd, &event) < 0)
    {
        dwError = VM_SOCK_POSIX_ERROR_SYS_CALL_FAILED;
        BAIL_ON_POSIX_SOCK_ERROR(dwError);
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
    PVM_STREAM_BUFFER                pStrmBuf = NULL;

    dwError = VmRESTAllocateMemory(
                  sizeof(*pSocket),
                  (PVOID*)&pSocket
                  );
    BAIL_ON_POSIX_SOCK_ERROR(dwError);

    dwError = VmRESTAllocateMemory(
                  sizeof(*pStrmBuf),
                  (void**)&pStrmBuf
                  );
    BAIL_ON_POSIX_SOCK_ERROR(dwError);

    pStrmBuf->dataProcessed = 0;
    pStrmBuf->dataRead = 0;
    memset(pStrmBuf->pData, '\0', 4096);

    pSocket->pStreamBuffer = pStrmBuf;

    pSocket->refCount = 1;

    dwError = VmRESTAllocateMutex(&pSocket->pMutex);
    BAIL_ON_POSIX_SOCK_ERROR(dwError);

    pSocket->protocol = pListener->protocol;
    pSocket->type = VM_SOCK_TYPE_SERVER;

    fd = accept(pListener->fd, &pSocket->addr, &pSocket->addrLen);
    if (fd < 0)
    {
        dwError = VM_SOCK_POSIX_ERROR_SYS_CALL_FAILED;
        BAIL_ON_POSIX_SOCK_ERROR(dwError);
    }

    pSocket->fd = fd;

    pSocket->pAddr = &pSocket->addr;

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
        BAIL_ON_POSIX_SOCK_ERROR(dwError);
    }

    flags |= O_NONBLOCK;

    if ((flags = fcntl(fd, F_SETFL, flags)) < 0)
    {
        dwError = VM_SOCK_POSIX_ERROR_SYS_CALL_FAILED;
        BAIL_ON_POSIX_SOCK_ERROR(dwError);
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
        BAIL_ON_POSIX_SOCK_ERROR(dwError);
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
    if (pSocket->pStreamBuffer)
    {
        VmRESTFreeMemory(pSocket->pStreamBuffer);
    }
    VmRESTFreeMemory(pSocket);
}

DWORD
VmSockPosixAllocateIoBuffer(
    PVMREST_HANDLE                   pRESTHandle,
    VM_SOCK_EVENT_TYPE               eventType,
    DWORD                            dwSize,
    PVM_SOCK_IO_BUFFER*              ppIoBuffer
    )
{
    DWORD                            dwError = REST_ENGINE_SUCCESS;
    PVM_SOCK_IO_CONTEXT              pIoContext = NULL;

    if (!ppIoBuffer)
    {
        VMREST_LOG_ERROR(pRESTHandle,"Invalid params");
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMREST_ERROR(dwError);
    }

    dwError = VmRESTAllocateMemory(
                    sizeof(VM_SOCK_IO_CONTEXT) + dwSize,
                    (PVOID*)&pIoContext);
    BAIL_ON_VMREST_ERROR(dwError);

    pIoContext->eventType = eventType;
    pIoContext->IoBuffer.dwExpectedSize = dwSize;
    pIoContext->IoBuffer.pData = pIoContext->DataBuffer;

    *ppIoBuffer = &pIoContext->IoBuffer;

cleanup:

    return dwError;

error:

    if (pIoContext)
    {
        VmSockPosixFreeIoBuffer(pRESTHandle,&pIoContext->IoBuffer);
    }

    goto cleanup;
}

VOID
VmSockPosixFreeIoBuffer(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCK_IO_BUFFER               pIoBuffer
    )
{
    PVM_SOCK_IO_CONTEXT              pIoContext = CONTAINING_RECORD(pIoBuffer, VM_SOCK_IO_CONTEXT, IoBuffer);
    
    if (pIoContext)
    {
        VmRESTFreeMemory(pIoContext);
        pIoContext = NULL;
    }
}

VOID
VmSockPosixGetStreamBuffer(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket,
    PVM_STREAM_BUFFER*               ppStreamBuffer
    )
{
    if (pSocket->pStreamBuffer)
    {
        *ppStreamBuffer = pSocket->pStreamBuffer;
    }
    else
    {
        *ppStreamBuffer = NULL;
    }
}

VOID
VmSockPosixSetStreamBuffer(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket,
    PVM_STREAM_BUFFER                pStreamBuffer
    )
{
    if (pStreamBuffer)
    {
        pSocket->pStreamBuffer = pStreamBuffer;
    }
    else
    {
        pSocket->pStreamBuffer = NULL;
    }
}
