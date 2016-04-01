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

uint32_t
VmSockPosixCreateServerSocket(
    char*                            sslCertificate,
    char*                            sslKey,
    char*                            port,
    uint32_t                         clientCount
    );

void
VmSockPosixDestroyServerSocket(
   );

void *
VmSockPosixServerListenThread(
    void*
    );

uint32_t
VmSockPosixSetSocketNonBlocking(
    int                              server_fd
    );

uint32_t VmsockPosixAcceptNewConnection(
    int                              server_fd
    );

uint32_t VmsockPosixReadDataAtOnce(
    SSL*                             ssl
    );

/****** utils.c **********/

uint32_t
VmRESTInsertElement(
    PVM_EVENT_DATA                   data,
    uint32_t                         flag,
    QUEUE*                           queue
    );

EVENT_NODE*
VmRESTUtilsRemoveElement(
    QUEUE*                           queue
    );

uint32_t
VmRestUtilsInitQueue(
    QUEUE*                           queue
    );

void
VmRESTUtilsDestroyQueue(
    QUEUE*                           queue
    );


/****** globals.c **********/

uint32_t
VmInitGlobalServerSocket(
    char*                            port
    );

void
VmShutdownGlobalServerSocket(
    void
    );

uint32_t
VmRESTInsertClientFromGlobal(
    SSL*                             ssl,
    int                              fd,
    uint32_t*                        index
    );

uint32_t
VmRESTRemoveClientFromGlobal(
    uint32_t                         index
    );

