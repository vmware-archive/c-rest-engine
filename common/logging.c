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

#include "includes.h"

#define NSECS_PER_MSEC        1000000
#define EXTRA_LOG_MESSAGE_LEN 128
#define MAX_LOG_MESSAGE_LEN   4096

static const char *
logLevelToTag(
    int                              level
    );

static 
int
logLevelToSysLogLevel(
    int                              level
    );

uint32_t
VmRESTLogInitialize(
    PVMREST_HANDLE                   pRESTHandle
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;

    if (!pRESTHandle || !pRESTHandle->pRESTConfig)
    {
        dwError = REST_ENGINE_FAILURE;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    if ((!(IsNullOrEmptyString(pRESTHandle->pRESTConfig->pszDebugLogFile))) && (pRESTHandle->pRESTConfig->useSysLog == FALSE))
    {
        if ((pRESTHandle->logFile = fopen(pRESTHandle->pRESTConfig->pszDebugLogFile, "a")) == NULL)
        {
            fprintf( stderr, "logFileName: \"%s\" open failed", pRESTHandle->pRESTConfig->pszDebugLogFile);
            dwError = REST_ENGINE_FAILURE;
        }
    }
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:

    return dwError;

error:

    goto cleanup;

}

void
VmRESTLogTerminate(
    PVMREST_HANDLE                   pRESTHandle
    )
{
    if (pRESTHandle && pRESTHandle->logFile != NULL)
    {
       fclose(pRESTHandle->logFile);
       pRESTHandle->logFile = NULL;
    }
}

void
VmRESTLog(
    PVMREST_HANDLE    pRESTHandle,
    VMREST_LOG_LEVEL  level,
    const char*       fmt,
    ...)
{
    char        extraLogMessage[EXTRA_LOG_MESSAGE_LEN] = {0};
    char        logMessage[MAX_LOG_MESSAGE_LEN];
    struct      tm* tm_info = NULL;
    struct      timeval tv = {0};
	
    va_list     va;
    const char* logLevelTag = "";
    int sysLogLevel = 0;

    if (!pRESTHandle || !pRESTHandle->pRESTConfig)
    {
        return;
    }

    if (level <= pRESTHandle->debugLogLevel)
    {
        va_start( va, fmt );
        vsnprintf( logMessage, sizeof(logMessage), fmt, va );
        logMessage[sizeof(logMessage)-1] = '\0';
        va_end( va );
        gettimeofday(&tv, NULL);

        tm_info = localtime(&tv.tv_sec);
        logLevelTag = logLevelToTag(level);
        strftime(extraLogMessage, sizeof(extraLogMessage) - 1, "%F %T", tm_info);   

        if (pRESTHandle->pRESTConfig->useSysLog)
        {
            sysLogLevel = logLevelToSysLogLevel(level);
            syslog(sysLogLevel, "%s:%lu t@%lu %-3.7s: %s\n", extraLogMessage, (long unsigned)(tv.tv_usec), (unsigned long) pthread_self(),(logLevelTag? logLevelTag : "UNKNOWN"),logMessage);
        }
        else if (pRESTHandle->logFile != NULL)
        {
            fprintf(pRESTHandle->logFile, "%s:%lu t@%lu %-3.7s: %s\n", extraLogMessage, (long unsigned)(tv.tv_usec), (unsigned long) pthread_self(),(logLevelTag? logLevelTag : "UNKNOWN"),logMessage);
            fflush( pRESTHandle->logFile );
        }
    }
}

static const char *
logLevelToTag(
    int level
    )
{
   switch( level )
   {
       case VMREST_LOG_LEVEL_ERROR:
            return "ERROR";
       case VMREST_LOG_LEVEL_WARNING:
            return "WARNING";
       case VMREST_LOG_LEVEL_INFO:
            return "INFO";
       case VMREST_LOG_LEVEL_DEBUG:
            return "DEBUG";
      default:
            return "DEBUG";
   }
}

static int
logLevelToSysLogLevel(
   int level)
{
   switch( level )
   {
      case VMREST_LOG_LEVEL_ERROR:
         return LOG_ERR;
      case VMREST_LOG_LEVEL_WARNING:
         return LOG_WARNING;
      case VMREST_LOG_LEVEL_DEBUG:
         return LOG_DEBUG;
      case VMREST_LOG_LEVEL_INFO:
         return LOG_INFO;
      default:
         return LOG_ERR;
   }
}


