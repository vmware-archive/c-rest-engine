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

#ifdef WIN32
#include <pthread.h>
#endif

#ifndef __VMRESTCOMMON_H__
#define __VMRESTCOMMON_H__

#ifdef __cplusplus
extern "C" {
#endif

//extern int  vmrest_syslog;
//extern int  vmrest_debug;

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
	char*   pLogFileName
	);

void
VmRESTLogTerminate();

void
VmRESTLog(
   VMREST_LOG_LEVEL level,
   const char*      fmt,
   ...);

/*
typedef struct _VMREST_LOG_HANDLE* PVMREST_LOG_HANDLE;

extern PVMREST_LOG_HANDLE gpVMRESTLogHandle;
extern VMREST_LOG_LEVEL   gVMRESTLogLevel;
extern HANDLE           gpEventLog;
extern VMREST_LOG_TYPE    gVMRESTLogType;
extern VMREST_LOG_LEVEL VMRESTLogGetLevel();
*/

#ifndef WIN32


#define VMREST_LOG_( Level, Format, ... ) \
    do                                             \
    {                                              \
        VmRESTLog(                                   \
               Level,                              \
               Format,                             \
               ##__VA_ARGS__);                     \
    } while (0)

#define VMREST_LOG_GENERAL_( Level, Format, ... ) \
    VMREST_LOG_( Level, Format, ##__VA_ARGS__ )

#define VMREST_LOG_WARNING( Format, ... ) \
    VMREST_LOG_GENERAL_( VMREST_LOG_LEVEL_WARNING, Format, ##__VA_ARGS__ )
#define VMREST_LOG_INFO( Format, ... )    \
    VMREST_LOG_GENERAL_( VMREST_LOG_LEVEL_INFO, Format, ##__VA_ARGS__ )
#define VMREST_LOG_VERBOSE( Format, ... ) \
    VMREST_LOG_GENERAL_( VMREST_LOG_LEVEL_DEBUG, Format, ##__VA_ARGS__ )


#define VMREST_LOG_DEBUG(Format, ... )       \
    VMREST_LOG_GENERAL_(                      \
        VMREST_LOG_LEVEL_DEBUG,               \
    Format " [file: %s][line: %d]",     \
    ##__VA_ARGS__, __FILE__, __LINE__ )


#define VMREST_LOG_ERROR(Format, ... )       \
    VMREST_LOG_GENERAL_(                      \
        VMREST_LOG_LEVEL_ERROR,               \
    Format " [file: %s][line: %d]",     \
    ##__VA_ARGS__, __FILE__, __LINE__ )

#else

#define VMREST_LOG_( Level, Format, ... ) \
    do                                             \
    {                                              \
        VmRESTLog(                                   \
               Level,                              \
               Format,                             \
               ##__VA_ARGS__);                     \
    } while (0)

#define VMREST_LOG_GENERAL_( Level, Format, ... ) \
    VMREST_LOG_( Level, Format, ##__VA_ARGS__ )

#define VMREST_LOG_ERROR( Format, ... )   \
    VMREST_LOG_GENERAL_( VMREST_LOG_LEVEL_ERROR, Format, ##__VA_ARGS__ )
#define VMREST_LOG_WARNING( Format, ... ) \
    VMREST_LOG_GENERAL_( VMREST_LOG_LEVEL_WARNING, Format, ##__VA_ARGS__ )
#define VMREST_LOG_INFO( Format, ... )    \
    VMREST_LOG_GENERAL_( VMREST_LOG_LEVEL_INFO, Format, ##__VA_ARGS__ )
#define VMREST_LOG_VERBOSE( Format, ... ) \
    VMREST_LOG_GENERAL_( VMREST_LOG_LEVEL_DEBUG, Format, ##__VA_ARGS__ )


#define VMREST_LOG_DEBUG(Format, ... )       \
    VMREST_LOG_GENERAL_(                      \
        VMREST_LOG_LEVEL_DEBUG,               \
    Format " [file: %s][line: %d]",     \
    ##__VA_ARGS__, __FILE__, __LINE__ )

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

//#ifndef WIN32
typedef pthread_t VMREST_THREAD;
//#endif

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

/**** TODO: This is not proper place for this struct ****/
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



typedef struct _VM_SOCK_PACKAGE *PVM_SOCK_PACKAGE;
typedef struct _REST_ENG_GLOBALS *PREST_ENG_GLOBALS;
typedef struct _REST_PROCESSOR *PREST_PROCESSOR;

typedef struct _VMREST_HANDLER
{
    int                              debugLogLevel;
    PVM_SOCK_PACKAGE                 pPackage;
    PVM_SOCK_SSL_INFO                pSSLInfo;
    PREST_PROCESSOR                  pHttpHandler;
    PREST_ENG_GLOBALS                pInstanceGlobal;
    PVMREST_SOCK_CONTEXT             pSockContext;
    PVM_REST_CONFIG                  pRESTConfig;                     

} VMREST_HANDLER,*PVMREST_HANDLER;

typedef struct _VM_WORKER_THREAD_DATA
{
    PVMREST_SOCK_CONTEXT             pSockContext;
    PVMREST_HANDLER                  pRESTHandler;

}VM_WORKER_THREAD_DATA, *PVM_WORKER_THREAD_DATA;



#define VMW_REST_PORT                 (81)

#define VMW_REST_DEFAULT_THREAD_COUNT (5)

#ifndef WIN32
#define ERROR_BUSY                    200
#define ERROR_POSSIBLE_DEADLOCK       201
#define VMREST_UDP_PACKET_SIZE        512 
#define ERROR_IO_PENDING              300
#define ERROR_INVALID_MESSAGE         301
#define WSAECONNRESET                 10054
#define ERROR_SHUTDOWN_IN_PROGRESS    600

#endif

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
    PVMREST_HANDLER                  pRESTHandler
    );

VOID
VmRESTShutdownProtocolServer(
    PVMREST_HANDLER                  pRESTHandler
    );

uint32_t
VmsockPosixGetXBytes(
    PVMREST_HANDLER                  pRESTHandler,
    uint32_t                         bytesRequested,
    char*                            appBuffer,
    PVM_SOCKET                       pSocket,
    uint32_t*                        bytesRead,
    uint8_t                          shouldBlock
    );

uint32_t
VmSockPosixAdjustProcessedBytes(
    PVMREST_HANDLER                  pRESTHandler,
    PVM_SOCKET                       pSocket,
    uint32_t                         dataSeen
    );

uint32_t
VmsockPosixWriteDataAtOnce(
    PVMREST_HANDLER                  pRESTHandler,
    PVM_SOCKET                       pSocket,
    char*                            buffer,
    uint32_t                         bytes
    );

uint32_t
VmRESTProcessIncomingData(
    PVMREST_HANDLER                  pRESTHandler,
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
