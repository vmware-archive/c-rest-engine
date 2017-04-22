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
VmRESTAllocateMemory(
    size_t                           dwSize,
    void**                           ppMemory
    )
{
    uint32_t                         dwError = 0;
    void*                            pMemory = NULL;

    if (!ppMemory || !dwSize)
    {
        dwError = EINVAL;
        BAIL_ON_VMREST_ERROR(dwError);
    }

    pMemory = calloc(1, dwSize);
    if (!pMemory)
    {
        dwError = ENOMEM;
        BAIL_ON_VMREST_ERROR(dwError);
    }

    *ppMemory = pMemory;

cleanup:

    return dwError;

error:
    VMREST_SAFE_FREE_MEMORY(pMemory);

    goto cleanup;
}


void
VmRESTFreeMemory(
    void*                            pMemory
    )
{
    if (pMemory)
    {
        free(pMemory);
    }

}

