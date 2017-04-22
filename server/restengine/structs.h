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

typedef struct _REST_ENG_GLOBALS
{
    PVMREST_THREAD                   pThreadpool;
    uint32_t                         nThreads;
    pthread_mutex_t                  mutex;
    PREST_ENDPOINT                   pEndPointQueue;
    uint32_t                         useEndPoint;
    REST_PROCESSOR                   internalHandler;

} REST_ENG_GLOBALS;

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
    uint32_t                         dataNotRcvd; 

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
	int junk;
}VM_REST_RESPONSE_DATA, *PVM_REST_RESPONSE_DATA;

