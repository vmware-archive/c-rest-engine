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

#include "includes.h"

VMREST_GLOBALS gVMRESTGlobals =
{ 
    .mutex = PTHREAD_MUTEX_INITIALIZER
};

VMREST_ENGINE_METHODS gVmRestHandlers =
{
    .pfnHandleHTTP_GET      = &VmRESTHandleHTTP_GET,
    .pfnHandleHTTP_POST     = &VmRESTHandleHTTP_POST,
    .pfnHandleHTTP_HEAD     = &VmRESTHandleHTTP_HEAD, 
    .pfnHandleHTTP_PUT      = &VmRESTHandleHTTP_PUT,
    .pfnHandleHTTP_DELETE   = &VmRESTHandleHTTP_DELETE, 
    .pfnHandleHTTP_TRACE    = &VmRESTHandleHTTP_TRACE,
    .pfnHandleHTTP_CONNECT  = &VmRESTHandleHTTP_CONNECT
};

PVMREST_ENGINE_METHODS gpVmRestHandlers = &gVmRestHandlers;
