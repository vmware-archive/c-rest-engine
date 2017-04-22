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

/**
 * @brief  initialize windows socket package
 *
 * @param[in] ppPackage pointer to socket package
 *
 * @return DWORD - 0 on success
 */


DWORD
VmWinSockInitialize(
    PVM_SOCK_PACKAGE* ppPackage
    );

/**
 * @brief  shutdown windows socket package
 *
 */
VOID
VmWinSockShutdown(
    PVM_SOCK_PACKAGE pPackage
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
VmSockWinOpenServer(
    PVMREST_HANDLE       pRESTHandle,
    USHORT               usPort,
    int                  iListenQueueSize,
    VM_SOCK_CREATE_FLAGS dwFlags,
    PVM_SOCKET*          ppSocket,
	char*                sslCert,
	char*                sslKey
    );

/**
 * @brief Creates a Event queue to be used for detecting events on sockets
 *
 * @param[in,optional] iEventQueueSize
 *       specifies the event queue size
 *       This value can be (-1) to use the default value
 * @param[out] ppQueue Pointer to accept created event queue
 *
 * @return 0 on success
 */
DWORD
VmSockWinCreateEventQueue(
    PVMREST_HANDLE          pRESTHandle,
    int                     iEventQueueSize,
    PVM_SOCK_EVENT_QUEUE*   ppQueue
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
VmSockWinEventQueueAdd(
    PVMREST_HANDLE       pRESTHandle,
    PVM_SOCK_EVENT_QUEUE pQueue,
    PVM_SOCKET           pSocket
    );

/**
 * @brief Waits for an event on the event queue
 *
 * @param[in] pQueue   Pointer to event queue
 * @param[in,optional] iTimeoutMS
 *       Timeout in milliseconds
 *       Waits forever if (-1) is passed in.
 * @param[out]    ppSocket   Pointer to socket that has an event
 * @param[in,out] pEventType Event type detected on socket
 *
 * @return 0 on success
 */
DWORD
VmSockWinWaitForEvent(
    PVMREST_HANDLE       pRESTHandle,
    PVM_SOCK_EVENT_QUEUE pQueue,
    int                  iTimeoutMS,
    PVM_SOCKET*          ppSocket,
    PVM_SOCK_EVENT_TYPE  pEventType,
    PVM_SOCK_IO_BUFFER*  ppIoEvent
    );

/**
 * @brief Closes and frees event queue
 *
 * @param[in] pQueue Pointer to event queue
 *
 * @return 0 on success
 */

VOID
VmSockWinCloseEventQueue(
    PVMREST_HANDLE       pRESTHandle,
    PVM_SOCK_EVENT_QUEUE pQueue
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
VmSockWinRead(
    PVMREST_HANDLE      pRESTHandle,
    PVM_SOCKET          pSocket,
    PVM_SOCK_IO_BUFFER  pIoBuffer
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
VmSockWinWrite(
    PVMREST_HANDLE      pRESTHandle,
    PVM_SOCKET          pSocket,
    struct sockaddr*    pClientAddress,
    socklen_t           addrLength,
    PVM_SOCK_IO_BUFFER  pIoBuffer
    );

/**
 * @brief  Acquires a reference on the socket
 *
 * @return Pointer to acquired socket
 */

PVM_SOCKET
VmSockWinAcquire(
    PVMREST_HANDLE      pRESTHandle,
    PVM_SOCKET          pSocket
    );

/**
 * @brief Releases current reference to socket
 *
 */
VOID
VmSockWinRelease(
    PVMREST_HANDLE      pRESTHandle,
    PVM_SOCKET          pSocket
    );

/**
 * @brief Closes the socket
 *        This call does not release the reference to the socket or free it.
 */
DWORD
VmSockWinClose(
    PVMREST_HANDLE      pRESTHandle,
    PVM_SOCKET          pSocket
    );

DWORD
VmSockWinAllocateIoBuffer(
    PVMREST_HANDLE          pRESTHandle,
    VM_SOCK_EVENT_TYPE      eventType,
    DWORD                   dwSize,
    PVM_SOCK_IO_BUFFER*     ppIoContext
    );

/**
 * @brief  Free socket IO Buffer
 *
 * @param[in] pIoBuffer
 *
 * @return VOID - 0 on success
 */
VOID
VmSockWinFreeIoBuffer(
    PVMREST_HANDLE       pRESTHandle,
    PVM_SOCK_IO_BUFFER   pIoBuffer
    );

VOID
VmSockWinGetStreamBuffer(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket,
    PVM_STREAM_BUFFER*               ppStreamBuffer
    );

VOID
VmSockWinSetStreamBuffer(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket,
    PVM_STREAM_BUFFER                pStreamBuffer
    );

uint32_t
VmRESTGetSockPackageWin(
     PVM_SOCK_PACKAGE*               ppSockPackageWin
     );

VOID
VmRESTFreeSockPackageWin(
    PVM_SOCK_PACKAGE                 pSockPackageWin
    );
