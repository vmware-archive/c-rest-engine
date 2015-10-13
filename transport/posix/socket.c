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
VmSockPosixCreateServerSocket(
    void
    )
{   
    uint32_t dwError = 0;
    pthread_t *thr = NULL;
    QUEUE *myQueue = NULL;
    
    dwError = VmRESTAllocateMemory(sizeof(QUEUE), (void *)&(myQueue));
    BAIL_ON_POSIX_SOCK_ERROR(dwError);

    dwError = init_queue(myQueue);
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

void * 
VmSockPosixServerListenThread(
    void * Args
    )
{

    int server_fd = -1;
    int epoll_fd = -1;
    int control_fd = -1;
    int hot_sockets = -1;
    struct addrinfo hints = {0};
    struct addrinfo *serinfo = NULL;
    struct addrinfo *p = NULL;
    uint32_t dwError = 0;
    int yes = 1;
    struct epoll_event ev = {0};
    struct epoll_event events[MAX_EVENT];
    int i = 0;

    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    dwError = getaddrinfo(NULL, "61001", &hints, &serinfo);
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
    ev.data.fd = server_fd;
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
        /* Just for debugging. Will remove */
        write(1,"\nPosting events", 20);    
        pthread_mutex_lock(&(pQueue->lock));
        for (i= 0; i< hot_sockets;i++)
        {
            insert_element(events[i].data.fd, events[i].events, pQueue);
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
    int server_fd
    )
{
    int cur_flags = 0;
    int set_flags = 0;
    uint32_t dwError = 0;

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
    EVENT_NODE *temp = NULL;
    uint32_t dwError = 0;
    
    /* Just for debugging purpose -  will remove */
    write(1,"\nTHREAD Handler ..", 20);
    
    while (1) 
    { 
        /* get an element from queue */
        pthread_mutex_lock(&(pQueue->lock));
        if (pQueue->count == 0)
        {
            pthread_cond_wait(&(pQueue->signal), &(pQueue->lock));
        }
        temp = remove_element(pQueue);
        pthread_mutex_unlock(&(pQueue->lock));
        write(1,"\nTHREAD Handler - AfterWait", 28);
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
            dwError = VmsockPosixReadDataAtOnce(temp->fd); 
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
    int server_fd
    )
{   
    socklen_t sin_size = 0;
    struct sockaddr_storage client_addr = {0};
    uint32_t dwError = 0;
    int accept_fd = -1;
    int control_fd = -1;
    struct epoll_event ev = {0};
   
    /* Just for debugging: Will remove */
    write(1,"\nIn accept Connection", 25);

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

    ev.data.fd = accept_fd;
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
    int fd
    )
{
    int read_cnt;
    char buffer[128];
    write(1,"\nDATA Received ..", 20);
    while(1)
    {
        memset(buffer,'\0',128);
        read_cnt = read(fd, buffer, (sizeof(buffer) -1));
        buffer[read_cnt+1] = '\n';
        write(1,buffer, read_cnt+1);

        printf("Server received %s", buffer);
        if (read_cnt == -1 || read_cnt == 0)
        {
            break;
        }
    }
    return 0;
}
