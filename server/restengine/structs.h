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

#include "defines.h"

typedef struct _REST_ENG_THREAD_DATA
{
    pthread_mutex_t  mutex;
    pthread_mutex_t* pMutex;

    pthread_cond_t   cond;
    pthread_cond_t*  pCond;

    int              bShutdown;

} VMREST_THREAD_DATA, *PVMREST_THREAD_DATA;

typedef struct _REST_ENG_THREAD
{
    pthread_t  thr;
    pthread_t* pThread;

    PVMREST_THREAD_DATA data;

} VMREST_THREAD, *PVMREST_THREAD;

typedef struct _REST_ENG_GLOBALS
{
    PVMREST_THREAD   pThreadpool;
    uint32_t         nThreads;
} REST_ENG_GLOBALS, *PREST_ENG_GLOBALS;

   


