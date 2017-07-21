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

#ifndef __VMREST_H__
#define __VMREST_H__


#ifdef _WIN32
#ifdef LIBTRIDENT_EXPORTS
#define VMREST_API __declspec(dllexport)
#else
#define VMREST_API __declspec(dllimport)
#endif
#else
#define VMREST_API
#endif

/**** REST ENGINE ERROR CODES ****/
#define     REST_ENGINE_SUCCESS                            0
#define     REST_ENGINE_FAILURE                            1
#define     REST_ERROR_MISSING_CONFIG                      100
#define     REST_ERROR_INVALID_CONFIG                      101
#define     REST_ERROR_NO_MEMORY                           102
#define     REST_ERROR_INVALID_REST_PROCESSER              103
#define     REST_ERROR_ENDPOINT_EXISTS                     104
#define     REST_ERROR_ENDPOINT_BAD_URI                    105
#define     REST_ERROR_INVALID_CONFIG_PORT                 106
#define     REST_ERROR_INVALID_CONFIG_SSL_CERT             107
#define     REST_ERROR_INVALID_CONFIG_CLT_CNT              108
#define     REST_ERROR_INVALID_CONFIG_WKR_THR_CNT          109
#define     REST_ERROR_BAD_CONFIG_FILE_PATH                110
#define     REST_ERROR_PREV_INSTANCE_NOT_CLEAN             111
#define     REST_ERROR_INVALID_HANDLER                     112
#define     REST_ENGINE_MORE_IO_REQUIRED                   7001
#define     REST_ENGINE_IO_COMPLETED                       0




/**** REST ENGINE CONFIG PARAMS ****/
#define     MAX_DATA_BUFFER_LEN                             4096
#define     MAX_PATH_LEN                                    128
#define     MAX_SSL_DATA_BUF_LEN                            64000
#define     MAX_SERVER_PORT_LEN                             7
#define     MAX_CLIENT_ALLOWED_LEN                          6
#define     MAX_WORKER_COUNT_LEN                            6
#define     MAX_LINE_LEN                                    256
#define     MAX_STATUS_LENGTH                               6
#define     SSL_DATA_TYPE_KEY                               1
#define     SSL_DATA_TYPE_CERT                              2

typedef struct _VMREST_HANDLE* PVMREST_HANDLE;

typedef struct _VM_REST_HTTP_REQUEST_PACKET*  PREST_REQUEST;

typedef struct _VM_REST_HTTP_RESPONSE_PACKET* PREST_RESPONSE;


typedef uint32_t(
*PFN_PROCESS_HTTP_REQUEST)(
    PVMREST_HANDLE                   pRESTHandle,
    PREST_REQUEST                    pRequest,
    PREST_RESPONSE*                  ppResponse
    );

typedef uint32_t(
*PFN_PROCESS_REST_CRUD)(
    PVMREST_HANDLE                   pRESTHandle,
    PREST_REQUEST                    pRequest,
    PREST_RESPONSE*                  ppResponse,
    uint32_t                         paramsCount
    );

typedef struct _REST_PROCESSOR
{
    PFN_PROCESS_HTTP_REQUEST         pfnHandleRequest;
    PFN_PROCESS_REST_CRUD            pfnHandleCreate;
    PFN_PROCESS_REST_CRUD            pfnHandleRead;
    PFN_PROCESS_REST_CRUD            pfnHandleUpdate;
    PFN_PROCESS_REST_CRUD            pfnHandleDelete;
    PFN_PROCESS_REST_CRUD            pfnHandleOthers;

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
    char*                             pszEndPointURI;
    PREST_PROCESSOR                   pHandler;
    struct _REST_ENDPOINT*            next;
} REST_ENDPOINT, *PREST_ENDPOINT;

/*
 * @brief Rest engine initialization
 *
 * @param[in]                        Rest engine configuration.
 *                                   For default behaviour call this with NULL
 *                                   to read config params from file.
 * @param[in]                        Config file Path.
 *                                   If this is NULL, restengine will try
 *                                   reading config from /root/restconfig.txt.
 * @param[out]                       Handle to Library instance.
 * @return                           Returns 0 for success.
 */

VMREST_API
uint32_t
VmRESTInit(
    PREST_CONF                       pConfig,
    char const*                      file,
    PVMREST_HANDLE*                  ppRESTHandle
    );

/*
 * @brief Set SSL information from buffer
 *
 * @param[in]                        Rest engine Handle
 * @param[in]                        Buffer holding SSL data.
 * @param[in]                        Size of certificate buffer
 * @param[in]                        ssl data type, SSL_DATA_TYPE_KEY or SSL_DATA_TYPE_CERT
 * @return                           Returns 0 for success.
 */
VMREST_API
uint32_t
VmRESTSetSSLInfo(
     PVMREST_HANDLE                   pRESTHandle,
     char*                            pDataBuffer,
     uint32_t                         bufferSize,
     uint32_t                         sslDataType
     );

/**
 * @brief Starts the HTTP(S) listeners
 *
 * @param[in]                        Handle to Library instance.
 * @return                           Returns 0 for success.
 */
VMREST_API
uint32_t
VmRESTStart(
    PVMREST_HANDLE                   pRESTHandle
    );

/**
 * @brief Register a REST Endpoint and Handler
 * @param[in]                        Handle to Library instance.
 * @param[in]                        pszEndpoint Endpoint URL to register
 * @param[in]                        pHandler Callback functions registered for endpoint
 * @param[out]                       ppEndpoint Optionally return the endpoint registration object
 *                                   NOT SUPPORTED CURRENTLY. Use Find API.
 * @return                           Returns 0 for Success
 */
VMREST_API
uint32_t
VmRESTRegisterHandler(
    PVMREST_HANDLE                   pRESTHandle,
    char const*                      pszEndpoint,
    PREST_PROCESSOR                  pHandler,
    PREST_ENDPOINT*                  ppEndpoint
    );

/**
 * @brief Find a registration object matching the endpoint URL
 * @param[in]                        Handle to Library instance.
 * @param[in]                        pszEndpoint Endpoint URL to lookup
 * @param[out]                       ppEndpoint Endpoint registration to return for given URL
 * @return                           Returns 0 for Success
 */
VMREST_API
uint32_t
VmRESTFindEndpoint(
    PVMREST_HANDLE                   pRESTHandle,
    char const*                      pszEndpoint,
    PREST_ENDPOINT*                  ppEndpoint
    );

/**
 * @brief Free the pointer returned by VmRESTFindEndpoint API.
 * @param[in]                        EndPoint pointer return by Find API
 */
VMREST_API
void
VmRESTFreeEndPoint(
    PREST_ENDPOINT                   pEndPoint
    );

/**
 * @brief Unregister an endpoint
 * @return                           Returns 0 for success
 */
VMREST_API
uint32_t
VmRESTUnRegisterHandler(
    PVMREST_HANDLE                   pRESTHandle,
    char const*                      pEndpointURI
    );

/**
 * @brief Get the endpoint URI from http URI.(Truncate URL by '?' character)
 * @param[in]                        Http URI
 * @param[out]                       Endpoint URI.Must be freed by caller.
 */
VMREST_API
uint32_t
VmRestGetEndPointURIfromRequestURI(
    char const*                      pRequestURI,
    char**                           endPointURI
    );

/*
 * @brief Retrieve method name associated with request http object.
 *
 * @param[in]                        Reference to HTTP Request object
 * @param[out]                       HTTP method present in request object.(Freed by caller).
 * @return                           Returns 0 for success else Error code.
 */
VMREST_API
uint32_t
VmRESTGetHttpMethod(
    PREST_REQUEST                    pRequest,
    char**                           ppResponse
    );

/*
 * @brief Retrieve URI associated with request http object.
 *
 * @param[in]                        Reference to HTTP Request object.
 * @param[out]                       URI present in request object.(Freed by caller)
 * @return                           Returns 0 for success else error code.
 */
VMREST_API
uint32_t
VmRESTGetHttpURI(
    PREST_REQUEST                    pRequest,
    char**                           ppResponse
    );

/*
 * @brief Retrieve HTTP Version associated with request http object.
 *
 * @param[in]                        Reference to HTTP Request object.
 * @param[out]                       HTTP version (1.0/1.1) present in request object.(Freed by caller)
 * @return                           Returns 0 for success else error code.
 */
VMREST_API
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
 * @param[out]                       Value of header present in request object.(Freed by caller)
 * @return                           Returns 0 for success else error code.
 */
VMREST_API
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
VMREST_API
uint32_t
VmRESTSetHttpHeader(
    PREST_RESPONSE*                  ppResponse,
    char const*                      pszName,
    char const*                      pValue
    );

/*
 * @brief Get Data received from client.
 *
 * @param[in]                        Handle to Library instance.
 * @param[in]                        Reference to HTTP Request object.
 * @param[out]                       Pre allocated Data buffer.
 * @param[out]                       Actual bytes returned in buffer.
 * @return                           Returns REST_ENGINE_IO_COMPLETED for success,
                                     REST_ENGINE_MORE_IO_REQUIRED or Error codes.
 */
VMREST_API 
uint32_t
VmRESTGetData(
    PVMREST_HANDLE                   pRESTHandle,
    PREST_REQUEST                    pRequest,
    char*                            pBuffer,
    uint32_t*                        bytesRead
    );

/*
 * @brief Get the params associated with URI of HTTP req object.
 *
 * @param[in]                        Reference to HTTP Request object.
 * @param[in]                        Total params found in URL.
 * @param[in]                        Params number for this index.
 * @param[out]                       Pointer to result key.Must be freed by caller.
 * @param[out]                       Pointer to result Value.Must be freed by caller.
 * @return Returns 0 for success
 */
VMREST_API 
uint32_t
VmRESTGetParamsByIndex(
    PREST_REQUEST                    pRequest,
    uint32_t                         paramsCount,
    uint32_t                         paramIndex,
    char**                           pszKey,
    char**                           pszValue
    );

/*
 * @brief Get the number of wild card strings present in Endpoint.
 *
 * @param[in]                        Handle to Library instance.
 * @param[in]                        Reference to HTTP Request object..
 * @param[out]                       Pointer to result Value.
 * @return Returns 0 for success
 */
VMREST_API
uint32_t
VmRESTGetWildCardCount(
    PVMREST_HANDLE                   pRESTHandle,
    PREST_REQUEST                    pRequest,
    uint32_t*                        wildCardCount
    );

/*
 * @brief Get the wild card string in request by index.
 *
 * @param[in]                        Handle to Library instance.
 * @param[in]                        Reference to HTTP Request object.
 * @param[in]                        Index for wild card string.
 * @param[out]                       Pointer to resultant string.Must be freed by caller.
 * @return Returns 0 for success
 */
VMREST_API
uint32_t
VmRESTGetWildCardByIndex(
    PVMREST_HANDLE                   pRESTHandle,
    PREST_REQUEST                    pRequest,
    uint32_t                         index,
    char**                           ppszWildCard
    );

/*
 * @brief Set length of data in response object(< 4096 bytes) or NULL for chunked.
 *
 * @param[in]                        Reference to HTTP Request object.
 * @param[in]                        Data Length if less than 4096 else NULL.
 * @return                           Returns 0 for success
 */
VMREST_API 
uint32_t
VmRESTSetDataLength(
    PREST_RESPONSE*                  ppResponse,
    char*                            dataLen
    );

/*
 * @brief Prepare response object for successful app processing.
 *        This will populate 200 OK HTTP status code in response object.
 *
 * @param[in]                        Reference to HTTP Request object.
 * @param[in]                        Reference to HTTP Response object.
 * @return                           Returns 0 for success
 */
VMREST_API 
uint32_t
VmRESTSetSuccessResponse(
    PREST_REQUEST                    pRequest,
    PREST_RESPONSE*                  ppResponse
    );

/*
 * @brief Prepare response object for errored app processing.
 *        This will populate 500 Internal Server Error
 *        in  HTTP response object if no error code is specified.
 *
 * @param[in]                        Reference to HTTP Request object.
 * @param[in]                        Error code (NULL is default has to sent).
 * @param[in]                        Reason Phrase (NULL is default has to sent).
 * @return                           Returns 0 for success
 */
VMREST_API 
uint32_t
VmRESTSetFailureResponse(
    PREST_RESPONSE*                  ppResponse,
    char const*                      pErrorCode,
    char const*                      pErrorMessage
    );

/*
 * @brief Set data in response object to be send back to client.
 *
 * @param[in]                        Handle to Library instance.
 * @param[in]                        Reference to HTTP Response object.
 * @param[in]                        Payload data.
 * @param[in]                        Payload data length.
 * @param[in]                        Actual bytes written.
 * @return                           Returns REST_ENGINE_IO_COMPLETED for success,
                                     REST_ENGINE_MORE_IO_REQUIRED or Error codes.
 */
VMREST_API 
uint32_t
VmRESTSetData(
    PVMREST_HANDLE                   pRESTHandle,
    PREST_RESPONSE*                  ppResponse,
    char const*                      buffer,
    uint32_t                         dataLen,
    uint32_t*                        bytesWritten
    );

/**
 * @brief Stop the REST Engine
 * @param[in]                        Handle to Library instance.
 * @return                           Returns 0 for success
 */
VMREST_API 
uint32_t
VmRESTStop(
    PVMREST_HANDLE                   pRESTHandle
    );

/*
 * @brief Shutdown the REST Library
 * @param[in]                        Handle to Library instance.
 */
VMREST_API 
void
VmRESTShutdown(
    PVMREST_HANDLE                  pRESTHandle
    );


/****************************************************************************
* 
* Following are the exposed API to interact with HTTP engine directly I would
* highly recommend services not to interact with HTTP directly. In Future
* releases, these will be depricated.
*
*****************************************************************************/

/*
 * @brief Set given value to HTTP reponse object status line.
 *
 * @param[in]                        Reference to HTTP Response object.
 * @param[in]                        StatusCode to be set.
 * @return Returns 0 for success
 */
VMREST_API 
uint32_t
VmRESTSetHttpStatusCode(
    PREST_RESPONSE*                  ppResponse,
    char const*                      statusCode
    );

/*
 * @brief Set given HTTP Version to HTTP reponse object status line.
 *
 * @param[in]                        Reference to HTTP Response object.
 * @param[in]                        HTTP Version to be set.
 * @return Returns 0 for success
 */
VMREST_API 
uint32_t
VmRESTSetHttpStatusVersion(
    PREST_RESPONSE*                  ppResponse,
    char const*                      version
    );

/*
 * @brief Set given reason phrase to HTTP reponse object status line.
 *
 * @param[in]                        Reference to HTTP Response object.
 * @param[in]                        Reason phrase to be set.
 * @return Returns 0 for success
 */
VMREST_API 
uint32_t
VmRESTSetHttpReasonPhrase(
    PREST_RESPONSE*                  ppResponse,
    char const*                      reasonPhrase
    );

/*
 * @brief Get payload from HTTP request object.
 *
 * @param[in]                        Handle to Library instance.
 * @param[in]                        Reference to HTTP Request object.
 * @param[out]                       Payload buffer(must be allocated by caller).
 * @param[out]                       Actual bytes read in buffer.
 * @return                           Returns REST_ENGINE_IO_COMPLETED for success,
                                     REST_ENGINE_MORE_IO_REQUIRED or Error codes.
 */
VMREST_API 
uint32_t
VmRESTGetHttpPayload(
    PVMREST_HANDLE                   pRESTHandle,
    PREST_REQUEST                    pRequest,
    char*                            response,
    uint32_t*                        bytesRead
    );


/*
 * @brief Set payload in HTTP response object.
 *
 * @param[in]                        Handle to Library instance.
 * @param[in]                        Reference to HTTP Response object.
 * @param[in]                        Payload data.
 * @param[in]                        Payload data length.
 * @param[out]                       Actual bytes written in buffer.
 * @param[out]                       Returns REST_ENGINE_IO_COMPLETED for success,
                                     REST_ENGINE_MORE_IO_REQUIRED or Error codes.
 * @return Returns 0 for success
 */
VMREST_API 
uint32_t
VmRESTSetHttpPayload(
    PVMREST_HANDLE                   pRESTHandle,
    PREST_RESPONSE*                  ppResponse,
    char const*                      buffer,
    uint32_t                         dataLen,
    uint32_t*                        bytesWritten
    );

#endif /* __VMREST_H__ */
