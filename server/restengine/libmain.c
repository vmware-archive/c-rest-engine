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
VmRESTEngineInit(
    PVMREST_ENGINE_METHODS*     pHandlers
    ) 
{   
    uint32_t dwError = 0;
    PVMREST_THREAD pThreadpool = NULL;
    uint32_t nThreads = 0;

    /* Init Transport */
    dwError = VmRestTransportInit("80");
    BAIL_ON_VMREST_ERROR(dwError);


    /*  Adding test code - will remove 
    dwError = VmRESTTestHTTPResponse();
    */
    
    dwError = VmRestSpawnThreads(
                    &VmRestWorkerThread,
                    &pThreadpool,
                    &nThreads);
    BAIL_ON_VMREST_ERROR(dwError);

    gRESTEngGlobals.pThreadpool = pThreadpool;
    gRESTEngGlobals.nThreads = nThreads;
    gpHttpHandler = *pHandlers;

cleanup:

    return dwError;

error:

    goto cleanup;
}

void
VmRESTEngineShutdown(
    void
    )
{
    if (gRESTEngGlobals.nThreads)
    {
        VmRestFreeThreadpool(gRESTEngGlobals.pThreadpool, gRESTEngGlobals.nThreads);

        gRESTEngGlobals.pThreadpool = NULL;
        gRESTEngGlobals.nThreads = 0;
    }
}

