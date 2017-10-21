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

#ifndef __VREST_DEFINE_H__
#define __VREST_DEFINE_H__

/***** global macros **************/
#define PASS                                            1
#define FAIL                                            0

#ifndef WIN32
#define ERROR_NOT_SUPPORTED                             100
#endif

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
#define VMREST_TRANSPORT_SSL_INVALID_CIPHER_SUITES      61007

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

#define SSL_INFO_NOT_SET                                0
#define SSL_INFO_NO_SSL_PLAIN                           1
#define SSL_INFO_FROM_CONFIG_FILE                       2
#define SSL_INFO_FROM_BUFFER_API                        3
#define SSL_INFO_USE_APP_CONTEXT                        4

#define VMREST_DEFAULT_WORKER_THR_COUNT                 5
#define VMREST_DEFAULT_CLIENT_COUNT                     100
#define VMREST_DEFAULT_CONN_TIMEOUT_SEC                 60
#define VMREST_DEFAULT_CONN_PAYLOAD_LIMIT_MB            25

#define VMREST_MAX_WORKER_THR_COUNT                     100
#define VMREST_MAX_CLIENT_COUNT                         10000
#define VMREST_MAX_CONN_TIMEOUT_SEC                     600
#define VMREST_MAX_CONN_PAYLOAD_LIMIT_MB                50


#define TRUE                             1
#define FALSE                            0

#define INVALID                          -1


#ifndef WIN32
#define ERROR_SUCCESS                    0
#define ERROR_INVALID_PARAMETER          101
#define ERROR_CONNECTION_UNAVAIL         102
#define ERROR_INVALID_STATE              103
#define ERROR_INTERNAL_ERROR             104

#endif

#ifndef _WIN32
#define VMREST_SF_INIT( fieldName, fieldValue ) fieldName = fieldValue
#else
#define VMREST_SF_INIT( fieldName, fieldValue ) fieldValue
#endif

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

#define BAIL_ON_VMREST_ERROR(dwError)       \
    do {                                  \
        if (dwError) {                    \
            goto error;                   \
        }                                 \
    } while(0)

#define BAIL_ON_VMREST_INVALID_POINTER(p, errCode)     \
        if (p == NULL) {                          \
            errCode = ERROR_INVALID_PARAMETER;    \
            BAIL_ON_VMREST_ERROR(errCode);          \
        }

#ifndef IsNullOrEmptyString
#define IsNullOrEmptyString(str) (!(str) || !(*str))
#endif

#ifndef VMREST_SAFE_STRING
#define VMREST_SAFE_STRING(str) ((str) ? (str) : "")
#endif

#ifdef __cplusplus
}
#endif

#endif /* __VREST_DEFINE_H__ */
