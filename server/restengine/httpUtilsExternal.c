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

    if (!(pRequest) || !(pRequest->requestLine) || !(ppResponse))
    {
        VMREST_LOG_ERROR("Invalid params");
        dwError = VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    methodLen = strlen(pRequest->requestLine->method);
    if (methodLen == 0)
    {
        VMREST_LOG_ERROR("Method seems invalid");
        dwError = VMREST_HTTP_VALIDATION_FAILED;
    }
    BAIL_ON_VMREST_ERROR(dwError);
    *ppResponse = pRequest->requestLine->method;
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

    if (!(pRequest) || !(pRequest->requestLine) || !(ppResponse))
    {
        VMREST_LOG_ERROR("Invalid params");
        dwError = VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    uriLen = strlen(pRequest->requestLine->uri);
    if (uriLen == 0 || uriLen > MAX_URI_LEN)
    {
        VMREST_LOG_ERROR("URI seems invalid in request object");
        dwError = VMREST_HTTP_VALIDATION_FAILED;
    }
    BAIL_ON_VMREST_ERROR(dwError);
    *ppResponse = pRequest->requestLine->uri;

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

    if (!(pRequest) || !(pRequest->requestLine) || !(ppResponse))
    {
        VMREST_LOG_ERROR("Invalid params");
        dwError = VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    versionLen = strlen(pRequest->requestLine->version);
    if (versionLen == 0 || versionLen > MAX_VERSION_LEN)
    {
        VMREST_LOG_ERROR("Version info invalid in request object");
        dwError = VMREST_HTTP_VALIDATION_FAILED;
    }
    BAIL_ON_VMREST_ERROR(dwError);
    *ppResponse = pRequest->requestLine->version;

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
    char const*                      header,
    char**                           ppResponse
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    size_t                           headerValLen = 0;

    if (!(pRequest) || !(header) || !(ppResponse))
    {
        VMREST_LOG_ERROR("Invalid params");
        dwError = VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTGetHTTPMiscHeader(
                  pRequest->miscHeader,
                  header,
                  ppResponse
                  );
    BAIL_ON_VMREST_ERROR(dwError);
    if (*ppResponse)
    {
         headerValLen = strlen(*ppResponse);
         if (headerValLen > MAX_HTTP_HEADER_VAL_LEN)
         {
             dwError = VMREST_HTTP_VALIDATION_FAILED;
         }
    }
    else
    {
       //VMREST_LOG_DEBUG("%s","WARNING :: Header %s not found in request object", header);
    }
    BAIL_ON_VMREST_ERROR(dwError);

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
VmRESTGetHttpPayload(
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
    uint32_t                         maxTry = 5000;
    char*                            res = NULL;
    char*                            contentLength = NULL;
    char*                            transferEncoding = NULL;

    if (!pRequest || !response  || !done)
    {
        VMREST_LOG_ERROR("Invalid params");
        dwError = VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);
    *done = 0;

    if (sizeof(response) > MAX_DATA_BUFFER_LEN)
    {
        VMREST_LOG_ERROR("Response buffer size %u not large enough",sizeof(response));
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
tryagain:
        dwError = VmsockPosixGetXBytes(
                      readXBytes,
                      localAppBuffer,
                      pRequest->pSocket,
                      &bytesRead,
                      1
                      );

        /**** If Expect:100-continue was received, re-attempt read considering RTT delay ****/
        if (dwError !=0 && pRequest->dataNotRcvd == 1 && tryCnt < maxTry)
        { 
            tryCnt++;
            goto tryagain;
        }
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
            VMREST_LOG_ERROR("No data available over socket to read");
            *done = 1;
        }
    }
    else if((transferEncoding != NULL) && (strcmp(transferEncoding,"chunked")) == 0)
    {
        res = response;
        dataRemaining = pRequest->dataRemaining;
        if (dataRemaining == 0)
        {
            readXBytes = HTTP_CHUNCKED_DATA_LEN;
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
tryagain1:
        dwError = VmsockPosixGetXBytes(
                      readXBytes,
                      localAppBuffer,
                      pRequest->pSocket,
                      &bytesRead,
                      1
                      );

        /**** If Expect:100-continue was received, re-attempt read considering RTT delay ****/
        if (dwError !=0 && pRequest->dataNotRcvd == 1 && tryCnt < maxTry)
        {
            tryCnt++;
            goto tryagain1;
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
       //VMREST_LOG_DEBUG("%s","WARNING: Data length Specific Header not set");
        *done = 1;
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
    char*                            buffer,
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
        VMREST_LOG_ERROR("Invalid params");
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
            VMREST_LOG_ERROR("Invalid content length %u", contentLen);
            dwError = VMREST_HTTP_VALIDATION_FAILED;
        }
        BAIL_ON_VMREST_ERROR(dwError);

        dwError = VmRESTSendHeaderAndPayload(
                      ppResponse
                      );
       VMREST_LOG_DEBUG("Sending Header and Payload done, returned code %u", dwError);
        BAIL_ON_VMREST_ERROR(dwError);
        pResponse->headerSent = 1;
        *done = 1;
    }
    else if ((transferEncoding != NULL) && (strcmp(transferEncoding, "chunked") == 0))
    {
         if (dataLen > MAX_DATA_BUFFER_LEN)
         {
             VMREST_LOG_ERROR("Chunked data length %u not allowed", dataLen);
             dwError = VMREST_HTTP_VALIDATION_FAILED;
         }
         BAIL_ON_VMREST_ERROR(dwError);

         memcpy(pResponse->messageBody->buffer, buffer, dataLen);
         if (pResponse->headerSent == 0)
         {
             /**** Send Header first ****/
             dwError = VmRESTSendHeader(
                           ppResponse
                           );
             BAIL_ON_VMREST_ERROR(dwError);
             pResponse->headerSent = 1;
         }
         dwError = VmRESTSendChunkedPayload(
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
        VMREST_LOG_ERROR("Both Content length and TransferEncoding missing");
        dwError = VMREST_HTTP_VALIDATION_FAILED;
    }
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:
    return dwError;
error:
    VMREST_LOG_ERROR("Set Payload Failed");
    goto cleanup;
}


uint32_t
VmRESTSetHttpHeader(
    PREST_RESPONSE*                  ppResponse,
    char const*                      header,
    char*                            value
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    PREST_RESPONSE                   pResponse = NULL;

    if (!ppResponse  || (*ppResponse == NULL) || !header  || !value)
    {
        VMREST_LOG_ERROR("Invalid params");
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
    char*                            statusCode
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    size_t                           statusLen = 0;
    PREST_RESPONSE    pResponse = NULL;

    if (!ppResponse || (*ppResponse == NULL) || !statusCode)
    {
        /* Response object not allocated any memory */
        VMREST_LOG_ERROR("Invalid params");
        dwError = VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    pResponse = *ppResponse;
    statusLen = strlen(statusCode);

    if (statusLen >= MAX_STATUS_LEN)
    {
        VMREST_LOG_ERROR("Status length too large");
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
    char*                            version
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    size_t                           versionLen = 0;
    PREST_RESPONSE                   pResponse = NULL;


    if (!ppResponse || (*ppResponse == NULL) || !version)
    {
        VMREST_LOG_ERROR("Invalid params");
        dwError = VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    pResponse = *ppResponse;

    versionLen = strlen(version);

    if (versionLen > MAX_VERSION_LEN)
    {
        VMREST_LOG_ERROR("Bad version length");
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
    char*                            reasonPhrase
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    PREST_RESPONSE                   pResponse = NULL;


    if (!ppResponse  || (*ppResponse == NULL) || !reasonPhrase)
    {
        VMREST_LOG_ERROR("Invalid params");
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

