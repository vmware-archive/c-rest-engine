#ifndef __VMREST_H__
#define __VMREST_H__



/************** libtransport exposed API's  *****/

uint32_t
VmRestTransportInit(
    char *port
    );

uint32_t
VmRESTTransportShutdown(
    void
    );

uint32_t VmsockPosixWriteDataAtOnce(
    SSL*     ssl,
    char*    buffer,
    uint32_t bytes
    );


uint32_t VmSockPosixHandleEventsFromQueue(
    void
    );


uint32_t
VmRESTProcessIncomingData(
    char     *buffer,
    uint32_t byteRead,
    SSL*     ssl
    );


typedef struct _VM_REST_HTTP_REQUEST_PACKET *PVM_REST_HTTP_REQUEST_PACKET;

typedef struct _VM_REST_HTTP_RESPONSE_PACKET *PVM_REST_HTTP_RESPONSE_PACKET;


typedef uint32_t (*PFN_PROCESS_HTTP_GET)(
                     PVM_REST_HTTP_REQUEST_PACKET    pRequest,
                     PVM_REST_HTTP_RESPONSE_PACKET*  ppResponse
                     ); 

typedef uint32_t (*PFN_PROCESS_HTTP_POST)(
                     PVM_REST_HTTP_REQUEST_PACKET    pRequest,
                     PVM_REST_HTTP_RESPONSE_PACKET*  ppResponse
                     );

typedef uint32_t (*PFN_PROCESS_HTTP_HEAD)(
                     PVM_REST_HTTP_REQUEST_PACKET    pRequest,
                     PVM_REST_HTTP_RESPONSE_PACKET*  ppResponse
                     );

typedef uint32_t (*PFN_PROCESS_HTTP_PUT)(
                     PVM_REST_HTTP_REQUEST_PACKET    pRequest,
                     PVM_REST_HTTP_RESPONSE_PACKET*  ppResponse
                     );

typedef uint32_t (*PFN_PROCESS_HTTP_DELETE)(
                     PVM_REST_HTTP_REQUEST_PACKET    pRequest,
                     PVM_REST_HTTP_RESPONSE_PACKET*  ppResponse
                     );

typedef uint32_t (*PFN_PROCESS_HTTP_TRACE)(
                     PVM_REST_HTTP_REQUEST_PACKET    pRequest,
                     PVM_REST_HTTP_RESPONSE_PACKET*  ppResponse
                     );

typedef uint32_t (*PFN_PROCESS_HTTP_CONNECT)(
                     PVM_REST_HTTP_REQUEST_PACKET    pRequest,
                     PVM_REST_HTTP_RESPONSE_PACKET*  ppResponse
                     );

typedef struct _VMREST_ENGINE_METHODS
{
PFN_PROCESS_HTTP_GET           pfnHandleHTTP_GET;
PFN_PROCESS_HTTP_POST          pfnHandleHTTP_POST;
PFN_PROCESS_HTTP_HEAD          pfnHandleHTTP_HEAD;
PFN_PROCESS_HTTP_PUT           pfnHandleHTTP_PUT;
PFN_PROCESS_HTTP_DELETE        pfnHandleHTTP_DELETE;
PFN_PROCESS_HTTP_TRACE         pfnHandleHTTP_TRACE;
PFN_PROCESS_HTTP_CONNECT       pfnHandleHTTP_CONNECT;
}VMREST_ENGINE_METHODS, *PVMREST_ENGINE_METHODS;

/*
 * @brief Rest engine initialization
 *
 * @param[in]           Handler callbacks
 * @param[out]          void
 * @return Returns 0 for success
 */
uint32_t
VmRESTEngineInit(
    PVMREST_ENGINE_METHODS *pHandlers
    );

/*
 * @brief Rest engine shutdown
 *
 * @param[in]           void
 * @param[out]          void
 * @return Returns 0 for success
 */
void
VmRESTEngineShutdown(
    void
    );

/*
 * @brief Rest engine exposed API to handle data from raw socket
 *
 * @param[in]           char*
 * @param[in]           byteRead
 * @return Returns 0 for success
 */

uint32_t
VmRESTProcessIncomingData(
    char     *buffer,
    uint32_t byteRead,
    SSL*     ssl
    );

/* Exposed Rest engine API's */

/* httpUtils.c */


/*
 * @brief Retrieve method name associated with request http object.
 *
 * @param[in]           Reference to HTTP Request object
 * @param[out]          HTTP method present in request object.
 * @return Returns 0 for success
 */
uint32_t
VmRESTGetHttpMethod(
    PVM_REST_HTTP_REQUEST_PACKET    pRequest,
    char*                           response
    );

/*
 * @brief Retrieve URI associated with request http object.
 *
 * @param[in]           Reference to HTTP Request object.
 * @param[out]          URI present in request object.
 * @return Returns 0 for success
 */
uint32_t
VmRESTGetHttpURI(
    PVM_REST_HTTP_REQUEST_PACKET    pRequest,
    char*                           response
    );

/*
 * @brief Retrieve HTTP Version associated with request http object.
 *
 * @param[in]           Reference to HTTP Request object.
 * @param[out]          HTTP version (1.0/1.1) present in request object.
 * @return Returns 0 for success
 */
uint32_t
VmRESTGetHttpVersion(
    PVM_REST_HTTP_REQUEST_PACKET    pRequest,
    char*                           response
    );

/*
 * @brief Retrieve Value of HTTP header associated with request http object.
 *
 * @param[in]           Reference to HTTP Request object.
 * @param[in]           Header field to be retrieve.
 * @param[out]          Value of header present in request object.
 * @return Returns 0 for success
 */
uint32_t
VmRESTGetHttpHeader(
    PVM_REST_HTTP_REQUEST_PACKET    pRequest,
    char*                           header,
    char*                           response
    );

/*
 * @brief Set given value to given HTTP header in the response http object.
 *
 * @param[in]           Reference to HTTP Response object.
 * @param[in]           Header field to be set.
 * @param[in]           Value of header field to be set.
 * @return Returns 0 for success
 */
uint32_t
VmRESTSetHttpHeader(
    PVM_REST_HTTP_RESPONSE_PACKET*  ppResponse,
    char*                           header,
    char*                           value
    );

/*
 * @brief Set given value to HTTP reponse object status line.
 *
 * @param[in]           Reference to HTTP Response object.
 * @param[in]           StatusCode to be set.
 * @return Returns 0 for success
 */
uint32_t
VmRESTSetHttpStatusCode(
    PVM_REST_HTTP_RESPONSE_PACKET*  ppResponse,
    char*                           statusCode
    );

/*
 * @brief Set given HTTP Version to HTTP reponse object status line.
 *
 * @param[in]           Reference to HTTP Response object.
 * @param[in]           HTTP Version to be set.
 * @return Returns 0 for success
 */
uint32_t
VmRESTSetHttpStatusVersion(
    PVM_REST_HTTP_RESPONSE_PACKET*  ppResponse,
    char*                           version
    );

/*
 * @brief Set given reason phrase to HTTP reponse object status line.
 *
 * @param[in]           Reference to HTTP Response object.
 * @param[in]           reason phrase to be set.
 * @return Returns 0 for success
 */
uint32_t
VmRESTSetHttpReasonPhrase(
    PVM_REST_HTTP_RESPONSE_PACKET*  ppResponse,
    char*                           reasonPhrase
    );

/*
 * @brief Get payload from HTTP request object.
 *
 * @param[in]           Reference to HTTP Request object.
 * @param[out]          Payload buffer(must be allocated by caller).
 * @return Returns 0 for success
 */
uint32_t
VmRESTGetHttpPayload(
    PVM_REST_HTTP_REQUEST_PACKET    pRequest,
    char*                           response
    );


/*
 * @brief Set payload in HTTP response object.
 *
 * @param[in]           Reference to HTTP Response object.
 * @param[in]           Payload data.
 * @return Returns 0 for success
 */
uint32_t
VmRESTSetHttpPayload(
    PVM_REST_HTTP_RESPONSE_PACKET*  ppResponse,
    char*                           buffer
    );

#endif /* __VMREST_H__ */
