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

#include "defines.h"

typedef struct _VM_SOCKET
{
    int fd;
    char address[MAX_ADDRESS_LEN];
    char port[MAX_PORT_LEN];

} VM_SOCKET;

typedef struct _QUEUE_NODE
{
    int fd;
    uint32_t flag;
    struct _QUEUE_NODE *next;
}EVENT_NODE;

typedef struct _VM_EVENT_QUEUE
{
    EVENT_NODE *head;
    EVENT_NODE *tail;
    uint32_t count;
    pthread_mutex_t lock;
    pthread_cond_t  signal;
    int epoll_fd;
    int server_fd;
    pthread_t *server_thread;
}QUEUE;
