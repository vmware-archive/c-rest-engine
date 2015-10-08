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

    int server_fd = -1;
    int accept_fd = -1;
    int epoll_fd = -1;
    int control_fd = -1;
    int hot_sockets = -1;
    struct addrinfo hints = {0};
    struct addrinfo *serinfo = NULL;
    struct addrinfo *p = NULL;
    socklen_t sin_size = 0;
    struct sockaddr_storage client_addr = {0};
    uint32_t ret = 0;
    int yes = 1;
    struct epoll_event ev = {0};
    struct epoll_event events[MAX_EVENT];
    int i = 0;

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

    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) 
    {
        perror("epoll create Error");
        ret = ERROR_NOT_SUPPORTED;
        BAIL_ON_POSIX_SOCK_ERROR(ret);
    }
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
        for (i= 0; i< hot_sockets;i++) 
        {
            if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP) || (!(events[i].events & EPOLLIN))) 
            {

                /* Error on this socket, hence closing */
                close(events[i].data.fd);
                continue;
            } 
            else if (events[i].data.fd == server_fd) 
            {
                /* This event of for new connect. Accept connection and add it to epoll */
                write(1,"New connection request", 24);
                sin_size  =sizeof(client_addr);
                accept_fd = accept(server_fd, (struct sockaddr *)&client_addr, &sin_size);
                if (accept_fd == -1) 
                {
                    perror("accept failed");
                    continue;
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

                control_fd = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, accept_fd, &ev);
                if (control_fd == -1) 
                {
                    perror("epoll_ctl command failed");
                    ret = ERROR_NOT_SUPPORTED;
                    BAIL_ON_POSIX_SOCK_ERROR(ret);
                }
            } 
            else
            {
                /* This corresponds to incoming data from different connection. */
                /* TODO :: Spawn threads for each connection. Thread poll will come into picture*/
                int read_cnt;
                char buffer[128];
                while(1) 
                {
                    /* As this is edge-triggered mode of epoll, just keep on reading from the fd 
                    unless no more data is left on the fd to be read. We are not going to get further 
                    notificatiopn on this fd for same data */
                    memset(buffer,'\0',128);
                    read_cnt = read(events[i].data.fd, buffer, (sizeof(buffer) -1));
                    buffer[read_cnt+1] = '\n';
                    write(1,buffer, read_cnt+1);

                    printf("Server received %s", buffer);
                    if (read_cnt == -1 || read_cnt == 0) 
                    {
                        break;
                    }
                }
            }
        }
    }
cleanup:
    return ret;

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


