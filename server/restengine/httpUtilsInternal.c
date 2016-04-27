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
VmRESTMapHeaderToEnum(
    PCSTR                            header,
    uint32_t*                        result,
    uint32_t*                        resStatus
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;

    if (header == NULL || result == NULL)
    {
       VMREST_LOG_DEBUG("VmRESTMapHeaderToEnum: Invalid params");
       dwError =  VMREST_HTTP_INVALID_PARAMS;
       *resStatus = BAD_REQUEST;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    if ((strcmp(header, "Accept")) == 0)
    {
        *result = HTTP_REQUEST_HEADER_ACCEPT;
    }
    else if ((strcmp(header, "Accept-Charset")) == 0)
    {
        *result = HTTP_REQUEST_HEADER_ACCEPT_CHARSET;
    }
    else if ((strcmp(header, "Accept-Encoding")) == 0)
    {
        *result = HTTP_REQUEST_HEADER_ACCEPT_ENCODING;
    }
    else if ((strcmp(header, "Accept-Language")) == 0)
    {
        *result = HTTP_REQUEST_HEADER_ACCEPT_LANGUAGE;
    }
    else if ((strcmp(header, "Authorization")) == 0)
    {
        *result = HTTP_REQUEST_HEADER_ACCEPT_AUTHORIZATION;
    }
    else if ((strcmp(header, "From")) == 0)
    {
        *result = HTTP_REQUEST_HEADER_FROM;
    }
    else if ((strcmp(header, "Host")) == 0)
    {
        *result = HTTP_REQUEST_HEADER_HOST;
    }
    else if ((strcmp(header, "Referer")) == 0)
    {
        *result = HTTP_REQUEST_HEADER_REFERER;
    }
    else if ((strcmp(header, "Accept-Ranges")) == 0)
    {
        *result = HTTP_RESPONSE_HEADER_ACCEPT_RANGE;
    }
    else if ((strcmp(header, "Location")) == 0)
    {
        *result = HTTP_RESPONSE_HEADER_LOCATION;
    }
    else if ((strcmp(header, "Proxy-Authenticate")) == 0)
    {
        *result = HTTP_RESPONSE_HEADER_PROXY_AUTH;
    }
    else if ((strcmp(header, "Server")) == 0)
    {
        *result = HTTP_RESPONSE_HEADER_SERVER;
    }
    else if ((strcmp(header, "Cache-Control")) == 0)
    {
        *result = HTTP_GENERAL_HEADER_CACHE_CONTROL;
    }
    else if ((strcmp(header, "Connection")) == 0)
    {
        *result = HTTP_GENERAL_HEADER_CONNECTION;
    }
    else if ((strcmp(header, "Trailer")) == 0)
    {
        *result = HTTP_GENERAL_HEADER_TRAILER;
    }
    else if ((strcmp(header, "Transfer-Encoding")) == 0)
    {
        *result = HTTP_GENERAL_HEADER_TRANSFER_ENCODING;
    }
    else if ((strcmp(header, "Allow")) == 0)
    {
        *result = HTTP_ENTITY_HEADER_ALLOW;
    }
    else if ((strcmp(header, "Content-Encoding")) == 0)
    {
        *result = HTTP_ENTITY_HEADER_CONTENT_ENCODING;
    }
    else if ((strcmp(header, "Content-Language")) == 0)
    {
        *result = HTTP_ENTITY_HEADER_CONTENT_LANGUAGE;
    }
    else if ((strcmp(header, "Content-Location")) == 0)
    {
        *result = HTTP_ENTITY_HEADER_CONTENT_LOCATION;
    }
    else if ((strcmp(header, "Content-Length")) == 0)
    {
        *result = HTTP_ENTITY_HEADER_CONTENT_LENGTH;
    }
    else if ((strcmp(header, "Content-MD5")) == 0)
    {
        *result = HTTP_ENTITY_HEADER_CONTENT_MD5;
    }
    else if ((strcmp(header, "Content-Range")) == 0)
    {
        *result = HTTP_ENTITY_HEADER_CONTENT_RANGE;
    }
    else if ((strcmp(header, "Content-Type")) == 0)
    {
        *result = HTTP_ENTITY_HEADER_CONTENT_TYPE;
    }
    else
    {
        *result = HTTP_MISC_HEADER_ALL;
    }
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VmRESTMapMethodToEnum(
    char*                            method,
    uint32_t*                        result
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;

    if (method == NULL || result == NULL)
    {
        /* Bad request:: No memory */
        VMREST_LOG_DEBUG("VmRESTMapMethodToEnum(): Invalid params");
        dwError = VMREST_APPLICATION_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    if ((strcmp(method,"GET")) == 0 )
    {
        *result = HTTP_METHOD_GET;
    }
    else if ((strcmp(method,"HEAD")) == 0 )
    {
        *result = HTTP_METHOD_HEAD;
    }
    else if ((strcmp(method,"POST")) == 0 )
    {
        *result = HTTP_METHOD_POST;
    }
    else if ((strcmp(method,"PUT")) == 0 )
    {
        *result = HTTP_METHOD_PUT;
    }
     else if ((strcmp(method,"DELETE")) == 0 )
    {
        *result = HTTP_METHOD_DELETE;
    }
    else if ((strcmp(method,"TRACE")) == 0 )
    {
        *result = HTTP_METHOD_TRACE;
    }
    else if ((strcmp(method,"CONNECT")) == 0 )
    {
        *result = HTTP_METHOD_CONNECT;
    }
    else
    {
        /* No allowed method found in request object */
        dwError = VMREST_APPLICATION_VALIDATION_FAILED;
        VMREST_LOG_DEBUG("VmRESTMapMethodToEnum(): Not valid method");
        BAIL_ON_VMREST_ERROR(dwError);
    }

cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VmRESTCopyString(
    char*                            src,
    char*                            des
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    uint32_t                         headerValLen = 0;

    if (src == NULL || des == NULL)
    {
        VMREST_LOG_DEBUG("VmRESTCopyString(): Invalid params");
        dwError = VMREST_APPLICATION_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    headerValLen = strlen(src);
    if (headerValLen > 0 && headerValLen < MAX_HTTP_HEADER_VAL_LEN)
    {
        strcpy(des, src);
    }
    else
    {
        VMREST_LOG_DEBUG("VmRESTCopyString(): Bad source length");
        dwError = VMREST_APPLICATION_VALIDATION_FAILED;
    }
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VmRESTGetHttpResponseHeader(
    PVM_REST_HTTP_RESPONSE_PACKET    pResponse,
    char*                            header,
    char*                            response
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    uint32_t                         headerNo = 0;
    uint32_t                         resStatus = OK;
    char*                            source = NULL;
    PSTR                             pRes = NULL;

    if (pResponse == NULL || header == NULL || response == NULL)
    {
        VMREST_LOG_DEBUG("VmRESTGetHttpResponseHeader(): Invalid params");
        dwError = VMREST_APPLICATION_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTMapHeaderToEnum(
                  header,
                  &headerNo,
                  &resStatus
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    if ((headerNo < HTTP_REQUEST_HEADER_ACCEPT) || (headerNo > HTTP_MISC_HEADER_ALL))
    {
        VMREST_LOG_DEBUG("VmRESTGetHttpResponseHeader(): Not a valid HTTP header");
        dwError = VMREST_APPLICATION_VALIDATION_FAILED;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    /* No request header data is returned */
    if ((headerNo >= HTTP_REQUEST_HEADER_ACCEPT) && (headerNo <= HTTP_REQUEST_HEADER_REFERER))
    {
        VMREST_LOG_DEBUG("VmRESTGetHttpResponseHeader(): Not a valid Response header");
        dwError = VMREST_APPLICATION_VALIDATION_FAILED;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    memset(source,'\0', MAX_HTTP_HEADER_VAL_LEN);
    switch (headerNo)
    {
        case HTTP_RESPONSE_HEADER_ACCEPT_RANGE:
                 source = pResponse->responseHeader->acceptRange;
                 break;
        case HTTP_RESPONSE_HEADER_LOCATION:
                 source = pResponse->responseHeader->location;
                 break;
        case HTTP_RESPONSE_HEADER_PROXY_AUTH:
                 source = pResponse->responseHeader->proxyAuth;
                 break;
        case HTTP_RESPONSE_HEADER_SERVER:
                 source = pResponse->responseHeader->server;
                 break;
        case HTTP_GENERAL_HEADER_CACHE_CONTROL:
                 source = pResponse->generalHeader->cacheControl;
                 break;
        case HTTP_GENERAL_HEADER_CONNECTION:
                 source = pResponse->generalHeader->connection;
                 break;
         case HTTP_GENERAL_HEADER_TRAILER:
                 source = pResponse->generalHeader->trailer;
                 break;
        case HTTP_GENERAL_HEADER_TRANSFER_ENCODING:
                 source = pResponse->generalHeader->transferEncoding;
                 break;
        case HTTP_ENTITY_HEADER_ALLOW:
                 source = pResponse->entityHeader->allow;
                 break;
       case HTTP_ENTITY_HEADER_CONTENT_ENCODING:
                 source = pResponse->entityHeader->contentEncoding;
                 break;
        case HTTP_ENTITY_HEADER_CONTENT_LANGUAGE:
                 source = pResponse->entityHeader->contentLanguage;
                 break;
        case HTTP_ENTITY_HEADER_CONTENT_LENGTH:
                 source = pResponse->entityHeader->contentLength;
                 break;
        case HTTP_ENTITY_HEADER_CONTENT_LOCATION:
                 source = pResponse->entityHeader->contentLocation;
                 break;
        case HTTP_ENTITY_HEADER_CONTENT_MD5:
                 source = pResponse->entityHeader->contentMD5;
                 break;
        case HTTP_ENTITY_HEADER_CONTENT_RANGE:
                 source = pResponse->entityHeader->contentRange;
                 break;
        case HTTP_ENTITY_HEADER_CONTENT_TYPE:
                 source = pResponse->entityHeader->contentType;
                 break;
        case HTTP_MISC_HEADER_ALL:
                 dwError = VmRESTGetHTTPMiscHeader(
                               pResponse->miscHeader,
                               header,
                               &pRes
                               );
                 BAIL_ON_VMREST_ERROR(dwError);
                 source = pRes;
                 break;
        default:
                 VMREST_LOG_DEBUG("VmRESTGetHttpResponseHeader(): Header not recognised");
                 dwError = VMREST_APPLICATION_VALIDATION_FAILED;
                 BAIL_ON_VMREST_ERROR(dwError);
                 break;

    }
    dwError = VmRESTCopyString(
                  source,
                  response
                  );
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:
    return dwError;
error:
    response = NULL;
    goto cleanup;
}

uint32_t
VmRESTMapStatusCodeToEnumAndReasonPhrase(
    char*                            statusCode,
    uint32_t*                        result,
    char*                            reasonPhrase
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;

    if (statusCode == NULL || result == NULL || reasonPhrase == NULL)
    {
        VMREST_LOG_DEBUG("VmRESTMapStatusCodeToEnumAndReasonPhrase(): Invalid params");
        dwError = VMREST_APPLICATION_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    if ((strcmp(statusCode, "100")) == 0)
    {
        *result = CONTINUE;
        strcpy(reasonPhrase, "continue");
    }
    else if ((strcmp(statusCode, "101")) == 0)
    {
        *result = SWITCHING_PROTOCOL;
        strcpy(reasonPhrase,"Switching Protocols");
    }
    else if ((strcmp(statusCode, "200")) == 0)
    {
        *result = OK;
        strcpy(reasonPhrase,"OK");
    }
    else if ((strcmp(statusCode, "201")) == 0)
    {
        *result = CREATED;
        strcpy(reasonPhrase,"Created");
    }
    else if ((strcmp(statusCode, "202")) == 0)
    {
        *result = ACCEPTED;
        strcpy(reasonPhrase,"Accepted");
    }
    else if ((strcmp(statusCode, "203")) == 0)
    {
        *result = NON_AUTH_INFO;
        strcpy(reasonPhrase,"Non-Authoritative Information");
    }
    else if ((strcmp(statusCode, "204")) == 0)
    {
        *result = NO_CONTENT;
        strcpy(reasonPhrase,"No Content");
    }
    else if ((strcmp(statusCode, "205")) == 0)
    {
        *result = RESET_CONTENT;
        strcpy(reasonPhrase,"Reset Content");
    }
    else if ((strcmp(statusCode, "206")) == 0)
    {
        *result = PARTIAL_CONTENT;
        strcpy(reasonPhrase,"Partial Content");
    }
    else if ((strcmp(statusCode, "300")) == 0)
    {
        *result = MULTIPLE_CHOICES;
        strcpy(reasonPhrase,"Multiple Choices");
    }
    else if ((strcmp(statusCode, "301")) == 0)
    {
        *result = MOVED_PERMANENTLY;
        strcpy(reasonPhrase,"Moved Permanently");
    }
    else if ((strcmp(statusCode, "302")) == 0)
    {
        *result = FOUND;
        strcpy(reasonPhrase,"Found");
    }
    else if ((strcmp(statusCode, "303")) == 0)
    {
        *result = SEE_OTHER;
        strcpy(reasonPhrase,"See Other");
    }
    else if ((strcmp(statusCode, "304")) == 0)
    {
        *result = NOT_MODIFIED;
        strcpy(reasonPhrase,"Not Modified");
    }
    else if ((strcmp(statusCode, "305")) == 0)
    {
        *result = USE_PROXY;
        strcpy(reasonPhrase,"Use Proxy");
    }
    else if ((strcmp(statusCode, "306")) == 0)
    {
        *result = TEMPORARY_REDIRECT;
        strcpy(reasonPhrase,"Temporary Redirect");
    }
    else if ((strcmp(statusCode, "400")) == 0)
    {
        *result = BAD_REQUEST;
        strcpy(reasonPhrase,"Bad Request");
    }
    else if ((strcmp(statusCode, "401")) == 0)
    {
        *result = UNAUTHORIZED;
        strcpy(reasonPhrase,"Unauthorized");
    }
    else if ((strcmp(statusCode, "402")) == 0)
    {
        *result = PAYMENT_REQUIRED;
        strcpy(reasonPhrase,"Payment Required");
    }
    else if ((strcmp(statusCode, "403")) == 0)
    {
        *result = FORBIDDEN;
        strcpy(reasonPhrase,"Forbidden");
    }
    else if ((strcmp(statusCode, "404")) == 0)
    {
        *result = NOT_FOUND;
        strcpy(reasonPhrase,"Not Found");
    }
    else if ((strcmp(statusCode, "405")) == 0)
    {
        *result = METHOD_NOT_ALLOWED;
        strcpy(reasonPhrase,"Method Not Allowed");
    }
    else if ((strcmp(statusCode, "406")) == 0)
    {
        *result = NOT_ACCEPTABLE;
        strcpy(reasonPhrase,"Not Acceptable");
    }
    else if ((strcmp(statusCode, "407")) == 0)
    {
        *result = PROXY_AUTH_REQUIRED;
        strcpy(reasonPhrase,"Proxy Authentication Required");
    }
    else if ((strcmp(statusCode, "408")) == 0)
    {
        *result = REQUEST_TIMEOUT;
        strcpy(reasonPhrase,"Request Timeout");
    }
    else if ((strcmp(statusCode, "409")) == 0)
    {
        *result = CONFLICT;
        strcpy(reasonPhrase,"Conflict");
    }
    else if ((strcmp(statusCode, "410")) == 0)
    {
        *result = GONE;
        strcpy(reasonPhrase,"Gone");
    }
    else if ((strcmp(statusCode, "411")) == 0)
    {
        *result = LENGTH_REQUIRED;
        strcpy(reasonPhrase,"Length Required");
    }
    else if ((strcmp(statusCode, "412")) == 0)
    {
        *result = PRECONDITION_FAILED;
        strcpy(reasonPhrase,"Precondition Failed");
    }
    else if ((strcmp(statusCode, "413")) == 0)
    {
        *result = REQUEST_ENTITY_TOO_LARGE;
        strcpy(reasonPhrase,"Request Entity Too Large");
    }
    else if ((strcmp(statusCode, "414")) == 0)
    {
        *result = REQUEST_URI_TOO_LARGE;
        strcpy(reasonPhrase,"Request-URI Too Large ");
    }
    else if ((strcmp(statusCode, "415")) == 0)
    {
        *result = UNSUPPORTED_MEDIA_TYPE;
        strcpy(reasonPhrase,"Unsupported Media Type");
    }
    else if ((strcmp(statusCode, "416")) == 0)
    {
        *result = REQUEST_RANGE_NOT_SATISFIABLE;
        strcpy(reasonPhrase,"Request range not satisfiable");
    }
    else if ((strcmp(statusCode, "417")) == 0)
    {
        *result = EXPECTATION_FAILED;
        strcpy(reasonPhrase,"Expectation Failed");
    }
    else if ((strcmp(statusCode, "500")) == 0)
    {
        *result = INTERNAL_SERVER_ERROR;
        strcpy(reasonPhrase,"Internal Server Error");
    }
    else if ((strcmp(statusCode, "501")) == 0)
    {
        *result = NOT_IMPLEMENTED;
        strcpy(reasonPhrase,"Not Implemented");
    }
    else if ((strcmp(statusCode, "502")) == 0)
    {
        *result = BAD_GATEWAY;
        strcpy(reasonPhrase,"Bad Gateway");
    }
    else if ((strcmp(statusCode, "503")) == 0)
    {
        *result = SERVICE_UNAVAILABLE;
        strcpy(reasonPhrase,"Service Unavailable");
    }
    else if ((strcmp(statusCode, "504")) == 0)
    {
        *result = GATEWAY_TIMEOUT;
        strcpy(reasonPhrase,"Gateway Time-out");
    }
    else if ((strcmp(statusCode, "505")) == 0)
    {
        *result = HTTP_VERSION_NOT_SUPPORTED;
        strcpy(reasonPhrase,"HTTP Version not supported");
    }
    else
    {
        result = NULL;
        dwError = VMREST_APPLICATION_VALIDATION_FAILED;
        BAIL_ON_VMREST_ERROR(dwError);
    }

cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VmRESTSetHttpRequestHeader(
    PVM_REST_HTTP_REQUEST_PACKET     pRequest,
    char*                            header,
    char*                            value,
    uint32_t*                        resStatus
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    uint32_t                         headerNo = 0;

    if (pRequest == NULL || header == NULL || value == NULL || *resStatus != OK)
    {
        VMREST_LOG_DEBUG("VmRESTSetHttpRequestHeader(): Invalid Params");
        dwError =  VMREST_HTTP_INVALID_PARAMS;
        *resStatus = BAD_REQUEST;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTMapHeaderToEnum(
                  header,
                  &headerNo,
                  resStatus
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    if ((headerNo < HTTP_REQUEST_HEADER_ACCEPT) || (headerNo > HTTP_MISC_HEADER_ALL))
    {
        /* Not a valid HTTP header */
        VMREST_LOG_DEBUG("VmRESTSetHttpRequestHeader(): Header not in acceptable range");
        dwError =  VMREST_HTTP_VALIDATION_FAILED;
        *resStatus = BAD_REQUEST;
    }
    /* No request header data is returned */
    if ((headerNo >= HTTP_RESPONSE_HEADER_ACCEPT_RANGE) && (headerNo <= HTTP_RESPONSE_HEADER_SERVER))
    {
        /* Not a valid HTTP request packet header */
        VMREST_LOG_DEBUG("VmRESTSetHttpRequestHeader(): Header not in allowed HTTP request headers");
        dwError =  VMREST_HTTP_VALIDATION_FAILED;
        *resStatus = BAD_REQUEST;
    }
    switch (headerNo)
    {
        case HTTP_REQUEST_HEADER_ACCEPT:
                 strcpy(pRequest->requestHeader->accept, value);
                 break;
        case HTTP_REQUEST_HEADER_ACCEPT_CHARSET:
                 strcpy(pRequest->requestHeader->acceptCharSet, value);
                 break;
        case HTTP_REQUEST_HEADER_ACCEPT_ENCODING:
                 strcpy(pRequest->requestHeader->acceptEncoding, value);
                 break;
        case HTTP_REQUEST_HEADER_ACCEPT_LANGUAGE:
                 strcpy(pRequest->requestHeader->acceptLanguage,value);
                 break;
        case HTTP_REQUEST_HEADER_ACCEPT_AUTHORIZATION:
                 strcpy(pRequest->requestHeader->authorization, value);
                 break;
        case HTTP_REQUEST_HEADER_FROM:
                 strcpy(pRequest->requestHeader->from, value);
                 break;
        case HTTP_REQUEST_HEADER_HOST:
                 strcpy(pRequest->requestHeader->host, value);
                 break;
        case HTTP_REQUEST_HEADER_REFERER:
                 strcpy(pRequest->requestHeader->referer, value);
                 break;
        case HTTP_GENERAL_HEADER_CACHE_CONTROL:
                 strcpy(pRequest->generalHeader->cacheControl, value);
                 break;
        case HTTP_GENERAL_HEADER_CONNECTION:
                 strcpy(pRequest->generalHeader->connection, value);
                 break;
        case HTTP_GENERAL_HEADER_TRAILER:
                 strcpy(pRequest->generalHeader->trailer, value);
                 break;
        case HTTP_GENERAL_HEADER_TRANSFER_ENCODING:
                 strcpy(pRequest->generalHeader->transferEncoding, value);
                 break;
        case HTTP_ENTITY_HEADER_ALLOW:
                 strcpy(pRequest->entityHeader->allow, value);
                 break;
        case HTTP_ENTITY_HEADER_CONTENT_ENCODING:
                 strcpy(pRequest->entityHeader->contentEncoding, value);
                 break;
        case HTTP_ENTITY_HEADER_CONTENT_LANGUAGE:
                 strcpy(pRequest->entityHeader->contentLanguage, value);
                 break;
        case HTTP_ENTITY_HEADER_CONTENT_LENGTH:
                 strcpy(pRequest->entityHeader->contentLength, value);
                 break;
        case HTTP_ENTITY_HEADER_CONTENT_LOCATION:
                 strcpy(pRequest->entityHeader->contentLocation, value);
                 break;
        case HTTP_ENTITY_HEADER_CONTENT_MD5:
                 strcpy(pRequest->entityHeader->contentMD5, value);
                 break;
        case HTTP_ENTITY_HEADER_CONTENT_RANGE:
                 strcpy(pRequest->entityHeader->contentRange, value);
                 break;
        case HTTP_ENTITY_HEADER_CONTENT_TYPE:
                 strcpy(pRequest->entityHeader->contentType, value);
                 break;
        case HTTP_MISC_HEADER_ALL:
                 dwError = VmRESTSetHTTPMiscHeader(
                               pRequest->miscHeader,
                               header,
                               value
                               );
                 break;
        default:
                 VMREST_LOG_DEBUG("VmRESTSetHttpRequestHeader(): Header not found in req object");
                 dwError =  VMREST_HTTP_VALIDATION_FAILED;
                 *resStatus = BAD_REQUEST;
                 BAIL_ON_VMREST_ERROR(dwError);
                 break;
    }
cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VmRESTParseAndPopulateConfigFile(
    char*                            configFile,
    VM_REST_CONFIG**                 ppRESTConfig
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    FILE*                            fp = NULL;
    char                             word[MAX_LINE_LEN];
    char*                            result = NULL;
    VM_REST_CONFIG*                  pRESTConfig = NULL;
    uint32_t                         resultLen = 0;

    if (configFile == NULL)
    {
        VMREST_LOG_DEBUG("VmRESTParseAndPopulateConfigFile(): No Config file found");
        dwError = REST_ENGINE_MISSING_CONFIG;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTAllocateMemory(
              sizeof(VM_REST_CONFIG),
              (void**)&pRESTConfig
              );
    BAIL_ON_VMREST_ERROR(dwError);
    fp = fopen(
             configFile,
             "r"
         );
    if (fp == NULL)
    {
        VMREST_LOG_DEBUG("VmRESTParseAndPopulateConfigFile(): Unable to open config file");
        dwError = REST_ENGINE_INVALID_CONFIG;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    memset(word,'\0', MAX_LINE_LEN);
    while (fscanf(fp, "%s", word) != EOF)
    {
        if (resultLen != 0 && result != NULL)
        {
            strncpy(result, word, resultLen);
            resultLen = 0;
            result = NULL;
        }
        if (strcmp (word, "SSL-Certificate") == 0)
        {
            result = pRESTConfig->ssl_certificate;
            resultLen = MAX_PATH_LEN;
        }
        else if (strcmp(word, "SSL-Key") == 0)
        {
            result = pRESTConfig->ssl_key;
            resultLen = MAX_PATH_LEN;
        }
        else if (strcmp(word, "Port") == 0)
        {
            result = pRESTConfig->server_port;
            resultLen = MAX_SERVER_PORT_LEN;
        }
        else if (strcmp(word, "Log-File") == 0)
        {
            result = pRESTConfig->debug_log_file;
            resultLen = MAX_PATH_LEN;
        }
        else if (strcmp(word, "Client-Count") == 0)
        {
            result = pRESTConfig->client_count;
            resultLen = MAX_CLIENT_ALLOWED_LEN;
        }
        else if (strcmp(word, "Worker-Thread-Count") == 0)
        {
            result = pRESTConfig->worker_thread_count;
            resultLen = MAX_WORKER_COUNT_LEN;
        }
    }
    *ppRESTConfig = pRESTConfig;

cleanup:
    if (fp)
    {
        fclose(fp);
        fp = NULL;
    }
    return dwError;
error:
    if (pRESTConfig)
    {
        VmRESTFreeMemory(
            pRESTConfig
            );
    }
    *ppRESTConfig = NULL;
    goto cleanup;
}

void
VmRESTFreeConfigFileStruct(
    PVM_REST_CONFIG                  pRESTConfig
    )
{
    if (pRESTConfig)
    {
        VmRESTFreeMemory(
            pRESTConfig
            );
    }
}

uint32_t
VmRESTValidateConfig(
    PVM_REST_CONFIG                  pRESTConfig
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    if (pRESTConfig == NULL)
    {
        dwError = REST_ENGINE_FAILURE;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    if ((pRESTConfig->server_port == NULL) || (pRESTConfig->worker_thread_count == NULL) || (pRESTConfig->client_count == NULL))
    {
        VMREST_LOG_DEBUG("VmRESTValidateConfig(): Configuration Validation failed (check port/th cnt/client cnt)");
        dwError = REST_ENGINE_FAILURE;
    }
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VmRESTCopyConfig(
    PREST_CONF                       pConfig,
    PVM_REST_CONFIG*                 ppRESTConfig
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    PVM_REST_CONFIG                  pRESTConfig = NULL;

    if (pConfig == NULL)
    {
        VMREST_LOG_DEBUG("VmRESTCopyConfig(): No Config found");
        dwError = REST_ENGINE_MISSING_CONFIG;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTAllocateMemory(
                  sizeof(VM_REST_CONFIG),
                  (void**)&pRESTConfig
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    strcpy(pRESTConfig->ssl_certificate, pConfig->pSSLCertificate);
    strcpy(pRESTConfig->ssl_key, pConfig->pSSLKey);
    strcpy(pRESTConfig->server_port, pConfig->pServerPort);
    strcpy(pRESTConfig->debug_log_file, pConfig->pDebugLogFile);
    strcpy(pRESTConfig->client_count, pConfig->pClientCount);
    strcpy(pRESTConfig->worker_thread_count, pConfig->pMaxWorkerThread);

    *ppRESTConfig = pRESTConfig;

cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VmRESTSetHTTPMiscHeader(
    PMISC_HEADER_QUEUE               miscHeaderQueue,
    PCSTR                            header,
    PSTR                             value
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    PVM_REST_HTTP_HEADER_NODE        node = NULL;
    PVM_REST_HTTP_HEADER_NODE        temp = NULL;
    uint32_t                         headerLen = 0;
    uint32_t                         valueLen = 0;

    if (miscHeaderQueue == NULL || header == NULL || value == NULL)
    {
        VMREST_LOG_DEBUG("VmRESTSetHTTPMiscHeader: Invalid Params");
        dwError =  VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    headerLen = strlen(header);
    valueLen = strlen(value);

    if (headerLen >= MAX_HTTP_HEADER_ATTR_LEN || valueLen >= MAX_HTTP_HEADER_VAL_LEN)
    {
        VMREST_LOG_DEBUG("VmRESTSetHTTPMiscHeader: Header or value length too long");
        dwError = VMREST_HTTP_VALIDATION_FAILED;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    /**** Allocate the node ****/
    dwError = VmRESTAllocateMemory(
                  sizeof(VM_REST_HTTP_HEADER_NODE),
                  (void**)&node
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    strcpy(node->header, header);
    strcpy(node->value, value);
    node->next = NULL;

    if (miscHeaderQueue->head == NULL)
    {
        miscHeaderQueue->head = node;
    }
    else
    {
        temp = miscHeaderQueue->head;
        while(temp->next != NULL)
        {
            temp = temp->next;
        }
        temp->next = node;
    }

cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VmRESTRemoveAllHTTPMiscHeader(
    PMISC_HEADER_QUEUE               miscHeaderQueue
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    PVM_REST_HTTP_HEADER_NODE        node = NULL;
    PVM_REST_HTTP_HEADER_NODE        temp = NULL;

    temp = miscHeaderQueue->head;
    while (temp != NULL)
    {
        node = temp;
        temp = temp->next;
        node->next = NULL;
        VmRESTFreeMemory(
            node
            );
    }
    BAIL_ON_VMREST_ERROR(dwError);
    miscHeaderQueue->head = NULL;

cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VmRESTGetHTTPMiscHeader(
    PMISC_HEADER_QUEUE               miscHeaderQueue,
    PCSTR                            header,
    PSTR*                            response
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    PVM_REST_HTTP_HEADER_NODE        temp = NULL;

    if (miscHeaderQueue == NULL)
    {
        VMREST_LOG_DEBUG("VmRESTGetHTTPMiscHeader: Invalid Params");
        dwError =  VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    *response = NULL;
    temp = miscHeaderQueue->head;
    while (temp != NULL)
    {
        if (strcmp(temp->header, header) == 0)
        {
            *response = temp->value;
            break;
        }
        temp = temp->next;
    }

cleanup:
    return dwError;
error:
    goto cleanup;
}
