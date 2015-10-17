
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

static
uint32_t 
VmRESTAllocateRequestLine(
    PVM_REST_HTTP_REQUEST_LINE* ppReqLine
    );

static
uint32_t
VmRESTAllocateStatusLine(
    PVM_REST_HTTP_STATUS_LINE* ppStatusLine
    );

static
uint32_t
VmRESTAllocateGeneralHeader(
    PVM_REST_HTTP_GENERAL_HEADER* ppGenHeader
    );

static
uint32_t
VmRESTAllocateRequestHeader(
    PVM_REST_HTTP_REQUEST_HEADER* ppReqHeader
    );

static
uint32_t
VmRESTAllocateResponseHeader(
    PVM_REST_HTTP_RESPONSE_HEADER* ppResHeader
    );

static
uint32_t
VmRESTAllocateEntityHeader(
    PVM_REST_HTTP_ENTITY_HEADER* ppEntityHeader
    );

static
uint32_t
VmRESTAllocateMessageBody(
    PVM_REST_HTTP_MESSAGE_BODY* ppMsgBody
    );


uint32_t 
VmRESTAllocateHTTPRequestPacket(
    PVM_REST_HTTP_REQUEST_PACKET* ppReqPacket
    )
{
    uint32_t dwError = 0;
    PVM_REST_HTTP_REQUEST_PACKET pReqPacket = NULL;
    
    PVM_REST_HTTP_REQUEST_LINE     pReqLine = NULL;
    PVM_REST_HTTP_GENERAL_HEADER   pGenHeader = NULL;
    PVM_REST_HTTP_REQUEST_HEADER   pReqHeader = NULL;
    PVM_REST_HTTP_ENTITY_HEADER    pEntityHeader= NULL;
    PVM_REST_HTTP_MESSAGE_BODY     pMessageBody = NULL;

    dwError = VmRESTAllocateRequestLine(
              &pReqLine);
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTAllocateGeneralHeader(
              &pGenHeader);
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTAllocateRequestHeader(
              &pReqHeader);
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTAllocateEntityHeader(
              &pEntityHeader);
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTAllocateMessageBody(
              &pMessageBody);
    BAIL_ON_VMREST_ERROR(dwError);   
 
    dwError = VmRESTAllocateMemory(
                   sizeof(VM_REST_HTTP_REQUEST_PACKET),
                   (void**)&pReqPacket);
    BAIL_ON_VMREST_ERROR(dwError);

    pReqPacket->requestLine = pReqLine;  
    pReqPacket->generalHeader = pGenHeader;
    pReqPacket->requestHeader = pReqHeader;
    pReqPacket->entityHeader = pEntityHeader; 
    pReqPacket->messageBody = pMessageBody;

    *ppReqPacket = pReqPacket;
    
cleanup:
    return dwError;
error:
    /* TODO: Write cleanup routine */
    goto cleanup;
}

uint32_t
VmRESTFreeHTTPRequestPacket(
    PVM_REST_HTTP_REQUEST_PACKET* ppReqPacket
    )
{
    uint32_t dwError = 0;
    
    BAIL_ON_VMREST_ERROR(dwError);


cleanup:
    return dwError;
error:
    goto cleanup;
}


uint32_t
VmRESTAllocateHTTPResponsePacket(
    PVM_REST_HTTP_RESPONSE_PACKET* ppResPacket
    )
{
    uint32_t dwError = 0;
    PVM_REST_HTTP_RESPONSE_PACKET pResPacket = NULL;

    PVM_REST_HTTP_STATUS_LINE      pStatusLine = NULL;
    PVM_REST_HTTP_GENERAL_HEADER   pGenHeader = NULL;
    PVM_REST_HTTP_RESPONSE_HEADER  pResHeader = NULL;
    PVM_REST_HTTP_ENTITY_HEADER    pEntityHeader= NULL;
    PVM_REST_HTTP_MESSAGE_BODY     pMessageBody = NULL;

    dwError = VmRESTAllocateStatusLine(
              &pStatusLine);
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTAllocateGeneralHeader(
              &pGenHeader);
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTAllocateResponseHeader(
              &pResHeader);
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTAllocateEntityHeader(
              &pEntityHeader);
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTAllocateMessageBody(
              &pMessageBody);
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTAllocateMemory(
                   sizeof(VM_REST_HTTP_RESPONSE_PACKET),
                   (void**)&pResPacket);
    BAIL_ON_VMREST_ERROR(dwError);

    pResPacket->statusLine = pStatusLine;
    pResPacket->generalHeader = pGenHeader;
    pResPacket->responseHeader = pResHeader;
    pResPacket->entityHeader = pEntityHeader;
    pResPacket->messageBody = pMessageBody;

    *ppResPacket = pResPacket;

cleanup:
    return dwError;
error:
    /* TODO: Write cleanup routines */
    goto cleanup;
}

uint32_t
VmRESTFreeHTTPResponsePacket(
    PVM_REST_HTTP_RESPONSE_PACKET* ppResPacket
    )
{
    uint32_t dwError = 0;
    BAIL_ON_VMREST_ERROR(dwError);



cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t 
VmRESTHTTPGetmethod(
    char*  line,
    char*  result
    )
{
    uint32_t dwError = 0;
    char*    buffer = NULL;
    
    /* method will be first letter in line */
    buffer = strtok(strdup(line)," ");
    if (strcmp(buffer,"GET") == 0)
    {
        strcpy(result,"GET");
    }
    else if (strcmp(buffer,"POST") == 0)
    {
        strcpy(result,"POST");
    }
    else if (strcmp(buffer,"DELETE") == 0)
    {
        strcpy(result,"DELETE");
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
    char*  line,
    char*  result
    )
{
    uint32_t  dwError = 0;
    char*     buffer = NULL;

    /* URI will be second letter in line */
    buffer = strtok(strdup(line)," ");
    buffer = strtok(NULL," ");
    if (buffer != NULL)
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
    char*  line,
    char*  result
    )
{
    uint32_t   dwError = 0;
    uint32_t   count   = 1;
    char*      buffer = NULL;

    /* Version will be third letter in line */
    
    buffer = strtok(strdup(line)," ");
    while (buffer != NULL)
    {   
        buffer = strtok(NULL," ");
        count++;
 
        if (count == 3)
        { 
           if ((strcmp(buffer, "HTTP/1.1")) == 0 )
           {
               strcpy(result,buffer);
               break;
           }
           else 
           {
               dwError = ERROR_NOT_SUPPORTED;
               BAIL_ON_VMREST_ERROR(dwError);
           }
        }
    }
cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VmRESTHTTPPopulateHeader(
    char*                        line,
    PVM_REST_HTTP_REQUEST_PACKET pReqPacket
    )
{
    uint32_t dwError = 0;
    uint32_t count = 0;
    char*    buffer = NULL; //[MAX_HTTP_HEADER_VAL_LEN] = {0};
    char     attribute[MAX_HTTP_HEADER_ATTR_LEN] = {0};
    char     value[MAX_HTTP_HEADER_VAL_LEN] = {0};

    buffer = strtok(strdup(line),":");
    while (buffer != NULL)
    {
        if (count == 0) 
        {
            strcpy(attribute,buffer);
        }
        if (count == 1)
        {
            strcpy(value,buffer);
        }
        count++;
        buffer = strtok(NULL, ":");
    }
    
    /* write the specific attribute header field after matching */

    if ((strcmp(attribute, "Accept")) == 0)
    {
        strcpy(pReqPacket->requestHeader->accept, value);
    }
    else if ((strcmp(attribute, "Accept-Charset")) == 0)
    {
        strcpy(pReqPacket->requestHeader->acceptCharSet, value);


    }
    else if ((strcmp(attribute, "Connection")) == 0)
    {
        strcpy(pReqPacket->generalHeader->connection, value); 
    

    }
    else if ((strcmp(attribute, "Content-Length")) == 0)
    {
        strcpy(pReqPacket->entityHeader->contentLength, value);


    }
    else if ((strcmp(attribute, "Content-Type")) == 0)
    {
        strcpy(pReqPacket->entityHeader->contentType, value);


    }
    else if ((strcmp(attribute, "Content-Encoding")) == 0)
    {
        strcpy(pReqPacket->entityHeader->contentEncoding, value);


    }
    else if ((strcmp(attribute, "Host")) == 0)
    {
        strcpy(pReqPacket->requestHeader->host, value);


    }
    else if ((strcmp(attribute, "From")) == 0)
    {
        strcpy(pReqPacket->requestHeader->from, value);


    }
    else 
    {
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_VMREST_ERROR(dwError);
    }
    /* keep on adding as required */

cleanup:
    return dwError;
error:
    goto cleanup;
}


uint32_t
VmRESTParseHTTPReqLine(
    uint32_t                      lineNo,
    char*                         line,
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
                      method
                      );
        BAIL_ON_VMREST_ERROR(dwError);
        
        strcpy(pReqPacket->requestLine->method, method);

        dwError = VmRESTHTTPGetReqURI(
                      line,
                      URI
                      );
        BAIL_ON_VMREST_ERROR(dwError);
        strcpy(pReqPacket->requestLine->uri, URI);

        dwError = VmRESTHTTPGetReqVersion(
                      line,
                      version
                      );
        BAIL_ON_VMREST_ERROR(dwError);
        strcpy(pReqPacket->requestLine->version, version);
         
    } else
    {
        /* These are header lines */
        dwError = VmRESTHTTPPopulateHeader(
                      line,
                      pReqPacket
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
    char*                        buffer,
    uint32_t                     packetLen,
    PVM_REST_HTTP_REQUEST_PACKET pReqPacket
    )
{
    uint32_t dwError = 0;
    uint32_t bytesRead = 0;
    uint32_t lineNo = 0;
    char*    reqLine = NULL;
    uint32_t lineLen = 0;

    reqLine = strtok(strdup(buffer), "\n");
    while (1)
    {
        lineNo++;
        lineLen = strlen(reqLine);
        bytesRead = (bytesRead + lineLen + 1);

        if(reqLine == NULL || bytesRead >= packetLen) {
            
            /* Copy the payload to http structure */
            memcpy(pReqPacket->messageBody->buffer, (buffer+bytesRead), (packetLen - bytesRead));
            break;
        } 
        
        /* call handler function with reqLine */
        
        dwError = VmRESTParseHTTPReqLine(
                      lineNo,
                      reqLine,
                      pReqPacket              
                      );
        BAIL_ON_VMREST_ERROR(dwError);
   
        reqLine = strtok(NULL, "\n");
    
    }


cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VmRESTCreateHTTPResponseMessage(
    PVM_REST_HTTP_RESPONSE_PACKET* pResPacket
    )
{
    uint32_t dwError = 0;
    BAIL_ON_VMREST_ERROR(dwError);



cleanup:
    return dwError;
error:
    goto cleanup;
}

static
uint32_t
VmRESTAllocateRequestLine(
    PVM_REST_HTTP_REQUEST_LINE* ppReqLine
    )
{
    uint32_t                   dwError = 0;
    PVM_REST_HTTP_REQUEST_LINE pReqLine = NULL;
    
    dwError = VmRESTAllocateMemory(
                  sizeof(VM_REST_HTTP_REQUEST_LINE),
                  (void**)&pReqLine);
    BAIL_ON_VMREST_ERROR(dwError);

    *ppReqLine = pReqLine;

cleanup:
    return dwError;
error:
    goto cleanup;
}

static
uint32_t
VmRESTAllocateStatusLine(
    PVM_REST_HTTP_STATUS_LINE *ppStatusLine
    )
{
    uint32_t                  dwError = 0;
    PVM_REST_HTTP_STATUS_LINE pStatusLine = NULL;

    dwError = VmRESTAllocateMemory(
                  sizeof(VM_REST_HTTP_STATUS_LINE),
                  (void**)&pStatusLine);
    BAIL_ON_VMREST_ERROR(dwError);

    *ppStatusLine = pStatusLine;

cleanup:
    return dwError;
error:
    goto cleanup;
}

static
uint32_t
VmRESTAllocateGeneralHeader(
    PVM_REST_HTTP_GENERAL_HEADER* ppGenHeader
    )
{
    uint32_t                     dwError = 0;
    PVM_REST_HTTP_GENERAL_HEADER pGenHeader = NULL;

    dwError = VmRESTAllocateMemory(
                  sizeof(VM_REST_HTTP_GENERAL_HEADER),
                  (void**)&pGenHeader);
    BAIL_ON_VMREST_ERROR(dwError);

    *ppGenHeader = pGenHeader;

cleanup:
    return dwError;
error:
    goto cleanup;
}

static
uint32_t
VmRESTAllocateRequestHeader(
    PVM_REST_HTTP_REQUEST_HEADER* ppReqHeader
    )
{
    uint32_t                     dwError = 0;
    PVM_REST_HTTP_REQUEST_HEADER pReqHeader = NULL;

    dwError = VmRESTAllocateMemory(
                  sizeof(VM_REST_HTTP_REQUEST_HEADER),
                  (void**)&pReqHeader);
    BAIL_ON_VMREST_ERROR(dwError);

    *ppReqHeader = pReqHeader;

cleanup:
    return dwError;
error:
    goto cleanup;
}

static
uint32_t
VmRESTAllocateResponseHeader(
    PVM_REST_HTTP_RESPONSE_HEADER* ppResHeader
    )
{
    uint32_t                      dwError = 0;
    PVM_REST_HTTP_RESPONSE_HEADER pResHeader = NULL;

    dwError = VmRESTAllocateMemory(
                  sizeof(VM_REST_HTTP_RESPONSE_HEADER),
                  (void**)&pResHeader);
    BAIL_ON_VMREST_ERROR(dwError);

    *ppResHeader = pResHeader;

cleanup:
    return dwError;
error:
    goto cleanup;
}

static
uint32_t
VmRESTAllocateEntityHeader(
    PVM_REST_HTTP_ENTITY_HEADER  *ppEntityHeader
    )
{
    uint32_t                    dwError = 0;
    PVM_REST_HTTP_ENTITY_HEADER pEntityHeader = NULL;

    dwError = VmRESTAllocateMemory(
                  sizeof(VM_REST_HTTP_ENTITY_HEADER),
                  (void**)&pEntityHeader);
    BAIL_ON_VMREST_ERROR(dwError);

    *ppEntityHeader = pEntityHeader;

cleanup:
    return dwError;
error:
    goto cleanup;
}

static
uint32_t
VmRESTAllocateMessageBody(
    PVM_REST_HTTP_MESSAGE_BODY  *ppMsgBody
    )
{
    uint32_t                   dwError = 0;
    PVM_REST_HTTP_MESSAGE_BODY pMsgBody = NULL;

    dwError = VmRESTAllocateMemory(
                  sizeof(VM_REST_HTTP_MESSAGE_BODY),
                  (void**)&pMsgBody);
    BAIL_ON_VMREST_ERROR(dwError);

    *ppMsgBody = pMsgBody;

cleanup:
    return dwError;
error:
    goto cleanup;
}



