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

uint32_t VmRESTEngineInit(void);
