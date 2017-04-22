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

