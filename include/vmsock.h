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


/***************************************
* TODO: Local typedefs, this must be fix 
* properply.
***************************************/

typedef uint32_t                         UINT32;
typedef char*                            PBYTE;
typedef uint32_t                         DWORD;
typedef char*                            PSTR;
typedef char*                            PCHAR;
typedef uint16_t                         USHORT;
typedef void                             VOID;
typedef uint32_t*                        PDWORD;
typedef const char*                      PCSTR;
typedef void*                            PVOID;
typedef uint8_t                          BOOLEAN;
typedef uint64_t                         LONG;
typedef char                             CHAR;

typedef UINT32                           VM_SOCK_CREATE_FLAGS;

#define VM_SOCK_CREATE_FLAGS_NONE        0x00000000
#define VM_SOCK_CREATE_FLAGS_IPV4        0x00000001
#define VM_SOCK_CREATE_FLAGS_IPV6        0x00000002
#define VM_SOCK_CREATE_FLAGS_TCP         0x00000004
#define VM_SOCK_CREATE_FLAGS_UDP         0x00000008
#define VM_SOCK_CREATE_FLAGS_REUSE_ADDR  0x00000010
#define VM_SOCK_CREATE_FLAGS_NON_BLOCK   0x00000020

typedef struct _VM_SOCKET*               PVM_SOCKET;
typedef struct _VM_SOCK_EVENT_QUEUE*     PVM_SOCK_EVENT_QUEUE;

typedef struct _VM_SOCK_IO_BUFFER
{
    PBYTE                                pData;
    DWORD                                dwExpectedSize;
    DWORD                                dwCurrentSize;
    DWORD                                dwBytesTransferred;
    struct sockaddr_storage              clientAddr;
    socklen_t                            addrLen;

} VM_SOCK_IO_BUFFER, *PVM_SOCK_IO_BUFFER;

typedef struct _VM_STREAM_BUFFER
{
    uint32_t                             dataProcessed;
    uint32_t                             dataRead;
    char                                 pData[MAX_DATA_BUFFER_LEN];
} VM_STREAM_BUFFER, *PVM_STREAM_BUFFER;

typedef enum
{
    VM_SOCK_EVENT_TYPE_UNKNOWN = 0,
    VM_SOCK_EVENT_TYPE_NEW_CONNECTION,
    VM_SOCK_EVENT_TYPE_DATA_AVAILABLE,
    VM_SOCK_EVENT_TYPE_TCP_SIZE_READ_COMPLETED,
    VM_SOCK_EVENT_TYPE_TCP_DATA_READ_COMPLETED,
    VM_SOCK_EVENT_TYPE_TCP_SIZE_WRITE_COMPLETED,
    VM_SOCK_EVENT_TYPE_TCP_DATA_WRITE_COMPLETED,
    VM_SOCK_EVENT_TYPE_UDP_DATA_READ_COMPLETED,
    VM_SOCK_EVENT_TYPE_UDP_DATA_WRITE_COMPLETED,
    VM_SOCK_EVENT_TYPE_CONNECTION_CLOSED
} VM_SOCK_EVENT_TYPE, *PVM_SOCK_EVENT_TYPE;


/**
 * @brief  initialize windows socket package
 *
 * @param[in] ppPackage pointer to socket package
 *
 * @return DWORD - 0 on success
 */
DWORD
VmwSockInitialize(
    );

/**
 * @brief Opens a client socket
 *
 * @param[in]  pszHost  Target hostname or IP Address.
 *                      An empty string will imply the localhost.
 * @param[in]  usPort   16 bit port number
 * @param[in]  dwFlags  32 bit flags specifying socket creation preferences
 * @param[out] ppSocket Pointer to created socket context
 *
 * @return 0 on success
 */
DWORD
VmwSockOpenClient(
    PCSTR                            pszHost,
    USHORT                           usPort,
    VM_SOCK_CREATE_FLAGS             dwFlags,
    PVM_SOCKET*                      ppSocket
    );

/**
 * @brief Opens a server socket
 *
 * @param[in] usPort 16 bit local port number that the server listens on
 * @param[in,optional] iListenQueueSize
 *       size of connection acceptance queue.
 *       This value can be (-1) to use the default value.
 *
 * @param[in]  dwFlags 32 bit flags defining socket creation preferences
 * @param[out] ppSocket Pointer to created socket
 *
 * @return 0 on success
 */
DWORD
VmwSockOpenServer(
    USHORT                           usPort,
    int                              iListenQueueSize,
    VM_SOCK_CREATE_FLAGS             dwFlags,
    PVM_SOCKET*                      ppSocket
    );

/**
 * @brief Starts socket listener
 *
 * @param[in] pSocket Pointer to Socket
 * @param[in,optional] iListenQueueSize
 *
 * @return 0 on success
 */
DWORD
VmwSockStartListening(
    PVM_SOCKET                       pSocket,
    int                              iListenQueueSize
    );

/**
 * @brief Creates a Event queue to be used for detecting events on sockets
 *
 * @param[in,optional] iEventQueueSize
 *       specifies the event queue size.
 *       This value can be (-1) to use the default value
 * @param[out] ppQueue Pointer to accept created event queue
 *
 * @return 0 on success
 */
DWORD
VmwSockCreateEventQueue(
    int                              iEventQueueSize,
    PVM_SOCK_EVENT_QUEUE*            ppQueue
    );

/**
 * @brief Adds a socket to the event queue
 *
 * @param[in] pQueue  Pointer to Event queue
 * @param[in] pSocket Pointer to Socket
 *
 * @return 0 on success
 */
DWORD
VmwSockEventQueueAdd(
    PVM_SOCK_EVENT_QUEUE             pQueue,
    PVM_SOCKET                       pSocket
    );

/**
 * @brief Waits for an event on the event queue
 *
 * @param[in] pQueue   Pointer to event queue
 * @param[in,optional] iTimeoutMS
 *       Timeout in milliseconds.
 *       Waits forever if (-1) is passed in.
 * @param[out]    ppSocket   Pointer to socket that has an event
 * @param[in,out] pEventType Event type detected on socket
 *
 * @return 0 on success
 */
DWORD
VmwSockWaitForEvent(
    PVM_SOCK_EVENT_QUEUE             pQueue,
    int                              iTimeoutMS,
    PVM_SOCKET*                      ppSocket,
    PVM_SOCK_EVENT_TYPE              pEventType,
    PVM_SOCK_IO_BUFFER*              ppIoEvent
    );

/**
 * @brief Closes and frees event queue
 *
 * @param[in] pQueue Pointer to event queue
 *
 * @return 0 on success
 */

VOID
VmwSockCloseEventQueue(
    PVM_SOCK_EVENT_QUEUE             pQueue
    );

/**
 * @brief sets socket to be non-blocking
 *
 * @param[in] pSocket Pointer to socket
 *
 * @return 0 on success
 */

DWORD
VmwSockSetNonBlocking(
    PVM_SOCKET                       pSocket
    );

/**
 * @brief Retrieves the protocol the socket has been configured with
 *
 * @param[in]     pSocket     Pointer to socket
 * @param[in,out] pdwProtocol Protocol the socket has been configured with
 *                            This will be one of { SOCK_STREAM, SOCK_DGRAM... }
 */
DWORD
VmwSockGetProtocol(
    PVM_SOCKET                       pSocket,
    PDWORD                           pdwProtocol
    );

/**
 * @brief Sets data associated with the socket
 *
 * @param[in] pSocket Pointer to socket
 * @param[in] pData   Pointer to data associated with the socket
 * @param[in,out,optional] ppOldData Pointer to receive old data
 *
 * @return 0 on success
 */
DWORD
VmwSockSetData(
    PVM_SOCKET                       pSocket,
    PVOID                            pData,
    PVOID*                           ppOldData
    );

/**
 * @brief Gets data currently associated with the socket.
 *
 * @param[in]     pSocket Pointer to socket
 * @param[in,out] ppData  Pointer to receive data
 *
 * @return 0 on success
 */
DWORD
VmwSockGetData(
    PVM_SOCKET                       pSocket,
    PVOID*                           ppData
    );

/**
 * @brief Reads data from the socket
 *
 * @param[in]     pSocket      Pointer to socket
 * @param[in]     pBuffer      Buffer to read the data into
 * @param[in]     dwBufSize    Maximum size of the passed in buffer
 * @param[in,out] pdwBytesRead Number of bytes read in to the buffer
 * @param[in,out,optional] pClientAddress Client address to fill in optionally
 * @param[in,out,optional] pAddrLength    Length of the client address
 *
 * @return 0 on success
 */
DWORD
VmwSockRead(
    PVM_SOCKET                       pSocket,
    PVM_SOCK_IO_BUFFER               pIoBuffer
    );

/**
 * @brief Writes data to the socket
 *
 * @param[in]     pSocket      Pointer to socket
 * @param[in]     pBuffer      Buffer from which bytes have to be written
 * @param[in]     dwBufLen     Number of bytes to write from the buffer
 * @param[in,out] pdwBytesRead Number of bytes written to the socket
 * @param[in,optional] pClientAddress Client address to send to
 * @param[in,optional] addrLength     Length of the client address
 *
 * In case of UDP sockets, it is mandatory to provide the client address and
 * length.
 *
 * @return 0 on success
 */
DWORD
VmwSockWrite(
    PVM_SOCKET                       pSocket,
    const struct sockaddr*           pClientAddress,
    socklen_t                        addrLength,
    PVM_SOCK_IO_BUFFER               pIoBuffer
    );

/**
 * @brief  Acquires a reference on the socket
 *
 * @return Pointer to acquired socket
 */

PVM_SOCKET
VmwSockAcquire(
    PVM_SOCKET                       pSocket
    );

/**
 * @brief Releases current reference to socket
 *
 */
VOID
VmwSockRelease(
    PVM_SOCKET                       pSocket
    );

/**
 * @brief Closes the socket
 *        This call does not release the reference to the socket or free it.
 */
DWORD
VmwSockClose(
    PVM_SOCKET                       pSocket
    );

/**
 * @brief Checks if the string forms a valid IPV4 or IPV6 Address
 *
 * @return TRUE(1) if the string is a valid IP Address, 0 otherwise.
 */
BOOLEAN
VmwSockIsValidIPAddress(
    PCSTR                            pszAddress
    );

/**
 * @brief  VmwGetClientAddreess
 *
 * @param[in] pSocket       socket bound to a service
 * @param[inout] pAddress   socket address
 * @param[in] addresLen     lenth of th address
 *
 * @return DWORD - 0 on success
 */
DWORD
VmwSockGetAddress(
    PVM_SOCKET                       pSocket,
    struct sockaddr_storage*         pAddress,
    socklen_t*                       pAddresLen
    );

DWORD
VmwSockAllocateIoBuffer(
    VM_SOCK_EVENT_TYPE               eventType,
    DWORD                            dwSize,
    PVM_SOCK_IO_BUFFER*              ppIoContext
    );

/**
 * @brief  VmwReleaseIoContext
 *
 * @param[in] pIoContext 
 *
 * @return DWORD - 0 on success
 */
DWORD
VmwSockReleaseIoBuffer(
    PVM_SOCK_IO_BUFFER               pIoBuffer
    );

/**
 * @brief  shutdown windows socket package
 *
 */
VOID
VmwSockShutdown(
    );

VOID
VmwSockGetStreamBuffer(
    PVM_SOCKET                       pSocket,
    PVM_STREAM_BUFFER*               ppStreamBuffer
    );

VOID
VmwSockSetStreamBuffer(
    PVM_SOCKET                       pSocket,
    PVM_STREAM_BUFFER                pStreamBuffer
    );