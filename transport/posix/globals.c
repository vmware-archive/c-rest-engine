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

VM_SOCKET gServerSocketInfo = {0};

QUEUE *pQueue = NULL;


uint32_t
VmInitGlobalServerSocket(
    char*                            port
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    uint32_t                         len = 0;
    uint32_t                         index = 0;

    if (port == NULL)
    {
        VMREST_LOG_DEBUG("VmInitGlobalServerSocket(): Invalid params");
        dwError =  ERROR_TRANSPORT_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    len = strlen(port);
    if (len > MAX_PORT_LEN)
    {
        VMREST_LOG_DEBUG("VmInitGlobalServerSocket(): Invalid port length");
        dwError = ERROR_TRANSPORT_VALIDATION_FAILED;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    for (index = 0; index < MAX_CONNECTIONS; index++)
    {
        gServerSocketInfo.clients[index].fd = -1;
        gServerSocketInfo.clients[index].notStale = 0;
        gServerSocketInfo.clients[index].ssl = NULL;
        gServerSocketInfo.clients[index].dataRead = 0;
        gServerSocketInfo.clients[index].dataProcessed = 0;
        gServerSocketInfo.clients[gServerSocketInfo.emptyIndex].self = NULL;
        memset(gServerSocketInfo.clients[index].streamDataBuffer,
               '\0',MAX_DATA_BUFFER_LEN);
    }

    gServerSocketInfo.clientCount = 0;
    gServerSocketInfo.emptyIndex  = 0;

    memset(gServerSocketInfo.clients, 0 , (sizeof(VM_CONNECTION) * MAX_CONNECTIONS));

    strcpy(gServerSocketInfo.port, port);

    if (pthread_mutex_init(&(gServerSocketInfo.lock), NULL) != 0)
    {
        VMREST_LOG_DEBUG("VmInitGlobalServerSocket(): Mutex init failed");
        dwError = VMREST_TRANSPORT_MUTEX_INIT_FAILED;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    gServerSocketInfo.keepOpen = 1;
    gServerSocketInfo.ServerAlive = 1;

cleanup:
    return dwError;
error:
    goto cleanup;
}

void
VmRemoveAllClientsFromGlobal(
    void
    )
{
    uint32_t                         index = 0;

    pthread_mutex_lock(&(gServerSocketInfo.lock));
    for (index = 0; index < MAX_CONNECTIONS; index++)
    {
        if (gServerSocketInfo.clients[index].notStale == 1)
        {
            if (gServerSocketInfo.isSecure && gServerSocketInfo.clients[index].ssl != NULL)
            {
                SSL_shutdown(gServerSocketInfo.clients[index].ssl);
                SSL_free(gServerSocketInfo.clients[index].ssl);
            }
            else if (gServerSocketInfo.clients[index].fd > 0)
            {
                close(gServerSocketInfo.clients[index].fd);
            }
        }
        gServerSocketInfo.clients[index].fd = -1;
        gServerSocketInfo.clients[index].notStale = 0;
        gServerSocketInfo.clients[index].ssl = NULL;
        gServerSocketInfo.clients[index].dataRead = 0;
        gServerSocketInfo.clients[index].dataProcessed = 0;
        gServerSocketInfo.clientCount--;
    }
    pthread_mutex_unlock(&(gServerSocketInfo.lock));
}

void
VmShutdownGlobalServerSocket(
    void
    )
{
    VmRemoveAllClientsFromGlobal();
    gServerSocketInfo.clientCount = 0;
    memset(gServerSocketInfo.port, '\0', MAX_PORT_LEN);
    gServerSocketInfo.fd = -1;
    pthread_mutex_destroy(&(gServerSocketInfo.lock));

}

uint32_t
VmRESTInsertClientFromGlobal(
    PVM_EVENT_DATA                   data,
    uint32_t*                        index
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    uint32_t                         count = 0;
    uint32_t                         temp = 0;
    uint32_t                         success = 0;

    if (data == NULL)
    {
        VMREST_LOG_DEBUG("VmRESTInsertClientFromGlobal(): Invalid data");
        dwError =  ERROR_TRANSPORT_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    if ((data->ssl == NULL && data->fd < 0) || (index == NULL))
    {
        VMREST_LOG_DEBUG("VmRESTInsertClientFromGlobal(): Invalid params");
        dwError =  ERROR_TRANSPORT_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    pthread_mutex_lock(&(gServerSocketInfo.lock));

    gServerSocketInfo.clientCount++;
    gServerSocketInfo.clients[gServerSocketInfo.emptyIndex].fd = data->fd;
    gServerSocketInfo.clients[gServerSocketInfo.emptyIndex].notStale = 1;
    gServerSocketInfo.clients[gServerSocketInfo.emptyIndex].ssl = data->ssl;
    gServerSocketInfo.clients[gServerSocketInfo.emptyIndex].self = data;
    temp = gServerSocketInfo.emptyIndex;

    while (count < MAX_CONNECTIONS)
    {
        temp++;
        if (temp >= MAX_CONNECTIONS)
        {
            temp = 0;
        }
        if (gServerSocketInfo.clients[temp].notStale == 0)
        {
            success = 1;
            break;
        }
        count++;
    }
    if (success)
    {
        *index = gServerSocketInfo.emptyIndex;
        gServerSocketInfo.emptyIndex = temp;
    }
    else
    {
        dwError = VMREST_TRANSPORT_INSERT_CLIENT_INGLOBAL_ERROR;
    }
    pthread_mutex_unlock(&(gServerSocketInfo.lock));
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:
    return dwError;

error:
    goto cleanup;
}

uint32_t
VmRESTRemoveClientFromGlobal(
    uint32_t                         index
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;

    if (index >= MAX_CONNECTIONS)
    {
        VMREST_LOG_DEBUG("VmRESTRemoveClientFromGlobal(): Invalid params");
        dwError =  ERROR_TRANSPORT_INVALID_PARAMS;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    pthread_mutex_lock(&(gServerSocketInfo.lock));

    gServerSocketInfo.clients[index].fd = -1;
    gServerSocketInfo.clients[index].notStale = 0;
    gServerSocketInfo.clients[index].ssl = NULL;
    gServerSocketInfo.clients[index].self = NULL;
    gServerSocketInfo.clientCount--;

    pthread_mutex_unlock(&(gServerSocketInfo.lock));

cleanup:
    return dwError;
error:
    goto cleanup;
}

