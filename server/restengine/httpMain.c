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
VmHTTPInit(
    PVMREST_HANDLE                  pRESTHandle,
    PREST_CONF                       pConfig,
    char const*                      file
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    //PVM_REST_CONFIG                  restConfig = NULL;
//    PREST_ENG_GLOBALS                pInstanceGlobal = NULL;

    if (!pRESTHandle)
    {
        VMREST_LOG_DEBUG(pRESTHandle,"Invalid REST Handler");
        dwError = REST_ENGINE_INVALID_HANDLER;
    }
    BAIL_ON_VMREST_ERROR(dwError);

/*    dwError = VmRESTAllocateMemory(
                  sizeof(REST_ENG_GLOBALS),
                  (void **)&pInstanceGlobal
                  );
    BAIL_ON_VMREST_ERROR(dwError);
*/
    
//    pRESTHandle->pInstanceGlobal = pInstanceGlobal;
    pRESTHandle->debugLogLevel = VMREST_LOG_LEVEL_DEBUG;


    //vmrest_syslog_level = VMREST_LOG_LEVEL_DEBUG;

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
//    pRESTHandle->pInstanceGlobal->config = restConfig;
  //  pRESTHandle->pRESTConfig = restConfig;
    pRESTHandle->pInstanceGlobal->useEndPoint = 0;

    /**** Set the config at Global level ****/
  //  VmRESTSetConfig(restConfig);

cleanup:
    return dwError;
error:
    /*
    if (restConfig)
    {
        VmRESTFreeConfigFileStruct(
                restConfig
                );
        if (pRESTHandle && pRESTHandle->pRESTConfig)
        {
            pRESTHandle->pRESTConfig = NULL; 
        }
    //    VmRESTUnSetConfig();
    }
    */
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
        dwError = REST_ENGINE_INVALID_REST_PROCESSER;
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
        dwError = REST_ENGINE_INVALID_REST_PROCESSER;
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
