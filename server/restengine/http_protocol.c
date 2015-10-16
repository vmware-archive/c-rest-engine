
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
VmRESTParseAndPopulateRawHTTPMessage(
    char*                        buffer,
    PVM_REST_HTTP_REQUEST_PACKET* pReqPacket
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
    uint32_t dwError = 0;
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
    uint32_t dwError = 0;
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
    uint32_t dwError = 0;
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
    uint32_t dwError = 0;
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
    uint32_t dwError = 0;
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
    uint32_t dwError = 0;
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
    uint32_t dwError = 0;
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



