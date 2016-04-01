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


/*********** REST engine Configuration struct *************/
typedef struct _REST_CONFIG
{
    char    ssl_certificate[MAX_PATH_LEN];
    char    ssl_key[MAX_PATH_LEN];
    char    server_port[MAX_SERVER_PORT_LEN];
    char    debug_log_file[MAX_PATH_LEN];
    char    client_count[MAX_CLIENT_ALLOWED_LEN];
    char    worker_thread_count[MAX_WORKER_COUNT_LEN];
} VM_REST_CONFIG, *PVM_REST_CONFIG;

typedef struct _REST_ENG_THREAD_DATA
{
    pthread_mutex_t  mutex;
    pthread_mutex_t* pMutex;

    pthread_cond_t   cond;
    pthread_cond_t*  pCond;

    int              bShutdown;

} VMREST_THREAD_DATA, *PVMREST_THREAD_DATA;

typedef struct _REST_ENG_THREAD
{
    pthread_t  thr;
    pthread_t* pThread;

    PVMREST_THREAD_DATA data;

} VMREST_THREAD, *PVMREST_THREAD;

typedef struct _REST_ENG_GLOBALS
{
    PVMREST_THREAD   pThreadpool;
    uint32_t         nThreads;
    PVM_REST_CONFIG  config;

} REST_ENG_GLOBALS, *PREST_ENG_GLOBALS;

typedef struct _VM_REST_HTTP_MESSAGE_BODY
{
    char buffer[MAX_DATA_BUFFER_LEN];
}VM_REST_HTTP_MESSAGE_BODY, *PVM_REST_HTTP_MESSAGE_BODY;

/* http protocol structures */

typedef struct _VM_REST_HTTP_REQUEST_LINE
{
    char  method[MAX_METHOD_LEN];
    char  uri[MAX_URI_LEN];
    char  version[MAX_VERSION_LEN];

}VM_REST_HTTP_REQUEST_LINE, *PVM_REST_HTTP_REQUEST_LINE;

typedef struct _VM_REST_HTTP_STATUS_LINE
{
    char  version[MAX_VERSION_LEN];
    char  statusCode[MAX_STATUS_LEN];
    char  reason_phrase[MAX_REA_PHRASE_LEN];

}VM_REST_HTTP_STATUS_LINE, *PVM_REST_HTTP_STATUS_LINE;

typedef struct _VM_REST_HTTP_REQUEST_HEADER
{
    char accept[MAX_ACCEPT_LEN];
    char acceptCharSet[MAX_ACCEPT_CHARSET_LEN];
    char acceptEncoding[MAX_ACCEPT_ENCODING_LEN];
    char acceptLanguage[MAX_ACCEPT_LANGUAGE_LEN];
    char authorization[MAX_AUTH_LEN];
    char from[MAX_FROM_LEN];
    char host[MAX_HOST_LEN];
    char referer[MAX_REFERER_LEN];

}VM_REST_HTTP_REQUEST_HEADER, *PVM_REST_HTTP_REQUEST_HEADER;

typedef struct _VM_REST_HTTP_RESPONSE_HEADER
{
    char acceptRange[MAX_ACCEPT_RANGE_LEN];
    char location[MAX_LOCATION_LEN];
    char proxyAuth[MAX_PROXY_AUTH_LEN];
    char server[MAX_SERVER_LEN];

}VM_REST_HTTP_RESPONSE_HEADER, *PVM_REST_HTTP_RESPONSE_HEADER;

typedef struct _VM_REST_HTTP_GENERAL_HEADER
{
    char cacheControl[MAX_CACHE_CONTROL_LEN];
    char connection[MAX_CONN_LEN];
    char trailer[MAX_TRAILER_LEN];
    char transferEncoding[MAX_TRANSFER_ENCODING_LEN];

}VM_REST_HTTP_GENERAL_HEADER, *PVM_REST_HTTP_GENERAL_HEADER;

typedef struct _VM_REST_HTTP_ENTITY_HEADER
{
    char allow[MAX_ALLOW_LEN];
    char contentEncoding[MAX_CONTENT_ENCODING_LEN];
    char contentLanguage[MAX_CONTENT_LANGUAGE_LEN];
    char contentLength[MAX_CONTENT_LENGTH_LEN];
    char contentLocation[MAX_CONTENT_LOCATION_LEN];
    char contentMD5[MAX_CONTENT_MD5_LEN];
    char contentRange[MAX_CONTENT_RANGE_LEN];
    char contentType[MAX_CONTENT_TYPE_LEN];

}VM_REST_HTTP_ENTITY_HEADER, *PVM_REST_HTTP_ENTITY_HEADER;

typedef struct _VM_REST_HTTP_REQUEST_PACKET
{
    PVM_REST_HTTP_REQUEST_LINE     requestLine;
    PVM_REST_HTTP_GENERAL_HEADER   generalHeader;
    PVM_REST_HTTP_REQUEST_HEADER   requestHeader;
    PVM_REST_HTTP_ENTITY_HEADER    entityHeader;
    PVM_REST_HTTP_MESSAGE_BODY     messageBody;
    SSL*                           clientSocketSSL;

}VM_REST_HTTP_REQUEST_PACKET;

typedef struct _VM_REST_HTTP_RESPONSE_PACKET
{
    PVM_REST_HTTP_STATUS_LINE      statusLine;
    PVM_REST_HTTP_GENERAL_HEADER   generalHeader;
    PVM_REST_HTTP_RESPONSE_HEADER  responseHeader;
    PVM_REST_HTTP_ENTITY_HEADER    entityHeader;
    PVM_REST_HTTP_MESSAGE_BODY     messageBody;
    SSL*                           clientSocketSSL;

}VM_REST_HTTP_RESPONSE_PACKET;

struct _VM_REST_RESPONSE_DATA
{

}VM_REST_RESPONSE_DATA, *PVM_REST_RESPONSE_DATA;
