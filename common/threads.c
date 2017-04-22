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

/*
 * Module   : Threads.c
 *
 * Abstract :
 *
 *            VMware REST threading implementation
 *
 *            Common Utilities.
 *
 *            Threading
 *
 */

#include "includes.h"

static
void
VmRESTFreeLockCount(
    void*                            pkeyData
    );

static
int*
VmRESTGetLockKey(
    pthread_key_t*                   pLockKey
    );

DWORD
VmRESTAllocateMutex(
    PVMREST_MUTEX*                   ppMutex
)
{
    DWORD                            dwError = ERROR_SUCCESS;
    PVMREST_MUTEX                    pVmRESTMutex = NULL;

    if ( ppMutex == NULL )
    {
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMREST_ERROR(dwError);
    }

    dwError = VmRESTAllocateMemory(
                  sizeof(VMREST_MUTEX),
                  ((PVOID*)&pVmRESTMutex)
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTInitializeMutexContent( pVmRESTMutex );
    BAIL_ON_VMREST_ERROR(dwError);

    *ppMutex = pVmRESTMutex;
    pVmRESTMutex = NULL;

error:
    if (pVmRESTMutex)
    {
        VmRESTFreeMemory(pVmRESTMutex);
        pVmRESTMutex = NULL;
    }

    return dwError;
}

DWORD
VmRESTInitializeMutexContent(
    PVMREST_MUTEX                    pMutex
)
{
    DWORD                            dwError = ERROR_SUCCESS;

    if ( ( pMutex == NULL ) || ( pMutex->bInitialized != FALSE ) )
    {
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMREST_ERROR(dwError);
    }

    memset( &(pMutex->critSect), 0, sizeof(pthread_mutex_t) );

    dwError = pthread_mutex_init(
                  &(pMutex->critSect),
                  NULL
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    pMutex->bInitialized = TRUE;

error:

    return dwError;
}

VOID
VmRESTFreeMutex(
    PVMREST_MUTEX                    pMutex
)
{
    VmRESTFreeMutexContent(pMutex);
    if (pMutex)
    { 
        VmRESTFreeMemory(pMutex);
        pMutex = NULL;
    }
}

VOID
VmRESTFreeMutexContent(
    PVMREST_MUTEX                    pMutex
)
{
    if ( ( pMutex != NULL ) &&  ( pMutex->bInitialized != FALSE ) )
    {
        pthread_mutex_destroy(&(pMutex->critSect));
        pMutex->bInitialized = FALSE;
    }
}

DWORD
VmRESTLockMutex(
    PVMREST_MUTEX                    pMutex
)
{
    DWORD                            dwError = ERROR_SUCCESS;

    if (
        ( pMutex == NULL )
        ||
        (pMutex->bInitialized == FALSE)
        )
    {
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMREST_ERROR(dwError);
    }

    dwError = pthread_mutex_lock( &(pMutex->critSect) );
    BAIL_ON_VMREST_ERROR(dwError);

error:

    return dwError;
}

DWORD
VmRESTUnlockMutex(
    PVMREST_MUTEX                    pMutex
)
{
    DWORD                            dwError = ERROR_SUCCESS;

    if (
        ( pMutex == NULL )
        ||
        (pMutex->bInitialized == FALSE)
        )
    {
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMREST_ERROR(dwError);
    }

    dwError = pthread_mutex_unlock( &(pMutex->critSect) );
    BAIL_ON_VMREST_ERROR(dwError);

error:

    return dwError;
}

BOOLEAN
VmRESTIsMutexInitialized(
    PVMREST_MUTEX                    pMutex
)
{
    return ( pMutex != NULL ) &&
           ( pMutex->bInitialized != FALSE );
}

DWORD
VmRESTAllocateCondition(
    PVMREST_COND*                    ppCondition
)
{
    DWORD                            dwError = ERROR_SUCCESS;
    PVMREST_COND                     pVmRESTCond = NULL;

    if ( ppCondition == NULL )
    {
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMREST_ERROR(dwError);
    }

    dwError = VmRESTAllocateMemory(
                  sizeof(VMREST_COND),
                  ((PVOID*)&pVmRESTCond)
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTInitializeConditionContent( pVmRESTCond );
    BAIL_ON_VMREST_ERROR(dwError);

    *ppCondition = pVmRESTCond;
    pVmRESTCond = NULL;

error:

    if (pVmRESTCond)
    {
        VmRESTFreeMemory(pVmRESTCond);
        pVmRESTCond = NULL;
    }

    return dwError;
}

DWORD
VmRESTInitializeConditionContent(
    PVMREST_COND                     pCondition
)
{
    DWORD                            dwError = ERROR_SUCCESS;

    if ( ( pCondition == NULL ) || ( pCondition->bInitialized != FALSE ) )
    {
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMREST_ERROR(dwError);
    }

    memset( &(pCondition->cond), 0, sizeof(pthread_cond_t) );

    dwError = pthread_cond_init( &(pCondition->cond), NULL);
    BAIL_ON_VMREST_ERROR(dwError);
    pCondition->bInitialized = TRUE;

error:

    return dwError;
}

VOID
VmRESTFreeCondition(
    PVMREST_COND                     pCondition
)
{
    VmRESTFreeConditionContent( pCondition );
    if (pCondition)
    {
        VmRESTFreeMemory(pCondition);
        pCondition = NULL;
    }
}

VOID
VmRESTFreeConditionContent(
    PVMREST_COND                     pCondition
)
{
    if ( ( pCondition != NULL ) &&  ( pCondition->bInitialized != FALSE ) )
    {
        pthread_cond_destroy(&(pCondition->cond));
        pCondition->bInitialized = FALSE;
    }
}

DWORD
VmRESTConditionWait(
    PVMREST_COND                     pCondition,
    PVMREST_MUTEX                    pMutex
)
{
    DWORD                            dwError = ERROR_SUCCESS;

    if ( ( pCondition == NULL )
         ||
         ( pCondition->bInitialized == FALSE )
         ||
         ( pMutex == NULL )
         ||
         ( pMutex->bInitialized == FALSE )
       )
    {
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMREST_ERROR(dwError);
    }

    dwError = pthread_cond_wait(
                  &(pCondition->cond),
                  &(pMutex->critSect)
                  );
    BAIL_ON_VMREST_ERROR(dwError);

error:

    return dwError;
}

DWORD
VmRESTConditionTimedWait(
    PVMREST_COND                     pCondition,
    PVMREST_MUTEX                    pMutex,
    DWORD                            dwMilliseconds
)
{
    DWORD                            dwError = ERROR_SUCCESS;
    struct timespec                  ts = {0};
    BOOLEAN                          bLocked = FALSE;

    if ( ( pCondition == NULL )
         ||
         ( pCondition->bInitialized == FALSE )
         ||
         ( pMutex == NULL )
         ||
         ( pMutex->bInitialized == FALSE )
       )
    {
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMREST_ERROR(dwError);
    }

    ts.tv_sec = time(NULL) + dwMilliseconds/1000;
    ts.tv_nsec = 0;

    dwError = VmRESTLockMutex(pMutex);
    BAIL_ON_VMREST_ERROR(dwError);

    bLocked = TRUE;

    dwError = pthread_cond_timedwait(
                  &(pCondition->cond),
                  &(pMutex->critSect),
                  &ts
                  );
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:

    if (bLocked)
    {
        VmRESTUnlockMutex(pMutex);
    }

    return dwError;

error:

    goto cleanup;
}

DWORD
VmRESTConditionSignal(
    PVMREST_COND                     pCondition
)
{
    DWORD                            dwError = ERROR_SUCCESS;

    if ( ( pCondition == NULL )
         ||
         ( pCondition->bInitialized == FALSE )
       )
    {
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMREST_ERROR(dwError);
    }

    dwError = pthread_cond_signal( &(pCondition->cond) );
    BAIL_ON_VMREST_ERROR(dwError);

error:

    return dwError;
}

static
PVOID
ThreadFunction(
  PVOID                              pArgs
)
{
    DWORD                            dwError = ERROR_SUCCESS;
    PVMREST_START_ROUTINE            pThreadStart = NULL;
    PVOID                            pThreadArgs = NULL;
    union
    {
        DWORD dwError;
        PVOID pvRet;
    } retVal = { 0 };

    if( pArgs == NULL)
    {
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMREST_ERROR(dwError);
    }

    pThreadStart = ((PVMREST_THREAD_START_INFO)pArgs)->pStartRoutine;
    pThreadArgs = ((PVMREST_THREAD_START_INFO)pArgs)->pArgs;

    if( pThreadStart == NULL )
    {
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMREST_ERROR(dwError);
    }

    dwError = pThreadStart( pThreadArgs );
    BAIL_ON_VMREST_ERROR(dwError);

error:
   if (pArgs)
    {
        VmRESTFreeMemory(pArgs);
        pArgs = NULL;
    }

    retVal.dwError = dwError;
    return retVal.pvRet;
}

DWORD
VmRESTCreateThread(
    PVMREST_THREAD                   pThread,
    BOOLEAN                          bDetached,
    VmRESTStartRoutine*              pStartRoutine,
    PVOID                            pArgs
)
{
    DWORD                            dwError = ERROR_SUCCESS;
    PVMREST_THREAD_START_INFO        pThreadStartInfo = NULL;
    pthread_attr_t                   thrAttr;
    BOOLEAN                          bThreadAttrInited = FALSE;

    if ( ( pThread == NULL ) || ( pStartRoutine == NULL ) )
    {
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMREST_ERROR(dwError);
    }

    if( bDetached != FALSE )
    {
        pthread_attr_init(&thrAttr);
        bThreadAttrInited = TRUE;
        pthread_attr_setdetachstate(
            &thrAttr,
            PTHREAD_CREATE_DETACHED
            );
    }

    dwError = VmRESTAllocateMemory(
                  sizeof(VMREST_THREAD_START_INFO),
                  ((PVOID*)&pThreadStartInfo)
                  );
    BAIL_ON_VMREST_ERROR(dwError);


    pThreadStartInfo->pStartRoutine = pStartRoutine;
    pThreadStartInfo->pArgs = pArgs;

    dwError = pthread_create(
                  pThread,
                  ((bDetached == FALSE) ? NULL : &thrAttr),
                  ThreadFunction,
                  pThreadStartInfo
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    /**** 
    we started successfully -> pThreadStartInfo is now owned by
    ThreadFunction
    ****/
    pThreadStartInfo = NULL;

error:

    if(bThreadAttrInited != FALSE)
    {
        pthread_attr_destroy(&thrAttr);
    }

    if (pThreadStartInfo)
    {
        VmRESTFreeMemory(pThreadStartInfo);
        pThreadStartInfo = NULL;
    }

    return dwError;
}

DWORD
VmRESTThreadJoin(
    PVMREST_THREAD                   pThread,
    PDWORD                           pRetVal
)
{
    DWORD                            dwError = ERROR_SUCCESS;
    union
    {
        DWORD dwError;
        PVOID pvRet;
    } retVal = { 0 };

    if(pThread == NULL)
    {
        dwError = ERROR_INVALID_PARAMETER;
        BAIL_ON_VMREST_ERROR(dwError);
    }

    dwError = pthread_join(
                  (*pThread),
                  ((pRetVal != NULL) ? &(retVal.pvRet) : NULL)
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    if( pRetVal != NULL )
    {
        /**** our ThreadFunction returns error code ****/
        *pRetVal = retVal.dwError;
    }

error:

    return dwError;
}

VOID
VmRESTFreeThread(
    PVMREST_THREAD                   pThread
)
{
    if ( pThread != NULL )
    {
        VmRESTFreeMemory(pThread);

        /**** nothing to free really ****/
#ifndef _WIN32
        //(*pThread) = 0;
#endif
    }
}

DWORD
VmRESTAllocateRWLock(
    PVMREST_RWLOCK*                  ppLock
    )
{
    DWORD                            dwError = ERROR_SUCCESS;
    PVMREST_RWLOCK                   pLock = NULL;

    if (ppLock == NULL)
    {
        dwError = ERROR_INVALID_PARAMETER;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTAllocateMemory(
                  sizeof(VMREST_RWLOCK),
                  (void**)&pLock
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = pthread_rwlock_init(
                  &pLock->rwLock,
                  NULL
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = pthread_key_create(
                  &pLock->readKey,
                  VmRESTFreeLockCount
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = pthread_key_create(
                  &pLock->writeKey,
                  VmRESTFreeLockCount
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    *ppLock = pLock;

cleanup:
    return dwError;
error:
    if (pLock)
    {
        VmRESTFreeMemory(pLock);
        pLock = NULL;
    } 
    goto cleanup;
}

VOID
VmRESTFreeRWLock(
    PVMREST_RWLOCK                   pLock
    )
{
    pthread_key_delete(pLock->readKey);
    pthread_key_delete(pLock->writeKey);
    VmRESTFreeMemory(pLock);
}

void
VmRESTLockRead(
    PVMREST_RWLOCK                   pLock
    )
{
    int* pWriteLockCount = VmRESTGetLockKey(&pLock->writeKey);
    int* pReadLockCount = VmRESTGetLockKey(&pLock->readKey);

    if (!pWriteLockCount || !pReadLockCount)
    {
        pthread_rwlock_rdlock(&pLock->rwLock);
    }
    else
    {
        if (*pWriteLockCount > 0)
        {
            /****
            Simply increment a read count but don't lock for read
            as that would cause undefined behavior.
            ****/
        }
        else
        {
            if (*pReadLockCount == 0)
            {
                pthread_rwlock_rdlock(&pLock->rwLock);
            }
        }
        (*pReadLockCount)++;
    }
}

int
VmRESTTryLockRead(
    PVMREST_RWLOCK                   pLock
    )
{
    int                              result = ERROR_BUSY;
    int*                             pWriteLockCount = VmRESTGetLockKey(&pLock->writeKey);
    int*                             pReadLockCount = VmRESTGetLockKey(&pLock->readKey);

    if (pWriteLockCount && pReadLockCount)
    {
        if (*pWriteLockCount > 0)
        {
            /****
             Simply increment a read count but don't lock for read
             as that would cause undefined behavior.
            ****/
            result = 0;
        }
        else
        {
            if (*pReadLockCount == 0)
            {
                result = pthread_rwlock_tryrdlock(&pLock->rwLock);
            }
            else
            {
                result = 0;
            }
        }

        if (!result)
        {
            (*pReadLockCount)++;
        }
    }

    return result;
}

void
VmRESTUnlockRead(
    PVMREST_RWLOCK                   pLock
    )
{
    int*                             pWriteLockCount = VmRESTGetLockKey(&pLock->writeKey);
    int*                             pReadLockCount = VmRESTGetLockKey(&pLock->readKey);

    if (!pWriteLockCount || !pReadLockCount)
    {
    }
    else
    {
        if (*pWriteLockCount > 0)
        {
        }
        else
        {
            if (*pReadLockCount ==1)
            {
                pthread_rwlock_unlock(&pLock->rwLock);
            }
        }

        if (*pReadLockCount > 0)
        {
            (*pReadLockCount)--;
        }
        else
        {
        }
    }
}

void
VmRESTLockWrite(
    PVMREST_RWLOCK                   pLock
    )
{
    int*                             pWriteLockCount = VmRESTGetLockKey(&pLock->writeKey);
    if (!pWriteLockCount)
    {
        pthread_rwlock_wrlock(&pLock->rwLock);
    }
    else
    {
        if (*pWriteLockCount == 0)
        {
            pthread_rwlock_wrlock(&pLock->rwLock);
        }
        (*pWriteLockCount)++;
    }
}

int
VmRESTTryLockWrite(
    PVMREST_RWLOCK                   pLock
    )
{
    int                              result = ERROR_BUSY;
    int*                             pWriteLockCount = VmRESTGetLockKey(&pLock->writeKey);
    int*                             pReadLockCount = VmRESTGetLockKey(&pLock->readKey);
    if (pWriteLockCount && pReadLockCount)
    {
        if (*pReadLockCount > 0)
        {
            result = ERROR_POSSIBLE_DEADLOCK;
        }
        else
        {
            if (*pWriteLockCount <= 0)
            {
                result = pthread_rwlock_trywrlock(&pLock->rwLock);
            }
            else
            {
                result = 0;
            }

            if (!result)
            {
                (*pWriteLockCount)++;
            }
        }
    }

    return result;
}

void
VmRESTUnlockWrite(
    PVMREST_RWLOCK                   pLock
    )
{
    int*                             pWriteLockCount = VmRESTGetLockKey(&pLock->writeKey);
    if (!pWriteLockCount)
    {
    }
    else
    {
        if (*pWriteLockCount ==1)
        {
            pthread_rwlock_unlock(&pLock->rwLock);
        }

        if (*pWriteLockCount > 0)
        {
            (*pWriteLockCount)--;
        }
    }
}

static
void
VmRESTFreeLockCount(
    void*                            pkeyData
    )
{
    VmRESTFreeMemory(pkeyData);
}

static
int*
VmRESTGetLockKey(
    pthread_key_t*                   pLockKey
)
{
    DWORD                            dwError = ERROR_SUCCESS;
    int*                             pCounter = NULL;

    int*                             pCount = (int*)pthread_getspecific(*pLockKey);
    if (!pCount)
    {
        dwError = VmRESTAllocateMemory(sizeof(int), (void**)&(pCounter));
        BAIL_ON_VMREST_ERROR(dwError);

        dwError = pthread_setspecific(*pLockKey, pCounter);
        BAIL_ON_VMREST_ERROR(dwError);
        pCounter = NULL;

        pCount = (int*)pthread_getspecific(*pLockKey);
    }

cleanup:
    return pCount;

error:
    if (pCounter)
    {
        VmRESTFreeMemory(pCounter);
        pCounter = NULL;
    }
    goto cleanup;
}

