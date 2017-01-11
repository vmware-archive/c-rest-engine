
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
VmRESTParseAndPopulateHTTPHeaders(
    char*                            buffer,
    uint32_t                         packetLen,
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
VMRESTWriteChunkedMessageInResponseStream(
    char*                            src,
    uint32_t                         srcSize,
    char*                            buffer,
    uint32_t*                        bytes
);

uint32_t
VmRESTSendHeader(
    PVM_REST_HTTP_RESPONSE_PACKET*   ppResPacket
    );

uint32_t
VmRESTSendChunkedPayload(
    PVM_REST_HTTP_RESPONSE_PACKET*   ppResPacket,
    uint32_t                         dataLen
    );

uint32_t
VmRESTSendHeaderAndPayload(
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

uint32_t
VmRESTCloseClient(
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

uint32_t
VmRESTAllocateEndPoint(
     PREST_ENDPOINT*                 ppEndPoint
     );

void
VmRESTFreeEndPoint(
    PREST_ENDPOINT                   pEndPoint
    );

/***************** httpUtilsInternal.c ************/

uint32_t
VmRESTMapHeaderToEnum(
    char const*                      header,
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
    char**                           response
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

uint32_t
VmRESTSetHTTPMiscHeader(
    PMISC_HEADER_QUEUE               miscHeaderQueue,
    char const*                      header,
    char*                            value
    );

uint32_t
VmRESTRemoveAllHTTPMiscHeader(
    PMISC_HEADER_QUEUE               miscHeaderQueue
    );

uint32_t
VmRESTGetHTTPMiscHeader(
    PMISC_HEADER_QUEUE               miscHeaderQueue,
    char const*                      header,
    char**                           ppResponse
    );

uint32_t
VmRESTGetChunkSize(
    char*                            lineStart,
    uint32_t*                        skipBytes,
    uint32_t*                        chunkSize
    );

uint32_t
VmRESTCopyDataWithoutCRLF(
    uint32_t                         maxBytes,
    char*                            src,
    char*                            des,
    uint32_t*                        actualBytes
    );

/***************** restProtocolHead.c  ************/

uint32_t
VmRestEngineHandler(
    PREST_REQUEST                    pRequest,
    PREST_RESPONSE*                  ppResponse
    );

uint32_t
VmRestEngineInitEndPointRegistration(
    );

void
VmRestEngineShutdownEndPointRegistration(
    );

uint32_t
VmRestEngineAddEndpoint(
    char*                            pEndPointURI,
    PREST_PROCESSOR                  pHandler
    );

uint32_t
VmRestEngineRemoveEndpoint(
    char*                            pEndPointURI
    );

uint32_t
VmRestEngineGetEndPoint(
    char*                            pEndPointURI,
    PREST_ENDPOINT*                  ppEndPoint
    );

uint32_t
VmRestGetEndPointURIfromRequestURI(
    char*                            pRequestURI,
    char*                            endPointURI
    );

uint32_t
VmRestGetParamsCountInReqURI(
    char*                            pRequestURI,
    uint32_t*                        paramCount
    );

uint32_t
VmRestParseParams(
    char*                            pRequestURI,
    uint32_t                         paramsCount,
    PREST_REQUEST                    pRequest
    );

uint32_t
VmRESTMatchEndPointURI(
    char*                            pattern,
    char*                            pEndPointURI
    );

uint32_t
VmRESTGetPreSlashIndex(
    char*                            patternURI,
    uint32_t                         wildCardIndex,
    uint32_t*                        preSlashIndex
    );

uint32_t
VmRESTCopyWCStringByIndex(
    char*                            requestEndPointURI,
    char*                            des,
    uint32_t                         wcIndex,
    uint32_t                         totalWC,
    uint32_t                         preSlashIndex
    );

/***************** httpMain.c  ************/

uint32_t
VmHTTPInit(
    PREST_CONF                       pConfig,
    char*                            file
    );

uint32_t
VmHTTPStart(
    void
    );

uint32_t
VmHTTPRegisterHandler(
    PREST_PROCESSOR                  pHandler
    );

uint32_t
VmHTTPUnRegisterHandler(
    void
    );

uint32_t
VmHTTPStop(
    void
    );

void
VmHTTPShutdown(
    void
    );

/********************* httpValidate.c *******************/

uint32_t
VmRESTRemovePreSpace(
    char*                            src,
    char*                            dest
    );

uint32_t
VmRESTRemovePostSpace(
    char*                            src,
    char*                            dest
    );

uint32_t
VmRESTConverUpperToLower(
    char*                            src,
    char*                            dest
    );

uint32_t
VmRESTValidateHTTPVersion(
    PVM_REST_HTTP_REQUEST_PACKET     pRequest,
    uint32_t*                        result,
    uint32_t*                        err
    );

uint32_t
VmRESTValidateHTTPRequestURI(
    PVM_REST_HTTP_REQUEST_PACKET     pRequest,
    uint32_t*                        result,
    uint32_t*                        err
    );

uint32_t
VmRESTValidateHTTPContentType(
    PVM_REST_HTTP_REQUEST_PACKET     pRequest,
    uint32_t*                        result,
    uint32_t*                        err
    );


