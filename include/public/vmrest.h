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



