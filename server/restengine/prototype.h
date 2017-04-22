/* C-REST-Engine
*
* Copyright (c) 2017 VMware, Inc. All Rights Reserved. 
*
* This product is licensed to you under the Apache 2.0 license (the "License").
* You may not use this product except in compliance with the Apache 2.0 License.  
*
* This product may include a number of subcomponents with separate copyright 
* notices and license terms. Your use of these subcomponents is subject to the 
* terms and conditions of the subcomponent's license, as noted in the LICENSE file. 
*
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
    PVMREST_HANDLE                   pRESTHandle,
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
    PVMREST_HANDLE                   pRESTHandle,
    PVM_REST_HTTP_RESPONSE_PACKET*   ppResPacket
    );

uint32_t
VmRESTSendChunkedPayload(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_REST_HTTP_RESPONSE_PACKET*   ppResPacket,
    uint32_t                         dataLen
    );

uint32_t
VmRESTSendHeaderAndPayload(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_REST_HTTP_RESPONSE_PACKET*   ppResPacket
    );

uint32_t
VmRESTTriggerAppCb(
    PVMREST_HANDLE                   pRESTHandle,
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

uint32_t
VmRESTAllocateHandle(
     PVMREST_HANDLE*                 ppRESTHandle
     );

void
VmRESTFreeHandle(
     PVMREST_HANDLE                  pRESTHandle
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
    char const*                      configFile,
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
    char const*                      value
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

VOID
VmRESTDecodeEncodedURLString(
    PCSTR                            src,
    PSTR                             dst
    );

uint32_t
VmRESTGetResponseBufferSize(
    PVM_REST_HTTP_RESPONSE_PACKET    pResPacket,
    uint32_t*                        pSize
    );

/***************** restProtocolHead.c  ************/

uint32_t
VmRestEngineHandler(
    PVMREST_HANDLE                   pRESTHandle,
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
    PVMREST_HANDLE                   pRESTHandle,
    char*                            pEndPointURI,
    PREST_PROCESSOR                  pHandler
    );

uint32_t
VmRestEngineRemoveEndpoint(
    PVMREST_HANDLE                   pRESTHandle,
    char const*                      pEndPointURI
    );

uint32_t
VmRestEngineGetEndPoint(
    PVMREST_HANDLE                   pRESTHandle,
    char*                            pEndPointURI,
    PREST_ENDPOINT*                  ppEndPoint
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
    PVMREST_HANDLE                   pRESTHandle,
    PREST_CONF                       pConfig,
    char const*                      file
    );

uint32_t
VmHTTPStart(
    PVMREST_HANDLE                   pRESTHandle
    );

uint32_t
VmHTTPRegisterHandler(
    PVMREST_HANDLE                   pRESTHandle,
    PREST_PROCESSOR                  pHandler
    );

uint32_t
VmHTTPUnRegisterHandler(
    PVMREST_HANDLE                   pRESTHandle
    );

uint32_t
VmHTTPStop(
    PVMREST_HANDLE                   pRESTHandle
    );

void
VmHTTPShutdown(
    PVMREST_HANDLE                   pRESTHandle
    );

/********************* httpValidate.c *******************/

uint32_t
VmRESTTrimSpaces(
    char*                            src,
    char**                           des
    );

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


