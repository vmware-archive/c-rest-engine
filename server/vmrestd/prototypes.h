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

/* service.c */

uint32_t
VmRESTSrvInitialize(
    char *configFile
    );

uint32_t
VmRESTSrvShutdown(
    void
    );

/* signal.c */

void
VmRESTSrvBlockSignals(
    void
    );

uint32_t
VmRESTSrvHandleSignals(
    void
    );

/* global.c */
void
VmAppStoreEndpoint(
    uint32_t                         index,
    PFN_PROCESS_REST_CRUD            pfnCreate,
    PFN_PROCESS_REST_CRUD            pfnRead,
    PFN_PROCESS_REST_CRUD            pfnUpdate,
    PFN_PROCESS_REST_CRUD            pfnDelete
    );

/* httpHandlers.c */

uint32_t
VmRESTHandleHTTP_REQUEST(
    PREST_REQUEST                    pRequest,
    PREST_RESPONSE*                  ppResponse
    );

/*
uint32_t
VmRESTHandleCreate(
    PREST_REQUEST                    pRequest,
    PREST_RESPONSE*                  ppResponse,
    uint32_t                         paramsCount
    );

uint32_t
VmRESTHandleRead(
    PREST_REQUEST                    pRequest,
    PREST_RESPONSE*                  ppResponse,
    uint32_t                         paramsCount
    );

uint32_t
VmRESTHandleDelete(
    PREST_REQUEST                    pRequest,
    PREST_RESPONSE*                  ppResponse,
    uint32_t                         paramsCount
    );

uint32_t
VmRESTHandleUpdate(
    PREST_REQUEST                    pRequest,
    PREST_RESPONSE*                  ppResponse,
    uint32_t                         paramsCount
    );
*/

/**** tdnf.c ****/

/***** TDNF Version ****/
uint32_t
VmHandleTDNFVersionGet(
    PREST_REQUEST                    pRequest,
    PREST_RESPONSE*                  ppResponse,
    uint32_t                         paramsCount
    );

uint32_t
VmHandleTDNFVersionSet(
    PREST_REQUEST                    pRequest,
    PREST_RESPONSE*                  ppResponse,
    uint32_t                         paramsCount
    );


/***** Package related operation ****/
uint32_t
VmHandlePackageRead(
    PREST_REQUEST                    pRequest,
    PREST_RESPONSE*                  ppResponse,
    uint32_t                         paramsCount
    );

uint32_t
VmHandlePackageWrite(
    PREST_REQUEST                    pRequest,
    PREST_RESPONSE*                  ppResponse,
    uint32_t                         paramsCount
    );

uint32_t
VmHandlePackageUpdate(
    PREST_REQUEST                    pRequest,
    PREST_RESPONSE*                  ppResponse,
    uint32_t                         paramsCount
    );

uint32_t
VmHandlePackageDelete(
    PREST_REQUEST                    pRequest,
    PREST_RESPONSE*                  ppResponse,
    uint32_t                         paramsCount
    );

