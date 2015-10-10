
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
    

    dwError = pthread_create(
        thr, 
        NULL,
        &(VmRestWorkerThread),
        NULL
    );
    return dwError;
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
    gRESTEngGlobals.nthreads = count;
    // kaushik
    write(1,"\nCreating Threads", 20);    
    for (i = 0; i < 5; i++)
    {
        VmRestCreateThread(&(gRESTEngGlobals.threadpool[i]),NULL, NULL);
    }
    return 0;
}


