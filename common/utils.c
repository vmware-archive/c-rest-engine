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

#include "includes.h"

uint32_t
VmRESTUtilsConvertInttoString(
    int                              num,
    char*                            str
    )
{
    uint32_t                         dwError = 0;
    if(str == NULL)
    {
        dwError = 1;
    }
    BAIL_ON_VMREST_ERROR(dwError);
    sprintf(str, "%d", num);

cleanup:
    return dwError;
error:
    goto cleanup;

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
