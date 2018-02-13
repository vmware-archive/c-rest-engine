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

        if (pReqPacket->pszPayload)
        {
            VmRESTFreeMemory(pReqPacket->pszPayload);
            pReqPacket->pszPayload = NULL;
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

    dwError = VmRESTAllocateMemory(
                  sizeof(VM_REST_HTTP_RESPONSE_PACKET),
                  (void**)&pResPacket
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTAllocateStatusLine(
                  &pStatusLine
                  );
    BAIL_ON_VMREST_ERROR(dwError);
    pResPacket->statusLine = pStatusLine;

    dwError = VmRESTAllocateMessageBody(
                  &pMessageBody
                  );
    BAIL_ON_VMREST_ERROR(dwError);
    pResPacket->messageBody = pMessageBody;

    dwError = VmRESTAllocateMiscQueue(
                  &pMiscHeaderQueue
                  );
    BAIL_ON_VMREST_ERROR(dwError);
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
    pEndPoint->pszEndPointURI = pEndPointURI;

    dwError = VmRESTAllocateMemory(
                  sizeof(REST_PROCESSOR),
                  (void**)&pHandler
                  );
    BAIL_ON_VMREST_ERROR(dwError);
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

uint32_t
VmRESTAllocateHandle(
     PVMREST_HANDLE*                 ppRESTHandle
     )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    PVMREST_HANDLE                   pRESTHandle = NULL;
    PVM_SOCK_PACKAGE                 pPackage = NULL;
    PVM_SOCK_SSL_INFO                pSSLInfo = NULL;
    PREST_ENG_GLOBALS                pInstanceGlobal = NULL;
    PVMREST_SOCK_CONTEXT             pSockContext = NULL;
    PVM_REST_CONFIG                  pRESTConfig = NULL;

    dwError = VmRESTAllocateMemory(
                  sizeof(VMREST_HANDLE),
                  (void**)&pRESTHandle
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTAllocateMemory(
                  sizeof(REST_ENG_GLOBALS),
                  (void **)&pInstanceGlobal
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    pRESTHandle->pInstanceGlobal = pInstanceGlobal;

    dwError = VmRESTAllocateMemory(
                  sizeof(VM_REST_CONFIG),
                  (void**)&pRESTConfig
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    pRESTHandle->pRESTConfig = pRESTConfig;

    dwError = VmRESTAllocateMemory(
                  sizeof(VMREST_SOCK_CONTEXT),
                  (PVOID*)&pSockContext
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    pRESTHandle->pSockContext = pSockContext;

    dwError = VmRESTAllocateMemory(
                  sizeof(VM_SOCK_PACKAGE),
                  (void **)&pPackage
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    pRESTHandle->pPackage = pPackage;

    dwError = VmRESTAllocateMemory(
                  sizeof(VM_SOCK_SSL_INFO),
                  (void **)&pSSLInfo
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    pRESTHandle->pSSLInfo = pSSLInfo;

    pRESTHandle->pHttpHandler = NULL;

    pRESTHandle->instanceState = VMREST_INSTANCE_UNINITIALIZED;

    *ppRESTHandle = pRESTHandle;

cleanup:

    return dwError;

error:
    if (pRESTHandle)
    {
        VmRESTFreeHandle(
            pRESTHandle
            );
    }
    goto cleanup;
}

void
VmRESTFreeHandle(
     PVMREST_HANDLE                  pRESTHandle
     )
{
    if (pRESTHandle)
    {
        if (pRESTHandle->pInstanceGlobal)
        {
            VmRESTFreeMemory(pRESTHandle->pInstanceGlobal);
            pRESTHandle->pInstanceGlobal = NULL;
        }

        if (pRESTHandle->pRESTConfig)
        {
             VmRESTFreeMemory(pRESTHandle->pRESTConfig);
             pRESTHandle->pRESTConfig = NULL;
        }

        if (pRESTHandle->pSockContext)
        {
            VmRESTFreeMemory(pRESTHandle->pSockContext);
            pRESTHandle->pSockContext = NULL;
        }

        if (pRESTHandle->pPackage)
        {
            VmRESTFreeMemory(pRESTHandle->pPackage);
            pRESTHandle->pPackage = NULL;
        }

        if (pRESTHandle->pSSLInfo)
        {
            VmRESTFreeMemory(pRESTHandle->pSSLInfo);
            pRESTHandle->pSSLInfo = NULL;
        }

        VmRESTFreeMemory(pRESTHandle);
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
                  sizeof(MISC_HEADER_QUEUE),
                  (void**)&pMiscQueue
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    pMiscQueue->head = NULL;

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

