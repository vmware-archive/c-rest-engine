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
int  vmrest_syslog_level;

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
    char const*                      file
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;

    dwError = VmHTTPInit(
                  pConfig,
                  file
                  );
    BAIL_ON_VMREST_ERROR(dwError);

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
    void
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;

    dwError = VmHTTPStart(
                  );
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:
    return dwError;
error:
    goto cleanup;

}

uint32_t
VmRESTRegisterHandler(
    char const*                      pszEndpoint,
    PREST_PROCESSOR                  pHandler,
    PREST_ENDPOINT*                  ppEndpoint
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    PREST_PROCESSOR                  pzHandler = NULL;

    if (!pHandler)
    {
        dwError = REST_ENGINE_INVALID_REST_PROCESSER;
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
        if (gRESTEngGlobals.useEndPoint == 0)
        {
            dwError = VmRestEngineInitEndPointRegistration(
                      );
            BAIL_ON_VMREST_ERROR(dwError);
        }
        dwError = VmRestEngineAddEndpoint(
                      (char  *)pszEndpoint,
                      pHandler
                      );
        BAIL_ON_VMREST_ERROR(dwError);
        pzHandler = &(gRESTEngGlobals.internalHandler);

    }
    if (!gpHttpHandler)
    {
        dwError = VmHTTPRegisterHandler(
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
    char const*                      pszEndpoint,
    PREST_ENDPOINT*                  ppEndpoint
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    PREST_ENDPOINT                   temp = NULL;
    PREST_ENDPOINT                   pEndPoint = NULL;

    dwError = VmRestEngineGetEndPoint(
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
    char const*                      pzEndPointURI
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;

    if (!pzEndPointURI)
    {
        dwError = VmHTTPUnRegisterHandler(
                      );
    }
    else
    {
       // dwError = VmRestEngineRemoveEndpoint(
         //             pzEndPointURI
           //           );
    }
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VmRESTStop(
    void
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;

    if (gRESTEngGlobals.useEndPoint == 1)
    {
        VmRestEngineShutdownEndPointRegistration(
            );
    }
   
    dwError = VmHTTPStop(
                  );
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:
    return dwError;
error:
    goto cleanup;
}

void
VmRESTShutdown(
    void
    )
{
    VmHTTPShutdown();
}

uint32_t
VmRESTGetData(
    PREST_REQUEST                    pRequest,
    char**                           ppData,
    uint32_t*                        done
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    char                             result[MAX_DATA_BUFFER_LEN] = {0};
    char*                            pData = NULL;

    if (ppData == NULL)
    {
        VMREST_LOG_ERROR("Invalid result pointer");
        dwError = VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    memset(result, '\0', MAX_DATA_BUFFER_LEN);
    dwError = VmRESTGetHttpPayload(
                  pRequest,
                  result,
                  done
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTAllocateMemory(
                  MAX_DATA_BUFFER_LEN,
                  (void**)&pData
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    memset(pData, '\0', MAX_DATA_BUFFER_LEN);
    memcpy(pData,result,MAX_DATA_BUFFER_LEN);

    *ppData = pData;
                  
cleanup:
    return dwError;
error:
    if (pData != NULL)
    {
        VmRESTFreeMemory(pData);
        pData = NULL;
    }
    if (ppData != NULL)
    {
        *ppData = NULL;
    }
    goto cleanup;

}

uint32_t
VmRESTSetData(
    PREST_RESPONSE*                  ppResponse,
    char const*                      buffer,
    uint32_t                         dataLen,
    uint32_t*                        done
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;

    dwError = VmRESTSetHttpPayload(
                  ppResponse,
                  buffer,
                  dataLen,
                  done
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
        VMREST_LOG_DEBUG("%s","ERROR: Data Length Invalid");
        dwError = VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:
    return dwError;
error:
    goto cleanup;
}
