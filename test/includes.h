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
#include "testUtils.h"

extern PREST_PROCESSOR gpVmRestHandlers;
