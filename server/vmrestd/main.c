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

#ifdef WIN32
#include "..\transport\win\includes.h"
char* configPath = "c:\\tmp\\restconfig.txt";
char* configPath1 = "c:\\tmp\\restconfig1.txt";
#else
#include <getopt.h>
#include <sys/types.h>
#include <sys/inotify.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <stdio.h>
char* configPath = "/tmp/restconfig.txt";
char* configPath1 = "/tmp/restconfig1.txt";
#endif


PVMREST_HANDLE  gpRESTHandle = NULL;
PVMREST_HANDLE  gpRESTHandle1 = NULL;

REST_PROCESSOR gVmRestHandlers;
REST_PROCESSOR gVmRestHandlers1;

uint32_t
VmRESTUtilsConvertInttoString(
    int                              num,
    char*                            str
    );


int main()
{
    uint32_t                         dwError = 0;
    uint32_t                         cnt = 0;

    gVmRestHandlers.pfnHandleRequest = NULL;
    gVmRestHandlers.pfnHandleCreate = &VmHandleEchoData;
    gVmRestHandlers.pfnHandleRead = &VmHandleEchoData;
    gVmRestHandlers.pfnHandleUpdate = &VmHandleEchoData;
    gVmRestHandlers.pfnHandleDelete = &VmHandleEchoData;
    gVmRestHandlers.pfnHandleOthers = &VmHandleEchoData;

    gVmRestHandlers1.pfnHandleRequest = NULL;
    gVmRestHandlers1.pfnHandleCreate = &VmHandleEchoData1;
    gVmRestHandlers1.pfnHandleRead = &VmHandleEchoData1;
    gVmRestHandlers1.pfnHandleUpdate = &VmHandleEchoData1;
    gVmRestHandlers1.pfnHandleDelete = &VmHandleEchoData1;
    gVmRestHandlers1.pfnHandleOthers = &VmHandleEchoData1;


    dwError = VmRESTInit(NULL,configPath, &gpRESTHandle);
    dwError = VmRESTInit(NULL,configPath1, &gpRESTHandle1);

    VmRESTRegisterHandler(gpRESTHandle, "/v1/pkg", &gVmRestHandlers, NULL);
    VmRESTRegisterHandler(gpRESTHandle1, "/v1/blah", &gVmRestHandlers1, NULL);

    VmRESTStart(gpRESTHandle);
    VmRESTStart(gpRESTHandle1);


    while(cnt < 10)
    {
#ifdef WIN32
        Sleep(1000);
#else
		sleep(1);
#endif
        cnt++;
    }

    dwError = VmRESTStop(gpRESTHandle);
    dwError = VmRESTStop(gpRESTHandle1);

    dwError = VmRESTUnRegisterHandler(gpRESTHandle,"/v1/pkg");
    dwError = VmRESTUnRegisterHandler(gpRESTHandle1,"/v1/blah");

    VmRESTShutdown(gpRESTHandle);
    VmRESTShutdown(gpRESTHandle1);


return dwError;

}

uint32_t
VmHandleEchoData1(
    PVMREST_HANDLE                   pRESTHandle,
    PREST_REQUEST                    pRequest,
    PREST_RESPONSE*                  ppResponse,
    uint32_t                         paramsCount
    )
{
    uint32_t                         dwError = 0;
    uint32_t                         done = 0;
    char                             AllData[MAX_IN_MEM_PAYLOAD_LEN] = {0};
    char                             buffer[4097] = {0};
    int                              nRead = 0;
    int                              nWrite = 0;
    uint32_t                         bytesLeft = 0;
    char                             size[10] = {0};
    int                              resLength = 0;
    uint32_t                         index = 0;
   // char*                            key = NULL;
   // char*                            value = NULL;

    memset(buffer, '\0', 4097);
    memset(AllData, '\0', MAX_IN_MEM_PAYLOAD_LEN);
    memset(size, '\0', 10);

    done = 0;
    //write(1,"\nParams.....:", 14);
  /*  while (done < paramsCount)
    {
        dwError = VmRESTGetParamsByIndex(
                      pRequest,
                      paramsCount,
                      (done +1),
                      &key,
                      &value
                      );
        BAIL_ON_VMREST_ERROR(dwError);
        //write(1,"\nParams.....:", 14);
        write(1,key, strlen(key));
        write(1,":", 1);
        write(1,value, strlen(value));
        write(1,"\n", 1);
        if (key)
        {
            free(key);
            key = NULL;
        }
        if (value)
        {
            free(value);
            value = NULL;
        }
        done++;
    }
    done = 0;
    */
    while(done != 1)
    {
        dwError = VmRESTGetData(
                      pRESTHandle,
                      pRequest,
                      buffer,
                      &done
                      );
        BAIL_ON_VMREST_ERROR(dwError);

        nRead = strlen(buffer);

        if (nRead > 0)
        {
            if ((index + nRead)< MAX_IN_MEM_PAYLOAD_LEN)
            {
                memcpy((AllData + index),buffer,nRead);
                index += nRead;
            }
            else
            {
                dwError = 500;
            }
            BAIL_ON_VMREST_ERROR(dwError);

        }
        memset(buffer, '\0', 4097);
        nRead = 0;
    }

    dwError = VmRESTSetSuccessResponse(
                  pRequest,
                  ppResponse
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    memset(buffer, '\0', 4097);

    resLength = strlen(AllData);

    if (resLength < 4096 )
    {
        dwError = VmRESTUtilsConvertInttoString(
                          resLength,
                          size);
        BAIL_ON_VMREST_ERROR(dwError);

        dwError = VmRESTSetDataLength(
                  ppResponse,
                  size
                  );
        BAIL_ON_VMREST_ERROR(dwError);
    }
    else
    {
        dwError = VmRESTSetDataLength(
                  ppResponse,
                  NULL
                  );
        BAIL_ON_VMREST_ERROR(dwError);
    }

    done = 0;
    index = 0;
    bytesLeft = resLength;

    if (bytesLeft >= MAX_IN_MEM_PAYLOAD_LEN)
    {
        dwError = 400;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    memset(buffer, '\0', 4097);

    while(done != 1)
    {
        nWrite = (bytesLeft > 4096) ? 4096: bytesLeft;
        memcpy(buffer, (AllData + index), nWrite);

        dwError = VmRESTSetData(
                  pRESTHandle,
                  ppResponse,
                  buffer,
                  nWrite,
                  &done
                  );
        BAIL_ON_VMREST_ERROR(dwError);
        index = index + nWrite;
        bytesLeft = bytesLeft - nWrite;
        memset(buffer, '\0', 4097);

    }

cleanup:
    return dwError;
error:
    goto cleanup;
}




uint32_t
VmHandleEchoData(
    PVMREST_HANDLE                   pRESTHandle,
    PREST_REQUEST                    pRequest,
    PREST_RESPONSE*                  ppResponse,
    uint32_t                         paramsCount
    )
{
    uint32_t                         dwError = 0;
    uint32_t                         done = 0;
    char                             AllData[MAX_IN_MEM_PAYLOAD_LEN] = {0};
    char                             buffer[4097] = {0};
    int                              nRead = 0;
    int                              nWrite = 0;
    uint32_t                         bytesLeft = 0;
    char                             size[10] = {0};    
    int                              resLength = 0;
    uint32_t                         index = 0;
   // char*                            key = NULL;
   // char*                            value = NULL;

    memset(buffer, '\0', 4097);
    memset(AllData, '\0', MAX_IN_MEM_PAYLOAD_LEN);
    memset(size, '\0', 10);

    done = 0;
    //write(1,"\nParams.....:", 14);
  /*  while (done < paramsCount)
    {
        dwError = VmRESTGetParamsByIndex(
                      pRequest,
                      paramsCount,
                      (done +1),
                      &key,
                      &value
                      );
        BAIL_ON_VMREST_ERROR(dwError);
        //write(1,"\nParams.....:", 14);
        write(1,key, strlen(key));
        write(1,":", 1);
        write(1,value, strlen(value));
        write(1,"\n", 1);
        if (key)
        {
            free(key);
            key = NULL;
        }
        if (value)
        {
            free(value);
            value = NULL;
        }
        done++;
    }
    done = 0;
    */


    while(done != 1)
    {
        dwError = VmRESTGetData(
                      pRESTHandle,
                      pRequest,
                      buffer,
                      &done
                      );
        BAIL_ON_VMREST_ERROR(dwError);

        nRead = strlen(buffer);        

        if (nRead > 0)
        {
            if ((index + nRead)< MAX_IN_MEM_PAYLOAD_LEN)
            {
                memcpy((AllData + index),buffer,nRead);
                index += nRead;
            }
            else
            {
                dwError = 500;
            }
            BAIL_ON_VMREST_ERROR(dwError);

        }
        memset(buffer, '\0', 4097);
        nRead = 0;
    }

    dwError = VmRESTSetSuccessResponse(
                  pRequest,
                  ppResponse
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    memset(buffer, '\0', 4097);

    resLength = strlen(AllData);
   
    if (resLength < 4096 )
    {
        dwError = VmRESTUtilsConvertInttoString(
                          resLength,
                          size);
        BAIL_ON_VMREST_ERROR(dwError);

        dwError = VmRESTSetDataLength(
                  ppResponse,
                  size
                  );
        BAIL_ON_VMREST_ERROR(dwError);
    }
    else
    {
        dwError = VmRESTSetDataLength(
                  ppResponse,
                  NULL
                  );
        BAIL_ON_VMREST_ERROR(dwError);
    }

    done = 0;
    index = 0;
    bytesLeft = resLength;

    if (bytesLeft >= MAX_IN_MEM_PAYLOAD_LEN)
    {
        dwError = 400;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    memset(buffer, '\0', 4097);

    while(done != 1)
    {
        nWrite = (bytesLeft > 4096) ? 4096: bytesLeft;
        memcpy(buffer, (AllData + index), nWrite); 

        dwError = VmRESTSetData(
                  pRESTHandle,
                  ppResponse,
                  buffer,
                  nWrite,
                  &done
                  );
        BAIL_ON_VMREST_ERROR(dwError);
        index = index + nWrite;
        bytesLeft = bytesLeft - nWrite;
        memset(buffer, '\0', 4097);
        
    }

cleanup:
    return dwError;
error:
    goto cleanup;
}

