/*
 * Copyright © 2012-2015 VMware, Inc.  All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the ~@~\License~@~]); you may not
 * use this file except in compliance with the License.  You may obtain a copy
 * of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an ~@~\AS IS~@~] BASIS, without
 * warranties or conditions of any kind, EITHER EXPRESS OR IMPLIED.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

#include "includes.h"

REST_PROCESSOR gVmRestHandlers =
{
	
    VMREST_SF_INIT(.pfnHandleRequest ,NULL),
    VMREST_SF_INIT(.pfnHandleCreate , &VmHandleTestOTHERS),
    VMREST_SF_INIT(.pfnHandleRead ,   &VmHandleTestOTHERS),
    VMREST_SF_INIT(.pfnHandleUpdate , &VmHandleTestOTHERS),
    VMREST_SF_INIT(.pfnHandleDelete , &VmHandleTestOTHERS),
    VMREST_SF_INIT(.pfnHandleOthers , &VmHandleTestOTHERS)
};

PREST_PROCESSOR gpVmRestHandlers = &gVmRestHandlers;


uint32_t
VmHandleTestOTHERS(
    PREST_REQUEST                    pRequest,
    PREST_RESPONSE*                  ppResponse,
    uint32_t                         paramsCount
    )
{
    uint32_t                         dwError = 0;
    return dwError;
}

uint32_t
getRESTEmptyReqPacket(
    PREST_REQUEST*                   ppRequest
)
{ 
    PREST_REQUEST                    pRequest = NULL;
    uint32_t                         dwError = 0;

    dwError = VmRESTAllocateHTTPRequestPacket(&pRequest);
    BAIL_ON_VMREST_ERROR(dwError);
    
    *ppRequest = pRequest;

cleanup:
    return dwError;
error:
    goto cleanup;
}

