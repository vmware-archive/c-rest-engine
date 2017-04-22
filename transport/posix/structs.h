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

typedef struct _VM_SOCKET
{
    LONG                             refCount;
    VM_SOCK_TYPE                     type;
    VM_SOCK_PROTOCOL                 protocol;
    struct sockaddr                  addr;
    socklen_t                        addrLen;
    PVM_STREAM_BUFFER                pStreamBuffer;
    struct sockaddr*                 pAddr;
    PVMREST_MUTEX                    pMutex;
    int                              fd;
    SSL*                             ssl;
    PVOID                            pData;
    uint32_t                         inUse;
    uint32_t                         wThrCnt;
} VM_SOCKET;

typedef struct _VM_SOCK_EVENT_QUEUE
{
    PVMREST_MUTEX                    pMutex;
    uint32_t                         bShutdown;
    PVM_SOCKET                       pSignalReader;
    PVM_SOCKET                       pSignalWriter;
    VM_SOCK_POSIX_EVENT_STATE        state;
    int                              epollFd;
    struct epoll_event *             pEventArray;
    DWORD                            dwSize;
    int                              nReady;
    int                              iReady;
    uint32_t                         thrCnt;
} VM_SOCK_EVENT_QUEUE;

typedef struct _VM_SOCK_IO_CONTEXT
{
    VM_SOCK_EVENT_TYPE               eventType;
    VM_SOCK_IO_BUFFER                IoBuffer;
    CHAR                             DataBuffer[1];
} VM_SOCK_IO_CONTEXT, *PVM_SOCK_IO_CONTEXT;

