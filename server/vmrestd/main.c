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

int main(int argc, char *argv[])
{
    uint32_t                         dwError = 0;
    PREST_CONF                       pConfig = NULL;

    /**** Create and populate the config  struct ****/

    char* sslCert = "/root/mycert.pem";
    char* sslKey  = "/root/mycert.pem";
    char* port = "443";
    char* debugLogFile = "/tmp/restServer.log";
    char* clientCnt = "5";
    char* workerThCnt = "5";

    pConfig = (PREST_CONF)malloc(sizeof(REST_CONF));

    pConfig->pSSLCertificate = sslCert; 
    pConfig->pSSLKey = sslKey;
    pConfig->pServerPort = port;
    pConfig->pDebugLogFile = debugLogFile;
    pConfig->pClientCount = clientCnt;
    pConfig->pMaxWorkerThread = workerThCnt;

    /***********************************
    *  Call VmRESTInit with pConfig or 
    *  with NULL for default config from 
    *  file /root/restConfig.txt 
    ************************************/
    
    dwError = VmRESTInit(
                  NULL
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTRegisterHandler(
                  NULL,
                  gpVmRestHandlers,
                  NULL
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTStart(
                  );
    BAIL_ON_VMREST_ERROR(dwError);
 
    sleep(10);
    
    dwError = VmRESTStop(
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    VmRESTShutdown(
        );

cleanup:

    return dwError;

error:

    goto cleanup;
}
