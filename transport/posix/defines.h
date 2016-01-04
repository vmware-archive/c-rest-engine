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


/* provide port and ip information here for the time being */
#define PORT 61001
#define SERVERIP 127.0.0.1
#define MAX_EVENT 64


#define MAX_ADDRESS_LEN 128
#define MAX_PORT_LEN 6

#define MAX_CONNECTIONS 1000


/* Transport internal error code */

#define VMREST_TRANSPORT_NO_ERROR                        0
#define VMREST_TRANSPORT_INVALID_PARAM                   100
/* Secure socket config error */
#define VMREST_TRANSPORT_SSL_ERROR                       101
#define VMREST_TRANSPORT_SSL_CONFIG_ERROR                102
#define VMREST_TRANSPORT_SSL_CERTIFICATE_ERROR           103
#define VMREST_TRANSPORT_SSL_PRIVATEKEY_ERROR            104
#define VMREST_TRANSPORT_SSL_PRIVATEKEY_CHECK_ERROR      105
#define VMREST_TRANSPORT_SSL_ACCEPT_FAILED               106

#define VMREST_TRANSPORT_QUEUE_INIT_FAILED               111
#define VMREST_TRANSPORT_SERVER_THREAD_CREATE_FAILED     112
#define VMREST_TRANSPORT_SOCKET_GET_ADDRINFO_ERROR       113
#define VMREST_TRANSPORT_SOCKET_SETSOCKOPT_ERROR         114
#define VMREST_TRANSPORT_SOCKET_BIND_ERROR               115
#define VMREST_TRANSPORT_SOCKET_LISTEN_ERROR             116
#define VMREST_TRANSPORT_EPOLL_CREATE_ERROR              117
#define VMREST_TRANSPORT_EPOLL_CTL_ERROR                 118
#define VMREST_TRANSPORT_SOCKET_NON_BLOCK_ERROR          119
#define VMREST_TRANSPORT_QUEUE_EMPTY                     120
#define VMREST_TRANSPORT_ACCEPT_CONN_FAILED              121
#define VMREST_TRANSPORT_INSERT_CLIENT_INGLOBAL_ERROR    122
#define VMREST_TRANSPORT_MAX_CONN_REACHED_ERROR          123
#define VMREST_TRANSPORT_MUTEX_INIT_FAILED               124
#define VMREST_TRANSPORT_COND_INIT_FAILED                125




#define ERROR_NOT_SUPPORTED 100

#define BAIL_ON_POSIX_SOCK_ERROR(dwError) \
        if (dwError) \
            goto error;
