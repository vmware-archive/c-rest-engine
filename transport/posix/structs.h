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

typedef struct _CONNECTION
{
    int                              fd;
    int                              notStale;
    SSL*                             ssl;
} VM_CONNECTION;

typedef struct _EVENT_DATA
{
    int                              fd;
    SSL*                             ssl;
    int                              index;
} VM_EVENT_DATA, *PVM_EVENT_DATA;

typedef struct _VM_SOCKET
{
    int                              fd;
    SSL_CTX*                         sslContext;
    VM_CONNECTION                    clients[MAX_CONNECTIONS];
    uint32_t                         clientCount;
    uint32_t                         emptyIndex;
    pthread_mutex_t                  lock;
    uint32_t                         keepOpen;
    uint32_t                         ServerAlive;
    char                             address[MAX_ADDRESS_LEN];
    char                             port[MAX_PORT_LEN];
} VM_SOCKET;

typedef struct _QUEUE_NODE
{
    VM_EVENT_DATA                    data;
    uint32_t                         flag;
    struct _QUEUE_NODE*              next;
}EVENT_NODE;

typedef struct _VM_EVENT_QUEUE
{
    EVENT_NODE*                      head;
    EVENT_NODE*                      tail;
    uint32_t                         count;
    pthread_mutex_t                  lock;
    pthread_cond_t                   signal;
    int                              epoll_fd;
    int                              server_fd;
    pthread_t*                       server_thread;
}QUEUE;

typedef struct _VM_SERVER_THR_PARAMS
{
    char                             sslCert[MAX_PATH_LEN];
    char                             sslKey[MAX_PATH_LEN];
    char                             serverPort[MAX_SERVER_PORT_LEN];
    uint32_t                         clientCount;
} VM_SERVER_THR_PARAMS, *PVM_SERVER_THR_PARAMS;

