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
    int level
    );

uint32_t
VmRESTLogInitialize(
    PVMREST_HANDLE    pRESTHandle
    )
{
    uint32_t   dwError = 0;
   
    if (!pRESTHandle || !(pRESTHandle->pRESTConfig) || (strlen(pRESTHandle->pRESTConfig->debug_log_file) == 0))
    {
        dwError = 1;
        BAIL_ON_VMREST_ERROR(dwError); 
    }

    if ((pRESTHandle->logFile = fopen(pRESTHandle->pRESTConfig->debug_log_file, "a")) == NULL)
    {
        fprintf( stderr, "logFileName: \"%s\" open failed", pRESTHandle->pRESTConfig->debug_log_file);
        dwError = 1;
        BAIL_ON_VMREST_ERROR(dwError);
    }

cleanup: 
    return dwError;
error:
    goto cleanup;
}

void
VmRESTLogTerminate(
    PVMREST_HANDLE    pRESTHandle
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
    struct      timespec tspec = {0};
    time_t      ltime;
    struct      tm mytm = {0};
    char        logMessage[MAX_LOG_MESSAGE_LEN];
	
    va_list     va;
    const char* logLevelTag = "";

    if (!pRESTHandle)
    {
        return;
    }


#if 1
    if (level <= pRESTHandle->debugLogLevel)
    {
        va_start( va, fmt );
        vsnprintf( logMessage, sizeof(logMessage), fmt, va );
        logMessage[sizeof(logMessage)-1] = '\0';
        va_end( va );

        ltime = time(&ltime);
        logLevelTag = logLevelToTag(level);
		//TODO: FIX COMPILE ISSUE 
#ifndef WIN32
        localtime_r(&ltime, &mytm);
        snprintf(extraLogMessage, sizeof(extraLogMessage) - 1,
#else
        _snprintf(extraLogMessage, sizeof(extraLogMessage) - 1,
#endif
                  "%4d%2d%2d%2d%2d%2d.%03ld:t@%lu:%-3.7s: ",
                  mytm.tm_year+1900,
                  mytm.tm_mon+1,
                  mytm.tm_mday,
                  mytm.tm_hour,
                  mytm.tm_min,
                  mytm.tm_sec,
                  tspec.tv_nsec/NSECS_PER_MSEC,
#ifndef WIN32
                  (unsigned long) pthread_self(),
#else
			      pthread_self(),
#endif
                  logLevelTag? logLevelTag : "UNKNOWN");

         if(pRESTHandle->logFile != NULL )
         {
            fprintf(pRESTHandle->logFile, "%s%s\n", extraLogMessage, logMessage);
            fflush( pRESTHandle->logFile );
         }
         else
         {
            fprintf(stderr, "%s%s\n", extraLogMessage, logMessage);
            fflush( stderr );
         }
    }
#endif
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

