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
    pthread_t thr;
    
    pthread_create(&thr,NULL,&(VmSockPosixServerListenThread),NULL);
    return 0;
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
    uint32_t ret = 0;
    int yes = 1;
    struct epoll_event ev = {0};
    struct epoll_event events[MAX_EVENT];
    int i = 0;
    QUEUE *myQueue = NULL;

    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    ret = getaddrinfo(NULL, "61001", &hints, &serinfo);
    if (ret != 0) 
    {
        printf("getaddrifo return error: %s", gai_strerror(ret));
        ret = ERROR_NOT_SUPPORTED;
        BAIL_ON_POSIX_SOCK_ERROR(ret);
    }

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
            ret = ERROR_NOT_SUPPORTED;
            BAIL_ON_POSIX_SOCK_ERROR(ret);
        }
        if(bind(server_fd, p->ai_addr,p->ai_addrlen) == -1) 
        {
            close(server_fd);
            perror("Socket bind at Server failed");
            continue;
        }
        break;
    }
    freeaddrinfo(serinfo);
    if(p == NULL)
    {
        perror("Server Failed to bind");
        ret = ERROR_NOT_SUPPORTED;
        BAIL_ON_POSIX_SOCK_ERROR(ret);
    }

    ret = VmSockPosixSetSocketNonBlocking(server_fd);
    if (ret == ERROR_NOT_SUPPORTED) 
    {
        perror("fcntl error");
        ret = ERROR_NOT_SUPPORTED;
        BAIL_ON_POSIX_SOCK_ERROR(ret);
    }

    if (listen(server_fd, 10) == -1) 
    {
        perror("Listen failed");
        ret = ERROR_NOT_SUPPORTED;
        BAIL_ON_POSIX_SOCK_ERROR(ret);
    }

    write(1,"Listening", 10);

    myQueue = (QUEUE*)malloc(sizeof(QUEUE));
    init_queue(myQueue);
    pQueue = myQueue;
    

    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) 
    {
        perror("epoll create Error");
        ret = ERROR_NOT_SUPPORTED;
        BAIL_ON_POSIX_SOCK_ERROR(ret);
    }
    pQueue->epoll_fd = epoll_fd;
    pQueue->server_fd = server_fd;
    ev.data.fd = server_fd;
    ev.events = EPOLLIN | EPOLLET;

    control_fd = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &ev);
    if (control_fd == -1) 
    {
        perror("epoll_ctl command failed");
        ret = ERROR_NOT_SUPPORTED;
        BAIL_ON_POSIX_SOCK_ERROR(ret);
    }
    while (1) 
    {
        hot_sockets = epoll_wait(epoll_fd, events, MAX_EVENT, -1);
        // kaushik
        write(1,"\nPosting events", 20);    
        pthread_mutex_lock(&(myQueue->lock));
        for (i= 0; i< hot_sockets;i++)
        {
            insert_element(events[i].data.fd, events[i].events, pQueue);
        }
        pthread_mutex_unlock(&(myQueue->lock));
        pthread_cond_broadcast(&(myQueue->signal));        
        
    }
cleanup:
    return NULL;

error:
    goto cleanup;
}


uint32_t VmSockPosixSetSocketNonBlocking(
    int server_fd
    )
{
    int cur_flags = 0;
    int set_flags = 0;
    uint32_t ret = 0;

    cur_flags = fcntl(server_fd, F_GETFL, 0);
    if (cur_flags == -1)
    {
        perror("fcntl error");
        ret = ERROR_NOT_SUPPORTED;
        BAIL_ON_POSIX_SOCK_ERROR(ret);
    }
    cur_flags |= O_NONBLOCK;
    set_flags = fcntl(server_fd, F_SETFL, cur_flags);

    if (set_flags == -1)
    {
        perror("Unable to mark listening socket as non blocking");
        ret = ERROR_NOT_SUPPORTED;
        BAIL_ON_POSIX_SOCK_ERROR(ret);
    }

cleanup:
    return ret;

error:
    goto cleanup;
}

uint32_t VmSockPosixHandleEventsFromQueue(
    void
    )
{
    EVENT_NODE *temp = NULL;
    uint32_t ret = 0;
    
    // kaushik
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
        ret = ERROR_NOT_SUPPORTED;
        BAIL_ON_POSIX_SOCK_ERROR(ret);

    }   
    
    if ((temp->flag & EPOLLERR) || (temp->flag & EPOLLHUP) || (!(temp->flag & EPOLLIN)))
    {
        /* Error on this socket, hence closing */
        close(temp->fd);
       // continue;
    }
    else if (temp->fd == pQueue->server_fd)
    {
        ret = VmsockPosixAcceptNewConnection(temp->fd);
      
    }
    else 
    {  
        ret = VmsockPosixReadDataAtOnce(temp->fd); 
    }
   }
cleanup:
    free(temp);
    return ret;

error:
    goto cleanup;
}


uint32_t VmsockPosixAcceptNewConnection(
    int server_fd
    )
{   
    socklen_t sin_size = 0;
    struct sockaddr_storage client_addr = {0};
    uint32_t ret = 0;
    int accept_fd = -1;
    int control_fd = -1;
    struct epoll_event ev = {0};
    //kaushik

    write(1,"\nIn accept Connection", 25);

    /* This event of for new connect. Accept connection and add it to epoll */
    write(1,"New connection request", 24);
    sin_size  = sizeof(client_addr);
    accept_fd = accept(server_fd, (struct sockaddr *)&client_addr, &sin_size);
    if (accept_fd == -1)
    {
        perror("accept failed");
        return 1;
    }
    ret = VmSockPosixSetSocketNonBlocking(accept_fd);
    if (ret == ERROR_NOT_SUPPORTED)
    {
        perror("fcntl error");
        ret = ERROR_NOT_SUPPORTED;
        BAIL_ON_POSIX_SOCK_ERROR(ret);
    }

    ev.data.fd = accept_fd;
    ev.events = EPOLLIN | EPOLLET;

    control_fd = epoll_ctl(pQueue->epoll_fd, EPOLL_CTL_ADD, accept_fd, &ev);
    if (control_fd == -1)
    {
        perror("epoll_ctl command failed");
        ret = ERROR_NOT_SUPPORTED;
        BAIL_ON_POSIX_SOCK_ERROR(ret);
    }
           
cleanup:
    return ret;

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
