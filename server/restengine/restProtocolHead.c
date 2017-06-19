/* C-REST-Engine
*
* Copyright (c) 2017 VMware, Inc. All Rights Reserved. 
*
* This product is licensed to you under the Apache 2.0 license (the "License").
* You may not use this product except in compliance with the Apache 2.0 License.  
*
* This product may include a number of subcomponents with separate copyright 
* notices and license terms. Your use of these subcomponents is subject to the 
* terms and conditions of the subcomponent's license, as noted in the LICENSE file. 
*
*/

#include "includes.h"

uint32_t
VmRestEngineHandler(
    PVMREST_HANDLE                   pRESTHandle,
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

    VMREST_LOG_DEBUG(pRESTHandle,"%s","Internal Handler called");

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
    if (ptr != NULL)
    {
        VmRESTFreeMemory(ptr);
        ptr = NULL;
    }
    VMREST_LOG_DEBUG(pRESTHandle,"HTTP method %s", httpMethod);

    /**** 3. Get the URI ****/

    dwError = VmRESTGetHttpURI(
                  pRequest,
                  &ptr
                  );
    BAIL_ON_VMREST_ERROR(dwError);
    memset(httpURI, '\0',MAX_URI_LEN);
    strncpy(httpURI,ptr,(MAX_URI_LEN - 1));
    if (ptr != NULL)
    {
        VmRESTFreeMemory(ptr);
        ptr = NULL;
    }

    VMREST_LOG_DEBUG(pRESTHandle,"HTTP URI %s", httpURI);

    /**** 4. Get the End point from URI ****/
    dwError = VmRestGetEndPointURIfromRequestURI(
                  httpURI,
                  &ptr
                  );
    BAIL_ON_VMREST_ERROR(dwError);
    memset(endPointURI, '\0',MAX_URI_LEN);
    strncpy(endPointURI,ptr,(MAX_URI_LEN - 1));
    if (ptr != NULL)
    {
        free(ptr);
        ptr = NULL;
    }

    VMREST_LOG_DEBUG(pRESTHandle,"EndPoint URI %s", endPointURI);

    dwError = VmRestEngineGetEndPoint(
                  pRESTHandle,
                  endPointURI,
                  &pEndPoint
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    VMREST_LOG_DEBUG(pRESTHandle,"EndPoint found for URI %s",endPointURI);

    /**** 5. Get Params count ****/

    dwError = VmRestGetParamsCountInReqURI(
                  pRequest->requestLine->uri,
                  &paramsCount
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    VMREST_LOG_DEBUG(pRESTHandle,"Params count %u", paramsCount);

    /**** 6. Parse and populate all params in request URL ****/

    if (paramsCount > 0)
    {
        dwError = VmRestParseParams(
                      pRESTHandle,
                      pRequest->requestLine->uri,
                      paramsCount,
                      pRequest
                      );
        BAIL_ON_VMREST_ERROR(dwError);
        VMREST_LOG_DEBUG(pRESTHandle,"Params parsing done, returned code %u", dwError);
    }

    /**** 7. Give App CB based on HTTP method and registered endpoint ****/

    if (strcmp(httpMethod,"GET") == 0)
    {
        if (pEndPoint && pEndPoint->pHandler && pEndPoint->pHandler->pfnHandleRead)
        {
            dwError = pEndPoint->pHandler->pfnHandleRead(pRESTHandle, pRequest, ppResponse, paramsCount);
        }
        else
        {
            VMREST_LOG_ERROR(pRESTHandle,"Read on resource %s not allowed",endPointURI);
            dwError = VMREST_HTTP_INVALID_PARAMS;
        }
    }
    else if (strcmp(httpMethod,"POST") == 0)
    {
        if (pEndPoint && pEndPoint->pHandler && pEndPoint->pHandler->pfnHandleCreate)
        {
            dwError = pEndPoint->pHandler->pfnHandleCreate(pRESTHandle, pRequest, ppResponse, paramsCount);
        }
        else
        {
            VMREST_LOG_ERROR(pRESTHandle,"Create on resource %s not allowed",endPointURI);
            dwError = VMREST_HTTP_INVALID_PARAMS;
        }
    }
    else if (strcmp(httpMethod,"PUT") == 0)
    {
        if (pEndPoint && pEndPoint->pHandler && pEndPoint->pHandler->pfnHandleUpdate)
        {
            dwError = pEndPoint->pHandler->pfnHandleUpdate(pRESTHandle, pRequest, ppResponse, paramsCount);
        }
        else
        {
            VMREST_LOG_ERROR(pRESTHandle,"Update on resource %s not allowed",endPointURI);
            dwError = VMREST_HTTP_INVALID_PARAMS;
        }
    }
    else if (strcmp(httpMethod,"DELETE") == 0)
    {
        if (pEndPoint && pEndPoint->pHandler && pEndPoint->pHandler->pfnHandleDelete)
        {
            dwError = pEndPoint->pHandler->pfnHandleDelete(pRESTHandle, pRequest, ppResponse, paramsCount);
        }
        else
        {
            VMREST_LOG_ERROR(pRESTHandle,"Delete on resource %s not allowed",endPointURI);
            dwError = VMREST_HTTP_INVALID_PARAMS;
        }
    }
    else if ((strcmp(httpMethod,"OPTIONS") == 0) || (strcmp(httpMethod,"PATCH") == 0))
    {
        /**** Add all allowed HTTP methods ****/
        if (pEndPoint && pEndPoint->pHandler && pEndPoint->pHandler->pfnHandleOthers)
        {
            dwError = pEndPoint->pHandler->pfnHandleOthers(pRESTHandle, pRequest, ppResponse, paramsCount);
        }
        else
        {
            VMREST_LOG_ERROR(pRESTHandle," %s Not a valid HTTP method for resource %s", httpMethod,endPointURI);
            dwError = VMREST_HTTP_INVALID_PARAMS;
        }
    }
    else
    {
        VMREST_LOG_ERROR(pRESTHandle,"CRUD on resource %s not allowed",endPointURI);
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
    PVMREST_HANDLE                   pRESTHandle
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;

    if (!pRESTHandle)
    {
        VMREST_LOG_DEBUG(pRESTHandle,"%s","Invalid REST handler");
        dwError = REST_ERROR_INVALID_HANDLER;
    }
    BAIL_ON_VMREST_ERROR(dwError);    

    if(pRESTHandle->pInstanceGlobal)
    {

        dwError = pthread_mutex_init(
                      &(pRESTHandle->pInstanceGlobal->mutex),
                      NULL
                      );
        BAIL_ON_VMREST_ERROR(dwError);

        pthread_mutex_lock(&(pRESTHandle->pInstanceGlobal->mutex));
        pRESTHandle->pInstanceGlobal->pEndPointQueue = NULL;
        pRESTHandle->pInstanceGlobal->useEndPoint = 1;
        pthread_mutex_unlock(&(pRESTHandle->pInstanceGlobal->mutex));

        pRESTHandle->pInstanceGlobal->internalHandler.pfnHandleRequest = &VmRestEngineHandler;
        pRESTHandle->pInstanceGlobal->internalHandler.pfnHandleCreate = NULL;
        pRESTHandle->pInstanceGlobal->internalHandler.pfnHandleDelete = NULL;
        pRESTHandle->pInstanceGlobal->internalHandler.pfnHandleUpdate = NULL;
        pRESTHandle->pInstanceGlobal->internalHandler.pfnHandleRead = NULL;
        pRESTHandle->pInstanceGlobal->internalHandler.pfnHandleOthers = NULL;
    }

cleanup:
    return dwError;
error:
    goto cleanup;
}

void
VmRestEngineShutdownEndPointRegistration(
    PVMREST_HANDLE                   pRESTHandle
    )
{
    PREST_ENDPOINT                   temp = NULL;
    PREST_ENDPOINT                   prev = NULL;

    if (!pRESTHandle)
    {
        VMREST_LOG_ERROR(pRESTHandle,"%s","Invalid REST handler");
        return;
    }

    /**** TODO: Add check to perform this only when engine is not running ****/

    pthread_mutex_lock(&(pRESTHandle->pInstanceGlobal->mutex));

    temp = pRESTHandle->pInstanceGlobal->pEndPointQueue;

    while(temp != NULL)
    {
        prev = temp;
        temp = temp->next;

        VmRESTFreeEndPoint(prev);
    }
    pRESTHandle->pInstanceGlobal->pEndPointQueue = NULL;
    pRESTHandle->pInstanceGlobal->useEndPoint = 0; 
    pthread_mutex_unlock(&(pRESTHandle->pInstanceGlobal->mutex));

    pthread_mutex_destroy(
        &(pRESTHandle->pInstanceGlobal->mutex)
        );
}

uint32_t
VmRestEngineAddEndpoint(
    PVMREST_HANDLE                   pRESTHandle,
    char*                            pEndPointURI,
    PREST_PROCESSOR                  pHandler
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    size_t                           endPointURILen = 0;
    PREST_ENDPOINT                   pEndPoint = NULL;
    PREST_ENDPOINT                   temp = NULL;
    char*                            hasSpace = NULL;

    /**** TODO: Add check to perform this only when engine is not running ****/

    if (!pEndPointURI || !pHandler || !pRESTHandle)
    {
        VMREST_LOG_ERROR(pRESTHandle,"%s","Invalid params");
        dwError =  VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    endPointURILen = strlen(pEndPointURI);

    /**** check for space in URL ****/
    hasSpace = strchr(pEndPointURI, ' ');
    if (hasSpace != NULL)
    {
        VMREST_LOG_ERROR(pRESTHandle,"%s","Space found in URL - Not Valid");
        dwError =  VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    /**** If Endpoint already exists - return****/
    dwError = VmRestEngineGetEndPoint(
                  pRESTHandle,
                  pEndPointURI,
                  &temp
                  );
    if(dwError != NOT_FOUND)
    {
        dwError = REST_ERROR_ENDPOINT_EXISTS;
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
        dwError = REST_ERROR_ENDPOINT_BAD_URI;
        BAIL_ON_VMREST_ERROR(dwError);
    }

    /**** Add to list of endpoints ****/
    pthread_mutex_lock(&(pRESTHandle->pInstanceGlobal->mutex));

    if (pRESTHandle->pInstanceGlobal->pEndPointQueue == NULL)
    {
        pRESTHandle->pInstanceGlobal->pEndPointQueue = pEndPoint;
    }
    else
    {
        temp = pRESTHandle->pInstanceGlobal->pEndPointQueue;
        while(temp->next != NULL)
        { 
            temp = temp->next;
        }
        temp->next = pEndPoint;
    }
    pthread_mutex_unlock(&(pRESTHandle->pInstanceGlobal->mutex));
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
    PVMREST_HANDLE                   pRESTHandle,
    char const *                     pEndPointURI
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    PREST_ENDPOINT                   temp = NULL;
    PREST_ENDPOINT                   prev = NULL;

    /**** TODO: Add check to perform this only when engine is not running ****/

    if (!pEndPointURI || !pRESTHandle)
    {
        VMREST_LOG_ERROR(pRESTHandle,"%s","Invalid params");
        dwError =  VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    /**** Remove from list of endpoints ****/
    pthread_mutex_lock(&(pRESTHandle->pInstanceGlobal->mutex));

    temp = pRESTHandle->pInstanceGlobal->pEndPointQueue;
    prev = temp;

    if ((temp != NULL) && (temp->pszEndPointURI != NULL) && (strcmp(temp->pszEndPointURI,pEndPointURI) == 0))
    {
        pRESTHandle->pInstanceGlobal->pEndPointQueue = temp->next;
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
           VMREST_LOG_ERROR(pRESTHandle,"%s","Requested endpoint %s not registered");
       }
       else
       {
           prev->next = temp->next;
       }
    }
    pthread_mutex_unlock(&(pRESTHandle->pInstanceGlobal->mutex));

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
    PVMREST_HANDLE                   pRESTHandle,
    char*                            pEndPointURI,
    PREST_ENDPOINT*                  ppEndPoint
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    uint32_t                         found = 0;

    PREST_ENDPOINT                   temp = NULL;

    if (!pEndPointURI || !pRESTHandle)
    {
        VMREST_LOG_ERROR(pRESTHandle,"%s","Invalid params");
        dwError =  VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    *ppEndPoint = NULL;
    temp = pRESTHandle->pInstanceGlobal->pEndPointQueue;

    while (temp != NULL)
    {
        if (temp->pszEndPointURI != NULL)
        {
            found = VmRESTMatchEndPointURI(
                        temp->pszEndPointURI,
                        pEndPointURI
                        );
            if (found == 0)
            {
                found = VmRESTMatchEndPointURI(
                            pEndPointURI,
                            temp->pszEndPointURI
                            );
            }

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
VmRestGetParamsCountInReqURI(
    char*                            pRequestURI,
    uint32_t*                        paramCount
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    uint32_t                         ampCnt = 0;
    uint32_t                         eqCnt = 0;
    char*                            temp = NULL;
    char*                            hasSpace = NULL;


    if (!pRequestURI || !paramCount)
    {
        dwError =  VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    hasSpace = strchr(pRequestURI, ' ');
    if (hasSpace != NULL || strlen(pRequestURI) == 0 || strlen(pRequestURI) > MAX_URI_LEN)
    {
        dwError = BAD_REQUEST;
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
    PVMREST_HANDLE                   pRESTHandle,
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
        dwError =  VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    if (paramsCount > MAX_URL_PARAMS_ARR_SIZE)
    {
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

    if (key == NULL)
    {
        /**** Check if '?' is present in encoded format ****/
        key = strstr(pRequestURI, "%3F");
        if (key != NULL)
        {
           key = key + 2;
        }
    }

    while (i < paramsCount)
    {
        if (key)
        {
            value = strchr(key,'=');
            if (value)
            {
                res = pRequest->paramArray[i].key;
                diff = value - key - 1;
                if (diff < MAX_KEY_VAL_PARAM_LEN)
                {
                    strncpy(res, (key + 1), diff);
                    *(res + diff) = '\0';
                }
                else
                {
                    dwError = REQUEST_ENTITY_TOO_LARGE;
                }
                BAIL_ON_VMREST_ERROR(dwError);

                key = NULL;
                key = strchr(value, '&');
                res= pRequest->paramArray[i].value;
                if (key)
                {
                    diff = key - value - 1;
                    if (diff < MAX_KEY_VAL_PARAM_LEN)
                    {
                        strncpy(res, (value + 1), diff);
                        *(res + diff) = '\0';
                    }
                    else
                    {
                        dwError = REQUEST_ENTITY_TOO_LARGE;
                    }
                    BAIL_ON_VMREST_ERROR(dwError);
                }
                else
                {
                    if ((*(value +1) != '\0') && (strlen(value+1) < MAX_KEY_VAL_PARAM_LEN))
                    {
                        strncpy(res, (value +1), (MAX_KEY_VAL_PARAM_LEN -1));
                    }
                    else
                    {
                        dwError = REQUEST_ENTITY_TOO_LARGE;
                    }
                }    
            }
            else
            {
                dwError = VMREST_HTTP_INVALID_PARAMS;
            }
        }
        else
        {
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

/**** Exposed API to manupulate over params present in URI ****/

uint32_t
VmRESTGetParamsByIndex(
    PREST_REQUEST                    pRequest,
    uint32_t                         paramsCount,
    uint32_t                         paramIndex,
    char**                           ppszKey,
    char**                           ppszValue
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    uint32_t                         index = 0;
    char*                            pszKey = NULL;
    char*                            pszValue = NULL;

    if (paramIndex > paramsCount || paramIndex == 0) 
    {
        dwError = VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    if (!ppszKey || !ppszValue)
    {
        dwError = VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    index = paramIndex - 1;

    dwError = VmRESTAllocateMemory(
                  MAX_KEY_VAL_PARAM_LEN,
                  (void **)&pszKey
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTAllocateMemory(
                  MAX_KEY_VAL_PARAM_LEN,
                  (void **)&pszValue              
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    memset(pszKey, '\0', MAX_KEY_VAL_PARAM_LEN);
    memset(pszValue, '\0', MAX_KEY_VAL_PARAM_LEN);
    
    if ((pRequest != NULL) && (strlen(pRequest->paramArray[index].key) > 0))
    {
        VmRESTDecodeEncodedURLString(
            pRequest->paramArray[index].key,
            pszKey);
    }
    else
    {
        dwError = VMREST_HTTP_INVALID_PARAMS;
    }

    if ((pRequest != NULL) && (strlen(pRequest->paramArray[index].value) > 0))
    {
        VmRESTDecodeEncodedURLString(
            pRequest->paramArray[index].value,
            pszValue);
    }

    *ppszKey = pszKey;
    *ppszValue = pszValue;    

cleanup:
    return dwError;
error:
    if (pszKey != NULL)
    {
        VmRESTFreeMemory(pszKey);
        pszKey = NULL;
    }
    if (pszValue != NULL)
    {
        VmRESTFreeMemory(pszValue);
        pszValue = NULL;
    }
    if (ppszKey != NULL)
    {
        *ppszKey = NULL;
    }
    if (ppszValue != NULL)
    {
        *ppszValue = NULL;
    }
    goto cleanup;
}

uint32_t
VmRESTGetWildCardCount(
    PVMREST_HANDLE                   pRESTHandle,
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
        VMREST_LOG_ERROR(pRESTHandle,"%s","Invalid Params");
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
    if (ptr != NULL)
    {
        VmRESTFreeMemory(ptr);
        ptr = NULL;
    }
    
    dwError = VmRestGetEndPointURIfromRequestURI(
                  httpURI,
                  &ptr
                  );
    BAIL_ON_VMREST_ERROR(dwError);
    strncpy(endPointURI, ptr, (MAX_URI_LEN - 1));
    if (ptr != NULL)
    {
        VmRESTFreeMemory(ptr);
        ptr = NULL;    
    }

    dwError = VmRestEngineGetEndPoint(
                  pRESTHandle,
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
    if (wildCardCount != NULL)
    {
        *wildCardCount = 0;
    }
    goto cleanup;
}


uint32_t
VmRESTGetWildCardByIndex(
    PVMREST_HANDLE                   pRESTHandle,
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
        VMREST_LOG_ERROR(pRESTHandle,"%s","Invalid Params");
        dwError = VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTGetWildCardCount(
                  pRESTHandle,
                  pRequest,
                  &count
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    if (index > count)
    {
        VMREST_LOG_ERROR(pRESTHandle,"Invalid index count %u index %u", count, index);
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
    if (ptr != NULL)
    {
        VmRESTFreeMemory(ptr);
        ptr = NULL;
    }

    dwError = VmRestGetEndPointURIfromRequestURI(
                  httpURI,
                  &ptr
                  );
    BAIL_ON_VMREST_ERROR(dwError);
    strncpy(endPointURI,ptr,(MAX_URI_LEN - 1));
    if (ptr != NULL)
    {
        VmRESTFreeMemory(ptr);
        ptr = NULL;
    }

    dwError = VmRestEngineGetEndPoint(
                  pRESTHandle,
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
VmRestGetEndPointURIfromRequestURI(
    char const*                      pRequestURI,
    char**                           ppszEndPointURI
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    char*                            foundCharacter = NULL;
    uint64_t                         copyBytes = 0;
    char*                            hasSpace = NULL;
    char*                            pszEndPointURI = NULL;

    if (!pRequestURI || !ppszEndPointURI)
    {
        dwError =  VMREST_HTTP_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    if ((strlen(pRequestURI) == 0) || (strlen(pRequestURI) > MAX_URI_LEN))
    {
        dwError = BAD_REQUEST;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTAllocateMemory(
                  MAX_URI_LEN,
                  (void **)&pszEndPointURI
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    memset(pszEndPointURI, '\0', MAX_URI_LEN);

    foundCharacter = strchr(pRequestURI, '?');
    if (foundCharacter != NULL)
    {
        copyBytes = foundCharacter - pRequestURI;
        strncpy(pszEndPointURI,pRequestURI, copyBytes);
        *(pszEndPointURI + copyBytes) = '\0';
    }
    else
    {
        strcpy(pszEndPointURI,pRequestURI);
    }

    hasSpace = strchr(pszEndPointURI, ' ');
    if (hasSpace != NULL)
    {
        dwError = BAD_REQUEST;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    *ppszEndPointURI = pszEndPointURI;

cleanup:
    return dwError;
error:
    if (pszEndPointURI != NULL)
    {
        VmRESTFreeMemory(pszEndPointURI);
        pszEndPointURI = NULL;
    }
    if (ppszEndPointURI)
    {
        *ppszEndPointURI = NULL;
    }
    goto cleanup;
}
