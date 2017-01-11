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
    memset(httpMethod, '\0', MAX_METHOD_LEN);
    strncpy(httpMethod,ptr, (MAX_METHOD_LEN - 1));
    ptr = NULL;

    VMREST_LOG_DEBUG("HTTP method %s", httpMethod);

    /**** 3. Get the URI ****/

    dwError = VmRESTGetHttpURI(
                  pRequest,
                  &ptr
                  );
    BAIL_ON_VMREST_ERROR(dwError);
    memset(httpURI, '\0',MAX_URI_LEN);
    strncpy(httpURI,ptr,(MAX_URI_LEN - 1));
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
        if (pEndPoint && pEndPoint->pHandler && pEndPoint->pHandler->pfnHandleOthers)
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

    /**** If Endpoint already exists - return****/
    dwError = VmRestEngineGetEndPoint(
                  pEndPointURI,
                  &temp
                  );
    if(dwError != NOT_FOUND)
    {
        dwError = REST_ENGINE_ENDPOINT_EXISTS;
    }
    else
    {
        dwError = 0;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    temp = NULL;

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
    else
    {
        dwError = REST_ENGINE_ENDPOINT_BAD_URI;
        BAIL_ON_VMREST_ERROR(dwError);
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
    if (pEndPoint)
    {
        VmRESTFreeEndPoint(pEndPoint);
        pEndPoint = NULL;
    }
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
    uint32_t                         found = 0;

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
        if (temp->pszEndPointURI != NULL)
        {
            found = VmRESTMatchEndPointURI(
                        temp->pszEndPointURI,
                        pEndPointURI
                        );
            if(found == 1)
            {
                *ppEndPoint = temp;
                break;
            }
        }
        temp = temp->next;
    }

cleanup:
    if (temp == NULL)
    {
        dwError = NOT_FOUND;
    }
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
                    strncpy(res, (value +1), (MAX_KEY_VAL_PARAM_LEN -1));
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

uint32_t
VmRESTMatchEndPointURI(
    char*                            pattern,
    char*                            pEndPointURI
    )
{
    /**** return 0 = fail, return 1 = success ****/

    if (*pattern == '\0' && *pEndPointURI == '\0')
    {
        return 1;
    }

    if (*pattern == '*' && *(pattern+1) != '\0' && *pEndPointURI == '\0')
    {
        return 0;
    }

    if (*pattern == *pEndPointURI)
    {
        return VmRESTMatchEndPointURI(pattern+1, pEndPointURI+1);
    }

    if (*pattern == '*')
    {
        return VmRESTMatchEndPointURI(pattern+1, pEndPointURI) || VmRESTMatchEndPointURI(pattern, pEndPointURI+1);
    }
    return 0;
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
    
    if ((pRequest != NULL) && (strlen(pRequest->paramArray[index].key) > 0))
    {
        *pszKey = pRequest->paramArray[index].key;
    }
    else
    {
        VMREST_LOG_ERROR("Key Not Found for index %u", paramIndex);
        dwError = VMREST_HTTP_INVALID_PARAMS;
    }

    if ((pRequest != NULL) && (strlen(pRequest->paramArray[index].value) > 0))
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

uint32_t
VmRESTGetWildCardCount(
    PREST_REQUEST                    pRequest,
    uint32_t*                        wildCardCount
    )
{
    char                             httpURI[MAX_URI_LEN] = {0};
    char                             endPointURI[MAX_URI_LEN] = {0};
    char*                            ptr = NULL;
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    uint32_t                         count = 0;
    PREST_ENDPOINT                   pEndPoint = NULL;

    if (pRequest == NULL || wildCardCount == NULL)
    {
        VMREST_LOG_ERROR("Invalid Params");
        dwError = VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);
    *wildCardCount = 0;

    memset(httpURI, '\0', MAX_URI_LEN);
    memset(endPointURI, '\0', MAX_URI_LEN);

    dwError = VmRESTGetHttpURI(
                  pRequest,
                  &ptr
                  );
    BAIL_ON_VMREST_ERROR(dwError);
    strncpy(httpURI,ptr,(MAX_URI_LEN - 1));
    ptr = NULL;
    
    dwError = VmRestGetEndPointURIfromRequestURI(
                  httpURI,
                  endPointURI
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRestEngineGetEndPoint(
                  endPointURI,
                  &pEndPoint
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    ptr = pEndPoint->pszEndPointURI;

    while(*ptr != '\0')
    {
        if(*ptr == '*')
        {
            count++;
        }
        ptr++;
    }

    *wildCardCount = count;

cleanup:
    return dwError;
error:
    goto cleanup;
}


uint32_t
VmRESTGetWildCardByIndex(
    PREST_REQUEST                    pRequest,
    uint32_t                         index,
    char**                           ppszWildCard
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    uint32_t                         count = 0;
    uint32_t                         preSlashIndex = 0;
    char                             httpURI[MAX_URI_LEN] = {0};
    char                             endPointURI[MAX_URI_LEN] = {0};
    char*                            ptr = NULL;
    char*                            pszWildCard = NULL;
    PREST_ENDPOINT                   pEndPoint = NULL;

    if (pRequest == NULL)
    {
        VMREST_LOG_ERROR("Invalid Params");
        dwError = VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTGetWildCardCount(
                  pRequest,
                  &count
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    if (index > count)
    {
        VMREST_LOG_ERROR("Invalid index count %u index %u", count, index);
        dwError = VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTAllocateMemory(
                  MAX_URI_LEN,
                  (void **)&pszWildCard
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    memset(httpURI, '\0', MAX_URI_LEN);
    memset(endPointURI, '\0', MAX_URI_LEN);

    dwError = VmRESTGetHttpURI(
                  pRequest,
                  &ptr
                  );
    BAIL_ON_VMREST_ERROR(dwError);
    strncpy(httpURI,ptr,(MAX_URI_LEN - 1));
    ptr = NULL;

    dwError = VmRestGetEndPointURIfromRequestURI(
                  httpURI,
                  endPointURI
                  );
    BAIL_ON_VMREST_ERROR(dwError);
    ptr = endPointURI;

    dwError = VmRestEngineGetEndPoint(
                  endPointURI,
                  &pEndPoint
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTGetPreSlashIndex(
              pEndPoint->pszEndPointURI,
              index,
              &preSlashIndex
              );
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTCopyWCStringByIndex(
                  endPointURI,
                  pszWildCard,
                  index,
                  count,
                  preSlashIndex
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    *ppszWildCard = pszWildCard;


cleanup:
    return dwError;
error:
    if (pszWildCard)
    {
        VmRESTFreeMemory(pszWildCard);
        pszWildCard = NULL;
    }
    goto cleanup;
}

uint32_t
VmRESTGetPreSlashIndex(
    char*                            patternURI,
    uint32_t                         wildCardIndex,
    uint32_t*                        preSlashIndex
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    char*                            ptr = NULL;
    uint32_t                         slashCnt = 0;
    uint32_t                         wcCharCnt = 0;

    if (patternURI == NULL || preSlashIndex == NULL)
    {
        VMREST_LOG_ERROR("Invalid Params");
        dwError = VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    ptr = patternURI;
    *preSlashIndex = 0;

    while(*ptr != '\0')
    {
        if (*ptr == '*')
        {
            wcCharCnt++;
        }
        if (wcCharCnt == wildCardIndex)
        {
            break;
        }
        if (*ptr == '/')
        {
            slashCnt++;
        }
        ptr++;
    }

    *preSlashIndex = slashCnt;
    
cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VmRESTCopyWCStringByIndex(
    char*                            requestEndPointURI,
    char*                            des,
    uint32_t                         wcIndex,
    uint32_t                         totalWC,
    uint32_t                         preSlashIndex
    )
{
     uint32_t                         dwError = REST_ENGINE_SUCCESS;
     char*                            ptr = NULL;
     uint32_t                         slashCnt = 0;
     uint32_t                         copyBytes = 0;
     char*                            lastChar = NULL;

    if (requestEndPointURI == NULL || des == NULL)
    {
        VMREST_LOG_ERROR("Invalid Params");
        dwError = VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    ptr = requestEndPointURI;

    while(*ptr != '\0')
    {
        if (*ptr == '/')
        {
            slashCnt++;
        }
        ptr++;
        if(slashCnt == preSlashIndex)
        {
            break;
        }
    }

    if (*ptr != '\0')
    {
        lastChar = strchr(ptr,'/');

        if (lastChar != NULL)
        {
            copyBytes = lastChar - ptr; 
            strncpy(des, ptr, copyBytes);
            *(des + copyBytes) = '\0';
        }
        else if (wcIndex == totalWC)
        {
            lastChar = strchr(ptr, '\0');
            if (lastChar != NULL)
            {
                copyBytes = lastChar - ptr;
                strncpy(des, ptr, copyBytes);
                *(des + copyBytes) = '\0';
            }
            else
            {
                dwError = BAD_REQUEST;
            }
        }
        else
        {
            dwError = BAD_REQUEST;
        }
    }
    else
    {
        /**** URL end with '/' - nothing to copy ****/
    }
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:
    return dwError;
error:
    goto cleanup;

}
    
