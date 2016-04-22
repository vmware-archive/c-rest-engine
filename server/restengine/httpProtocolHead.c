
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
    char*                            buffer = NULL;
    char                             local[MAX_METHOD_LEN] = {0};
    char*                            temp = NULL;
    uint32_t                         i = 0;

    buffer = line;
    temp = local;

    if (lineLen > MAX_REQ_LIN_LEN || line == NULL || result == NULL || *resStatus != OK)
    {
       VMREST_LOG_DEBUG("VmRESTHTTPGetReqMethod(): Invalid params");
       dwError =  VMREST_HTTP_INVALID_PARAMS;
       *resStatus = BAD_REQUEST;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    while ((buffer != NULL) && (i <= lineLen))
    {
        if (*buffer == ' ')
        {
            break;
        }
        *temp = *buffer;
        temp++;
        buffer++;
        i++;
    }
    *temp = '\0';
    if (strlen(local) > MAX_METHOD_LEN)
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
    char*     result,
    uint32_t* resStatus
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    char*                            buffer = NULL;
    char                             local[MAX_URI_LEN] = {0};
    char*                            temp = NULL;
    char                             flag = '0';
    uint32_t                         i = 0;
    uint32_t                         uriLen = 0;

    buffer = line;
    temp = local;

    if (lineLen > MAX_REQ_LIN_LEN || line == NULL || result == NULL || *resStatus != OK)
    {
       VMREST_LOG_DEBUG("VmRESTHTTPGetReqURI(): Invalid params");
       dwError =  VMREST_HTTP_INVALID_PARAMS;
       *resStatus = BAD_REQUEST;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    while ((buffer != NULL) && (i <= lineLen))
    {
        if (flag == '1')
        {
            *temp = *buffer;
            temp++;
        }
        if (*buffer == ' ')
        {
            if (flag == '1')
            {
                break;
            }
            flag = '1';
        }
        buffer++;
        i++;
    }
    *temp = '\0';

    /* URI will be second letter in line */
    uriLen = strlen(local);
    if (uriLen == 0)
    {
        VMREST_LOG_DEBUG("VmRESTHTTPGetReqURI(): No URI found in request");
        dwError = VMREST_HTTP_VALIDATION_FAILED;
        *resStatus = BAD_REQUEST;
    }
    else if (uriLen > MAX_URI_LEN)
    {
        VMREST_LOG_DEBUG("VmRESTHTTPGetReqURI(): URI length too large");
        dwError = VMREST_HTTP_VALIDATION_FAILED;
        *resStatus = REQUEST_URI_TOO_LARGE;
    }
    else
    {
        strcpy(result,buffer);
    }
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:
    return dwError;
error:
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
    uint32_t                         count   = 0;
    char*                            buffer = NULL;
    char                             local[MAX_VERSION_LEN] = {0};
    char*                            temp = NULL;
    uint32_t                         i = 0;
    uint32_t                         verLen = 0;

    /* Version will be third letter in line */

    buffer = line;
    temp = local;

    if (lineLen > MAX_REQ_LIN_LEN || line == NULL || result == NULL || *resStatus != OK)
    {
       VMREST_LOG_DEBUG("VmRESTHTTPGetReqVersion(): Invalid params");
       dwError =  VMREST_HTTP_INVALID_PARAMS;
       *resStatus = BAD_REQUEST;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    while ((buffer != NULL) && (i <= lineLen))
    {
        if (*buffer == ' ')
        {
            count++;
            buffer++;
            continue;
        }
        if (count == 2)
        {
            *temp = *buffer;
            temp++;
        }
        buffer++;
        i++;
    }
    *temp = '\0';

    verLen = strlen(local);
    if (verLen == 0 || verLen > MAX_VERSION_LEN)
    {
        VMREST_LOG_DEBUG("VmRESTHTTPGetReqVersion(): HTTP version not found");
        dwError = VMREST_HTTP_VALIDATION_FAILED;
        *resStatus = BAD_REQUEST;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    if ((strcmp(local, "HTTP/1.1")) == 0 )
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
            temp = '\0';
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
    uint32_t                         streamBytes = 0;
    char*                            curr = NULL;

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
        memcpy(curr,"Cache-Control : ", 16);
        curr = curr + 16;
        memcpy(curr, pResPacket->generalHeader->cacheControl, len);
        curr = curr + len;
        memcpy(curr, "\r\n", 2);
        curr = curr + 2;
        streamBytes = streamBytes + len + 2 + 16;
        len = 0;
    }
    len = strlen(pResPacket->generalHeader->connection);
    if ( len > 0)
    {
        memcpy(curr,"Connection : ", 13);
        curr = curr + 13;
        memcpy(curr, pResPacket->generalHeader->connection, len);
        curr = curr + len;
        memcpy(curr, "\r\n", 2);
        curr = curr + 2;
        streamBytes = streamBytes + len + 2 + 13;
        len = 0;
    }
    len = strlen(pResPacket->generalHeader->trailer);
    if ( len > 0)
    {
        memcpy(curr,"Trailer : ", 10);
        curr = curr + 10;
        memcpy(curr, pResPacket->generalHeader->trailer, len);
        curr = curr + len;
        memcpy(curr, "\r\n", 2);
        curr = curr + 2;
        streamBytes = streamBytes + len + 2 + 10;
        len = 0;
    }
    len = strlen(pResPacket->generalHeader->transferEncoding);
    if ( len > 0)
    {
        memcpy(curr,"Transfer-Encoding: ", 19);
        curr = curr + 19;
        memcpy(curr, pResPacket->generalHeader->transferEncoding, len);
        curr = curr + len;
        memcpy(curr, "\r\n", 2);
        curr = curr + 2;
        streamBytes = streamBytes + len + 2 + 19;
        len = 0;
    }
    len = strlen(pResPacket->responseHeader->acceptRange);
    if ( len > 0)
    {
        memcpy(curr,"Accept-Ranges : ", 16);
        curr = curr + 16;
        memcpy(curr, pResPacket->responseHeader->acceptRange, len);
        curr = curr + len;
        memcpy(curr, "\r\n", 2);
        curr = curr + 2;
        streamBytes = streamBytes + len + 2 + 16;
        len = 0;
    }
    len = strlen(pResPacket->responseHeader->location);
    if ( len > 0)
    {
        memcpy(curr,"Location : ", 11);
        curr = curr + 11;
        memcpy(curr, pResPacket->responseHeader->location, len);
        curr = curr + len;
        memcpy(curr, "\r\n", 2);
        curr = curr + 2;
        streamBytes = streamBytes + len + 2 + 11;
        len = 0;
    }
    len = strlen(pResPacket->responseHeader->proxyAuth);
    if ( len > 0)
    {
        memcpy(curr,"Proxy-Authenticate : ", 21);
        curr = curr + 21;
        memcpy(curr, pResPacket->responseHeader->proxyAuth, len);
        curr = curr + len;
        memcpy(curr, "\r\n", 2);
        curr = curr + 2;
        streamBytes = streamBytes + len + 2 + 21;
        len = 0;
    }
    len = strlen(pResPacket->responseHeader->server);
    if ( len > 0)
    {
        memcpy(curr,"Server : ", 9);
        curr = curr + 9;
        memcpy(curr, pResPacket->responseHeader->server, len);
        curr = curr + len;
        memcpy(curr, "\r\n", 2);
        curr = curr + 2;
        streamBytes = streamBytes + len + 2 + 9;
        len = 0;
    }
    len = strlen(pResPacket->entityHeader->allow);
    if ( len > 0)
    {
        memcpy(curr,"Allow : ", 8);
        curr = curr + 8;
        memcpy(curr, pResPacket->entityHeader->allow, len);
        curr = curr + len;
        memcpy(curr, "\r\n", 2);
        curr = curr + 2;
        streamBytes = streamBytes + len + 2 + 8;
        len = 0;
    }
    len = strlen(pResPacket->entityHeader->contentEncoding);
    if ( len > 0)
    {
        memcpy(curr,"Content-Encoding : ", 19);
        curr = curr + 19;
        memcpy(curr, pResPacket->entityHeader->contentEncoding, len);
        curr = curr + len;
        memcpy(curr, "\r\n", 2);
        curr = curr + 2;
        streamBytes = streamBytes + len + 2 + 19;
        len = 0;
    }
    len = strlen(pResPacket->entityHeader->contentLanguage);
    if ( len > 0)
    {
        memcpy(curr,"Content-Language : ", 19);
        curr = curr + 19;
        memcpy(curr, pResPacket->entityHeader->contentLanguage, len);
        curr = curr + len;
        memcpy(curr, "\r\n", 2);
        curr = curr + 2;
        streamBytes = streamBytes + len + 2 + 19;
        len = 0;
    }
    len = strlen(pResPacket->entityHeader->contentLength);
    if ( len > 0)
    {
        memcpy(curr,"Content-Length : ", 17);
        curr = curr + 17;
        memcpy(curr,pResPacket->entityHeader->contentLength, len);
        curr = curr + len;
        memcpy(curr, "\r\n", 2);
        curr = curr + 2;
        streamBytes = streamBytes + len + 2 + 17;
        len = 0;
    }
    len = strlen(pResPacket->entityHeader->contentLocation);
    if ( len > 0)
    {
        memcpy(curr,"Content-Location : ", 19);
        curr = curr + 19;
        memcpy(curr, pResPacket->entityHeader->contentLocation, len);
        curr = curr + len;
        memcpy(curr, "\r\n", 2);
        curr = curr + 2;
        streamBytes = streamBytes + len + 2 + 19;
        len = 0;
    }
    len = strlen(pResPacket->entityHeader->contentMD5);
    if ( len > 0)
    {
        memcpy(curr,"Content-MD5 : ", 14);
        curr = curr + 14;
        memcpy(curr, pResPacket->entityHeader->contentMD5, len);
        curr = curr + len;
        memcpy(curr, "\r\n", 2);
        curr = curr + 2;
        streamBytes = streamBytes + len + 2 + 14;
        len = 0;
    }
    len = strlen(pResPacket->entityHeader->contentRange);
    if ( len > 0)
    {
        memcpy(curr,"Content-Range : ", 16);
        curr = curr + 16;
        memcpy(curr, pResPacket->entityHeader->contentRange, len);
        curr = curr + len;
        memcpy(curr, "\r\n", 2);
        curr = curr + 2;
        streamBytes = streamBytes + len + 2 + 16;
        len = 0;
    }
    len = strlen(pResPacket->entityHeader->contentType);
    if ( len > 0)
    {
        memcpy(curr,"Content-Type : ", 15);
        curr = curr + 15;
        memcpy(curr, pResPacket->entityHeader->contentType, len);
        curr = curr + len;
        memcpy(curr, "\r\n", 2);
        curr = curr + 2;
        streamBytes = streamBytes + len + 2 + 15;
        len = 0;
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

    dwError = VmsockPosixWriteDataAtOnce(
                  pResPacket->clientSocketSSL,
                  pResPacket->clientSocketFd,
                  buffer,
                  totalBytes
                  );
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
    SSL*                             ssl,
    int                              fd
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    uint32_t                         tempStatus = 0;
    PVM_REST_HTTP_REQUEST_PACKET     pReqPacket = NULL;
    PVM_REST_HTTP_RESPONSE_PACKET    pResPacket = NULL;
    uint32_t                         resStatus = OK;
    uint32_t                         responseSent = 0;
    char                             statusStng[MAX_STATUS_LENGTH] = {0};
    int                              fileDes = -1;
    SSL*                             sslDes = NULL;

    dwError = VmRESTAllocateHTTPRequestPacket(
                  &pReqPacket
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTAllocateHTTPResponsePacket(
                  &pResPacket
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    if ((fd == -1) && (ssl != NULL))
    {
        sslDes = ssl;
    }
    else if ((ssl == NULL) && (fd >= 0))
    {
        fileDes = fd;
    }

    pReqPacket->clientSocketSSL = sslDes;
    pReqPacket->clientSocketFd = fileDes;
    pResPacket->clientSocketSSL = pReqPacket->clientSocketSSL;
    pResPacket->clientSocketFd = pReqPacket->clientSocketFd;
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
        dwError = my_itoa(
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

    /**** Test function :: Will be removed ****/
    dwError = VmRESTTestHTTPResponse(
                  pReqPacket,
                  pResPacket
                  );
    /************* End Test Function **********/

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
            tempStatus =  my_itoa(
                              INTERNAL_SERVER_ERROR,
                              statusStng
                              );
            if (!tempStatus)
            {
                VMREST_LOG_DEBUG("VmRESTProcessIncomingData(): Error in my_itoa");
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
    uint32_t                         methodNo = 0;

    if (pRequest == NULL || ppResponse == NULL)
    {
       VMREST_LOG_DEBUG("VmRESTTriggerAppCb(): Invalid params");
       dwError =  VMREST_APPLICATION_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTMapMethodToEnum(
                  pRequest->requestLine->method,
                  &methodNo
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    if (gpHttpHandler == NULL)
    {
        VMREST_LOG_DEBUG("VmRESTTriggerAppCb(): No application callback registered");
        dwError = VMREST_APPLICATION_NO_CB_REGISTERED;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    switch(methodNo)
    {
        case HTTP_METHOD_GET:
             if (gpHttpHandler->pfnHandleGET)
             {
                 dwError = gpHttpHandler->pfnHandleGET(pRequest,
                                        ppResponse
                                        );
             }
             else
             {
                 VMREST_LOG_DEBUG("VmRESTTriggerAppCb(): No HTTP GET CB registered");
                 dwError = VMREST_APPLICATION_NO_METHOD_GET_CB;
             }
             break;
        case HTTP_METHOD_HEAD:
             if (gpHttpHandler->pfnHandleHEAD)
             {
                 dwError = gpHttpHandler->pfnHandleHEAD(pRequest,
                                        ppResponse
                                        );
             }
             else
             {
                 VMREST_LOG_DEBUG("VmRESTTriggerAppCb(): No HTTP HEAD CB registered");
                 dwError = VMREST_APPLICATION_NO_METHOD_HEAD_CB;
             }
             break;
        case HTTP_METHOD_POST:
             if (gpHttpHandler->pfnHandlePOST)
             {
                 dwError = gpHttpHandler->pfnHandlePOST(pRequest,
                                        ppResponse
                                        );
             }
             else
             {
                 VMREST_LOG_DEBUG("VmRESTTriggerAppCb(): No HTTP POST CB registered");
                 dwError = VMREST_APPLICATION_NO_METHOD_POST_CB;
             }
             break;
        case HTTP_METHOD_PUT:
             if (gpHttpHandler->pfnHandlePUT)
             {
                 dwError = gpHttpHandler->pfnHandlePUT(pRequest,
                                        ppResponse
                                        );
             }
             else
             {
                 VMREST_LOG_DEBUG("VmRESTTriggerAppCb(): No HTTP PUT CB registered");
                 dwError = VMREST_APPLICATION_NO_METHOD_PUT_CB;
             }
             break;
        case HTTP_METHOD_DELETE:
             if (gpHttpHandler->pfnHandleDELETE)
             {
                 dwError = gpHttpHandler->pfnHandleDELETE(pRequest,
                                        ppResponse
                                        );
             }
             else
             {
                 VMREST_LOG_DEBUG("VmRESTTriggerAppCb(): No HTTP DELETE CB registered");
                 dwError = VMREST_APPLICATION_NO_METHOD_DELETE_CB;
             }
             break;
        case HTTP_METHOD_TRACE:
             if (gpHttpHandler->pfnHandleTRACE)
             {
                 dwError = gpHttpHandler->pfnHandleTRACE(pRequest,
                                        ppResponse
                                        );
             }
             else
             {
                 VMREST_LOG_DEBUG("VmRESTTriggerAppCb(): No HTTP TRACE CB registered");
                 dwError = VMREST_APPLICATION_NO_METHOD_TRACE_CB;
             }
             break;
        case HTTP_METHOD_CONNECT:
             if (gpHttpHandler->pfnHandleCONNECT)
             {
                 dwError = gpHttpHandler->pfnHandleCONNECT(pRequest,
                                        ppResponse
                                        );
             }
             else
             {
                 VMREST_LOG_DEBUG("VmRESTTriggerAppCb(): No HTTP CONNECT CB registered");
                 dwError = VMREST_APPLICATION_NO_METHOD_CONNECT_CB;
             }
             break;
        default:
             dwError = VMREST_APPLICATION_VALIDATION_FAILED;
             break;
    }
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:
    return dwError;
error:
    goto cleanup;
}
