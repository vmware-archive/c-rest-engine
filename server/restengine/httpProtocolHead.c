
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

    if (lineLen > MAX_REQ_LIN_LEN || line == NULL || result == NULL || *resStatus != OK)
    {
       VMREST_LOG_DEBUG("VmRESTHTTPGetReqMethod(): Invalid params");
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
        VMREST_LOG_DEBUG("VmRESTHTTPGetReqMethod(): Bad method name in request");
        dwError = VMREST_HTTP_VALIDATION_FAILED;
        *resStatus = BAD_REQUEST;
    }

    if (strlen(result) > MAX_METHOD_LEN)
    {
        VMREST_LOG_DEBUG("VmRESTHTTPGetReqMethod(): method len too large");
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
        VMREST_LOG_DEBUG("VmRESTHTTPGetReqMethod(): Method not allowed");
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

    if (lineLen > MAX_REQ_LIN_LEN || line == NULL || result == NULL || *resStatus != OK)
    {
       VMREST_LOG_DEBUG("VmRESTHTTPGetReqURI(): Invalid params");
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
            result[secondSpace - firstSpace] = '\0';
        }
        else
        {
            VMREST_LOG_DEBUG("VmRESTHTTPGetReqURI(): Invalid params");
            dwError =  VMREST_HTTP_INVALID_PARAMS;
            *resStatus = BAD_REQUEST;
        }
    }
    BAIL_ON_VMREST_ERROR(dwError);

    uriLen = strlen(result);
    if (uriLen == 0 || uriLen > MAX_URI_LEN)
    {
        VMREST_LOG_DEBUG("VmRESTHTTPGetReqURI(): URI length too large");
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
    if (lineLen > MAX_REQ_LIN_LEN || line == NULL || result == NULL || *resStatus != OK)
    {
       VMREST_LOG_DEBUG("VmRESTHTTPGetReqVersion(): Invalid params");
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
                VMREST_LOG_DEBUG("VmRESTHTTPGetReqVersion(): HTTP version not found");
                dwError = VMREST_HTTP_VALIDATION_FAILED;
                *resStatus = BAD_REQUEST;
            }
        }
    }
    BAIL_ON_VMREST_ERROR(dwError);

    verLen = strlen(local);
    if (verLen == 0 || verLen > MAX_VERSION_LEN)
    {
        VMREST_LOG_DEBUG("VmRESTHTTPGetReqVersion(): HTTP version not found");
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
        VMREST_LOG_DEBUG("VmRESTHTTPGetReqVersion():HTTP version not supported");
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

    if (lineLen > MAX_REQ_LIN_LEN || line == NULL || pReqPacket == NULL || *resStatus != OK)
    {
       VMREST_LOG_DEBUG("VmRESTHTTPPopulateHeader(): Invalid params");
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
        VMREST_LOG_DEBUG("VmRESTHTTPPopulateHeader(): Either header or value missing");
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

    if (lineLen > MAX_REQ_LIN_LEN || line == NULL || pReqPacket == NULL || *resStatus != OK)
    {
       VMREST_LOG_DEBUG("VmRESTParseHTTPReqLine(): Invalid params");
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
VmRESTParseAndPopulateRawHTTPMessage(
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
    uint32_t                         contentLen = 0;
    char*                            temp = buffer;
    char*                            line = local;

    if (buffer == NULL || pReqPacket == NULL || *resStatus != OK)
    {
       VMREST_LOG_DEBUG("VmRESTParseAndPopulateRawHTTPMessage(): Invalid params");
       dwError =  VMREST_HTTP_INVALID_PARAMS;
       *resStatus = BAD_REQUEST;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    while(bytesRead <= packetLen)
    {
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

            if((*(temp+2) == '\r') && (*(temp+3) == '\n'))
            {
                if (pReqPacket->entityHeader->contentLength != NULL)
                {
                    contentLen = atoi(pReqPacket->entityHeader->contentLength);
                    if (contentLen > 0 && contentLen <= MAX_HTTP_PAYLOAD_LEN)
                    {
                        memcpy(pReqPacket->messageBody->buffer, (temp+4), contentLen);
                    }
                }
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
VMRESTWriteMessageBodyInResponseStream(
    PVM_REST_HTTP_RESPONSE_PACKET    pResPacket,
    char*                            buffer,
    uint32_t*                        bytes
)
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    char*                            curr = NULL;
    uint32_t                         contentLen = 0;

    if (pResPacket == NULL || buffer == NULL)
    {
        VMREST_LOG_DEBUG("VMRESTWriteMessageBodyInResponseStream(): Invalid params");
        dwError = VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    curr = buffer;

    if (strlen(pResPacket->entityHeader->contentLength) > 0)
    {
        contentLen = atoi(pResPacket->entityHeader->contentLength);
        if ((contentLen > 0) && (contentLen <= MAX_HTTP_PAYLOAD_LEN))
        {
            memcpy(curr, pResPacket->messageBody->buffer, contentLen);
            curr = curr + contentLen;
        }
    }
    else
    {
        VMREST_LOG_DEBUG("VMRESTWriteMessageBodyInResponseStream(): No data to send");
        dwError = VMREST_HTTP_VALIDATION_FAILED;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    memcpy(curr,"\r\n",2);
    curr = curr + 2;

    *bytes = (contentLen + 2);

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

    if (pResPacket == NULL || buffer == NULL)
    {
        VMREST_LOG_DEBUG("VMRESTWriteStatusLineInResponseStream(): Invalid params");
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
    uint32_t                         len = 0;
    uint32_t                         headerLen = 0;
    uint32_t                         valueLen = 0;
    uint32_t                         streamBytes = 0;
    char*                            curr = NULL;
    PVM_REST_HTTP_HEADER_NODE        miscHeaderNode = NULL;

    if (buffer == NULL || pResPacket == NULL)
    {
        VMREST_LOG_DEBUG("VmRESTAddAllHeaderInResponseStream(): Invalid params");
        dwError = VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    curr = buffer;

    len = strlen(pResPacket->generalHeader->cacheControl);
    if ( len > 0)
    {
        memcpy(curr,"Cache-Control:", 14);
        curr = curr + 14;
        memcpy(curr, pResPacket->generalHeader->cacheControl, len);
        curr = curr + len;
        memcpy(curr, "\r\n", 2);
        curr = curr + 2;
        streamBytes = streamBytes + len + 2 + 14;
        len = 0;
    }
    len = strlen(pResPacket->generalHeader->connection);
    if ( len > 0)
    {
        memcpy(curr,"Connection:", 11);
        curr = curr + 11;
        memcpy(curr, pResPacket->generalHeader->connection, len);
        curr = curr + len;
        memcpy(curr, "\r\n", 2);
        curr = curr + 2;
        streamBytes = streamBytes + len + 2 + 11;
        len = 0;
    }
    len = strlen(pResPacket->generalHeader->trailer);
    if ( len > 0)
    {
        memcpy(curr,"Trailer:", 8);
        curr = curr + 8;
        memcpy(curr, pResPacket->generalHeader->trailer, len);
        curr = curr + len;
        memcpy(curr, "\r\n", 2);
        curr = curr + 2;
        streamBytes = streamBytes + len + 2 + 8;
        len = 0;
    }
    len = strlen(pResPacket->generalHeader->transferEncoding);
    if ( len > 0)
    {
        memcpy(curr,"Transfer-Encoding:", 18);
        curr = curr + 18;
        memcpy(curr, pResPacket->generalHeader->transferEncoding, len);
        curr = curr + len;
        memcpy(curr, "\r\n", 2);
        curr = curr + 2;
        streamBytes = streamBytes + len + 2 + 18;
        len = 0;
    }
    len = strlen(pResPacket->responseHeader->acceptRange);
    if ( len > 0)
    {
        memcpy(curr,"Accept-Ranges:", 14);
        curr = curr + 14;
        memcpy(curr, pResPacket->responseHeader->acceptRange, len);
        curr = curr + len;
        memcpy(curr, "\r\n", 2);
        curr = curr + 2;
        streamBytes = streamBytes + len + 2 + 14;
        len = 0;
    }
    len = strlen(pResPacket->responseHeader->location);
    if ( len > 0)
    {
        memcpy(curr,"Location:", 9);
        curr = curr + 9;
        memcpy(curr, pResPacket->responseHeader->location, len);
        curr = curr + len;
        memcpy(curr, "\r\n", 2);
        curr = curr + 2;
        streamBytes = streamBytes + len + 2 + 9;
        len = 0;
    }
    len = strlen(pResPacket->responseHeader->proxyAuth);
    if ( len > 0)
    {
        memcpy(curr,"Proxy-Authenticate:", 19);
        curr = curr + 19;
        memcpy(curr, pResPacket->responseHeader->proxyAuth, len);
        curr = curr + len;
        memcpy(curr, "\r\n", 2);
        curr = curr + 2;
        streamBytes = streamBytes + len + 2 + 19;
        len = 0;
    }
    len = strlen(pResPacket->responseHeader->server);
    if ( len > 0)
    {
        memcpy(curr,"Server:", 7);
        curr = curr + 7;
        memcpy(curr, pResPacket->responseHeader->server, len);
        curr = curr + len;
        memcpy(curr, "\r\n", 2);
        curr = curr + 2;
        streamBytes = streamBytes + len + 2 + 7;
        len = 0;
    }
    len = strlen(pResPacket->entityHeader->allow);
    if ( len > 0)
    {
        memcpy(curr,"Allow:", 6);
        curr = curr + 6;
        memcpy(curr, pResPacket->entityHeader->allow, len);
        curr = curr + len;
        memcpy(curr, "\r\n", 2);
        curr = curr + 2;
        streamBytes = streamBytes + len + 2 + 6;
        len = 0;
    }
    len = strlen(pResPacket->entityHeader->contentEncoding);
    if ( len > 0)
    {
        memcpy(curr,"Content-Encoding:", 17);
        curr = curr + 17;
        memcpy(curr, pResPacket->entityHeader->contentEncoding, len);
        curr = curr + len;
        memcpy(curr, "\r\n", 2);
        curr = curr + 2;
        streamBytes = streamBytes + len + 2 + 17;
        len = 0;
    }
    len = strlen(pResPacket->entityHeader->contentLanguage);
    if ( len > 0)
    {
        memcpy(curr,"Content-Language:", 17);
        curr = curr + 17;
        memcpy(curr, pResPacket->entityHeader->contentLanguage, len);
        curr = curr + len;
        memcpy(curr, "\r\n", 2);
        curr = curr + 2;
        streamBytes = streamBytes + len + 2 + 17;
        len = 0;
    }
    len = strlen(pResPacket->entityHeader->contentLength);
    if ( len > 0)
    {
        memcpy(curr,"Content-Length:", 15);
        curr = curr + 15;
        memcpy(curr,pResPacket->entityHeader->contentLength, len);
        curr = curr + len;
        memcpy(curr, "\r\n", 2);
        curr = curr + 2;
        streamBytes = streamBytes + len + 2 + 15;
        len = 0;
    }
    len = strlen(pResPacket->entityHeader->contentLocation);
    if ( len > 0)
    {
        memcpy(curr,"Content-Location:", 17);
        curr = curr + 17;
        memcpy(curr, pResPacket->entityHeader->contentLocation, len);
        curr = curr + len;
        memcpy(curr, "\r\n", 2);
        curr = curr + 2;
        streamBytes = streamBytes + len + 2 + 17;
        len = 0;
    }
    len = strlen(pResPacket->entityHeader->contentMD5);
    if ( len > 0)
    {
        memcpy(curr,"Content-MD5:", 12);
        curr = curr + 12;
        memcpy(curr, pResPacket->entityHeader->contentMD5, len);
        curr = curr + len;
        memcpy(curr, "\r\n", 2);
        curr = curr + 2;
        streamBytes = streamBytes + len + 2 + 12;
        len = 0;
    }
    len = strlen(pResPacket->entityHeader->contentRange);
    if ( len > 0)
    {
        memcpy(curr,"Content-Range:", 14);
        curr = curr + 14;
        memcpy(curr, pResPacket->entityHeader->contentRange, len);
        curr = curr + len;
        memcpy(curr, "\r\n", 2);
        curr = curr + 2;
        streamBytes = streamBytes + len + 2 + 14;
        len = 0;
    }
    len = strlen(pResPacket->entityHeader->contentType);
    if ( len > 0)
    {
        memcpy(curr,"Content-Type:", 13);
        curr = curr + 13;
        memcpy(curr, pResPacket->entityHeader->contentType, len);
        curr = curr + len;
        memcpy(curr, "\r\n", 2);
        curr = curr + 2;
        streamBytes = streamBytes + len + 2 + 13;
        len = 0;
    }
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
VmRESTSendResponsePacket(
    PVM_REST_HTTP_RESPONSE_PACKET*   ppResPacket
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    char                             buffer[MAX_DATA_BUFFER_LEN] = {0};
    uint32_t                         totalBytes = 0;
    uint32_t                         bytes = 0;
    char*                            curr = NULL;
    PVM_REST_HTTP_RESPONSE_PACKET    pResPacket = NULL;

    if (ppResPacket == NULL || *ppResPacket == NULL)
    {
        VMREST_LOG_DEBUG("VmRESTSendResponsePacket(): Invalid params");
        dwError = VMREST_HTTP_INVALID_PARAMS;
    }
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
    VMREST_LOG_DEBUG("Entire response stream\n--------\n %s\n----------", buffer);

    dwError = VmsockPosixWriteDataAtOnce(
                  pResPacket->clientIndex,
                  buffer,
                  totalBytes
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    /**** Closing the connection all the time. TODO:: check based close of connection ****/
    dwError = VmSockPosixCloseConnection(
                  pResPacket->clientIndex
                  );    
    BAIL_ON_VMREST_ERROR(dwError);

    /**** Free all associcated request and response object memory ****/
    VmRESTFreeMemory(pResPacket->requestPacket);
    pResPacket->requestPacket = NULL;
    VmRESTFreeMemory(pResPacket);        

    

cleanup:
    return dwError;
error:
    goto cleanup;

}

uint32_t
VmRESTProcessIncomingData(
    char*                            buffer,
    uint32_t                         byteRead,
    uint32_t                         clientIndex
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    uint32_t                         tempStatus = 0;
    PVM_REST_HTTP_REQUEST_PACKET     pReqPacket = NULL;
    PVM_REST_HTTP_RESPONSE_PACKET    pResPacket = NULL;
    uint32_t                         resStatus = OK;
    uint32_t                         responseSent = 0;
    char                             statusStng[MAX_STATUS_LENGTH] = {0};

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
    pReqPacket->clientIndex = clientIndex;
    pResPacket->clientIndex = pReqPacket->clientIndex;
    pResPacket->requestPacket = pReqPacket;
    memset(statusStng,'\0', MAX_STATUS_LENGTH);

    dwError = VmRESTParseAndPopulateRawHTTPMessage(
                  buffer,
                  byteRead,
                  pReqPacket,
                  &resStatus
                  );

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

    /********************************************
    *  Application should set all response packet
    *  header and payload before returing from
    *  callback function
    ********************************************/

    /**** Test function :: Will be removed ****
    dwError = VmRESTTestHTTPResponse(
                  pReqPacket,
                  pResPacket
                  );
    ************* End Test Function **********/

    dwError = VmRESTSendResponsePacket(
                  &pResPacket
                  );
    BAIL_ON_VMREST_ERROR(dwError);
    responseSent = 1;

cleanup:
    return dwError;
error:
    if (pReqPacket)
    {
        VmRESTFreeMemory(
            pReqPacket
            );
    }
    if (pResPacket)
    {
        if (!responseSent)
        {
            memset(statusStng,'\0', MAX_STATUS_LENGTH);
            tempStatus =  VmRESTUtilsConvertInttoString(
                              INTERNAL_SERVER_ERROR,
                              statusStng
                              );
            if (!tempStatus)
            {
                VMREST_LOG_DEBUG("VmRESTProcessIncomingData(): Error in VmRESTUtilsConvertInttoString");
                goto cleanup;
            }

            tempStatus = VmRESTSetHttpStatusCode(
                             &pResPacket,
                             statusStng
                             );
            if (!tempStatus)
            {
                VMREST_LOG_DEBUG("VmRESTProcessIncomingData(): Error in VmRESTSetHttpStatusCode");
                goto cleanup;
            }
            tempStatus = VmRESTSendResponsePacket(
                          &pResPacket
                          );
            if (!tempStatus)
            {
                VMREST_LOG_DEBUG("VmRESTProcessIncomingData(): Error in VmRESTSendResponsePacket");
                goto cleanup;
            }
        }
        VmRESTFreeMemory(
            pResPacket
            );
    }
    goto cleanup;
}

/* This is unit test API which will be removed */

uint32_t
VmRESTTestHTTPResponse(
    PVM_REST_HTTP_REQUEST_PACKET     pReqPacket,
    PVM_REST_HTTP_RESPONSE_PACKET    pResPacket
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    char                             header[32] = {0};
    char                             value[128] = {0};

    write(1,"\n METHOD:", 9);
    write(1,pReqPacket->requestLine->method, 10);
    write(1,"\n CONNECTION:", 14);
    write(1,pReqPacket->generalHeader->connection, 20);
    write(1,"\n Content Length: ", 18);
    write(1,pReqPacket->entityHeader->contentLength,5);
    write(1,"\n Message Body: ", 15);
    write(1, pReqPacket->messageBody->buffer,30);

    /******* set headers with exposed API */
    dwError = VmRESTSetHttpStatusCode(
                  &pResPacket,
                  "200"
                  );

    dwError = VmRESTSetHttpStatusVersion(
                  &pResPacket,
                  "HTTP/1.1"
                  );

    dwError = VmRESTSetHttpReasonPhrase(
                  &pResPacket,
                  "OK"
                  );

    strcpy(header,"Connection");
    strcpy(value, "close");
    dwError = VmRESTSetHttpHeader(
                  &pResPacket,
                  header,
                  value
                  );

    memset(header, '\0', 32);
    memset(value, '\0', 128);

    strcpy(header,"Content-Length");
    strcpy(value, "31");
    dwError = VmRESTSetHttpHeader(
                  &pResPacket,
                  header,
                  value
                  );

    memset(header, '\0', 32);
    memset(value, '\0', 128);

    dwError = VmRESTSetHttpPayload(
                  &pResPacket,
                  "Payload Response with Length 31"
                  );

    return dwError;
}

uint32_t
VmRESTTriggerAppCb(
    PVM_REST_HTTP_REQUEST_PACKET     pRequest,
    PVM_REST_HTTP_RESPONSE_PACKET*   ppResponse
)
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;

    if (pRequest == NULL || ppResponse == NULL)
    {
       VMREST_LOG_DEBUG("VmRESTTriggerAppCb(): Invalid params");
       dwError =  VMREST_APPLICATION_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    if (gpHttpHandler == NULL)
    {
        VMREST_LOG_DEBUG("VmRESTTriggerAppCb(): No application callback registered");
        dwError = VMREST_APPLICATION_NO_CB_REGISTERED;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    if (gpHttpHandler->pfnHandleRequest)
    {
        dwError = gpHttpHandler->pfnHandleRequest(pRequest, ppResponse);
    }
    else
    {
        VMREST_LOG_DEBUG("VmRESTTriggerAppCb(): No Request callback registered");
        dwError = VMREST_APPLICATION_VALIDATION_FAILED;
    }
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:
    return dwError;
error:
    goto cleanup;
}
