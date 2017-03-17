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


#include "includes.h"

static DWORD
VmSockWinAcceptConnection(
    PVMREST_HANDLE          pRESTHandle,
    PVM_SOCKET              pListenSocket,
    SOCKET                  clientSocket,
    struct sockaddr*        pClientAddress,
    int                     addrLen,
	PVM_SOCKET              pSocket
    );

static VOID
VmSockWinFreeSocket(
    PVM_SOCKET              pSocket
    );

static
uint32_t
VmRESTSecureSocket(
    PVMREST_HANDLE                   pRESTHandle,
    char*                            certificate,
    char*                            key
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    int                              ret = 0;
	long                             options = 0;
    const SSL_METHOD*                method = NULL;
    SSL_CTX*                         context = NULL;

    if (key == NULL || certificate == NULL)
    {
        VMREST_LOG_ERROR(pRESTHandle,"%s","Invalid params");
        dwError = VMREST_TRANSPORT_INVALID_PARAM;
    }
    BAIL_ON_VMREST_ERROR(dwError);

	SSL_library_init();
    SSL_load_error_strings();
    SSLeay_add_ssl_algorithms();
    method = SSLv23_server_method();
    context = SSL_CTX_new (method);
    if (!context) 
	{
		dwError = VMREST_TRANSPORT_SSL_CONFIG_ERROR;
        VMREST_LOG_ERROR(pRESTHandle,"%s","SSL Context NULL");
    }
	BAIL_ON_VMREST_ERROR(dwError);

	options = SSL_CTX_get_options(context);

    options = options | SSL_OP_NO_TLSv1|SSL_OP_NO_SSLv3|SSL_OP_NO_SSLv2;

    options = SSL_CTX_set_options(context, options);

    ret = SSL_CTX_set_cipher_list(context, "!aNULL:kECDH+AESGCM:ECDH+AESGCM:RSA+AESGCM:kECDH+AES:ECDH+AES:RSA+AES");
    if (ret == 0)
    {
        VMREST_LOG_ERROR(pRESTHandle,"%s","SSL_CTX_set_cipher_list() : Cannot apply security approved cipher suites");
        dwError = VMREST_TRANSPORT_SSL_INVALID_CIPHER_SUITES;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    if (SSL_CTX_use_certificate_file(context, certificate, SSL_FILETYPE_PEM) <= 0) 
	{
         dwError = VMREST_TRANSPORT_SSL_CERTIFICATE_ERROR;
         VMREST_LOG_ERROR(pRESTHandle,"%s","SSL Certificate cannot be used");
    }
	BAIL_ON_VMREST_ERROR(dwError);

    if (SSL_CTX_use_PrivateKey_file(context, key, SSL_FILETYPE_PEM) <= 0)
	{
        dwError = VMREST_TRANSPORT_SSL_PRIVATEKEY_ERROR;
        VMREST_LOG_ERROR(pRESTHandle,"%s","SSL key cannot be used");
    }
	BAIL_ON_VMREST_ERROR(dwError);

    if (!SSL_CTX_check_private_key(context)) 
	{
        dwError = VMREST_TRANSPORT_SSL_PRIVATEKEY_CHECK_ERROR;
        VMREST_LOG_ERROR(pRESTHandle,"%s","SSL Error in private key");
	}
	BAIL_ON_VMREST_ERROR(dwError);

	pRESTHandle->pSSLInfo->sslContext = context;
	
cleanup:
    return dwError;

error:
    dwError = VMREST_TRANSPORT_SSL_ERROR;
    goto cleanup;
}


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
    )
{
    DWORD dwError = 0;
	

    union
    {
#ifdef AF_INET6
        struct sockaddr_in6 servaddr_ipv6;
#endif
        struct sockaddr_in  servaddr_ipv4;
    } servaddr;
    struct
    {
        int domain;
        int type;
        int protocol;
    } socketParams;
    struct sockaddr* pSockAddr = NULL;
    socklen_t addrLen = 0;
    SOCKET socket = INVALID_SOCKET;
    PVM_SOCKET pSocket = NULL;
    DWORD dwSockFlags = 0;
	PVM_SOCK_SSL_INFO                pSSLInfo = NULL;

    if (dwFlags & VM_SOCK_CREATE_FLAGS_IPV6)
    {
#ifdef AF_INET6
        socketParams.domain = AF_INET6;
#else
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_VMREST_ERROR(dwError);
#endif
    }
    else
    {
        socketParams.domain = AF_INET;
    }

    if (dwFlags & VM_SOCK_CREATE_FLAGS_UDP)
    {
        socketParams.type = SOCK_DGRAM;
    }
    else
    {
        socketParams.type = SOCK_STREAM;
    }

    socketParams.protocol = 0;

    if (dwFlags & VM_SOCK_CREATE_FLAGS_NON_BLOCK)
    {
        dwSockFlags = WSA_FLAG_OVERLAPPED;
    }

	pSSLInfo = pRESTHandle->pSSLInfo;

    /**** Check if connection is over SSL ****/
    if(dwFlags & VM_SOCK_IS_SSL)
    {
		//sslCert = "./MYCERT.crt";
		//sslKey = "./MYKEY.key";
        dwError = VmRESTSecureSocket(
			          pRESTHandle,
                      sslCert,
                      sslKey
                      );
        BAIL_ON_VMREST_ERROR(dwError);
        pSSLInfo->isSecure = 1;
    }
    else
    {
        pSSLInfo->isSecure = 0;
    }

    socket = WSASocketW(
                    socketParams.domain,
                    socketParams.type,
                    socketParams.protocol,
                    NULL,
                    0,
                    dwSockFlags);
    if (socket == INVALID_SOCKET)
    {
        dwError = WSAGetLastError();
        BAIL_ON_VMREST_ERROR(dwError);
    }

    memset(&servaddr, 0, sizeof(servaddr));

    if (dwFlags & VM_SOCK_CREATE_FLAGS_IPV6)
    {
#ifdef AF_INET6
        servaddr.servaddr_ipv6.sin6_family = AF_INET6;
        servaddr.servaddr_ipv6.sin6_addr = in6addr_any;
        servaddr.servaddr_ipv6.sin6_port = htons(usPort);

        pSockAddr = (struct sockaddr*) &servaddr.servaddr_ipv6;
        addrLen = sizeof(servaddr.servaddr_ipv6);
#else
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_VMREST_ERROR(dwError);
#endif
    }
    else
    {
        servaddr.servaddr_ipv4.sin_family = AF_INET;
        servaddr.servaddr_ipv4.sin_addr.s_addr = htonl(INADDR_ANY);
        servaddr.servaddr_ipv4.sin_port = htons(usPort);

        pSockAddr = (struct sockaddr*) &servaddr.servaddr_ipv4;
        addrLen = sizeof(servaddr.servaddr_ipv4);
    }

    if (bind(socket, pSockAddr, addrLen) < 0)
    {
        dwError = WSAGetLastError();
        BAIL_ON_VMREST_ERROR(dwError);
    }

    if (!(dwFlags & VM_SOCK_CREATE_FLAGS_UDP))
    {
        if (iListenQueueSize <= 0)
        {
            iListenQueueSize = VM_SOCK_WINDOWS_DEFAULT_LISTEN_QUEUE_SIZE;
        }

        if (listen(socket, iListenQueueSize) < 0)
        {
            dwError = WSAGetLastError();
            BAIL_ON_VMREST_ERROR(dwError);
        }
    }

    dwError = VmRESTAllocateMemory(sizeof(*pSocket), (PVOID*)&pSocket);
    BAIL_ON_VMREST_ERROR(dwError);

    pSocket->pStreamBuffer = NULL;
	pSocket->ssl = NULL;
    pSocket->refCount = 1;
	pSocket->wThrCnt = iListenQueueSize;
    pSocket->type = VM_SOCK_TYPE_LISTENER;

    if (dwFlags & VM_SOCK_CREATE_FLAGS_UDP)
    {
        pSocket->protocol = VM_SOCK_PROTOCOL_UDP;
    }
    else
    {
        pSocket->protocol = VM_SOCK_PROTOCOL_TCP;
    }

	if (dwFlags & VM_SOCK_CREATE_FLAGS_IPV6)
    {
		pSocket->v4v6 = VM_SOCK_TYPE_TCP_V6;
    }
    else if (dwFlags & VM_SOCK_CREATE_FLAGS_IPV4)
    {
        pSocket->v4v6 = VM_SOCK_TYPE_TCP_V4;
    }
    else
    {
		pSocket->v4v6 = VM_SOCK_TYPE_UNKNOWN; 
    }

    pSocket->hSocket = socket;
    socket = INVALID_SOCKET;

    *ppSocket = pSocket;
	 
cleanup:
    
    return dwError;

error:

    if (ppSocket)
    {
        *ppSocket = NULL;
    }

    if (pSocket)
    {
        VmSockWinFreeSocket(pSocket);
    }
    if (socket != INVALID_SOCKET)
    {
        closesocket(socket);
    }

    goto cleanup;
}

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
VmSockWinCreateEventQueue(
    PVMREST_HANDLE          pRESTHandle,
    int                     iEventQueueSize,
    PVM_SOCK_EVENT_QUEUE*   ppQueue
    )
{
    DWORD dwError = 0;
    int sockError = 0;
    PVM_SOCK_EVENT_QUEUE pQueue = NULL;

    if (!ppQueue || !pRESTHandle)
    {
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMREST_ERROR(dwError);
    }

    if (iEventQueueSize <= 0)
    {
        iEventQueueSize = VM_SOCK_WINDOWS_DEFAULT_QUEUE_SIZE;
    }

    dwError = VmRESTAllocateMemory(sizeof(*pQueue), (PVOID*)&pQueue);
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTAllocateMutex(&pQueue->pMutex);
    BAIL_ON_VMREST_ERROR(dwError);

    pQueue->hIOCP = CreateIoCompletionPort(
                                INVALID_HANDLE_VALUE,
                                NULL,
                                0,
                                0);
    if (!pQueue->hIOCP)
    {
        dwError = GetLastError();
        BAIL_ON_VMREST_ERROR(dwError);
    }

    pQueue->hEventListen = WSACreateEvent();
    if (pQueue->hEventListen == WSA_INVALID_EVENT)
    {
        dwError = WSAGetLastError();
        BAIL_ON_VMREST_ERROR(dwError);
    }

    *ppQueue = pQueue;
	pRESTHandle->pSSLInfo->isQueueInUse = 1;

cleanup:

    return dwError;

error:

    if (ppQueue)
    {
        *ppQueue = NULL;
    }

    VmSockWinCloseEventQueue(pRESTHandle, pQueue);
	if (pQueue && pQueue->pMutex)
	{
        VmRESTFreeMemory(pQueue->pMutex);
	}
	if (pQueue)
	{
        VmRESTFreeMemory(pQueue);
	}

    goto cleanup;
}

DWORD
VmSockWinEventQueueAdd(
    PVMREST_HANDLE       pRESTHandle,
    PVM_SOCK_EVENT_QUEUE pQueue,
    PVM_SOCKET           pSocket
    )
{
    DWORD   dwError = 0;
    BOOLEAN bLocked = TRUE;
    int sockError = 0;
    HANDLE hTemp = NULL;

    if (!pQueue || !pSocket || pSocket->hSocket == INVALID_SOCKET)
    {
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMREST_ERROR(dwError);
    }

    hTemp = CreateIoCompletionPort(
                        (HANDLE)pSocket->hSocket,
                        pQueue->hIOCP,
                        0,
                        0);
    if (hTemp != pQueue->hIOCP)
    {
        dwError = GetLastError();
        BAIL_ON_VMREST_ERROR(dwError);
    }

    sockError = WSAEventSelect(
                        pSocket->hSocket,
                        pQueue->hEventListen,
                        FD_ACCEPT);
    BAIL_ON_VMREST_ERROR(dwError);

	if ((pSocket->v4v6 == VM_SOCK_TYPE_TCP_V4) && (pSocket->type == VM_SOCK_TYPE_LISTENER))
	{
		pQueue->pListenerTCPv4 = pSocket;
		pQueue->thrCnt = pSocket->wThrCnt;
	}
	else if ((pSocket->v4v6 == VM_SOCK_TYPE_TCP_V6) && (pSocket->type == VM_SOCK_TYPE_LISTENER))
	{
		pQueue->pListenerTCPv6 = pSocket;
		pQueue->thrCnt = pSocket->wThrCnt;
	}

	pSocket->pEventQueue = pQueue;

cleanup:

    return dwError;

error:

    goto cleanup;
}

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
VmSockWinWaitForEvent(
	PVMREST_HANDLE       pRESTHandle,				    
    PVM_SOCK_EVENT_QUEUE pQueue,
    int                  iTimeoutMS,
    PVM_SOCKET*          ppSocket,
    PVM_SOCK_EVENT_TYPE  pEventType,
    PVM_SOCK_IO_BUFFER*  ppIoBuffer
    )
{
    DWORD                dwError = 0;
    PVM_SOCKET           pListenSocket = NULL;
    WSANETWORKEVENTS     events = { 0 };
    int                  socketError = 0;
    SOCKET               clientSocket = INVALID_SOCKET;
    int                  nAddrLen = -1;
	PVM_SOCKET           pSocket = NULL;
	BOOLEAN              blocked = FALSE;
	uint32_t             freeEventQueue = 0;

    if (!pQueue)
    {
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMREST_ERROR(dwError);
    }

	if (pQueue->pListenerTCPv4 != NULL)
	{
		pListenSocket = pQueue->pListenerTCPv4;
	}
	else if(pQueue->pListenerTCPv6 != NULL)
	{
		pListenSocket = pQueue->pListenerTCPv6;
	}

	if (!pListenSocket->pEventQueue)
	{
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMREST_ERROR(dwError);
	}

    dwError = VmRESTLockMutex(pQueue->pMutex);
    BAIL_ON_VMREST_ERROR(dwError);

	blocked = TRUE;

    dwError = WSAWaitForMultipleEvents(
                  1,
                  &pQueue->hEventListen,
                  FALSE,
                  100,
                  FALSE);
    if (dwError == WSA_WAIT_TIMEOUT)
    {
        dwError = 0;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    if (pQueue->bShutdown)
    {
		pQueue->thrCnt--;
		if (pQueue->thrCnt == 0)
        {
            freeEventQueue = 1;
        }
		dwError = ERROR_SHUTDOWN_IN_PROGRESS;
		BAIL_ON_VMREST_ERROR(dwError);
    }

    socketError = WSAEnumNetworkEvents(
                      pListenSocket->hSocket,
                      pQueue->hEventListen,
                      &events);
    if (socketError == SOCKET_ERROR)
    {
        BAIL_ON_VMREST_ERROR(WSAGetLastError());
    }

    if (events.lNetworkEvents & FD_ACCEPT)
    {
        if (events.iErrorCode[FD_ACCEPT_BIT] == 0 && !pQueue->bShutdown)
        {
            struct sockaddr_storage clientAddress = { 0 };
            int addLen = sizeof clientAddress;
            clientSocket = accept( pListenSocket->hSocket,
                                       (struct sockaddr*)&clientAddress,
                                        &addLen);
            if (clientSocket == SOCKET_ERROR)
            {
                BAIL_ON_VMREST_ERROR(WSAGetLastError());
            }
            else
            {
                dwError = VmSockWinAcceptConnection(
					          pRESTHandle,
                              pListenSocket,
                              clientSocket,
                              (struct sockaddr*)&clientAddress,
                              addLen,
						      &pSocket);
                BAIL_ON_VMREST_ERROR(WSAGetLastError());
            }
        }
        else
        {
            BAIL_ON_VMREST_ERROR(WSAGetLastError());
        }
    }
    VmRESTUnlockMutex(pQueue->pMutex);
	blocked = FALSE;
	
    *ppSocket = pSocket;
    *pEventType = VM_SOCK_EVENT_TYPE_TCP_NEW_CONNECTION;
    *ppIoBuffer = NULL;

cleanup:

	if (dwError == ERROR_SHUTDOWN_IN_PROGRESS && freeEventQueue == 1)
    {
        if (pQueue->hEventListen != WSA_INVALID_EVENT)
        {
            WSACloseEvent(pQueue->hEventListen);
        }

        if (pQueue->hIOCP)
        {
            CloseHandle(pQueue->hIOCP);
        
		}
        if (pQueue && pQueue->pMutex)
	    {
            VmRESTFreeMemory(pQueue->pMutex);
	    }
	    if (pQueue)
	    {
            VmRESTFreeMemory(pQueue);
	    }
       pRESTHandle->pSSLInfo->isQueueInUse = 0;
    }

    return dwError;

error:

	if (blocked == TRUE && pQueue != NULL)
	{
        VmRESTUnlockMutex(pQueue->pMutex);
	}
    goto cleanup;
}

/**
 * @brief Closes and frees event queue
 *
 * @param[in] pQueue Pointer to event queue
 *
 * @return 0 on success
 */

VOID
VmSockWinCloseEventQueue(
    PVMREST_HANDLE        pRESTHandle,
    PVM_SOCK_EVENT_QUEUE  pQueue
    )
{
	uint32_t             retry = 0;
    if (pQueue)
    {
		pQueue->bShutdown = 1;
    }
	/**** Worker threads are detached threads, give them some time for cleanup. Block upto 10 seconds *****/

    while(retry < 10)
    {
        if (pRESTHandle->pSSLInfo->isQueueInUse == 0)
        {
           break;
        }
        Sleep(1000);
        retry++;
    }
}

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
    )
{
    DWORD               dwError = 0;
    int                 sockError = 0;
    DWORD               dwBytesRead = 0;
    DWORD               dwFlags = 0;
	DWORD               dwBufSize = 0;
	char*               buffer = NULL;
	int                 errorCode = 0;
	unsigned int        tryCnt = 0;
	unsigned int        maxTry = 500000;

    if (!pSocket || !pIoBuffer)
    {
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMREST_ERROR(dwError);
    }

    if (pIoBuffer->dwExpectedSize <= pIoBuffer->dwCurrentSize)
    {
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMREST_ERROR(dwError);
    }

    dwBufSize = pIoBuffer->dwExpectedSize - pIoBuffer->dwCurrentSize;
    buffer = pIoBuffer->pData + pIoBuffer->dwCurrentSize;

tryAgain:
    if (pSocket->protocol == VM_SOCK_PROTOCOL_TCP)
    {
         if (pRESTHandle->pSSLInfo->isSecure && (pSocket->ssl != NULL))
         {
             sockError = SSL_read(pSocket->ssl, buffer, dwBufSize);
			 errorCode = SSL_get_error(pSocket->ssl, sockError);
			 if ( sockError < 0 && errorCode == SSL_ERROR_WANT_READ && tryCnt < maxTry)
			 {
                  tryCnt++;
				  goto tryAgain;
			 } 
			 else if (sockError < 0)
			 {
                 dwError = errorCode;
                 VMREST_LOG_ERROR(pRESTHandle,"SSL read failed, sockError = %u", dwError);
			     BAIL_ON_VMREST_ERROR(dwError);
			 }
			 dwBytesRead = sockError;
         }
		 else if(pSocket->hSocket > 0)
		 {
             sockError = recv(pSocket->hSocket, buffer, dwBufSize,dwFlags);
			 errorCode = WSAGetLastError();
			 if ( sockError < 0  && tryCnt < maxTry)
			 {
                  tryCnt++;
				  goto tryAgain;
			 } 
			 else if (sockError < 0)
			 {
                 dwError = errorCode;
				 VMREST_LOG_ERROR(pRESTHandle,"Socket %u read failed with errorCode %d", pSocket->hSocket, errorCode);
			     BAIL_ON_VMREST_ERROR(dwError);
			 }
			 dwBytesRead = sockError;
        }
	}

	pIoBuffer->dwCurrentSize = dwBytesRead;
	pIoBuffer->dwBytesTransferred += dwBytesRead;

cleanup:

    return dwError;

error:

    goto cleanup;
}
/**
 * @brief Writes data to the socket
 *
 * @param[in]     pSocket      Pointer to socket
 * @param[in]     pBuffer      Buffer from which bytes have to be written
 * @param[in]     dwBufLen     Number of bytes to write from the buffer
 * @param[in,out] pdwBytesWrtten Number of bytes written to the socket
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
    )
{
    DWORD               dwError = 0;
    DWORD               dwBytesWritten = 0;
    DWORD               dwFlags = 0;
	DWORD               dwBytesToWrite = 0;
	DWORD               bytes = 0;
	DWORD               bytesLeft = 0;
	int                 bytesWritten = 0;
	char*               buffer = NULL;
	int                 errorCode = 0;

    if (!pSocket || !pIoBuffer)
    {
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMREST_ERROR(dwError);
    }

    if (pIoBuffer->dwExpectedSize <= pIoBuffer->dwCurrentSize)
    {
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMREST_ERROR(dwError);
    }

	dwBytesToWrite = pIoBuffer->dwExpectedSize;
	bytesLeft = dwBytesToWrite;
	buffer = pIoBuffer->pData + pIoBuffer->dwCurrentSize;

    while(bytesWritten < dwBytesToWrite)
    {
        if (pRESTHandle->pSSLInfo->isSecure && (pSocket->ssl != NULL))
        {    
            dwBytesWritten = SSL_write(pSocket->ssl,(pIoBuffer->pData + bytesWritten),bytesLeft);
		    errorCode = SSL_get_error(pSocket->ssl, dwBytesWritten);
        }
	    else if (pSocket->hSocket > 0)
		{
	        dwBytesWritten = send(pSocket->hSocket, (buffer + bytesWritten), bytesLeft, dwFlags);
		    errorCode = WSAGetLastError();    
		}

        if (dwBytesWritten >= 0)
        {
            bytesWritten += dwBytesWritten;
            bytesLeft -= dwBytesWritten;
           VMREST_LOG_DEBUG(pRESTHandle,"Bytes written this write %d, Total bytes written %u", dwBytesWritten, bytesWritten);
            dwBytesWritten = 0;
        }
        else
        {
            if (errorCode == 11)
            {
                dwBytesWritten = 0;
                continue;
            }
           VMREST_LOG_ERROR(pRESTHandle,"Write failed with Error Code %d", errorCode);
            dwError = errorCode;
            BAIL_ON_VMREST_ERROR(dwError);
        }
	}
    
    pIoBuffer->dwBytesTransferred = bytesWritten;
    pIoBuffer->dwCurrentSize += bytesWritten;

cleanup:

    return dwError;

error:

    goto cleanup;
}
/**
 * @brief  Acquires a reference on the socket
 *
 * @return Pointer to acquired socket
 */

PVM_SOCKET
VmSockWinAcquire(
    PVMREST_HANDLE       pRESTHandle,
    PVM_SOCKET           pSocket
    )
{
    if (pSocket)
    {
        InterlockedIncrement(&pSocket->refCount);
    }

    return pSocket;
}

/**
 * @brief Releases current reference to socket
 *
 */
VOID
VmSockWinRelease(
    PVMREST_HANDLE       pRESTHandle,
    PVM_SOCKET           pSocket
    )
{
    if (pSocket)
    {
        if (InterlockedDecrement(&pSocket->refCount) == 0)
        {
            VmSockWinFreeSocket(pSocket);
        }
    }
}

/**
 * @brief Closes the socket
 *        This call does not release the reference to the socket or free it.
 */
DWORD
VmSockWinClose(
    PVMREST_HANDLE       pRESTHandle,
    PVM_SOCKET           pSocket
    )
{
   VMREST_LOG_DEBUG(pRESTHandle,"Close Connectiom - Socket: %d", (DWORD)pSocket->hSocket);

    if (pSocket->hSocket != INVALID_SOCKET)
    {
        if (pRESTHandle->pSSLInfo->isSecure)
        {
            if (pSocket->ssl)
            {
                SSL_shutdown(pSocket->ssl);
                SSL_free(pSocket->ssl);
                pSocket->ssl = NULL;
            }
        }
		shutdown(pSocket->hSocket, 2);
        closesocket(pSocket->hSocket);
        pSocket->hSocket = INVALID_SOCKET;
    }

    return 0;
}

static VOID
VmSockWinFreeSocket(
    PVM_SOCKET  pSocket
    )
{   
    if (pSocket->hSocket != INVALID_SOCKET)
    {
        CancelIo((HANDLE)pSocket->hSocket);
        closesocket(pSocket->hSocket);
        pSocket->hSocket = INVALID_SOCKET;
    }
    if (pSocket->pStreamBuffer)
    {
        VmRESTFreeMemory(pSocket->pStreamBuffer);
    }
    if(pSocket != NULL)
	{
        VmRESTFreeMemory(pSocket);
	}
	
}

static VOID
VmSockWinDisconnectSocket(
    SOCKET clientSocket
    )
{
    LINGER sockopt = { 0 };

    sockopt.l_onoff = 1;
    sockopt.l_linger = 0;

    setsockopt(clientSocket, SOL_SOCKET, SO_LINGER, (char*)&sockopt, sizeof(sockopt));

    CancelIo((HANDLE)clientSocket);
    closesocket(clientSocket);
}

DWORD
VmSockWinAcceptConnection(
    PVMREST_HANDLE          pRESTHandle,
    PVM_SOCKET              pListenSocket,
    SOCKET                  clientSocket,
    struct sockaddr*        pClientAddr,
    int                     addrlen,
	PVM_SOCKET*             ppSocket
    )
{
    DWORD                   dwError = 0;
    HANDLE                  hTemp = NULL;
    const char              chOpt = 1;
    PVM_SOCKET              pClientSocket = NULL;
    PVM_STREAM_BUFFER       pStrmBuf = NULL;
	SSL*                    ssl = NULL;
	DWORD                   cntRty = 0;
	int                     err = 0;


    if (!pListenSocket ||
        !pListenSocket->hSocket ||
        !pListenSocket->pEventQueue ||
        !pListenSocket->pEventQueue->hIOCP
        || clientSocket == INVALID_SOCKET)
    {
        dwError = ERROR_INVALID_SERVER_STATE;
        BAIL_ON_VMREST_ERROR(dwError);
    }

	dwError = VmRESTAllocateMemory(
                    sizeof(VM_SOCKET),
                    (void **)&pClientSocket);
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTAllocateMemory(
                  sizeof(VM_STREAM_BUFFER),
                  (void**)&pStrmBuf
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    if (pRESTHandle->pSSLInfo->isSecure)
	{
        ssl = SSL_new(pRESTHandle->pSSLInfo->sslContext);
		if (!ssl)
		{
            dwError = VMREST_TRANSPORT_SSL_ACCEPT_FAILED;
		}
        BAIL_ON_VMREST_ERROR(dwError);

        SSL_set_fd(ssl, clientSocket);

retry:
		err = SSL_accept (ssl);
		if (err == -1)
		{
             if (cntRty <= 500000)
			 {
                 cntRty++;
				 goto retry;
			 }
			 else
			 {
                 VMREST_LOG_ERROR(pRESTHandle,"SSL Accept failed dwError = %d error code %d", err, SSL_get_error(ssl,err));
			      dwError = 101;
				  BAIL_ON_VMREST_ERROR(dwError);
			 }
		}
		pClientSocket->ssl = ssl;
	}
	else
	{
        pClientSocket->ssl = NULL;
	}
          
    pStrmBuf->dataProcessed = 0;
    pStrmBuf->dataRead = 0;
    memset(pStrmBuf->pData, '\0', 4096);

    pClientSocket->pStreamBuffer = pStrmBuf;
    pClientSocket->hSocket = clientSocket;
    pClientSocket->pEventQueue = pListenSocket->pEventQueue;
    pClientSocket->protocol = pListenSocket->protocol;
    pClientSocket->type = VM_SOCK_TYPE_SERVER;
    memcpy_s(&pClientSocket->addr, sizeof pClientSocket->addr, pClientAddr, addrlen);
    pClientSocket->addrLen = addrlen;
    pClientSocket->refCount = 1;


    clientSocket = INVALID_SOCKET;

    if (setsockopt(
                pClientSocket->hSocket,
                IPPROTO_TCP,
                TCP_NODELAY,
                &chOpt,
                sizeof(char)))
    {
        dwError = WSAGetLastError();
        BAIL_ON_VMREST_ERROR(dwError);
    }

    *ppSocket = pClientSocket;

cleanup:

    return dwError;

error:

    if (pClientSocket)
    {
        VmSockWinFreeSocket(pClientSocket);
    }

    goto cleanup;
}

DWORD
VmSockWinAllocateIoBuffer(
    PVMREST_HANDLE          pRESTHandle,
    VM_SOCK_EVENT_TYPE      eventType,
    DWORD                   dwSize,
    PVM_SOCK_IO_BUFFER*     ppIoBuffer
    )
{
    DWORD dwError = 0;
    PVM_SOCK_IO_CONTEXT pIoContext = NULL;

    if (!ppIoBuffer)
    {
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMREST_ERROR(dwError);
    }

    dwError = VmRESTAllocateMemory(sizeof(VM_SOCK_IO_CONTEXT) + dwSize, (PVOID*)&pIoContext);
    BAIL_ON_VMREST_ERROR(dwError);

    pIoContext->eventType = eventType;
    pIoContext->IoBuffer.dwExpectedSize = dwSize;
    pIoContext->IoBuffer.pData = pIoContext->DataBuffer;
    *ppIoBuffer = &(pIoContext->IoBuffer);

cleanup:

    return dwError;

error:

    if (ppIoBuffer)
    {
        *ppIoBuffer = NULL;
    }

    if (pIoContext)
    {
        VmSockWinFreeIoBuffer(pRESTHandle, &pIoContext->IoBuffer);
    }

    goto cleanup;
}

VOID
VmSockWinFreeIoBuffer(
    PVMREST_HANDLE         pRESTHandle,
    PVM_SOCK_IO_BUFFER     pIoBuffer
    )
{
    PVM_SOCK_IO_CONTEXT pIoContext = CONTAINING_RECORD(pIoBuffer, VM_SOCK_IO_CONTEXT, IoBuffer);
	if (pIoContext)
	{
	    VmRESTFreeMemory(pIoContext);
	}
}

VOID
VmSockWinGetStreamBuffer(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket,
    PVM_STREAM_BUFFER*               ppStreamBuffer
    )
{
    if (pSocket->pStreamBuffer)
    {
        *ppStreamBuffer = pSocket->pStreamBuffer;
    }
    else
    {
        *ppStreamBuffer = NULL;
    }
}

VOID
VmSockWinSetStreamBuffer(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket,
    PVM_STREAM_BUFFER                pStreamBuffer
    )
{
    if (pStreamBuffer)
    {
        pSocket->pStreamBuffer = pStreamBuffer;
    }
    else
    {
        pSocket->pStreamBuffer = NULL;
    }
}
