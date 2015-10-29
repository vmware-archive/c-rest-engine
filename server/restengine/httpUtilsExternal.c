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
    PVM_REST_HTTP_REQUEST_PACKET    pRequest,
    char*                           response
    )
{
    uint32_t dwError   = 0;
    uint32_t methodLen = 0; 
    uint32_t methodNo  = 0;
    if ((pRequest == NULL) || (pRequest->requestLine == NULL))
    {
        /* Bad request:: No memory */
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_VMREST_ERROR(dwError);
    }
    methodLen = strlen(pRequest->requestLine->method);
    if (methodLen == 0 || methodLen > MAX_METHOD_LEN)
    {   
        /* Bad method name found in request object */
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_VMREST_ERROR(dwError);
    }
    if (response == NULL)
    {
        /* No memory for response object */
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_VMREST_ERROR(dwError);

    }
    dwError = VmRESTMapMethodToEnum(
                  pRequest->requestLine->method,
                  &methodNo
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    if ((methodNo >= HTTP_METHOD_GET) && (methodNo <= HTTP_METHOD_CONNECT))
    {
        strcpy(response, pRequest->requestLine->method);
    } else 
    {
        /* This request does not have any valid HTTP method */
        response = NULL;
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_VMREST_ERROR(dwError);
    }
   

cleanup:
 
    return dwError;

error:

    goto cleanup;

}


uint32_t
VmRESTGetHttpURI(
    PVM_REST_HTTP_REQUEST_PACKET    pRequest,
    char*                           response
    )
{
    uint32_t dwError   = 0;
    uint32_t uriLen = 0;
    if ((pRequest == NULL) || (pRequest->requestLine == NULL))
    {
        /* Bad request:: No memory */
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_VMREST_ERROR(dwError);
    }
    uriLen = strlen(pRequest->requestLine->uri);
    if (uriLen == 0 || uriLen > MAX_URI_LEN)
    {
        /* Bad uri found in request object */
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_VMREST_ERROR(dwError);
    }
    if (response == NULL)
    {
        /* No memory for response object */
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_VMREST_ERROR(dwError);

    }
    strcpy(response, pRequest->requestLine->uri);    

cleanup:

    return dwError;

error:

    goto cleanup;
}

uint32_t 
VmRESTGetHttpVersion(
    PVM_REST_HTTP_REQUEST_PACKET    pRequest,
    char*                           response
    )
{
    uint32_t dwError   = 0;
    uint32_t versionLen = 0;
    if ((pRequest == NULL) || (pRequest->requestLine == NULL))
    {
        /* Bad request:: No memory */
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_VMREST_ERROR(dwError);
    }
    versionLen = strlen(pRequest->requestLine->version);
    if (versionLen == 0 || versionLen > MAX_VERSION_LEN)
    {
        /* Bad version found in request object */
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_VMREST_ERROR(dwError);
    }
    if (response == NULL)
    {
        /* No memory for response object */
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_VMREST_ERROR(dwError);

    }
    strcpy(response, pRequest->requestLine->version);

cleanup:

    return dwError;

error:

    goto cleanup;
}

uint32_t 
VmRESTGetHttpHeader(
    PVM_REST_HTTP_REQUEST_PACKET    pRequest,
    char*                           header,
    char*                           response
    )
{
    uint32_t   dwError = 0;
    uint32_t   headerNo = 0;
    uint32_t   headerValLen = 0;
    if (pRequest == NULL)
    {
        /* Bad request:: No memory */
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_VMREST_ERROR(dwError);
    }
    if (header == NULL)
    {
        /* No header found to query */
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_VMREST_ERROR(dwError);
    }
    if (response == NULL)
    {
        /* No memory for response object */
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_VMREST_ERROR(dwError);
    }
    dwError = VmRESTMapHeaderToEnum(
                  header,
                  &headerNo
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    if ((headerNo < HTTP_REQUEST_HEADER_ACCEPT) || (headerNo > HTTP_ENTITY_HEADER_CONTENT_TYPE))
    {
        /* Not a valid HTTP header */
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_VMREST_ERROR(dwError);
    }
    
    /* No response header data is returned */
    if ((headerNo >= HTTP_RESPONSE_HEADER_ACCEPT_RANGE) && (headerNo <= HTTP_RESPONSE_HEADER_SERVER))    
    {
        /* Not a valid HTTP request packet header */
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_VMREST_ERROR(dwError);
    }

    switch (headerNo)
    {
        case HTTP_REQUEST_HEADER_ACCEPT:
                 headerValLen = strlen(pRequest->requestHeader->accept);
                 if (headerValLen > 0)
                 {
                     strcpy(response, pRequest->requestHeader->accept);
                 }
                 else
                 {
                      /*Respose object has invalid value */
                      dwError = ERROR_NOT_SUPPORTED;
                      BAIL_ON_VMREST_ERROR(dwError);
                 }
                 break;
        case HTTP_REQUEST_HEADER_ACCEPT_CHARSET:
                 headerValLen = strlen(pRequest->requestHeader->acceptCharSet);
                 if (headerValLen > 0)
                 {
                     strcpy(response, pRequest->requestHeader->acceptCharSet);
                 }
                 else
                 {
                      /*Respose object has invalid value */
                      dwError = ERROR_NOT_SUPPORTED;
                      BAIL_ON_VMREST_ERROR(dwError);
                 }
                 break;
        case HTTP_REQUEST_HEADER_ACCEPT_ENCODING:
                 headerValLen = strlen(pRequest->requestHeader->acceptEncoding);
                 if (headerValLen > 0)
                 {
                     strcpy(response, pRequest->requestHeader->acceptEncoding);
                 }
                 else
                 {
                      /*Respose object has invalid value */
                      dwError = ERROR_NOT_SUPPORTED;
                      BAIL_ON_VMREST_ERROR(dwError);
                 }
                 break;
        case HTTP_REQUEST_HEADER_ACCEPT_LANGUAGE:
                 headerValLen = strlen(pRequest->requestHeader->acceptLanguage);
                 if (headerValLen > 0)
                 {
                     strcpy(response, pRequest->requestHeader->acceptLanguage);
                 }
                 else
                 {
                      /*Respose object has invalid value */
                      dwError = ERROR_NOT_SUPPORTED;
                      BAIL_ON_VMREST_ERROR(dwError);
                 }
                 break;
        case HTTP_REQUEST_HEADER_ACCEPT_AUTHORIZATION:
                 headerValLen = strlen(pRequest->requestHeader->authorization);
                 if (headerValLen > 0)
                 {
                     strcpy(response, pRequest->requestHeader->authorization);
                 }
                 else
                 {
                      /*Respose object has invalid value */
                      dwError = ERROR_NOT_SUPPORTED;
                      BAIL_ON_VMREST_ERROR(dwError);
                 }
                 break;
       case HTTP_REQUEST_HEADER_FROM:
                 headerValLen = strlen(pRequest->requestHeader->from);
                 if (headerValLen > 0)
                 {
                     strcpy(response, pRequest->requestHeader->from);
                 }
                 else
                 {
                      /*Respose object has invalid value */
                      dwError = ERROR_NOT_SUPPORTED;
                      BAIL_ON_VMREST_ERROR(dwError);
                 }
                 break;
       case HTTP_REQUEST_HEADER_HOST:
                 headerValLen = strlen(pRequest->requestHeader->host);
                 if (headerValLen > 0)
                 {
                     strcpy(response, pRequest->requestHeader->host);
                 }
                 else
                 {
                      /*Respose object has invalid value */
                      dwError = ERROR_NOT_SUPPORTED;
                      BAIL_ON_VMREST_ERROR(dwError);
                 }
                 break;
       case HTTP_REQUEST_HEADER_REFERER:
                 headerValLen = strlen(pRequest->requestHeader->referer);
                 if (headerValLen > 0)
                 {
                     strcpy(response, pRequest->requestHeader->referer);
                 }
                 else
                 {
                      /*Respose object has invalid value */
                      dwError = ERROR_NOT_SUPPORTED;
                      BAIL_ON_VMREST_ERROR(dwError);
                 }
                 break;
       case HTTP_GENERAL_HEADER_CACHE_CONTROL:
                 headerValLen = strlen(pRequest->generalHeader->cacheControl);
                 if (headerValLen > 0)
                 {
                     strcpy(response, pRequest->generalHeader->cacheControl);
                 }
                 else
                 {
                      /*Respose object has invalid value */
                      dwError = ERROR_NOT_SUPPORTED;
                      BAIL_ON_VMREST_ERROR(dwError);
                 }
                 break;
        case HTTP_GENERAL_HEADER_CONNECTION:
                 headerValLen = strlen(pRequest->generalHeader->connection);
                 if (headerValLen > 0)
                 {
                     strcpy(response, pRequest->generalHeader->connection);
                 }
                 else
                 {
                      /*Respose object has invalid value */
                      dwError = ERROR_NOT_SUPPORTED;
                      BAIL_ON_VMREST_ERROR(dwError);
                 }
                 break;
         case HTTP_GENERAL_HEADER_TRAILER:
                 headerValLen = strlen(pRequest->generalHeader->trailer);
                 if (headerValLen > 0)
                 {
                     strcpy(response, pRequest->generalHeader->trailer);
                 }
                 else
                 {
                      /*Respose object has invalid value */
                      dwError = ERROR_NOT_SUPPORTED;
                      BAIL_ON_VMREST_ERROR(dwError);
                 }
                 break;
        case HTTP_GENERAL_HEADER_TRANSFER_ENCODING:
                 headerValLen = strlen(pRequest->generalHeader->transferEncoding);
                 if (headerValLen > 0)
                 {
                     strcpy(response, pRequest->generalHeader->transferEncoding);
                 }
                 else
                 {
                      /*Respose object has invalid value */
                      dwError = ERROR_NOT_SUPPORTED;
                      BAIL_ON_VMREST_ERROR(dwError);
                 }
                 break;
        case HTTP_ENTITY_HEADER_ALLOW:
                 headerValLen = strlen(pRequest->entityHeader->allow);
                 if (headerValLen > 0)
                 {
                     strcpy(response, pRequest->entityHeader->allow);
                 }
                 else
                 {
                      /*Respose object has invalid value */
                      dwError = ERROR_NOT_SUPPORTED;
                      BAIL_ON_VMREST_ERROR(dwError);
                 }
                 break;
       case HTTP_ENTITY_HEADER_CONTENT_ENCODING:
                 headerValLen = strlen(pRequest->entityHeader->contentEncoding);
                 if (headerValLen > 0)
                 {
                     strcpy(response, pRequest->entityHeader->contentEncoding);
                 }
                 else
                 {
                      /*Respose object has invalid value */
                      dwError = ERROR_NOT_SUPPORTED;
                      BAIL_ON_VMREST_ERROR(dwError);
                 }
                 break;
        case HTTP_ENTITY_HEADER_CONTENT_LANGUAGE:
                 headerValLen = strlen(pRequest->entityHeader->contentLanguage);
                 if (headerValLen > 0)
                 {
                     strcpy(response, pRequest->entityHeader->contentLanguage);
                 }
                 else
                 {
                      /*Respose object has invalid value */
                      dwError = ERROR_NOT_SUPPORTED;
                      BAIL_ON_VMREST_ERROR(dwError);
                 }
                 break;
        case HTTP_ENTITY_HEADER_CONTENT_LENGTH:
                 headerValLen = strlen(pRequest->entityHeader->contentLength);
                 if (headerValLen > 0)
                 {
                     strcpy(response, pRequest->entityHeader->contentLength);
                 }
                 else
                 {
                      /*Respose object has invalid value */
                      dwError = ERROR_NOT_SUPPORTED;
                      BAIL_ON_VMREST_ERROR(dwError);
                 }
                 break;
        case HTTP_ENTITY_HEADER_CONTENT_LOCATION:
                 headerValLen = strlen(pRequest->entityHeader->contentLocation);
                 if (headerValLen > 0)
                 {
                     strcpy(response, pRequest->entityHeader->contentLocation);
                 }
                 else
                 {
                      /*Respose object has invalid value */
                      dwError = ERROR_NOT_SUPPORTED;
                      BAIL_ON_VMREST_ERROR(dwError);
                 }
                 break;
        case HTTP_ENTITY_HEADER_CONTENT_MD5:
                 headerValLen = strlen(pRequest->entityHeader->contentMD5);
                 if (headerValLen > 0)
                 {
                     strcpy(response, pRequest->entityHeader->contentMD5);
                 }
                 else
                 {
                      /*Respose object has invalid value */
                      dwError = ERROR_NOT_SUPPORTED;
                      BAIL_ON_VMREST_ERROR(dwError);
                 }
                 break;
        case HTTP_ENTITY_HEADER_CONTENT_RANGE:
                 headerValLen = strlen(pRequest->entityHeader->contentRange);
                 if (headerValLen > 0)
                 {
                     strcpy(response, pRequest->entityHeader->contentRange);
                 }
                 else
                 {
                      /*Respose object has invalid value */
                      dwError = ERROR_NOT_SUPPORTED;
                      BAIL_ON_VMREST_ERROR(dwError);
                 }
                 break;
        case HTTP_ENTITY_HEADER_CONTENT_TYPE:
                 headerValLen = strlen(pRequest->entityHeader->contentType);
                 if (headerValLen > 0)
                 {
                     strcpy(response, pRequest->entityHeader->contentType);
                 }
                 else
                 {
                      /*Respose object has invalid value */
                      dwError = ERROR_NOT_SUPPORTED;
                      BAIL_ON_VMREST_ERROR(dwError);
                 }
                 break;
        default:
                 dwError = ERROR_NOT_SUPPORTED;
                 BAIL_ON_VMREST_ERROR(dwError);
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
    PVM_REST_HTTP_REQUEST_PACKET    pRequest,
    char*                           response
    )
{

    uint32_t     dwError = 0;
    uint32_t     contentLen = 0;

    if (pRequest == NULL || pRequest->entityHeader == NULL)
    {
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_VMREST_ERROR(dwError);
    }
   
    if (response == NULL)
    {
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_VMREST_ERROR(dwError);
    }

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
            dwError = ERROR_NOT_SUPPORTED;
            BAIL_ON_VMREST_ERROR(dwError);
        }
    }
    else 
    {
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_VMREST_ERROR(dwError);
    }
    
cleanup:

    return dwError;

error:

    response = NULL;
    goto cleanup;
}

uint32_t
VmRESTSetHttpPayload(
    PVM_REST_HTTP_RESPONSE_PACKET*  ppResponse,
    char*                           buffer
    )
{
    uint32_t                           dwError = 0;
    uint32_t                           contentLen = 0;
    PVM_REST_HTTP_RESPONSE_PACKET      pResponse = NULL;

    if (ppResponse == NULL || *ppResponse == NULL)
    {
        /* Response object not allocated any memory */
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_VMREST_ERROR(dwError);
    }
    if (buffer == NULL)
    {
        /* No parameters to set */
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_VMREST_ERROR(dwError);
    }

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
            dwError = ERROR_NOT_SUPPORTED;
            BAIL_ON_VMREST_ERROR(dwError);
        }
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
VmRESTSetHttpHeader(
    PVM_REST_HTTP_RESPONSE_PACKET*  ppResponse,
    char*                           header,
    char*                           value
    )
{
    uint32_t                           dwError  = 0;
    uint32_t                           headerNo = 0;
    uint32_t                           valLen   = 0;
    PVM_REST_HTTP_RESPONSE_PACKET      pResponse = NULL;

    if (ppResponse == NULL || *ppResponse == NULL)
    {
        /* Response object not allocated any memory */
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_VMREST_ERROR(dwError);
    }
    if (header == NULL || value == NULL)
    {
        /* No parameters to set */
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_VMREST_ERROR(dwError); 
    }
 
    pResponse = *ppResponse;
   
    dwError = VmRESTMapHeaderToEnum(
                  header,
                  &headerNo
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    if ((headerNo < HTTP_REQUEST_HEADER_ACCEPT) || (headerNo > HTTP_ENTITY_HEADER_CONTENT_TYPE))
    {
        /* Not a valid HTTP header */
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_VMREST_ERROR(dwError);
    }

    /* No request header data will be set */
    if ((headerNo >= HTTP_REQUEST_HEADER_ACCEPT) && (headerNo <= HTTP_REQUEST_HEADER_REFERER))
    {
        /* Not a valid HTTP response packet header */
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_VMREST_ERROR(dwError);
    }
    
    valLen = strlen(value);
    if (valLen >= MAX_HTTP_HEADER_VAL_LEN)
    {
        /* bad string value to be set */
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_VMREST_ERROR(dwError);
    }
      
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
                     dwError = ERROR_NOT_SUPPORTED;
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
    PVM_REST_HTTP_RESPONSE_PACKET*  ppResponse,
    char*                           statusCode
    )
{
    uint32_t                        dwError = 0;
    uint32_t                        statusLen = 0;
    PVM_REST_HTTP_RESPONSE_PACKET   pResponse = NULL;
  

    if (ppResponse == NULL || *ppResponse == NULL)
    {
        /* Response object not allocated any memory */
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_VMREST_ERROR(dwError);
    }
       
    pResponse = *ppResponse;
    statusLen = strlen(statusCode);
  
    if (statusLen >= MAX_STATUS_LEN)
    {
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_VMREST_ERROR(dwError);
    }
 
    /* TODO :: Check validity of Status Code */
   
    strcpy(pResponse->statusLine->statusCode, statusCode);
    
cleanup:

    return dwError;

error:

    goto cleanup;

}

uint32_t
VmRESTSetHttpStatusVersion(
    PVM_REST_HTTP_RESPONSE_PACKET*  ppResponse,
    char*                           version
    )
{
    uint32_t                        dwError = 0;
    uint32_t                        versionLen = 0;
    PVM_REST_HTTP_RESPONSE_PACKET   pResponse = NULL;


    if (ppResponse == NULL || *ppResponse == NULL)
    {
        /* Response object not allocated any memory */
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_VMREST_ERROR(dwError);
    }

    pResponse = *ppResponse;

    versionLen = strlen(version);

    if (versionLen > MAX_VERSION_LEN)
    {
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_VMREST_ERROR(dwError);
    }
    

    strcpy(pResponse->statusLine->version, version);
    
cleanup:

    return dwError;

error:

    goto cleanup;

}

uint32_t
VmRESTSetHttpReasonPhrase(
    PVM_REST_HTTP_RESPONSE_PACKET*  ppResponse,
    char*                           reasonPhrase
    )
{
    uint32_t                        dwError = 0;
    PVM_REST_HTTP_RESPONSE_PACKET   pResponse = NULL;


    if (ppResponse == NULL || *ppResponse == NULL)
    {
        /* Response object not allocated any memory */
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_VMREST_ERROR(dwError);
    }
 
    if (reasonPhrase == NULL)
    {
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_VMREST_ERROR(dwError);
    }
   
    pResponse = *ppResponse;
    
    strcpy(pResponse->statusLine->reason_phrase, reasonPhrase);

cleanup:

    return dwError;

error:

    goto cleanup;

}

