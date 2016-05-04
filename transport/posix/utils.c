/*
 * Copyright © 2012-2015 VMware, Inc.  All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the ~@~\License~@~]); you may not
 * use this file except in compliance with the License.  You may obtain a copy
 * of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an ~@~\AS IS~@~] BASIS, without
 * warranties or conditions of any kind, EITHER EXPRESS OR IMPLIED.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

#include "includes.h"

uint32_t
VmRESTInsertElement(
    PVM_EVENT_DATA                   data,
    uint32_t                         flag,
    QUEUE*                           queue
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    EVENT_NODE*                      node = NULL;

    if (queue == NULL)
    {
        VMREST_LOG_DEBUG("VmRESTInsertElement(): Invalid params");
        dwError = VMREST_TRANSPORT_INVALID_PARAM;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTAllocateMemory(
                  sizeof(EVENT_NODE),
                  (void*)&node
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    memcpy(&(node->data), data, sizeof(VM_EVENT_DATA));
    node->flag  = flag;
    node->next  = NULL;

    if (queue->count == 0)
    {
        queue->head = node;
        queue->tail = node;

    } else
    {
        queue->tail->next = node;
        queue->tail = node;
    }
    queue->count++;

cleanup:
    return dwError;
error:
    goto cleanup;
}


EVENT_NODE*
VmRESTUtilsRemoveElement(
    QUEUE*                           queue
    )
{
    EVENT_NODE*                      temp = NULL;
    uint32_t                         dwError = REST_ENGINE_SUCCESS;

    if (queue == NULL)
    {
        VMREST_LOG_DEBUG("VmRESTUtilsRemoveElement(): Invalid params");
        dwError = VMREST_TRANSPORT_INVALID_PARAM;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    if (queue->count == 0)
    {
        dwError = VMREST_TRANSPORT_QUEUE_EMPTY;
    }
    else if (queue->count == 1)
    {
        temp = queue->head;
        queue->head = NULL;
        queue->tail = NULL;

    } else
    {
        temp = queue->head;
        queue->head = queue->head->next;
    }
    BAIL_ON_VMREST_ERROR(dwError);
    queue->count--;

cleanup:
    return temp;
error:
    goto cleanup;
}

uint32_t
VmRestUtilsInitQueue(
    QUEUE*                           queue
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    uint32_t                         mutexInited = 0;

    if (queue == NULL)
    {
        VMREST_LOG_DEBUG("VmRestUtilsInitQueue(): Invalid params");
        dwError = VMREST_TRANSPORT_INVALID_PARAM;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    queue->count = 0;
    queue->head = NULL;
    queue->tail = NULL;

    dwError = pthread_mutex_init(
                  &(queue->lock),
                  NULL
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    mutexInited = 1;

    dwError = pthread_cond_init(
                  &(queue->signal),
                  NULL
                  );
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:
    return dwError;
error:
    if (mutexInited)
    {
        pthread_mutex_destroy(
            &(queue->lock)
            );
    }
    dwError = VMREST_TRANSPORT_QUEUE_INIT_FAILED;
    goto cleanup;
}

void
VmRESTUtilsRemoveAllNodes(
    QUEUE*                           queue
    )
{
    EVENT_NODE*                      temp = NULL;
    EVENT_NODE*                      old = NULL;

    if (queue == NULL)
    {
        VMREST_LOG_DEBUG("VmRESTUtilsDestroyQueue(): Invalid params");
        return;
    }

    pthread_mutex_lock(&(queue->lock));
    temp = queue->head;

    while(temp != NULL)
    {
        old = temp;
        temp = temp->next;

        VmRESTFreeMemory(
            old
            );

    }
    queue->count = 0;
    queue->head = NULL;
    queue->tail = NULL;

    pthread_mutex_unlock(&(queue->lock));
}


void
VmRESTUtilsDestroyQueue(
    QUEUE*                           queue
    )
{
    pthread_mutex_destroy(&(queue->lock));
    pthread_cond_destroy(&(queue->signal));
}

char
VmRESTUtilsGetLastChar(
    char*                            src
    )
{
    char                             ret = '\0';
    char*                            temp = NULL;

    if (src == NULL || (strlen(src) > MAX_SERVER_PORT_LEN))
    {
        VMREST_LOG_DEBUG("VmRESTUtilsGetLastChar(): Invalid Params");
        return ret;
    }
    temp = src;
    while (*temp != '\0')
    {
        ret = *temp;
        temp++;
    }
    return ret;
}
