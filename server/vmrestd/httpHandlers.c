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
#define TEST_SEND_CHUNK 0


uint32_t
VmRESTHandleHTTP_REQUEST(
    PREST_REQUEST                    pRequest,
    PREST_RESPONSE*                  ppResponse
    )
{
    uint32_t                         dwError = 0;
    char                             buffer[56];
    char*                            ptr = NULL;
    uint32_t                         temp = 0;
    uint32_t                         done = 0;

    memset(buffer, '\0', 56);

    BAIL_ON_VMREST_ERROR(dwError);


    dwError = VmRESTGetHttpMethod(pRequest, &ptr);
    write(1,"\nMethod: ", 9);
    write(1,ptr,5);

    dwError = VmRESTGetHttpURI(pRequest, &ptr);
    write(1,"\nURI: ", 6);
    write(1,ptr,56);

    dwError = VmRESTGetHttpVersion(pRequest, &ptr);
    write(1,"\nVer: ", 6);
    write(1,ptr,8);

    dwError = VmRESTGetHttpHeader(pRequest,"Connection", &ptr);
    write(1,"\nConnection: ", 12);
    write(1,ptr,11);

    dwError = VmRESTGetHttpHeader(pRequest,"Transfer-Encoding", &ptr);
    if ((ptr != NULL) && (strlen(ptr) > 0))
    {
        write(1,"\nTransfer-Encoding: ", 20);
        write(1,ptr,8);
    }
    else
    {
        dwError = VmRESTGetHttpHeader(pRequest,"Content-Length", &ptr);
        write(1,"\nContent-Length: ", 17);
        write(1,ptr,3);
    }

    write(1,"\nPayload: ", 9);
    /**** Get the payload ****/
    while(done != 1)
    {
        dwError = VmRESTGetHttpPayload(
                      pRequest,
                      buffer,
                      &done
                      );
        if (strlen(buffer) > 0)
        {
            write(1,buffer,strlen(buffer));
        }
        memset(buffer, '\0', 56);
    }

    /*** SET all HTTP response Headers before setting payload ****/

    dwError = VmRESTSetHttpHeader(ppResponse, "Kumar", "Kaushik");
    dwError = VmRESTSetHttpHeader(ppResponse, "Location", "United States");
    dwError = VmRESTSetHttpStatusCode(ppResponse, "200");
    dwError = VmRESTSetHttpStatusVersion(ppResponse,"HTTP/1.1");
    dwError = VmRESTSetHttpReasonPhrase(ppResponse,"OK");
    dwError = VmRESTSetHttpHeader(ppResponse, "Unix", "Linux");
    dwError = VmRESTSetHttpHeader(ppResponse, "Connection", "close");

    /**** Set the payload ****/
    if (TEST_SEND_CHUNK == 1)
    {
        dwError = VmRESTSetHttpHeader(
                      ppResponse,
                      "Transfer-Encoding",
                      "chunked"
                      );
        dwError = VmRESTSetHttpPayload(ppResponse,
                  "This is response payload with length 39",39,&temp);
        dwError = VmRESTSetHttpPayload(ppResponse, "My name is Kumar",16, &temp );
        dwError = VmRESTSetHttpPayload(ppResponse, "Kaush",5, &temp );
        dwError = VmRESTSetHttpPayload(ppResponse, "ik  ",2, &temp );
        dwError = VmRESTSetHttpPayload(ppResponse, "0",0, &temp );
    }
    else if(TEST_SEND_CHUNK == 0)
    {
        dwError = VmRESTSetHttpHeader(
                      ppResponse,
                      "Content-Length",
                      "39"
                      );
        dwError = VmRESTSetHttpPayload(ppResponse,
                  "This is response payload with length 39",39,&temp);

    }
    write(1, "\nThis is App CB for Method", 26);

cleanup:

    return dwError;

error:

    goto cleanup;
}

