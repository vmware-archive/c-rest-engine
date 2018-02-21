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
    VM_SOCK_TYPE                     type;
    struct sockaddr                  addr;
    socklen_t                        addrLen;
    PVMREST_MUTEX                    pMutex;
    int                              fd;
    SSL*                             ssl;
    BOOLEAN                          bSSLHandShakeCompleted;
    BOOLEAN                          bTimerExpired;
    char*                            pszBuffer;
    uint32_t                         nBufData;
    uint32_t                         nProcessed;
    PREST_REQUEST                    pRequest;
    struct _VM_SOCKET*               pIoSocket;
    struct _VM_SOCKET*               pTimerSocket;
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
