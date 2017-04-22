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

#ifndef WIN32
#include <config.h>
#include <stdint.h>
#else
typedef __int32 int32_t;
typedef __int64 int64_t;
typedef unsigned __int32 uint32_t;
typedef unsigned __int64 uint64_t;
typedef unsigned __int16 uint16_t;
typedef unsigned __int8 uint8_t;
#endif
#include <stdio.h>

#include <vmrestsys.h>
#include <vmrestdefines.h>
#include <vmrest.h>
#include <vmsock.h>
#include <vmrestcommon.h>
#include "defines.h"
#include "structs.h"
#include "prototype.h"
