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
VmRESTEngineInit(
    PVMREST_ENGINE_METHODS*    pHandlers,
    char*                      configFile
    )
{
    uint32_t                   dwError = 0;
    uint32_t                   isTransportInit = 0;
    PVMREST_THREAD             pThreadpool = NULL;
    PVM_REST_CONFIG            restConfig = NULL;

    vmrest_syslog_level = VMREST_LOG_LEVEL_DEBUG;

    /**** Parse the rest engine configuration ****/
    dwError = VmRESTParseAndPopulateConfigFile(
                  configFile,
                  &restConfig
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    /**** Init the debug log ****/
    dwError = VmRESTLogInitialize(
                  restConfig->debug_log_file
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    /**** Validate the config param ****/
    dwError= VmRESTValidateConfig(
                        restConfig
                        );
    BAIL_ON_VMREST_ERROR(dwError);

    /**** Init Transport ****/
    dwError = VmRestTransportInit(
                  restConfig->server_port,
                  restConfig->ssl_certificate,
                  restConfig->ssl_key
                  );
    BAIL_ON_VMREST_ERROR(dwError);
    isTransportInit = 1;

    /*************************************
    *  Adding test code - will remove
    *  dwError = VmRESTTestHTTPResponse();
    *************************************/

    /**** Create the thread pool of worker threads ****/
    dwError = VmRestSpawnThreads(
                    &VmRestWorkerThread,
                    &pThreadpool,
                    atoi(restConfig->worker_thread_count)
                    );
    BAIL_ON_VMREST_ERROR(dwError);

    /**** Update the global context for this lib instance ****/
    gRESTEngGlobals.pThreadpool = pThreadpool;
    gRESTEngGlobals.nThreads = atoi(restConfig->worker_thread_count);
    gRESTEngGlobals.config = restConfig;
    gpHttpHandler = *pHandlers;

cleanup:

    return dwError;

error:
    if (restConfig)
    {
        VmRESTFreeConfigFileStruct(
                restConfig
                );
        gRESTEngGlobals.config = NULL;
    }
    if (isTransportInit == 1)
    {
        VmRESTTransportShutdown(
            );
        isTransportInit = 0;  
    }
    goto cleanup;
}

void
VmRESTEngineShutdown(
    void
    )
{
    /**** Shutdown transport ****/
    VmRESTTransportShutdown(
        );

    /**** Stop and free the thread pool of worker thread ****/
    if (gRESTEngGlobals.nThreads)
    {
        VmRestFreeThreadpool(
            gRESTEngGlobals.pThreadpool,
            gRESTEngGlobals.nThreads
            );
        gRESTEngGlobals.pThreadpool = NULL;
        gRESTEngGlobals.nThreads = 0;
    }
    if (gRESTEngGlobals.config)
    {
        VmRESTFreeConfigFileStruct(
            gRESTEngGlobals.config
            );
    }
}

