
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
    PVM_REST_HTTP_REQUEST_LINE*      ppReqLine
    );

static
void
VmRESTFreeRequestLine(
    PVM_REST_HTTP_REQUEST_LINE       pReqLine
    );

static
uint32_t
VmRESTAllocateStatusLine(
    PVM_REST_HTTP_STATUS_LINE*       ppStatusLine
    );

static
void
VmRESTFreeStatusLine(
    PVM_REST_HTTP_STATUS_LINE        pStatusLine
    );

static
uint32_t
VmRESTAllocateMiscQueue(
    PMISC_HEADER_QUEUE*              ppMiscHeaderQueue
    );

static
void
VmRESTFreeMiscQueue(
    PMISC_HEADER_QUEUE               pMiscHeaderQueue
    );

static
uint32_t
VmRESTAllocateMessageBody(
    PVM_REST_HTTP_MESSAGE_BODY*      ppMsgBody
    );

static
void
VmRESTFreeMessageBody(
    PVM_REST_HTTP_MESSAGE_BODY       pMsgBody
    );


uint32_t
VmRESTAllocateHTTPRequestPacket(
    PVM_REST_HTTP_REQUEST_PACKET*    ppReqPacket
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    PVM_REST_HTTP_REQUEST_PACKET     pReqPacket = NULL;
    PVM_REST_HTTP_REQUEST_LINE       pReqLine = NULL;
    PMISC_HEADER_QUEUE               pMiscHeaderQueue = NULL;

    dwError = VmRESTAllocateMemory(
                  sizeof(VM_REST_HTTP_REQUEST_PACKET),
                  (void**)&pReqPacket
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTAllocateRequestLine(
                  &pReqLine
                  );
    BAIL_ON_VMREST_ERROR(dwError);
    pReqPacket->requestLine = pReqLine;

    dwError = VmRESTAllocateMiscQueue(
                  &pMiscHeaderQueue
                  );
    BAIL_ON_VMREST_ERROR(dwError);
    pReqPacket->miscHeader = pMiscHeaderQueue;

    *ppReqPacket = pReqPacket;

cleanup:
    return dwError;
error:
    VmRESTFreeHTTPRequestPacket(
        &pReqPacket
        );
    *ppReqPacket = NULL;
    goto cleanup;
}

void
VmRESTFreeHTTPRequestPacket(
    PVM_REST_HTTP_REQUEST_PACKET*    ppReqPacket
    )
{
    PVM_REST_HTTP_REQUEST_PACKET     pReqPacket = NULL;
    pReqPacket = *ppReqPacket;
    if (pReqPacket)
    {
        if (pReqPacket->requestLine)
        {
            VmRESTFreeRequestLine(pReqPacket->requestLine);
        }
        if (pReqPacket->miscHeader)
        {
            VmRESTFreeMiscQueue(pReqPacket->miscHeader);
        }

        pReqPacket->requestLine = NULL;
        pReqPacket->miscHeader = NULL;

        VmRESTFreeMemory(pReqPacket);

        *ppReqPacket = NULL;
    }
}

uint32_t
VmRESTAllocateHTTPResponsePacket(
    PVM_REST_HTTP_RESPONSE_PACKET*   ppResPacket
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    PVM_REST_HTTP_RESPONSE_PACKET    pResPacket = NULL;
    PVM_REST_HTTP_STATUS_LINE        pStatusLine = NULL;
    PVM_REST_HTTP_MESSAGE_BODY       pMessageBody = NULL;
    PMISC_HEADER_QUEUE               pMiscHeaderQueue = NULL;

    dwError = VmRESTAllocateStatusLine(
                  &pStatusLine
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTAllocateMessageBody(
                  &pMessageBody
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTAllocateMemory(
                  sizeof(VM_REST_HTTP_RESPONSE_PACKET),
                  (void**)&pResPacket
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTAllocateMiscQueue(
                  &pMiscHeaderQueue
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    pResPacket->statusLine = pStatusLine;
    pResPacket->messageBody = pMessageBody;
    pResPacket->miscHeader = pMiscHeaderQueue;

    *ppResPacket = pResPacket;

cleanup:
    return dwError;
error:
    VmRESTFreeHTTPResponsePacket(
        &pResPacket
        );
    *ppResPacket = NULL;
    goto cleanup;
}

void
VmRESTFreeHTTPResponsePacket(
    PVM_REST_HTTP_RESPONSE_PACKET*   ppResPacket
    )
{
    PVM_REST_HTTP_RESPONSE_PACKET    pResPacket = NULL;

    pResPacket = *ppResPacket;
    if (pResPacket)
    {
        if (pResPacket->statusLine)
        {
            VmRESTFreeStatusLine(pResPacket->statusLine);
        }
        if (pResPacket->messageBody)
        {
            VmRESTFreeMessageBody(pResPacket->messageBody);
        }
        if (pResPacket->miscHeader)
        {
            VmRESTFreeMiscQueue(pResPacket->miscHeader);
        }

        pResPacket->statusLine = NULL;
        pResPacket->messageBody = NULL;
        pResPacket->miscHeader = NULL;

        VmRESTFreeMemory(pResPacket);

        *ppResPacket = NULL;
    }
}

uint32_t
VmRESTAllocateEndPoint(
     PREST_ENDPOINT*                 ppEndPoint
     )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    PREST_ENDPOINT                   pEndPoint = NULL;
    char*                            pEndPointURI = NULL;
    PREST_PROCESSOR                  pHandler = NULL;

    dwError = VmRESTAllocateMemory(
                  sizeof(REST_ENDPOINT),
                  (void**)&pEndPoint
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTAllocateMemory(
                  MAX_URI_LEN,
                  (void**)&pEndPointURI
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTAllocateMemory(
                  sizeof(REST_PROCESSOR),
                  (void**)&pHandler
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    pEndPoint->pszEndPointURI = pEndPointURI;
    pEndPoint->pHandler = pHandler;

    *ppEndPoint = pEndPoint;
     
cleanup:
    return dwError;
error:
    if (pEndPoint)
    {
        VmRESTFreeEndPoint(
            pEndPoint
            );
    }
    goto cleanup;
}

void
VmRESTFreeEndPoint(
    PREST_ENDPOINT                   pEndPoint
    )
{
    if (pEndPoint)
    {
        if (pEndPoint->pszEndPointURI)
        {
            VmRESTFreeMemory(
                pEndPoint->pszEndPointURI
                );
            pEndPoint->pszEndPointURI = NULL;
        }
        if (pEndPoint->pHandler)
        {
            VmRESTFreeMemory(
                pEndPoint->pHandler
                );
            pEndPoint->pHandler = NULL;
        }
        VmRESTFreeMemory(pEndPoint);
    }
}

static
uint32_t
VmRESTAllocateRequestLine(
    PVM_REST_HTTP_REQUEST_LINE*      ppReqLine
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    PVM_REST_HTTP_REQUEST_LINE       pReqLine = NULL;

    dwError = VmRESTAllocateMemory(
                  sizeof(VM_REST_HTTP_REQUEST_LINE),
                  (void**)&pReqLine
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    *ppReqLine = pReqLine;

cleanup:
    return dwError;
error:
    goto cleanup;
}

static
void
VmRESTFreeRequestLine(
    PVM_REST_HTTP_REQUEST_LINE       pReqLine
    )
{
    if (pReqLine)
    {
        VmRESTFreeMemory(pReqLine);
    }
}

static
uint32_t
VmRESTAllocateStatusLine(
    PVM_REST_HTTP_STATUS_LINE*       ppStatusLine
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    PVM_REST_HTTP_STATUS_LINE        pStatusLine = NULL;

    dwError = VmRESTAllocateMemory(
                  sizeof(VM_REST_HTTP_STATUS_LINE),
                  (void**)&pStatusLine
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    *ppStatusLine = pStatusLine;

cleanup:
    return dwError;
error:
    goto cleanup;
}

static
void
VmRESTFreeStatusLine(
    PVM_REST_HTTP_STATUS_LINE        pStatusLine
    )
{
    if (pStatusLine)
    {
        VmRESTFreeMemory(pStatusLine
            );
    }
}

uint32_t
VmRESTAllocateMessageBody(
    PVM_REST_HTTP_MESSAGE_BODY*      ppMsgBody
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    PVM_REST_HTTP_MESSAGE_BODY       pMsgBody = NULL;

    dwError = VmRESTAllocateMemory(
                  sizeof(VM_REST_HTTP_MESSAGE_BODY),
                  (void**)&pMsgBody
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    *ppMsgBody = pMsgBody;

cleanup:
    return dwError;
error:
    goto cleanup;
}

static
void
VmRESTFreeMessageBody(
    PVM_REST_HTTP_MESSAGE_BODY       pMsgBody
    )
{
    if (pMsgBody)
    {
        VmRESTFreeMemory(pMsgBody
            );
    }
}

static
uint32_t
VmRESTAllocateMiscQueue(
    PMISC_HEADER_QUEUE*              ppMiscHeaderQueue
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    PMISC_HEADER_QUEUE               pMiscQueue = NULL;

    dwError = VmRESTAllocateMemory(
                  sizeof(VM_REST_HTTP_MESSAGE_BODY),
                  (void**)&pMiscQueue
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    *ppMiscHeaderQueue = pMiscQueue;

cleanup:
    return dwError;
error:
    goto cleanup;
}

static
void
VmRESTFreeMiscQueue(
    PMISC_HEADER_QUEUE               pMiscHeaderQueue
    )
{
    if (pMiscHeaderQueue)
    {
        VmRESTRemoveAllHTTPMiscHeader(
            pMiscHeaderQueue
            );
        VmRESTFreeMemory(pMiscHeaderQueue
            );
    }
}

