#ifndef __VMREST_H__
#define __VMREST_H__

uint32_t
VmRestTransportInit(
    char *addr,
    char *port
    );

uint32_t VmSockPosixHandleEventsFromQueue(
    void
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
    uint32_t byteRead
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


#endif /* __VMREST_H__ */
