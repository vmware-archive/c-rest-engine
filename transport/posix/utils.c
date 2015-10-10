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

uint32_t insert_element(int fd, uint32_t event_flag, QUEUE *q)
{
    EVENT_NODE *node = (EVENT_NODE*)malloc (sizeof(EVENT_NODE)); 
    if (node == NULL) 
    {
        return 1; 

    }
    node->fd = fd;
    node->flag = event_flag;
    node->next  = NULL;
    if (q->count == 0)
    {
        q->head = node;
        q->tail = node;
 
    } else 
    {
        q->tail->next = node;
        q->tail = node;
         

    }
    q->count++;
    return 0;
}


EVENT_NODE* remove_element(QUEUE *q) 
{
    EVENT_NODE *temp; 
    if (q->count == 0)
    {
        return NULL;
    }
    else if (q->count == 1)
    {
        temp = q->head;
        q->head = NULL;
        q->tail = NULL;

    } else 
    {
        temp = q->head;
        q->head = q->head->next;
    }
    q->count--;
    return temp;
}

uint32_t init_queue(QUEUE *q)
{
    q->count = 0;
    q->head = NULL;
    q->tail = NULL;
    pthread_mutex_init(&(q->lock),NULL);
    pthread_cond_init(&(q->signal),NULL);
    return 0;
}

