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

    VmRESTDecodeEncodedURLString(
       pRequest->requestLine->uri,
       pHttpURI
       );

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
    uint32_t*                        done
    )
{

    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    uint32_t                         dataRemaining = 0;
    char                             localAppBuffer[MAX_DATA_BUFFER_LEN] = {0};
    uint32_t                         chunkLenBytes = 0;
    uint32_t                         chunkLen = 0;
    uint32_t                         bytesRead = 0;
    uint32_t                         readXBytes = 0;
    uint32_t                         actualBytesCopied = 0;
    uint32_t                         newChunk = 0;
    uint32_t                         extraRead = 0;
    uint32_t                         tryCnt = 0;
    char*                            res = NULL;
    char*                            contentLength = NULL;
    char*                            transferEncoding = NULL;

    if (!pRequest || !response  || !done)
    {
        VMREST_LOG_ERROR(pRESTHandle,"Invalid params");
        dwError = VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);
    *done = 0;

    if (sizeof(response) > MAX_DATA_BUFFER_LEN)
    {
        VMREST_LOG_ERROR(pRESTHandle,"Response buffer size %u not large enough",sizeof(response));
        dwError = VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);
    memset(localAppBuffer,'\0', MAX_DATA_BUFFER_LEN);

    dwError = VmRESTGetHttpHeader(
                  pRequest,
                  "Content-Length",
                  &contentLength
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTGetHttpHeader(
                  pRequest,
                  "Transfer-Encoding",
                  &transferEncoding
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    if (contentLength != NULL && strlen(contentLength) > 0)
    {
       /**** Content-Length based packets ****/

        dataRemaining = pRequest->dataRemaining;
        if ((dataRemaining > 0) && (dataRemaining <= MAX_DATA_BUFFER_LEN))
        {
            readXBytes = dataRemaining;
        }
        else if (dataRemaining > MAX_DATA_BUFFER_LEN)
        {
            readXBytes = MAX_DATA_BUFFER_LEN;
        }

        /**** If Expect:100-continue was received, re-attempt read considering RTT delay ****/
        do
        {
            dwError = VmsockPosixGetXBytes(
                      pRESTHandle,
                      readXBytes,
                      localAppBuffer,
                      pRequest->pSocket,
                      &bytesRead,
                      1
                      );
            tryCnt++;
        } while (dwError !=0 && pRequest->dataNotRcvd == 1 && tryCnt < MAX_READ_RETRIES);
        BAIL_ON_VMREST_ERROR(dwError);

        if (pRequest->dataNotRcvd == 1)
        {
            pRequest->dataNotRcvd = 0;
        }

        dwError = VmRESTCopyDataWithoutCRLF(
                      bytesRead,
                      localAppBuffer,
                      response,
                      &actualBytesCopied
                      );
        BAIL_ON_VMREST_ERROR(dwError);
        pRequest->dataRemaining = pRequest->dataRemaining - actualBytesCopied;

        if (pRequest->dataRemaining == 0)
        {
            *done = 1;
        }
        if (bytesRead == 0 && readXBytes != 0)
        {
            dwError = VMREST_HTTP_VALIDATION_FAILED;
            VMREST_LOG_ERROR(pRESTHandle,"No data available over socket to read");
            *done = 1;
        }
    }
    else if((transferEncoding != NULL) && (strcmp(transferEncoding,"chunked")) == 0)
    {
        res = response;
        dataRemaining = pRequest->dataRemaining;
        if (dataRemaining == 0)
        {
            readXBytes = HTTP_CHUNKED_DATA_LEN;
            newChunk = 1;
        }
        else if (dataRemaining > MAX_DATA_BUFFER_LEN)
        {
            readXBytes = MAX_DATA_BUFFER_LEN;
            newChunk = 0;
        }
        else if (dataRemaining < MAX_DATA_BUFFER_LEN)
        {
            readXBytes = dataRemaining;
            newChunk = 0;
        }

        /**** This is chunked encoded packet ****/

        /**** If Expect:100-continue was received, re-attempt read considering RTT delay ****/
        do
        {
            dwError = VmsockPosixGetXBytes(
                          pRESTHandle,
                          readXBytes,
                          localAppBuffer,
                          pRequest->pSocket,
                          &bytesRead,
                          1
                          );
            tryCnt++;
        } while (dwError == ERROR_SYS_CALL_FAILED && pRequest->dataNotRcvd == 1 && tryCnt < MAX_READ_RETRIES);

        /**** Cross examine size if its last chuck ****/
        if (dwError == ERROR_SYS_CALL_FAILED && bytesRead > 0 && bytesRead < HTTP_CHUNKED_DATA_LEN)
        {
            dwError = 0;
        }
        BAIL_ON_VMREST_ERROR(dwError);

        if (pRequest->dataNotRcvd == 1)
        {
            pRequest->dataNotRcvd = 0;
        }

        if (newChunk)
        {
            dwError = VmRESTGetChunkSize(
                          localAppBuffer,
                          &chunkLenBytes,
                          &chunkLen
                          );
            BAIL_ON_VMREST_ERROR(dwError);
            pRequest->dataRemaining = chunkLen;
            VMREST_LOG_DEBUG(pRESTHandle,"Chunk Len = %u", chunkLen);
            if (chunkLen == 0)
            {
                *done = 1;
            }
            if (*done == 0)
            {
                /**** Copy the extra data from last read if it exists ****/
                extraRead = bytesRead - chunkLenBytes;
                if (extraRead > 0)
                {
                    memcpy(res, (localAppBuffer + chunkLenBytes), extraRead);
                    res = res + extraRead;
                    pRequest->dataRemaining = pRequest->dataRemaining - extraRead;
                }

                memset(localAppBuffer,'\0',MAX_DATA_BUFFER_LEN);

                if (pRequest->dataRemaining > (MAX_DATA_BUFFER_LEN - extraRead))
                {
                    readXBytes = MAX_DATA_BUFFER_LEN -extraRead;
                }
                else if (pRequest->dataRemaining <= (MAX_DATA_BUFFER_LEN - extraRead))
                {
                    readXBytes = pRequest->dataRemaining;
                }
                dwError = VmsockPosixGetXBytes(
                              pRESTHandle,
                              readXBytes,
                              localAppBuffer,
                              pRequest->pSocket,
                              &bytesRead,
                              1
                              );
                BAIL_ON_VMREST_ERROR(dwError);

                dwError = VmRESTCopyDataWithoutCRLF(
                              bytesRead,
                              localAppBuffer,
                              res,
                              &actualBytesCopied
                              );
                BAIL_ON_VMREST_ERROR(dwError);
                pRequest->dataRemaining = pRequest->dataRemaining - actualBytesCopied;

                /**** Read the /r/n succeeding the chunk ****/
                if (pRequest->dataRemaining == 0)
                {
                     dwError = VmsockPosixGetXBytes(
                                  pRESTHandle,
                                  2,
                                  localAppBuffer,
                                  pRequest->pSocket,
                                  &bytesRead,
                                  0
                                  );
                BAIL_ON_VMREST_ERROR(dwError);
                }
            }
        }
    }
    else
    {
        *done = 1;
    }
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:
    if (contentLength != NULL)
    {
        VmRESTFreeMemory(contentLength);
    }
    if (transferEncoding != NULL)
    {
        VmRESTFreeMemory(transferEncoding);
    }

    return dwError;
error:
    response = NULL;
    if (dwError == ERROR_SYS_CALL_FAILED)
    {
        dwError = LENGTH_REQUIRED;
    }
    goto cleanup;
}

uint32_t
VmRESTSetHttpPayload(
    PVMREST_HANDLE                   pRESTHandle,
    PREST_RESPONSE*                  ppResponse,
    char const*                      buffer,
    uint32_t                         dataLen,
    uint32_t*                        done
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    uint32_t                         contentLen = 0;
    PREST_RESPONSE                   pResponse = NULL;
    char*                            contentLength = NULL;
    char*                            transferEncoding = NULL;


    if (!ppResponse  || (*ppResponse == NULL) || !buffer || !done)
    {
        VMREST_LOG_ERROR(pRESTHandle,"Invalid params");
        dwError = VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    pResponse = *ppResponse;
    *done = 0;

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
        pResponse->headerSent = 1;
        *done = 1;
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
         if (pResponse->headerSent == 0)
         {
             /**** Send Header first ****/
             dwError = VmRESTSendHeader(
                           pRESTHandle,
                           ppResponse
                           );
             BAIL_ON_VMREST_ERROR(dwError);
             pResponse->headerSent = 1;
         }
         dwError = VmRESTSendChunkedPayload(
                       pRESTHandle,
                       ppResponse,
                       dataLen
                       );
         BAIL_ON_VMREST_ERROR(dwError);
         if (dataLen == 0)
         {
             *done = 1;
         }
    }
    else
    {
        VMREST_LOG_ERROR(pRESTHandle,"Both Content length and TransferEncoding missing");
        dwError = VMREST_HTTP_VALIDATION_FAILED;
    }
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:
    return dwError;
error:
    VMREST_LOG_ERROR(pRESTHandle,"Set Payload Failed");
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

