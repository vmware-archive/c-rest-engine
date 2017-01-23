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
    PREST_CONF                       pConfig,
    char const*                      file
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    PVM_REST_CONFIG                  restConfig = NULL;

    vmrest_syslog_level = VMREST_LOG_LEVEL_DEBUG;

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
    dwError = VmwSockInitialize();
    BAIL_ON_VMREST_ERROR(dwError);

    /**** Update the global context for this lib instance ****/
    gRESTEngGlobals.config = restConfig;
    gRESTEngGlobals.useEndPoint = 0;

    /**** Set the config at Global level ****/
    VmRESTSetConfig(restConfig);

cleanup:
    return dwError;
error:
    if (restConfig)
    {
        VmRESTFreeConfigFileStruct(
                restConfig
                );
        gRESTEngGlobals.config = NULL;
        VmRESTUnSetConfig();
    }
    goto cleanup;
}

uint32_t
VmHTTPStart(
    void
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;

    dwError = VmRESTInitProtocolServer();
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:

    return dwError;
error:
    goto cleanup;
}

uint32_t
VmHTTPRegisterHandler(
    PREST_PROCESSOR                  pHandler
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;

    if (pHandler == NULL)
    {
        dwError = REST_ENGINE_INVALID_REST_PROCESSER;
    }
    BAIL_ON_VMREST_ERROR(dwError);
    gpHttpHandler = pHandler;

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
    void
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;

    VMREST_LOG_DEBUG("Shutting down rest engine ....");
    VmRESTShutdownProtocolServer();
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:
    return dwError;
error:
    goto cleanup;
}

void
VmHTTPShutdown(
    void
    )
{
    VmwSockShutdown();

    if (gRESTEngGlobals.config)
    {
        VmRESTFreeConfigFileStruct(
            gRESTEngGlobals.config
            );
        VmRESTLogTerminate(
            );
        gRESTEngGlobals.config = NULL;
    }
}
