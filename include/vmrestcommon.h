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

#include <vmrest.h>

#ifdef WIN32
#include <pthread.h>
#include "openssl\ssl.h"
#include "openssl\err.h"
#endif

#ifndef __VMRESTCOMMON_H__
#define __VMRESTCOMMON_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    VMREST_LOG_TYPE_CONSOLE = 0,
    VMREST_LOG_TYPE_FILE,
    VMREST_LOG_TYPE_SYSLOG
} VMREST_LOG_TYPE;

#ifndef _VMREST_LOG_LEVEL_DEFINED_
#define _VMREST_LOG_LEVEL_DEFINED_
typedef enum
{
   VMREST_LOG_LEVEL_EMERGENCY = 0,
   VMREST_LOG_LEVEL_ALERT,
   VMREST_LOG_LEVEL_CRITICAL,
   VMREST_LOG_LEVEL_ERROR,
   VMREST_LOG_LEVEL_WARNING,
   VMREST_LOG_LEVEL_NOTICE,
   VMREST_LOG_LEVEL_INFO,
   VMREST_LOG_LEVEL_DEBUG
} VMREST_LOG_LEVEL;
#endif

uint32_t
VmRESTLogInitialize(
    PVMREST_HANDLE                   pRESTHandle
    );

void
VmRESTLogTerminate(
   PVMREST_HANDLE                    pRESTHandle
   );

void
VmRESTLog(
   PVMREST_HANDLE                    pRESTHandle,
   VMREST_LOG_LEVEL                  level,
   const char*                       fmt,
   ...);


#if 1 //ndef WIN32


#define VMREST_LOG_( Level, Format, ... )          \
    do                                             \
    {                                              \
        VmRESTLog(                                 \
               Level,                              \
               Format,                             \
               ##__VA_ARGS__);                     \
    } while (0)

#define VMREST_LOG_GENERAL_( pRESTHandle,Level, Format, ... ) \
    VMREST_LOG_( pRESTHandle, Level, Format, ##__VA_ARGS__ )

#define VMREST_LOG_WARNING( pRESTHandle,Format, ... )         \
    VMREST_LOG_GENERAL_( pRESTHandle, VMREST_LOG_LEVEL_WARNING, Format, ##__VA_ARGS__ )
#define VMREST_LOG_INFO( pRESTHandle,Format, ... )            \
    VMREST_LOG_GENERAL_( pRESTHandle,VMREST_LOG_LEVEL_INFO, Format, ##__VA_ARGS__ )
#define VMREST_LOG_VERBOSE( pRESTHandle,Format, ... )         \
    VMREST_LOG_GENERAL_( pRESTHandle,VMREST_LOG_LEVEL_DEBUG, Format, ##__VA_ARGS__ )


#define VMREST_LOG_DEBUG(pRESTHandle,Format, ... )\
    VMREST_LOG_GENERAL_(                          \
        pRESTHandle,                              \
        VMREST_LOG_LEVEL_DEBUG,                   \
    Format " [file: %s][line: %d]",               \
    ##__VA_ARGS__, __FILE__, __LINE__ )


#define VMREST_LOG_ERROR(pRESTHandle,Format, ... )\
    VMREST_LOG_GENERAL_(                          \
        pRESTHandle,                              \
        VMREST_LOG_LEVEL_ERROR,                   \
    Format " [file: %s][line: %d]",               \
    ##__VA_ARGS__, __FILE__, __LINE__ )

#else

#define VMREST_LOG_( pRESTHandle,Level, Format, ... )\
    do                                               \
    {                                                \
        VmRESTLog(                                   \
               pRESTHandle,                          \
               Level,                                \
               Format,                               \
               ##__VA_ARGS__);                       \
    } while (0)

#define VMREST_LOG_GENERAL_( pRESTHandle, Level, Format, ... ) \
    VMREST_LOG_( pRESTHandle,Level, Format, ##__VA_ARGS__ )

#endif

#define VMW_REST_PORT                 (81)
#define VMW_REST_DEFAULT_THREAD_COUNT (5)

#ifndef WIN32

#define ERROR_BUSY                    200
#define ERROR_POSSIBLE_DEADLOCK       201
#define VMREST_UDP_PACKET_SIZE        512 
#define ERROR_IO_PENDING              300
#define ERROR_INVALID_MESSAGE         301
#define ERROR_SHUTDOWN_IN_PROGRESS    600
#define WSAECONNRESET                 10054

#endif



/*
 * @brief Allocation of heap memory for rest engine.
 *
 * @param[in]                        size of memory to be allocated
 * @param[out]                       pointer to allocated memory
 * @return Returns 0 for success
 */
uint32_t
VmRESTAllocateMemory(
    size_t                           dwSize,
    void**                           ppMemory
    );

/*
 * @brief Free of head memory for rest engine.
 *
 * @param[in]                        pointer to allocated memory
 * @return Returns 0 for success
 */

void
VmRESTFreeMemory(
    void*                            pMemory
    );

/*
 * @brief Rest engine implementation of itoa function.
 * @param[in]                        integer number
 * @param[in]                        pointer to hold result
 * @return Returns 1 for failure, 0 for success,
 */

uint32_t
VmRESTUtilsConvertInttoString(
    int                              num,
    char*                            str
    );

char
VmRESTUtilsGetLastChar(
    char*                            src
    );

typedef pthread_t VMREST_THREAD;

typedef VMREST_THREAD* PVMREST_THREAD;


typedef struct _VMREST_MUTEX
{
    uint8_t                          bInitialized;
    pthread_mutex_t                  critSect;

} VMREST_MUTEX, *PVMREST_MUTEX;

typedef struct _VMREST_COND
{
    uint8_t                          bInitialized;
    pthread_cond_t                   cond;

} VMREST_COND, *PVMREST_COND;

typedef struct _VMREST_RWLOCK
{
    pthread_key_t                    readKey;
    pthread_key_t                    writeKey;
    pthread_rwlock_t                 rwLock;

} VMREST_RWLOCK, *PVMREST_RWLOCK;


typedef struct _VMREST_SOCK_CONTEXT
{
    PVMREST_MUTEX                    pMutex;
    uint8_t                          bShutdown;
    PVM_SOCKET                       pListenerUDP;
    PVM_SOCKET                       pListenerUDP6;
    PVM_SOCKET                       pListenerTCP;
    PVM_SOCKET                       pListenerTCP6;
    PVM_SOCK_EVENT_QUEUE             pEventQueue;
    PVMREST_THREAD*                  pWorkerThreads;
    uint32_t                         dwNumThreads;

} VMREST_SOCK_CONTEXT, *PVMREST_SOCK_CONTEXT;

typedef struct _SOCK_SSL_INFO
{
    SSL_CTX*                         sslContext;
    uint32_t                         isSecure;
    uint32_t                         isQueueInUse;
    uint32_t                         isCertSet;
    uint32_t                         isKeySet;

} VM_SOCK_SSL_INFO, *PVM_SOCK_SSL_INFO;

/*********** REST engine Configuration struct *************/

typedef struct _REST_CONFIG
{
    char                             ssl_certificate[MAX_PATH_LEN];
    char                             ssl_key[MAX_PATH_LEN];
    char                             server_port[MAX_SERVER_PORT_LEN];
    char                             debug_log_file[MAX_PATH_LEN];
    char                             client_count[MAX_CLIENT_ALLOWED_LEN];
    char                             worker_thread_count[MAX_WORKER_COUNT_LEN];
} VM_REST_CONFIG, *PVM_REST_CONFIG;

typedef struct _REST_ENG_GLOBALS *PREST_ENG_GLOBALS;

typedef struct _VMREST_HANDLE
{
    int                              debugLogLevel;
    FILE*                            logFile;
    PVM_SOCK_PACKAGE                 pPackage;
    PVM_SOCK_SSL_INFO                pSSLInfo;
    PREST_PROCESSOR                  pHttpHandler;
    PREST_ENG_GLOBALS                pInstanceGlobal;
    PVMREST_SOCK_CONTEXT             pSockContext;
    PVM_REST_CONFIG                  pRESTConfig;                     

} VMREST_HANDLE;

typedef struct _VM_WORKER_THREAD_DATA
{
    PVMREST_SOCK_CONTEXT             pSockContext;
    PVMREST_HANDLE                   pRESTHandle;

}VM_WORKER_THREAD_DATA, *PVM_WORKER_THREAD_DATA;

typedef DWORD (VmRESTStartRoutine)(PVOID);
typedef VmRESTStartRoutine* PVMREST_START_ROUTINE;

typedef struct _VMREST_THREAD_START_INFO
{
    VmRESTStartRoutine*              pStartRoutine;
    PVOID                            pArgs;

} VMREST_THREAD_START_INFO, *PVMREST_THREAD_START_INFO;


/**** sockInterface.c Exposed API's ****/

DWORD
VmRESTInitProtocolServer(
    PVMREST_HANDLE                   pRESTHandle
    );

VOID
VmRESTShutdownProtocolServer(
    PVMREST_HANDLE                   pRESTHandle
    );

uint32_t
VmsockPosixGetXBytes(
    PVMREST_HANDLE                   pRESTHandle,
    uint32_t                         bytesRequested,
    char*                            appBuffer,
    PVM_SOCKET                       pSocket,
    uint32_t*                        bytesRead,
    uint8_t                          shouldBlock
    );

uint32_t
VmSockPosixAdjustProcessedBytes(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket,
    uint32_t                         dataSeen
    );

uint32_t
VmSockPosixDecrementProcessedBytes(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket,
    uint32_t                         offset
    );

uint32_t
VmsockPosixWriteDataAtOnce(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket,
    char*                            buffer,
    uint32_t                         bytes
    );

uint32_t
VmRESTCommonGetPeerInfo(
    PVMREST_HANDLE                   pRESTHandle,
    PVM_SOCKET                       pSocket,
    char*                            pIpAddress,
    uint32_t                         nLen,
    int*                             pPortNo
    );

uint32_t
VmRESTProcessIncomingData(
    PVMREST_HANDLE                   pRESTHandle,
    char*                            buffer,
    uint32_t                         byteRead,
    PVM_SOCKET                       pSocket
    );

void
VmRESTSetConfig(
   PVM_REST_CONFIG                   pRESTConfig         
   );

void
VmRESTUnSetConfig(
    void
    );

/************ threads.c API's ****************/

DWORD
VmRESTAllocateMutex(
    PVMREST_MUTEX*                   ppMutex
    );

DWORD
VmRESTInitializeMutexContent(
    PVMREST_MUTEX                    pMutex
    );

VOID
VmRESTFreeMutex(
    PVMREST_MUTEX                    pMutex
    );

VOID
VmRESTFreeMutexContent(
    PVMREST_MUTEX                    pMutex
    );

DWORD
VmRESTLockMutex(
    PVMREST_MUTEX                    pMutex
    );

DWORD
VmRESTUnlockMutex(
    PVMREST_MUTEX                    pMutex
    );

BOOLEAN
VmRESTIsMutexInitialized(
    PVMREST_MUTEX                    pMutex
    );

DWORD
VmRESTAllocateCondition(
    PVMREST_COND*                    ppCondition
    );

DWORD
VmRESTInitializeConditionContent(
    PVMREST_COND                     pCondition
    );

VOID
VmRESTFreeCondition(
    PVMREST_COND                     pCondition
    );

VOID
VmRESTFreeConditionContent(
    PVMREST_COND                     pCondition
    );

DWORD
VmRESTConditionWait(
    PVMREST_COND                     pCondition,
    PVMREST_MUTEX                    pMutex
    );

DWORD
VmRESTConditionTimedWait(
    PVMREST_COND                     pCondition,
    PVMREST_MUTEX                    pMutex,
    DWORD                            dwMilliseconds
    );

DWORD
VmRESTConditionSignal(
    PVMREST_COND                     pCondition
    );

DWORD
VmRESTCreateThread(
    PVMREST_THREAD                   pThread,
    BOOLEAN                          bDetached,
    VmRESTStartRoutine*              pStartRoutine,
    PVOID                            pArgs
    );

DWORD
VmRESTThreadJoin(
    PVMREST_THREAD                   pThread,
    PDWORD                           pRetVal
    );

VOID
VmRESTFreeThread(
    PVMREST_THREAD                   pThread
    );

DWORD
VmRESTAllocateRWLock(
    PVMREST_RWLOCK*                  ppLock
    );

VOID
VmRESTFreeRWLock(
    PVMREST_RWLOCK                   pLock
    );

void
VmRESTLockRead(
    PVMREST_RWLOCK                   pLock
    );

int
VmRESTTryLockRead(
    PVMREST_RWLOCK                   pLock
    );

void
VmRESTUnlockRead(
    PVMREST_RWLOCK                   pLock
    );

void
VmRESTLockWrite(
    PVMREST_RWLOCK                   pLock
    );

int
VmRESTTryLockWrite(
    PVMREST_RWLOCK                   pLock
    );

void
VmRESTUnlockWrite(
    PVMREST_RWLOCK                   pLock
    );

/************ threads.c API's End ****************/


#ifdef __cplusplus
}
#endif

#endif /*  __VMRESTCOMMON_H__ */
