/* * Copyright © 2012-2015 VMware, Inc.  All Rights Reserved.
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

static
void
VmRestJoinThread(
    PVMREST_THREAD                   pThread
    );

static
void
VmRestFreeThreadContents(
    PVMREST_THREAD                   pThread
    );

uint32_t
VmRestSpawnThreads(
    PFN_VMREST_THR_ROUTINE           pThrRoutine,
    PVMREST_THREAD*                  ppThreadpool,
    uint32_t                         maxWorkerThread
    )
{
    uint32_t                         dwError = ERROR_VMREST_SUCCESS;
    uint32_t                         thrcount = maxWorkerThread;
    PVMREST_THREAD                   pThreadpool = NULL;
    uint32_t                         iThr = 0;

    dwError = VmRESTAllocateMemory(
                   sizeof(VMREST_THREAD) * thrcount,
                   (void**)&pThreadpool
                   );
    BAIL_ON_VMREST_ERROR(dwError);

    for (; iThr < thrcount; iThr++)
    {
        PVMREST_THREAD pThread = &pThreadpool[iThr];
        PVMREST_THREAD_DATA pThrData = NULL;

        dwError = VmRESTAllocateMemory(
                      sizeof(VMREST_THREAD_DATA),
                      (void**)&pThrData
                      );
        BAIL_ON_VMREST_ERROR(dwError);

        dwError = pthread_mutex_init(
                      &(pThrData->mutex),
                      NULL
                      );
        BAIL_ON_VMREST_ERROR(dwError);

        pThrData->pMutex = &pThrData->mutex;

        dwError = pthread_cond_init(
                      &(pThrData->cond),
                      NULL
                      );
        BAIL_ON_VMREST_ERROR(dwError);

        pThrData->pCond = &pThrData->cond;

        dwError = pthread_create(
                      &pThread->thr,
                      NULL,
                      pThrRoutine,
                      pThrData
                      );
        BAIL_ON_VMREST_ERROR(dwError);

        pThread->pThread = &pThread->thr;
        pThread->data = pThrData;
    }

    *ppThreadpool = pThreadpool;

cleanup:

    return dwError;

error:

    *ppThreadpool = NULL;
    if (pThreadpool)
    {
        VmRestFreeThreadpool(pThreadpool, thrcount);
    }
    goto cleanup;
}

void*
VmRestWorkerThread(
    void*                            pArgs
    )
{
    /* TODO ::  Pass thread data to socket handler
    PVMREST_THREAD_DATA pThrData = (PVMREST_THREAD_DATA)pArgs; */

    VmSockPosixHandleEventsFromQueue();

    return NULL;
}

void
VmRestFreeThreadpool(
    PVMREST_THREAD                   pThreadpool,
    uint32_t                         threadcount
    )
{
    uint32_t                         iThr = 0;

    if (pThreadpool)
    {
        for (iThr = 0; iThr < threadcount; iThr++)
        {
            PVMREST_THREAD pThread = &pThreadpool[iThr];
            if (pThread->pThread)
            {
                VmRestJoinThread(pThread);
            }

            VmRestFreeThreadContents(pThread);
        }

        VmRESTFreeMemory(pThreadpool);
    }
}

static
void
VmRestJoinThread(
    PVMREST_THREAD                   pThread
    )
{
    if (pThread && pThread->thr)
    {
        pthread_join(pThread->thr, NULL);
    }
}

static
void
VmRestFreeThreadContents(
    PVMREST_THREAD                   pThread
    )
{
    PVMREST_THREAD_DATA pThrData = pThread->data;

    if (pThrData->pCond)
    {
        pthread_cond_destroy(&pThrData->cond);
        pThrData->pCond = NULL;
    }

    if (pThrData->pMutex)
    {
        pthread_mutex_destroy(&pThrData->mutex);
        pThrData->pMutex = NULL;
    }
    if (pThrData)
    {
        VmRESTFreeMemory(pThrData);
    }
}
