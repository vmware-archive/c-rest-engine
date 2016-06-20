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

typedef struct _VM_REST_HTTP_REQUEST_PACKET*  PREST_REQUEST;

typedef struct _VM_REST_HTTP_RESPONSE_PACKET* PREST_RESPONSE;

typedef uint32_t(
*PFN_PROCESS_HTTP_REQUEST)(
    PREST_REQUEST                    pRequest,
    PREST_RESPONSE*                  ppResponse
    );

typedef struct _REST_PROCESSOR
{
    PFN_PROCESS_HTTP_REQUEST         pfnHandleRequest;

} REST_PROCESSOR, *PREST_PROCESSOR;

typedef struct _REST_CONF
{
    char*                            pSSLCertificate;
    char*                            pSSLKey;
    char*                            pServerPort;
    char*                            pDebugLogFile;
    char*                            pClientCount;
    char*                            pMaxWorkerThread;
} REST_CONF, *PREST_CONF;

typedef struct _REST_ENDPOINT
{
    char*                             junk;
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
    void
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
    char const*                      pszEndpoint,
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
    char const*                      pszEndpoint,
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
void
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
    char**                           ppResponse
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
    char**                           ppResponse
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
    char**                           ppResponse
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
    char const*                      pszName,
    char**                           ppszResponse
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
    char const*                      pszName,
    char*                            pValue
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
    char*                            statusCode
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
    char*                            version
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
    char*                            reasonPhrase
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
    char*                            response,
    uint32_t*                        done
    );


/*
 * @brief Set payload in HTTP response object.
 *
 * @param[in]                        Reference to HTTP Response object.
 * @param[in]                        Payload data.
 * @param[in]                        Payload data length.
 * @param[out]                       Status after all data chunks sent.
 * @return Returns 0 for success
 */
uint32_t
VmRESTSetHttpPayload(
    PREST_RESPONSE*                  ppResponse,
    char*                            buffer,
    uint32_t                         dataLen,
    uint32_t*                        done
    );

/**
 * @brief Stop the REST Engine
 */
uint32_t
VmRESTStop(
    void
    );

/*
 * @brief Shutdown the REST Library
 */
void
VmRESTShutdown(
    void
    );

#endif /* __VMREST_H__ */
