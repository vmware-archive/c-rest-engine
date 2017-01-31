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
#include "..\transport\win\includes.h"

uint32_t                         useFile = 0;
uint32_t                         cbIndex = 0;

#define MAX_ARGUMENTS 10
#define MAX_ARG_LEN 20
#define MAX_COMMAND_LEN 100
#define COMMAND_FILE_EXTENSION ".tcf"

int     exitLoop = 0;

uint32_t
VmRegisterHandler(
    char*                            URI,
    uint32_t                         index
    )
{
    uint32_t                         dwError = 0;
    /**** Call helper in global with resource specific pointer ****

    VmAppStoreEndpoint(
        index,
        &VmHandlePackageWrite,
        &VmHandlePackageRead,
        &VmHandlePackageUpdate,
        &VmHandlePackageDelete
        );
    */
     VmAppStoreEndpoint(
        index,
        &VmHandleEchoData,
        &VmHandleEchoData,
        &VmHandleEchoData,
        &VmHandleEchoData
        );

     dwError = VmRESTRegisterHandler(
                  URI,
                  &(gVmEndPointHandler[index]),
                  NULL
                  );
     BAIL_ON_VMREST_ERROR(dwError);

cleanup:
    return dwError;
error:
    goto cleanup;
}

int main()
{
    uint32_t                         dwError = 0;

    dwError = VmRESTInit(NULL,"c:\\tmp\\restconfig.txt");
    VmRegisterHandler("/v1/pkg", 0);

    VmRESTStart();

    Sleep(20000);

    dwError = VmRESTStop();

    dwError = VmRESTUnRegisterHandler("/v1/pkg");

    VmRESTShutdown();
   
    return dwError;

}

