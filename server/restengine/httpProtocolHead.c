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

#include "includes.h"

BOOLEAN
VmRESTIsValidHTTPMethod(
    char*                            pszMethod
    )
{
    int                              i = 0;
    const char*                      validMethodTable[HTTP_VALID_METHODS_COUNT]=\
                                     { "GET",
                                       "PUT",
                                       "POST",
                                       "DELETE",
                                       "OPTIONS",
                                       "HEAD",
                                       "CONNECT",
                                       "PATCH"};

    if (!pszMethod || (strlen(pszMethod) > MAX_METHOD_LEN))
    {
        return FALSE;
    }


    for (i = 0; i < HTTP_VALID_METHODS_COUNT; i++)
    {
        if ((strncmp(pszMethod, validMethodTable[i], MAX_METHOD_LEN) == 0) && (strlen(pszMethod) == strlen(validMethodTable[i])))
        {
            return TRUE;
        }
    }
    return FALSE;
}

BOOLEAN
VmRESTIsValidHTTPVesion(
   char*                             pszVersion
   )
{
    size_t                           nLen;

    if (!pszVersion)
    {
        return FALSE;
    }

    nLen = strlen(pszVersion);

    if (nLen == 0 || nLen > MAX_VERSION_LEN)
    {
        return FALSE;
    }

    if (((strncmp(pszVersion, "HTTP/1.1", HTTP_VER_LEN) == 0) || (strncmp(pszVersion, "HTTP/1.0", HTTP_VER_LEN) == 0)) && (nLen == (MAX_VERSION_LEN - 1)))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

uint32_t
VmRESTSetPayloadType(
    PREST_REQUEST                    pRequest
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    char*                            pszContentLen = NULL;
    char*                            pszTransferEncoding = NULL;

    if (!pRequest)
    {
        dwError = REST_ERROR_INVALID_HANDLER;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTGetHttpHeader(
                  pRequest,
                  HTTP_HEADER_STR_CONTENT_LENGTH,
                  &pszContentLen
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTGetHttpHeader(
                  pRequest,
                  HTTP_HEADER_STR_TRANSFER_ENCODING,
                  &pszTransferEncoding
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    if (pszContentLen && !pszTransferEncoding && (strlen(pszContentLen) > 0))
    {
        if (pRequest->payloadType == HTTP_PAYLOAD_TYPE_INVALID)
        {
             pRequest->payloadType = HTTP_PAYLOAD_CONTENT_LENGTH;
             pRequest->dataRemaining = strtoul(pszContentLen, NULL, 10);
        }
    }
    else if (pszTransferEncoding && !pszContentLen && strstr(pszTransferEncoding,"chunked"))
    {
        if (pRequest->payloadType == HTTP_PAYLOAD_TYPE_INVALID)
        {
            pRequest->payloadType = HTTP_PAYLOAD_TRANSFER_ENCODING;
            pRequest->dataRemaining = 0;
        }
    }
    else if ((!pszContentLen && !pszTransferEncoding))
    {
        /**** Assume no length of data info is present in headers ****/
        if (pRequest->payloadType == HTTP_PAYLOAD_TYPE_INVALID)
        {
             pRequest->payloadType = HTTP_PAYLOAD_CONTENT_LENGTH;
             pRequest->dataRemaining = 0;
        }
    }
    else
    {
        dwError = LENGTH_REQUIRED;
    }
    BAIL_ON_VMREST_ERROR(dwError);
    

cleanup:

    if (pszContentLen != NULL)
    {
        VmRESTFreeMemory(pszContentLen);
        pszContentLen = NULL;
    }

    if (pszTransferEncoding != NULL)
    {
        VmRESTFreeMemory(pszTransferEncoding);
        pszTransferEncoding = NULL;
    }

    return dwError;

error:

    goto cleanup;

}

uint32_t
VmRESTHandleExpect(
    PVMREST_HANDLE                   pRESTHandle,
    PREST_REQUEST                    pRequest
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    char*                            pszExpect = NULL;
    char*                            pszHttpURI = NULL;
    char*                            pszEndPointURI = NULL;
    PREST_ENDPOINT                   pEndPoint = NULL;
    uint32_t                         nWrite = 0;
    PREST_RESPONSE                   pIntResPacket = NULL;

    if (!pRequest || !pRESTHandle)
    {
        dwError = REST_ERROR_INVALID_HANDLER;
    }
    BAIL_ON_VMREST_ERROR(dwError);


    /**** If Expect:100-continue is received, send the continue message back to client ****/
    dwError = VmRESTGetHttpHeader(
                  pRequest,
                  HTTP_HEADER_STR_EXPECT,
                  &pszExpect
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    if (pszExpect && strstr(pszExpect, "100-continue"))
    {
        /**** Do not send 100-continue for invalid URI ****/
        VMREST_LOG_DEBUG(pRESTHandle,"%s","Expect:100-Continue header received, processing.....");

        if (pRESTHandle->pInstanceGlobal->useEndPoint == 1)
        {
            dwError = VmRESTGetHttpURI(
                          pRequest,
                          TRUE,
                          &pszHttpURI
                          );
            BAIL_ON_VMREST_ERROR(dwError);

            if (pszHttpURI)
            {
                dwError = VmRestGetEndPointURIfromRequestURI(
                              pszHttpURI,
                              &pszEndPointURI
                              );
                BAIL_ON_VMREST_ERROR(dwError);
            }

            /**** For bad endpoint, this will return error ****/
            dwError = VmRestEngineGetEndPoint(
                          pRESTHandle,
                          pszEndPointURI,
                          &pEndPoint
                          );
            BAIL_ON_VMREST_ERROR(dwError);
        }

        dwError = VmRESTAllocateHTTPResponsePacket(
                      &pIntResPacket
                      );
        BAIL_ON_VMREST_ERROR(dwError);

        pIntResPacket->miscHeader->head = NULL;
        pIntResPacket->pSocket = pRequest->pSocket;
        pIntResPacket->requestPacket = pRequest;
        pIntResPacket->bHeaderSent = FALSE;

        dwError = VmRESTSetFailureResponse(
                      &pIntResPacket,
                      HTTP_STATUSCODE_STR_100,
                      HTTP_REASON_STR_CONTINUE 
                      );
        BAIL_ON_VMREST_ERROR(dwError);

        dwError = VmRESTSetDataLength(
                      &pIntResPacket,
                      "0"
                      );
        BAIL_ON_VMREST_ERROR(dwError);

        dwError = VmRESTSetData(
                     pRESTHandle,
                     &pIntResPacket,
                     "",
                     0,
                     &nWrite
                     );
        BAIL_ON_VMREST_ERROR(dwError);
        pRequest->dataNotRcvd = 1;

        VMREST_LOG_DEBUG(pRESTHandle,"%s","Intermediate response sent successfully for 100-Continue header");
    }

cleanup:

    if (pszExpect)
    {
        VmRESTFreeMemory(pszExpect);
        pszExpect = NULL;
    }
    if (pszHttpURI)
    {
        VmRESTFreeMemory(pszHttpURI);
        pszHttpURI = NULL;
    }
    if (pszEndPointURI)
    {
        VmRESTFreeMemory(pszEndPointURI);
        pszEndPointURI = NULL;
    }

    if (pIntResPacket)
    {
        VmRESTFreeHTTPResponsePacket(&pIntResPacket);
        pIntResPacket = NULL;
    }

    return dwError;

error:
    VMREST_LOG_ERROR(pRESTHandle,"Error in processing 100-Continue header, error code %u", dwError);
    goto cleanup;


}

uint32_t
VMRESTWriteChunkedMessageInResponseStream(
    char*                            src,
    uint32_t                         srcSize,
    char*                            buffer,
    uint32_t*                        nWritten
)
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    char*                            curr = NULL;
    char                             chunkSize[HTTP_CHUNKED_DATA_LEN] = {0};
    size_t                           chunkLen = 0;

    if (!buffer || srcSize > MAX_DATA_BUFFER_LEN)
    {
        dwError = VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    sprintf(chunkSize, "%x", srcSize);
    curr = buffer;

    if (srcSize == 0)
    {
        /**** This is the last chunk ****/
        memcpy(curr,"0",1);
        curr = curr + 1;
        memcpy(curr,"\r\n\r\n",4);
        curr = curr + 4;
        *nWritten = 5;
    }
    else
    {
        /**** Write the chunk length ****/
        chunkLen = strlen(chunkSize);
        memcpy(curr,chunkSize,chunkLen);
        curr = curr + chunkLen;
        memcpy(curr,"\r\n",2);
        curr = curr + 2;

        /**** Write actual chunk data ****/
        memcpy(curr, src, srcSize);
        curr = curr + srcSize;
        memcpy(curr,"\r\n",2);
        curr = curr + 2;
        *nWritten = (uint32_t)(srcSize + chunkLen + 4);
    }

cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VMRESTWriteMessageBodyInResponseStream(
    PVM_REST_HTTP_RESPONSE_PACKET    pResPacket,
    char*                            buffer,
    uint32_t*                        nWritten
)
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    char*                            curr = NULL;
    uint32_t                         pszContentLen = 0;
    char*                            lenBytes = NULL;

    if (!pResPacket || !buffer)
    {
        dwError = VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    curr = buffer;

    dwError = VmRESTGetHttpResponseHeader(
                  pResPacket,
                  HTTP_HEADER_STR_CONTENT_LENGTH,
                  &lenBytes
                  );
    BAIL_ON_VMREST_ERROR(dwError);
    if ((lenBytes != NULL) && (strlen(lenBytes) > 0))
    {
        pszContentLen = strtoul(lenBytes,NULL, 10);
        if ((pszContentLen > 0) && (pszContentLen <= MAX_DATA_BUFFER_LEN))
        {
            memcpy(curr, pResPacket->messageBody->buffer, pszContentLen);
        }
    }
    else
    {
        dwError = VMREST_HTTP_VALIDATION_FAILED;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    *nWritten = pszContentLen;

cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VMRESTWriteStatusLineInResponseStream(
    PVM_REST_HTTP_RESPONSE_PACKET    pResPacket,
    char*                            buffer,
    uint32_t*                        nWritten
)
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    uint32_t                         bytesCount = 0;
    size_t                           len = 0;
    char*                            curr = NULL;

    if (!pResPacket || !buffer)
    {
        dwError = VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    curr = buffer;
    len = strlen(pResPacket->statusLine->version);
    if (len > 0)
    {
        memcpy(curr, pResPacket->statusLine->version, len);
        curr = curr + len;
        memcpy(curr, " ", 1);
        curr = curr + 1;
        bytesCount = (uint32_t)(bytesCount + len + 1);
        len = 0;
    }
    else
    {
        dwError = INTERNAL_SERVER_ERROR;
        BAIL_ON_VMREST_ERROR(dwError);
    }

    len = strlen(pResPacket->statusLine->statusCode);
    if (len > 0)
    {
        memcpy(curr, pResPacket->statusLine->statusCode, len);
        curr = curr + len;
        memcpy(curr, " ", 1);
        curr = curr + 1;
        bytesCount = (uint32_t)(bytesCount + len + 1);
        len = 0;
    }
    else
    {
        dwError = INTERNAL_SERVER_ERROR;
        BAIL_ON_VMREST_ERROR(dwError);
    }

    len = strlen(pResPacket->statusLine->reason_phrase);
    if (len > 0)
    {
        memcpy(curr, pResPacket->statusLine->reason_phrase, len);
        curr = curr + len;
        bytesCount = (uint32_t)(bytesCount + len);
        len = 0;
    }

    memcpy(curr, "\r\n", 2);
    curr = curr + 2;
    bytesCount = bytesCount + 2;

    *nWritten = bytesCount;

cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VmRESTAddAllHeaderInResponseStream(
    PVM_REST_HTTP_RESPONSE_PACKET    pResPacket,
    char*                            buffer,
    uint32_t*                        nWritten
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    size_t                           headerLen = 0;
    size_t                           valueLen = 0;
    uint32_t                         streamBytes = 0;
    char*                            curr = NULL;
    PVM_REST_HTTP_HEADER_NODE        miscHeaderNode = NULL;

    if (!buffer || !pResPacket)
    {
        dwError = VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    curr = buffer;

    miscHeaderNode = pResPacket->miscHeader->head;
    while (miscHeaderNode != NULL)
    {
        headerLen = strlen(miscHeaderNode->header);
        valueLen = strlen(miscHeaderNode->value);
        memcpy(curr, miscHeaderNode->header, headerLen);
        curr = curr + headerLen;
        memcpy(curr, ":", 1);
        curr = curr + 1;
        memcpy(curr, miscHeaderNode->value, valueLen);
        curr = curr + valueLen;
        memcpy(curr, "\r\n", 2);
        curr = curr + 2;
        streamBytes = (uint32_t)(streamBytes + headerLen + valueLen + 1 + 2);
        miscHeaderNode = miscHeaderNode->next;
    }

    /* Last Header written, Write one extra CR LF */
    memcpy(curr, "\r\n", 2);
    curr = curr + 2;
    streamBytes = streamBytes + 2;

    *nWritten = streamBytes;
cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VmRESTSendHeader(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_REST_HTTP_RESPONSE_PACKET*   ppResPacket
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    char*                            buffer = NULL;
    uint32_t                         totalBytes = 0;
    uint32_t                         bytes = 0;
    char*                            curr = NULL;
    uint32_t                         size = 0;
    PVM_REST_HTTP_RESPONSE_PACKET    pResPacket = NULL;

    if (!ppResPacket  || (*ppResPacket == NULL))
    {
        VMREST_LOG_ERROR(pRESTHandle,"%s","Invalid params");
        dwError = VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    /**** Get the size of buffer big enough to hold response packet ****/
    dwError = VmRESTGetResponseBufferSize(
                  *ppResPacket,
                  &size
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    /**** Allocate buffer to hold stream data *****/
    dwError = VmRESTAllocateMemory(
                  size,
                  (void**)&buffer
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    pResPacket = *ppResPacket;
    curr = buffer;

    /* Use response object to write to buffer */
    /* 1. Status Line */
    dwError = VMRESTWriteStatusLineInResponseStream(
                  pResPacket,
                  curr,
                  &bytes
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    curr = curr + bytes;
    totalBytes = totalBytes + bytes;
    bytes = 0;

    /* 2. Response Headers */
    dwError = VmRESTAddAllHeaderInResponseStream(
                  pResPacket,
                  curr,
                  &bytes
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    curr = curr + bytes;
    totalBytes = totalBytes + bytes;
    bytes = 0;

    dwError = VmRESTCommonWriteDataAtOnce(
                  pRESTHandle,
                  pResPacket->pSocket,
                  buffer,
                  totalBytes
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    VmRESTFreeMemory(
        buffer
        );
    buffer = NULL;

cleanup:
    return dwError;
error:
    if (buffer)
    {
        VmRESTFreeMemory(
            buffer
            );
        buffer = NULL;
    }
    VMREST_LOG_ERROR(pRESTHandle,"%s","Sending header data failed");
    goto cleanup;
}

uint32_t
VmRESTSendChunkedPayload(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_REST_HTTP_RESPONSE_PACKET*   ppResPacket,
    uint32_t                         dataLen
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    char*                            buffer = NULL;
    uint32_t                         totalBytes = 0;
    uint32_t                         bytes = 0;
    char*                            curr = NULL;
    PVM_REST_HTTP_RESPONSE_PACKET    pResPacket = NULL;

    if (!ppResPacket  || (*ppResPacket == NULL))
    {
        VMREST_LOG_ERROR(pRESTHandle,"%s","Invalid params");
        dwError = VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTAllocateMemory(
                  (MAX_DATA_BUFFER_LEN + MAX_EXTRA_CRLF_BUF_SIZE),
                  (void**)&buffer
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    pResPacket = *ppResPacket;
    curr = buffer;

    dwError = VMRESTWriteChunkedMessageInResponseStream(
                  pResPacket->messageBody->buffer,
                  dataLen,
                  curr,
                  &bytes
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    /**** reset the buffer in response packet ****/
    memset(pResPacket->messageBody->buffer, '\0', MAX_DATA_BUFFER_LEN);

    totalBytes = totalBytes + bytes;
    bytes = 0;

    
    dwError = VmRESTCommonWriteDataAtOnce(
                  pRESTHandle,
                  pResPacket->pSocket,
                  buffer,
                  totalBytes
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    VmRESTFreeMemory(
        buffer
        );
    buffer = NULL;

cleanup:
    return dwError;
error:
    if (buffer)
    {
        VmRESTFreeMemory(
            buffer
            );
        buffer = NULL;
    }
    VMREST_LOG_ERROR(pRESTHandle,"%s","Sending chunked payload data failed");
    goto cleanup;
}

uint32_t
VmRESTSendHeaderAndPayload(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_REST_HTTP_RESPONSE_PACKET*   ppResPacket
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    char*                            buffer = NULL;
    uint32_t                         totalBytes = 0;
    uint32_t                         bytes = 0;
    char*                            curr = NULL;
    uint32_t                         size = 0;
    PVM_REST_HTTP_RESPONSE_PACKET    pResPacket = NULL;

    if (!ppResPacket || (*ppResPacket == NULL))
    {
        VMREST_LOG_ERROR(pRESTHandle,"%s","Invalid params");
        dwError = VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTGetResponseBufferSize(
                  *ppResPacket,
                  &size
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    /**** This fuction is called only when size of payload is 
          less than 4096. Adding 10 extra bytes on top of it ****/

    size += MAX_DATA_BUFFER_LEN;
    size += 10;

    dwError = VmRESTAllocateMemory(
                  size,
                  (void**)&buffer
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    pResPacket = *ppResPacket;
    curr = buffer;

    /* Use response object to write to buffer */
    /* 1. Status Line */
    dwError = VMRESTWriteStatusLineInResponseStream(
                  pResPacket,
                  curr,
                  &bytes
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    curr = curr + bytes;
    totalBytes = totalBytes + bytes;
    bytes = 0;

    /* 2. Response Headers */
    dwError = VmRESTAddAllHeaderInResponseStream(
                  pResPacket,
                  curr,
                  &bytes
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    curr = curr + bytes;
    totalBytes = totalBytes + bytes;
    bytes = 0;

    /* 3. Message Body */

    dwError = VMRESTWriteMessageBodyInResponseStream(
                  pResPacket,
                  curr,
                  &bytes
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    curr = curr + bytes;
    totalBytes = totalBytes + bytes;
    bytes = 0;

    /**** This is for debug purpose:: will be removed ****/

    dwError = VmRESTCommonWriteDataAtOnce(
                  pRESTHandle,
                  pResPacket->pSocket,
                  buffer,
                  totalBytes
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    VmRESTFreeMemory(
        buffer
        );
    buffer = NULL;

cleanup:
    return dwError;
error:
    if (buffer)
    {
        VmRESTFreeMemory(
            buffer
            );
        buffer = NULL;
    }
    VMREST_LOG_ERROR(pRESTHandle,"%s","Sending header and payload data failed");
    goto cleanup;
}

uint32_t
VmRESTGetRequestHandle(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket,
    PREST_REQUEST*                   ppRequest
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    PREST_REQUEST                    pRequest = NULL;
    PREST_RESPONSE                   pResponse = NULL;

    if (!pRESTHandle)
    {
        VMREST_LOG_ERROR(pRESTHandle,"%s","Invalid REST Handler");
        dwError = REST_ERROR_INVALID_HANDLER;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTAllocateHTTPRequestPacket(
                  &pRequest
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTAllocateHTTPResponsePacket(
                  &pResponse
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    pRequest->miscHeader->head = NULL;
    pRequest->pSocket = pSocket;
    pRequest->dataNotRcvd  = 0;
    pRequest->nPayload = 0;
    pRequest->state = PROCESS_REQUEST_LINE;
    pRequest->pszPayload = NULL;
    pRequest->nBytesGetPayload = 0;
    pRequest->payloadType = HTTP_PAYLOAD_TYPE_INVALID;
    
    pResponse->miscHeader->head = NULL;
    pResponse->bHeaderSent = FALSE;
    pResponse->pSocket = pSocket;

    pResponse->requestPacket = pRequest;
    pRequest->pResponse = pResponse;

    /**** Peer IP and port info is available, store it in request object ****/
    memset(pRequest->clientIP, '\0', MAX_CLIENT_IP_ADDR_LEN);
    pRequest->clientPort = -1;

    dwError = VmRESTCommonGetPeerInfo(
                  pRESTHandle,
                  pSocket,
                  (pRequest->clientIP),
                  MAX_CLIENT_IP_ADDR_LEN,
                  &(pRequest->clientPort)
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    *ppRequest = pRequest;

cleanup:

    return dwError;

error:

    if (pRequest)
    {
        VmRESTFreeHTTPRequestPacket(
            &pRequest
            );
        pRequest = NULL;
    }

    if (pResponse)
    {
        VmRESTFreeHTTPResponsePacket(
            &pResponse
            );
        pResponse = NULL;
    }

    goto cleanup;

}

void
VmRESTFreeRequestHandle(
    PVMREST_HANDLE                   pRESTHandle,
    PREST_REQUEST                    pRequest
    )
{
    if (!pRESTHandle || !pRequest)
    {
        VMREST_LOG_ERROR(pRESTHandle,"%s","Invalid params");
        return;
    }

    if (pRequest->pResponse)
    {
        VmRESTFreeHTTPResponsePacket(
            &(pRequest->pResponse)
            );
        pRequest->pResponse = NULL;
    }

    if (pRequest)
    {
        VmRESTFreeHTTPRequestPacket(
            &pRequest
            );
    }
}

uint32_t
VmRESTProcessRequestLine(
    PVMREST_HANDLE                   pRESTHandle,
    PREST_REQUEST                    pRequest,
    char*                            pszBuffer,
    uint32_t                         nBytes,
    uint32_t*                        nProcessed
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    char*                            pszStartNewLine = pszBuffer;
    char*                            pszEndNewLine = NULL;
    char*                            pszFirstSpace = NULL;
    char*                            pszSecondSpace = NULL;
    uint32_t                         nLineLen = 0;

    if (!pRESTHandle || !pRequest || !nProcessed || !pszBuffer)
    {
        VMREST_LOG_ERROR(pRESTHandle,"%s","Invalid Params");
        dwError = REST_ERROR_INVALID_HANDLER;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    *nProcessed = 0;
 
    pszEndNewLine = strstr(pszStartNewLine, "\r\n");
    if (pszEndNewLine != NULL)
    {
        nLineLen = pszEndNewLine - pszStartNewLine;

        if (nLineLen < MAX_REQ_LIN_LEN)
        {
            /**** 1. Parsing HTTP METHOD ****/
            pszFirstSpace = strchr(pszStartNewLine, ' ');
            if (pszFirstSpace != NULL && ((pszFirstSpace - pszStartNewLine) <= MAX_METHOD_LEN) && ((pszFirstSpace - pszStartNewLine) > 0))
            {
                strncpy(pRequest->requestLine->method, pszStartNewLine, (pszFirstSpace - pszStartNewLine));
                pRequest->requestLine->method[pszFirstSpace - pszStartNewLine] = '\0';

                if(!VmRESTIsValidHTTPMethod(pRequest->requestLine->method))
                {
                    VMREST_LOG_ERROR(pRESTHandle,"%s","Bad HTTP method in request");
                    dwError = METHOD_NOT_ALLOWED;
                }
                BAIL_ON_VMREST_ERROR(dwError);

                /**** 2. Parse HTTP URI****/
                pszSecondSpace = strchr((pszFirstSpace + 1), ' ');
             
                if (pszSecondSpace != NULL && ((pszSecondSpace - pszFirstSpace) < MAX_URI_LEN) && ((pszSecondSpace - pszFirstSpace) > 0))
                {
                    strncpy(pRequest->requestLine->uri, (pszFirstSpace + 1), (pszSecondSpace - pszFirstSpace - 1));
                    pRequest->requestLine->uri[pszSecondSpace - pszFirstSpace - 1] = '\0';

                    /**** 3. Parse HTTP Version ****/
                    if (((pszEndNewLine - pszSecondSpace - 1) <= HTTP_VER_LEN) && ((pszEndNewLine - pszSecondSpace - 1) > 0))
                    {
                        strncpy(pRequest->requestLine->version, (pszSecondSpace + 1), (pszEndNewLine - pszSecondSpace - 1));
                        pRequest->requestLine->version[pszEndNewLine - pszSecondSpace - 1] = '\0';

                        if(!VmRESTIsValidHTTPVesion(pRequest->requestLine->version))
                        {
                            VMREST_LOG_ERROR(pRESTHandle,"%s","Validation failed for HTTP version");
                            dwError = HTTP_VERSION_NOT_SUPPORTED;
                        }
                    }
                    else
                    {
                        VMREST_LOG_ERROR(pRESTHandle,"Bad HTTP Version in request, length %u", (pszEndNewLine - pszSecondSpace));
                        dwError = HTTP_VERSION_NOT_SUPPORTED;
                    }
                }
                else
                {
                    VMREST_LOG_ERROR(pRESTHandle,"%s","Either Too large URI or URI not present");
                    dwError =  REQUEST_URI_TOO_LARGE;
                }
            }
            else
            {
                 VMREST_LOG_ERROR(pRESTHandle,"%s","Either too large HTTP method or method not present");
                 dwError = METHOD_NOT_ALLOWED;
            }
        }
        else
        {
            dwError = BAD_REQUEST;
            VMREST_LOG_ERROR(pRESTHandle,"%s","Request Line too large.");
        }
        BAIL_ON_VMREST_ERROR(dwError);

        /**** Change Status to processing headers now  ****/
        pRequest->state = PROCESS_REQUEST_HEADERS;
        *nProcessed = nLineLen + HTTP_CRLF_LEN;
        VMREST_LOG_DEBUG(pRESTHandle,"REQUEST LINE processed successfully, bytesProcessed %u, Total Bytes %u", *nProcessed, nBytes);
    }
    else if (nBytes < MAX_REQ_LIN_LEN)
    {
        /**** Need to wait for next IO notification on socket ****/
        VMREST_LOG_DEBUG(pRESTHandle,"REQUEST LINE NOT yet processed.. will wait till next IO ...., Total Bytes %u", nBytes);
        *nProcessed = 0;
    }
    else
    {
        dwError = REQUEST_URI_TOO_LARGE;
        BAIL_ON_VMREST_ERROR(dwError);
    }

cleanup:

    return dwError;

error:
    if (nProcessed)
    {
        *nProcessed = 0;
    }
    goto cleanup;
}

uint32_t
VmRESTProcessHeaders(
    PVMREST_HANDLE                   pRESTHandle,
    PREST_REQUEST                    pRequest,
    char*                            pszBuffer,
    uint32_t                         nBytes,
    uint32_t*                        nProcessed
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    char*                            pszStartNewLine = pszBuffer;
    char*                            pszEndNewLine = NULL;
    char*                            pszColonSeparator = NULL;
    uint32_t                         nLineLen = 0;
    uint32_t                         nAttrLen = 0;
    uint32_t                         nValueLen = 0;
    char                             attribute[MAX_HTTP_HEADER_ATTR_LEN] = {0};
    char                             value[MAX_HTTP_HEADER_VAL_LEN] = {0};
    uint32_t                         bytesProcessed  = 0;

    if (!pRESTHandle || !pRequest || !nProcessed || !pszBuffer)
    {
        VMREST_LOG_ERROR(pRESTHandle,"%s","Invalid Params");
        dwError = REST_ERROR_INVALID_HANDLER;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    *nProcessed = 0;

    pszEndNewLine = strstr(pszStartNewLine, "\r\n");

    if (pszEndNewLine)
    {
        nLineLen = pszEndNewLine - pszStartNewLine;
        VMREST_LOG_DEBUG(pRESTHandle,"Start processing header line, nLineLen %u", nLineLen);
        if (nLineLen == 0)
        {
            /**** We processed all headers, change processing state to PROCESS Payload ****/
            pRequest->state = PROCESS_REQUEST_PAYLOAD;

            *nProcessed = HTTP_CRLF_LEN;
            VMREST_LOG_DEBUG(pRESTHandle,"Finished Processing all HTTP headers .... Processed bytes this read %u", *nProcessed);

            dwError = VmRESTSetPayloadType(
                          pRequest
                          );
            BAIL_ON_VMREST_ERROR(dwError);

            dwError = VmRESTHandleExpect(
                          pRESTHandle,
                          pRequest
                          );
            BAIL_ON_VMREST_ERROR(dwError);

            goto cleanup;
        }
        if (nLineLen > MAX_REQ_LIN_LEN)
        {
             VMREST_LOG_ERROR(pRESTHandle,"Header line too large, length %u", nLineLen);
             dwError = BAD_REQUEST;
        }
        BAIL_ON_VMREST_ERROR(dwError);

        pszColonSeparator = strchr(pszStartNewLine, ':' );

        if (pszColonSeparator)
        {
            nAttrLen = pszColonSeparator - pszStartNewLine;
            if ((nAttrLen > 0) && (nAttrLen < MAX_HTTP_HEADER_ATTR_LEN) && (nAttrLen < nLineLen))
            {
                strncpy(attribute,pszStartNewLine, nAttrLen);
                attribute[nAttrLen] = '\0';
            }
            else
            {
                VMREST_LOG_ERROR(pRESTHandle,"Header name empty or too large, AttLen %u, nLineLen %u", nAttrLen, nLineLen);
                dwError = BAD_REQUEST;
                BAIL_ON_VMREST_ERROR(dwError);
            }

            nValueLen = pszEndNewLine - pszColonSeparator - 1;

            if ((nValueLen > 0) && (nValueLen < MAX_HTTP_HEADER_VAL_LEN) && (nValueLen < nLineLen))
            {
                strncpy(value,(pszColonSeparator + 1), nValueLen);
                value[nValueLen] = '\0';
            }
            else
            {
                VMREST_LOG_ERROR(pRESTHandle,"%s","Header value empty or too large");
                dwError = BAD_REQUEST;
            }
            BAIL_ON_VMREST_ERROR(dwError);

            dwError = VmRESTSetHttpRequestHeader(
                          pRequest,
                          attribute,
                          value
                          );
            BAIL_ON_VMREST_ERROR(dwError);

            *nProcessed = nLineLen + HTTP_CRLF_LEN;
            VMREST_LOG_DEBUG(pRESTHandle,"%s","Finished Processing header line");
        }
        else  /**** pszColonSeparator = NULL ****/
        {
            VMREST_LOG_ERROR(pRESTHandle,"%s", "No Header separator(:) found in request line");
            dwError = BAD_REQUEST;
            BAIL_ON_VMREST_ERROR(dwError);
        }
    }
    else if (nBytes < MAX_REQ_LIN_LEN)    /**** pszEndNewLine = NULL  ****/
    {
        VMREST_LOG_DEBUG(pRESTHandle,"Incomplete line processing.. wait for IO.., bytesProcessed %u, nBytes %u", bytesProcessed, nBytes);
    }
    else
    {
        dwError = BAD_REQUEST;
        BAIL_ON_VMREST_ERROR(dwError);
    }
    

cleanup:

    return dwError;

error:
    if (nProcessed)
    {
        *nProcessed = 0;
    }
    VMREST_LOG_ERROR(pRESTHandle,"Failed while processing headers... dwError %u", dwError);
    goto cleanup;

}


uint32_t
VmRESTProcessPayload(
    PVMREST_HANDLE                   pRESTHandle,
    PREST_REQUEST                    pRequest,
    char*                            pszBuffer,
    uint32_t                         nBytes,
    uint32_t*                        nProcessed
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    uint32_t                         nCopyBytes = 0;
    uint32_t                         nChunkBufferLen = 0;
    uint32_t                         nChunkLen = 0;
    uint32_t                         nCRLF = 0;

    if (!pRESTHandle || !pRequest || !nProcessed)
    {
        VMREST_LOG_ERROR(pRESTHandle,"%s","Invalid Params");
        dwError = REST_ERROR_INVALID_HANDLER;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    *nProcessed = 0;

    if (pRequest->payloadType == HTTP_PAYLOAD_CONTENT_LENGTH)
    {
        /**** As size of payload is already know, allocate the memory just once ****/
        if ((pRequest->pszPayload ==  NULL) && (pRequest->dataRemaining > 0))
        {
            dwError = VmRESTAllocateMemory(
                          pRequest->dataRemaining,
                          (void **)&pRequest->pszPayload
                          );
            BAIL_ON_VMREST_ERROR(dwError);
        }
        else if (pRequest->dataRemaining == 0)
        {
            /**** We are done processing payload, get ready to give callback to application ****/
            pRequest->state = PROCESS_APPLICATION_CALLBACK;
        }
    }
    else if (pRequest->payloadType == HTTP_PAYLOAD_TRANSFER_ENCODING)
    {
        if (pRequest->dataRemaining == 0)
        {
            if ((nBytes >= HTTP_CRLF_LEN) && (strncmp(pszBuffer, "\r\n", HTTP_CRLF_LEN) == 0))
            {
                /**** Handle the case when CR LF succeeding the last chunk data is not processed during last call to this function ****/
                *nProcessed = HTTP_CRLF_LEN;
            }
            else
            {
                /**** This is new Chunk ****/
                dwError = VmRESTGetChunkSize(
                              pszBuffer,
                              nBytes,
                              &nChunkBufferLen,
                              &nChunkLen
                              );
                BAIL_ON_VMREST_ERROR(dwError);    // handle REST_ENGINE_MORE_IO_REQUIRED
                VMREST_LOG_DEBUG(pRESTHandle,"Chunk Size %u, Chunk Buffer Len %u",nChunkLen, nChunkBufferLen);
                pRequest->dataRemaining = nChunkLen;
                if (nChunkLen == 0)
                {
                     /**** We are done processing payload, get ready to give callback to application ****/
                     pRequest->state = PROCESS_APPLICATION_CALLBACK;
                }
                else
                {
                    dwError = VmRESTReallocateMemory(
                                  (void *)pRequest->pszPayload,
                                  (void **)&pRequest->pszPayload,
                                  (pRequest->nPayload + nChunkLen)
                                  );
                    BAIL_ON_VMREST_ERROR(dwError);
                    nCRLF = HTTP_CRLF_LEN;
                }
            }
        }
    }

    nCopyBytes = ((pRequest->dataRemaining <= (nBytes - nChunkBufferLen)) ? pRequest->dataRemaining : (nBytes - nChunkBufferLen));
    if (nCopyBytes > 0)
    {
        memcpy((pRequest->pszPayload + pRequest->nPayload), (pszBuffer + nChunkBufferLen), nCopyBytes);
        pRequest->nPayload += nCopyBytes;
        pRequest->dataRemaining -= nCopyBytes;
        *nProcessed = nCopyBytes + nChunkBufferLen;
        if (((nBytes - *nProcessed) >= HTTP_CRLF_LEN) &&( pRequest->payloadType == HTTP_PAYLOAD_TRANSFER_ENCODING))
        {
            *nProcessed = *nProcessed + nCRLF;
        }
    }

cleanup:

    return dwError;

error:
    if (dwError == REST_ENGINE_MORE_IO_REQUIRED)
    {
        *nProcessed = 0;
        dwError = REST_ENGINE_SUCCESS;
    }
    else
    {
        VMREST_LOG_ERROR(pRESTHandle,"Failed while processing payload ... dwError %u", dwError);
    }
    goto cleanup;

}


uint32_t
VmRESTProcessBuffer(
    PVMREST_HANDLE                   pRESTHandle,
    char*                            pszBuffer,
    uint32_t                         nBytes,
    PREST_REQUEST                    pRequest,
    uint32_t*                        nBytesProcessed
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    VM_REST_PROCESSING_STATE         prevState = PROCESS_INVALID;
    VM_REST_PROCESSING_STATE         currState = PROCESS_INVALID;
    uint32_t                         nProcessed = 0;
    uint32_t                         nTotalProcessed = 0;
    BOOLEAN                          bInitiateClose = FALSE;

    if (!pRESTHandle || !pRequest || !pszBuffer || nBytes == 0)
    {
        VMREST_LOG_ERROR(pRESTHandle,"%s","Invalid REST Handler or Request Handle");
        dwError = REST_ERROR_INVALID_HANDLER;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    /**** Get the request processing state ****/
    currState = pRequest->state;
    *nBytesProcessed = 0;

    while (!((nProcessed == 0) && (currState == prevState)) && (nTotalProcessed <= nBytes))
    {
        prevState = currState;
        nProcessed = 0;
        switch(currState)
        {
            case PROCESS_REQUEST_LINE:
                 dwError = VmRESTProcessRequestLine(
                               pRESTHandle,
                               pRequest,
                               (pszBuffer + nTotalProcessed),
                               (nBytes - nTotalProcessed),
                               &nProcessed
                               );
                 BAIL_ON_VMREST_ERROR(dwError);
                 break;

            case PROCESS_REQUEST_HEADERS:

                 dwError = VmRESTProcessHeaders(
                               pRESTHandle,
                               pRequest,
                               (pszBuffer + nTotalProcessed),
                               (nBytes - nTotalProcessed),
                               &nProcessed
                               );
                 BAIL_ON_VMREST_ERROR(dwError);
                 break;

            case PROCESS_REQUEST_PAYLOAD:
                 dwError = VmRESTProcessPayload(
                               pRESTHandle,
                               pRequest,
                               (pszBuffer + nTotalProcessed),
                               (nBytes - nTotalProcessed),
                               &nProcessed
                               );
                 BAIL_ON_VMREST_ERROR(dwError);
                 break;

             case PROCESS_APPLICATION_CALLBACK:
                 /**** Give callback to application ****/
                 VMREST_LOG_INFO(pRESTHandle,"%s","C-REST-ENGINE: Giving callback to application...");
                 dwError = VmRESTTriggerAppCb(
                               pRESTHandle,
                               pRequest,
                               &(pRequest->pResponse)
                               );
                 VMREST_LOG_INFO(pRESTHandle,"C-REST-ENGINE: Application callback returns dwError %u", dwError);
                 BAIL_ON_VMREST_ERROR(dwError);
                 bInitiateClose = TRUE;
                 break;

             case PROCESS_INVALID:
                 VMREST_LOG_DEBUG(pRESTHandle,"%s","Bad Request object state detected");
                 break;
        }
        VMREST_LOG_DEBUG(pRESTHandle,"Total bytes before %u, nProcessed %u", nTotalProcessed, nProcessed);
        nTotalProcessed += nProcessed;
        currState = pRequest->state;
    }

    /**** We are going to wait for next IO inless ****/
    if (!bInitiateClose)
    {
        dwError = REST_ENGINE_MORE_IO_REQUIRED;
    }

cleanup:

    *nBytesProcessed = nTotalProcessed;
    return dwError;

error:

    VMREST_LOG_ERROR(pRESTHandle,"Process buffer failed with error code %u, sending failure response", dwError);
    VmRESTSendFailureResponse(
        pRESTHandle,
        dwError,
        pRequest
        );

    dwError = REST_ENGINE_SUCCESS;

    goto cleanup;

}

uint32_t
VmRESTTriggerAppCb(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_REST_HTTP_REQUEST_PACKET     pRequest,
    PVM_REST_HTTP_RESPONSE_PACKET*   ppResponse
)
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;

    if (!pRequest || !ppResponse || !pRESTHandle)
    {
       VMREST_LOG_ERROR(pRESTHandle,"%s","Invalid params");
       dwError =  VMREST_APPLICATION_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    if (pRESTHandle->pHttpHandler == NULL)
    {
        VMREST_LOG_ERROR(pRESTHandle,"%s","No application callback registered");
        dwError = VMREST_APPLICATION_NO_CB_REGISTERED;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    if (pRESTHandle->pHttpHandler->pfnHandleRequest)
    {
        dwError = pRESTHandle->pHttpHandler->pfnHandleRequest(pRESTHandle,pRequest, ppResponse);
    }
    else
    {
        VMREST_LOG_ERROR(pRESTHandle,"%s","No Request callback registered");
        dwError = VMREST_APPLICATION_VALIDATION_FAILED;
    }
    BAIL_ON_VMREST_ERROR(dwError);
    

    pRequest->state = PROCESS_INVALID;

cleanup:
    return dwError;



error:
    goto cleanup;
}

uint32_t
VmRESTSendFailureResponse(
     PVMREST_HANDLE                  pRESTHandle,
     uint32_t                        errorCode,
     PVM_REST_HTTP_REQUEST_PACKET    pRequest
     )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    char*                            pszStatusCode = NULL;
    char*                            pszReasonPhrase = NULL;
    uint32_t                         nBytesWritten = 0;
    PVM_REST_HTTP_RESPONSE_PACKET    pResponse = NULL;

    if (!pRESTHandle || !pRequest || !pRequest->pResponse)
    {
        VMREST_LOG_ERROR(pRESTHandle,"%s","Invalid params");
        dwError =  VMREST_APPLICATION_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    if (pRESTHandle->instanceState == VMREST_INSTANCE_STOPPED)
    {
        VMREST_LOG_ERROR(pRESTHandle, "%s", "Library trying to stop .. Rejecting request to send negative response");
        goto cleanup;
    }

    pResponse = pRequest->pResponse;

    VMREST_LOG_ERROR(pRESTHandle,"Preparing to send negative response to client, Error %u...", errorCode);

    switch(errorCode)
    {
        case BAD_REQUEST:
             pszStatusCode = "400";
             pszReasonPhrase = "Bad Request";
             break;

        case NOT_FOUND:
             pszStatusCode = "404";
             pszReasonPhrase = "URI Not Found";
             break;

        case METHOD_NOT_ALLOWED:
             pszStatusCode = "405";
             pszReasonPhrase = "Method Not Allowed";
             break;

        case REQUEST_TIMEOUT:
             pszStatusCode = "408";
             pszReasonPhrase = "Request Timeout";
             break;

        case LENGTH_REQUIRED:
             pszStatusCode = "411";
             pszReasonPhrase = "Length Required";
             break;

        case REQUEST_ENTITY_TOO_LARGE:
             pszStatusCode = "413";
             pszReasonPhrase = "Entity too Long";
             break;

        case REQUEST_URI_TOO_LARGE:
             pszStatusCode = "414";
             pszReasonPhrase = "URI too Long";
             break;

        case REQUEST_HEADER_FIELD_TOO_LARGE:
             pszStatusCode = "431";
             pszReasonPhrase = "Large Header Field";
             break;

        case INTERNAL_SERVER_ERROR:
             pszStatusCode = "500";
             pszReasonPhrase = "Internal server error";
             break;

        case HTTP_VERSION_NOT_SUPPORTED:
             pszStatusCode = "505";
             pszReasonPhrase = "HTTP Version not supported";
             break;

        default:
             pszStatusCode = "400";
             pszReasonPhrase = "Bad Request";
             break;
    }

    if (pResponse->bHeaderSent == FALSE)
    {
        dwError = VmRESTSetFailureResponse(
                      &pResponse,
                      pszStatusCode, 
                      pszReasonPhrase
                      );
        BAIL_ON_VMREST_ERROR(dwError);

        dwError = VmRESTSetDataLength(
                      &pResponse,
                      "0"
                      );
        BAIL_ON_VMREST_ERROR(dwError);

        dwError = VmRESTSetData(
                      pRESTHandle,
                      &pResponse,
                      "",
                      0,
                      &nBytesWritten
                      );
        BAIL_ON_VMREST_ERROR(dwError);

        pResponse->bHeaderSent = TRUE;
    }

cleanup:

    return dwError;

error:
    VMREST_LOG_ERROR(pRESTHandle,"%s", "Double failure observed while sending negative response...");

    goto cleanup;

}

uint32_t
VmRESTSetHttpPayloadZeroCopy(
    PVMREST_HANDLE                   pRESTHandle,
    PREST_RESPONSE*                  ppResponse,
    char const*                      pszBuffer,
    uint32_t                         nBytes
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    int                              ret = 0;
    PREST_RESPONSE                   pResponse = NULL;
    char                             pszContentLen[MAX_CONTENT_LEN_STR_SIZE] = {0};

    if (!pRESTHandle  || !ppResponse)
    {
        VMREST_LOG_ERROR(pRESTHandle,"%s","Invalid params");
        dwError = VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    ret = snprintf(pszContentLen, MAX_CONTENT_LEN_STR_SIZE ,"%u", nBytes);

    if ((ret < 0) || (ret >= MAX_CONTENT_LEN_STR_SIZE - 1))
    {
        VMREST_LOG_ERROR(pRESTHandle,"Bad content length, nBytes %u", nBytes);
        dwError = VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    VMREST_LOG_DEBUG(pRESTHandle,"Response content length is %s", pszContentLen);

    dwError = VmRESTSetHttpHeader(
                  ppResponse,
                  HTTP_HEADER_STR_CONTENT_LENGTH,
                  pszContentLen
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    pResponse = *ppResponse;

    /**** Send Header first ****/
    dwError = VmRESTSendHeader(
                  pRESTHandle,
                  ppResponse
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    if (nBytes > 0)
    {
        dwError = VmRESTCommonWriteDataAtOnce(
                      pRESTHandle,
                      pResponse->pSocket,
                      (char*)pszBuffer,
                      nBytes
                      );
        BAIL_ON_VMREST_ERROR(dwError);
    }

cleanup:

    return dwError;

error:

    VMREST_LOG_ERROR(pRESTHandle,"%s","Set Zero copy payload Failed");
    goto cleanup;
}

uint32_t
VmRESTEntertainPersistentConn(
    PVMREST_HANDLE                   pRESTHandle,
    PREST_REQUEST                    pRequest,
    BOOLEAN*                         bKeepOpen
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    char*                            pszKeepAliveRequest = NULL;
    char*                            pszKeepAliveResponse = NULL;
    BOOLEAN                          bKeepConnOpen = FALSE;

    if (!pRESTHandle || !pRequest || !bKeepOpen || !pRequest->pResponse)
    {
        VMREST_LOG_ERROR(pRESTHandle,"%s","Invalid params");
        dwError = VMREST_HTTP_INVALID_PARAMS;
    }

    /**** Get client's say on persistent connection ****/
    dwError = VmRESTGetHttpHeader(
                  pRequest,
                  "Connection",
                  &pszKeepAliveRequest
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    if ((pszKeepAliveRequest != NULL) && (strncmp(pszKeepAliveRequest, "keep-alive", strlen("keep-alive")) == 0))
    {
        bKeepConnOpen = TRUE;
    }

    /**** Inspect application response on connection (set from application callback) ****/
    if (bKeepConnOpen)
    {
        dwError = VmRESTGetHttpResponseHeader(
                      pRequest->pResponse,
                      "Connection",
                      &pszKeepAliveResponse
                      );
        BAIL_ON_VMREST_ERROR(dwError);

        if (!((pszKeepAliveResponse != NULL) && (strncmp(pszKeepAliveResponse, "keep-alive", strlen("keep-alive")) == 0)))
        {
            VMREST_LOG_WARNING(pRESTHandle,"%s","Client's request for persistent connection not entertained by server");
            bKeepConnOpen = FALSE;
        }
    }

    *bKeepOpen = bKeepConnOpen;

cleanup:

    if (pszKeepAliveRequest)
    {
        VmRESTFreeMemory(
            pszKeepAliveRequest
            );
        pszKeepAliveRequest = NULL;
    }

    return dwError;

error:

    if (bKeepOpen)
    {
        *bKeepOpen = FALSE;
    }

    goto cleanup;
}    

