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


#include <config.h>
#include <vmrestsys.h>

//#ifdef HAVE_SYS_EPOLL_H
#include <sys/epoll.h>
//#endif

#include <vmrestdefines.h>
#include <vmsock.h>
#include <vmrestcommon.h>
#include <vmsockapi.h>
#include <vmsockposix.h>

#include "defines.h"
#include "structs.h"
#include "prototypes.h"
#include "externs.h"

#include <openssl/ssl.h>
#include <openssl/err.h>

//#include <vmrestsys.h>
//#include <vmrestsock.h>

#include <vmrestcommon.h>
#include <vmrest.h>
#include <fcntl.h>
#include <sys/epoll.h>

