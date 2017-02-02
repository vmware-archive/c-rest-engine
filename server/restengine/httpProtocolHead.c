
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
    if (firstSpace != NULL && ((firstSpace - line) < MAX_METHOD_LEN))
    {
        strncpy(local, line, firstSpace - line);
        local[firstSpace - line] = '\0';
    }
    else
    {
        VMREST_LOG_ERROR("Bad method name in request");
        dwError = METHOD_NOT_ALLOWED;
        *resStatus = BAD_REQUEST;
    }
    BAIL_ON_VMREST_ERROR(dwError);

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
    else if (strcmp(local,"OPTIONS") == 0)
    {
        strcpy(result,"OPTIONS");
    }
    else
    {
        dwError = METHOD_NOT_ALLOWED;
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
    size_t                           uriLen = 0;

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
        if (secondSpace != NULL && ((secondSpace - firstSpace) < MAX_URI_LEN))
        {
            strncpy(result, firstSpace+1, secondSpace - firstSpace);
            result[secondSpace - firstSpace - 1] = '\0';
        }
        else
        {
            VMREST_LOG_ERROR("Invalid params");
            dwError =  REQUEST_URI_TOO_LARGE;
            *resStatus = BAD_REQUEST;
        }
    }
    BAIL_ON_VMREST_ERROR(dwError);

    uriLen = strlen(result);
    if (uriLen == 0 || uriLen > MAX_URI_LEN)
    {
        VMREST_LOG_ERROR("URI length too large");
        dwError = MAX_URI_LEN;
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
    size_t                           verLen = 0;
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
            if (endLine != NULL && ((endLine - secondSpace - 3) < HTTP_VER_LEN))
            {
                strncpy(local, secondSpace+1, HTTP_VER_LEN);
                local[HTTP_VER_LEN] = '\0';

                /**** Supports only HTTP 1.1 ****/
                if (strcmp(local, "HTTP/1.1") != 0)
                {
                    dwError = HTTP_VERSION_NOT_SUPPORTED;
                }
            }
            else
            {
                VMREST_LOG_ERROR("HTTP version not found");
                dwError = HTTP_VERSION_NOT_SUPPORTED;
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
    size_t                           attrLen = 0;
    size_t                           valLen  = 0;

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
        if ((*buffer == ':') && (attrLen  == 0))
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

    if (lineLen > MAX_REQ_LIN_LEN || !line  || !pReqPacket || (*resStatus != OK) || lineNo == 0)
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
    size_t                           lineLen = 0;
    char                             local[MAX_REQ_LIN_LEN]={0};
    char*                            temp = buffer;
    char*                            line = local;
    char                             appBuffer[MAX_DATA_BUFFER_LEN]={0};
    uint32_t                         bytesReadInBuffer = 0;
    uint32_t                         skipRead = 0;
    uint32_t                         extraBytes = 0;

    if (!buffer || !pReqPacket || (*resStatus != OK) || (packetLen <= 4))
    {
       VMREST_LOG_ERROR("Invalid params");
       dwError =  BAD_REQUEST;
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
                extraBytes = packetLen - bytesRead;
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

                if ((packetLen <= 4) && (strcmp(appBuffer, "\r\n\r\n") != 0))
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
                          (uint32_t)lineLen,
                          pReqPacket,
                          resStatus
                          );
            BAIL_ON_VMREST_ERROR(dwError);
            bytesRead = bytesRead + 2;
            if((*(temp+2) == '\r') && (*(temp+3) == '\n'))
            {
                bytesRead = bytesRead + 2;
                VMREST_LOG_DEBUG("Finished headers parsing with bytesRead %u", bytesRead);
                /**** All headers processed : data starts from here ***/
                dwError = VmSockPosixAdjustProcessedBytes(
                              pReqPacket->pSocket,
                              (bytesRead - extraBytes)
                              );
                BAIL_ON_VMREST_ERROR(dwError);
                //VMREST_LOG_DEBUG("%s",("Finished all header parsing, total header bytes %u", bytesRead);
                break;
            }
            temp = temp + 2;
            memset(local, '\0', MAX_REQ_LIN_LEN);
            line = local;
            continue;
        }
        if ((line - local) < MAX_REQ_LIN_LEN )
        {
            *line = *temp;
            temp++;
            line++;
            bytesRead++;
        }
        else
        {
            dwError = REQUEST_HEADER_FIELD_TOO_LARGE;
            BAIL_ON_VMREST_ERROR(dwError);
        }
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
    size_t                           chunkLen = 0;

    if (!buffer || srcSize < 0 || srcSize > MAX_DATA_BUFFER_LEN)
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
        *bytes = (uint32_t)(srcSize + chunkLen + 4);
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
    size_t                           len = 0;
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
        bytesCount = (uint32_t)(bytesCount + len + 1);
        len = 0;
    }
    else
    {
        VMREST_LOG_ERROR("HTTP version information missing in response");
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
        VMREST_LOG_ERROR("HTTP status code information missing in response");
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
    size_t                           headerLen = 0;
    size_t                           valueLen = 0;
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
        streamBytes = (uint32_t)(streamBytes + headerLen + valueLen + 1 + 2);
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
    //VMREST_LOG_DEBUG("%s",("Sending chunked payload completed.....");

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
    //VMREST_LOG_DEBUG("%s",("Entire response stream\n--------\n%s\n----------", buffer);

	VMREST_LOG_DEBUG("%s", "Start");
    dwError = VmsockPosixWriteDataAtOnce(
                  pResPacket->pSocket,
                  buffer,
                  totalBytes
                  );
	VMREST_LOG_DEBUG("end returned %u", dwError);
    BAIL_ON_VMREST_ERROR(dwError);

    //VMREST_LOG_DEBUG("%s",("Writen %u bytes at socket", totalBytes);

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

    if (!pResPacket)
    {
        VMREST_LOG_ERROR("Invalid params");
        dwError = VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

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
        
    VmRESTFreeHTTPResponsePacket(
        &pResPacket
        );
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:
    if (connection != NULL)
    {
        VmRESTFreeMemory(connection);
        connection = NULL;
    }
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
    PVM_REST_HTTP_RESPONSE_PACKET    pIntResPacket = NULL;
    uint32_t                         resStatus = OK;
    uint32_t                         connectionClosed = 0;
    char                             statusStng[MAX_STATUS_LENGTH] = {0};
    char*                            contentLen = NULL;
    char*                            transferEncoding = NULL;
    char*                            expect = NULL;
    uint32_t                         done = 0;
    char                             httpURI[MAX_URI_LEN] = {0};
    char                             endPointURI[MAX_URI_LEN] = {0};
    char*                            ptr = NULL;
    PREST_ENDPOINT                   pEndPoint = NULL;

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
    pReqPacket->dataNotRcvd = 0;
    memset(statusStng,'\0', MAX_STATUS_LENGTH);

    /**** 2. Start parsing the request line ****/

    dwError = VmRESTParseAndPopulateHTTPHeaders(
                  buffer,
                  byteRead,
                  pReqPacket,
                  &resStatus
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    VMREST_LOG_DEBUG("Header parsing done : return code %u", dwError);

    /**** 3: If Expect:100-continue is received, send the continue message back to client ****/
    dwError = VmRESTGetHttpHeader(
                  pReqPacket,
                  "Expect",
                  &expect
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    /**** FIXME:: space before header value ****/
    if (expect != NULL && ((strcmp(" 100-continue", expect) == 0) || (strcmp("100-continue", expect) == 0)))
    {
         /**** Do not send 100-continue for invalid URI ****/
         if (gRESTEngGlobals.useEndPoint == 1)
         {
             memset(httpURI, '\0', MAX_URI_LEN);
             memset(endPointURI, '\0', MAX_URI_LEN);
             dwError = VmRESTGetHttpURI(
                           pReqPacket,
                           &ptr
                           );
             BAIL_ON_VMREST_ERROR(dwError);
             memset(httpURI, '\0',MAX_URI_LEN);
             strncpy(httpURI,ptr,(MAX_URI_LEN - 1));
             if (ptr != NULL)
             {
                 VmRESTFreeMemory(ptr);
                 ptr = NULL;
             }

             dwError = VmRestGetEndPointURIfromRequestURI(
                           httpURI,
                           &ptr
                           );
             BAIL_ON_VMREST_ERROR(dwError);
             strncpy(endPointURI,ptr,(MAX_URI_LEN - 1));
             if (ptr != NULL)
             {
                 VmRESTFreeMemory(ptr);
                 ptr = NULL;
             }

             dwError = VmRestEngineGetEndPoint(
                            endPointURI,
                            &pEndPoint
                            );
             BAIL_ON_VMREST_ERROR(dwError);
         }

         dwError = VmRESTAllocateHTTPResponsePacket(
                  &pIntResPacket
                  );
         BAIL_ON_VMREST_ERROR(dwError);
         pIntResPacket->miscHeader->head = NULL;
         pIntResPacket->pSocket = pReqPacket->pSocket;
         pIntResPacket->requestPacket = pReqPacket;
         pIntResPacket->headerSent = 0;
         dwError = VmRESTSetFailureResponse( &pIntResPacket, "100","Continue");
         BAIL_ON_VMREST_ERROR(dwError);

         dwError = VmRESTSetDataLength(
                  &pIntResPacket,
                  "0"
                  );
         BAIL_ON_VMREST_ERROR(dwError);

         dwError = VmRESTSetData(
                  &pIntResPacket,
                  "",
                  0,
                  &done
                  );
         BAIL_ON_VMREST_ERROR(dwError);
         pReqPacket->dataNotRcvd = 1;

         VmRESTFreeHTTPResponsePacket(
            &pIntResPacket
            );
        pIntResPacket = NULL;
    }

    if (expect != NULL)
    {
        VmRESTFreeMemory(expect);
        expect = NULL;
    }

    /**** 4. Set the total payload information in request object ****/

    dwError = VmRESTGetHttpHeader(
                  pReqPacket,
                  "Content-Length",
                  &contentLen
                  );

    dwError = VmRESTGetHttpHeader(
                  pReqPacket,
                  "Transfer-Encoding",
                  &transferEncoding
                  );

    /**** 5. Either of Content length or transfer encoding must be set ****/

    if (contentLen != NULL && transferEncoding == NULL && (strlen(contentLen) > 0))
    {
        pReqPacket->dataRemaining = atoi(contentLen);
    }
    else if (transferEncoding != NULL && contentLen == NULL && ((strcmp(transferEncoding, "chunked") == 0) || (strcmp(transferEncoding, " chunked") == 0)))
    {
        pReqPacket->dataRemaining = 0;
    }
    else if (!(contentLen == NULL && transferEncoding == NULL))
    {
        dwError = LENGTH_REQUIRED;
        BAIL_ON_VMREST_ERROR(dwError);
    }

    if (contentLen != NULL)
    {
        VmRESTFreeMemory(contentLen);
        contentLen = NULL;
    }

    if (transferEncoding != NULL)
    {
        VmRESTFreeMemory(transferEncoding);
        transferEncoding = NULL;
    }

    /**** 6. Give application the callback ****/

    if (!dwError)
    {
        /***** Give the application callback ****/
        dwError = VmRESTTriggerAppCb(
                      pReqPacket,
                      &pResPacket
                      );
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

    /**** 7. Close the connection and free associated memory ****/

    dwError = VmRESTCloseClient(
                  pResPacket
                  );
    BAIL_ON_VMREST_ERROR(dwError);
    connectionClosed = 1;
    pResPacket = NULL;

cleanup:
    if ((pResPacket != NULL)  && connectionClosed == 0)
    {
        tempStatus = VmRESTCloseClient(
                         pResPacket
                         );
        /**** Error response is already sent to client, return success ****/
        dwError = REST_ENGINE_SUCCESS;
    }
    return dwError;
error:
    VMREST_LOG_ERROR("Something failed, dwError = %u", dwError);
    if (expect != NULL)
    {
        VmRESTFreeMemory(expect);
        expect = NULL;
    }
    if (pIntResPacket)
    {
        VmRESTFreeHTTPResponsePacket(
            &pIntResPacket
            );
        pIntResPacket = NULL;
    }
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
                if (dwError == BAD_REQUEST)
                {
                    tempStatus = VmRESTSetFailureResponse(
                                     &pResPacket,
                                     "400",
                                     "Bad Request"
                                     );
                }
                else if (dwError == NOT_FOUND)
                {
                    tempStatus = VmRESTSetFailureResponse(
                                     &pResPacket,
                                     "404",
                                     "URI Not Found"
                                     );
                }
                else if (dwError == LENGTH_REQUIRED)
                {
                    tempStatus = VmRESTSetFailureResponse(
                                     &pResPacket,
                                     "411",
                                     "Length Required"
                                     );
                }
                else if (dwError == REQUEST_HEADER_FIELD_TOO_LARGE)
                {
                    tempStatus = VmRESTSetFailureResponse(
                                     &pResPacket,
                                     "431",
                                     "Large Header Field"
                                     );
                }
                else if (dwError == METHOD_NOT_ALLOWED)
                {
                    tempStatus = VmRESTSetFailureResponse(
                                     &pResPacket,
                                     "405",
                                     "Method Not Allowed"
                                     );
                }
                else if (dwError == REQUEST_URI_TOO_LARGE)
                {
                    tempStatus = VmRESTSetFailureResponse(
                                     &pResPacket,
                                     "414",
                                     "URI too Long"
                                     );
                }
                else if (dwError == HTTP_VERSION_NOT_SUPPORTED)
                {
                    tempStatus = VmRESTSetFailureResponse(
                                     &pResPacket,
                                     "505",
                                     "HTTP Version not supported"
                                     );
                }
                else
                {
                    tempStatus = VmRESTSetFailureResponse(
                                     &pResPacket,
                                     "400",
                                     "Bad Request"
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
                    VMREST_LOG_ERROR("ERROR: DOUBLE FAILURE :: setting data in failure response");
                }

                pResPacket->headerSent = 1;
            }
            if (tempStatus)
            {
                VMREST_LOG_ERROR("Error in VmRESTSendHeaderAndPayload");
                goto cleanup;
            }
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
