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
    PREST_CONF                       pConfig
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;

    if (!pRESTHandle || !pConfig)
    {
        VMREST_LOG_DEBUG(pRESTHandle,"%s","Invalid params");
        dwError = REST_ERROR_INVALID_CONFIG;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTCopyConfig(
                  pConfig,
                  &(pRESTHandle->pRESTConfig)
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    /**** Validate the config param ****/
    dwError = VmRESTValidateConfig(
                  pRESTHandle,
                  pRESTHandle->pRESTConfig
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    /**** Init logging and transport ****/
    dwError = VmRESTInitProtocolServer(
                  pRESTHandle
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    pRESTHandle->debugLogLevel = pRESTHandle->pRESTConfig->debugLogLevel;

    /**** Update context Info for this lib instance ****/
    pRESTHandle->pInstanceGlobal->useEndPoint = 0;

cleanup:

    return dwError;

error:

    goto cleanup;
}

uint32_t
VmHTTPStart(
    PVMREST_HANDLE                   pRESTHandle
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;

    dwError = VmRESTStartProtocolServer(
                  pRESTHandle
                  );
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
    PVMREST_HANDLE                   pRESTHandle,
    uint32_t                         waitSecond
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;

    VMREST_LOG_INFO(pRESTHandle,"%s","Stopping c-rest engine ....");
    dwError = VmRESTStopProtocolServer(pRESTHandle, waitSecond);
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:

    return dwError;
error:

    VMREST_LOG_ERROR(pRESTHandle,"C-REST-ENGINE: Library Stop failed ... Do not attempt shutdown, dwError = %u", dwError);
    goto cleanup;
}

void
VmHTTPShutdown(
    PVMREST_HANDLE                  pRESTHandle
    )
{
    VmRESTShutdownProtocolServer(
        pRESTHandle
        );

    if (pRESTHandle)
    {
        VmRESTFreeHandle(pRESTHandle);        
    }
}
