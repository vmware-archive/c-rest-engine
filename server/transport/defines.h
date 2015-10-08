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


/* provide port and ip information here for the time being */
#define PORT 61001
#define SERVERIP 127.0.0.1
#define MAX_EVENT 64


#define MAX_ADDRESS_LEN 128
#define MAX_PORT_LEN 6

#define ERROR_NOT_SUPPORTED 100

#define BAIL_ON_POSIX_SOCK_ERROR(dwError) \
        if (dwError) \
            goto error;
