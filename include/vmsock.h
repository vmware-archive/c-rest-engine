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
    VM_SOCK_EVENT_TYPE_CONNECTION_TIMEOUT,
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
 * @brief Starts a server socket
 *
 * @param[in] Handle to library instance.
 * @param[in]  dwFlags 32 bit flags defining socket creation preferences
 * @param[out] ppSocket Pointer to created socket
 *
 * @return 0 on success
 */
DWORD
VmwSockStartServer(
    PVMREST_HANDLE                   pRESTHandle,
    VM_SOCK_CREATE_FLAGS             dwFlags,
    PVM_SOCKET*                      ppSocket
    );

/**
 * @brief Creates a Event queue to be used for detecting events on sockets
 *
 * @param[in] Handle to library instance.
 * @param[out] ppQueue Pointer to accept created event queue
 *
 * @return 0 on success
 */
DWORD
VmwSockCreateEventQueue(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCK_EVENT_QUEUE*            ppQueue
    );

/**
 * @brief Add a socket to the event queue
 *
 * @param[in] Handle to library instance.
 * @param[in] pQueue  Pointer to Event queue
 * @param[in] pSocket Pointer to Socket
 *
 * @return 0 on success
 */
DWORD
VmwSockAddEventToQueueInLock(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCK_EVENT_QUEUE             pQueue,
    PVM_SOCKET                       pSocket
    );

/**
 * @brief Deletes a socket from the event queue
 *
 * @param[in] Handle to library instance.
 * @param[in] pQueue  Pointer to Event queue
 * @param[in] pSocket Pointer to Socket
 *
 * @return 0 on success
 */
DWORD
VmwSockDeleteEventFromQueue(
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
    PVM_SOCK_EVENT_TYPE              pEventType
    );

/**
 * @brief Closes and frees event queue
 *
 * @param[in] Handle to library instance.
 * @param[in] pQueue Pointer to event queue
 * @param[in] wait time for clean closure.
 *
 * @return 0 on success
 */
DWORD
VmwSockCloseEventQueue(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCK_EVENT_QUEUE             pQueue,
    uint32_t                         waitSecond
    );

/**
 * @brief Reads data from the socket
 *
 * @param[in]     pRESTHandle  Handle to library instance.
 * @param[in]     pSocket      Pointer to socket
 * @param[in]     ppszBuffer   Pointer to point to data.
 * @param[in]     dwBufSize    Bytes to be read from the buffer
 *
 * @return 0 on success
 */
DWORD
VmwSockRead(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket,
    char**                           ppszBuffer,
    uint32_t*                        nBufLen
    );

/**
 * @brief Writes data to the socket
 *
 * @param[in]     pRESTHandle  Handle to library instance.
 * @param[in]     pSocket      Pointer to socket
 * @param[in]     pszBuffer    Buffer from which bytes have to be written
 * @param[in]     nBufLen      Number of bytes to write from the buffer
 *
 * @return 0 on success
 */
DWORD
VmwSockWrite(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket,
    char*                            pszBuffer,
    uint32_t                         nBufLen
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

/**
 * @brief  shutdown windows socket package
 *
 */
VOID
VmwSockShutdown(
    );

DWORD
VmwSockGetRequestHandle(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket,
    PREST_REQUEST*                   ppRequest
    );

DWORD
VmwSockSetRequestHandle(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket,
    PREST_REQUEST                    pRequest,
    uint32_t                         nProcessed,
    BOOLEAN                          bPersistentConn
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
    VM_SOCK_TYPE_TIMER,
    VM_SOCK_TYPE_TCP_V4,
    VM_SOCK_TYPE_TCP_V6
} VM_SOCK_TYPE;

typedef DWORD (*PFN_START_SERVER_SOCKET)(
                    PVMREST_HANDLE       pRESTHandle,
                    VM_SOCK_CREATE_FLAGS dwFlags,
                    PVM_SOCKET*          ppSocket
                    );

typedef DWORD (*PFN_CREATE_EVENT_QUEUE)(
                    PVMREST_HANDLE        pRESTHandle,
                    PVM_SOCK_EVENT_QUEUE* ppQueue
                    );

typedef DWORD (*PFN_ADD_EVENT_TO_QUEUE)(
                    PVMREST_HANDLE       pRESTHandle,
                    PVM_SOCK_EVENT_QUEUE pQueue,
                    PVM_SOCKET           pSocket
                    );

typedef DWORD (*PFN_DELETE_EVENT_FROM_QUEUE)(
                    PVMREST_HANDLE       pRESTHandle,
                    PVM_SOCK_EVENT_QUEUE pQueue,
                    PVM_SOCKET           pSocket
                    );

typedef DWORD (*PFN_WAIT_FOR_EVENT)(
                    PVMREST_HANDLE       pRESTHandle,
                    PVM_SOCK_EVENT_QUEUE pQueue,
                    int                  iTimeoutMS,
                    PVM_SOCKET*          ppSocket,
                    PVM_SOCK_EVENT_TYPE  pEventType
                    );

typedef DWORD (*PFN_CLOSE_EVENT_QUEUE)(
                    PVMREST_HANDLE       pRESTHandle,
                    PVM_SOCK_EVENT_QUEUE pQueue,
                    uint32_t             waitSecond
                    );

typedef DWORD (*PFN_READ)(
                    PVMREST_HANDLE      pRESTHandle,
                    PVM_SOCKET          pSocket,
                    char**              ppszBuffer,
                    uint32_t*           nBufLen
                    );

typedef DWORD (*PFN_WRITE)(
                    PVMREST_HANDLE      pRESTHandle,
                    PVM_SOCKET          pSocket,
                    char*               pszBuffer,
                    uint32_t            nBufLen
                    );

typedef VOID (*PFN_RELEASE_SOCKET)(
                    PVMREST_HANDLE       pRESTHandle,
                    PVM_SOCKET           pSocket
                    );

typedef DWORD (*PFN_CLOSE_SOCKET)(
                    PVMREST_HANDLE       pRESTHandle,
                    PVM_SOCKET pSocket
                    );

typedef DWORD(*PFN_GET_REQUEST_HANDLE)(
                    PVMREST_HANDLE      pRESTHandle,
                    PVM_SOCKET          pSocket,
                    PREST_REQUEST*      ppRequest
                    );

typedef DWORD(*PFN_SET_REQUEST_HANDLE)(
                    PVMREST_HANDLE        pRESTHandle,
                    PVM_SOCKET            pSocket,
                    PREST_REQUEST         pRequest,
                    uint32_t              nProcessed,
                    BOOLEAN               bPersistentConn
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
    PFN_START_SERVER_SOCKET             pfnStartServerSocket;
    PFN_CREATE_EVENT_QUEUE              pfnCreateEventQueue;
    PFN_ADD_EVENT_TO_QUEUE              pfnAddEventToQueue;
    PFN_DELETE_EVENT_FROM_QUEUE         pfnDeleteEventFromQueue;
    PFN_WAIT_FOR_EVENT                  pfnWaitForEvent;
    PFN_CLOSE_EVENT_QUEUE               pfnCloseEventQueue;
    PFN_READ                            pfnRead;
    PFN_WRITE                           pfnWrite;
    PFN_RELEASE_SOCKET                  pfnReleaseSocket;
    PFN_CLOSE_SOCKET                    pfnCloseSocket;
    PFN_GET_REQUEST_HANDLE              pfnGetRequestHandle;
    PFN_SET_REQUEST_HANDLE              pfnSetRequestHandle;
    PFN_GET_PEER_INFO                   pfnGetPeerInfo;
} VM_SOCK_PACKAGE, *PVM_SOCK_PACKAGE;
