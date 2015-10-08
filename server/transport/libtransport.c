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
    char *addr,
    char *port
    )
{
    uint32_t ret = 0;
    uint32_t portlen = 0;
    uint32_t addrlen = 0;
    if (addr == NULL || port == NULL) 
    {
        ret = ERROR_NOT_SUPPORTED;
        BAIL_ON_POSIX_SOCK_ERROR(ret);        
    }
    portlen = strlen(port);
    addrlen = strlen(addr);
    /* populate the global server structure for keeping address and port information of server */
    
    /* TODO:  If addr information is not passed, make default as localhost */

    strncpy(gServerSocketInfo.address, addr, addrlen);

    gServerSocketInfo.address[addrlen] = '\0';
    gServerSocketInfo.port[portlen] = '\0';

    strncpy(gServerSocketInfo.port, port, portlen);

    ret = VmSockPosixCreateServerSocket();
cleanup:
    return ret;


error:
    goto cleanup;
}
