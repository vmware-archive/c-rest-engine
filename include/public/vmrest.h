/*
 * Copyright © 2012-2015 VMware, Inc.  All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the ~@~\License~@~]); you may not
 * use this file except in compliance with the License.  You may obtain a copy
 * of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an ~@~\AS IS~@~] BASIS, without
 * warranties or conditions of any kind, EITHER EXPRESS OR IMPLIED.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */
#ifndef __VMREST_H__
#define __VMREST_H__

#ifndef REST_PCSTR_DEFINED
typedef char const* REST_PCSTR;
typedef char* REST_PSTR;
#define REST_PCSTR_DEFINED 1
#endif /* REST_PCSTR_DEFINED */

#ifndef REST_VOID_DEFINED
typedef void REST_VOID;
#define REST_VOID_DEFINED 1
#endif /* REST_VOID_DEFINED */


typedef REST_PCSTR PCSTR;
typedef REST_PSTR  PSTR;
typedef REST_VOID  VOID;
typedef VOID*      PVOID;

typedef struct _VM_REST_HTTP_REQUEST_PACKET*  PREST_REQUEST;

typedef struct _VM_REST_HTTP_RESPONSE_PACKET* PREST_RESPONSE;

typedef uint32_t(
*PFN_PROCESS_HTTP_GET)(
    PREST_REQUEST                    pRequest,
    PREST_RESPONSE*                  ppResponse
    );

typedef uint32_t(
*PFN_PROCESS_HTTP_POST)(
    PREST_REQUEST                    pRequest,
    PREST_RESPONSE*                  ppResponse
    );

typedef uint32_t(
*PFN_PROCESS_HTTP_HEAD)(
    PREST_REQUEST                    pRequest,
    PREST_RESPONSE*                  ppResponse
    );

typedef uint32_t(
*PFN_PROCESS_HTTP_PUT)(
    PREST_REQUEST                    pRequest,
    PREST_RESPONSE*                  ppResponse
    );

typedef uint32_t(
*PFN_PROCESS_HTTP_DELETE)(
    PREST_REQUEST                    pRequest,
    PREST_RESPONSE*                  ppResponse
    );

typedef uint32_t(
*PFN_PROCESS_HTTP_TRACE)(
    PREST_REQUEST                    pRequest,
    PREST_RESPONSE*                  ppResponse
    );

typedef uint32_t(
*PFN_PROCESS_HTTP_CONNECT)(
    PREST_REQUEST                    pRequest,
    PREST_RESPONSE*                  ppResponse
    );

typedef struct _REST_PROCESSOR
{
    PFN_PROCESS_HTTP_GET             pfnHandleGET;
    PFN_PROCESS_HTTP_POST            pfnHandlePOST;
    PFN_PROCESS_HTTP_HEAD            pfnHandleHEAD;
    PFN_PROCESS_HTTP_PUT             pfnHandlePUT;
    PFN_PROCESS_HTTP_DELETE          pfnHandleDELETE;
    PFN_PROCESS_HTTP_TRACE           pfnHandleTRACE;
    PFN_PROCESS_HTTP_CONNECT         pfnHandleCONNECT;

} REST_PROCESSOR, *PREST_PROCESSOR;

typedef struct _REST_CONF
{
    PSTR                             pSSLCertificate;
    PSTR                             pSSLKey;
    PSTR                             pServerPort;
    PSTR                             pDebugLogFile;
    PSTR                             pClientCount;
    PSTR                             pMaxWorkerThread;
} REST_CONF, *PREST_CONF;

typedef struct _REST_ENDPOINT
{
    PSTR                             junk;
} REST_ENDPOINT, *PREST_ENDPOINT;

/*
 * @brief Rest engine initialization
 *
 * @param[in]                        Rest engine configuration.
 *                                   For default behaviour call this with NULL
 *                                   This will read config from file /root/restConfig.txt
 * @return                           Returns 0 for success.
 */

uint32_t
VmRESTInit(
    PREST_CONF                       pConfig
    );

/**
 * @brief Starts the HTTP(S) listeners
 *
 * @param[in]                        Void.
 * @return                           Returns 0 for success.
 */
uint32_t
VmRESTStart(
    VOID
    );

/**
 * @brief Register a REST Endpoint and Handler
 * @param[in]                        pszEndpoint Endpoint URL to register
 * @param[in]                        pHandler Callback functions registered for endpoint
 * @param[out]                       ppEndpoint Optionally return the endpoint registration object
 * @return                           Returns 0 for Success
 */
uint32_t
VmRESTRegisterHandler(
    PCSTR                            pszEndpoint,
    PREST_PROCESSOR                  pHandler,
    PREST_ENDPOINT*                  ppEndpoint
    );

/**
 * @brief Find a registration object matching the endpoint URL
 * @param[in]                        pszEndpoint Endpoint URL to lookup
 * @param[out]                       ppEndpoint Endpoint registration to return for given URL
 * @return                           Returns 0 for Success
 */
uint32_t
VmRESTFindEndpoint(
    PCSTR                            pszEndpoint,
    PREST_ENDPOINT*                  ppEndpoint
    );

/**
 * @brief Unregister an endpoint
 * @return                           Returns 0 for success
 */
uint32_t
VmRESTUnregisterHandler(
    PREST_ENDPOINT                   pEndpoint
    );

/**
 * @brief Release the memory associated with the endpoint
 */
VOID
VmRESTReleaseEndpoint(
    PREST_ENDPOINT                   pEndpoint
    );

/*
 * @brief Retrieve method name associated with request http object.
 *
 * @param[in]                        Reference to HTTP Request object
 * @param[out]                       HTTP method present in request object.
 * @return                           Returns 0 for success else Error code.
 */

uint32_t
VmRESTGetHttpMethod(
    PREST_REQUEST                    pRequest,
    PSTR                             response
    );

/*
 * @brief Retrieve URI associated with request http object.
 *
 * @param[in]                        Reference to HTTP Request object.
 * @param[out]                       URI present in request object.
 * @return                           Returns 0 for success else error code.
 */
uint32_t
VmRESTGetHttpURI(
    PREST_REQUEST                    pRequest,
    PSTR                             response
    );

/*
 * @brief Retrieve HTTP Version associated with request http object.
 *
 * @param[in]                        Reference to HTTP Request object.
 * @param[out]                       HTTP version (1.0/1.1) present in request object.
 * @return                           Returns 0 for success else error code.
 */
uint32_t
VmRESTGetHttpVersion(
    PREST_REQUEST                    pRequest,
    PSTR                             response
    );

/*
 * @brief Retrieve Value of HTTP header associated with request http object.
 *
 * @param[in]                        Reference to HTTP Request object.
 * @param[in]                        Header field to be retrieve.
 * @param[out]                       Value of header present in request object.
 * @return                           Returns 0 for success else error code.
 */
uint32_t
VmRESTGetHttpHeader(
    PREST_REQUEST                    pRequest,
    PCSTR                            pszName,
    PSTR                             ppszResponse
    );

/*
 * @brief Set given value to given HTTP header in the response http object.
 *
 * @param[in]                        Reference to HTTP Response object.
 * @param[in]                        Header field to be set.
 * @param[in]                        Value of header field to be set.
 * @return Returns 0 for success
 */
uint32_t
VmRESTSetHttpHeader(
    PREST_RESPONSE*                  ppResponse,
    PCSTR                            pszName,
    PSTR                             pValue 
    );

/*
 * @brief Set given value to HTTP reponse object status line.
 *
 * @param[in]                        Reference to HTTP Response object.
 * @param[in]                        StatusCode to be set.
 * @return Returns 0 for success
 */
uint32_t
VmRESTSetHttpStatusCode(
    PREST_RESPONSE*                  ppResponse,
    PSTR                             statusCode
    );

/*
 * @brief Set given HTTP Version to HTTP reponse object status line.
 *
 * @param[in]                        Reference to HTTP Response object.
 * @param[in]                        HTTP Version to be set.
 * @return Returns 0 for success
 */
uint32_t
VmRESTSetHttpStatusVersion(
    PREST_RESPONSE*                  ppResponse,
    PSTR                             version
    );

/*
 * @brief Set given reason phrase to HTTP reponse object status line.
 *
 * @param[in]                        Reference to HTTP Response object.
 * @param[in]                        Reason phrase to be set.
 * @return Returns 0 for success
 */
uint32_t
VmRESTSetHttpReasonPhrase(
    PREST_RESPONSE*                  ppResponse,
    PSTR                             reasonPhrase
    );

/*
 * @brief Get payload from HTTP request object.
 *
 * @param[in]                        Reference to HTTP Request object.
 * @param[out]                       Payload buffer(must be allocated by caller).
 * @return Returns 0 for success
 */
uint32_t
VmRESTGetHttpPayload(
    PREST_REQUEST                    pRequest,
    PSTR                             response
    );


/*
 * @brief Set payload in HTTP response object.
 *
 * @param[in]                        Reference to HTTP Response object.
 * @param[in]                        Payload data.
 * @return Returns 0 for success
 */
uint32_t
VmRESTSetHttpPayload(
    PREST_RESPONSE*                  ppResponse,
    PSTR                             buffer
    );

/**
 * @brief Stop the REST Engine
 */
uint32_t
VmRESTStop(
    VOID
    );

/*
 * @brief Shutdown the REST Library
 */
VOID
VmRESTShutdown(
    VOID
    );

#endif /* __VMREST_H__ */
