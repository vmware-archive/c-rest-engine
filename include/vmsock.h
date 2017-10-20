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

#include <vmrest.h>
#ifdef WIN32
#include <winsock2.h>
#include <windows.h>
typedef int socklen_t;
#else
#include <vmrestsys.h>
#endif

typedef uint32_t                         UINT32;
typedef char*                            PSTR;
typedef char*                            PCHAR;
typedef uint16_t                         USHORT;
#ifndef WIN32
typedef void                             VOID;
typedef uint64_t                         LONG;
typedef uint32_t*                        PDWORD;
typedef uint32_t                         DWORD; 
typedef char*                            PBYTE;
#endif
typedef const char*                      PCSTR;
typedef void*                            PVOID;
typedef uint8_t                          BOOLEAN;
typedef char                             CHAR;
typedef UINT32                           VM_SOCK_CREATE_FLAGS;

#define VM_SOCK_CREATE_FLAGS_NONE        0x00000000
#define VM_SOCK_CREATE_FLAGS_IPV4        0x00000001
#define VM_SOCK_CREATE_FLAGS_IPV6        0x00000002
#define VM_SOCK_CREATE_FLAGS_TCP         0x00000004
#define VM_SOCK_CREATE_FLAGS_UDP         0x00000008
#define VM_SOCK_CREATE_FLAGS_REUSE_ADDR  0x00000010
#define VM_SOCK_CREATE_FLAGS_NON_BLOCK   0x00000020
#define VM_SOCK_IS_SSL                   0x00000040

typedef struct _VM_SOCKET*               PVM_SOCKET;
typedef struct _VM_SOCK_EVENT_QUEUE*     PVM_SOCK_EVENT_QUEUE;

typedef struct _VM_SOCK_IO_BUFFER
{
    char*                                pData;
    uint32_t                             dwExpectedSize;
    uint32_t                             dwCurrentSize;
    uint32_t                             dwBytesTransferred;
	uint32_t                             dwTotalBytesTransferred;
    struct sockaddr_storage              clientAddr;
    socklen_t                            addrLen;
} VM_SOCK_IO_BUFFER, *PVM_SOCK_IO_BUFFER;

typedef struct _VM_STREAM_BUFFER
{
    uint32_t                             dataProcessed;
    uint32_t                             dataRead;
    char                                 pData[4096];
} VM_STREAM_BUFFER, *PVM_STREAM_BUFFER;

typedef enum
{
    VM_SOCK_EVENT_TYPE_UNKNOWN = 0,
    VM_SOCK_EVENT_TYPE_DATA_AVAILABLE,
    VM_SOCK_EVENT_TYPE_TCP_NEW_CONNECTION,
    VM_SOCK_EVENT_TYPE_TCP_REQUEST_SIZE_READ,
    VM_SOCK_EVENT_TYPE_TCP_REQUEST_DATA_READ,
    VM_SOCK_EVENT_TYPE_TCP_RESPONSE_SIZE_WRITE,
    VM_SOCK_EVENT_TYPE_TCP_RESPONSE_DATA_WRITE,
    VM_SOCK_EVENT_TYPE_TCP_FWD_REQUEST,
    VM_SOCK_EVENT_TYPE_TCP_FWD_REQUEST_SIZE_WRITE,
    VM_SOCK_EVENT_TYPE_TCP_FWD_REQUES_DATA_WRITE,
    VM_SOCK_EVENT_TYPE_TCP_FWD_RESPONSE_SIZE_READ,
    VM_SOCK_EVENT_TYPE_TCP_FWD_RESPONSE_DATA_READ,
    VM_SOCK_EVENT_TYPE_UDP_REQUEST_DATA_READ,
    VM_SOCK_EVENT_TYPE_UDP_RESPONSE_DATA_WRITE,
    VM_SOCK_EVENT_TYPE_UDP_FWD_REQUEST,
    VM_SOCK_EVENT_TYPE_UDP_FWD_REQUEST_DATA_WRITE,
    VM_SOCK_EVENT_TYPE_UDP_FWD_RESPONSE_DATA_READ,
    VM_SOCK_EVENT_TYPE_CONNECTION_CLOSED,
    VM_SOCK_EVENT_TYPE_MAX,
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
    PVMREST_HANDLE                   pRESTHandle    
    );

/**
 * @brief Opens a server socket
 *
 * @param[in] Handle to library instance.
 * @param[in] usPort 16 bit local port number that the server listens on
 * @param[in,optional] iListenQueueSize
 *       size of connection acceptance queue.
 *       This value can be (-1) to use the default value.
 *
 * @param[in]  dwFlags 32 bit flags defining socket creation preferences
 * @param[in]  Path to SSL Certificate file
 * @param[in]  Path to SSL Key File
 * @param[out] ppSocket Pointer to created socket
 *
 * @return 0 on success
 */
DWORD
VmwSockOpenServer(
    PVMREST_HANDLE                   pRESTHandle,
    USHORT                           usPort,
    int                              iListenQueueSize,
    VM_SOCK_CREATE_FLAGS             dwFlags,
    PVM_SOCKET*                      ppSocket,
    char*                            sslCert,
    char*                            sslKey
    );

/**
 * @brief Creates a Event queue to be used for detecting events on sockets
 *
 * @param[in] Handle to library instance.
 * @param[in,optional] iEventQueueSize
 *       specifies the event queue size.
 *       This value can be (-1) to use the default value
 * @param[out] ppQueue Pointer to accept created event queue
 *
 * @return 0 on success
 */
DWORD
VmwSockCreateEventQueue(
    PVMREST_HANDLE                   pRESTHandle,
    int                              iEventQueueSize,
    PVM_SOCK_EVENT_QUEUE*            ppQueue
    );

/**
 * @brief Adds a socket to the event queue
 *
 * @param[in] Handle to library instance.
 * @param[in] pQueue  Pointer to Event queue
 * @param[in] pSocket Pointer to Socket
 *
 * @return 0 on success
 */
DWORD
VmwSockEventQueueAdd(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCK_EVENT_QUEUE             pQueue,
    PVM_SOCKET                       pSocket
    );

/**
 * @brief Waits for an event on the event queue
 *
 * @param[in] Handle to library instance.
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
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCK_EVENT_QUEUE             pQueue,
    int                              iTimeoutMS,
    PVM_SOCKET*                      ppSocket,
    PVM_SOCK_EVENT_TYPE              pEventType,
    PVM_SOCK_IO_BUFFER*              ppIoEvent
    );

/**
 * @brief Closes and frees event queue
 *
 * @param[in] Handle to library instance.
 * @param[in] pQueue Pointer to event queue
 *
 * @return 0 on success
 */

VOID
VmwSockCloseEventQueue(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCK_EVENT_QUEUE             pQueue
    );

/**
 * @brief Reads data from the socket
 *
 * @param[in]     pRESTHandle  Handle to library instance.
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
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket,
    PVM_SOCK_IO_BUFFER               pIoBuffer
    );

/**
 * @brief Writes data to the socket
 *
 * @param[in]     pRESTHandle  Handle to library instance.
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
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket,
    const struct sockaddr*           pClientAddress,
    socklen_t                        addrLength,
    PVM_SOCK_IO_BUFFER               pIoBuffer
    );

/**
 * @brief  Acquires a reference on the socket
 * @param[in] Handle to library instance.
 *
 * @return Pointer to acquired socket
 */

PVM_SOCKET
VmwSockAcquire(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket
    );

/**
 * @brief Releases current reference to socket
 * @param[in] Handle to library instance.
 *
 */
VOID
VmwSockRelease(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket
    );

/**
 * @brief Closes the socket
 *        This call does not release the reference to the socket or free it.
 * @param[in] Handle to library instance.
 */
DWORD
VmwSockClose(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket
    );

DWORD
VmwSockAllocateIoBuffer(
    PVMREST_HANDLE                   pRESTHandle,
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
    PVMREST_HANDLE                   pRESTHandle,
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
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket,
    PVM_STREAM_BUFFER*               ppStreamBuffer
    );

VOID
VmwSockSetStreamBuffer(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket,
    PVM_STREAM_BUFFER                pStreamBuffer
    );

DWORD
VmwSockGetPeerInfo(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket,
    char*                            pIpAddress,
    uint32_t                         nLen,
    int*                             pPortNo
    );

typedef enum
{
    VM_SOCK_PROTOCOL_UNKNOWN = 0,
    VM_SOCK_PROTOCOL_TCP,
    VM_SOCK_PROTOCOL_UDP
} VM_SOCK_PROTOCOL;

typedef enum
{
    VM_SOCK_TYPE_UNKNOWN = 0,
    VM_SOCK_TYPE_CLIENT,
    VM_SOCK_TYPE_SERVER,
    VM_SOCK_TYPE_LISTENER,
    VM_SOCK_TYPE_SIGNAL,
    VM_SOCK_TYPE_TCP_V4,
    VM_SOCK_TYPE_TCP_V6
} VM_SOCK_TYPE;

typedef DWORD (*PFN_OPEN_SERVER_SOCKET)(
                    PVMREST_HANDLE       pRESTHandle,
                    USHORT               usPort,
                    int                  iListenQueueSize,
                    VM_SOCK_CREATE_FLAGS dwFlags,
                    PVM_SOCKET*          ppSocket,
                    char*                sslCert,
                    char*                sslKey
                    );

typedef DWORD (*PFN_CREATE_EVENT_QUEUE)(
                    PVMREST_HANDLE        pRESTHandle,
                    int                   iEventQueueSize,
                    PVM_SOCK_EVENT_QUEUE* ppQueue
                    );

typedef DWORD (*PFN_ADD_EVENT_QUEUE)(
                    PVMREST_HANDLE       pRESTHandle,
                    PVM_SOCK_EVENT_QUEUE pQueue,
                    PVM_SOCKET           pSocket
                    );

typedef DWORD (*PFN_WAIT_FOR_EVENT)(
                    PVMREST_HANDLE       pRESTHandle,
                    PVM_SOCK_EVENT_QUEUE pQueue,
                    int                  iTimeoutMS,
                    PVM_SOCKET*          ppSocket,
                    PVM_SOCK_EVENT_TYPE  pEventType,
                    PVM_SOCK_IO_BUFFER*  ppIoBuffer
                    );

typedef VOID (*PFN_CLOSE_EVENT_QUEUE)(
                    PVMREST_HANDLE       pRESTHandle,
                    PVM_SOCK_EVENT_QUEUE pQueue
                    );

typedef DWORD (*PFN_READ)(
                    PVMREST_HANDLE      pRESTHandle,
                    PVM_SOCKET          pSocket,
                    PVM_SOCK_IO_BUFFER  pIoBuffer
                    );

typedef DWORD (*PFN_WRITE)(
                    PVMREST_HANDLE      pRESTHandle,
                    PVM_SOCKET          pSocket,
                    const struct sockaddr*    pClientAddress,
                    socklen_t           addrLength,
                    PVM_SOCK_IO_BUFFER  pIoBuffer
                    );

typedef PVM_SOCKET (*PFN_ACQUIRE_SOCKET)(
                    PVMREST_HANDLE       pRESTHandle,
                    PVM_SOCKET           pSocket
                    );

typedef VOID (*PFN_RELEASE_SOCKET)(
                    PVMREST_HANDLE       pRESTHandle,
                    PVM_SOCKET           pSocket
                    );

typedef DWORD (*PFN_CLOSE_SOCKET)(
                    PVMREST_HANDLE       pRESTHandle,
                    PVM_SOCKET pSocket
                    );

typedef DWORD (*PFN_ALLOCATE_IO_BUFFER)(
                    PVMREST_HANDLE       pRESTHandle,
                    VM_SOCK_EVENT_TYPE   eventType,
                    DWORD                dwSize,
                    PVM_SOCK_IO_BUFFER*  ppIoBuffer
                    );

typedef VOID(*PFN_RELEASE_IO_BUFFER)(
                    PVMREST_HANDLE       pRESTHandle,
                    PVM_SOCK_IO_BUFFER   pIoBuffer
                    );

typedef VOID(*PFN_GET_STREAM_BUFFER)(
                    PVMREST_HANDLE      pRESTHandle,
                    PVM_SOCKET          pSocket,
                    PVM_STREAM_BUFFER*  ppStreamBuffer
                    );

typedef VOID(*PFN_SET_STREAM_BUFFER)(
                    PVMREST_HANDLE      pRESTHandle,
                    PVM_SOCKET          pSocket,
                    PVM_STREAM_BUFFER   pStreamBuffer
                    );

typedef DWORD(*PFN_GET_PEER_INFO)(
                    PVMREST_HANDLE        pRESTHandle,
                    PVM_SOCKET            pSocket,
                    char*                 pIpAddress,
                    uint32_t              nLen,
                    int*                  pPortNo
                    );

typedef struct _VM_SOCK_PACKAGE
{
    PFN_OPEN_SERVER_SOCKET              pfnOpenServerSocket;
    PFN_CREATE_EVENT_QUEUE              pfnCreateEventQueue;
    PFN_ADD_EVENT_QUEUE                 pfnAddEventQueue;
    PFN_WAIT_FOR_EVENT                  pfnWaitForEvent;
    PFN_CLOSE_EVENT_QUEUE               pfnCloseEventQueue;
    PFN_READ                            pfnRead;
    PFN_WRITE                           pfnWrite;
    PFN_ACQUIRE_SOCKET                  pfnAcquireSocket;
    PFN_RELEASE_SOCKET                  pfnReleaseSocket;
    PFN_CLOSE_SOCKET                    pfnCloseSocket;
    PFN_ALLOCATE_IO_BUFFER              pfnAllocateIoBuffer;
    PFN_RELEASE_IO_BUFFER               pfnReleaseIoBuffer;
    PFN_GET_STREAM_BUFFER               pfnGetStreamBuffer;
    PFN_SET_STREAM_BUFFER               pfnSetStreamBuffer;
    PFN_GET_PEER_INFO                   pfnGetPeerInfo;
} VM_SOCK_PACKAGE, *PVM_SOCK_PACKAGE;
