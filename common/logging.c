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

    if (pRESTHandle->pRESTConfig->useSysLog)
    {
        /**** Use syslog ****/
        openlog(pRESTHandle->pRESTConfig->pszDaemonName, 0, LOG_DAEMON);
        setlogmask(LOG_UPTO(logLevelToSysLogLevel(pRESTHandle->debugLogLevel)));
    }
    else
    {
        if ((pRESTHandle->logFile = fopen(pRESTHandle->pRESTConfig->pszDebugLogFile, "a")) == NULL)
        {
            fprintf( stderr, "logFileName: \"%s\" open failed", pRESTHandle->pRESTConfig->pszDebugLogFile);
            dwError = 1;
            BAIL_ON_VMREST_ERROR(dwError);
        }
    }   

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
    if (pRESTHandle && pRESTHandle->pRESTConfig->useSysLog)
    {
        closelog();
    }
    else if (pRESTHandle && pRESTHandle->logFile != NULL)
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
    struct      timespec tspec = {0};
    time_t      ltime;
    struct      tm mytm = {0};
    char        logMessage[MAX_LOG_MESSAGE_LEN];
	
    va_list     va;
    const char* logLevelTag = "";
    int sysLogLevel = 0;

    if (!pRESTHandle)
    {
        return;
    }

    if (level <= pRESTHandle->debugLogLevel)
    {
        va_start( va, fmt );
        vsnprintf( logMessage, sizeof(logMessage), fmt, va );
        logMessage[sizeof(logMessage)-1] = '\0';
        va_end( va );
        ltime = time(&ltime);
        logLevelTag = logLevelToTag(level);
        localtime_r(&ltime, &mytm);
        snprintf(extraLogMessage, sizeof(extraLogMessage) - 1,
                  "%4d%2d%2d%2d%2d%2d.%03ld:t@%lu:%-3.7s: ",
                  mytm.tm_year+1900,
                  mytm.tm_mon+1,
                  mytm.tm_mday,
                  mytm.tm_hour,
                  mytm.tm_min,
                  mytm.tm_sec,
                  tspec.tv_nsec/NSECS_PER_MSEC,
                  (unsigned long) pthread_self(),
                  logLevelTag? logLevelTag : "UNKNOWN");

        if (pRESTHandle->pRESTConfig->useSysLog)
        {
            sysLogLevel = logLevelToSysLogLevel(level);
            snprintf(extraLogMessage, sizeof(extraLogMessage) - 1, "t@%lu: ", (unsigned long) pthread_self());
            syslog(sysLogLevel, "%s: %s%s", logLevelToTag(level), extraLogMessage, logMessage);
        }
        else if (pRESTHandle->logFile != NULL)
        {
            fprintf(pRESTHandle->logFile, "%s%s\n", extraLogMessage, logMessage);
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


