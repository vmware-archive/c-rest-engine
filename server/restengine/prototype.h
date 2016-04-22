
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

/***************** Thread.c ***********************/
uint32_t
VmRestSpawnThreads(
    PFN_VMREST_THR_ROUTINE           pThrRoutine,
    PVMREST_THREAD*                  ppThreadpool,
    uint32_t                         maxWorkerThread
    );

void *
VmRestWorkerThread(
    void*                            pArgs
    );

void
VmRestFreeThreadpool(
    PVMREST_THREAD                   pThreadpool,
    uint32_t                         threadcount
    );


/***************** httpProtocolHead.c *************/

uint32_t
VmRESTHTTPGetReqMethod(
    char*                            line,
    uint32_t                         lineLen,
    char*                            result,
    uint32_t*                        resStatus
    );

uint32_t
VmRESTHTTPGetReqURI(
    char*                            line,
    uint32_t                         lineLen,
    char*                            result,
    uint32_t*                        resStatus
    );

uint32_t
VmRESTHTTPGetReqVersion(
    char*                            line,
    uint32_t                         lineLen,
    char*                            result,
    uint32_t*                        resStatus
    );

uint32_t
VmRESTHTTPPopulateHeader(
    char*                            line,
    uint32_t                         lineLen,
    PVM_REST_HTTP_REQUEST_PACKET     pReqPacket,
    uint32_t*                        resStatus
    );

uint32_t
VmRESTParseHTTPReqLine(
    uint32_t                         lineNo,
    char*                            line,
    uint32_t                         lineLen,
    PVM_REST_HTTP_REQUEST_PACKET     pReqPacket,
    uint32_t*                        resStatus
    );

uint32_t
VmRESTParseAndPopulateRawHTTPMessage(
    char*                            buffer,
    uint32_t                         packetLen,
    PVM_REST_HTTP_REQUEST_PACKET     pReqPacket,
    uint32_t*                        resStatus
    );

uint32_t
VmRESTAddAllHeaderInResponseStream(
    PVM_REST_HTTP_RESPONSE_PACKET    pResPacket,
    char*                            buffer,
    uint32_t*                        bytes
    );

uint32_t
VMRESTWriteStatusLineInResponseStream(
    PVM_REST_HTTP_RESPONSE_PACKET    pResPacket,
    char*                            buffer,
    uint32_t*                        bytes
    );

uint32_t
VMRESTWriteMessageBodyInResponseStream(
    PVM_REST_HTTP_RESPONSE_PACKET    pResPacket,
    char*                            buffer,
    uint32_t*                        bytes
    );

uint32_t
VmRESTSendResponsePacket(
    PVM_REST_HTTP_RESPONSE_PACKET*   ppResPacket
    );

uint32_t
VmRESTTriggerAppCb(
    PVM_REST_HTTP_REQUEST_PACKET     pRequest,
    PVM_REST_HTTP_RESPONSE_PACKET*   ppResponse
    );

uint32_t
VmRESTTestHTTPResponse(
    PVM_REST_HTTP_REQUEST_PACKET     pReqPacket,
    PVM_REST_HTTP_RESPONSE_PACKET    pResPacket
    );

/***************** httpAllocStruct.c  *************/

uint32_t
VmRESTAllocateHTTPRequestPacket(
    PVM_REST_HTTP_REQUEST_PACKET*    ppReqPacket
    );

void
VmRESTFreeHTTPRequestPacket(
    PVM_REST_HTTP_REQUEST_PACKET*    ppReqPacket
    );

uint32_t
VmRESTAllocateHTTPResponsePacket(
    PVM_REST_HTTP_RESPONSE_PACKET*   ppResPacket
    );

void
VmRESTFreeHTTPResponsePacket(
    PVM_REST_HTTP_RESPONSE_PACKET*   ppResPacket
    );


/***************** httpUtilsInternal.c ************/

uint32_t
VmRESTMapHeaderToEnum(
    PCSTR                            header,
    uint32_t*                        result,
    uint32_t*                        resStatus
    );

uint32_t
VmRESTMapMethodToEnum(
    char*                            method,
    uint32_t*                        result
    );

uint32_t
VmRESTMapStatusCodeToEnumAndReasonPhrase(
    char*                            statusCode,
    uint32_t*                        result,
    char*                            reasonPhrase
    );

uint32_t
VmRESTGetHttpResponseHeader(
    PVM_REST_HTTP_RESPONSE_PACKET    pResponse,
    char*                            header,
    char*                            response
    );

uint32_t
VmRESTSetHttpRequestHeader(
    PVM_REST_HTTP_REQUEST_PACKET     pRequest,
    char*                            header,
    char*                            value,
    uint32_t*                        resStatus
    );

uint32_t
VmRESTParseAndPopulateConfigFile(
    char*                            configFile,
    VM_REST_CONFIG**                 ppRESTConfig
    );

void
VmRESTFreeConfigFileStruct(
    VM_REST_CONFIG*                  pRESTConfig
    );

uint32_t
VmRESTValidateConfig(
    PVM_REST_CONFIG                  pRESTConfig
    );

uint32_t
VmRESTCopyString(
    char*                            src,
    char*                            des
    );

uint32_t
VmRESTCopyConfig(
    PREST_CONF                       pConfig,
    PVM_REST_CONFIG*                 ppRESTConfig
    );
