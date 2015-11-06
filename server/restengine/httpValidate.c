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

uint32_t
VmRESTRemovePreSpace(
    char*               src,
    char*               dest
    )
{
    uint32_t     dwError = 0;
    char*        temp = NULL;
    
    if (src == NULL || dest == NULL)
    {
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_VMREST_ERROR(dwError);
    }
    temp = src;
    while (temp != NULL)
    {
        if(*temp == ' ')
        {
            temp++;
        }
        else
        {
            strcpy(dest, temp);
            break;
        }
    }

cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VmRESTRemovePostSpace(
    char*               src,
    char*               dest
    )
{
    uint32_t     dwError = 0;
    char*        temp = NULL;
    uint32_t     len = 0;
    uint32_t     skip = 0;
    
    if (src == NULL || dest == NULL)
    {
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_VMREST_ERROR(dwError);
    }
    len = strlen(src);
    temp = (src+ len - 1);
    while (temp != src)
    {
        if(*temp == ' ')
        {
            temp--;
            skip++; 
        }
        else
        {
            memcpy(dest, src, (len - skip));
            *(dest + len - skip + 1) = '\0';
            break;
        }
    }    

cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VmRESTConverUpperToLower(
    char*               src,
    char*               dest
    )
{
    uint32_t     dwError = 0;
    char*        temp = NULL;
    char*        tempDes = NULL;

    if (src == NULL || dest == NULL)
    {
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_VMREST_ERROR(dwError);
    }
    temp = src;
    tempDes = dest;

    while(temp != NULL)
    {
        if (*temp >= 65 && *temp <= 90)
        {
            *tempDes = (*temp) + 32;
        }
        else
        {
            *tempDes = *temp;
        }
        temp++;
        tempDes++;
    }
    *tempDes = '\0';

cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VmRESTValidateHTTPMethodGET(
    PVM_REST_HTTP_REQUEST_PACKET   Request,
    uint32_t*                      result
    )
{
    uint32_t     dwError = 0;
    


    dwError = ERROR_NOT_SUPPORTED;
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VmRESTValidateHTTPMethodHEAD(
    PVM_REST_HTTP_REQUEST_PACKET   Request,
    uint32_t*                      result
    )
{
    uint32_t     dwError = 0;



    dwError = ERROR_NOT_SUPPORTED;
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VmRESTValidateHTTPMethodPOST(
    PVM_REST_HTTP_REQUEST_PACKET   Request,
    uint32_t*                      result
    )
{
    uint32_t     dwError = 0;



    dwError = ERROR_NOT_SUPPORTED;
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VmRESTValidateHTTPMethodPUT(
    PVM_REST_HTTP_REQUEST_PACKET   Request,
    uint32_t*                      result
    )
{
    uint32_t     dwError = 0;



    dwError = ERROR_NOT_SUPPORTED;
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VmRESTValidateHTTPMethodDELETE(
    PVM_REST_HTTP_REQUEST_PACKET   Request,
    uint32_t*                      result
    )
{
    uint32_t     dwError = 0;



    dwError = ERROR_NOT_SUPPORTED;
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VmRESTValidateHTTPMethodTRACE(
    PVM_REST_HTTP_REQUEST_PACKET   Request,
    uint32_t*                      result
    )
{
    uint32_t     dwError = 0;



    dwError = ERROR_NOT_SUPPORTED;
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VmRESTValidateHTTPMethodCONNECT(
    PVM_REST_HTTP_REQUEST_PACKET   Request,
    uint32_t*                      result
    )
{
    uint32_t     dwError = 0;



    dwError = ERROR_NOT_SUPPORTED;
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VmRESTValidateHTTPVersion(
    PVM_REST_HTTP_REQUEST_PACKET   Request,
    uint32_t*                      result,
    uint32_t*                      err
    )
{
    uint32_t     dwError = 0;
    uint32_t     len = 0;
    
    if ( Request == NULL || result == NULL || err == NULL )
    {
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_VMREST_ERROR(dwError);
    }

    *err = HTTP_VERSION_NOT_SUPPORTED;
    *result = FAIL;

    len = strlen(Request->requestLine->version);
    if (len > 0)
    {
        if (strcmp(Request->requestLine->version, "HTTP/1.1") == 0)
        {
            *err = 0;
            *result = PASS;
        }
    }

cleanup:
    return dwError;
error:
    goto cleanup;
}


uint32_t
VmRESTValidateHTTPRequestURI(
    PVM_REST_HTTP_REQUEST_PACKET   Request,
    uint32_t*                      result,
    uint32_t*                      err
    )
{
    uint32_t     dwError = 0;  
    uint32_t     len = 0;
    char*        temp = NULL;
  
    if ( Request == NULL || result == NULL || err == NULL )
    {
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_VMREST_ERROR(dwError);
    }
    
    *result = FAIL;

    /*1.  Check Max URI len 
      2.  Support both Req Line format
          
          GET http://www.w3.org/pub/www/project.html HTTP/1.1
           
          &&

          GET /pub/www/project.html HTTP/1.1
          HOST: www.w3.org
 
    */

    len = strlen(Request->requestLine->uri);
    if (len > MAX_URI_LEN)
    {
        *err = REQUEST_URI_TOO_LARGE;
        goto cleanup; 
    }
 
    temp = Request->requestLine->uri;
  
    if ((memcmp((void*)temp, "http", 4) != 0))
    {
        len = strlen(Request->requestHeader->host);
        if (len == 0)
        {
            /* No host name found on request */
            *err = BAD_REQUEST;
            goto cleanup;
        }
    } else 
    {
        if ((memcmp((temp+4) ,"://", 3)) != 0)
        { 
            *err = BAD_REQUEST;
            goto cleanup;
        }
    } 
    
    *result = PASS;    

cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VmRESTValidateHTTPContentType(
    PVM_REST_HTTP_REQUEST_PACKET   Request,
    uint32_t*                      result,
    uint32_t*                      err
    )
{
    uint32_t     dwError = 0;
    uint32_t     len = 0;
    char*        temp = NULL;
    if ( Request == NULL || result == NULL || err == NULL )
    {
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_VMREST_ERROR(dwError);
    }
    *result = PASS;
    *err = 0;
   
    len = strlen(Request->entityHeader->contentType);

    if (len > 0)
    {
        temp = strtok(strdup(Request->entityHeader->contentType), ",");
        while (temp != NULL)
        {
            if(strcmp(temp, "application/json") == 0)
            {
                *err = 0;
                *result = PASS;
                break;
            }
            else
            {
                *result = FAIL;
                *err = UNSUPPORTED_MEDIA_TYPE;
            }

            temp = strtok(NULL,",");
        }
    }
        
cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VmRESTValidateAccept(
    PVM_REST_HTTP_REQUEST_PACKET   Request,
    uint32_t*                      result,
    uint32_t*                      err
    )
{
    uint32_t     dwError = 0;
    uint32_t     len = 0;
    char*        temp = NULL;

    if ( Request == NULL || result == NULL || err == NULL )
    {
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_VMREST_ERROR(dwError);
    }
    *result = PASS;
    *err = 0;

    len = strlen(Request->requestHeader->accept);

    if (len > 0)
    {
        temp = strtok(strdup(Request->requestHeader->accept), ",");
        while (temp != NULL)
        {
            if(strcmp(temp, "application/json") == 0)
            {
                *err = 0;
                *result = PASS;
                break;
            }
            else
            {
                *result = FAIL;
                *err = UNSUPPORTED_MEDIA_TYPE;
            }

            temp = strtok(NULL,",");
        }

    }
    
cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VmRESTValidateAcceptCharSet(
    PVM_REST_HTTP_REQUEST_PACKET   Request,
    uint32_t*                      result,
    uint32_t*                      err
    )
{
    uint32_t     dwError = 0;
    uint32_t     len = 0;
    char*        temp = NULL;

    if ( Request == NULL || result == NULL || err == NULL )
    {
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_VMREST_ERROR(dwError);
    }
    *result = PASS;
    *err = 0;

    len = strlen(Request->requestHeader->acceptCharSet);
    if (len > 0)
    {
        temp = strtok(strdup(Request->requestHeader->acceptCharSet), ",");
        while (temp != NULL)
        {
            if(strcmp(temp, "utf-8") == 0)
            {
                *err = 0;
                *result = PASS;
                break;
            }
            else
            {
                *result = FAIL;
                *err = UNSUPPORTED_MEDIA_TYPE;
            }
            temp = strtok(NULL,",");
        }
    }

cleanup:
    return dwError;
error:
    goto cleanup;
}



uint32_t
VmRESTValidateHTTPRequest(
    PVM_REST_HTTP_REQUEST_PACKET   Request,
    uint32_t*                      result              
    )
{
    uint32_t     dwError = 0;

    



    dwError = ERROR_NOT_SUPPORTED;
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VmRESTValidateHTTPResponse(
    PVM_REST_HTTP_REQUEST_PACKET   Request,
    PVM_REST_HTTP_RESPONSE_PACKET  Response,
    uint32_t*                      result
    )
{
    uint32_t     dwError = 0;



    dwError = ERROR_NOT_SUPPORTED;
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:
    return dwError;
error:
    goto cleanup;
}



