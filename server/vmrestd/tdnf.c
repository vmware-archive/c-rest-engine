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
#include <stdio.h>

char   EchoDataVroot[MAX_DIRECTORY_LENGTH] = "/tmp";

uint32_t
VmRESTUtilsConvertInttoString(
    int                              num,
    char*                            str
    );

/****************************************
*  
* TDNF Version related handler
*
****************************************/

/*** Get the tdnf version ****/
uint32_t
VmHandleTDNFVersionGet(
    PREST_REQUEST                    pRequest,
    PREST_RESPONSE*                  ppResponse,
    uint32_t                         paramsCount
    )
{
    uint32_t                         dwError = 0;
    char                             buffer[MAX_DATA_LEN + 1] = {0};
    uint32_t                         done = 0;

    memset(buffer, '\0', (MAX_DATA_LEN + 1));

  //  write(1,"\nPayload for tdnf version GET: ", 31);

    /**** Read the data if present ****/

    while(done != 1)
    {
        dwError = VmRESTGetData(
                      pRequest,
                      buffer,
                      &done
                      );
        if (strlen(buffer) > 0)
        {
            //write(1,buffer,strlen(buffer));
        }
        memset(buffer, '\0', (MAX_DATA_LEN +1));
    }
    BAIL_ON_VMREST_ERROR(dwError);

    done = 0;
    
    /*** Return hardcoded tdnf version v1.0.5 ****/

    dwError = VmRESTSetSuccessResponse(
                  pRequest,
                  ppResponse
                  );
    BAIL_ON_VMREST_ERROR(dwError);
    
    dwError = VmRESTSetDataLength(
                  ppResponse,
                  "6"
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTSetData(
                  ppResponse,
                  "v1.0.5",
                  6,
                  &done
                  );
    BAIL_ON_VMREST_ERROR(dwError);


cleanup:
    return dwError;
error:
    goto cleanup;
}

/**** Set the TDNF version ****/
uint32_t
VmHandleTDNFVersionSet(
    PREST_REQUEST                    pRequest,
    PREST_RESPONSE*                  ppResponse,
    uint32_t                         paramsCount
    )
{
    uint32_t                         dwError = 0;
    char                             buffer[MAX_DATA_LEN] = {0};
    uint32_t                         done = 0;
    char*                            key = NULL;
    char*                            value = NULL;

    memset(buffer, '\0', MAX_DATA_LEN);

  //  write(1,"\nPayload for tdnf version SET: ", 31);

    /**** Get the payload ****/
    while(done != 1)
    {
        dwError = VmRESTGetData(
                      pRequest,
                      buffer,
                      &done
                      );
        if (strlen(buffer) > 0)
        {
//            write(1,buffer,strlen(buffer));
        }
        memset(buffer, '\0', MAX_DATA_LEN);
    }
    BAIL_ON_VMREST_ERROR(dwError);

    done = 0;

    /*** new version should be present as param in URI as newVersion=v1.0.6 ****/

    if (paramsCount != 1)
    {
        
		//write(1, "ERROR - Invalid Param count",26);
        dwError  = 100;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTGetParamsByIndex(
                  pRequest,
                  paramsCount,
                  1,
                  &key,
                  &value
                  );
    BAIL_ON_VMREST_ERROR(dwError);

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


    /**** Example Call the version set api  ****/

    //write(1,"\nNew Version:", 13);
   // write(1,value, 6);

    dwError = VmRESTSetSuccessResponse(
                  pRequest,
                  ppResponse
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTSetDataLength(
                  ppResponse,
                  "38"
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTSetData(
                  ppResponse,
                  "TDNF Version v1.0.6 is Set SuccessFully",
                  38,
                  &done
                  );
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:
    return dwError;
error:
    goto cleanup;
}

/****************************************
*  
* Package related Handler 
*
****************************************/

/**** Modify any package related data ****/
uint32_t
VmHandlePackageOTHERS(
    PREST_REQUEST                    pRequest,
    PREST_RESPONSE*                  ppResponse,
    uint32_t                         paramsCount
    )
{
    uint32_t                         dwError = 0;
    uint32_t                         done = 0;
    char*                            httpMethod = NULL;

    //write(1,"\nPackage OTHERS operation:",26);

    dwError = VmRESTGetHttpMethod(
                  pRequest,
                  &httpMethod
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    //write(1,httpMethod, strlen(httpMethod));

    if (strcmp(httpMethod, "OPTIONS") == 0)
    {
        dwError = VmRESTSetHttpHeader(
                      ppResponse,
                      "Access-Control-Allow-Headers",
                      "Content-Type"
                      );
        BAIL_ON_VMREST_ERROR(dwError);
        dwError = VmRESTSetHttpHeader(
                      ppResponse,
                      "Access-Control-Allow-Methods",
                      "POST, GET, OPTIONS, PUT"
                      );
        BAIL_ON_VMREST_ERROR(dwError);

        dwError = VmRESTSetHttpHeader(
                      ppResponse,
                      "Access-Control-Allow-Origin",
                      "*"
                      );
        BAIL_ON_VMREST_ERROR(dwError);
    }


    dwError = VmRESTSetSuccessResponse(
                  pRequest,
                  ppResponse
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTSetDataLength(
                  ppResponse,
                  "0"
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTSetData(
                  ppResponse,
                  "",
                  0,
                  &done
                  );
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:
    return dwError;
error:
    goto cleanup;
}


/**** Read any package related data ****/
uint32_t
VmHandlePackageRead(
    PREST_REQUEST                    pRequest,
    PREST_RESPONSE*                  ppResponse,
    uint32_t                         paramsCount
    )
{
    uint32_t                         dwError = 0;
    char                             buffer[MAX_DATA_LEN] = {0};
    uint32_t                         done = 0;
    char*                            key = NULL;
    char*                            value = NULL;

    memset(buffer, '\0', MAX_DATA_LEN);

    //write(1,"\nPayload for Package read operation: ", 36);

    /**** Get the payload ****/
    while(done != 1)
    {
        dwError = VmRESTGetData(
                      pRequest,
                      buffer,
                      &done
                      );
        if (strlen(buffer) > 0)
        {
            printf("Data read %s", buffer);
            //write(1,buffer,strlen(buffer));
        }
        memset(buffer, '\0', MAX_DATA_LEN);
    }
    BAIL_ON_VMREST_ERROR(dwError);

    done = 0;

    if (paramsCount < 1)
    {
        //write(1, "ERROR - Invalid Param count",26 );
        dwError  = 100;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    
    while (done < paramsCount)
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
      //  write(1,key, strlen(key));
      //  write(1,value, 4);

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

    dwError = VmRESTSetSuccessResponse(
                  pRequest,
                  ppResponse
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTSetDataLength(
                  ppResponse,
                  "43"
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTSetData(
                  ppResponse,
                  "Read request on package served Successfully",
                  43,
                  &done
                  );
    BAIL_ON_VMREST_ERROR(dwError);



    printf("\nUpdate\n");

    BAIL_ON_VMREST_ERROR(dwError);

cleanup:
    return dwError;
error:
    goto cleanup;
}

/**** Write any package related data ****/
uint32_t
VmHandlePackageWrite(
    PREST_REQUEST                    pRequest,
    PREST_RESPONSE*                  ppResponse,
    uint32_t                         paramsCount
    )
{
    uint32_t                         dwError = 0;
    uint32_t                         done = 0;

   // write(1,"\nPackage Write operation: ",26);

    dwError = VmRESTSetSuccessResponse(
                  pRequest,
                  ppResponse
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTSetDataLength(
                  ppResponse,
                  "0"
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTSetData(
                  ppResponse,
                  "",
                  0,
                  &done
                  );
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:
    return dwError;
error:
    goto cleanup;
}

/**** Modify any package related data ****/
uint32_t
VmHandlePackageUpdate(
    PREST_REQUEST                    pRequest,
    PREST_RESPONSE*                  ppResponse,
    uint32_t                         paramsCount
    )
{
    uint32_t                         dwError = 0;
    uint32_t                         done = 0;

    //write(1,"\nPackage UPDATE operation:",26);

    dwError = VmRESTSetSuccessResponse(
                  pRequest,
                  ppResponse
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTSetDataLength(
                  ppResponse,
                  "0"
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTSetData(
                  ppResponse,
                  "",
                  0,
                  &done
                  );
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VmHandlePackageDelete(
    PREST_REQUEST                    pRequest,
    PREST_RESPONSE*                  ppResponse,
    uint32_t                         paramsCount
    )
{
    uint32_t                         dwError = 0;
    uint32_t                         done = 0;

    //write(1,"\nPackage DELETE operation:",26);

    dwError = VmRESTSetSuccessResponse(
                  pRequest,
                  ppResponse
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTSetDataLength(
                  ppResponse,
                  "0"
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTSetData(
                  ppResponse,
                  "",
                  0,
                  &done
                  );
    BAIL_ON_VMREST_ERROR(dwError);

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
