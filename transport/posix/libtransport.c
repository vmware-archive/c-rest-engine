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
    char *port
    )
{
    uint32_t dwError = 0;
    uint32_t len = 0;   
    if (port == NULL)
    {
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_POSIX_SOCK_ERROR(dwError);

    }
    len = strlen(port);
    if ( len > MAX_PORT_LEN)
    {
        dwError = ERROR_NOT_SUPPORTED;
        BAIL_ON_POSIX_SOCK_ERROR(dwError);
    }
   
    strcpy(gServerSocketInfo.port, port); 

    dwError = VmSockPosixCreateServerSocket();
    BAIL_ON_POSIX_SOCK_ERROR(dwError);

cleanup:
    return dwError;


error:
    goto cleanup;
}
