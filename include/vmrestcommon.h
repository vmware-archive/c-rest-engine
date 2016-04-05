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
#define VMREST_LOG_DEBUG( Format, ... )       \
    VMREST_LOG_GENERAL_(                      \
        VMREST_LOG_LEVEL_DEBUG,               \
    Format " [file: %s][line: %d]",     \
    ##__VA_ARGS__, __FILE__, __LINE__ )


/*
 * @brief Allocation of heap memory for rest engine.
 *
 * @param[in]           size of memory to be allocated
 * @param[out]          pointer to allocated memory
 * @return Returns 0 for success
 */
uint32_t
VmRESTAllocateMemory(
    size_t   dwSize,
    void**   ppMemory
    );

/*
 * @brief Free of head memory for rest engine.
 *
 * @param[in]           pointer to allocated memory
 * @return Returns 0 for success
 */

void
VmRESTFreeMemory(
    void*  pMemory
    );

/*
 * @brief Rest engine implementation of itoa function.
 * @param[in]           integer number
 * @param[in]           pointer to hold result
 * @return Returns 1 for failure, 0 for success,
 */

uint32_t
my_itoa(
    int    num,
    char*  str
    );


#ifdef __cplusplus
}
#endif

#endif /*  __VMRESTCOMMON_H__ */
