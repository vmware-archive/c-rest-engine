
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

typedef void* (PFN_VMREST_THR_ROUTINE)(void*);

#define VMREST_WORKER_THREAD_COUNT 5

#define BAIL_ON_POSIX_THREAD_ERROR(dwError) \
        if (dwError) \
            goto error;

#define ERROR_NOT_SUPPORTED 100


/* HTTP protocol header defines */

#define MAX_METHOD_LEN             32
#define MAX_URI_LEN                32
#define MAX_VERSION_LEN            32
#define MAX_STATUS_LEN             32
#define MAX_REA_PHRASE_LEN         32
#define MAX_ACCEPT_LEN             32
#define MAX_ACCEPT_CHARSET_LEN     32
#define MAX_ACCEPT_ENCODING_LEN    32
#define MAX_ACCEPT_LANGUAGE_LEN    32
#define MAX_AUTH_LEN               32
#define MAX_FROM_LEN               32
#define MAX_HOST_LEN               32
#define MAX_REFERER_LEN            32
#define MAX_ACCEPT_RANGE_LEN       32
#define MAX_LOCATION_LEN           32
#define MAX_PROXY_AUTH_LEN         32
#define MAX_SERVER_LEN             32
#define MAX_CACHE_CONTROL_LEN      32
#define MAX_CONN_LEN               32
#define MAX_TRAILER_LEN            32
#define MAX_TRANSFER_ENCODING_LEN  32
#define MAX_ALLOW_LEN              32
#define MAX_CONTENT_ENCODING_LEN   32 
#define MAX_CONTENT_LANGUAGE_LEN   32
#define MAX_CONTENT_LENGTH_LEN     32
#define MAX_CONTENT_LOCATION_LEN   32 
#define MAX_CONTENT_MD5_LEN        32
#define MAX_CONTENT_RANGE_LEN      32
#define MAX_CONTENT_TYPE_LEN       32

#define MAX_DATA_BUFFER_LEN        4096
#define MAX_REQ_LIN_LEN            1024

#define MAX_HTTP_HEADER_ATTR_LEN   32
#define MAX_HTTP_HEADER_VAL_LEN    1024
