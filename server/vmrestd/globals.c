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

VMREST_GLOBALS gVMRESTGlobals =
{
    .mutex = PTHREAD_MUTEX_INITIALIZER
};

REST_PROCESSOR gVmRestHandlers =
{
    .pfnHandleRequest = &VmRESTHandleHTTP_REQUEST,
    .pfnHandleCreate = NULL,
    .pfnHandleRead = NULL,
    .pfnHandleUpdate = NULL,
    .pfnHandleDelete = NULL
};

PREST_PROCESSOR gpVmRestHandlers = &gVmRestHandlers;

REST_PROCESSOR gVmEndPointHandler[MAX_RESOURCE];

void
VmAppStoreEndpoint(
    uint32_t                         index,
    PFN_PROCESS_REST_CREATE          pfnCreate,
    PFN_PROCESS_REST_READ            pfnRead,
    PFN_PROCESS_REST_UPDATE          pfnUpdate,
    PFN_PROCESS_REST_DELETE          pfnDelete
    )
{
    gVmEndPointHandler[index].pfnHandleCreate = pfnCreate;
    gVmEndPointHandler[index].pfnHandleRead = pfnRead;
    gVmEndPointHandler[index].pfnHandleUpdate = pfnUpdate;
    gVmEndPointHandler[index].pfnHandleDelete = pfnDelete;
}

