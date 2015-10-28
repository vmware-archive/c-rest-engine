
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
VmRestSpawnThreads(
    PFN_VMREST_THR_ROUTINE pThrRoutine,
    PVMREST_THREAD*        ppThreadpool,
    uint32_t*              pThrCount
    );

void *
VmRestWorkerThread(
    void* pArgs
    );

void
VmRestFreeThreadpool(
    PVMREST_THREAD pThreadpool,
    uint32_t       threadcount
    );


/* Add all Http internal exposed routines here */

uint32_t
VmRESTHTTPGetmethod(
    char*    line,
    uint32_t lineLen,
    char*    result
    );

uint32_t
VmRESTHTTPGetReqURI(
    char*    line,
    uint32_t lineLen,
    char*    result
    );

uint32_t
VmRESTHTTPGetReqVersion(
    char*    line,
    uint32_t lineLen,
    char*    result
    );

uint32_t
VmRESTHTTPPopulateHeader(
    char*                        line,
    uint32_t                     lineLen,
    PVM_REST_HTTP_REQUEST_PACKET pReqPacket
    );

uint32_t
VmRESTParseHTTPReqLine(
    uint32_t                      lineNo,
    char*                         line,
    uint32_t                      lineLen,
    PVM_REST_HTTP_REQUEST_PACKET  pReqPacket
    );

uint32_t
VmRESTParseAndPopulateRawHTTPMessage(
    char*                        buffer,
    uint32_t                     packetLen,
    PVM_REST_HTTP_REQUEST_PACKET pReqPacket
    );

uint32_t
VmRESTCreateHTTPResponseMessage(
    PVM_REST_HTTP_RESPONSE_PACKET* pResPacket
    );

/* This will be removed once cli module for modular testing is implemented */
uint32_t
VmRESTTestHTTPParser(
    void
    );

uint32_t
VmRESTAllocateHTTPRequestPacket(
    PVM_REST_HTTP_REQUEST_PACKET* ppReqPacket
    );

void
VmRESTFreeHTTPRequestPacket(
    PVM_REST_HTTP_REQUEST_PACKET*   ppReqPacket
    );

uint32_t
VmRESTAllocateHTTPResponsePacket(
    PVM_REST_HTTP_RESPONSE_PACKET* ppResPacket
    );

void
VmRESTFreeHTTPResponsePacket(
    PVM_REST_HTTP_RESPONSE_PACKET* ppResPacket
    );




void
VmRESTFreeHTTPRequestPacket(
    PVM_REST_HTTP_REQUEST_PACKET*   ppReqPacket
    );

void
VmRESTFreeHTTPResponsePacket(
    PVM_REST_HTTP_RESPONSE_PACKET* ppResPacket
    );

/* httpUtilsInternal.c */

uint32_t
VmRESTMapHeaderToEnum(
    char*             header,
    uint32_t*         result
    );

uint32_t
VmRESTMapMethodToEnum(
    char*             method,
    uint32_t*         result
    );

uint32_t
VmRESTMapStatusCodeToEnumAndReasonPhrase(
    char*             statusCode,
    uint32_t*         result,
    char*             reasonPhrase
    );

uint32_t
VmRESTGetHttpResponseHeader(
    PVM_REST_HTTP_RESPONSE_PACKET    pResponse,
    char*                           header,
    char*                           response
    );










