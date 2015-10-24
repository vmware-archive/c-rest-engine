
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

#include <includes.h>

uint32_t 
VmRESTGetHttpMethod(
    PVM_REST_HTTP_REQUEST_PACKET    pRequest,
    char*                           response
    )
{
    uint32_t dwError   = 0;
    uint32_t methodLen = 0;
    if ((pRequest == NULL) || (pRequest->requestLine == NULL))
    {
        /* Bad request:: No memory */
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_VMREST_ERROR(dwError);
    }
    methodLen = strlen(pRequest->requestLine->method);
    if (methodLen == 0 || methodLen > MAX_METHOD_LEN)
    {   
        /* Bad method name found in request object */
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_VMREST_ERROR(dwError);
    }
    if (response == NULL)
    {
        /* No memory for response object */
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_VMREST_ERROR(dwError);

    }
    if ((strcmp(pRequest->requestLine->method,"GET")) == 0 )
    {
        strcpy(response, "GET");
    } 
    else if ((strcmp(pRequest->requestLine->method,"HEAD")) == 0 )
    {
        strcpy(response, "HEAD");
    }
    else if ((strcmp(pRequest->requestLine->method,"POST")) == 0 )
    {
        strcpy(response, "POST");
    }
    else if ((strcmp(pRequest->requestLine->method,"PUT")) == 0 )
    {
        strcpy(response, "PUT");
    }
    else if ((strcmp(pRequest->requestLine->method,"DELETE")) == 0 )
    {
        strcpy(response, "DELETE");
    }
    else if ((strcmp(pRequest->requestLine->method,"TRACE")) == 0 )
    {
        strcpy(response, "TRACE");
    }
    else if ((strcmp(pRequest->requestLine->method,"CONNECT")) == 0 )
    {
        strcpy(response, "CONNECT");
    }
    else
    {
        /* No allowed method found in request object */
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_VMREST_ERROR(dwError);
    }

cleanup:
 
    return dwError;

error:

    goto cleanup;

}


uint32_t
VmRESTGetHttpURI(
    PVM_REST_HTTP_REQUEST_PACKET    pRequest,
    char*                           response
    )
{
    uint32_t dwError   = 0;
    uint32_t uriLen = 0;
    if ((pRequest == NULL) || (pRequest->requestLine == NULL))
    {
        /* Bad request:: No memory */
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_VMREST_ERROR(dwError);
    }
    uriLen = strlen(pRequest->requestLine->uri);
    if (uriLen == 0 || uriLen > MAX_URI_LEN)
    {
        /* Bad uri found in request object */
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_VMREST_ERROR(dwError);
    }
    if (response == NULL)
    {
        /* No memory for response object */
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_VMREST_ERROR(dwError);

    }
    strcpy(response, pRequest->requestLine->uri);    

cleanup:

    return dwError;

error:

    goto cleanup;
}

uint32_t 
VmRESTGetHttpVersion(
    PVM_REST_HTTP_REQUEST_PACKET    pRequest,
    char*                           response
    )
{
    uint32_t dwError   = 0;
    uint32_t versionLen = 0;
    if ((pRequest == NULL) || (pRequest->requestLine == NULL))
    {
        /* Bad request:: No memory */
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_VMREST_ERROR(dwError);
    }
    versionLen = strlen(pRequest->requestLine->version);
    if (versionLen == 0 || versionLen > MAX_VERSION_LEN)
    {
        /* Bad version found in request object */
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_VMREST_ERROR(dwError);
    }
    if (response == NULL)
    {
        /* No memory for response object */
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_VMREST_ERROR(dwError);

    }
    strcpy(response, pRequest->requestLine->version);

cleanup:

    return dwError;

error:

    goto cleanup;
}

uint32_t 
VmRESTGetHttpHeader(
    PVM_REST_HTTP_REQUEST_PACKET    pRequest,
    char*                           header,
    char*                           response
    )
{
    uint32_t dwError = 0;
    BAIL_ON_VMREST_ERROR(dwError);      

cleanup:

    return dwError;

error:

    goto cleanup;


}


