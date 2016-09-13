
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

#include <includes.h>

uint32_t
VmRESTHTTPGetReqMethod(
    char*                            line,
    uint32_t                         lineLen,
    char*                            result,
    uint32_t*                        resStatus
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    char                             local[MAX_METHOD_LEN] = {0};
    char*                            firstSpace = NULL;

    if (lineLen > MAX_REQ_LIN_LEN || !line  || !result || (*resStatus != OK))
    {
       VMREST_LOG_ERROR("Invalid params");
       dwError =  VMREST_HTTP_INVALID_PARAMS;
       *resStatus = BAD_REQUEST;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    firstSpace = strchr(line, ' ');
    if (firstSpace != NULL)
    {
        strncpy(local, line, firstSpace - line);
        local[firstSpace - line] = '\0';
    }
    else
    {
        VMREST_LOG_ERROR("Bad method name in request");
        dwError = VMREST_HTTP_VALIDATION_FAILED;
        *resStatus = BAD_REQUEST;
    }

    if (strlen(result) > MAX_METHOD_LEN)
    {
        VMREST_LOG_ERROR("Method len too large");
        dwError = VMREST_HTTP_VALIDATION_FAILED;
        *resStatus = BAD_REQUEST;
    }

    /* method will be first letter in line */
    if (strcmp(local,"GET") == 0)
    {
        strcpy(result,"GET");
    }
    else if (strcmp(local,"POST") == 0)
    {
        strcpy(result,"POST");
    }
    else if (strcmp(local,"DELETE") == 0)
    {
        strcpy(result,"DELETE");
    }
    else if (strcmp(local,"PATCH") == 0)
    {
        strcpy(result,"PATCH");
    }
    else if (strcmp(local,"HEAD") == 0)
    {
        strcpy(result,"HEAD");
    }
    else if (strcmp(local,"PUT") == 0)
    {
        strcpy(result,"PUT");
    }
    else if (strcmp(local,"TRACE") == 0)
    {
        strcpy(result,"TRACE");
    }
    else if (strcmp(local,"CONNECT") == 0)
    {
        strcpy(result,"CONNECT");
    }
    else
    {
        dwError = VMREST_HTTP_VALIDATION_FAILED;
        *resStatus = METHOD_NOT_ALLOWED;
        VMREST_LOG_ERROR("HTTP Method not allowed");
        BAIL_ON_VMREST_ERROR(dwError);
    }

cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VmRESTHTTPGetReqURI(
    char*                            line,
    uint32_t                         lineLen,
    char*                            result,
    uint32_t*                        resStatus
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    char*                            firstSpace = NULL;
    char*                            secondSpace = NULL;
    uint32_t                         uriLen = 0;

    if (lineLen > MAX_REQ_LIN_LEN || !line || !result  || (*resStatus != OK))
    {
       VMREST_LOG_ERROR("Invalid params");
       dwError =  VMREST_HTTP_INVALID_PARAMS;
       *resStatus = BAD_REQUEST;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    firstSpace = strchr(line, ' ');
    if (firstSpace != NULL)
    {
        secondSpace = strchr(firstSpace + 1, ' ');
        if (secondSpace != NULL)
        {
            strncpy(result, firstSpace+1, secondSpace - firstSpace);
            result[secondSpace - firstSpace - 1] = '\0';
        }
        else
        {
            VMREST_LOG_ERROR("Invalid params");
            dwError =  VMREST_HTTP_INVALID_PARAMS;
            *resStatus = BAD_REQUEST;
        }
    }
    BAIL_ON_VMREST_ERROR(dwError);

    uriLen = strlen(result);
    if (uriLen == 0 || uriLen > MAX_URI_LEN)
    {
        VMREST_LOG_ERROR("URI length too large");
        dwError = VMREST_HTTP_VALIDATION_FAILED;
        *resStatus = REQUEST_URI_TOO_LARGE;
    }
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:
    return dwError;
error:
    result = NULL;
    goto cleanup;
}

uint32_t
VmRESTHTTPGetReqVersion(
    char*                            line,
    uint32_t                         lineLen,
    char*                            result,
    uint32_t*                        resStatus
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    char                             local[MAX_VERSION_LEN] = {0};
    uint32_t                         verLen = 0;
    char*                            firstSpace = NULL;
    char*                            secondSpace = NULL;
    char*                            endLine = NULL;


    /* Version will be third letter in line */
    if (lineLen > MAX_REQ_LIN_LEN || !line || !result || (*resStatus != OK))
    {
       VMREST_LOG_ERROR("Invalid params");
       dwError =  VMREST_HTTP_INVALID_PARAMS;
       *resStatus = BAD_REQUEST;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    firstSpace = strchr(line, ' ');
    if (firstSpace != NULL)
    {
        secondSpace = strchr(firstSpace + 1, ' ');
        if (secondSpace != NULL)
        {
            endLine = strchr(secondSpace +1, '\0');
            if (endLine != NULL)
            {
                strncpy(local, secondSpace+1, HTTP_VER_LEN);
                local[HTTP_VER_LEN] = '\0';
            }
            else
            {
                VMREST_LOG_ERROR("HTTP version not found");
                dwError = VMREST_HTTP_VALIDATION_FAILED;
                *resStatus = BAD_REQUEST;
            }
        }
    }
    BAIL_ON_VMREST_ERROR(dwError);

    verLen = strlen(local);
    if (verLen == 0 || verLen > MAX_VERSION_LEN)
    {
        VMREST_LOG_ERROR("HTTP version not found");
        dwError = VMREST_HTTP_VALIDATION_FAILED;
        *resStatus = BAD_REQUEST;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    if ((strcmp(local, "HTTP/1.1") == 0) || (strcmp(local, "HTTP/1.0") == 0))
    {
        strcpy(result,local);
    }
    else
    {
        VMREST_LOG_ERROR("HTTP version not supported");
        dwError = VMREST_HTTP_VALIDATION_FAILED;
        *resStatus = HTTP_VERSION_NOT_SUPPORTED;
    }
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:
    return dwError;
error:
    result = NULL;
    goto cleanup;
}

uint32_t
VmRESTHTTPPopulateHeader(
    char*                            line,
    uint32_t                         lineLen,
    PVM_REST_HTTP_REQUEST_PACKET     pReqPacket,
    uint32_t*                        resStatus
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    char*                            buffer = NULL;
    char                             local[MAX_REQ_LIN_LEN] = {0};
    char                             attribute[MAX_HTTP_HEADER_ATTR_LEN] = {0};
    char                             value[MAX_HTTP_HEADER_VAL_LEN] = {0};
    char*                            temp = NULL;
    uint32_t                         i = 0;
    uint32_t                         attrLen = 0;
    uint32_t                         valLen  = 0;

    buffer = line;
    temp = local;

    if (lineLen > MAX_REQ_LIN_LEN || !line  || !pReqPacket  || (*resStatus != OK))
    {
       VMREST_LOG_ERROR("Invalid params");
       dwError =  VMREST_HTTP_INVALID_PARAMS;
       *resStatus = BAD_REQUEST;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    while(buffer != NULL && i <= lineLen)
    {
        if (*buffer == ':')
        {
            buffer++;
            *temp = '\0';
            strcpy(attribute,local);
            attrLen = strlen(attribute);
            memset(local,'\0', sizeof(local));
            temp = local;
            continue;
        }
        *temp = *buffer;
        buffer++;
        temp++;
        i++;
    }
    *temp = '\0';

    strcpy(value,local);
    valLen = strlen(value);

    if (attrLen == 0 || valLen == 0)
    {
        VMREST_LOG_ERROR("Either header or value missing");
        dwError =  VMREST_HTTP_VALIDATION_FAILED;
        *resStatus = BAD_REQUEST;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTSetHttpRequestHeader(
                  pReqPacket,
                  attribute,
                  value,
                  resStatus
                  );
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:
    return dwError;
error:
    goto cleanup;
}


uint32_t
VmRESTParseHTTPReqLine(
    uint32_t                         lineNo,
    char*                            line,
    uint32_t                         lineLen,
    PVM_REST_HTTP_REQUEST_PACKET     pReqPacket,
    uint32_t*                        resStatus
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    char                             method[MAX_METHOD_LEN] = {0};
    char                             URI[MAX_URI_LEN]={0};
    char                             version[MAX_VERSION_LEN] = {0};

    if (lineLen > MAX_REQ_LIN_LEN || !line  || !pReqPacket || (*resStatus != OK))
    {
       VMREST_LOG_ERROR("Invalid params");
       dwError =  VMREST_HTTP_INVALID_PARAMS;
       *resStatus = BAD_REQUEST;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    if (lineNo == 1)
    {
        /* This is request Line */
        dwError = VmRESTHTTPGetReqMethod(
                      line,
                      lineLen,
                      method,
                      resStatus
                      );
        BAIL_ON_VMREST_ERROR(dwError);
        strcpy(pReqPacket->requestLine->method, method);

        dwError = VmRESTHTTPGetReqURI(
                      line,
                      lineLen,
                      URI,
                      resStatus
                      );
        BAIL_ON_VMREST_ERROR(dwError);
        strcpy(pReqPacket->requestLine->uri, URI);

        dwError = VmRESTHTTPGetReqVersion(
                      line,
                      lineLen,
                      version,
                      resStatus
                      );
        BAIL_ON_VMREST_ERROR(dwError);
        strcpy(pReqPacket->requestLine->version, version);
    }
    else
    {
        /* These are header lines */
        dwError = VmRESTHTTPPopulateHeader(
                      line,
                      lineLen,
                      pReqPacket,
                      resStatus
                      );
        BAIL_ON_VMREST_ERROR(dwError);
    }

cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VmRESTParseAndPopulateHTTPHeaders(
    char*                            buffer,
    uint32_t                         packetLen,
    PVM_REST_HTTP_REQUEST_PACKET     pReqPacket,
    uint32_t*                        resStatus
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    uint32_t                         bytesRead = 0;
    uint32_t                         lineNo = 0;
    uint32_t                         lineLen = 0;
    char                             local[MAX_REQ_LIN_LEN]={0};
    char*                            temp = buffer;
    char*                            line = local;
    char                             appBuffer[MAX_DATA_BUFFER_LEN]={0};
    uint32_t                         bytesReadInBuffer = 0;
    uint32_t                         skipRead = 0;

    if (!buffer || !pReqPacket || (*resStatus != OK) || (packetLen <= 4))
    {
       VMREST_LOG_ERROR("Invalid params");
       dwError =  VMREST_HTTP_INVALID_PARAMS;
       *resStatus = BAD_REQUEST;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    while(1)
    {
        if (bytesRead >= (packetLen - 4))
        {

            if (temp && (strcmp(temp,"\r\n\r\n") == 0))
            {
                skipRead = 1;
            }
            /**** More socket read required to process the headers ****/

            if (!skipRead)
            {
                dwError = VmSockPosixAdjustProcessedBytes(
                              pReqPacket->pSocket,
                              bytesRead
                              );
                BAIL_ON_VMREST_ERROR(dwError);
                memset(appBuffer, '\0', MAX_DATA_BUFFER_LEN);

                dwError = VmsockPosixGetXBytes(
                              MAX_DATA_BUFFER_LEN,
                              appBuffer,
                              pReqPacket->pSocket,
                              &bytesReadInBuffer,
                              1
                              );
                BAIL_ON_VMREST_ERROR(dwError);
                temp = appBuffer;
                bytesRead = 0;
                packetLen = bytesReadInBuffer;

                if ((packetLen <= 4) && (appBuffer != NULL)  && (strcmp(appBuffer, "\r\n\r\n") != 0))
                {
                    skipRead = 1;
                    VMREST_LOG_ERROR("Bad HTTP request detected");
                    dwError =  VMREST_HTTP_VALIDATION_FAILED;
                    *resStatus = BAD_REQUEST;
                }
                BAIL_ON_VMREST_ERROR(dwError);
            }
        }
        if((*temp == '\r') && (*(temp+1) == '\n'))
        {
            lineNo++;
            *line = '\0';
            lineLen = strlen(local);
            /* call handler function with reqLine */
            dwError = VmRESTParseHTTPReqLine(
                          lineNo,
                          local,
                          lineLen,
                          pReqPacket,
                          resStatus
                          );
            BAIL_ON_VMREST_ERROR(dwError);
            bytesRead = bytesRead + 2;
            if((*(temp+2) == '\r') && (*(temp+3) == '\n'))
            {
                bytesRead = bytesRead + 2;
                /**** All headers processed : data starts from here ***/
                dwError = VmSockPosixAdjustProcessedBytes(
                              pReqPacket->pSocket,
                              bytesRead
                              );
                BAIL_ON_VMREST_ERROR(dwError);
                VMREST_LOG_DEBUG("Finished all header parsing, total header bytes %u", bytesRead);
                break;
            }
            temp = temp + 2;
            memset(local, '\0', MAX_REQ_LIN_LEN);
            line = local;
            continue;
        }
        *line = *temp;
        temp++;
        line++;
        bytesRead++;
    }
cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VMRESTWriteChunkedMessageInResponseStream(
    char*                            src,
    uint32_t                         srcSize,
    char*                            buffer,
    uint32_t*                        bytes
)
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    char*                            curr = NULL;
    char                             chunkSize[HTTP_CHUNCKED_DATA_LEN] = {0};
    uint32_t                         chunkLen = 0;

    if (!buffer)
    {
        VMREST_LOG_ERROR("Invalid params");
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
        *bytes = 5;
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
        *bytes = srcSize + chunkLen + 4;
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
    uint32_t*                        bytes
)
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    char*                            curr = NULL;
    uint32_t                         contentLen = 0;
    char*                            lenBytes = NULL;

    if (!pResPacket || !buffer)
    {
        VMREST_LOG_ERROR("Invalid params");
        dwError = VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    curr = buffer;

    dwError = VmRESTGetHttpResponseHeader(
                  pResPacket,
                  "Content-Length",
                  &lenBytes
                  );
    BAIL_ON_VMREST_ERROR(dwError);
    if ((lenBytes != NULL) && (strlen(lenBytes) > 0))
    {
        contentLen = atoi(lenBytes);
        if ((contentLen > 0) && (contentLen <= MAX_DATA_BUFFER_LEN))
        {
            memcpy(curr, pResPacket->messageBody->buffer, contentLen);
            curr = curr + contentLen;
        }
    }
    else
    {
        VMREST_LOG_ERROR("No data to send");
        dwError = VMREST_HTTP_VALIDATION_FAILED;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    *bytes = contentLen;

cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VMRESTWriteStatusLineInResponseStream(
    PVM_REST_HTTP_RESPONSE_PACKET    pResPacket,
    char*                            buffer,
    uint32_t*                        bytes
)
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    uint32_t                         bytesCount = 0;
    uint32_t                         len = 0;
    char*                            curr = NULL;

    if (!pResPacket || !buffer)
    {
        VMREST_LOG_ERROR("Invalid params");
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
        bytesCount = bytesCount + len + 1;
        len = 0;
    }

    len = strlen(pResPacket->statusLine->statusCode);
    if (len > 0)
    {
        memcpy(curr, pResPacket->statusLine->statusCode, len);
        curr = curr + len;
        memcpy(curr, " ", 1);
        curr = curr + 1;
        bytesCount = bytesCount + len + 1;
        len = 0;
    }

    len = strlen(pResPacket->statusLine->reason_phrase);
    if (len > 0)
    {
        memcpy(curr, pResPacket->statusLine->reason_phrase, len);
        curr = curr + len;
        bytesCount = bytesCount + len;
        len = 0;
    }

    memcpy(curr, "\r\n", 2);
    curr = curr + 2;
    bytesCount = bytesCount + 2;

    *bytes = bytesCount;

cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VmRESTAddAllHeaderInResponseStream(
    PVM_REST_HTTP_RESPONSE_PACKET    pResPacket,
    char*                            buffer,
    uint32_t*                        bytes
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    uint32_t                         headerLen = 0;
    uint32_t                         valueLen = 0;
    uint32_t                         streamBytes = 0;
    char*                            curr = NULL;
    PVM_REST_HTTP_HEADER_NODE        miscHeaderNode = NULL;

    if (!buffer || !pResPacket)
    {
        VMREST_LOG_ERROR("Invalid params");
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
        streamBytes = streamBytes + headerLen + valueLen + 1 + 2;
        miscHeaderNode = miscHeaderNode->next;
    }

    /* Last Header written, Write one extra CR LF */
    memcpy(curr, "\r\n", 2);
    curr = curr + 2;
    streamBytes = streamBytes + 2;

    *bytes = streamBytes;
cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VmRESTSendHeader(
    PVM_REST_HTTP_RESPONSE_PACKET*   ppResPacket
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
        VMREST_LOG_ERROR("Invalid params");
        dwError = VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    /**** TODO: Fix this. changes at max 10 header node in response ****/
    dwError = VmRESTAllocateMemory(
                  (sizeof(VM_REST_HTTP_HEADER_NODE) * 10),
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

    dwError = VmsockPosixWriteDataAtOnce(
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
    goto cleanup;
}

uint32_t
VmRESTSendChunkedPayload(
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
        VMREST_LOG_ERROR("Invalid params");
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

    
    dwError = VmsockPosixWriteDataAtOnce(
                  pResPacket->pSocket,
                  buffer,
                  totalBytes
                  );
    BAIL_ON_VMREST_ERROR(dwError);
    VMREST_LOG_DEBUG("Sending chunked payload completed.....");

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
    goto cleanup;
}

uint32_t
VmRESTSendHeaderAndPayload(
    PVM_REST_HTTP_RESPONSE_PACKET*   ppResPacket
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    char*                            buffer = NULL;
    uint32_t                         totalBytes = 0;
    uint32_t                         bytes = 0;
    char*                            curr = NULL;
    PVM_REST_HTTP_RESPONSE_PACKET    pResPacket = NULL;

    if (!ppResPacket || (*ppResPacket == NULL))
    {
        VMREST_LOG_ERROR("Invalid params");
        dwError = VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTAllocateMemory(
                  (MAX_DATA_BUFFER_LEN * 4),
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
    VMREST_LOG_DEBUG("Entire response stream\n--------\n%s\n----------", buffer);

    dwError = VmsockPosixWriteDataAtOnce(
                  pResPacket->pSocket,
                  buffer,
                  totalBytes
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    VMREST_LOG_DEBUG("Writen %u bytes at socket", totalBytes);

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
    goto cleanup;
}

uint32_t
VmRESTCloseClient(
    PVM_REST_HTTP_RESPONSE_PACKET    pResPacket
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    char*                            connection = NULL;
    uint32_t                         closeSocket = 1;
    char*                            statusStartChar = NULL;

    /**** Look for response status ****/
    statusStartChar = pResPacket->statusLine->statusCode;

    if(statusStartChar != NULL && (*statusStartChar == '4' || *statusStartChar == '5'))
    {
        /**** Failure response sent, must close client ****/
        closeSocket = 1;
    }
    else
    {
        /**** Non failure response sent, respect client say on connection close ****/
        if (pResPacket->requestPacket != NULL)
        {
            dwError = VmRESTGetHttpHeader(
                          pResPacket->requestPacket,
                          "Connection",
                          &connection
                          );
        }

        if ((connection != NULL) && (strcmp(connection, " keep-alive") == 0))
        {
            closeSocket = 0;
        }
    }
    if (closeSocket == 1)
    {
        /**** TODO: Inform transport to close connection else MUST NOT close it****/
    }

    /**** Free all associcated request and response object memory ****/
    if (pResPacket->requestPacket)
    {
        VmRESTFreeHTTPRequestPacket(
            &(pResPacket->requestPacket)
            );
        pResPacket->requestPacket = NULL;
    }
    if (pResPacket)
    {
        VmRESTFreeHTTPResponsePacket(
            &pResPacket
            );
    }
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VmRESTProcessIncomingData(
    char*                            buffer,
    uint32_t                         byteRead,
    PVM_SOCKET                       pSocket
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    uint32_t                         tempStatus = 0;
    PVM_REST_HTTP_REQUEST_PACKET     pReqPacket = NULL;
    PVM_REST_HTTP_RESPONSE_PACKET    pResPacket = NULL;
    uint32_t                         resStatus = OK;
    uint32_t                         connectionClosed = 0;
    char                             statusStng[MAX_STATUS_LENGTH] = {0};
    char*                            contentLen = NULL;
    uint32_t                         done = 0;

    VMREST_LOG_DEBUG("Process HTTP called with %u bytes and buffer data looks like\n%s\n", byteRead, buffer); 

    /**** 1. Allocate and init request and response objects ****/

    dwError = VmRESTAllocateHTTPRequestPacket(
                  &pReqPacket
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTAllocateHTTPResponsePacket(
                  &pResPacket
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    pReqPacket->miscHeader->head = NULL;
    pResPacket->miscHeader->head = NULL;
    pReqPacket->pSocket = pSocket;
    pResPacket->pSocket = pReqPacket->pSocket;
    pResPacket->requestPacket = pReqPacket;
    pResPacket->headerSent = 0;
    memset(statusStng,'\0', MAX_STATUS_LENGTH);

    /**** 2. Start parsing the request line ****/

    dwError = VmRESTParseAndPopulateHTTPHeaders(
                  buffer,
                  byteRead,
                  pReqPacket,
                  &resStatus
                  );
    VMREST_LOG_DEBUG("Header parsing done : return code %u", dwError);

    /**** 3. Set the total payload information in request object ****/

    dwError = VmRESTGetHttpHeader(
                  pReqPacket,
                  "Content-Length",
                  &contentLen
                  );
    BAIL_ON_VMREST_ERROR(dwError);


    if ((contentLen != NULL) && (strlen(contentLen) > 0))
    {
        pReqPacket->dataRemaining = atoi(contentLen);
    }
    else
    {
        pReqPacket->dataRemaining = 0;
    }
    

    /**** 4. Give application the callback ****/

    if (!dwError)
    {
        /***** Give the application callback ****/
        dwError = VmRESTTriggerAppCb(
                      pReqPacket,
                      &pResPacket
                      );
        VMREST_LOG_DEBUG("CallBack given to App: return code %u", dwError);
    }
    else
    {
        dwError = VmRESTUtilsConvertInttoString(
                      resStatus,
                      statusStng
                      );
        BAIL_ON_VMREST_ERROR(dwError);

        /**** There was error in request - No CB to app ****/
        dwError = VmRESTSetHttpStatusCode(
                      &pResPacket,
                      statusStng
                      );
    }
    BAIL_ON_VMREST_ERROR(dwError);

    /**** 5. Close the connection and free associated memory ****/

    dwError = VmRESTCloseClient(
                  pResPacket
                  );
    VMREST_LOG_DEBUG("Closed Client Connection: error code %u", dwError);
    BAIL_ON_VMREST_ERROR(dwError);
    connectionClosed = 1;

cleanup:
    return dwError;
error:
    VMREST_LOG_ERROR("Something failed");
    if (pReqPacket)
    {
        VmRESTFreeHTTPRequestPacket(
            &pReqPacket
            );
        if (pResPacket)
        {
            pResPacket->requestPacket = NULL;
        }
        pReqPacket = NULL;
    }
    if (pResPacket)
    {
        if (!connectionClosed)
        {
            if (pResPacket->headerSent == 0)
            {
                if (dwError == 100)
                {
                    tempStatus = VmRESTSetFailureResponse(
                                     &pResPacket,
                                     "400",
                                     "Bad Request"
                                     );
                }
                else
                {
                    tempStatus = VmRESTSetFailureResponse(
                                     &pResPacket,
                                     NULL,
                                     NULL
                                     );
                }
                if (tempStatus)
                {
                    VMREST_LOG_ERROR("ERROR setting the failure response object");
                    goto cleanup;
                }


                tempStatus = VmRESTSetDataLength(
                                 &pResPacket,
                                 "0"
                                 );

                if (tempStatus)
                {
                    VMREST_LOG_ERROR("ERROR setting the data length in failure response");
                    goto cleanup;
                }

                tempStatus =  VmRESTSetData(
                                  &pResPacket,
                                  "",
                                  0,
                                  &done
                                  );

                if (tempStatus)
                {
                    VMREST_LOG_ERROR("ERROR setting data in failure response");
                    goto cleanup;
                }

                pResPacket->headerSent = 1;
            }
            if (tempStatus)
            {
                VMREST_LOG_ERROR("Error in VmRESTSendHeaderAndPayload");
                goto cleanup;
            }
            tempStatus = VmRESTCloseClient(
                         pResPacket
                         );
            /**** Error response is already sent to client, return success ****/
            dwError = REST_ENGINE_SUCCESS;
        }
    }
    goto cleanup;
}

uint32_t
VmRESTTriggerAppCb(
    PVM_REST_HTTP_REQUEST_PACKET     pRequest,
    PVM_REST_HTTP_RESPONSE_PACKET*   ppResponse
)
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;

    if (!pRequest || !ppResponse)
    {
       VMREST_LOG_ERROR("Invalid params");
       dwError =  VMREST_APPLICATION_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    if (gpHttpHandler == NULL)
    {
        VMREST_LOG_ERROR("No application callback registered");
        dwError = VMREST_APPLICATION_NO_CB_REGISTERED;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    if (gpHttpHandler->pfnHandleRequest)
    {
        dwError = gpHttpHandler->pfnHandleRequest(pRequest, ppResponse);
    }
    else
    {
        VMREST_LOG_ERROR("No Request callback registered");
        dwError = VMREST_APPLICATION_VALIDATION_FAILED;
    }
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:
    return dwError;
error:
    goto cleanup;
}
