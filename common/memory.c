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
VmRESTAllocateMemory(
    size_t   dwSize,
    void**   ppMemory
    )
{
    uint32_t dwError = 0;
    void* pMemory = NULL;

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
    void*  pMemory
    )
{
    if (pMemory)
    {
        free(pMemory);
    }

}



