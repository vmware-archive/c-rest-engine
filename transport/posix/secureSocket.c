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
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
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
    void
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    QUEUE*                           myQueue = NULL;

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

cleanup:
    return dwError;
error:
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
    dwError = VMREST_TRANSPORT_SERVER_THREAD_CREATE_FAILED;
    goto cleanup;
}

uint32_t
VmSockPosixStartServerSocket(
    char*                            sslCertificate,
    char*                            sslKey,
    char*                            port,
    uint32_t                         clientCount
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    pthread_t*                       thr = NULL;
    PVM_SERVER_THR_PARAMS            thrParams = NULL;

    dwError = VmRESTAllocateMemory(
                  sizeof(VM_SERVER_THR_PARAMS),
                  (void *)&(thrParams)
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    if (gServerSocketInfo.isSecure)
    {
        strcpy(thrParams->sslCert, sslCertificate);
        strcpy(thrParams->sslKey, sslKey);
    }
    else
    {
        memset(thrParams->sslKey, '\0', MAX_PATH_LEN);
        memset(thrParams->sslCert, '\0', MAX_PATH_LEN);
    }
    strcpy(thrParams->serverPort, port);
    thrParams->clientCount = clientCount;

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
    if (thr)
    {
       VmRESTFreeMemory(thr);
    }
    dwError = VMREST_TRANSPORT_SERVER_THREAD_START_FAILED;
    goto cleanup;
}

void
VmSockPosixStopServerSocket(
    void
    )
{
    /* Exit the infinite loop of server thread */
    gServerSocketInfo.keepOpen = 0;

    /* join the server thread */
    if (pQueue && pQueue->server_thread)
    {
        pthread_join(*(pQueue->server_thread),
                        NULL
                        );
        VmRESTFreeMemory(pQueue->server_thread
                        );
        pQueue->server_thread = NULL;

        VmRESTUtilsRemoveAllNodes(
            pQueue
            );
    }
}

void
VmSockPosixDestroyServerSocket(
   void
   )
{
    if (pQueue)
    {
        VmRESTUtilsDestroyQueue(
            pQueue
            );
        VmRESTFreeMemory(
            pQueue
            );
        pQueue = NULL;
    }
}

void*
VmSockPosixServerListenThread(
    void*                            Args
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
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
    char                             portNo[MAX_SERVER_PORT_LEN];
    uint32_t                         portLen = 0;

    if (Args == NULL)
    {
        VMREST_LOG_DEBUG("VmSockPosixServerListenThread(): Invalid params");
        dwError = VMREST_TRANSPORT_INVALID_PARAM;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    thrArgs = Args;
    memset(portNo, '\0', MAX_SERVER_PORT_LEN);
    strcpy(portNo, thrArgs->serverPort);

    if (gServerSocketInfo.isSecure)
    {
        SSL_library_init();
        dwError = VmRESTSecureSocket(
                      thrArgs->sslCert,
                      thrArgs->sslKey
                      );
        BAIL_ON_VMREST_ERROR(dwError);

        /**** The port string ends with char 's': remove it ****/
        portLen = strlen(portNo);
        portNo[--portLen] = '\0';
    }

    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    dwError = getaddrinfo(NULL,
                  portNo,
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

    epoll_fd = epoll_create(128);
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
        hot_sockets = epoll_wait(epoll_fd, events, MAX_EVENT, 100);
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
        if (hot_sockets != 0)
        {
            pthread_cond_broadcast(&(pQueue->signal));
        }
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
    uint32_t                         dwError = REST_ENGINE_SUCCESS;

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
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    PVM_EVENT_DATA                   acceptData = NULL;
    char                             appBuffer[MAX_DATA_BUFFER_LEN] = {0};
    uint32_t                         bytesRead = 0;

    while (gServerSocketInfo.ServerAlive)
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
            if (((gServerSocketInfo.clients[acceptData->index].ssl != NULL) && (gServerSocketInfo.clients[acceptData->index].ssl == acceptData->ssl)) || ((gServerSocketInfo.clients[acceptData->index].fd > 0) && (gServerSocketInfo.clients[acceptData->index].fd == acceptData->fd)))
            {
                dwError = VmSockPosixCloseConnection(
                              acceptData->index
                              );
            }
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
            memset(appBuffer,'\0',MAX_DATA_BUFFER_LEN);
            dwError = VmsockPosixGetXBytes(
                          MAX_DATA_BUFFER_LEN,
                          appBuffer,
                          acceptData->index,
                          &bytesRead
                          );
            BAIL_ON_VMREST_ERROR(dwError);

            if (bytesRead > 0)
            {
                dwError = VmRESTProcessIncomingData(
                              appBuffer,
                              bytesRead,
                              acceptData->index
                              );
                BAIL_ON_VMREST_ERROR(dwError);
            }
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
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    int                              accept_fd = -1;
    int                              control_fd = -1;
    struct epoll_event               ev = {0};
    SSL*                             ssl = NULL;
    uint32_t                         try = MAX_RETRY_ATTEMPTS;
    uint32_t                         cntRty = 0;
    uint32_t                         clientIndex = 0;
    PVM_EVENT_DATA                   acceptData = NULL;

    dwError = VmRESTAllocateMemory(
                  sizeof(VM_EVENT_DATA),
                  (void**)&acceptData
                  );
    BAIL_ON_VMREST_ERROR(dwError);    

    sin_size  = sizeof(client_addr);
tryagain:
    accept_fd = accept(server_fd, (struct sockaddr *)&client_addr, &sin_size);
    if (accept_fd == -1)
    {
        VMREST_LOG_DEBUG("VmsockPosixAcceptNewConnection():Accept connection failed, %d", errno);
        if (errno == 11)
        {
            if (cntRty <= try )
            {
                cntRty++;
                sleep(1);
                goto tryagain;
            }
        }
        dwError = VMREST_TRANSPORT_ACCEPT_CONN_FAILED;
    }
    BAIL_ON_VMREST_ERROR(dwError);
    cntRty = 0;
    VMREST_LOG_DEBUG("DEBUG: Accept connection success, new client socket %d", accept_fd);

    dwError = VmSockPosixSetSocketNonBlocking(
                  accept_fd
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    if (gServerSocketInfo.isSecure)
    {
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
    }

    acceptData->fd = accept_fd;
    acceptData->ssl = ssl;

    dwError = VmRESTInsertClientFromGlobal(
                  acceptData,
                  &clientIndex
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    acceptData->index = clientIndex;

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
    if (acceptData != NULL)
    {
        VmRESTFreeMemory(acceptData);
        acceptData = NULL;
        dwError = REST_ENGINE_SUCCESS;
    }
    goto cleanup;
}

uint32_t
VmSockPosixCloseConnection(
    uint32_t                         clientIndex
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    int                              ctlFd = 0;

    ctlFd = epoll_ctl(pQueue->epoll_fd,
                         EPOLL_CTL_DEL,
                         gServerSocketInfo.clients[clientIndex].fd,
                         NULL
                         );
    if (ctlFd == -1)
    {
        VMREST_LOG_DEBUG("ERROR: Epoll ctl command failed");
    }

    if (gServerSocketInfo.isSecure)
    {
        SSL_shutdown(gServerSocketInfo.clients[clientIndex].ssl);
        SSL_free(gServerSocketInfo.clients[clientIndex].ssl);
    }
    else
    {
        shutdown(gServerSocketInfo.clients[clientIndex].fd, SHUT_WR);
        ctlFd = close(gServerSocketInfo.clients[clientIndex].fd);
    }
    VmRESTFreeMemory(
        gServerSocketInfo.clients[clientIndex].self
        );

    dwError = VmRESTRemoveClientFromGlobal(
                  clientIndex
                  );
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VmSockPosixAdjustProcessedBytes(
    uint32_t                         clientIndex,
    uint32_t                         dataSeen
)
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;

    if (dataSeen > MAX_DATA_BUFFER_LEN)
    {
        VMREST_LOG_DEBUG("Invalid new Processed Data Index %u", dataSeen);
        dwError = VMREST_TRANSPORT_INVALID_PARAM;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    gServerSocketInfo.clients[clientIndex].dataProcessed = dataSeen;

cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VmsockPosixGetLastDataReadCount(
    uint32_t                         clientIndex,
    uint32_t*                        readCount
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;

    if (readCount == NULL)
    {
        VMREST_LOG_DEBUG("VmsockPosixGetLastDataReadCount(): NULL result pointer");
        dwError = VMREST_TRANSPORT_INVALID_PARAM;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    *readCount = gServerSocketInfo.clients[clientIndex].dataRead;

cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VmsockPosixGetXBytes(
    uint32_t                         bytesRequested,
    char*                            appBuffer,
    uint32_t                         clientIndex,
    uint32_t*                        bytesRead
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    uint32_t                         dataIndex = 0;
    uint32_t                         remainingBytes = 0;
    uint32_t                         dataAvailableInCache = 0;

    if (bytesRequested > MAX_DATA_BUFFER_LEN || appBuffer == NULL || bytesRead == NULL)
    {
        VMREST_LOG_DEBUG("Bytes to be read %u Large or appBuffer: %s",
                         bytesRequested, appBuffer);
        dwError = VMREST_TRANSPORT_INVALID_PARAM;
    }
    BAIL_ON_VMREST_ERROR(dwError);


    if (sizeof(appBuffer) > MAX_DATA_BUFFER_LEN)
    {
        VMREST_LOG_DEBUG("VmsockPosixGetXBytes():Invalid app data buffer size");
        dwError = VMREST_TRANSPORT_INVALID_PARAM;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    dataIndex = gServerSocketInfo.clients[clientIndex].dataProcessed;
    dataAvailableInCache = (gServerSocketInfo.clients[clientIndex].dataRead
                           - gServerSocketInfo.clients[clientIndex].dataProcessed);
    if (dataAvailableInCache >= bytesRequested)
    {
        /**** Enough data available in stream cache buffer ****/
        memcpy(appBuffer,
               &(gServerSocketInfo.clients[clientIndex].streamDataBuffer[dataIndex]),
               bytesRequested
               );
        gServerSocketInfo.clients[clientIndex].dataProcessed = (
               gServerSocketInfo.clients[clientIndex].dataProcessed + bytesRequested);
        *bytesRead = bytesRequested;
    }
    else if(dataAvailableInCache < bytesRequested)
    {
        /**** Copy all remaining client Stream bytes and perform read ****/
        if (dataAvailableInCache > 0)
        {
            memcpy(appBuffer,
            &(gServerSocketInfo.clients[clientIndex].streamDataBuffer[dataIndex]),
            dataAvailableInCache);
            gServerSocketInfo.clients[clientIndex].dataProcessed += dataAvailableInCache;
        }

        dwError = VmsockPosixReadDataAtOnce(
                      clientIndex
                      );
        BAIL_ON_VMREST_ERROR(dwError);

        remainingBytes = bytesRequested - dataAvailableInCache;
        dataIndex = 0;
        if (remainingBytes > gServerSocketInfo.clients[clientIndex].dataRead)
        {
            remainingBytes = gServerSocketInfo.clients[clientIndex].dataRead;
            VMREST_LOG_DEBUG("VmsockPosixGetXBytes() WARNING::\
             Requested %u bytes, available only %u bytes",
             bytesRequested,
             (dataAvailableInCache + remainingBytes));
        }
        memcpy((appBuffer + dataAvailableInCache),
              &(gServerSocketInfo.clients[clientIndex].streamDataBuffer[dataIndex]),
              remainingBytes);
        gServerSocketInfo.clients[clientIndex].dataProcessed = remainingBytes;
        *bytesRead = dataAvailableInCache + remainingBytes;
    }

cleanup:
    return dwError;
error:
    goto cleanup;
}


uint32_t
VmsockPosixReadDataAtOnce(
    uint32_t                         clientIndex
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    int                              read_cnt = 0;

    if ((gServerSocketInfo.isSecure == 1) &&
        (gServerSocketInfo.clients[clientIndex].ssl == NULL))
    {
         VMREST_LOG_DEBUG("WARNING: Trying to read closed or invalid SSL socket");
         goto cleanup;
    }
    else if ((gServerSocketInfo.isSecure == 0)
             && ( gServerSocketInfo.clients[clientIndex].fd <= 0))
    {
         VMREST_LOG_DEBUG("WARNING: Trying to read closed or invalid socket");
         goto cleanup;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    memset(gServerSocketInfo.clients[clientIndex].streamDataBuffer,
           '\0',MAX_DATA_BUFFER_LEN);
    if (gServerSocketInfo.isSecure && gServerSocketInfo.clients[clientIndex].ssl != NULL)
    {
        read_cnt = SSL_read(gServerSocketInfo.clients[clientIndex].ssl,
                            gServerSocketInfo.clients[clientIndex].streamDataBuffer,
                            MAX_DATA_BUFFER_LEN);
    }
    else if(gServerSocketInfo.clients[clientIndex].fd > 0)
    {
        read_cnt = read(gServerSocketInfo.clients[clientIndex].fd,
                   &(gServerSocketInfo.clients[clientIndex].streamDataBuffer),
                   MAX_DATA_BUFFER_LEN);
        VMREST_LOG_DEBUG("DEBUG: STREAM READ>>\n%s\n",&(gServerSocketInfo.clients[clientIndex].streamDataBuffer));
    }
    if (read_cnt == -1 || read_cnt == 0)
    {
        VMREST_LOG_DEBUG("WARNING:: Attempt to read closed\
socket or no data to read, bytes read %d",read_cnt);
    }
    VMREST_LOG_DEBUG("Bytes Read : %d", read_cnt);
    if (read_cnt == -1)
    {
        read_cnt = 0;
    }

    gServerSocketInfo.clients[clientIndex].dataProcessed = 0;
    gServerSocketInfo.clients[clientIndex].dataRead = read_cnt;

    BAIL_ON_VMREST_ERROR(dwError);

cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VmsockPosixWriteDataAtOnce(
    uint32_t                         clientIndex,
    char*                            buffer,
    uint32_t                         bytes
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    uint32_t                         bytesWritten = 0;

    if ((gServerSocketInfo.isSecure) && (gServerSocketInfo.clients[clientIndex].ssl != NULL))
    {
        SSL_write(gServerSocketInfo.clients[clientIndex].ssl, buffer,bytes);
    }
    else
    {
        bytesWritten = write(gServerSocketInfo.clients[clientIndex].fd, buffer,bytes);
        VMREST_LOG_DEBUG("\nData..........\n%s\n written bytes %u\n..............",
            buffer,bytesWritten);
    }
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:
    return dwError;
error:
    goto cleanup;
}

