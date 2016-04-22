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
VmRestTransportInit(
    char*                            port
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;

    if (port == NULL)
    {
        VMREST_LOG_DEBUG("VmRestTransportInit(): Invalid params");
        dwError =  ERROR_TRANSPORT_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmInitGlobalServerSocket(
                  port
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmSockPosixCreateServerSocket(
                 );
    BAIL_ON_VMREST_ERROR(dwError);
   
    gServerSocketInfo.ServerAlive = 1;

cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
VmRestTransportStart(
    char*                            port,
    char*                            sslCertificate,
    char*                            sslKey,
    uint32_t                         clientCount
    )
{

    uint32_t                         dwError = REST_ENGINE_SUCCESS;

    if (port == NULL)
    {
        VMREST_LOG_DEBUG("VmRestTransportStart(): Invalid params");
        dwError =  ERROR_TRANSPORT_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    if (strcmp(port, "80") == 0)
    {
        gServerSocketInfo.isSecure = 0;
        sslCertificate = NULL;
        sslKey = NULL;
    }
    else if(strcmp(port, "443") == 0)
    {
        if (sslCertificate == NULL || sslKey == NULL)
        {
            VMREST_LOG_DEBUG("VmRestTransportStart(): Invalid SSL params");
            dwError =  ERROR_TRANSPORT_INVALID_PARAMS;
        }
        gServerSocketInfo.isSecure = 1;
    }
    else
    {
        VMREST_LOG_DEBUG("VmRestTransportStart(): Invalid port number");
        dwError =  ERROR_TRANSPORT_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmSockPosixStartServerSocket(
                  sslCertificate,
                  sslKey,
                  port,
                  clientCount
                  );
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:
    return dwError;
error:
    goto cleanup;
}

void
VmRESTTransportStop(
    void
    )
{
    /**** Mark all worker thread for cleanup ****/
    gServerSocketInfo.ServerAlive = 0;
    pthread_cond_broadcast(&(pQueue->signal));

    VmSockPosixStopServerSocket(
        );

    VmShutdownGlobalServerSocket(
        );
}

void
VmRESTTransportShutdown(
    void
    )
{
    VmSockPosixDestroyServerSocket(
        );
}

