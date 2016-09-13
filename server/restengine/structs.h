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

typedef struct _REST_ENG_GLOBALS
{
    PVMREST_THREAD                   pThreadpool;
    uint32_t                         nThreads;
    PVM_REST_CONFIG                  config;
    pthread_mutex_t                  mutex;
    PREST_ENDPOINT                   pEndPointQueue;
    uint32_t                         useEndPoint;
    REST_PROCESSOR                   internalHandler;

} REST_ENG_GLOBALS, *PREST_ENG_GLOBALS;

typedef struct _VM_REST_HTTP_MESSAGE_BODY
{
    char                             buffer[MAX_DATA_BUFFER_LEN];

}VM_REST_HTTP_MESSAGE_BODY, *PVM_REST_HTTP_MESSAGE_BODY;

typedef struct _VM_REST_URL_PARAMS
{
    char                             key[MAX_KEY_VAL_PARAM_LEN];
    char                             value[MAX_KEY_VAL_PARAM_LEN];

}VM_REST_URL_PARAMS, *PVM_REST_URL_PARAMS;

/* http protocol structures */

typedef struct _VM_REST_HTTP_REQUEST_LINE
{
    char                             method[MAX_METHOD_LEN];
    char                             uri[MAX_URI_LEN];
    char                             version[MAX_VERSION_LEN];

}VM_REST_HTTP_REQUEST_LINE, *PVM_REST_HTTP_REQUEST_LINE;

typedef struct _VM_REST_HTTP_STATUS_LINE
{
    char                             version[MAX_VERSION_LEN];
    char                             statusCode[MAX_STATUS_LEN];
    char                             reason_phrase[MAX_REA_PHRASE_LEN];

}VM_REST_HTTP_STATUS_LINE, *PVM_REST_HTTP_STATUS_LINE;

typedef struct _VM_REST_HTTP_HEADER_NODE
{
    char                             header[MAX_HTTP_HEADER_ATTR_LEN];
    char                             value[MAX_HTTP_HEADER_VAL_LEN];
    struct _VM_REST_HTTP_HEADER_NODE *next;

}VM_REST_HTTP_HEADER_NODE, *PVM_REST_HTTP_HEADER_NODE;

typedef struct _MISC_HEADER_QUEUE {

    PVM_REST_HTTP_HEADER_NODE        head;

}MISC_HEADER_QUEUE, *PMISC_HEADER_QUEUE;

typedef struct _VM_REST_HTTP_REQUEST_PACKET
{
    PVM_REST_HTTP_REQUEST_LINE       requestLine;
    PMISC_HEADER_QUEUE               miscHeader;
    PVM_SOCKET                       pSocket;
    uint32_t                         dataRemaining;
    VM_REST_URL_PARAMS               paramArray[MAX_URL_PARAMS_ARR_SIZE];    

}VM_REST_HTTP_REQUEST_PACKET, *PVM_REST_HTTP_REQUEST_PACKET;

typedef struct _VM_REST_HTTP_RESPONSE_PACKET
{
    PVM_REST_HTTP_STATUS_LINE        statusLine;
    PVM_REST_HTTP_MESSAGE_BODY       messageBody;
    PMISC_HEADER_QUEUE               miscHeader;
    PVM_SOCKET                       pSocket;
    PVM_REST_HTTP_REQUEST_PACKET     requestPacket;
    uint32_t                         headerSent;

}VM_REST_HTTP_RESPONSE_PACKET, *PVM_REST_HTTP_RESPONSE_PACKET;

struct _VM_REST_RESPONSE_DATA
{

}VM_REST_RESPONSE_DATA, *PVM_REST_RESPONSE_DATA;

