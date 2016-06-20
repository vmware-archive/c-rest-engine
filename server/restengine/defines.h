
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

/* HTTP protocol header defines */

#define MAX_METHOD_LEN             32
#define MAX_URI_LEN                255
#define MAX_VERSION_LEN            32
#define MAX_STATUS_LEN              4
#define MAX_REA_PHRASE_LEN         32
#define HTTP_VER_LEN                8
#define HTTP_CHUNCKED_DATA_LEN      6
#define HTTP_MIN_CHUNK_DATA_LEN     3

#define MAX_DATA_BUFFER_LEN        4096
#define MAX_REQ_LIN_LEN            1024

#define MAX_HTTP_HEADER_ATTR_LEN   64
#define MAX_HTTP_HEADER_VAL_LEN    2048

#define DEFAULT_WORKER_THR_CNT     "5"
#define DEFAULT_CLIENT_CNT         "5"

typedef enum _HTTP_METHODS
{
    HTTP_METHOD_GET = 1,
    HTTP_METHOD_HEAD,
    HTTP_METHOD_POST,
    HTTP_METHOD_PUT,
    HTTP_METHOD_DELETE,
    HTTP_METHOD_TRACE,
    HTTP_METHOD_CONNECT
}HTTP_METHODS;

/*
*  Please DO NOT change first and last header of any
*  category.
*  If new header has to be added, then it should be
*  added INBETWEEN first and last header of the same category
*  excluding first and last.
*  Various category are:
*
*  HTTP_REQUEST_HEADER
*  HTTP_RESPONSE_HEADER
*  HTTP_GENERAL_HEADER
*  HTTP_ENTITY_HEADER
*/

typedef enum _HTTP_HEADERS
{
    HTTP_REQUEST_HEADER_ACCEPT = 1,
    HTTP_REQUEST_HEADER_ACCEPT_CHARSET,
    HTTP_REQUEST_HEADER_ACCEPT_ENCODING,
    HTTP_REQUEST_HEADER_ACCEPT_LANGUAGE,
    HTTP_REQUEST_HEADER_ACCEPT_AUTHORIZATION,
    HTTP_REQUEST_HEADER_FROM,
    HTTP_REQUEST_HEADER_HOST,
    HTTP_REQUEST_HEADER_REFERER,
    HTTP_RESPONSE_HEADER_ACCEPT_RANGE,
    HTTP_RESPONSE_HEADER_LOCATION,
    HTTP_RESPONSE_HEADER_PROXY_AUTH,
    HTTP_RESPONSE_HEADER_SERVER,
    HTTP_GENERAL_HEADER_CACHE_CONTROL,
    HTTP_GENERAL_HEADER_CONNECTION,
    HTTP_GENERAL_HEADER_TRAILER,
    HTTP_GENERAL_HEADER_TRANSFER_ENCODING,
    HTTP_ENTITY_HEADER_ALLOW,
    HTTP_ENTITY_HEADER_CONTENT_ENCODING,
    HTTP_ENTITY_HEADER_CONTENT_LANGUAGE,
    HTTP_ENTITY_HEADER_CONTENT_LENGTH,
    HTTP_ENTITY_HEADER_CONTENT_LOCATION,
    HTTP_ENTITY_HEADER_CONTENT_MD5,
    HTTP_ENTITY_HEADER_CONTENT_RANGE,
    HTTP_ENTITY_HEADER_CONTENT_TYPE,
    HTTP_MISC_HEADER_ALL
}HTTP_HEADERS;

typedef enum _HTTP_STATUS_CODE
{
    CONTINUE                            = 100,
    SWITCHING_PROTOCOL,
    OK                                  = 200,
    CREATED,
    ACCEPTED,
    NON_AUTH_INFO,
    NO_CONTENT,
    RESET_CONTENT,
    PARTIAL_CONTENT,
    MULTIPLE_CHOICES                    = 300,
    MOVED_PERMANENTLY,
    FOUND,
    SEE_OTHER,
    NOT_MODIFIED,
    USE_PROXY,
    TEMPORARY_REDIRECT,
    BAD_REQUEST                         = 400,
    UNAUTHORIZED,
    PAYMENT_REQUIRED,
    FORBIDDEN,
    NOT_FOUND,
    METHOD_NOT_ALLOWED,
    NOT_ACCEPTABLE,
    PROXY_AUTH_REQUIRED,
    REQUEST_TIMEOUT,
    CONFLICT,
    GONE,
    LENGTH_REQUIRED,
    PRECONDITION_FAILED,
    REQUEST_ENTITY_TOO_LARGE,
    REQUEST_URI_TOO_LARGE,
    UNSUPPORTED_MEDIA_TYPE,
    REQUEST_RANGE_NOT_SATISFIABLE,
    EXPECTATION_FAILED,
    INTERNAL_SERVER_ERROR              = 500,
    NOT_IMPLEMENTED,
    BAD_GATEWAY,
    SERVICE_UNAVAILABLE,
    GATEWAY_TIMEOUT,
    HTTP_VERSION_NOT_SUPPORTED
}HTTP_STATUS_CODE;

