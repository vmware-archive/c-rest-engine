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
    char*                certificate,
    char*                key
    )
{
    uint32_t             dwError = VMREST_TRANSPORT_NO_ERROR;
    int                  ret = 0;
    const SSL_METHOD*    method = NULL;
    SSL_CTX*             context = NULL;
  

    if (key == NULL || certificate == NULL)
    {
        dwError = VMREST_TRANSPORT_INVALID_PARAM;
        BAIL_ON_POSIX_SOCK_ERROR(dwError);     
    }
    
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    method = SSLv3_server_method();
    context = SSL_CTX_new(method);
    if ( context == NULL )
    {
        dwError = VMREST_TRANSPORT_SSL_CONFIG_ERROR;
        BAIL_ON_POSIX_SOCK_ERROR(dwError);
    }
    
    ret = SSL_CTX_use_certificate_file(context, certificate, SSL_FILETYPE_PEM);
    if (ret <= 0) 
    {
        dwError = VMREST_TRANSPORT_SSL_CERTIFICATE_ERROR;
        BAIL_ON_POSIX_SOCK_ERROR(dwError);
    } 

    ret = SSL_CTX_use_PrivateKey_file(context, key, SSL_FILETYPE_PEM);  
    if (ret <= 0)
    {
        dwError = VMREST_TRANSPORT_SSL_PRIVATEKEY_ERROR;
        BAIL_ON_POSIX_SOCK_ERROR(dwError);
    } 
    if (!SSL_CTX_check_private_key(context))
    {
        dwError = VMREST_TRANSPORT_SSL_PRIVATEKEY_CHECK_ERROR;
        BAIL_ON_POSIX_SOCK_ERROR(dwError)
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
    uint32_t        dwError = 0;
    pthread_t*      thr = NULL;
    QUEUE*          myQueue = NULL;
    
    dwError = VmRESTAllocateMemory(sizeof(QUEUE), (void *)&(myQueue));
    BAIL_ON_POSIX_SOCK_ERROR(dwError);

    dwError = VmRestUtilsInitQueue(myQueue);
    BAIL_ON_POSIX_SOCK_ERROR(dwError);
    
    pQueue = myQueue;
    
    dwError = VmRESTAllocateMemory(sizeof(pthread_t), (void *)&(thr));
    BAIL_ON_POSIX_SOCK_ERROR(dwError);
      
    dwError = pthread_create(thr,NULL,&(VmSockPosixServerListenThread),NULL);
    pQueue->server_thread = thr;
    
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
    pthread_join(*(pQueue->server_thread), NULL);

    VmRESTFreeMemory(pQueue->server_thread);
    pQueue->server_thread = NULL;

    VmRESTUtilsDestroyQueue(pQueue);

    VmRESTFreeMemory(pQueue);

    pQueue = NULL;

}

void * 
VmSockPosixServerListenThread(
    void*                   Args
    )
{
    int                     server_fd = -1;
    int                     epoll_fd = -1;
    int                     control_fd = -1;
    int                     hot_sockets = -1;
    struct addrinfo         hints = {0};
    struct addrinfo*        serinfo = NULL;
    struct addrinfo*        p = NULL;
    uint32_t                dwError = VMREST_TRANSPORT_NO_ERROR;
    int                     yes = 1;
    struct epoll_event      ev =  {0};
    struct epoll_event      events[MAX_EVENT];
    int                     i = 0;
    VM_EVENT_DATA           acceptData = {0}; 
    VM_EVENT_DATA*          accData1 = NULL;

    SSL_library_init();

    /* TODO:: Must be configured from config module (to be added) */
    dwError = VmRESTSecureSocket(
                  "/root/mycert.pem",
                  "/root/mycert.pem"
              );

    BAIL_ON_POSIX_SOCK_ERROR(dwError);

    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    dwError = getaddrinfo(NULL, gServerSocketInfo.port, &hints, &serinfo);
    if (dwError != 0) 
    {   
        dwError = VMREST_TRANSPORT_SOCKET_GET_ADDRINFO_ERROR;
        BAIL_ON_POSIX_SOCK_ERROR(dwError);
    }
    for (p = serinfo; p!= NULL; p = p->ai_next) 
    {
        if((server_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) 
        {
            VMREST_LOG_DEBUG("Server Socket creation failed.. Will make more attempts..");
            continue;
        }
        if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) 
        {
            VMREST_LOG_DEBUG("Setsockopt failed");
            dwError = VMREST_TRANSPORT_SOCKET_SETSOCKOPT_ERROR;
            BAIL_ON_POSIX_SOCK_ERROR(dwError);
        }
        if(bind(server_fd, p->ai_addr,p->ai_addrlen) == -1) 
        {
            VMREST_LOG_DEBUG("Socket bind at Server failed...will make more attempts");
            continue;
        }
        break;
    }
    freeaddrinfo(serinfo);
    if(p == NULL)
    {
        VMREST_LOG_DEBUG("Server Failed to bind");
        dwError = VMREST_TRANSPORT_SOCKET_BIND_ERROR;
        BAIL_ON_POSIX_SOCK_ERROR(dwError);
    }

    dwError = VmSockPosixSetSocketNonBlocking(server_fd);
    BAIL_ON_POSIX_SOCK_ERROR(dwError);

    if (listen(server_fd, 10) == -1) 
    {
        VMREST_LOG_DEBUG("Listen failed on server socket %d", server_fd);
        dwError = VMREST_TRANSPORT_SOCKET_LISTEN_ERROR;
        BAIL_ON_POSIX_SOCK_ERROR(dwError);
    }
    
    VMREST_LOG_DEBUG("SERVER LISTENING on %d .....", server_fd); 
    /* This is for debugging purpose, will remove once proper debug framework is put inplace */
    
    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) 
    {
        VMREST_LOG_DEBUG("Epoll Create Error");
        dwError = VMREST_TRANSPORT_EPOLL_CREATE_ERROR;
        BAIL_ON_POSIX_SOCK_ERROR(dwError);
    }
    pQueue->epoll_fd = epoll_fd;
    pQueue->server_fd = server_fd;

    acceptData.fd = server_fd;
    acceptData.index = -1;
    acceptData.ssl = NULL;

    ev.data.ptr = &acceptData;
    ev.events = EPOLLIN | EPOLLET;
 
    control_fd = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &ev);
    if (control_fd == -1) 
    {
        VMREST_LOG_DEBUG("epoll_ctl command failed");
        dwError = VMREST_TRANSPORT_EPOLL_CTL_ERROR;
        BAIL_ON_POSIX_SOCK_ERROR(dwError);
    }
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
    if (server_fd> 0)
    {
        close(server_fd);
    }
    goto cleanup;
}


uint32_t VmSockPosixSetSocketNonBlocking(
    int             server_fd
    )
{
    int             cur_flags = 0;
    int             set_flags = 0;
    uint32_t        dwError = 0;

    cur_flags = fcntl(server_fd, F_GETFL, 0);
    if (cur_flags == -1)
    {
        VMREST_LOG_DEBUG("fcntl() Get error");
        dwError = VMREST_TRANSPORT_SOCKET_NON_BLOCK_ERROR;
        BAIL_ON_POSIX_SOCK_ERROR(dwError);
    }
    cur_flags |= O_NONBLOCK;
    set_flags = fcntl(server_fd, F_SETFL, cur_flags);

    if (set_flags == -1)
    {
        VMREST_LOG_DEBUG("fcntl() Set error: Unable to mark socket %d as non blocking", server_fd);
        dwError = VMREST_TRANSPORT_SOCKET_NON_BLOCK_ERROR;
        BAIL_ON_POSIX_SOCK_ERROR(dwError);
    }

cleanup:
    return dwError;

error:
    goto cleanup;
}

uint32_t VmSockPosixHandleEventsFromQueue(
    void
    )
{
    EVENT_NODE*         temp = NULL;
    uint32_t            dwError = 0;
    PVM_EVENT_DATA      acceptData = NULL;
    
    while (1) 
    { 
        /* get an element from queue */
        pthread_mutex_lock(&(pQueue->lock));
        if (pQueue->count == 0)
        {
            pthread_cond_wait(&(pQueue->signal), &(pQueue->lock));
        }
        temp = VmRESTUtilsRemoveElement(pQueue);
        pthread_mutex_unlock(&(pQueue->lock));
        if (!temp)
        {
            dwError = VMREST_TRANSPORT_QUEUE_EMPTY;
            BAIL_ON_POSIX_SOCK_ERROR(dwError);
        }
        acceptData = &(temp->data);
        
        if ((temp->flag & EPOLLERR) || (temp->flag & EPOLLHUP) || (!(temp->flag & EPOLLIN)))
        {
            /* Error on this socket, hence closing */
            dwError = VmRESTRemoveClientFromGlobal(
                      acceptData->index
                      );
            BAIL_ON_POSIX_SOCK_ERROR(dwError);
          
            close(acceptData->fd);
        }
        else if (acceptData->fd == pQueue->server_fd)
        {
            VMREST_LOG_DEBUG("Accepting new connection with fd %d", acceptData->fd);
            dwError = VmsockPosixAcceptNewConnection(acceptData->fd);
        }
        else 
        {  
            dwError = VmsockPosixReadDataAtOnce(acceptData->ssl); 
        }
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


uint32_t VmsockPosixAcceptNewConnection(
    int                     server_fd
    )
{   
    socklen_t               sin_size = 0;
    struct sockaddr_storage client_addr = {0};
    uint32_t                dwError = 0;
    int                     accept_fd = -1;
    int                     control_fd = -1;
    struct epoll_event      ev = {0};
    SSL*                    ssl = NULL;  
    uint32_t                try = 5000;
    uint32_t                cntRty = 0;
    uint32_t                globalIndex = 0;
    VM_EVENT_DATA*          acceptData = NULL;

    acceptData = (VM_EVENT_DATA*)malloc(sizeof(VM_EVENT_DATA));
    memset(acceptData, '\0', sizeof(VM_EVENT_DATA));             
    
    sin_size  = sizeof(client_addr);
    accept_fd = accept(server_fd, (struct sockaddr *)&client_addr, &sin_size);
    if (accept_fd == -1)
    {
        VMREST_LOG_DEBUG("Accept connection failed");
        dwError = VMREST_TRANSPORT_ACCEPT_CONN_FAILED;
        BAIL_ON_POSIX_SOCK_ERROR(dwError);
    }
    dwError = VmSockPosixSetSocketNonBlocking(accept_fd);
    BAIL_ON_POSIX_SOCK_ERROR(dwError);

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
             BAIL_ON_POSIX_SOCK_ERROR(dwError);
         }
    }

    dwError = VmRESTInsertClientFromGlobal(
              ssl,
              accept_fd,
              &globalIndex
              );
    BAIL_ON_POSIX_SOCK_ERROR(dwError);

    acceptData->fd = accept_fd;
    acceptData->index = globalIndex;
    acceptData->ssl = ssl;   
 
    ev.data.ptr = (void *)acceptData;
    ev.events = EPOLLIN | EPOLLET;

    control_fd = epoll_ctl(pQueue->epoll_fd, EPOLL_CTL_ADD, accept_fd, &ev);
    if (control_fd == -1)
    {
        VMREST_LOG_DEBUG("Epoll ctl command failed");
        dwError = VMREST_TRANSPORT_EPOLL_CTL_ERROR;
        BAIL_ON_POSIX_SOCK_ERROR(dwError);
    }
    
cleanup:
    return dwError;

error:
    goto cleanup;
}


uint32_t VmsockPosixReadDataAtOnce(
    SSL*          ssl
    )
{
    uint32_t      dwError = VMREST_TRANSPORT_NO_ERROR;
    int           read_cnt = 0;
    char          buffer[4096];

    while(1)
    {
        memset(buffer,'\0',4096);
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
        BAIL_ON_POSIX_SOCK_ERROR(dwError);

    }
cleanup:
    return dwError;

error:
    goto cleanup;    
}

uint32_t VmsockPosixWriteDataAtOnce(
    SSL*            ssl,
    char*           buffer,
    uint32_t        bytes
    )
{
    uint32_t        dwError = VMREST_TRANSPORT_NO_ERROR;
    
    SSL_write(ssl, buffer,bytes); 
    BAIL_ON_POSIX_SOCK_ERROR(dwError);

cleanup:
    return dwError;

error:
    goto cleanup;

}

