
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

#include <includes.h>

uint32_t 
VmRestCreateThread(
    pthread_t *thr,
    PVMREST_START_ROUTINE pStartRoutine,
    void * pArgs
)
{
    uint32_t dwError = 0;
    if ((!pStartRoutine) || (!thr))
    {   
        dwError = ERROR_NOT_SUPPORTED; 
        BAIL_ON_POSIX_THREAD_ERROR(dwError);

    }
    
    dwError = pthread_create(
        thr, 
        NULL,
        (pStartRoutine),
        NULL
    );
    BAIL_ON_POSIX_THREAD_ERROR(dwError);

cleanup: 
    return dwError;
error:
    goto cleanup;
}


uint32_t 
VmRestJoinThread(
    pthread_t thr
    )
{
    

    return 0;
} 


void* VmRestWorkerThread(
    void * Args
    )
{
   
    VmSockPosixHandleEventsFromQueue();
    return NULL;   
}

uint32_t VmRestSpawnThreads(
    uint32_t count
    )
{
    uint32_t i = 0;
    uint32_t  dwError = 0;
    PREST_ENG_THREAD thr = NULL;

 
    gRESTEngGlobals.nthreads = 0;

    /* For debug purpose - will remove */
    write(1,"\nCreating Threads", 20);    
    for (i = 0; i < WORKER_THREAD_COUNT; i++)
    {  
        dwError = VmRESTAllocateMemory(sizeof(PREST_ENG_THREAD), (void *)&thr);
        BAIL_ON_POSIX_THREAD_ERROR(dwError);
     
        gRESTEngGlobals.threadpool[i] = thr;
        gRESTEngGlobals.nthreads += 1;
        VmRestCreateThread(&(thr->thr),&(VmRestWorkerThread), NULL);
    }
    
cleanup:
    return dwError;
error:
    goto cleanup;
}


