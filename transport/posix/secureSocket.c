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

uint32_t
VmRESTSecureSocket(
    char*                            certificate,
    char*                            key
    )
{
    uint32_t                         dwError = ERROR_VMREST_SUCCESS;
    int                              ret = 0;
    const SSL_METHOD*                method = NULL;
    SSL_CTX*                         context = NULL;

    if (key == NULL || certificate == NULL)
    {
        VMREST_LOG_DEBUG("VmRESTSecureSocket(): Invalid params");
        dwError = VMREST_TRANSPORT_INVALID_PARAM;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    method = SSLv3_server_method();
    context = SSL_CTX_new(method);
    if ( context == NULL )
    {
        VMREST_LOG_DEBUG("VmRESTSecureSocket(): Context Null");
        dwError = VMREST_TRANSPORT_SSL_CONFIG_ERROR;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    ret = SSL_CTX_use_certificate_file(context, certificate, SSL_FILETYPE_PEM);
    if (ret <= 0)
    {
        VMREST_LOG_DEBUG("VmRESTSecureSocket(): Cannot Use SSL certificate");
        dwError = VMREST_TRANSPORT_SSL_CERTIFICATE_ERROR;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    ret = SSL_CTX_use_PrivateKey_file(context, key, SSL_FILETYPE_PEM);
    if (ret <= 0)
    {
        VMREST_LOG_DEBUG("VmRESTSecureSocket(): Cannot use private key file");
        dwError = VMREST_TRANSPORT_SSL_PRIVATEKEY_ERROR;
        BAIL_ON_VMREST_ERROR(dwError);
    }
    if (!SSL_CTX_check_private_key(context))
    {
        VMREST_LOG_DEBUG("VmRESTSecureSocket(): Error in Private Key");
        dwError = VMREST_TRANSPORT_SSL_PRIVATEKEY_CHECK_ERROR;
        BAIL_ON_VMREST_ERROR(dwError)
    }

    gServerSocketInfo.sslContext = context;

cleanup:
    return dwError;

error:
    dwError = VMREST_TRANSPORT_SSL_ERROR;
    goto cleanup;
}


uint32_t
VmSockPosixCreateServerSocket(
    char*                            sslCertificate,
    char*                            sslKey,
    char*                            port,
    uint32_t                         clientCount
    )
{
    uint32_t                dwError = ERROR_VMREST_SUCCESS;
    pthread_t*              thr = NULL;
    QUEUE*                  myQueue = NULL;
    PVM_SERVER_THR_PARAMS   thrParams = NULL;

    if ( sslCertificate == NULL || sslKey == NULL || port == NULL)
    {
        VMREST_LOG_DEBUG("VmSockPosixCreateServerSocket(): Invalid params");
        dwError = VMREST_TRANSPORT_INVALID_PARAM;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTAllocateMemory(
                  sizeof(VM_SERVER_THR_PARAMS),
                  (void *)&(thrParams)
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    strcpy(thrParams->sslCert, sslCertificate);
    strcpy(thrParams->sslKey, sslKey);
    strcpy(thrParams->serverPort, port);
    thrParams->clientCount = clientCount;

    dwError = VmRESTAllocateMemory(
                  sizeof(QUEUE),
                  (void *)&(myQueue)
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRestUtilsInitQueue(
                  myQueue
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    pQueue = myQueue;

    dwError = VmRESTAllocateMemory(
                  sizeof(pthread_t),
                  (void *)&(thr)
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = pthread_create(
                  thr,
                  NULL,
                  &(VmSockPosixServerListenThread),
                  thrParams
                  );
    pQueue->server_thread = thr;

cleanup:
    return dwError;
error:
    if (thrParams)
    {
        VmRESTFreeMemory(
            thrParams
            );
    }
    if (myQueue)
    {
        if (pQueue == myQueue)
        {
            VmRESTUtilsDestroyQueue(
                pQueue
                );
            pQueue = NULL;
        }
        VmRESTFreeMemory(myQueue);
    }
    if (thr)
    {
       VmRESTFreeMemory(thr);
    }
    dwError = VMREST_TRANSPORT_SERVER_THREAD_CREATE_FAILED;
    goto cleanup;
}

void
VmSockPosixDestroyServerSocket(
   )
{
    /* Exit the infinite loop of server thread */
    gServerSocketInfo.keepOpen = 0;

    /* join the server thread */
    if ( pQueue  && pQueue->server_thread )
    {
        pthread_join(*(pQueue->server_thread),
                        NULL
                        );
        VmRESTFreeMemory(pQueue->server_thread
                        );
        pQueue->server_thread = NULL;

        VmRESTUtilsDestroyQueue(pQueue
                        );
        VmRESTFreeMemory(pQueue
                        );
        pQueue = NULL;
    }
}

void*
VmSockPosixServerListenThread(
    void*                            Args
    )
{
    uint32_t                         dwError = ERROR_VMREST_SUCCESS;
    uint32_t                         maxListenClient = 0;
    int                              server_fd = -1;
    int                              epoll_fd = -1;
    int                              control_fd = -1;
    int                              hot_sockets = -1;
    int                              yes = 1;
    int                              i = 0;
    struct addrinfo                  hints = {0};
    struct addrinfo*                 serinfo = NULL;
    struct addrinfo*                 p = NULL;
    struct epoll_event               ev =  {0};
    struct epoll_event               events[MAX_EVENT];
    VM_EVENT_DATA                    acceptData = {0};
    VM_EVENT_DATA*                   accData1 = NULL;
    PVM_SERVER_THR_PARAMS            thrArgs = NULL;

    if (Args == NULL)
    {
        VMREST_LOG_DEBUG("VmSockPosixServerListenThread(): Invalid params");
        dwError = VMREST_TRANSPORT_INVALID_PARAM;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    SSL_library_init();

    thrArgs = Args;

    dwError = VmRESTSecureSocket(
                  thrArgs->sslCert,
                  thrArgs->sslKey
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    dwError = getaddrinfo(NULL,
                  thrArgs->serverPort,
                  &hints,
                  &serinfo
                  );
    if (dwError != 0)
    {
        VMREST_LOG_DEBUG("VmSockPosixServerListenThread(): getaddrinfo failed");
        dwError = VMREST_TRANSPORT_SOCKET_GET_ADDRINFO_ERROR;
    }
    BAIL_ON_VMREST_ERROR(dwError);
    maxListenClient = thrArgs->clientCount;

    if (thrArgs)
    {
        VmRESTFreeMemory(
            thrArgs
            );
        thrArgs = NULL;
    }
    for (p = serinfo; p!= NULL; p = p->ai_next)
    {
        if((server_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
            VMREST_LOG_DEBUG("VmSockPosixServerListenThread():Socket call failed.. Will make more attempts..");
            continue;
        }
        if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
        {
            VMREST_LOG_DEBUG("VmSockPosixServerListenThread(): Setsockopt failed");
            dwError = VMREST_TRANSPORT_SOCKET_SETSOCKOPT_ERROR;
        }
        BAIL_ON_VMREST_ERROR(dwError);

        if(bind(server_fd, p->ai_addr,p->ai_addrlen) == -1)
        {
            VMREST_LOG_DEBUG("VmSockPosixServerListenThread(): Bind failed...will make more attempts");
            continue;
        }
        break;
    }
    freeaddrinfo(serinfo);

    if(p == NULL)
    {
        VMREST_LOG_DEBUG("VmSockPosixServerListenThread(): Server Failed to bind");
        dwError = VMREST_TRANSPORT_SOCKET_BIND_ERROR;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmSockPosixSetSocketNonBlocking(
                  server_fd
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    if (listen(server_fd, maxListenClient) == -1)
    {
        VMREST_LOG_DEBUG("VmSockPosixServerListenThread(): Listen failed on server socket %d", server_fd);
        dwError = VMREST_TRANSPORT_SOCKET_LISTEN_ERROR;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    VMREST_LOG_DEBUG("SERVER LISTENING on %d .....", server_fd);

    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1)
    {
        VMREST_LOG_DEBUG("VmSockPosixServerListenThread(): Epoll Create Error");
        dwError = VMREST_TRANSPORT_EPOLL_CREATE_ERROR;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    pQueue->epoll_fd = epoll_fd;
    pQueue->server_fd = server_fd;

    acceptData.fd = server_fd;
    acceptData.index = -1;
    acceptData.ssl = NULL;

    ev.data.ptr = &acceptData;
    ev.events = EPOLLIN | EPOLLET;

    control_fd = epoll_ctl(
                     epoll_fd,
                     EPOLL_CTL_ADD,
                     server_fd,
                     &ev
                     );
    if (control_fd == -1)
    {
        VMREST_LOG_DEBUG("VmSockPosixServerListenThread(): epoll_ctl command failed");
        dwError = VMREST_TRANSPORT_EPOLL_CTL_ERROR;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    while (gServerSocketInfo.keepOpen)
    {
        /* TODO:: We need to provide timeout else during the cleanup,
        listnerer thread will keep on waiting unless epoll_wait returns */
        hot_sockets = epoll_wait(epoll_fd, events, MAX_EVENT, -1);
        pthread_mutex_lock(&(pQueue->lock));
        for (i= 0; i< hot_sockets;i++)
        {
            accData1 = (VM_EVENT_DATA*)events[i].data.ptr;
            dwError = VmRESTInsertElement(
                          accData1,
                          events[i].events,
                          pQueue
                          );
        }
        pthread_mutex_unlock(&(pQueue->lock));
        pthread_cond_broadcast(&(pQueue->signal));
    }
    close(server_fd);

cleanup:
    return NULL;
error:
    if (server_fd > 0)
    {
        close(server_fd);
    }
    goto cleanup;
}


uint32_t
VmSockPosixSetSocketNonBlocking(
    int                              server_fd
    )
{
    int                              cur_flags = 0;
    int                              set_flags = 0;
    uint32_t                         dwError = ERROR_VMREST_SUCCESS;

    cur_flags = fcntl(server_fd, F_GETFL, 0);
    if (cur_flags == -1)
    {
        VMREST_LOG_DEBUG("VmSockPosixSetSocketNonBlocking: fcntl() Get error");
        dwError = VMREST_TRANSPORT_SOCKET_NON_BLOCK_ERROR;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    cur_flags |= O_NONBLOCK;
    set_flags = fcntl(server_fd, F_SETFL, cur_flags);

    if (set_flags == -1)
    {
        VMREST_LOG_DEBUG("VmSockPosixSetSocketNonBlocking:fcntl() Set error");
        dwError = VMREST_TRANSPORT_SOCKET_NON_BLOCK_ERROR;
    }
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VmSockPosixHandleEventsFromQueue(
    void
    )
{
    EVENT_NODE*                      temp = NULL;
    uint32_t                         dwError = ERROR_VMREST_SUCCESS;
    PVM_EVENT_DATA                   acceptData = NULL;

    while (1)
    {
        /* get an element from queue */
        pthread_mutex_lock(&(pQueue->lock));
        if (pQueue->count == 0)
        {
            pthread_cond_wait(&(pQueue->signal), &(pQueue->lock));
        }
        temp = VmRESTUtilsRemoveElement(
                   pQueue
                   );
        pthread_mutex_unlock(&(pQueue->lock));

        if (!temp)
        {
            dwError = VMREST_TRANSPORT_QUEUE_EMPTY;
        }
        BAIL_ON_VMREST_ERROR(dwError);

        acceptData = &(temp->data);

        if ((temp->flag & EPOLLERR) || (temp->flag & EPOLLHUP) || (!(temp->flag & EPOLLIN)))
        {
            /* Error on this socket, hence closing */
            dwError = VmRESTRemoveClientFromGlobal(
                          acceptData->index
                          );
            close(acceptData->fd);
        }
        else if (acceptData->fd == pQueue->server_fd)
        {
            VMREST_LOG_DEBUG("VmSockPosixHandleEventsFromQueue(): Accepting new connection with fd %d", acceptData->fd);
            dwError = VmsockPosixAcceptNewConnection(
                          acceptData->fd
                          );
        }
        else
        {
            dwError = VmsockPosixReadDataAtOnce(
                          acceptData->ssl
                          );
        }
        BAIL_ON_VMREST_ERROR(dwError);
    }
cleanup:
    if (temp)
    {
        free(temp);
    }
    return dwError;

error:
    temp = NULL;
    goto cleanup;
}


uint32_t
VmsockPosixAcceptNewConnection(
    int                              server_fd
    )
{
    socklen_t                        sin_size = 0;
    struct sockaddr_storage          client_addr = {0};
    uint32_t                         dwError = ERROR_VMREST_SUCCESS;
    int                              accept_fd = -1;
    int                              control_fd = -1;
    struct epoll_event               ev = {0};
    SSL*                             ssl = NULL;
    uint32_t                         try = MAX_RETRY_ATTEMPTS;
    uint32_t                         cntRty = 0;
    uint32_t                         globalIndex = 0;
    VM_EVENT_DATA*                   acceptData = NULL;

    /* TODO : this has to replaced with vmfree function and memory free should be called **/
    acceptData = (VM_EVENT_DATA*)malloc(sizeof(VM_EVENT_DATA));
    memset(acceptData, '\0', sizeof(VM_EVENT_DATA));

    sin_size  = sizeof(client_addr);
    accept_fd = accept(server_fd, (struct sockaddr *)&client_addr, &sin_size);
    if (accept_fd == -1)
    {
        VMREST_LOG_DEBUG("VmsockPosixAcceptNewConnection():Accept connection failed");
        dwError = VMREST_TRANSPORT_ACCEPT_CONN_FAILED;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmSockPosixSetSocketNonBlocking(
                  accept_fd
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    ssl = SSL_new(gServerSocketInfo.sslContext);
    SSL_set_fd(ssl,accept_fd);

retry:
    if (SSL_accept(ssl) == -1)
    {
         if (cntRty <= try )
         {
             cntRty++;
             goto retry;
         }
         else
         {
             dwError = VMREST_TRANSPORT_SSL_ACCEPT_FAILED;
             BAIL_ON_VMREST_ERROR(dwError);
         }
    }

    dwError = VmRESTInsertClientFromGlobal(
                  ssl,
                  accept_fd,
                  &globalIndex
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    acceptData->fd = accept_fd;
    acceptData->index = globalIndex;
    acceptData->ssl = ssl;

    ev.data.ptr = (void *)acceptData;
    ev.events = EPOLLIN | EPOLLET;

    control_fd = epoll_ctl(pQueue->epoll_fd, EPOLL_CTL_ADD, accept_fd, &ev);
    if (control_fd == -1)
    {
        VMREST_LOG_DEBUG("VmsockPosixAcceptNewConnection():Epoll ctl command failed");
        dwError = VMREST_TRANSPORT_EPOLL_CTL_ERROR;
    }
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:
    return dwError;
error:
    goto cleanup;
}


uint32_t
VmsockPosixReadDataAtOnce(
    SSL*                             ssl
    )
{
    uint32_t                         dwError = ERROR_VMREST_SUCCESS;
    int                              read_cnt = 0;
    char                             buffer[MAX_DATA_BUFFER_LEN] = {0};

    if (ssl == NULL)
    {
        VMREST_LOG_DEBUG("VmsockPosixReadDataAtOnce(): Invalid params");
        dwError = VMREST_TRANSPORT_INVALID_PARAM;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    while(1)
    {
        memset(buffer,'\0',MAX_DATA_BUFFER_LEN);
        read_cnt = SSL_read(ssl, buffer, (sizeof(buffer)));
        if (read_cnt == -1 || read_cnt == 0)
        {
            break;
        }
        dwError =  VmRESTProcessIncomingData(
                       buffer,
                       (uint32_t)read_cnt,
                       ssl
                       );
        BAIL_ON_VMREST_ERROR(dwError);
    }
cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VmsockPosixWriteDataAtOnce(
    SSL*                             ssl,
    char*                            buffer,
    uint32_t                         bytes
    )
{
    uint32_t                         dwError = ERROR_VMREST_SUCCESS;

    SSL_write(ssl, buffer,bytes);
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:
    return dwError;
error:
    goto cleanup;
}

