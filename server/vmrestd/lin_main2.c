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
#include <getopt.h>
#include <sys/types.h>
#include <sys/inotify.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <stdio.h>

PVMREST_HANDLER  gpRESTHandler = NULL;
PVMREST_HANDLER  gpRESTHandler1 = NULL;

uint32_t
VmRESTUtilsConvertInttoString(
    int                              num,
    char*                            str
    );

REST_PROCESSOR gVmRestHandlers =
{
.pfnHandleRequest = NULL,
.pfnHandleCreate =  &VmHandleEchoData,
.pfnHandleRead = &VmHandleEchoData,
.pfnHandleUpdate = &VmHandleEchoData,
.pfnHandleDelete = &VmHandleEchoData,
.pfnHandleOthers =  &VmHandleEchoData
};

REST_PROCESSOR gVmRestHandlers1 =
{
.pfnHandleRequest = NULL,
.pfnHandleCreate =  &VmHandleEchoData1,
.pfnHandleRead = &VmHandleEchoData1,
.pfnHandleUpdate = &VmHandleEchoData1,
.pfnHandleDelete = &VmHandleEchoData1,
.pfnHandleOthers =  &VmHandleEchoData1
};





int main()
{
    uint32_t                         dwError = 0;
   // uint32_t                         cnt = 0;



    dwError = VmRESTInit(NULL,"/tmp/restconfig.txt", &gpRESTHandler);
    dwError = VmRESTInit(NULL,"/tmp/restconfig2.txt", &gpRESTHandler1);

    VmRESTRegisterHandler(gpRESTHandler, "/v1/pkg", &gVmRestHandlers, NULL);
    VmRESTRegisterHandler(gpRESTHandler1, "/v1/blah", &gVmRestHandlers1, NULL);

    VmRESTStart(gpRESTHandler);
    VmRESTStart(gpRESTHandler1);


    


    while(1) //cnt < 20)
    {
        sleep(1);
       // cnt++;
    }

    dwError = VmRESTStop(gpRESTHandler);
    dwError = VmRESTStop(gpRESTHandler1);

    //dwError = VmRESTUnRegisterHandler(gpRESTHandler,"/v1/pkg/*");

    VmRESTShutdown(gpRESTHandler);
    VmRESTShutdown(gpRESTHandler1);


return dwError;

}

uint32_t
VmHandleEchoData1(
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
                      gpRESTHandler1,
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
                  gpRESTHandler1,
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
                      gpRESTHandler,
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
                  gpRESTHandler,
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


#if 0

uint32_t
VmHandleEchoData(
    PREST_REQUEST                    pRequest,
    PREST_RESPONSE*                  ppResponse,
    uint32_t                         paramsCount
    )
{
    uint32_t                         dwError = 0;
    char                             buffer[4097] = {0};
    char                             buffer1[4097] = {0};
    uint32_t                         done = 0;
    char*                            res = NULL;
    FILE*                            fp = NULL;
    int                              bytesRead = 0;
    char                             size[10] = {0};
//    uint32_t                         wcCount = 0;
//    char*                            wildcard = NULL;
    char                             rcvdDataFilepath[MAX_DIRECTORY_LENGTH + 80];

    

    memset(buffer, '\0', 4097);
    memset(size, '\0', 10);

    dwError = VmRESTGetHttpMethod(
                  pRequest,
                  &res);
    BAIL_ON_VMREST_ERROR(dwError);

    if (res != NULL)
    {
        free(res);
        res= NULL;
    }
/*    dwError = VmRESTGetWildCardCount(
                  pRequest,
                  &wcCount);
    BAIL_ON_VMREST_ERROR(dwError);

    printf("WC COUNT %u", wcCount);

    dwError = VmRESTGetWildCardByIndex(
              pRequest,
              1,
              &wildcard);
    BAIL_ON_VMREST_ERROR(dwError);

    printf("WC String 1 = %s", wildcard);
    write(1,wildcard,strlen(wildcard));

    wildcard = NULL;

    dwError = VmRESTGetWildCardByIndex(
              pRequest,
              2,
              &wildcard);
    BAIL_ON_VMREST_ERROR(dwError);

    write(1,wildcard,strlen(wildcard));
    printf("WC String 2 = %s", wildcard);
*/
    strncpy(rcvdDataFilepath, EchoDataVroot, MAX_DIRECTORY_LENGTH-1);
    strcat(rcvdDataFilepath, "/rcvdData");
    fp  = fopen(rcvdDataFilepath, "w");
    if (fp == NULL)
    {
        BAIL_ON_VMREST_ERROR(60);
    }

    /**** Read the data if present ****/

    while(done != 1)
    {
        dwError = VmRESTGetData(
                      pRequest,
                      buffer,
                      &done
                      );
        BAIL_ON_VMREST_ERROR(dwError);

        if (strlen(buffer) > 0)
        {
            fwrite(buffer, strlen(buffer), 1, fp);

        }
        memset(buffer, '\0', 4097);
    }
    fclose(fp);
    fp = NULL;
    done = 0;

    /*** Return hardcoded tdnf version v1.0.5 ****/

    dwError = VmRESTSetSuccessResponse(
                  pRequest,
                  ppResponse
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    fp  = fopen(rcvdDataFilepath, "r");
    if (fp == NULL)
    {
        BAIL_ON_VMREST_ERROR(61);
    }

    memset(buffer1, '\0', 4097);
    bytesRead = fread(buffer1, 1, 4096, fp);
   
    if (bytesRead < 4096 )
    {
        // check this
        buffer1[4096] = '\0';
        dwError = VmRESTUtilsConvertInttoString(
                          strlen(buffer1),
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
  
    while(done != 1)
    {

        dwError = VmRESTSetData(
                  ppResponse,
                  buffer1,
                  bytesRead,
                  &done
                  );
        BAIL_ON_VMREST_ERROR(dwError);
        memset(buffer1, '\0', 4097);
        bytesRead = fread(buffer1, 1, 4096, fp);
        
    }
    fclose(fp);
    fp = NULL;


cleanup:
    return dwError;
error:
    if (fp)
    {
        fclose(fp);
    }
    goto cleanup;
}
#endif
