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
    PVM_EVENT_DATA       data,
    uint32_t             flag,
    QUEUE*               queue
    )
{
    uint32_t    dwError = EXIT_SUCCESS;
    EVENT_NODE* node = NULL;
    
    dwError = VmRESTAllocateMemory(
              sizeof(EVENT_NODE),
              (void*)&node
              );
    BAIL_ON_POSIX_SOCK_ERROR(dwError);       
  
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
    QUEUE*        queue
    ) 
{
    EVENT_NODE*   temp = NULL;
    uint32_t      dwError = EXIT_SUCCESS; 
    
    if (queue->count == 0)
    {
        dwError = ERROR_NOT_SUPPORTED; 
        BAIL_ON_POSIX_SOCK_ERROR(dwError);
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
    queue->count--;

cleanup:
    return temp;
error:
    goto cleanup;
}

uint32_t 
VmRestUtilsInitQueue(
    QUEUE*       queue
    )
{
    uint32_t     dwError = EXIT_SUCCESS;
    uint32_t     mutexInited = 0;  
    
    queue->count = 0;
    queue->head = NULL;
    queue->tail = NULL;
    
    dwError = pthread_mutex_init(
              &(queue->lock),
              NULL
              );
    BAIL_ON_POSIX_SOCK_ERROR(dwError);

    mutexInited = 1;

    dwError = pthread_cond_init(
              &(queue->signal),
              NULL
              );
    BAIL_ON_POSIX_SOCK_ERROR(dwError);

cleanup:
    return dwError;
error:
    if (mutexInited)
    {
        pthread_mutex_destroy(
        &(queue->lock)
        );
    }
    goto cleanup;
}

void 
VmRESTUtilsDestroyQueue(
    QUEUE*         queue
    )
{
    EVENT_NODE*    temp = NULL;
    EVENT_NODE*    old = NULL;

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
    
    pthread_mutex_destroy(&(queue->lock));
    pthread_cond_destroy(&(queue->signal)); 

}
