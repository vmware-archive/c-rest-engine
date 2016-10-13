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

#include "includes.h"



uint32_t
VmRESTCopyString(
    char*                            src,
    char*                            des
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    size_t                           headerValLen = 0;

    if ( !src  || !des )
    {
        VMREST_LOG_ERROR("Invalid params");
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
        VMREST_LOG_ERROR("Bad source length");
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
    char**                           response
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    size_t                           headerValLen = 0;

    if (!pResponse || !header || !response )
    {
        VMREST_LOG_ERROR("Invalid params");
        dwError = VMREST_APPLICATION_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTGetHTTPMiscHeader(
                  pResponse->miscHeader,
                  header,
                  response
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    if (*response)
    {
         headerValLen = strlen(*response);
         if (headerValLen < 0 || headerValLen > MAX_HTTP_HEADER_VAL_LEN)
         {
             dwError = VMREST_HTTP_VALIDATION_FAILED;
         }
    }
    else
    {
        VMREST_LOG_DEBUG("WARNING :: Header %s not found in request object", header);
    }
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

    if (!statusCode || !result || !reasonPhrase)
    {
        VMREST_LOG_ERROR("Invalid params");
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

    if (!pRequest || !header || !value || (*resStatus != OK))
    {
        VMREST_LOG_ERROR("Invalid Params");
        dwError =  VMREST_HTTP_INVALID_PARAMS;
        *resStatus = BAD_REQUEST;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTSetHTTPMiscHeader(
                  pRequest->miscHeader,
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

    if (!configFile)
    {
        VMREST_LOG_ERROR("No Config file found");
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
        VMREST_LOG_ERROR("Unable to open config file");
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

    /**** Use default if thread count or client count missing ****/
    if (strlen(pRESTConfig->worker_thread_count) == 0)
    {
        strcpy(pRESTConfig->worker_thread_count, DEFAULT_WORKER_THR_CNT);
    }
    if (strlen(pRESTConfig->client_count) == 0)
    {
        strcpy(pRESTConfig->client_count, DEFAULT_CLIENT_CNT);
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
    if (!pRESTConfig)
    {
        dwError = REST_ENGINE_FAILURE;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    if (!(pRESTConfig->server_port))
    {
        VMREST_LOG_ERROR("Configuration Validation failed: Port not specified");
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

    if (!pConfig)
    {
        VMREST_LOG_ERROR("No Config found");
        dwError = REST_ENGINE_MISSING_CONFIG;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTAllocateMemory(
                  sizeof(VM_REST_CONFIG),
                  (void**)&pRESTConfig
                  );
    BAIL_ON_VMREST_ERROR(dwError);
  
    if (pConfig->pSSLCertificate)
    {
        strcpy(pRESTConfig->ssl_certificate, pConfig->pSSLCertificate);
    }
    if (pConfig->pSSLKey)
    {
        strcpy(pRESTConfig->ssl_key, pConfig->pSSLKey);
    }
    if (pConfig->pServerPort)
    {
        strcpy(pRESTConfig->server_port, pConfig->pServerPort);
    }
    if (pConfig->pDebugLogFile)
    {
        strcpy(pRESTConfig->debug_log_file, pConfig->pDebugLogFile);
    }
    if (pConfig->pClientCount) 
    {
        strcpy(pRESTConfig->client_count, pConfig->pClientCount);
    }
    else
    {
        strcpy(pRESTConfig->worker_thread_count, DEFAULT_WORKER_THR_CNT);
    }

    if (pConfig->pMaxWorkerThread)
    {
        strcpy(pRESTConfig->worker_thread_count, pConfig->pMaxWorkerThread);
    }
    else
    {
        strcpy(pRESTConfig->client_count, DEFAULT_CLIENT_CNT);
    }

    *ppRESTConfig = pRESTConfig;

cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VmRESTSetHTTPMiscHeader(
    PMISC_HEADER_QUEUE               miscHeaderQueue,
    char const*                      header,
    char*                            value
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    PVM_REST_HTTP_HEADER_NODE        node = NULL;
    PVM_REST_HTTP_HEADER_NODE        temp = NULL;
    size_t                           headerLen = 0;
    size_t                           valueLen = 0;

    if (!miscHeaderQueue || !header || !value)
    {
        VMREST_LOG_ERROR("Invalid Params");
        dwError =  VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    headerLen = strlen(header);
    valueLen = strlen(value);

    if (headerLen >= MAX_HTTP_HEADER_ATTR_LEN || valueLen >= MAX_HTTP_HEADER_VAL_LEN)
    {
        VMREST_LOG_ERROR("Header or value length too long");
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
	int x = 1;

    temp = miscHeaderQueue->head;
    while (temp != NULL)
    {
        node = temp;
        temp = temp->next;
        //node->next = NULL;
		x++;
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
    char const*                      header,
    char**                           response
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    PVM_REST_HTTP_HEADER_NODE        temp = NULL;

    if (!miscHeaderQueue)
    {
        VMREST_LOG_ERROR("Invalid Params");
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

uint32_t
VmRESTGetChunkSize(
    char*                            lineStart,
    uint32_t*                        skipBytes,
    uint32_t*                        chunkSize
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    char*                            temp = NULL;
    char*                            line = NULL;
    char                             local[HTTP_CHUNCKED_DATA_LEN] = {0};
    uint32_t                         count = 0;
    uint32_t                         done = 0;
    long int                         hexToDec = 0;

    if (!lineStart || !skipBytes || !chunkSize)
    {
        VMREST_LOG_ERROR("Invalid Params");
        dwError =  VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);
    memset(local,'\0', HTTP_CHUNCKED_DATA_LEN);
    temp = lineStart;
    line = local;
    while ((count < (HTTP_CHUNCKED_DATA_LEN - 1)) && (*temp != '\0'))
    {
         if(*temp == '\r' && *(temp + 1) == '\n')
         {
             *line = '\0';
             done = 1;
             break;
         }
         *line = *temp;
         line++;
         temp++;
         count++;
    }
    if (done)
    {
        /**** now the local buffer contains size in hex ****/
        hexToDec = strtol(local,NULL,16);
        *chunkSize = (uint32_t)hexToDec;
        *skipBytes = count + 2;
    }
    else
    {
        VMREST_LOG_ERROR("Error in parsing content length");
        dwError =  VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);


cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VmRESTCopyDataWithoutCRLF(
    uint32_t                         maxBytes,
    char*                            src,
    char*                            des,
    uint32_t*                        actualBytes
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    uint32_t                         bytesCounter = 0;
    char*                            temp = NULL;
    char*                            res = NULL;
    uint32_t                         skip = 0;

    if (!src || !des || !actualBytes)
    {
        VMREST_LOG_ERROR("Invalid Params");
        dwError =  VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    if (maxBytes == 0)
    {
        *actualBytes = 0;
        goto cleanup;
    }

    temp = des;
    res = src;
  
    while (bytesCounter < (maxBytes -1))
    {
        if(*res == '\r' && *(res+1) == '\n')
        {
            skip = skip +2;
            res = res + 2;
            bytesCounter = bytesCounter + 2;
            continue;
        }
        *temp = *res;
        temp++;
        res++;
        bytesCounter++;
    }
    *temp = *res;
    bytesCounter++;
    *actualBytes = bytesCounter - skip;

cleanup:
    return dwError;
error:
    goto cleanup;
}
