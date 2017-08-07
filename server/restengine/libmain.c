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

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

#endif

#ifdef WIN32
#pragma comment(lib, "Ws2_32.lib")
#endif


uint32_t
VmRESTInit(
    PREST_CONF                       pConfig,
    char const*                      file,
    PVMREST_HANDLE*                  ppRESTHandle
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    PVMREST_HANDLE                   pRESTHandle = NULL;

    if (!ppRESTHandle)
    {
        dwError = REST_ERROR_INVALID_HANDLER;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    /**** 1. Allocate the handle object- this is per instance basis ****/

    dwError = VmRESTAllocateHandle(
                  &pRESTHandle
                  );
    BAIL_ON_VMREST_ERROR(dwError);

   /**** 2. Init HTTP ****/

    dwError = VmHTTPInit(
                  pRESTHandle,
                  pConfig,
                  file
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    *ppRESTHandle = pRESTHandle;

cleanup:
    return dwError;
error:
    if (pRESTHandle)
    {
        VmRESTFreeHandle(pRESTHandle);
    }

    if (ppRESTHandle)
    {
        *ppRESTHandle = NULL;
    }

    goto cleanup;
}

uint32_t
VmRESTSetSSLInfo(
     PVMREST_HANDLE                   pRESTHandle,
     char*                            pDataBuffer,
     uint32_t                         bufferSize,
     uint32_t                         sslDataType
     )
{
     uint32_t                         dwError = REST_ENGINE_SUCCESS;
     char                             fileName[MAX_PATH_LEN] = {0};
     FILE*                            fp = NULL;
     uint32_t                         writtenBytes = 0;

    if (!pRESTHandle || !pDataBuffer || (bufferSize == 0) || (bufferSize > MAX_SSL_DATA_BUF_LEN) || (sslDataType < SSL_DATA_TYPE_KEY) || (sslDataType > SSL_DATA_TYPE_CERT))
    {
        dwError = REST_ERROR_INVALID_HANDLER;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    if (((pRESTHandle->pSSLInfo->isCertSet != SSL_INFO_NOT_SET) && (sslDataType == SSL_DATA_TYPE_CERT)))
    {
        dwError = REST_ENGINE_SSL_CONFIG_FILE;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    if (((pRESTHandle->pSSLInfo->isKeySet != SSL_INFO_NOT_SET) && (sslDataType == SSL_DATA_TYPE_KEY)))
    {
        dwError = REST_ENGINE_SSL_CONFIG_FILE;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    if (sslDataType == SSL_DATA_TYPE_KEY)
    {
        snprintf(fileName, (MAX_PATH_LEN - 1),"%s%s.pem", "/tmp/key-port-", pRESTHandle->pRESTConfig->server_port);

    }
    else if (sslDataType == SSL_DATA_TYPE_CERT)
    {
        snprintf(fileName, (MAX_PATH_LEN - 1), "%s%s.pem", "/tmp/cert-port-", pRESTHandle->pRESTConfig->server_port);
    }

    fp = fopen(fileName, "w+");
    if (fp == NULL)
    {
        VMREST_LOG_ERROR(pRESTHandle,"Unable to Open SSL file %s", fileName);
        dwError = REST_ENGINE_FAILURE;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    writtenBytes = fwrite(pDataBuffer, 1, bufferSize, fp);
    fclose(fp);
    
    if (writtenBytes != bufferSize)
    {
        VMREST_LOG_WARNING(pRESTHandle,"Not all buffer bytes written to file, requested %u, written %u", bufferSize, writtenBytes);
    }

    if (sslDataType == SSL_DATA_TYPE_KEY)
    {
        memset(pRESTHandle->pRESTConfig->ssl_key, '\0', MAX_PATH_LEN);
        strncpy(pRESTHandle->pRESTConfig->ssl_key, fileName,( MAX_PATH_LEN - 1));
        pRESTHandle->pSSLInfo->isKeySet = SSL_INFO_FROM_BUFFER_API;

    }
    else if (sslDataType == SSL_DATA_TYPE_CERT)
    {
        memset(pRESTHandle->pRESTConfig->ssl_certificate, '\0', MAX_PATH_LEN);
        strncpy(pRESTHandle->pRESTConfig->ssl_certificate, fileName,( MAX_PATH_LEN - 1));
        pRESTHandle->pSSLInfo->isCertSet = SSL_INFO_FROM_BUFFER_API;
    }

cleanup:
    return dwError;
error:
    goto cleanup;

}


#ifdef WIN32
__declspec(dllexport)
#endif
uint32_t
VmRESTStart(
    PVMREST_HANDLE                   pRESTHandle
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;

    if (!pRESTHandle)
    {
        dwError = REST_ERROR_INVALID_REST_PROCESSER;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    if (pRESTHandle->pSSLInfo->isCertSet >= 1 && pRESTHandle->pSSLInfo->isKeySet >= 1)
    {
        dwError = VmHTTPStart(
                      pRESTHandle
                      );
    }
    else
    {
        VMREST_LOG_ERROR(pRESTHandle, "Cannot Start Server due to missing SSL key or certificate");
    }
    BAIL_ON_VMREST_ERROR(dwError);

    /**** delete the cert file if created by library ****/

    if (pRESTHandle->pSSLInfo->isCertSet == SSL_INFO_FROM_BUFFER_API)
    {
        remove(pRESTHandle->pRESTConfig->ssl_certificate);
    }
    if (pRESTHandle->pSSLInfo->isKeySet == SSL_INFO_FROM_BUFFER_API)
    {
        remove(pRESTHandle->pRESTConfig->ssl_key);
    }

cleanup:
    return dwError;
error:
    goto cleanup;

}

uint32_t
VmRESTRegisterHandler(
    PVMREST_HANDLE                   pRESTHandle,
    char const*                      pszEndpoint,
    PREST_PROCESSOR                  pHandler,
    PREST_ENDPOINT*                  ppEndpoint
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    PREST_PROCESSOR                  pzHandler = NULL;

    if (!pHandler || !pRESTHandle)
    {
        dwError = REST_ERROR_INVALID_REST_PROCESSER;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    if (!pszEndpoint)
    {
        /**** Interact directly with HTTP ****/
        pzHandler = pHandler;
    }
    else if (pszEndpoint != NULL)
    {
        /**** Endpoint based registration ****/
        if (pRESTHandle->pInstanceGlobal->useEndPoint == 0)
        {
            dwError = VmRestEngineInitEndPointRegistration(
                          pRESTHandle
                          );
            BAIL_ON_VMREST_ERROR(dwError);
        }
        dwError = VmRestEngineAddEndpoint(
                      pRESTHandle,
                      (char  *)pszEndpoint,
                      pHandler
                      );
        BAIL_ON_VMREST_ERROR(dwError);
        pzHandler = &(pRESTHandle->pInstanceGlobal->internalHandler);

    }
    if (!(pRESTHandle->pHttpHandler))
    {
        dwError = VmHTTPRegisterHandler(
                      pRESTHandle,
                      pzHandler
                      );
    }
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VmRESTFindEndpoint(
    PVMREST_HANDLE                   pRESTHandle,
    char const*                      pszEndpoint,
    PREST_ENDPOINT*                  ppEndpoint
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    PREST_ENDPOINT                   temp = NULL;
    PREST_ENDPOINT                   pEndPoint = NULL;

    dwError = VmRestEngineGetEndPoint(
                  pRESTHandle,
                  (char*)pszEndpoint,
                  &temp
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    /**** Allocate and copy ****/
    dwError = VmRESTAllocateEndPoint(
                  &pEndPoint
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    strcpy(pEndPoint->pszEndPointURI,temp->pszEndPointURI);
    if (temp->pHandler != NULL)
    {
        pEndPoint->pHandler->pfnHandleRequest = temp->pHandler->pfnHandleRequest;
        pEndPoint->pHandler->pfnHandleCreate = temp->pHandler->pfnHandleCreate;
        pEndPoint->pHandler->pfnHandleDelete = temp->pHandler->pfnHandleDelete;
        pEndPoint->pHandler->pfnHandleUpdate = temp->pHandler->pfnHandleUpdate;
        pEndPoint->pHandler->pfnHandleRead = temp->pHandler->pfnHandleRead;
        pEndPoint->pHandler->pfnHandleOthers = temp->pHandler->pfnHandleOthers;
        /**** Dont give the next pointer ****/
        temp->next = NULL;
    }

    *ppEndpoint = pEndPoint;
    
cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VmRESTUnRegisterHandler(
    PVMREST_HANDLE                   pRESTHandle,
    char const*                      pzEndPointURI
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;

    if (!pzEndPointURI)
    {
        /**** Interacting with HTTP directly ****/
        dwError = VmHTTPUnRegisterHandler(
                      pRESTHandle
                      );
    }
    else
    {
        /**** Endpoint based library instance ****/
        dwError = VmRestEngineRemoveEndpoint(
                      pRESTHandle,
                      pzEndPointURI
                      );
    }
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VmRESTStop(
    PVMREST_HANDLE                   pRESTHandle
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;

    if (!pRESTHandle)
    {  
        dwError = REST_ERROR_INVALID_HANDLER;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmHTTPStop(
                  pRESTHandle
                  );
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:
    return dwError;
error:
    goto cleanup;
}

void
VmRESTShutdown(
    PVMREST_HANDLE                   pRESTHandle
    )
{
    if (pRESTHandle->pInstanceGlobal->useEndPoint == 1)
    {
        VmRestEngineShutdownEndPointRegistration(
            pRESTHandle
            );
    }

    VmHTTPShutdown(pRESTHandle);
}

uint32_t
VmRESTGetData(
    PVMREST_HANDLE                   pRESTHandle,
    PREST_REQUEST                    pRequest,
    char*                            pBuffer,
    uint32_t*                        bytesRead
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;

    dwError = VmRESTGetHttpPayload(
                  pRESTHandle,
                  pRequest,
                  pBuffer,
                  bytesRead
                  );
                  
    return dwError;
}

uint32_t
VmRESTSetData(
    PVMREST_HANDLE                   pRESTHandle,
    PREST_RESPONSE*                  ppResponse,
    char const*                      buffer,
    uint32_t                         dataLen,
    uint32_t*                        bytesWritten
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;

    dwError = VmRESTSetHttpPayload(
                  pRESTHandle,
                  ppResponse,
                  buffer,
                  dataLen,
                  bytesWritten
                  );

    return dwError;

}

uint32_t
VmRESTSetSuccessResponse(
    PREST_REQUEST                    pRequest,
    PREST_RESPONSE*                  ppResponse
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    char*                            connection = NULL;

    dwError = VmRESTSetHttpStatusCode(
                  ppResponse,
                  "200"
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTSetHttpStatusVersion(
                  ppResponse,
                  "HTTP/1.1"
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTSetHttpReasonPhrase(
                  ppResponse,
                  "OK"
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTGetHttpHeader(
                  pRequest,
                  "Connection",
                  &connection
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    if ((connection != NULL) && (strcmp(connection, " keep-alive") == 0))
    {
        dwError = VmRESTSetHttpHeader(
                      ppResponse,
                      "Connection",
                      "keep-alive"
                      );
    }
    else
    {
        dwError = VmRESTSetHttpHeader(
                      ppResponse,
                      "Connection",
                      "close"
                      );
    }
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:
    if (connection != NULL)
    {
        VmRESTFreeMemory(connection);
        connection = NULL;
    }
    return dwError;
error:
    goto cleanup;

}

uint32_t
VmRESTSetFailureResponse(
    PREST_RESPONSE*                  ppResponse,
    char const*                      pErrorCode,
    char const*                      pErrorMessage
    )
{   
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    char                             errorCode[MAX_STATUS_LEN] = {0};
    char                             errorMessage[MAX_REA_PHRASE_LEN] = {0};

    memset(errorCode, '\0', MAX_STATUS_LEN);
    memset(errorMessage, '\0', MAX_REA_PHRASE_LEN);

    /**** If error code and message is not provided, send internal server error ****/

    if (!pErrorCode)
    {
        strcpy(errorCode,"500");
    }
    else if ((strlen(pErrorCode) > 0) && (strlen(pErrorCode) <= MAX_STATUS_LEN))
    {
        strcpy(errorCode, pErrorCode);
    }

    if (!pErrorMessage)
    {
        strcpy(errorMessage, "Internal Server Error");
    }
    else if ((strlen(pErrorMessage) > 0) && (strlen(pErrorMessage) <= MAX_REA_PHRASE_LEN))
    {
        strcpy(errorMessage, pErrorMessage);
    }

    dwError = VmRESTSetHttpStatusCode(
                  ppResponse,
                  errorCode
                  );
    BAIL_ON_VMREST_ERROR(dwError);
    
    dwError = VmRESTSetHttpStatusVersion(
                  ppResponse,
                  "HTTP/1.1"
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTSetHttpReasonPhrase(
                  ppResponse,
                  errorMessage
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTSetHttpHeader(
                  ppResponse,
                  "Connection",
                  "close"
                  );
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VmRESTSetDataLength(
    PREST_RESPONSE*                  ppResponse,
    char*                            dataLen
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;

    if ((dataLen != NULL) && (atoi(dataLen) <= MAX_DATA_BUFFER_LEN))
    {
        dwError = VmRESTSetHttpHeader(
                      ppResponse,
                      "Content-Length",
                      dataLen
                      );
    }
    else if (dataLen == NULL)
    {
        dwError = VmRESTSetHttpHeader(
                      ppResponse,
                      "Transfer-Encoding",
                      "chunked"
                      );
    }
    else
    {
        dwError = VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:
    return dwError;
error:
    goto cleanup;
}
