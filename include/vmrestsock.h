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
VmRestTransportInit(
    char*                            port
    );

uint32_t
VmRestTransportStart(
    char*                            port,
    char*                            sslCertificate,
    char*                            sslKey,
    uint32_t                         clientCount
    );

void
VmRESTTransportStop(
    void
    );
void
VmRESTTransportShutdown(
    void
    );

uint32_t
VmSockPosixHandleEventsFromQueue(
    void
    );

uint32_t
VmsockPosixReadDataAtOnce(
    uint32_t                         clientIndex
    );

uint32_t
VmsockPosixWriteDataAtOnce(
    uint32_t                         clientIndex,
    char*                            buffer,
    uint32_t                         bytes
    );

uint32_t
VmRESTProcessIncomingData(
    char*                            buffer,
    uint32_t                         byteRead,
    uint32_t                         clientIndex
    );

uint32_t
VmSockPosixCloseConnection(
    uint32_t                         clientIndex
    );

