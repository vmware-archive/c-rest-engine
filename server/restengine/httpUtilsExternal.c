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

#ifdef WIN32
__declspec(dllexport)
#endif
uint32_t
VmRESTGetHttpMethod(
    PREST_REQUEST                    pRequest,
    char**                           ppResponse
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    size_t                           methodLen = 0;
    char*                            pMethod = NULL;

    if (!(pRequest) || !(pRequest->requestLine) || !(ppResponse))
    {
        dwError = VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    methodLen = strlen(pRequest->requestLine->method);
    if (methodLen == 0)
    {
        dwError = VMREST_HTTP_VALIDATION_FAILED;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTAllocateMemory(
                 MAX_METHOD_LEN,
                 (void **)&pMethod
                 );
    BAIL_ON_VMREST_ERROR(dwError);

    strncpy(pMethod,pRequest->requestLine->method, (MAX_METHOD_LEN -1));

    *ppResponse = pMethod;

cleanup:
    return dwError;
error:
    if (ppResponse)
    {
        *ppResponse = NULL;
    }
    goto cleanup;
}

uint32_t
VmRESTGetHttpURI(
    PREST_REQUEST                    pRequest,
    bool                             bDecoded,
    char**                           ppResponse
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    size_t                           uriLen = 0;
    char*                            pHttpURI = NULL;

    if (!(pRequest) || !(pRequest->requestLine) || !(ppResponse))
    {
        dwError = VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    uriLen = strlen(pRequest->requestLine->uri);
    if (uriLen == 0 || uriLen > MAX_URI_LEN)
    {
        dwError = VMREST_HTTP_VALIDATION_FAILED;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTAllocateMemory(
                 MAX_URI_LEN,
                 (void **)&pHttpURI
                 );
    BAIL_ON_VMREST_ERROR(dwError);

    if (bDecoded)
    {
        VmRESTDecodeEncodedURLString(
            pRequest->requestLine->uri,
            pHttpURI
            );
    }
    else
    {
        strncpy(pHttpURI, pRequest->requestLine->uri, (MAX_URI_LEN - 1));
    }

    *ppResponse = pHttpURI;

cleanup:
    return dwError;
error:
    if (ppResponse)
    {
        *ppResponse = NULL;
    }
    goto cleanup;
}

uint32_t
VmRESTGetHttpVersion(
    PREST_REQUEST                    pRequest,
    char**                           ppResponse
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    size_t                           versionLen = 0;
    char*                            pVersion = NULL;

    if (!(pRequest) || !(pRequest->requestLine) || !(ppResponse))
    {
        dwError = VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    versionLen = strlen(pRequest->requestLine->version);
    if (versionLen == 0 || versionLen > MAX_VERSION_LEN)
    {
        dwError = VMREST_HTTP_VALIDATION_FAILED;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTAllocateMemory(
                 MAX_VERSION_LEN,
                 (void **)&pVersion
                 );
    BAIL_ON_VMREST_ERROR(dwError);

    strncpy(pVersion, pRequest->requestLine->version, (MAX_VERSION_LEN -1));

    *ppResponse = pVersion;

cleanup:
    return dwError;
error:
    if (ppResponse)
    {
        *ppResponse = NULL;
    }
    goto cleanup;
}

uint32_t
VmRESTGetHttpHeader(
    PREST_REQUEST                    pRequest,
    char const*                      pcszHeader,
    char**                           ppszResponse
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    size_t                           headerValLen = 0;
    char*                            headerValue = NULL;
    char*                            temp = NULL;

    if (!(pRequest) || !(pcszHeader) || !(ppszResponse))
    {
        dwError = VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTGetHTTPMiscHeader(
                  pRequest->miscHeader,
                  pcszHeader,
                  &temp
                  );
    BAIL_ON_VMREST_ERROR(dwError);
    if (temp != NULL)
    {
         headerValLen = strlen(temp);
         if (headerValLen == 0 || headerValLen > MAX_HTTP_HEADER_VAL_LEN)
         {
             dwError = VMREST_HTTP_VALIDATION_FAILED;
         }
         BAIL_ON_VMREST_ERROR(dwError);
         dwError = VmRESTAllocateMemory(
                       MAX_HTTP_HEADER_VAL_LEN,
                       (void **)&headerValue
                       );
         BAIL_ON_VMREST_ERROR(dwError);
         strncpy(headerValue, temp, (MAX_HTTP_HEADER_VAL_LEN - 1));
         *ppszResponse = headerValue;
    }
    else
    {
        *ppszResponse = NULL;
    }

cleanup:
    return dwError;
error:
    if (ppszResponse)
    {
        *ppszResponse = NULL;
    }
    goto cleanup;
}

uint32_t
VmRESTGetHttpPayload(
    PVMREST_HANDLE                   pRESTHandle,
    PREST_REQUEST                    pRequest,
    char*                            response,
    uint32_t*                        bytesRead
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    uint32_t                         nBytes = 0;

    if (!pRequest || !response  || !bytesRead)
    {
        VMREST_LOG_ERROR(pRESTHandle,"%s","Invalid params");
        dwError = VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);
    *bytesRead = 0;

    if (pRequest->nBytesGetPayload <= pRequest->nPayload)
    {
        nBytes = (((pRequest->nPayload - pRequest->nBytesGetPayload) >= MAX_DATA_BUFFER_LEN) ? MAX_DATA_BUFFER_LEN : (pRequest->nPayload - pRequest->nBytesGetPayload));
        memcpy(response, (pRequest->pszPayload + pRequest->nBytesGetPayload), nBytes);
        *bytesRead = nBytes;
        pRequest->nBytesGetPayload += nBytes;
    }
    
    if (pRequest->nBytesGetPayload < pRequest->nPayload)
    {
        dwError = REST_ENGINE_MORE_IO_REQUIRED;
    }
    else if (pRequest->nBytesGetPayload == pRequest->nPayload)
    {
        dwError = REST_ENGINE_IO_COMPLETED;
    }
    else
    {
        VMREST_LOG_ERROR(pRESTHandle,"%s","Error in payload length");
        dwError = VMREST_HTTP_INVALID_PARAMS;
    }

    

cleanup:

    return dwError;
error:

    goto cleanup;

}

uint32_t
VmRESTSetHttpPayload(
    PVMREST_HANDLE                   pRESTHandle,
    PREST_RESPONSE*                  ppResponse,
    char const*                      buffer,
    uint32_t                         dataLen,
    uint32_t*                        bytesWritten
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    uint32_t                         contentLen = 0;
    PREST_RESPONSE                   pResponse = NULL;
    char*                            contentLength = NULL;
    char*                            transferEncoding = NULL;


    if (!ppResponse  || (*ppResponse == NULL) || !buffer || !bytesWritten)
    {
        VMREST_LOG_ERROR(pRESTHandle,"%s","Invalid params");
        dwError = VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    pResponse = *ppResponse;
    *bytesWritten = 0;

    dwError = VmRESTGetHttpResponseHeader(
                  pResponse,
                  "Content-Length",
                  &contentLength
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTGetHttpResponseHeader(
                  pResponse,
                  "Transfer-Encoding",
                  &transferEncoding
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    /**** Either of Content-Length or chunked-Encoding header must be set ****/
    if ((contentLength != NULL) && (strlen(contentLength) > 0))
    {
        contentLen = atoi(contentLength);
        if (contentLen <= MAX_DATA_BUFFER_LEN)
        {
            memcpy(pResponse->messageBody->buffer, buffer, contentLen);
        }
        else
        {
            VMREST_LOG_ERROR(pRESTHandle,"Invalid content length %u", contentLen);
            dwError = VMREST_HTTP_VALIDATION_FAILED;
        }
        BAIL_ON_VMREST_ERROR(dwError);

        dwError = VmRESTSendHeaderAndPayload(
                      pRESTHandle,
                      ppResponse
                      );
       VMREST_LOG_DEBUG(pRESTHandle,"Sending Header and Payload done, returned code %u", dwError);
       BAIL_ON_VMREST_ERROR(dwError);
       pResponse->bHeaderSent = TRUE;
       *bytesWritten = contentLen;
       dwError = REST_ENGINE_IO_COMPLETED;
    }
    else if ((transferEncoding != NULL) && (strcmp(transferEncoding, "chunked") == 0))
    {
         if (dataLen > MAX_DATA_BUFFER_LEN)
         {
             VMREST_LOG_ERROR(pRESTHandle,"Chunked data length %u not allowed", dataLen);
             dwError = VMREST_HTTP_VALIDATION_FAILED;
         }
         BAIL_ON_VMREST_ERROR(dwError);

         memcpy(pResponse->messageBody->buffer, buffer, dataLen);
         if (pResponse->bHeaderSent == FALSE)
         {
             /**** Send Header first ****/
             dwError = VmRESTSendHeader(
                           pRESTHandle,
                           ppResponse
                           );
             BAIL_ON_VMREST_ERROR(dwError);
             pResponse->bHeaderSent = TRUE;
         }
         dwError = VmRESTSendChunkedPayload(
                       pRESTHandle,
                       ppResponse,
                       dataLen
                       );
         BAIL_ON_VMREST_ERROR(dwError);
         if (dataLen == 0)
         {
             dwError = REST_ENGINE_IO_COMPLETED;
         }
         else
         {
             dwError = REST_ENGINE_MORE_IO_REQUIRED;
         }
         *bytesWritten = dataLen;
         
    }
    else
    {
        VMREST_LOG_ERROR(pRESTHandle,"%s","Both Content length and TransferEncoding missing");
        dwError = VMREST_HTTP_VALIDATION_FAILED;
        BAIL_ON_VMREST_ERROR(dwError);
    }

cleanup:
    return dwError;
error:
    VMREST_LOG_ERROR(pRESTHandle,"%s","Set Payload Failed");
    goto cleanup;
}


uint32_t
VmRESTSetHttpHeader(
    PREST_RESPONSE*                  ppResponse,
    char const*                      header,
    char const*                      value
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    PREST_RESPONSE                   pResponse = NULL;

    if (!ppResponse  || (*ppResponse == NULL) || !header  || !value)
    {
        dwError = VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    pResponse = *ppResponse;

    dwError = VmRESTSetHTTPMiscHeader(
                  pResponse->miscHeader,
                  header,
                  value
                  );
    BAIL_ON_VMREST_ERROR(dwError);
cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VmRESTSetHttpStatusCode(
    PREST_RESPONSE*                  ppResponse,
    char const*                      statusCode
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    size_t                           statusLen = 0;
    PREST_RESPONSE    pResponse = NULL;

    if (!ppResponse || (*ppResponse == NULL) || !statusCode)
    {
        /* Response object not allocated any memory */
        dwError = VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    pResponse = *ppResponse;
    statusLen = strlen(statusCode);

    if (statusLen !=  3)
    {
        dwError = VMREST_HTTP_VALIDATION_FAILED;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    if ((atoi(statusCode) < 100) || (atoi(statusCode) > 600))
    {
        dwError = VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    strcpy(pResponse->statusLine->statusCode, statusCode);

cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VmRESTSetHttpStatusVersion(
    PREST_RESPONSE*                  ppResponse,
    char const*                      version
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    size_t                           versionLen = 0;
    PREST_RESPONSE                   pResponse = NULL;


    if (!ppResponse || (*ppResponse == NULL) || !version)
    {
        dwError = VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    pResponse = *ppResponse;

    versionLen = strlen(version);

    if (versionLen == 0 || versionLen > MAX_VERSION_LEN)
    {
        dwError = VMREST_HTTP_VALIDATION_FAILED;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    if (!(strcmp(version, "HTTP/1.1") == 0) || (strcmp(version, "HTTP/1.0") == 0))
    {
        dwError = VMREST_HTTP_VALIDATION_FAILED;
    }
    BAIL_ON_VMREST_ERROR(dwError);
    
    strcpy(pResponse->statusLine->version, version);

cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VmRESTSetHttpReasonPhrase(
    PREST_RESPONSE*                  ppResponse,
    char const*                      reasonPhrase
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    PREST_RESPONSE                   pResponse = NULL;


    if (!ppResponse  || (*ppResponse == NULL) || !reasonPhrase 
        || (strlen(reasonPhrase) == 0) || (strlen(reasonPhrase) > MAX_REA_PHRASE_LEN))
    {
        dwError = VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    pResponse = *ppResponse;

    memset(pResponse->statusLine->reason_phrase, '\0', MAX_REA_PHRASE_LEN);
    strncpy(pResponse->statusLine->reason_phrase, reasonPhrase, (MAX_REA_PHRASE_LEN - 1));

cleanup:
    return dwError;
error:
    goto cleanup;
}

