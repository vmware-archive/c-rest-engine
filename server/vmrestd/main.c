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

//#define USE_APP_CTX 1

#ifdef WIN32
#include "..\transport\win\includes.h"
char* configPath = "c:\\tmp\\restconfig.txt";
char* configPath1 = "c:\\tmp\\restconfig1.txt";
#else
#include <getopt.h>
#include <sys/types.h>
#include <sys/inotify.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <stdio.h>
#include <signal.h>
char* configPath = "/tmp/restconfig.txt";
char* configPath1 = "/tmp/restconfig1.txt";
#endif


PVMREST_HANDLE  gpRESTHandle = NULL;
PVMREST_HANDLE  gpRESTHandle1 = NULL;

REST_PROCESSOR gVmRestHandlers;
REST_PROCESSOR gVmRestHandlers1;

uint32_t
VmRESTUtilsConvertInttoString(
    int                              num,
    char*                            str
    );

#ifndef WIN32
void sig_handler(int signo)
{
  if (signo == SIGPIPE)
  {
      /* Do nothing */
  }
}
#endif

int main()
{
    uint32_t                         dwError = 0;
#ifdef USE_APP_CTX
    PREST_CONF                       pConfig = NULL;
    PREST_CONF                       pConfig1 = NULL;
    SSL_CTX*                         sslCtx = NULL;
    SSL_CTX*                         sslCtx1 = NULL;
#endif
    //uint32_t                         cnt = 0;

#ifndef WIN32
    signal(SIGPIPE, sig_handler);
#endif

    gVmRestHandlers.pfnHandleRequest = NULL;
    gVmRestHandlers.pfnHandleCreate = &VmHandleEchoData;
    gVmRestHandlers.pfnHandleRead = &VmHandleEchoData;
    gVmRestHandlers.pfnHandleUpdate = &VmHandleEchoData;
    gVmRestHandlers.pfnHandleDelete = &VmHandleEchoData;
    gVmRestHandlers.pfnHandleOthers = &VmHandleEchoData;

    gVmRestHandlers1.pfnHandleRequest = NULL;
    gVmRestHandlers1.pfnHandleCreate = &VmHandleEchoData1;
    gVmRestHandlers1.pfnHandleRead = &VmHandleEchoData1;
    gVmRestHandlers1.pfnHandleUpdate = &VmHandleEchoData1;
    gVmRestHandlers1.pfnHandleDelete = &VmHandleEchoData1;
    gVmRestHandlers1.pfnHandleOthers = &VmHandleEchoData1;



#ifdef USE_APP_CTX
    
    dwError = VmTESTInitSSL("/root/mycert.pem", "/root/mycert.pem", &sslCtx);

    dwError = VmTESTInitSSL("/root/mycert.pem", "/root/mycert.pem", &sslCtx1);

    pConfig = (PREST_CONF)malloc(sizeof(REST_CONF));
    pConfig->pSSLCertificate = NULL;
    pConfig->pSSLKey = NULL;
    pConfig->pServerPort = "81";
    pConfig->pDebugLogFile = "/tmp/restServer.log";
    pConfig->pClientCount = "5";
    pConfig->pMaxWorkerThread = "5";
    pConfig->pSSLContext = sslCtx;

    pConfig1 = (PREST_CONF)malloc(sizeof(REST_CONF));
    pConfig1->pSSLCertificate = NULL;
    pConfig1->pSSLKey = NULL;
    pConfig1->pServerPort = "82";
    pConfig1->pDebugLogFile = "/tmp/restServer1.log";
    pConfig1->pClientCount = "5";
    pConfig1->pMaxWorkerThread = "5";
    pConfig1->pSSLContext = sslCtx1;


    dwError = VmRESTInit(pConfig, NULL, &gpRESTHandle);
    dwError = VmRESTInit(pConfig1, NULL, &gpRESTHandle1);

#else
    dwError = VmRESTInit(NULL,configPath, &gpRESTHandle);
    dwError = VmRESTInit(NULL,configPath1, &gpRESTHandle1);

#endif

// test set SSL info API
#if 0 
   char buffer[8092]= {0};
   FILE *fp = fopen("/root/mycert.pem","r");

   dwError = fread(buffer, 1, 8092, fp);
   VmRESTSetSSLInfo(gpRESTHandle, buffer, dwError, SSL_DATA_TYPE_KEY);
   VmRESTSetSSLInfo(gpRESTHandle, buffer, dwError, SSL_DATA_TYPE_CERT);


#endif

    VmRESTRegisterHandler(gpRESTHandle, "/v1/pkg", &gVmRestHandlers, NULL);
    VmRESTRegisterHandler(gpRESTHandle1, "/v1/blah", &gVmRestHandlers1, NULL);

    VmRESTStart(gpRESTHandle);
    VmRESTStart(gpRESTHandle1);


    while(1)  //cnt < 10)
    {
#ifdef WIN32
        Sleep(1000);
#else
		sleep(1);
#endif
       // cnt++;
    }

    dwError = VmRESTStop(gpRESTHandle);
    dwError = VmRESTStop(gpRESTHandle1);

    dwError = VmRESTUnRegisterHandler(gpRESTHandle,"/v1/pkg");
    dwError = VmRESTUnRegisterHandler(gpRESTHandle1,"/v1/blah");

    VmRESTShutdown(gpRESTHandle);
    VmRESTShutdown(gpRESTHandle1);

#ifdef USE_APP_CTX
    VmRESTShutdownSSL(sslCtx);
    VmRESTShutdownSSL(sslCtx1);
#endif

return dwError;

}

uint32_t
VmHandleEchoData1(
    PVMREST_HANDLE                   pRESTHandle,
    PREST_REQUEST                    pRequest,
    PREST_RESPONSE*                  ppResponse,
    uint32_t                         paramsCount
    )
{
    uint32_t                         dwError = REST_ENGINE_MORE_IO_REQUIRED;
    char*                            AllData = NULL;
    char                             buffer[4097] = {0};
    int                              nRead = 0;
    int                              nWrite = 0;
    uint32_t                         bytesRW = 0;
    uint32_t                         bytesLeft = 0;
    char                             size[10] = {0};
    int                              resLength = 0;
    uint32_t                         index = 0;
    
    memset(size, '\0', 10);

    AllData = malloc(MAX_IN_MEM_PAYLOAD_LEN);
    memset(AllData, '\0', MAX_IN_MEM_PAYLOAD_LEN);

    bytesRW = 0;
    
    while(dwError == REST_ENGINE_MORE_IO_REQUIRED)
    {
        dwError = VmRESTGetData(
                      pRESTHandle,
                      pRequest,
                      buffer,
                      &bytesRW
                      );

        nRead = bytesRW; //strlen(buffer);

        if (nRead > 0)
        {
            if ((index + nRead)< MAX_IN_MEM_PAYLOAD_LEN)
            {
                memcpy((AllData + index),buffer,nRead);
                index += nRead;
            }
            else
            {
                dwError = 500;
                BAIL_ON_VMREST_ERROR(dwError);
            }

        }
        memset(buffer, '\0', 4097);
        nRead = 0;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    dwError = VmRESTSetSuccessResponse(
                  pRequest,
                  ppResponse
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    memset(buffer, '\0', 4097);

    resLength = strlen(AllData);

    if (resLength < 4096 )
    {
        dwError = VmRESTUtilsConvertInttoString(
                          resLength,
                          size);
        BAIL_ON_VMREST_ERROR(dwError);

        dwError = VmRESTSetDataLength(
                  ppResponse,
                  size
                  );
        BAIL_ON_VMREST_ERROR(dwError);
    }
    else
    {
        dwError = VmRESTSetDataLength(
                  ppResponse,
                  NULL
                  );
        BAIL_ON_VMREST_ERROR(dwError);
    }

    index = 0;
    bytesLeft = resLength;

    if (bytesLeft >= MAX_IN_MEM_PAYLOAD_LEN)
    {
        dwError = 400;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    memset(buffer, '\0', 4097);

    dwError = REST_ENGINE_MORE_IO_REQUIRED;
    while(dwError == REST_ENGINE_MORE_IO_REQUIRED)
    {
        nWrite = (bytesLeft > 4096) ? 4096: bytesLeft;
        memcpy(buffer, (AllData + index), nWrite);

        dwError = VmRESTSetData(
                  pRESTHandle,
                  ppResponse,
                  buffer,
                  nWrite,
                  &bytesRW
                  );
        index = index + bytesRW;
        bytesLeft = bytesLeft - bytesRW;
        memset(buffer, '\0', 4097);

    }
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:
    if (AllData != NULL)
    {
        free(AllData);
        AllData = NULL;
    }

    return dwError;
error:
    goto cleanup;
}




uint32_t
VmHandleEchoData(
    PVMREST_HANDLE                   pRESTHandle,
    PREST_REQUEST                    pRequest,
    PREST_RESPONSE*                  ppResponse,
    uint32_t                         paramsCount
    )
{
    uint32_t                         dwError = 0;
    char*                            AllData = NULL;
    char                             buffer[4097] = {0};
    int                              nRead = 0;
    int                              nWrite = 0;
    uint32_t                         bytesLeft = 0;
    char                             size[10] = {0};    
    int                              resLength = 0;
    uint32_t                         index = 0;
    uint32_t                         bytesRW = 0;
    FILE*                            fp = NULL;
    //char*                            ip = NULL;
    //int                              port = 0;

    memset(buffer, '\0', 4097);
    memset(size, '\0', 10);

    AllData = malloc(MAX_IN_MEM_PAYLOAD_LEN);
    memset(AllData, '\0', MAX_IN_MEM_PAYLOAD_LEN);

    dwError = REST_ENGINE_MORE_IO_REQUIRED;

    fp = fopen("/tmp/image.vmdk","wb+");

    while(dwError == REST_ENGINE_MORE_IO_REQUIRED)
    {
        dwError = VmRESTGetData(
                      pRESTHandle,
                      pRequest,
                      buffer,
                      &bytesRW
                      );

        nRead = bytesRW;

        fwrite(buffer,1,nRead,fp);        
#if 1
        if (nRead > 0)
        {
            if ((index + nRead)< MAX_IN_MEM_PAYLOAD_LEN)
            {
                memcpy((AllData + index),buffer,nRead);
                index += nRead;
            }
            else
            {
                dwError = 500;
                 BAIL_ON_VMREST_ERROR(dwError);
            }

        }
#else
        index++;
#endif

        memset(buffer, '\0', 4097);
        nRead = 0;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    fclose(fp);
    fp = NULL;

/*    dwError = VmRESTGetConnectionInfo(
                  pRequest,
                  &ip,
                  &port
                  );
   BAIL_ON_VMREST_ERROR(dwError);

   printf("\n IP Address:==%s==Port No %d", ip, port);
   //write(1, ip, 46);
   //write(1, &port, 4);

  */                

    dwError = VmRESTSetSuccessResponse(
                  pRequest,
                  ppResponse
                  );
    BAIL_ON_VMREST_ERROR(dwError);

    memset(buffer, '\0', 4097);

    resLength = strlen(AllData);
   
    if (resLength < 4096 )
    {
        dwError = VmRESTUtilsConvertInttoString(
                          resLength,
                          size);
        BAIL_ON_VMREST_ERROR(dwError);

        dwError = VmRESTSetDataLength(
                  ppResponse,
                  size
                  );
        BAIL_ON_VMREST_ERROR(dwError);
    }
    else
    {
        dwError = VmRESTSetDataLength(
                  ppResponse,
                  NULL
                  );
        BAIL_ON_VMREST_ERROR(dwError);
    }

    index = 0;
    bytesLeft = resLength;

    if (bytesLeft >= MAX_IN_MEM_PAYLOAD_LEN)
    {
        dwError = 400;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    memset(buffer, '\0', 4097);

    dwError = REST_ENGINE_MORE_IO_REQUIRED;
    while(dwError == REST_ENGINE_MORE_IO_REQUIRED)
    {
        nWrite = (bytesLeft > 4096) ? 4096: bytesLeft;
        memcpy(buffer, (AllData + index), nWrite); 

        dwError = VmRESTSetData(
                  pRESTHandle,
                  ppResponse,
                  buffer,
                  nWrite,
                  &bytesRW
                  );
        index = index + nWrite;
        bytesLeft = bytesLeft - nWrite;
        memset(buffer, '\0', 4097);
        
    }
    BAIL_ON_VMREST_ERROR(dwError);

cleanup:
    if (AllData != NULL)
    {
        free(AllData);
        AllData = NULL;
    }

    return dwError;
error:

    if (fp != NULL)
    {
        fclose(fp);
        fp = NULL;
    }
    goto cleanup;
}

