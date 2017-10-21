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

#define VM_SOCK_POSIX_DEFAULT_LISTEN_QUEUE_SIZE 8092

#define VM_SOCK_POSIX_DEFAULT_QUEUE_SIZE        (256)
#define VM_SOCK_POSIX_DEFAULT_WORKER_THR_COUNT   5

#ifndef PopEntryList
#define PopEntryList(ListHead) \
    (ListHead)->Next;\
        {\
        PSINGLE_LIST_ENTRY FirstEntry;\
        FirstEntry = (ListHead)->Next;\
        if (FirstEntry != NULL) {     \
            (ListHead)->Next = FirstEntry->Next;\
                }                             \
        }
#endif

#ifndef PushEntryList
#define PushEntryList(ListHead,Entry) \
    (Entry)->Next = (ListHead)->Next; \
    (ListHead)->Next = (Entry)
#endif

#ifndef CONTAINING_RECORD
#define CONTAINING_RECORD(address, type, field) ((type *)( \
                                                  (PCHAR)(address) - \
                                                  (uint64_t)(uintptr_t)(&((type *)0)->field)))

#endif

typedef enum
{
    VM_SOCK_POSIX_EVENT_STATE_UNKNOWN = 0,
    VM_SOCK_POSIX_EVENT_STATE_WAIT,
    VM_SOCK_POSIX_EVENT_STATE_PROCESS
} VM_SOCK_POSIX_EVENT_STATE;

/**** Transport internal error codes ****/
#define VM_SOCK_POSIX_ERROR_SYS_CALL_FAILED    5100
#define MAX_RETRY_ATTEMPTS                     50000


