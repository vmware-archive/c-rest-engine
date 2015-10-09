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
    void
    );


uint32_t 
VmSockPosixSetSocketNonBlocking(
    int server_fd
    );

uint32_t 
insert_element(
    int fd,
    uint32_t flag, 
    QUEUE* q
    );

EVENT_NODE* remove_element(QUEUE *q);

uint32_t init_queue(QUEUE *q);

uint32_t VmSockPosixHandleEventsFromQueue(
    QUEUE *q
    );

uint32_t VmsockPosixAcceptNewConnection(
    int server_fd
    );

uint32_t VmsockPosixReadDataAtOnce(
    int fd
    );



