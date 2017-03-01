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


typedef struct _VM_SOCKET
{
    SOCKET                           hSocket;
    VM_SOCK_PROTOCOL                 protocol;
    VM_SOCK_TYPE                     type;
    ULONG                            refCount;
    PVM_SOCK_EVENT_QUEUE             pEventQueue;
    PVM_STREAM_BUFFER                pStreamBuffer;
    HANDLE                           hThreadListen;
    struct sockaddr_storage          addr;
	SSL*                             ssl;
    int                              addrLen;
	VM_SOCK_TYPE                     v4v6;
	uint32_t                         wThrCnt;
} VM_SOCKET;

typedef struct _VM_SOCK_EVENT_QUEUE
{
    HANDLE                           hIOCP;
    HANDLE                           hEventListen;
    BOOL                             bShutdown;
	PVMREST_MUTEX                    pMutex;
	VM_SOCKET*                       pListenerTCPv4;
	VM_SOCKET*                       pListenerTCPv6;
	uint32_t                         thrCnt;
}VM_SOCK_EVENT_QUEUE;

typedef struct _VM_SOCK_IO_CONTEXT
{
    OVERLAPPED                       Overlapped;
    VM_SOCK_EVENT_TYPE               eventType;
    VM_SOCK_IO_BUFFER                IoBuffer;
    CHAR                             DataBuffer[1];
} VM_SOCK_IO_CONTEXT, *PVM_SOCK_IO_CONTEXT;

typedef struct _SOCK_SSL_INFO
{
    SSL_CTX*                         sslContext;
    uint32_t                         isSecure;
	uint32_t                         isQueueInUse;
} SOCK_SSL_INFO;
