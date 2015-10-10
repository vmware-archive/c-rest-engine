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

typedef struct _REST_ENG_THREAD
{
    pthread_t                   thr;
} REST_ENG_THREAD, *PREST_ENG_THREAD;

typedef struct _REST_ENG_GLOBALS
{
    pthread_t        threadpool[5];
    uint32_t         nthreads;
} REST_ENG_GLOBALS, *PREST_ENG_GLOBALS;

   


