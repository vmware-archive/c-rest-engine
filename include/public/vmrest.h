#ifndef __VMREST_H__
#define __VMREST_H__


#endif /* __VMREST_H__ */

uint32_t
VmRestTransportInit(
    char *addr,
    char *port
    );

uint32_t VmSockPosixHandleEventsFromQueue(
    void
    );


/*
 * @brief Rest engine initialization
 *
 * @param[in]           void
 * @param[out]          void
 * @return Returns 0 for success
 */
uint32_t 
VmRESTEngineInit(
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


