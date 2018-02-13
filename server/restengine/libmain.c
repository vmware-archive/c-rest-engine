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
    PVMREST_HANDLE*                  ppRESTHandle
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    PVMREST_HANDLE                   pRESTHandle = NULL;

    if (!pConfig || !ppRESTHandle)
    {
        dwError = REST_ENGINE_ERROR_INVALID_PARAM;
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
                  pConfig
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    pRESTHandle->instanceState = VMREST_INSTANCE_INITIALIZED;

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
        dwError = REST_ENGINE_ERROR_INVALID_PARAM;
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
        snprintf(fileName, (MAX_PATH_LEN - 1),"%s%u.pem", "/tmp/key-port-", pRESTHandle->pRESTConfig->serverPort);

    }
    else if (sslDataType == SSL_DATA_TYPE_CERT)
    {
        snprintf(fileName, (MAX_PATH_LEN - 1), "%s%u.pem", "/tmp/cert-port-", pRESTHandle->pRESTConfig->serverPort);
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
        memset(pRESTHandle->pRESTConfig->pszSSLKey, '\0', MAX_PATH_LEN);
        strncpy(pRESTHandle->pRESTConfig->pszSSLKey, fileName,( MAX_PATH_LEN - 1));
        pRESTHandle->pSSLInfo->isKeySet = SSL_INFO_FROM_BUFFER_API;

    }
    else if (sslDataType == SSL_DATA_TYPE_CERT)
    {
        memset(pRESTHandle->pRESTConfig->pszSSLCertificate, '\0', MAX_PATH_LEN);
        strncpy(pRESTHandle->pRESTConfig->pszSSLCertificate, fileName,( MAX_PATH_LEN - 1));
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
    int                              ret = 0;

    if (!(pRESTHandle) || (pRESTHandle->instanceState != VMREST_INSTANCE_INITIALIZED))
    {
        dwError = REST_ENGINE_ERROR_INVALID_PARAM;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    if ((pRESTHandle->pSSLInfo->isCertSet < SSL_INFO_NO_SSL_PLAIN) || (pRESTHandle->pSSLInfo->isKeySet < SSL_INFO_NO_SSL_PLAIN))
    {
        dwError = REST_ERROR_MISSING_CONFIG;
        VMREST_LOG_ERROR(pRESTHandle, "Missing/Bad SSL key or Certificate");
    }
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmHTTPStart(
                  pRESTHandle
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    /**** delete the cert file if created by library ****/

    if (pRESTHandle->pSSLInfo->isCertSet == SSL_INFO_FROM_BUFFER_API)
    {
        if ((ret = remove(pRESTHandle->pRESTConfig->pszSSLCertificate)) == -1)
        {
            VMREST_LOG_ERROR(pRESTHandle, "remove() syscall failed for temp certificate file (), ret %d", ret);
            dwError = REST_ENGINE_FAILURE;
        }
        BAIL_ON_VMREST_ERROR(dwError);
    }
    ret = 0;

    if (pRESTHandle->pSSLInfo->isKeySet == SSL_INFO_FROM_BUFFER_API)
    {
        if ((ret = remove(pRESTHandle->pRESTConfig->pszSSLKey)) == -1)
        {
            VMREST_LOG_ERROR(pRESTHandle, "remove temp file failed ()");
            dwError = REST_ENGINE_FAILURE;
        }
        BAIL_ON_VMREST_ERROR(dwError);
    }

    pRESTHandle->instanceState = VMREST_INSTANCE_STARTED;

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

    if (!pHandler || !pRESTHandle || (pRESTHandle->instanceState != VMREST_INSTANCE_INITIALIZED))
    {
        dwError = REST_ENGINE_ERROR_INVALID_PARAM;
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

    if (!pRESTHandle || !pszEndpoint || !ppEndpoint || (pRESTHandle->instanceState == VMREST_INSTANCE_UNINITIALIZED) || (pRESTHandle->instanceState == VMREST_INSTANCE_SHUTDOWN))
    {
        dwError = REST_ENGINE_ERROR_INVALID_PARAM;
    }
    BAIL_ON_VMREST_ERROR(dwError);

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
    char const*                      pcszEndPointURI
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;

    if (!pRESTHandle || !pcszEndPointURI || (pRESTHandle->instanceState != VMREST_INSTANCE_STOPPED))
    {
        dwError = REST_ENGINE_ERROR_INVALID_PARAM;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    if (!pcszEndPointURI)
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
                      pcszEndPointURI
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
    PVMREST_HANDLE                   pRESTHandle,
    uint32_t                         waitSeconds
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;

    if (!pRESTHandle || (waitSeconds > MAX_STOP_WAIT_SECONDS) || (pRESTHandle->instanceState != VMREST_INSTANCE_STARTED))
    {  
        dwError = REST_ENGINE_ERROR_INVALID_PARAM;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    pRESTHandle->instanceState = VMREST_INSTANCE_STOPPED;

    dwError = VmHTTPStop(
                  pRESTHandle,
                  waitSeconds
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
    if (pRESTHandle || (pRESTHandle->instanceState == VMREST_INSTANCE_STOPPED))
    {
        if (pRESTHandle->pInstanceGlobal->useEndPoint == 1)
        {
            VmRestEngineShutdownEndPointRegistration(
                pRESTHandle
                );
        }

        VmHTTPShutdown(pRESTHandle);
        pRESTHandle->instanceState = VMREST_INSTANCE_SHUTDOWN;
    }
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

    if (!pRESTHandle || (pRESTHandle->instanceState != VMREST_INSTANCE_STARTED))
    {
        dwError = REST_ENGINE_ERROR_INVALID_PARAM;
    }
    BAIL_ON_VMREST_ERROR(dwError);


    dwError = VmRESTGetHttpPayload(
                  pRESTHandle,
                  pRequest,
                  pBuffer,
                  bytesRead
                  );
    BAIL_ON_VMREST_ERROR(dwError);
                  
cleanup:

    return dwError;

error:

    goto cleanup;

}

/*** GetData Zero copy API ****/
uint32_t
VmRESTGetDataZC(
    PVMREST_HANDLE                   pRESTHandle,
    PREST_REQUEST                    pRequest,
    char**                           ppBuffer,
    uint32_t*                        nBytes
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;

    if (!pRESTHandle || !pRequest || !ppBuffer  || !nBytes || (pRESTHandle->instanceState != VMREST_INSTANCE_STARTED))
    {
        VMREST_LOG_ERROR(pRESTHandle,"%s","Invalid params");
        dwError = REST_ENGINE_ERROR_INVALID_PARAM;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    if (pRequest->nPayload > 0)
    {
        *ppBuffer = pRequest->pszPayload;
        *nBytes = pRequest->nPayload;
    }
    else if (pRequest->nPayload == 0)
    {
        *ppBuffer = NULL;
        *nBytes = 0;
    }

cleanup:

    return dwError;

error:

    goto cleanup;

}

uint32_t
VmRESTSetData(
    PVMREST_HANDLE                   pRESTHandle,
    PREST_RESPONSE*                  ppResponse,
    char const*                      pcszBuffer,
    uint32_t                         dataLen,
    uint32_t*                        bytesWritten
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;

    if (!pRESTHandle || !ppResponse || !pcszBuffer || !bytesWritten || (pRESTHandle->instanceState != VMREST_INSTANCE_STARTED))
    {
        VMREST_LOG_ERROR(pRESTHandle,"%s","Invalid params");
        dwError = REST_ENGINE_ERROR_INVALID_PARAM;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTSetHttpPayload(
                  pRESTHandle,
                  ppResponse,
                  pcszBuffer,
                  dataLen,
                  bytesWritten
                  );
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:

    return dwError;

error:

    goto cleanup;

}

/**** SetData Zero Copy API ****/
uint32_t
VmRESTSetDataZC(
    PVMREST_HANDLE                   pRESTHandle,
    PREST_RESPONSE*                  ppResponse,
    char const*                      pcszBuffer,
    uint32_t                         nBytes
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;

    if (!pRESTHandle || !ppResponse || !pcszBuffer || (pRESTHandle->instanceState != VMREST_INSTANCE_STARTED))
    {
        VMREST_LOG_ERROR(pRESTHandle,"%s","Invalid params");
        dwError = REST_ENGINE_ERROR_INVALID_PARAM;
    }
    BAIL_ON_VMREST_ERROR(dwError);


    dwError = VmRESTSetHttpPayloadZeroCopy(
                  pRESTHandle,
                  ppResponse,
                  pcszBuffer,
                  nBytes
                  );
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:

    return dwError;

error:

    goto cleanup;

}

uint32_t
VmRESTSetSuccessResponse(
    PREST_REQUEST                    pRequest,
    PREST_RESPONSE*                  ppResponse
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    char*                            connection = NULL;

    if (!pRequest || !ppResponse)
    {
        dwError = REST_ENGINE_ERROR_INVALID_PARAM;
    }
    BAIL_ON_VMREST_ERROR(dwError);

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
    char const*                      pcszErrorCode,
    char const*                      pcszErrorMessage
    )
{   
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    char                             errorCode[MAX_STATUS_LEN] = {0};
    char                             errorMessage[MAX_REA_PHRASE_LEN] = {0};

    memset(errorCode, '\0', MAX_STATUS_LEN);
    memset(errorMessage, '\0', MAX_REA_PHRASE_LEN);

    /**** If error code and message is not provided, send internal server error ****/

    if (!pcszErrorCode)
    {
        strcpy(errorCode,"500");
    }
    else if ((strlen(pcszErrorCode) > 0) && (strlen(pcszErrorCode) <= MAX_STATUS_LEN))
    {
        strcpy(errorCode, pcszErrorCode);
    }

    if (!pcszErrorMessage)
    {
        strcpy(errorMessage, "Internal Server Error");
    }
    else if ((strlen(pcszErrorMessage) > 0) && (strlen(pcszErrorMessage) <= MAX_REA_PHRASE_LEN))
    {
        strcpy(errorMessage, pcszErrorMessage);
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

uint32_t
VmRESTGetConnectionInfo(
    PREST_REQUEST                    pRequest,
    char**                           ppszIpAddress,
    int*                             pPort
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    char*                            pszIpAddress = NULL;

    if (!pRequest || !ppszIpAddress || !pPort)
    {
        dwError = VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTAllocateMemory(
                  MAX_CLIENT_IP_ADDR_LEN,
                  (void **)&pszIpAddress
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    strncpy(pszIpAddress, pRequest->clientIP, (MAX_CLIENT_IP_ADDR_LEN - 1));

    *pPort = pRequest->clientPort;
    *ppszIpAddress = pszIpAddress;

cleanup:

    return dwError;

error:
    if (ppszIpAddress)
    {
        *ppszIpAddress = NULL;
    }

    goto cleanup;

}
