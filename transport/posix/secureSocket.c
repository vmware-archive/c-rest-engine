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
    uint32_t             dwError = 0;
    int                  ret = 0;
    const SSL_METHOD*    method = NULL;
    SSL_CTX*             context = NULL;
  

    if (key == NULL || certificate == NULL)
    {
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_POSIX_SOCK_ERROR(dwError);     
    }
    
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    method = SSLv3_server_method();
    context = SSL_CTX_new(method);
    if ( context == NULL )
    {
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_POSIX_SOCK_ERROR(dwError);
    }
    
    ret = SSL_CTX_use_certificate_file(context, certificate, SSL_FILETYPE_PEM);
    if (ret <= 0) 
    {
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_POSIX_SOCK_ERROR(dwError);
    } 

    ret = SSL_CTX_use_PrivateKey_file(context, key, SSL_FILETYPE_PEM);  
    if (ret <= 0)
    {
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_POSIX_SOCK_ERROR(dwError);
    } 
    if (!SSL_CTX_check_private_key(context))
    {
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_POSIX_SOCK_ERROR(dwError)
    } 

    gServerSocketInfo.sslContext = context;
    
cleanup:
    return dwError;

error:
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
       VmRESTFreeMemory(myQueue);
    }
    if (thr)
    {
       VmRESTFreeMemory(thr);
    }
    goto cleanup;
}

void 
VmSockPosixDestroyServerSocket(
   )
{




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
    uint32_t                dwError = 0;
    int                     yes = 1;
    struct epoll_event      ev =  {0};
    struct epoll_event      events[MAX_EVENT];
    int                     i = 0;
    VM_EVENT_DATA           acceptData = {0}; 
    VM_EVENT_DATA*          accData1 = NULL;

    SSL_library_init();
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
    BAIL_ON_POSIX_SOCK_ERROR(dwError);

    for (p = serinfo; p!= NULL; p = p->ai_next) 
    {
        if((server_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) 
        {
            perror("Server Socket Failed");
            continue;
        }
        if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) 
        {
            perror("setsockopt failed");
            dwError = ERROR_NOT_SUPPORTED;
            BAIL_ON_POSIX_SOCK_ERROR(dwError);
        }
        if(bind(server_fd, p->ai_addr,p->ai_addrlen) == -1) 
        {
            perror("Socket bind at Server failed");
            continue;
        }
        break;
    }
    freeaddrinfo(serinfo);
    if(p == NULL)
    {
        perror("Server Failed to bind");
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_POSIX_SOCK_ERROR(dwError);
    }

    dwError = VmSockPosixSetSocketNonBlocking(server_fd);
    BAIL_ON_POSIX_SOCK_ERROR(dwError);

    if (listen(server_fd, 10) == -1) 
    {
        perror("Listen failed");
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_POSIX_SOCK_ERROR(dwError);
    }
    
    /* This is for debugging purpose, will remove once proper debug framework is put inplace */
    write(1,"Listening", 10);

    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) 
    {
        perror("epoll create Error");
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_POSIX_SOCK_ERROR(dwError);
    }
    pQueue->epoll_fd = epoll_fd;
    pQueue->server_fd = server_fd;

    acceptData.fd = server_fd;
    acceptData.clientNo = 2;
    acceptData.ssl = NULL;

    ev.data.ptr = &acceptData;
    ev.events = EPOLLIN | EPOLLET;
 
    control_fd = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &ev);
    if (control_fd == -1) 
    {
        perror("epoll_ctl command failed");
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_POSIX_SOCK_ERROR(dwError);
    }
    while (1) 
    {
        hot_sockets = epoll_wait(epoll_fd, events, MAX_EVENT, -1);
        pthread_mutex_lock(&(pQueue->lock));
        for (i= 0; i< hot_sockets;i++)
        {
            accData1 = (VM_EVENT_DATA*)events[i].data.ptr;        
            if (accData1->ssl != NULL)
            {
                VmRESTInsertElement(accData1->fd, accData1->ssl,events[i].events, pQueue);
            } 
            else 
            { 
                VmRESTInsertElement(accData1->fd, NULL,events[i].events, pQueue);
            }
        }
        pthread_mutex_unlock(&(pQueue->lock));
        pthread_cond_broadcast(&(pQueue->signal));        
        
    }
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
        perror("fcntl error");
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_POSIX_SOCK_ERROR(dwError);
    }
    cur_flags |= O_NONBLOCK;
    set_flags = fcntl(server_fd, F_SETFL, cur_flags);

    if (set_flags == -1)
    {
        perror("Unable to mark listening socket as non blocking");
        dwError = ERROR_NOT_SUPPORTED;
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
            dwError = ERROR_NOT_SUPPORTED;
            BAIL_ON_POSIX_SOCK_ERROR(dwError);
        }   
        
        if ((temp->flag & EPOLLERR) || (temp->flag & EPOLLHUP) || (!(temp->flag & EPOLLIN)))
        {
            /* Error on this socket, hence closing */
            close(temp->fd);
        }
        else if (temp->fd == pQueue->server_fd)
        {
            dwError = VmsockPosixAcceptNewConnection(temp->fd);
        }
        else 
        {  
            dwError = VmsockPosixReadDataAtOnce(temp->ssl); 
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
    VM_EVENT_DATA*          acceptData = NULL;

    acceptData = (VM_EVENT_DATA*)malloc(sizeof(VM_EVENT_DATA));
    memset(acceptData, '\0', sizeof(VM_EVENT_DATA));             
    

    sin_size  = sizeof(client_addr);
    accept_fd = accept(server_fd, (struct sockaddr *)&client_addr, &sin_size);
    if (accept_fd == -1)
    {
        perror("accept failed");
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_POSIX_SOCK_ERROR(dwError);
    }
    dwError = VmSockPosixSetSocketNonBlocking(accept_fd);
    if (dwError == ERROR_NOT_SUPPORTED)
    {
        perror("fcntl error");
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_POSIX_SOCK_ERROR(dwError);
    }

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
             dwError = ERROR_NOT_SUPPORTED;
             BAIL_ON_POSIX_SOCK_ERROR(dwError);
         }
    }

    acceptData->fd = accept_fd;
    acceptData->clientNo = 25;
    acceptData->ssl = ssl;   
 
    ev.data.ptr = (void *)acceptData;
    ev.events = EPOLLIN | EPOLLET;

    control_fd = epoll_ctl(pQueue->epoll_fd, EPOLL_CTL_ADD, accept_fd, &ev);
    if (control_fd == -1)
    {
        perror("epoll_ctl command failed");
        dwError = ERROR_NOT_SUPPORTED;
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
    uint32_t      dwError = 0;
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
    uint32_t        dwError = 0;
    
    SSL_write(ssl, buffer,bytes); 
    BAIL_ON_POSIX_SOCK_ERROR(dwError);

cleanup:
    return dwError;

error:
    goto cleanup;

}

