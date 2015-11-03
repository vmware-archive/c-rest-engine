
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
VmRESTHTTPGetmethod(
    char*    line,
    uint32_t lineLen,
    char*    result
    )
{
    uint32_t dwError = 0;
    char*    buffer = NULL;
    char     local[MAX_METHOD_LEN] = {0};
    char*    temp = NULL;
    uint32_t i = 0;
    
    buffer = line;
    temp = local;

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
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_VMREST_ERROR(dwError);
    }
    
cleanup:
    return dwError;
error:
    goto cleanup;
}    

uint32_t
VmRESTHTTPGetReqURI(
    char*    line,
    uint32_t lineLen,
    char*    result
    )
{
    uint32_t  dwError = 0;
    char*     buffer = NULL;
    char      local[MAX_URI_LEN] = {0};
    char*     temp = NULL;
    char      flag = '0';
    uint32_t  i = 0;

    buffer = line;
    temp = local;

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
     
    if (strlen(local) != 0)
    {
        strcpy(result,buffer);
    }
    else
    {
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_VMREST_ERROR(dwError);
    }

cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VmRESTHTTPGetReqVersion(
    char*    line,
    uint32_t lineLen,
    char*    result
    )
{
    uint32_t   dwError = 0;
    uint32_t   count   = 0;
    char*      buffer = NULL;
    char       local[MAX_VERSION_LEN] = {0};
    char*      temp = NULL;
    uint32_t   i = 0;

    /* Version will be third letter in line */
   
    buffer = line;
    temp = local;

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
        
    if ((strcmp(local, "HTTP/1.1")) == 0 )
    {
        strcpy(result,local);
    }
    else 
    {
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_VMREST_ERROR(dwError);
    }
       
cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VmRESTHTTPPopulateHeader(
    char*                        line,
    uint32_t                     lineLen,
    PVM_REST_HTTP_REQUEST_PACKET pReqPacket
    )
{
    uint32_t dwError = 0;
    char*    buffer = NULL;
    char     local[MAX_REQ_LIN_LEN] = {0};
    char     attribute[MAX_HTTP_HEADER_ATTR_LEN] = {0};
    char     value[MAX_HTTP_HEADER_VAL_LEN] = {0};
    char     *temp = NULL;
    uint32_t i = 0;

    buffer = line;
    temp = local;
    
    while(buffer != NULL && i <= lineLen)
    {
        if (*buffer == ':')
        {
            buffer++;
            temp = '\0';
            strcpy(attribute,local);
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

    dwError = VmRESTSetHttpRequestHeader(
                   pReqPacket,
                   attribute,
                   value
              );
    BAIL_ON_VMREST_ERROR(dwError);


cleanup:
    return dwError;
error:
    goto cleanup;
}


uint32_t
VmRESTParseHTTPReqLine(
    uint32_t                      lineNo,
    char*                         line,
    uint32_t                      lineLen,
    PVM_REST_HTTP_REQUEST_PACKET  pReqPacket
    )
{
    uint32_t  dwError = 0;
    char      method[MAX_METHOD_LEN] = {0};
    char      URI[MAX_URI_LEN]={0};
    char      version[MAX_VERSION_LEN] = {0};

    if (lineNo == 1)
    {
        /* This is request Line */ 
        dwError = VmRESTHTTPGetmethod(
                      line,
                      lineLen,
                      method
                      );
        BAIL_ON_VMREST_ERROR(dwError);
        
        strcpy(pReqPacket->requestLine->method, method);
        dwError = VmRESTHTTPGetReqURI(
                      line,
                      lineLen,
                      URI
                      );
        BAIL_ON_VMREST_ERROR(dwError);
        strcpy(pReqPacket->requestLine->uri, URI);
        dwError = VmRESTHTTPGetReqVersion(
                      line,
                      lineLen,
                      version
                      );
        BAIL_ON_VMREST_ERROR(dwError);
        strcpy(pReqPacket->requestLine->version, version);
         
    } else
    {
        /* These are header lines */
        
        dwError = VmRESTHTTPPopulateHeader(
                      line,
                      lineLen,
                      pReqPacket
                      );
        
    }

cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VmRESTParseAndPopulateRawHTTPMessage(
    char*                        buffer,
    uint32_t                     packetLen,
    PVM_REST_HTTP_REQUEST_PACKET pReqPacket
    )
{
    uint32_t dwError = 0;
    uint32_t bytesRead = 0;
    uint32_t lineNo = 0;
    uint32_t lineLen = 0;
    char     local[MAX_REQ_LIN_LEN]={0};
    uint32_t contentLen = 0;
    char*    temp = buffer;
    char*    line = local;
    
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
                          pReqPacket              
                      );
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
    BAIL_ON_VMREST_ERROR(0);
cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VmRESTPopulateStatusLine(
    char*                           httpVersion,
    char*                           statusCode,
    char*                           reasonPhrase,
    PVM_REST_HTTP_RESPONSE_PACKET   pResPacket
    )
{
    uint32_t                       dwError = 0;
    
    if ( httpVersion == NULL || statusCode == NULL)
    {
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_VMREST_ERROR(dwError);
    }
    
    /* Reason phrase can be NULL */
    
    memcpy(pResPacket->statusLine->version, httpVersion, (strlen(httpVersion) + 1));
    memcpy(pResPacket->statusLine->statusCode, statusCode, (strlen(statusCode) + 1));
    if (httpVersion)
    {
        memcpy(pResPacket->statusLine->version, httpVersion,(strlen(httpVersion) + 1));
    }

cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t 
VmRESTPopulateResponseHeader(
    char*                           attribute,
    char*                           value,
    PVM_REST_HTTP_RESPONSE_PACKET   pResPacket
    )
{
    uint32_t dwError = 0;
    
    if (attribute == NULL || value == NULL)
    {
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_VMREST_ERROR(dwError);
    }
   
    if ((strcmp(attribute, "Accept-Ranges")) == 0)
    {
        strcpy(pResPacket->responseHeader->acceptRange, value);
    }
    else if ((strcmp(attribute, "Location")) == 0)
    {
        strcpy(pResPacket->responseHeader->location, value);
    }
    else if ((strcmp(attribute, "Connection")) == 0)
    {
        strcpy(pResPacket->generalHeader->connection, value);
    }
    else if ((strcmp(attribute, "Content-Length")) == 0)
    {
        strcpy(pResPacket->entityHeader->contentLength, value);
    }
    else if ((strcmp(attribute, "Content-Type")) == 0)
    {
        strcpy(pResPacket->entityHeader->contentType, value);
    }
    else if ((strcmp(attribute, "Content-Encoding")) == 0)
    {
        strcpy(pResPacket->entityHeader->contentEncoding, value);
    }    
    else if ((strcmp(attribute, "Proxy-Authenticate")) == 0)
    {
        strcpy(pResPacket->responseHeader->proxyAuth, value);
    }
    else if ((strcmp(attribute, "Server")) == 0)
    {
        strcpy(pResPacket->responseHeader->server, value);
    }
    else 
    {
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_VMREST_ERROR(dwError);
    }
    /* Keep on Adding Response Packet Headers here */

cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VmRESTCreateHTTPResponseMessage(
    PVM_REST_HTTP_RESPONSE_PACKET* ppResPacket
    )
{
    uint32_t dwError = 0;
    PVM_REST_HTTP_RESPONSE_PACKET   pResPacket = NULL;
    dwError = VmRESTAllocateHTTPResponsePacket(
        &pResPacket
    );
    BAIL_ON_VMREST_ERROR(dwError);
    
    ppResPacket = &pResPacket;

    
cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VMRESTWriteMessageBodyInResponse(
    PVM_REST_HTTP_RESPONSE_PACKET pResPacket,
    char                          *buffer,
    uint32_t                      *bytes
)
{
    uint32_t dwError = 0;
    char*    curr = NULL;
    uint32_t contentLen = 0;

    if (pResPacket == NULL || buffer == NULL)
    {
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_VMREST_ERROR(dwError);
    }
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
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_VMREST_ERROR(dwError);
    } 

    memcpy(curr,"\r\n",2);
    curr = curr + 2;
    
    *bytes = (contentLen + 2);
   

cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t 
VMRESTWriteStatusLineInResponse(
    PVM_REST_HTTP_RESPONSE_PACKET pResPacket,
    char                          *buffer,
    uint32_t                      *bytes
)
{
    uint32_t dwError = 0;
    uint32_t bytesCount = 0;
    uint32_t len = 0;
    char*    curr = NULL;
 
    if (pResPacket == NULL || buffer == NULL)
    {
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_VMREST_ERROR(dwError);
    }
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
VmRESTAddAllHeaderInResponse(
    PVM_REST_HTTP_RESPONSE_PACKET pResPacket,
    char*                         buffer,
    uint32_t                      *bytes
    )
{
    uint32_t            dwError = 0;
    uint32_t            len = 0;
    uint32_t            streamBytes = 0;
    char*               curr = NULL;
  
    if (buffer == NULL || pResPacket == NULL)
    {
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_VMREST_ERROR(dwError);
    }

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
    PVM_REST_HTTP_RESPONSE_PACKET* ppResPacket
    )
{
    uint32_t                          dwError = 0;
    char                              buffer[MAX_DATA_BUFFER_LEN] = {0};
    uint32_t                          totalBytes = 0;
    uint32_t                          bytes = 0;
    char*                             curr = NULL;
    PVM_REST_HTTP_RESPONSE_PACKET     pResPacket = NULL;
   
    if (ppResPacket == NULL || *ppResPacket == NULL)
    {
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_VMREST_ERROR(dwError);
    }

    pResPacket = *ppResPacket;    
    curr = buffer;
   
    /* Use response object to write to buffer */

    /* 1. Status Line */

    dwError = VMRESTWriteStatusLineInResponse(
                       pResPacket,
                       curr,
                       &bytes
              );
    BAIL_ON_VMREST_ERROR(dwError);

    curr = curr + bytes;
    totalBytes = totalBytes + bytes;
    bytes = 0;

    /* 2. Response Headers */

    dwError = VmRESTAddAllHeaderInResponse(
                                  pResPacket,
                                  curr,
                                  &bytes
             );
    BAIL_ON_VMREST_ERROR(dwError);    

    curr = curr + bytes;
    totalBytes = totalBytes + bytes;
    bytes = 0;

    /* 3. Message Body */

    dwError = VMRESTWriteMessageBodyInResponse(
                                  pResPacket,
                                  curr,
                                  &bytes
             );
    BAIL_ON_VMREST_ERROR(dwError);

    curr = curr + bytes;
    totalBytes = totalBytes + bytes;
    bytes = 0;
    
    dwError = VmsockPosixWriteDataAtOnce(
                       pResPacket->clientSocket,
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
    char         *buffer,
    uint32_t     byteRead,
    int          fd
    )
{
    uint32_t                     dwError = 0;
    PVM_REST_HTTP_REQUEST_PACKET pReqPacket = NULL;
    
    dwError = VmRESTAllocateHTTPRequestPacket(
        &pReqPacket
    );
    BAIL_ON_VMREST_ERROR(dwError);
    
    dwError = VmRESTParseAndPopulateRawHTTPMessage(
                  buffer,
                  byteRead,
                  pReqPacket);

    pReqPacket->clientSocket  = fd;    

    
#if 1
    write(1,"\n METHOD:", 9);
    write(1,pReqPacket->requestLine->method, 10);
    write(1,"\n CONNECTION:", 14);
    write(1,pReqPacket->generalHeader->connection, 20);
    write(1,"\n Content Length: ", 18);
    write(1,pReqPacket->entityHeader->contentLength,5);
    write(1,"\n Message Body: ", 15);
    write(1, pReqPacket->messageBody->buffer,30);

#endif
    
    dwError = VmRESTTestHTTPResponse(pReqPacket);

cleanup:
    return dwError;
error:
    goto cleanup;

}

/* This is unit test API which will be removed */

uint32_t 
VmRESTTestHTTPParser(
    void
    )
{
    uint32_t dwError = 0;
    char input[4096] = {0};
    memcpy(input, "POST http://SITE/foobar.html HTTP/1.1\r\nHost: SITE\r\nConnection: Keep-Alive\r\nContent-Length: 17\r\n\0\r\nThis is payload\r\n",129);
    PVM_REST_HTTP_REQUEST_PACKET pReqPacket = NULL;

    dwError = VmRESTAllocateHTTPRequestPacket(
        &pReqPacket
    ); 

    dwError = VmRESTParseAndPopulateRawHTTPMessage(
                  input,
                  4096,
                  pReqPacket);
#if 1
    write(1,"\n METHOD:", 9);
    write(1,pReqPacket->requestLine->method, 10);
    write(1,"\n CONNECTION:", 12);
    write(1,pReqPacket->generalHeader->connection, 20);
    write(1,"\n Content Length: ", 18);
    write(1,pReqPacket->entityHeader->contentLength,5);
    write(1,"\n Message Body: ", 15);
    write(1, pReqPacket->messageBody->buffer,30);
#endif

    return dwError;
}

/* This is unit test API which will be removed */

uint32_t
VmRESTTestHTTPResponse(
    PVM_REST_HTTP_REQUEST_PACKET pReqPacket
    )
{
    uint32_t dwError = 0;
    char header[32] = {0};
    char value[128] = {0};
    PVM_REST_HTTP_RESPONSE_PACKET pResPacket = NULL;

    /******** Allocate memory to response object */
    
    dwError = VmRESTAllocateHTTPResponsePacket(
        &pResPacket
    );

    /* Copy client socket information from request object to response object */

    pResPacket->clientSocket = pReqPacket->clientSocket;

    /******* set headers with exposed API */
    
    dwError = VmRESTSetHttpStatusCode(&pResPacket,
                     "200");
    
    dwError = VmRESTSetHttpStatusVersion(&pResPacket,
                     "HTTP/1.1");

    dwError = VmRESTSetHttpReasonPhrase(&pResPacket,
                     "OK");
                   

    strcpy(header,"Connection");
    strcpy(value, "close");
    dwError = VmRESTSetHttpHeader( &pResPacket,
                  header,
                  value
    );
    memset(header, '\0', 32);
    memset(value, '\0', 128);

    strcpy(header,"Content-Length");
    strcpy(value, "31");
    dwError = VmRESTSetHttpHeader( &pResPacket,
                  header,
                  value
    );
    memset(header, '\0', 32);
    memset(value, '\0', 128);

    dwError = VmRESTSetHttpPayload(&pResPacket,
                   "Payload Response with Length 31"
              );
    

    dwError = VmRESTSendResponsePacket(
                     &pResPacket
              );

    return dwError;
}

uint32_t 
VmRESTTriggerAppCb(
    PVM_REST_HTTP_REQUEST_PACKET  pRequest,
    PVM_REST_HTTP_RESPONSE_PACKET* ppResponse
)
{
    uint32_t    dwError = 0;
    uint32_t    methodNo = 0;
 
    if (pRequest == NULL || ppResponse == NULL)
    {
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_VMREST_ERROR(dwError);
    }

    dwError = VmRESTMapMethodToEnum(
                  pRequest->requestLine->method,
                  &methodNo
                  );
    BAIL_ON_VMREST_ERROR(dwError);
 
    if (gpHttpHandler == NULL)
    {
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_VMREST_ERROR(dwError);
    }
    
    switch(methodNo)
    {
        case HTTP_METHOD_GET:
             if (gpHttpHandler->pfnHandleHTTP_GET)
             {
                 dwError = gpHttpHandler->pfnHandleHTTP_GET(pRequest,
                                        ppResponse
                                        );
             
                 BAIL_ON_VMREST_ERROR(dwError);
             }
             break;
        case HTTP_METHOD_HEAD:
             if (gpHttpHandler->pfnHandleHTTP_HEAD)
             {
                 dwError = gpHttpHandler->pfnHandleHTTP_HEAD(pRequest,
                                        ppResponse
                                        );

                 BAIL_ON_VMREST_ERROR(dwError);
             }
             break;
        case HTTP_METHOD_POST:
             if (gpHttpHandler->pfnHandleHTTP_POST)
             {
                 dwError = gpHttpHandler->pfnHandleHTTP_POST(pRequest,
                                        ppResponse
                                        );

                 BAIL_ON_VMREST_ERROR(dwError);
             }
             break;
        case HTTP_METHOD_PUT:
             if (gpHttpHandler->pfnHandleHTTP_PUT)
             {
                 dwError = gpHttpHandler->pfnHandleHTTP_PUT(pRequest,
                                        ppResponse
                                        );

                 BAIL_ON_VMREST_ERROR(dwError);
             }
             break;
        case HTTP_METHOD_DELETE:
             if (gpHttpHandler->pfnHandleHTTP_DELETE)
             {
                 dwError = gpHttpHandler->pfnHandleHTTP_DELETE(pRequest,
                                        ppResponse
                                        );

                 BAIL_ON_VMREST_ERROR(dwError);
             }
             break;
        case HTTP_METHOD_TRACE:
             if (gpHttpHandler->pfnHandleHTTP_TRACE)
             {
                 dwError = gpHttpHandler->pfnHandleHTTP_TRACE(pRequest,
                                        ppResponse
                                        );

                 BAIL_ON_VMREST_ERROR(dwError);
             }
             break;
        case HTTP_METHOD_CONNECT:
             if (gpHttpHandler->pfnHandleHTTP_CONNECT)
             {
                 dwError = gpHttpHandler->pfnHandleHTTP_CONNECT(pRequest,
                                        ppResponse
                                        );

                 BAIL_ON_VMREST_ERROR(dwError);
             }
             break;
        default:
             dwError = ERROR_NOT_SUPPORTED;
             BAIL_ON_VMREST_ERROR(dwError);   
             break;
    }


cleanup:
    return dwError;
error:
    goto cleanup;    
}
