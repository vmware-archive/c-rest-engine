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
VmRESTGetHttpMethod(
    PREST_REQUEST                    pRequest,
    PSTR                             response
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    uint32_t                         methodLen = 0;
    uint32_t                         methodNo  = 0;

    if ((pRequest == NULL) || (pRequest->requestLine == NULL) || (response == NULL))
    {
        VMREST_LOG_DEBUG("VmRESTGetHttpMethod(): Invalid params");
        dwError = VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    methodLen = strlen(pRequest->requestLine->method);
    if (methodLen == 0 || methodLen > MAX_METHOD_LEN)
    {
        VMREST_LOG_DEBUG("VmRESTGetHttpMethod(): Method seems invalid");
        dwError = VMREST_HTTP_VALIDATION_FAILED;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTMapMethodToEnum(
                  pRequest->requestLine->method,
                  &methodNo
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    if ((methodNo >= HTTP_METHOD_GET) && (methodNo <= HTTP_METHOD_CONNECT))
    {
        strcpy(response, pRequest->requestLine->method);
    }
    else
    {
        VMREST_LOG_DEBUG("VmRESTGetHttpMethod(): Method name in request object invalid");
        dwError = VMREST_HTTP_VALIDATION_FAILED;
    }
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VmRESTGetHttpURI(
    PREST_REQUEST                    pRequest,
    PSTR                             response
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    uint32_t                         uriLen = 0;

    if ((pRequest == NULL) || (pRequest->requestLine == NULL) || (response == NULL))
    {
        VMREST_LOG_DEBUG("VmRESTGetHttpURI(): Invalid params");
        dwError = VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    uriLen = strlen(pRequest->requestLine->uri);
    if (uriLen == 0 || uriLen > MAX_URI_LEN)
    {
        VMREST_LOG_DEBUG("VmRESTGetHttpURI(): URI seems invalid in request object");
        dwError = VMREST_HTTP_VALIDATION_FAILED;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    strcpy(response, pRequest->requestLine->uri);

cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VmRESTGetHttpVersion(
    PREST_REQUEST                    pRequest,
    PSTR                             response
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    uint32_t                         versionLen = 0;

    if ((pRequest == NULL) || (pRequest->requestLine == NULL) || (response == NULL))
    {
        VMREST_LOG_DEBUG("VmRESTGetHttpVersion(): Invalid params");
        dwError = VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    versionLen = strlen(pRequest->requestLine->version);
    if (versionLen == 0 || versionLen > MAX_VERSION_LEN)
    {
        VMREST_LOG_DEBUG("VmRESTGetHttpVersion(): Version info invalid in request object");
        dwError = VMREST_HTTP_VALIDATION_FAILED;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    strcpy(response, pRequest->requestLine->version);

cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VmRESTGetHttpHeader(
    PREST_REQUEST                    pRequest,
    PCSTR                            header,
    PSTR                             response
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    uint32_t                         headerNo = 0;
    uint32_t                         headerValLen = 0;
    uint32_t                         resStatus = 0;

    if ((pRequest == NULL) || (header == NULL) || (response == NULL))
    {
        VMREST_LOG_DEBUG("VmRESTGetHttpHeader(): Invalid params");
        dwError = VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTMapHeaderToEnum(
                  header,
                  &headerNo,
                  &resStatus
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    if ((headerNo < HTTP_REQUEST_HEADER_ACCEPT) || (headerNo > HTTP_ENTITY_HEADER_CONTENT_TYPE))
    {
        VMREST_LOG_DEBUG("VmRESTGetHttpHeader: Seems like invalid header in request object");
        dwError = VMREST_HTTP_VALIDATION_FAILED;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    /* No response header data is returned */
    if ((headerNo >= HTTP_RESPONSE_HEADER_ACCEPT_RANGE) && (headerNo <= HTTP_RESPONSE_HEADER_SERVER))
    {
        VMREST_LOG_DEBUG("VmRESTGetHttpHeader: Not a request header, its a response header");
        dwError = VMREST_HTTP_VALIDATION_FAILED;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    switch (headerNo)
    {
        case HTTP_REQUEST_HEADER_ACCEPT:
                 headerValLen = strlen(pRequest->requestHeader->accept);
                 if (headerValLen > 0 && headerValLen < MAX_HTTP_HEADER_VAL_LEN)
                 {
                     strcpy(response, pRequest->requestHeader->accept);
                 }
                 else
                 {
                      dwError = VMREST_HTTP_VALIDATION_FAILED;
                 }
                 break;
        case HTTP_REQUEST_HEADER_ACCEPT_CHARSET:
                 headerValLen = strlen(pRequest->requestHeader->acceptCharSet);
                 if (headerValLen > 0 && headerValLen < MAX_HTTP_HEADER_VAL_LEN)
                 {
                     strcpy(response, pRequest->requestHeader->acceptCharSet);
                 }
                 else
                 {
                      dwError = VMREST_HTTP_VALIDATION_FAILED;
                 }
                 break;
        case HTTP_REQUEST_HEADER_ACCEPT_ENCODING:
                 headerValLen = strlen(pRequest->requestHeader->acceptEncoding);
                 if (headerValLen > 0 && headerValLen < MAX_HTTP_HEADER_VAL_LEN)
                 {
                     strcpy(response, pRequest->requestHeader->acceptEncoding);
                 }
                 else
                 {
                      dwError = VMREST_HTTP_VALIDATION_FAILED;
                 }
                 break;
        case HTTP_REQUEST_HEADER_ACCEPT_LANGUAGE:
                 headerValLen = strlen(pRequest->requestHeader->acceptLanguage);
                 if (headerValLen > 0 && headerValLen < MAX_HTTP_HEADER_VAL_LEN)
                 {
                     strcpy(response, pRequest->requestHeader->acceptLanguage);
                 }
                 else
                 {
                     dwError = VMREST_HTTP_VALIDATION_FAILED;
                 }
                 break;
        case HTTP_REQUEST_HEADER_ACCEPT_AUTHORIZATION:
                 headerValLen = strlen(pRequest->requestHeader->authorization);
                 if (headerValLen > 0 && headerValLen < MAX_HTTP_HEADER_VAL_LEN)
                 {
                     strcpy(response, pRequest->requestHeader->authorization);
                 }
                 else
                 {
                     dwError = VMREST_HTTP_VALIDATION_FAILED;
                 }
                 break;
       case HTTP_REQUEST_HEADER_FROM:
                 headerValLen = strlen(pRequest->requestHeader->from);
                 if (headerValLen > 0 && headerValLen < MAX_HTTP_HEADER_VAL_LEN)
                 {
                     strcpy(response, pRequest->requestHeader->from);
                 }
                 else
                 {
                     dwError = VMREST_HTTP_VALIDATION_FAILED;
                 }
                 break;
       case HTTP_REQUEST_HEADER_HOST:
                 headerValLen = strlen(pRequest->requestHeader->host);
                 if (headerValLen > 0 && headerValLen < MAX_HTTP_HEADER_VAL_LEN)
                 {
                     strcpy(response, pRequest->requestHeader->host);
                 }
                 else
                 {
                     dwError = VMREST_HTTP_VALIDATION_FAILED;
                 }
                 break;
       case HTTP_REQUEST_HEADER_REFERER:
                 headerValLen = strlen(pRequest->requestHeader->referer);
                 if (headerValLen > 0 && headerValLen < MAX_HTTP_HEADER_VAL_LEN)
                 {
                     strcpy(response, pRequest->requestHeader->referer);
                 }
                 else
                 {
                     dwError = VMREST_HTTP_VALIDATION_FAILED;
                 }
                 break;
       case HTTP_GENERAL_HEADER_CACHE_CONTROL:
                 headerValLen = strlen(pRequest->generalHeader->cacheControl);
                 if (headerValLen > 0 && headerValLen < MAX_HTTP_HEADER_VAL_LEN)
                 {
                     strcpy(response, pRequest->generalHeader->cacheControl);
                 }
                 else
                 {
                      dwError = VMREST_HTTP_VALIDATION_FAILED;
                 }
                 break;
        case HTTP_GENERAL_HEADER_CONNECTION:
                 headerValLen = strlen(pRequest->generalHeader->connection);
                 if (headerValLen > 0 && headerValLen < MAX_HTTP_HEADER_VAL_LEN)
                 {
                     strcpy(response, pRequest->generalHeader->connection);
                 }
                 else
                 {
                      dwError = VMREST_HTTP_VALIDATION_FAILED;
                 }
                 break;
         case HTTP_GENERAL_HEADER_TRAILER:
                 headerValLen = strlen(pRequest->generalHeader->trailer);
                 if (headerValLen > 0 && headerValLen < MAX_HTTP_HEADER_VAL_LEN)
                 {
                     strcpy(response, pRequest->generalHeader->trailer);
                 }
                 else
                 {
                      dwError = VMREST_HTTP_VALIDATION_FAILED;
                 }
                 break;
        case HTTP_GENERAL_HEADER_TRANSFER_ENCODING:
                 headerValLen = strlen(pRequest->generalHeader->transferEncoding);
                 if (headerValLen > 0 && headerValLen < MAX_HTTP_HEADER_VAL_LEN)
                 {
                     strcpy(response, pRequest->generalHeader->transferEncoding);
                 }
                 else
                 {
                      dwError = VMREST_HTTP_VALIDATION_FAILED;
                 }
                 break;
        case HTTP_ENTITY_HEADER_ALLOW:
                 headerValLen = strlen(pRequest->entityHeader->allow);
                 if (headerValLen > 0 && headerValLen < MAX_HTTP_HEADER_VAL_LEN)
                 {
                     strcpy(response, pRequest->entityHeader->allow);
                 }
                 else
                 {
                      dwError = VMREST_HTTP_VALIDATION_FAILED;
                 }
                 break;
       case HTTP_ENTITY_HEADER_CONTENT_ENCODING:
                 headerValLen = strlen(pRequest->entityHeader->contentEncoding);
                 if (headerValLen > 0 && headerValLen < MAX_HTTP_HEADER_VAL_LEN)
                 {
                     strcpy(response, pRequest->entityHeader->contentEncoding);
                 }
                 else
                 {
                      dwError = VMREST_HTTP_VALIDATION_FAILED;
                 }
                 break;
        case HTTP_ENTITY_HEADER_CONTENT_LANGUAGE:
                 headerValLen = strlen(pRequest->entityHeader->contentLanguage);
                 if (headerValLen > 0 && headerValLen < MAX_HTTP_HEADER_VAL_LEN)
                 {
                     strcpy(response, pRequest->entityHeader->contentLanguage);
                 }
                 else
                 {
                      dwError = VMREST_HTTP_VALIDATION_FAILED;
                 }
                 break;
        case HTTP_ENTITY_HEADER_CONTENT_LENGTH:
                 headerValLen = strlen(pRequest->entityHeader->contentLength);
                 if (headerValLen > 0 && headerValLen < MAX_HTTP_HEADER_VAL_LEN)
                 {
                     strcpy(response, pRequest->entityHeader->contentLength);
                 }
                 else
                 {
                      dwError = VMREST_HTTP_VALIDATION_FAILED;
                 }
                 break;
        case HTTP_ENTITY_HEADER_CONTENT_LOCATION:
                 headerValLen = strlen(pRequest->entityHeader->contentLocation);
                 if (headerValLen > 0 && headerValLen < MAX_HTTP_HEADER_VAL_LEN)
                 {
                     strcpy(response, pRequest->entityHeader->contentLocation);
                 }
                 else
                 {
                      dwError = VMREST_HTTP_VALIDATION_FAILED;
                 }
                 break;
        case HTTP_ENTITY_HEADER_CONTENT_MD5:
                 headerValLen = strlen(pRequest->entityHeader->contentMD5);
                 if (headerValLen > 0 && headerValLen < MAX_HTTP_HEADER_VAL_LEN)
                 {
                     strcpy(response, pRequest->entityHeader->contentMD5);
                 }
                 else
                 {
                      dwError = VMREST_HTTP_VALIDATION_FAILED;
                 }
                 break;
        case HTTP_ENTITY_HEADER_CONTENT_RANGE:
                 headerValLen = strlen(pRequest->entityHeader->contentRange);
                 if (headerValLen > 0 && headerValLen < MAX_HTTP_HEADER_VAL_LEN)
                 {
                     strcpy(response, pRequest->entityHeader->contentRange);
                 }
                 else
                 {
                      dwError = VMREST_HTTP_VALIDATION_FAILED;
                 }
                 break;
        case HTTP_ENTITY_HEADER_CONTENT_TYPE:
                 headerValLen = strlen(pRequest->entityHeader->contentType);
                 if (headerValLen > 0 && headerValLen < MAX_HTTP_HEADER_VAL_LEN)
                 {
                     strcpy(response, pRequest->entityHeader->contentType);
                 }
                 else
                 {
                      dwError = VMREST_HTTP_VALIDATION_FAILED;
                 }
                 break;
        default:
                 dwError = VMREST_HTTP_VALIDATION_FAILED;
                 break;

    }
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:
    return dwError;
error:
    response = NULL;
    goto cleanup;
}

uint32_t
VmRESTGetHttpPayload(
    PREST_REQUEST                    pRequest,
    PSTR                             response
    )
{

    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    uint32_t                         contentLen = 0;

    if (pRequest == NULL || pRequest->entityHeader == NULL || response == NULL)
    {
        VMREST_LOG_DEBUG("VmRESTGetHttpPayload(): Invalid params");
        dwError = VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    /* Valid Content Length must be present in header for this API to success */
    if (strlen(pRequest->entityHeader->contentLength) > 0)
    {
        contentLen = atoi(pRequest->entityHeader->contentLength);
        if ((contentLen > 0) && (contentLen <= MAX_HTTP_PAYLOAD_LEN))
        {
            memcpy(response, pRequest->messageBody->buffer, contentLen);
        }
        else
        {
            VMREST_LOG_DEBUG("VmRESTGetHttpPayload(): Not a valid content length");
            dwError = VMREST_HTTP_VALIDATION_FAILED;
        }
    }
    else
    {
        VMREST_LOG_DEBUG("VmRESTGetHttpPayload(): Content length not specified");
        dwError = VMREST_HTTP_VALIDATION_FAILED;
    }
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:
    return dwError;
error:
    response = NULL;
    goto cleanup;
}

uint32_t
VmRESTSetHttpPayload(
    PREST_RESPONSE*                  ppResponse,
    PSTR                             buffer
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    uint32_t                         contentLen = 0;
    PREST_RESPONSE                   pResponse = NULL;

    if (ppResponse == NULL || *ppResponse == NULL || buffer == NULL)
    {
        VMREST_LOG_DEBUG("VmRESTSetHttpPayload(): Invalid params");
        dwError = VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    pResponse = *ppResponse;

    /* Valid Content Length must be present in header for this API to success */
    if (strlen(pResponse->entityHeader->contentLength) > 0)
    {
        contentLen = atoi(pResponse->entityHeader->contentLength);
        if ((contentLen > 0) && (contentLen <= MAX_HTTP_PAYLOAD_LEN))
        {
            memcpy(pResponse->messageBody->buffer, buffer, contentLen);
        }
        else
        {
            VMREST_LOG_DEBUG("VmRESTSetHttpPayload(): Invalid content length");
            dwError = VMREST_HTTP_VALIDATION_FAILED;
        }
    }
    else
    {
        VMREST_LOG_DEBUG("VmRESTSetHttpPayload(): Content length not specified");
        dwError = VMREST_HTTP_VALIDATION_FAILED;
    }
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:
    return dwError;
error:
    goto cleanup;
}


uint32_t
VmRESTSetHttpHeader(
    PREST_RESPONSE*                  ppResponse,
    PCSTR                            header,
    PSTR                             value
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    uint32_t                         headerNo = 0;
    uint32_t                         valLen   = 0;
    uint32_t                         resStatus = 0;
    PREST_RESPONSE                   pResponse = NULL;

    if (ppResponse == NULL || *ppResponse == NULL || header == NULL || value == NULL)
    {
        VMREST_LOG_DEBUG("VmRESTSetHttpHeader(): Invalid params");
        dwError = VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    pResponse = *ppResponse;

    if ((pResponse->responseHeader == NULL) || (pResponse->entityHeader == NULL) || (pResponse->generalHeader == NULL))
    {
        VMREST_LOG_DEBUG("VmRESTSetHttpHeader(): Null sub structures in response object");
        dwError = VMREST_HTTP_VALIDATION_FAILED;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTMapHeaderToEnum(
                  header,
                  &headerNo,
                  &resStatus
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    if ((headerNo < HTTP_REQUEST_HEADER_ACCEPT) || (headerNo > HTTP_ENTITY_HEADER_CONTENT_TYPE))
    {
        VMREST_LOG_DEBUG("VmRESTSetHttpHeader(): Not a valid HTTP header");
        dwError = VMREST_HTTP_VALIDATION_FAILED;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    /* No request header data will be set */
    if ((headerNo >= HTTP_REQUEST_HEADER_ACCEPT) && (headerNo <= HTTP_REQUEST_HEADER_REFERER))
    {
        VMREST_LOG_DEBUG("VmRESTSetHttpHeader(): Not a valid http response header");
        dwError = VMREST_HTTP_VALIDATION_FAILED;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    valLen = strlen(value);
    if (valLen >= MAX_HTTP_HEADER_VAL_LEN)
    {
        VMREST_LOG_DEBUG("VmRESTSetHttpHeader(): Bad value to be set");
        dwError = VMREST_HTTP_VALIDATION_FAILED;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    switch (headerNo)
    {
        case HTTP_RESPONSE_HEADER_ACCEPT_RANGE:
                     strcpy(pResponse->responseHeader->acceptRange, value);
                     break;

        case HTTP_RESPONSE_HEADER_LOCATION:
                     strcpy(pResponse->responseHeader->location, value);
                     break;

        case HTTP_RESPONSE_HEADER_PROXY_AUTH:
                     strcpy(pResponse->responseHeader->proxyAuth, value);
                     break;

        case HTTP_RESPONSE_HEADER_SERVER:
                     strcpy(pResponse->responseHeader->server, value);
                     break;

        case HTTP_GENERAL_HEADER_CACHE_CONTROL:
                     strcpy(pResponse->generalHeader->cacheControl, value);
                     break;

        case HTTP_GENERAL_HEADER_CONNECTION:
                     strcpy(pResponse->generalHeader->connection, value);
                     break;

        case HTTP_GENERAL_HEADER_TRAILER:
                     strcpy(pResponse->generalHeader->trailer, value);
                     break;

        case HTTP_GENERAL_HEADER_TRANSFER_ENCODING:
                     strcpy(pResponse->generalHeader->transferEncoding, value);
                     break;

        case HTTP_ENTITY_HEADER_ALLOW:
                     strcpy(pResponse->entityHeader->allow, value);
                     break;

        case HTTP_ENTITY_HEADER_CONTENT_ENCODING:
                     strcpy(pResponse->entityHeader->contentEncoding, value);
                     break;

        case HTTP_ENTITY_HEADER_CONTENT_LANGUAGE:
                     strcpy(pResponse->entityHeader->contentLanguage, value);
                     break;

        case HTTP_ENTITY_HEADER_CONTENT_LENGTH:
                     strcpy(pResponse->entityHeader->contentLength, value);
                     break;

        case HTTP_ENTITY_HEADER_CONTENT_LOCATION:
                     strcpy(pResponse->entityHeader->contentLocation, value);
                     break;

        case HTTP_ENTITY_HEADER_CONTENT_MD5:
                     strcpy(pResponse->entityHeader->contentMD5, value);
                     break;

        case HTTP_ENTITY_HEADER_CONTENT_RANGE:
                     strcpy(pResponse->entityHeader->contentRange, value);
                     break;

        case HTTP_ENTITY_HEADER_CONTENT_TYPE:
                     strcpy(pResponse->entityHeader->contentType, value);
                     break;

        default:
                     dwError = VMREST_HTTP_VALIDATION_FAILED;
                     BAIL_ON_VMREST_ERROR(dwError);
                     break;
    }
cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VmRESTSetHttpStatusCode(
    PREST_RESPONSE*                  ppResponse,
    PSTR                             statusCode
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    uint32_t                         statusLen = 0;
    PREST_RESPONSE    pResponse = NULL;

    if (ppResponse == NULL || *ppResponse == NULL || statusCode == NULL)
    {
        /* Response object not allocated any memory */
        VMREST_LOG_DEBUG("VmRESTSetHttpStatusCode(): Invalid params");
        dwError = VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    pResponse = *ppResponse;
    statusLen = strlen(statusCode);

    if (statusLen >= MAX_STATUS_LEN)
    {
        VMREST_LOG_DEBUG("VmRESTSetHttpStatusCode(): Status length too large");
        dwError = VMREST_HTTP_VALIDATION_FAILED;
    }
    BAIL_ON_VMREST_ERROR(dwError);
    /* TODO :: Check validity of Status Code */

    strcpy(pResponse->statusLine->statusCode, statusCode);

cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VmRESTSetHttpStatusVersion(
    PREST_RESPONSE*                  ppResponse,
    PSTR                             version
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    uint32_t                         versionLen = 0;
    PREST_RESPONSE                   pResponse = NULL;


    if (ppResponse == NULL || *ppResponse == NULL || version == NULL)
    {
        VMREST_LOG_DEBUG("VmRESTSetHttpStatusVersion(): Invalid params");
        dwError = VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    pResponse = *ppResponse;

    versionLen = strlen(version);

    if (versionLen > MAX_VERSION_LEN)
    {
        VMREST_LOG_DEBUG("VmRESTSetHttpStatusVersion(): Bad version length");
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
    PSTR                             reasonPhrase
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    PREST_RESPONSE                   pResponse = NULL;


    if (ppResponse == NULL || *ppResponse == NULL || reasonPhrase == NULL)
    {
        VMREST_LOG_DEBUG("VmRESTSetHttpReasonPhrase(): Invalid params");
        dwError = VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    pResponse = *ppResponse;

    strcpy(pResponse->statusLine->reason_phrase, reasonPhrase);

cleanup:
    return dwError;
error:
    goto cleanup;
}

