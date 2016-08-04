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

uint32_t
VmRESTInit(
    PREST_CONF                       pConfig,
    char*                            file
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

    if (pHandler == NULL)
    {
        dwError = REST_ENGINE_INVALID_REST_PROCESSER;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    if (pszEndpoint == NULL )
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
    if (gpHttpHandler == NULL)
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

    dwError = VmRestEngineGetEndPoint(
                  (char*)pszEndpoint,
                  ppEndpoint
                  );
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VmRESTUnRegisterHandler(
    char*                            pzEndPointURI
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;

    if (pzEndPointURI == NULL)
    {
        dwError = VmHTTPUnRegisterHandler(
                      );
    }
    else
    {
        dwError = VmRestEngineRemoveEndpoint(
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
    char*                            response,
    uint32_t*                        done
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;

    dwError = VmRESTGetHttpPayload(
                  pRequest,
                  response,
                  done
                  );
    BAIL_ON_VMREST_ERROR(dwError);
                  
cleanup:
    return dwError;
error:
    goto cleanup;

}

uint32_t
VmRESTSetData(
    PREST_RESPONSE*                  ppResponse,
    char*                            buffer,
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
    char*                            pErrorCode,
    char*                            pErrorMessage
    )
{   
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    char                             errorCode[MAX_STATUS_LEN] = {0};
    char                             errorMessage[MAX_REA_PHRASE_LEN] = {0};

    memset(errorCode, '\0', MAX_STATUS_LEN);
    memset(errorMessage, '\0', MAX_REA_PHRASE_LEN);

    /**** If error code and message is not provided, send internal server error ****/

    if (pErrorCode == NULL)
    {
        strcpy(errorCode,"500");
    }
    else if ((strlen(pErrorCode) > 0) && (strlen(pErrorCode) <= MAX_STATUS_LEN))
    {
        strcpy(errorCode, pErrorCode);
    }

    if (pErrorMessage == NULL)
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
        VMREST_LOG_DEBUG("ERROR: Data Length Invalid");
        dwError = VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:
    return dwError;
error:
    goto cleanup;
}
