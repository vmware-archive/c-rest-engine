/*
 * Copyright © 2012-2015 VMware, Inc.  All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the ~@~\License~@~]); you may not
 * use this file except in compliance with the License.  You may obtain a copy
 * of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an ~@~\AS IS~@~] BASIS, without
 * warranties or conditions of any kind, EITHER EXPRESS OR IMPLIED.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

#include "includes.h"

uint32_t
VmRestEngineHandler(
    PREST_REQUEST                    pRequest,
    PREST_RESPONSE*                  ppResponse
    )
{
    char                             httpPayload[MAX_DATA_BUFFER_LEN] = {0};
    char                             httpMethod[MAX_METHOD_LEN] = {0};
    char                             httpURI[MAX_URI_LEN] = {0};
    char                             endPointURI[MAX_URI_LEN] = {0};
    char*                            ptr = NULL;
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    uint32_t                         paramsCount = 0;
    PREST_ENDPOINT                   pEndPoint = NULL;

    VMREST_LOG_DEBUG("1","Internal Handler called");

    /**** 1. Init all the funcition variables *****/

    memset(httpPayload, '\0', MAX_DATA_BUFFER_LEN);
    memset(httpMethod, '\0', MAX_METHOD_LEN);
    memset(httpURI, '\0', MAX_URI_LEN);
    memset(endPointURI, '\0', MAX_URI_LEN);

    /**** 2. Get the method name ****/

    dwError = VmRESTGetHttpMethod(
                  pRequest,
                  &ptr
                  );
    BAIL_ON_VMREST_ERROR(dwError);
    strcpy(httpMethod,ptr);
    ptr = NULL;

    VMREST_LOG_DEBUG("HTTP method %s", httpMethod);

    /**** 3. Get the URI ****/

    dwError = VmRESTGetHttpURI(
                  pRequest,
                  &ptr
                  );
    BAIL_ON_VMREST_ERROR(dwError);
    strcpy(httpURI,ptr);
    ptr = NULL;

    VMREST_LOG_DEBUG("HTTP URI %s", httpURI);

    /**** 4. Get the End point from URI ****/
    dwError = VmRestGetEndPointURIfromRequestURI(
                  httpURI,
                  endPointURI
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    VMREST_LOG_DEBUG("EndPoint URI %s", endPointURI);

    dwError = VmRestEngineGetEndPoint(
                  endPointURI,
                  &pEndPoint
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    VMREST_LOG_DEBUG("EndPoint found for URI %s",endPointURI);

    /**** 5. Get Params count ****/
  
    dwError = VmRestGetParamsCountInReqURI(
                  httpURI,
                  &paramsCount
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    VMREST_LOG_DEBUG("Params count %u", paramsCount);

    /**** 6. Parse and populate all params in request URL ****/

    dwError = VmRestParseParams(
                  httpURI,
                  paramsCount,
                  pRequest
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    VMREST_LOG_DEBUG("Params parsing done, returned code %u", dwError);
           

    /**** 7. Give App CB based on HTTP method and registered endpoint ****/

    if (strcmp(httpMethod,"GET") == 0)
    {
        if (pEndPoint && pEndPoint->pHandler && pEndPoint->pHandler->pfnHandleRead)
        {
            dwError = pEndPoint->pHandler->pfnHandleRead(pRequest, ppResponse, paramsCount);
        }
        else
        {
            VMREST_LOG_ERROR("Read on resource %s not allowed",endPointURI);
            dwError = VMREST_HTTP_INVALID_PARAMS;
        }
    }
    else if (strcmp(httpMethod,"POST") == 0)
    {
        if (pEndPoint && pEndPoint->pHandler && pEndPoint->pHandler->pfnHandleCreate)
        {
            dwError = pEndPoint->pHandler->pfnHandleCreate(pRequest, ppResponse, paramsCount);
        }
        else
        {
            VMREST_LOG_ERROR("Create on resource %s not allowed",endPointURI);
            dwError = VMREST_HTTP_INVALID_PARAMS;
        }
    }
    else if (strcmp(httpMethod,"PUT") == 0)
    {
        if (pEndPoint && pEndPoint->pHandler && pEndPoint->pHandler->pfnHandleUpdate)
        {
            dwError = pEndPoint->pHandler->pfnHandleUpdate(pRequest, ppResponse, paramsCount);
        }
        else
        {
            VMREST_LOG_ERROR("Update on resource %s not allowed",endPointURI);
            dwError = VMREST_HTTP_INVALID_PARAMS;
        }
    }
    else if (strcmp(httpMethod,"DELETE") == 0)
    {
        if (pEndPoint && pEndPoint->pHandler && pEndPoint->pHandler->pfnHandleDelete)
        {
            dwError = pEndPoint->pHandler->pfnHandleDelete(pRequest, ppResponse, paramsCount);
        }
        else
        {
            VMREST_LOG_ERROR("Delete on resource %s not allowed",endPointURI);
            dwError = VMREST_HTTP_INVALID_PARAMS;
        }
    }
    else if ((strcmp(httpMethod,"OPTIONS") == 0) || (strcmp(httpMethod,"PATCH") == 0))
    {
        /**** Add all allowed HTTP methods ****/
        if (pEndPoint && pEndPoint->pHandler && pEndPoint->pHandler->pfnHandleDelete)
        {
            dwError = pEndPoint->pHandler->pfnHandleOthers(pRequest, ppResponse, paramsCount);
        }
        else
        {
            VMREST_LOG_ERROR(" %s Not a valid HTTP method for resource %s", httpMethod,endPointURI);
            dwError = VMREST_HTTP_INVALID_PARAMS;
        }
    }
    else
    {
        VMREST_LOG_ERROR("CRUD on resource %s not allowed",endPointURI);
        dwError = VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VmRestEngineInitEndPointRegistration(
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    
    dwError = pthread_mutex_init(
                  &(gRESTEngGlobals.mutex),
                  NULL
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    pthread_mutex_lock(&(gRESTEngGlobals.mutex));
    gRESTEngGlobals.pEndPointQueue = NULL;
    gRESTEngGlobals.useEndPoint = 1;
    pthread_mutex_unlock(&(gRESTEngGlobals.mutex));

    gRESTEngGlobals.internalHandler.pfnHandleRequest = &VmRestEngineHandler;
    gRESTEngGlobals.internalHandler.pfnHandleCreate = NULL;
    gRESTEngGlobals.internalHandler.pfnHandleDelete = NULL;
    gRESTEngGlobals.internalHandler.pfnHandleUpdate = NULL;
    gRESTEngGlobals.internalHandler.pfnHandleRead = NULL;
    gRESTEngGlobals.internalHandler.pfnHandleOthers = NULL;

cleanup:
    return dwError;
error:
    goto cleanup;
}

void
VmRestEngineShutdownEndPointRegistration(
    )
{
    PREST_ENDPOINT                   temp = NULL;
    PREST_ENDPOINT                   prev = NULL;

    /**** TODO: Add check to perform this only when engine is not running ****/

    pthread_mutex_lock(&(gRESTEngGlobals.mutex));

    temp = gRESTEngGlobals.pEndPointQueue;

    while(temp != NULL)
    {
        prev = temp;
        temp = temp->next;

        VmRESTFreeEndPoint(prev);
    }
    gRESTEngGlobals.pEndPointQueue = NULL;
    gRESTEngGlobals.useEndPoint = 0; 
    pthread_mutex_unlock(&(gRESTEngGlobals.mutex));

    pthread_mutex_destroy(
        &(gRESTEngGlobals.mutex)
        );
}

uint32_t
VmRestEngineAddEndpoint(
    char*                            pEndPointURI,
    PREST_PROCESSOR                  pHandler
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    size_t                           endPointURILen = 0;
    PREST_ENDPOINT                   pEndPoint = NULL;
    PREST_ENDPOINT                   temp = NULL;

    /**** TODO: Add check to perform this only when engine is not running ****/

    if (!pEndPointURI || !pHandler)
    {
        VMREST_LOG_ERROR("Invalid params");
        dwError =  VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    endPointURILen = strlen(pEndPointURI);

    /**** Allocate and Assign Endpoint ****/
    dwError = VmRESTAllocateEndPoint(
                  &pEndPoint
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    if ((endPointURILen > 0) && (endPointURILen < MAX_URI_LEN))
    {
        strcpy(pEndPoint->pszEndPointURI,pEndPointURI);
        pEndPoint->pHandler->pfnHandleRequest = NULL;
        pEndPoint->pHandler->pfnHandleCreate = pHandler->pfnHandleCreate;
        pEndPoint->pHandler->pfnHandleDelete = pHandler->pfnHandleDelete;
        pEndPoint->pHandler->pfnHandleUpdate = pHandler->pfnHandleUpdate;
        pEndPoint->pHandler->pfnHandleRead = pHandler->pfnHandleRead;
        pEndPoint->pHandler->pfnHandleOthers = pHandler->pfnHandleOthers;
        pEndPoint->next = NULL;
    }

    /**** Add to list of endpoints ****/
    pthread_mutex_lock(&(gRESTEngGlobals.mutex));

    if (gRESTEngGlobals.pEndPointQueue == NULL)
    {
        gRESTEngGlobals.pEndPointQueue = pEndPoint;
    }
    else
    {
        temp = gRESTEngGlobals.pEndPointQueue;
        while(temp->next != NULL)
        { 
            temp = temp->next;
        }
        temp->next = pEndPoint;
    }
    pthread_mutex_unlock(&(gRESTEngGlobals.mutex));
    
cleanup:
    return dwError;
error:
    /**** TODO: Free the allocated memory ****/
    goto cleanup;
}

uint32_t
VmRestEngineRemoveEndpoint(
    char*                            pEndPointURI
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    PREST_ENDPOINT                   temp = NULL;
    PREST_ENDPOINT                   prev = NULL;

    /**** TODO: Add check to perform this only when engine is not running ****/

    if (!pEndPointURI)
    {
        VMREST_LOG_ERROR("Invalid params");
        dwError =  VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    /**** Remove from list of endpoints ****/
    pthread_mutex_lock(&(gRESTEngGlobals.mutex));

    temp = gRESTEngGlobals.pEndPointQueue;
    prev = temp;

    if ((temp != NULL) && (temp->pszEndPointURI != NULL) && (strcmp(temp->pszEndPointURI,pEndPointURI) == 0))
    {
        gRESTEngGlobals.pEndPointQueue = temp->next;
    }
    else
    {
       while(temp != NULL && (temp->pszEndPointURI != NULL) && (strcmp(temp->pszEndPointURI,pEndPointURI) != 0))
       {
           prev = temp;
           temp = temp->next;
       }
       if (temp == NULL)
       {
           VMREST_LOG_ERROR("Requested endpoint %s not registered", pEndPointURI);
       }
       else
       {
           prev->next = temp->next;
       }
    }
    pthread_mutex_unlock(&(gRESTEngGlobals.mutex));

    if (temp)
    {
        VmRESTFreeEndPoint(temp);
    }
cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VmRestEngineGetEndPoint(
    char*                            pEndPointURI,
    PREST_ENDPOINT*                  ppEndPoint
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;

    PREST_ENDPOINT                   temp = NULL;

    if (!pEndPointURI)
    {
        VMREST_LOG_ERROR("Invalid params");
        dwError =  VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    *ppEndPoint = NULL;
    temp = gRESTEngGlobals.pEndPointQueue;

    while (temp != NULL)
    {
        if ((temp->pszEndPointURI != NULL) && (strcmp(temp->pszEndPointURI,pEndPointURI) == 0))
        {
            *ppEndPoint = temp;
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
VmRestGetEndPointURIfromRequestURI(
    char*                            pRequestURI,
    char*                            endPointURI
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    char*                            foundCharacter = NULL;
    uint64_t                         copyBytes = 0;
    
    if (!pRequestURI || !endPointURI)
    {
        VMREST_LOG_ERROR("Request URI is NULL");
        dwError =  VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    foundCharacter = strchr(pRequestURI, '?');
    if (foundCharacter != NULL)
    {
        copyBytes = foundCharacter - pRequestURI;
        strncpy(endPointURI,pRequestURI, copyBytes);
        *(endPointURI + copyBytes) = '\0';
    }
    else
    {
        strcpy(endPointURI,pRequestURI);
    }

cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VmRestGetParamsCountInReqURI(
    char*                            pRequestURI,
    uint32_t*                        paramCount
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    uint32_t                         ampCnt = 0;
    uint32_t                         eqCnt = 0;
    char*                            temp = NULL;

    if (!pRequestURI || !paramCount)
    {
        VMREST_LOG_ERROR("Request URI or result pointer is NULL");
        dwError =  VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    *paramCount = 0;
    temp = pRequestURI;

    while(*temp != '\0')
    {
        if (*temp == '&')
        {
            ampCnt++;
        }
        if (*temp == '=')
        {
            eqCnt++;
        }
        temp++;
    }

    /**** if ampCnt is not equal eqCnt -1, then its a bad URI ****/
    if ((eqCnt > 0) && (ampCnt != (eqCnt -1)))
    {
        dwError = VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    *paramCount = eqCnt;

cleanup:
    return dwError;
error:
    if (paramCount)
    {
        *paramCount = 0;
    }
    goto cleanup;
}

uint32_t
VmRestParseParams(
    char*                            pRequestURI,
    uint32_t                         paramsCount,
    PREST_REQUEST                    pRequest
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    char*                            key = NULL;
    char*                            value = NULL;
    char*                            res = NULL;
    uint32_t                         i = 0;
    uint64_t                         diff = 0;
    

    
    if (!pRequestURI || !pRequest)
    {
        VMREST_LOG_ERROR("Request URI or Request is NULL");
        dwError =  VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    if (paramsCount > MAX_URL_PARAMS_ARR_SIZE)
    {
        VMREST_LOG_ERROR("More than allowed limit of 10 params found in URL");
        dwError =  VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    for (i=0; i < MAX_URL_PARAMS_ARR_SIZE; i++)
    {
        memset(pRequest->paramArray[i].key, '\0', MAX_KEY_VAL_PARAM_LEN);
        memset(pRequest->paramArray[i].value, '\0', MAX_KEY_VAL_PARAM_LEN);
    }

    i = 0;

    key = strchr(pRequestURI, '?');

    while (i < paramsCount)
    {
        if (key)
        {
            value = strchr(key,'=');
            if (value)
            {
                res = pRequest->paramArray[i].key;
                diff = value - key - 1;
                strncpy(res, (key + 1), diff);
                *(res + diff) = '\0';

                key = strchr(value, '&');
                res= pRequest->paramArray[i].value;
                if (key)
                {
                    diff = key - value - 1;
                    strncpy(res, (value + 1), diff);
                    *(res + diff) = '\0';
                }
                else
                {
                    strcpy(res, (value +1));
                }    
            }
            else
            {
                VMREST_LOG_ERROR("Param %u has no data",i);
                dwError = VMREST_HTTP_INVALID_PARAMS;
            }
        }
        else
        {
             VMREST_LOG_ERROR("URI  has no Params or ? character");
             dwError = VMREST_HTTP_INVALID_PARAMS;
        }
        BAIL_ON_VMREST_ERROR(dwError);
        i++;
    }

cleanup:
    return dwError;
error:
    for (i=0; i < MAX_URL_PARAMS_ARR_SIZE; i++)
    {
        memset(pRequest->paramArray[i].key, '\0', MAX_KEY_VAL_PARAM_LEN);
        memset(pRequest->paramArray[i].value, '\0', MAX_KEY_VAL_PARAM_LEN);
    }
    goto cleanup;
}

/**** Exposed API to manupulate over params present in URI ****/

uint32_t
VmRESTGetParamsByIndex(
    PREST_REQUEST                    pRequest,
    uint32_t                         paramsCount,
    uint32_t                         paramIndex,
    char**                           pszKey,
    char**                           pszValue
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    uint32_t                         index = 0;

    if (paramIndex > paramsCount || paramIndex == 0) 
    {
        VMREST_LOG_ERROR("Param Index %u is wrong", paramIndex);
        dwError = VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    if (!pszKey || !pszValue)
    {
        VMREST_LOG_ERROR("Result variables are NULL");
        dwError = VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    index = paramIndex - 1; 
    
    if ((pRequest->paramArray[index].key) && (strlen(pRequest->paramArray[index].key) > 0))
    {
        *pszKey = pRequest->paramArray[index].key;
    }
    else
    {
        VMREST_LOG_ERROR("Key Not Found for index %u", paramIndex);
        dwError = VMREST_HTTP_INVALID_PARAMS;
    }

    if ((pRequest->paramArray[index].value) && (strlen(pRequest->paramArray[index].value) > 0))
    {
        *pszValue = pRequest->paramArray[index].value;
    }
    else
    {
        VMREST_LOG_DEBUG("WARNING: Value Not Found for index %u", paramIndex);
        *pszValue = NULL;
    }
    
cleanup:
    return dwError;
error:
    if (pszKey)
    {
        *pszKey = NULL;
    }
    if (pszValue)
    {
        *pszValue = NULL;
    }
    goto cleanup;
}
