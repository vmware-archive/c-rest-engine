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


#include <pthread.h>


typedef struct _VMREST_GLOBALS
{
    pthread_mutex_t mutex;
} VMREST_GLOBALS, *PVMREST_GLOBALS;

typedef struct _VMRESTD_CONFIG
{
    char                             sslCert[MAX_CONFIG_PARAMS_LEN];
    char                             sslKey[MAX_CONFIG_PARAMS_LEN];
    char                             port[MAX_CONFIG_PARAMS_LEN];
    char                             debugLogFile[MAX_CONFIG_PARAMS_LEN];
    char                             clientCnt[MAX_CONFIG_PARAMS_LEN];
    char                             workerThCnt[MAX_CONFIG_PARAMS_LEN];
    char                             configFile[MAX_CONFIG_PARAMS_LEN];
} VMRESTD_CONFIG, *PVMRESTD_CONFIG;
