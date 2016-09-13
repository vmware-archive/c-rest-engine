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



#ifndef __VREST_DEFINE_H__
#define __VREST_DEFINE_H__

/***** global macros **************/
#define REST_ENGINE_SUCCESS                             0
#define REST_ENGINE_FAILURE                             1

#define MAX_PATH_LEN                                    128
#define MAX_SERVER_PORT_LEN                             6
#define MAX_CLIENT_ALLOWED_LEN                          6
#define MAX_WORKER_COUNT_LEN                            6
#define MAX_LINE_LEN                                    256
#define MAX_STATUS_LENGTH                               6
#define MAX_DATA_BUFFER_LEN                             4096

#define PASS                                            1
#define FAIL                                            0

#define REST_ENGINE_MISSING_CONFIG                      100
#define REST_ENGINE_INVALID_CONFIG                      101
#define REST_ENGINE_NO_MEMORY                           102
#define REST_ENGINE_INVALID_REST_PROCESSER              103

#define ERROR_NOT_SUPPORTED                             100

#define VMREST_HTTP_INVALID_PARAMS                      61001
#define VMREST_HTTP_VALIDATION_FAILED                   61002

#define VMREST_APPLICATION_INVALID_PARAMS               61011
#define VMREST_APPLICATION_VALIDATION_FAILED            61012
#define VMREST_APPLICATION_NO_CB_REGISTERED             61013
#define VMREST_APPLICATION_NO_METHOD_GET_CB             61014
#define VMREST_APPLICATION_NO_METHOD_HEAD_CB            61015
#define VMREST_APPLICATION_NO_METHOD_POST_CB            61016
#define VMREST_APPLICATION_NO_METHOD_PUT_CB             61017
#define VMREST_APPLICATION_NO_METHOD_DELETE_CB          61018
#define VMREST_APPLICATION_NO_METHOD_TRACE_CB           61019
#define VMREST_APPLICATION_NO_METHOD_CONNECT_CB         61020


#define VMREST_TRANSPORT_INVALID_PARAM                  61100
#define VMREST_TRANSPORT_SSL_ERROR                      61101
#define VMREST_TRANSPORT_SSL_CONFIG_ERROR               61102
#define VMREST_TRANSPORT_SSL_CERTIFICATE_ERROR          61103
#define VMREST_TRANSPORT_SSL_PRIVATEKEY_ERROR           61104
#define VMREST_TRANSPORT_SSL_PRIVATEKEY_CHECK_ERROR     61105
#define VMREST_TRANSPORT_SSL_ACCEPT_FAILED              61106

#define VMREST_TRANSPORT_QUEUE_INIT_FAILED              61111
#define VMREST_TRANSPORT_SERVER_THREAD_CREATE_FAILED    61112
#define VMREST_TRANSPORT_SOCKET_GET_ADDRINFO_ERROR      61113
#define VMREST_TRANSPORT_SOCKET_SETSOCKOPT_ERROR        61114
#define VMREST_TRANSPORT_SOCKET_BIND_ERROR              61115
#define VMREST_TRANSPORT_SOCKET_LISTEN_ERROR            61116
#define VMREST_TRANSPORT_EPOLL_CREATE_ERROR             61117
#define VMREST_TRANSPORT_EPOLL_CTL_ERROR                61118
#define VMREST_TRANSPORT_SOCKET_NON_BLOCK_ERROR         61119
#define VMREST_TRANSPORT_QUEUE_EMPTY                    61120
#define VMREST_TRANSPORT_ACCEPT_CONN_FAILED             61121
#define VMREST_TRANSPORT_INSERT_CLIENT_INGLOBAL_ERROR   61122
#define VMREST_TRANSPORT_MAX_CONN_REACHED_ERROR         61123
#define VMREST_TRANSPORT_MUTEX_INIT_FAILED              61124
#define VMREST_TRANSPORT_COND_INIT_FAILED               61125
#define VMREST_TRANSPORT_SERVER_THREAD_START_FAILED     61126

#define ERROR_TRANSPORT_INVALID_PARAMS                  61040
#define ERROR_TRANSPORT_VALIDATION_FAILED               61041


#define TRUE                             1
#define FALSE                            0
#define ERROR_SUCCESS                    0
#define ERROR_INVALID_PARAMETER          101
#define ERROR_CONNECTION_UNAVAIL         102
#define ERROR_INVALID_STATE              103
#define ERROR_INTERNAL_ERROR             104





#ifdef __cplusplus
extern "C" {
#endif

#define VMREST_SAFE_FREE_STRINGA(PTR)      \
    do {                                  \
        if ((PTR)) {                      \
            VmRESTFreeStringA(PTR);        \
            (PTR) = NULL;                 \
        }                                 \
    } while(0)

#define VMREST_SECURE_FREE_STRINGA(PTR)    \
    do {                                  \
        if ((PTR)) {                      \
            if (*(PTR)) {                 \
                memset(PTR, 0, strlen(PTR)); \
            }                             \
            VmRESTFreeStringA(PTR);        \
            (PTR) = NULL;                 \
        }                                 \
    } while(0)

#define VMREST_SAFE_FREE_MEMORY(PTR)       \
    do {                                  \
        if ((PTR)) {                      \
            VmRESTFreeMemory(PTR);         \
            (PTR) = NULL;                 \
        }                                 \
    } while(0)

#define BAIL_ON_VMREST_ERROR(dwError)     \
    if (dwError)                          \
    {                                     \
        goto error;                       \
    }

#define BAIL_ON_VMREST_INVALID_POINTER(p, errCode)     \
        if (p == NULL) {                          \
            errCode = ERROR_INVALID_PARAMETER;    \
            BAIL_ON_VMREST_ERROR(errCode);          \
        }

#ifndef IsNullOrEmptyString
#define IsNullOrEmptyString(str) (!(str) || !*(str))
#endif

#ifndef VMREST_SAFE_STRING
#define VMREST_SAFE_STRING(str) ((str) ? (str) : "")
#endif

#ifdef __cplusplus
}
#endif

#endif /* __VREST_DEFINE_H__ */
