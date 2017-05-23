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
VmHTTPInit(
    PVMREST_HANDLE                   pRESTHandle,
    PREST_CONF                       pConfig,
    char const*                      file
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;

    if (!pRESTHandle)
    {
        VMREST_LOG_DEBUG(pRESTHandle,"%s","Invalid REST Handler");
        dwError = REST_ERROR_INVALID_HANDLER;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    pRESTHandle->debugLogLevel = VMREST_LOG_LEVEL_DEBUG;

    if (pConfig != NULL)
    {
        dwError = VmRESTCopyConfig(
                      pConfig,
                      &(pRESTHandle->pRESTConfig)
                      );
        BAIL_ON_VMREST_ERROR(dwError);
    }
    else   // pconfig ==  NULL
    {
        if (file == NULL)
        {
            /**** Init the rest engine with default config ****/
            dwError = VmRESTParseAndPopulateConfigFile(
                          "/root/restconfig.txt",
                          &(pRESTHandle->pRESTConfig)
                          );
        }
        else
        {
            dwError = VmRESTParseAndPopulateConfigFile(
                          file,
                          &(pRESTHandle->pRESTConfig)
                          );
        }
        BAIL_ON_VMREST_ERROR(dwError);
 

        /**** Validate the config param ****/
        dwError= VmRESTValidateConfig(
                      pRESTHandle->pRESTConfig
                      );
        BAIL_ON_VMREST_ERROR(dwError);
    }

    /**** Init the debug log ****/
    dwError = VmRESTLogInitialize(
                  pRESTHandle
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    /**** Validate the config param ****/
    dwError = VmRESTValidateConfig(
                  pRESTHandle->pRESTConfig
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    /**** Init Transport ****/
    dwError = VmwSockInitialize(pRESTHandle);
    BAIL_ON_VMREST_ERROR(dwError);

    /**** Update context Info for this lib instance ****/
    pRESTHandle->pInstanceGlobal->useEndPoint = 0;

cleanup:

    return dwError;

error:

    goto cleanup;
}

uint32_t
VmHTTPStart(
    PVMREST_HANDLE                  pRESTHandle
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;

    dwError = VmRESTInitProtocolServer(pRESTHandle);
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:

    return dwError;
error:
    goto cleanup;
}

uint32_t
VmHTTPRegisterHandler(
    PVMREST_HANDLE                  pRESTHandle,
    PREST_PROCESSOR                  pHandler
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;

    if (!pHandler || !pRESTHandle)
    {
        dwError = REST_ERROR_INVALID_REST_PROCESSER;
    }
    BAIL_ON_VMREST_ERROR(dwError);
    pRESTHandle->pHttpHandler = pHandler;

cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VmHTTPUnRegisterHandler(
    PVMREST_HANDLE                  pRESTHandle
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;

    if (pRESTHandle == NULL)
    {
        dwError = REST_ERROR_INVALID_REST_PROCESSER;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    pRESTHandle->pHttpHandler = NULL;

cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VmHTTPStop(
    PVMREST_HANDLE                  pRESTHandle
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;

    VMREST_LOG_DEBUG(pRESTHandle,"%s","Shutting down rest engine ....");
    VmRESTShutdownProtocolServer(pRESTHandle);
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:
    VMREST_LOG_DEBUG(pRESTHandle,"Stop returning %u", dwError);
    return dwError;
error:
    goto cleanup;
}

void
VmHTTPShutdown(
    PVMREST_HANDLE                  pRESTHandle
    )
{
    VmwSockShutdown(pRESTHandle);

    if (pRESTHandle)
    {
        VmRESTLogTerminate(pRESTHandle);
        VmRESTFreeHandle(pRESTHandle);        
    }
}
