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
    PVMREST_HANDLER                  pRESTHandler,
    PREST_CONF                       pConfig,
    char const*                      file
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    PVM_REST_CONFIG                  restConfig = NULL;
    PREST_ENG_GLOBALS                pInstanceGlobal = NULL;

    if (!pRESTHandler)
    {
        VMREST_LOG_DEBUG("Invalid REST Handler");
        dwError = REST_ENGINE_INVALID_HANDLER;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTAllocateMemory(
                  sizeof(REST_ENG_GLOBALS),
                  (void **)&pInstanceGlobal
                  );
    BAIL_ON_VMREST_ERROR(dwError);
    
    pRESTHandler->pInstanceGlobal = pInstanceGlobal;
    pRESTHandler->debugLogLevel = VMREST_LOG_LEVEL_DEBUG;


    //vmrest_syslog_level = VMREST_LOG_LEVEL_DEBUG;

    if (pConfig != NULL)
    {
        dwError = VmRESTCopyConfig(
                      pConfig,
                      &restConfig
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
                          &restConfig
                          );
        }
        else
        {
            dwError = VmRESTParseAndPopulateConfigFile(
                          file,
                          &restConfig
                          );
        }
        BAIL_ON_VMREST_ERROR(dwError);
 

        /**** Validate the config param ****/
        dwError= VmRESTValidateConfig(
                      restConfig
                      );
        BAIL_ON_VMREST_ERROR(dwError);
    }

    /**** Init the debug log ****/
    dwError = VmRESTLogInitialize(
                  restConfig->debug_log_file
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    /**** Validate the config param ****/
    dwError = VmRESTValidateConfig(
                  restConfig
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    /**** Init Transport ****/
    dwError = VmwSockInitialize(pRESTHandler);
    BAIL_ON_VMREST_ERROR(dwError);

    /**** Update context Info for this lib instance ****/
//    pRESTHandler->pInstanceGlobal->config = restConfig;
    pRESTHandler->pRESTConfig = restConfig;
    pRESTHandler->pInstanceGlobal->useEndPoint = 0;

    /**** Set the config at Global level ****/
  //  VmRESTSetConfig(restConfig);

cleanup:
    return dwError;
error:
    if (restConfig)
    {
        VmRESTFreeConfigFileStruct(
                restConfig
                );
        if (pRESTHandler && pRESTHandler->pRESTConfig)
        {
            pRESTHandler->pRESTConfig = NULL; 
        }
    //    VmRESTUnSetConfig();
    }
    
    goto cleanup;
}

uint32_t
VmHTTPStart(
    PVMREST_HANDLER                  pRESTHandler
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;

    dwError = VmRESTInitProtocolServer(pRESTHandler);
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:

    return dwError;
error:
    goto cleanup;
}

uint32_t
VmHTTPRegisterHandler(
    PVMREST_HANDLER                  pRESTHandler,
    PREST_PROCESSOR                  pHandler
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;

    if (!pHandler || !pRESTHandler)
    {
        dwError = REST_ENGINE_INVALID_REST_PROCESSER;
    }
    BAIL_ON_VMREST_ERROR(dwError);
    pRESTHandler->pHttpHandler = pHandler;

cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VmHTTPUnRegisterHandler(
    void
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;

    if (gpHttpHandler == NULL)
    {
        dwError = REST_ENGINE_INVALID_REST_PROCESSER;
    }
    BAIL_ON_VMREST_ERROR(dwError);
    gpHttpHandler = NULL;

cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VmHTTPStop(
    PVMREST_HANDLER                  pRESTHandler
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;

    VMREST_LOG_DEBUG("%s","Shutting down rest engine ....");
    VmRESTShutdownProtocolServer(pRESTHandler);
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:
    return dwError;
error:
    goto cleanup;
}

void
VmHTTPShutdown(
    PVMREST_HANDLER                  pRESTHandler
    )
{
    VmwSockShutdown(pRESTHandler);

    if (pRESTHandler->pRESTConfig)
    {
        VmRESTFreeConfigFileStruct(
            pRESTHandler->pRESTConfig
            );
        VmRESTLogTerminate(
            );
        pRESTHandler->pRESTConfig = NULL;
    }
}
