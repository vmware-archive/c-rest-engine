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

