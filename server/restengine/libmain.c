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
VmRESTInit(
    PREST_CONF                       pConfig
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    uint32_t                         isTransportInit = 0;
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
    else if (pConfig == NULL)
    {
        /**** Init the rest engine with default config ****/
        dwError = VmRESTParseAndPopulateConfigFile(
                      "/root/restconfig.txt",
                      &restConfig
                      );
        BAIL_ON_VMREST_ERROR(dwError);

        /**** Validate the config param ****/
        dwError= VmRESTValidateConfig(
                      restConfig
                      );
        BAIL_ON_VMREST_ERROR(dwError);
    }
    else
    {
        dwError = REST_ENGINE_INVALID_CONFIG;    
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
    dwError = VmRestTransportInit(
                  restConfig->server_port
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    isTransportInit = 1;

    /**** Update the global context for this lib instance ****/
    gRESTEngGlobals.config = restConfig;

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

uint32_t
VmRESTStart(
    void
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    uint32_t                         isTransportStarted = 0;
    PVMREST_THREAD                   pThreadpool = NULL;

    /**** Start Transport ****/
    dwError = VmRestTransportStart(
                  gRESTEngGlobals.config->server_port,
                  gRESTEngGlobals.config->ssl_certificate,
                  gRESTEngGlobals.config->ssl_key,
                  atoi(gRESTEngGlobals.config->client_count)
                  );
    BAIL_ON_VMREST_ERROR(dwError);
    isTransportStarted = 1;

    /*************************************
    *  Adding test code - will remove
    *  dwError = VmRESTTestHTTPResponse();
    *************************************/
    
    /**** Create the thread pool of worker threads ****/
    dwError = VmRestSpawnThreads(
                    &VmRestWorkerThread,
                    &pThreadpool,
                    atoi(gRESTEngGlobals.config->worker_thread_count)
                    );
    BAIL_ON_VMREST_ERROR(dwError);

    /**** Update the global context for this lib instance ****/
    gRESTEngGlobals.pThreadpool = pThreadpool;
    gRESTEngGlobals.nThreads = atoi(gRESTEngGlobals.config->worker_thread_count);
    
    //gpHttpHandler = *pHandlers;

cleanup:
    return dwError;
error:
    if (isTransportStarted == 1)
    {
        VmRESTTransportStop(
            );
        isTransportStarted = 0;
    }
    goto cleanup;
}

uint32_t
VmRESTRegisterHandler(
    char const*                      pszEndpoint,
    PREST_PROCESSOR                  pHandler,
    PREST_ENDPOINT*                  ppEndpoint
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;

    if (pHandler == NULL)
    {
        dwError = REST_ENGINE_INVALID_REST_PROCESSER;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    /**** TODO: END POINT HANDLING ****/

    /**** Register the HTTP method based handler ****/
    gpHttpHandler = pHandler;

cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VmRESTFindEndpoint(
    char const*                      pszEndpoint,
    PREST_ENDPOINT*                  ppEndpoint
    )
{
    /**** NOT YET IMPLEMENTED ****/
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VmRESTUnregisterHandler(
    PREST_ENDPOINT                   pEndpoint
    )
{
    /**** NOT YET IMPLEMENTED ****/
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:
    return dwError;
error:
    goto cleanup;
}

void
VmRESTReleaseEndpoint(
    PREST_ENDPOINT                   pEndpoint
    )
{
     /**** NOT YET IMPLEMENTED ****/


}

uint32_t
VmRESTStop(
    void
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;

    /**** Stop transport ****/
    VmRESTTransportStop(
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
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:
    return dwError;
error:
    goto cleanup;
}

void
VmRESTShutdown(
    void
    )
{
    /**** Shutdown transport ****/
    VmRESTTransportShutdown(
        );

    if (gRESTEngGlobals.config)
    {
        VmRESTFreeConfigFileStruct(
            gRESTEngGlobals.config
            );
    }
    VmRESTLogTerminate(
        );
}

