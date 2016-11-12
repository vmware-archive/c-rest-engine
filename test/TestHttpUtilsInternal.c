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

#include <CuTest.h>
#include "includes.h"


void Test_VmRESTCopyStringTest1(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             src[MAX_HTTP_HEADER_VAL_LEN] = {0};
    char                             des[MAX_HTTP_HEADER_VAL_LEN] = {0};

    memset(src, '\0', MAX_HTTP_HEADER_VAL_LEN);
    memset(des, '\0', MAX_HTTP_HEADER_VAL_LEN);

    strcpy(src, "Simple String");

    /**** TEST 1: Valid case ****/
    dwError = VmRESTCopyString(
                  src,
                  des);
    CuAssertTrue(tc, !dwError);
    CuAssertStrEquals(tc, "Simple String", des);
}

void Test_VmRESTCopyStringTest2(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             src[MAX_HTTP_HEADER_VAL_LEN] = {0};
    char                             des[MAX_HTTP_HEADER_VAL_LEN] = {0};

    memset(src, '\0', MAX_HTTP_HEADER_VAL_LEN);
    memset(des, '\0', MAX_HTTP_HEADER_VAL_LEN);

    strcpy(src, "");

    /**** TEST 2: Empty String ****/
    dwError = VmRESTCopyString(
                  src,
                  des);
    CuAssertTrue(tc, dwError);
}

void Test_VmRESTCopyStringTest3(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             src[MAX_HTTP_HEADER_VAL_LEN] = {0};
    char                             des[MAX_HTTP_HEADER_VAL_LEN] = {0};

    memset(src, '\0', MAX_HTTP_HEADER_VAL_LEN);
    memset(des, '\0', MAX_HTTP_HEADER_VAL_LEN);

    strcpy(src, "Simple String");

    /**** TEST 3: NULL src ****/
    dwError = VmRESTCopyString(
                  NULL,
                  des);
    CuAssertTrue(tc, dwError);
}

void Test_VmRESTCopyStringTest4(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             src[MAX_HTTP_HEADER_VAL_LEN] = {0};
    char                             des[MAX_HTTP_HEADER_VAL_LEN] = {0};

    memset(src, '\0', MAX_HTTP_HEADER_VAL_LEN);
    memset(des, '\0', MAX_HTTP_HEADER_VAL_LEN);

    strcpy(src, "Simple String");

    /**** TEST 4: NULL des ****/
    dwError = VmRESTCopyString(
                  src,
                  NULL);
    CuAssertTrue(tc, dwError);
}

void Test_VmRESTGetHttpResponseHeaderTest1(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    PREST_RESPONSE                   pResponse = NULL;
    char*                            result = NULL;

    dwError = VmRESTAllocateHTTPResponsePacket(&pResponse);
    CuAssertTrue(tc, !dwError);

    dwError = VmRESTSetHttpHeader(
                  &pResponse,
                  "Connection",
                  "Keep-alive");
    CuAssertTrue(tc, !dwError);

    /**** TEST 1: Valid case ****/
    dwError = VmRESTGetHttpResponseHeader(
                  pResponse,
                  "Connection",
                  &result);
    CuAssertTrue(tc, !dwError);
    CuAssertStrEquals(tc, "Keep-alive", result);
 
    VmRESTFreeHTTPResponsePacket(&pResponse);
}

void Test_VmRESTGetHttpResponseHeaderTest2(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    PREST_RESPONSE                   pResponse = NULL;
    char*                            result = NULL;

    dwError = VmRESTAllocateHTTPResponsePacket(&pResponse);
    CuAssertTrue(tc, !dwError);

    dwError = VmRESTSetHttpHeader(
                  &pResponse,
                  "Connection",
                  "Keep-alive");
    CuAssertTrue(tc, !dwError);

    dwError = VmRESTSetHttpHeader(
                  &pResponse,
                  "Header",
                  "Value");
    CuAssertTrue(tc, !dwError);

    /**** TEST 2: Valid case ****/
    dwError = VmRESTGetHttpResponseHeader(
                  pResponse,
                  "Header",
                  &result);
    CuAssertTrue(tc, !dwError);
    CuAssertStrEquals(tc, "Value", result);

    dwError = VmRESTGetHttpResponseHeader(
                  pResponse,
                  "Connection",
                  &result);
    CuAssertTrue(tc, !dwError);
    CuAssertStrEquals(tc, "Keep-alive", result);

    VmRESTFreeHTTPResponsePacket(&pResponse);
}

void Test_VmRESTGetHttpResponseHeaderTest3(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    PREST_RESPONSE                   pResponse = NULL;
    char*                            result = NULL;

    dwError = VmRESTAllocateHTTPResponsePacket(&pResponse);
    CuAssertTrue(tc, !dwError);

    /**** TEST 3: No header exist ****/
    dwError = VmRESTGetHttpResponseHeader(
                  pResponse,
                  "Connection",
                  &result);
    CuAssertTrue(tc, dwError);

    VmRESTFreeHTTPResponsePacket(&pResponse);
}

void Test_VmRESTGetHttpResponseHeaderTest4(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    PREST_RESPONSE                   pResponse = NULL;
    char*                            result = NULL;

    dwError = VmRESTAllocateHTTPResponsePacket(&pResponse);
    CuAssertTrue(tc, !dwError);

    dwError = VmRESTSetHttpHeader(
                  &pResponse,
                  "Connection",
                  "Keep-alive");
    CuAssertTrue(tc, !dwError);

    /**** TEST 4: Valid case ****/
    dwError = VmRESTGetHttpResponseHeader(
                  pResponse,
                  "blah",
                  &result);
    CuAssertTrue(tc, dwError);

    VmRESTFreeHTTPResponsePacket(&pResponse);
}

void Test_VmRESTGetHttpResponseHeaderTest5(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    PREST_RESPONSE                   pResponse = NULL;
    char*                            result = NULL;

    dwError = VmRESTAllocateHTTPResponsePacket(&pResponse);
    CuAssertTrue(tc, !dwError);

    /**** TEST 5: NULL header ****/
    dwError = VmRESTGetHttpResponseHeader(
                  pResponse,
                  NULL,
                  &result);
    CuAssertTrue(tc, dwError);

    VmRESTFreeHTTPResponsePacket(&pResponse);
}

void Test_VmRESTMapStatusCodeToEnumAndReasonPhraseTest1(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    uint32_t                         result = 0;
    char                             reasonPhrase[MAX_REA_PHRASE_LEN] = {0};

    memset(reasonPhrase, '\0', MAX_REA_PHRASE_LEN);

    /**** TEST 1: Valid Case ****/
    dwError = VmRESTMapStatusCodeToEnumAndReasonPhrase( 
                  "400",
                  &result,
                  reasonPhrase);
    CuAssertTrue(tc, !dwError);
    CuAssertIntEquals(tc, BAD_REQUEST, result);
    CuAssertStrEquals(tc, "Bad Request", reasonPhrase);
}

void Test_VmRESTMapStatusCodeToEnumAndReasonPhraseTest2(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    uint32_t                         result = 0;
    char                             reasonPhrase[MAX_REA_PHRASE_LEN] = {0};

    memset(reasonPhrase, '\0', MAX_REA_PHRASE_LEN);

    /**** TEST 2: Invalid code ****/
    dwError = VmRESTMapStatusCodeToEnumAndReasonPhrase(
                  "adsdsaf",
                  &result,
                  reasonPhrase);
    CuAssertTrue(tc, dwError);
}

void Test_VmRESTMapStatusCodeToEnumAndReasonPhraseTest3(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             reasonPhrase[MAX_REA_PHRASE_LEN] = {0};

    memset(reasonPhrase, '\0', MAX_REA_PHRASE_LEN);
    
    /**** TEST 3: NULL result ****/
    dwError = VmRESTMapStatusCodeToEnumAndReasonPhrase(
                  "400",
                  NULL,
                  reasonPhrase);
    CuAssertTrue(tc, dwError);
}

void Test_VmRESTMapStatusCodeToEnumAndReasonPhraseTest4(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    uint32_t                         result = 0;

    /**** TEST 4: NULL reasonPhrase ****/
    dwError = VmRESTMapStatusCodeToEnumAndReasonPhrase(
                  "400",
                  &result,
                  NULL);
    CuAssertTrue(tc, dwError);
}

void Test_VmRESTMapStatusCodeToEnumAndReasonPhraseTest5(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    uint32_t                         result = 0;
    char                             reasonPhrase[2] = {0};

    memset(reasonPhrase, '\0', 2);

    /**** TEST 5: NULL reasonPhrase ****/
    dwError = VmRESTMapStatusCodeToEnumAndReasonPhrase(
                  "400",
                  &result,
                  reasonPhrase);
    CuAssertTrue(tc, dwError);
}

void Test_VmRESTSetHttpRequestHeaderTest1(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    PREST_REQUEST                    pRequest = NULL;
    uint32_t                         resStatus = 200;
    char*                            result = NULL;

    dwError = VmRESTAllocateHTTPRequestPacket(&pRequest);
    CuAssertTrue(tc, !dwError);

     /**** TEST 1: Valid case ****/
    dwError = VmRESTSetHttpRequestHeader(
                  pRequest,
                  "Connection",
                  "close",
                  &resStatus);
    CuAssertTrue(tc, !dwError);

    dwError = VmRESTGetHttpHeader(
                  pRequest,
                  "Connection",
                  &result);
    CuAssertTrue(tc, !dwError);
    CuAssertStrEquals(tc, "close", result);

    VmRESTFreeHTTPRequestPacket(&pRequest);
}

void Test_VmRESTSetHttpRequestHeaderTest2(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    PREST_REQUEST                    pRequest = NULL;
    uint32_t                         resStatus = 200;
    char*                            result = NULL;

    dwError = VmRESTAllocateHTTPRequestPacket(&pRequest);
    CuAssertTrue(tc, !dwError);

     /**** TEST 2: Wrong Query ****/
    dwError = VmRESTSetHttpRequestHeader(
                  pRequest,
                  "Connection",
                  "close",
                  &resStatus);
    CuAssertTrue(tc, !dwError);

    dwError = VmRESTGetHttpHeader(
                  pRequest,
                  "Accept",
                  &result);
    CuAssertTrue(tc, dwError);

    VmRESTFreeHTTPRequestPacket(&pRequest);
}

void Test_VmRESTSetHttpRequestHeaderTest3(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    PREST_REQUEST                    pRequest = NULL;
    uint32_t                         resStatus = 200;

    dwError = VmRESTAllocateHTTPRequestPacket(&pRequest);
    CuAssertTrue(tc, !dwError);

     /**** TEST 3: NULL case ****/
    dwError = VmRESTSetHttpRequestHeader(
                  pRequest,
                  NULL,
                  "blah",
                  &resStatus);
    CuAssertTrue(tc, dwError);

    VmRESTFreeHTTPRequestPacket(&pRequest);
}

void Test_VmRESTSetHttpRequestHeaderTest4(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    PREST_REQUEST                    pRequest = NULL;
    uint32_t                         resStatus = 200;

    dwError = VmRESTAllocateHTTPRequestPacket(&pRequest);
    CuAssertTrue(tc, !dwError);

     /**** TEST 4: NULL case ****/
    dwError = VmRESTSetHttpRequestHeader(
                  pRequest,
                  "Connection",
                  NULL,
                  &resStatus);
    CuAssertTrue(tc, dwError);

    VmRESTFreeHTTPRequestPacket(&pRequest);
}

void Test_VmRESTSetHttpRequestHeaderTest5(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    PREST_REQUEST                    pRequest = NULL;
    uint32_t                         resStatus = 200;
    char*                            result = NULL;

    dwError = VmRESTAllocateHTTPRequestPacket(&pRequest);
    CuAssertTrue(tc, !dwError);

     /**** TEST 5: Empty spaces ****/
    dwError = VmRESTSetHttpRequestHeader(
                  pRequest,
                  " Connection ",
                  " close ",
                  &resStatus);
    CuAssertTrue(tc, !dwError);

    dwError = VmRESTGetHttpHeader(
                  pRequest,
                  "Connection",
                  &result);
    CuAssertTrue(tc, !dwError);
    CuAssertStrEquals(tc, "close", result);

    VmRESTFreeHTTPRequestPacket(&pRequest);
}

void Test_VmRESTParseAndPopulateConfigFileTest1(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    VM_REST_CONFIG*                  pRESTConfig = NULL;
    FILE                             *fp = NULL;
    char                             buffer[2048] = {0};

    memset(buffer, '\0', 2048);
    strcpy(buffer, "SSL-Certificate /root/mycert.pem\nSSL-Key /root/mycert.pem\nPort 81\nLog-File /tmp/restServer.log\nClient-Count 5\nWorker-Thread-Count 5");
    fp  = fopen("/tmp/testConfig.txt", "w");
    if (fp)
    {
        fprintf(fp, "%s",buffer);
        fclose(fp);

        dwError = VmRESTParseAndPopulateConfigFile(
                  "/tmp/testConfig.txt",
                  &pRESTConfig);
        CuAssertTrue(tc, !dwError);

        CuAssertStrEquals(tc, "/root/mycert.pem", pRESTConfig->ssl_certificate);
        CuAssertStrEquals(tc, "/root/mycert.pem", pRESTConfig->ssl_key);
        CuAssertStrEquals(tc, "81", pRESTConfig->server_port);
        CuAssertStrEquals(tc, "/tmp/restServer.log", pRESTConfig->debug_log_file);
        CuAssertStrEquals(tc, "5", pRESTConfig->client_count);
        CuAssertStrEquals(tc, "5", pRESTConfig->worker_thread_count);
    }
    
    VmRESTFreeMemory(pRESTConfig);
}

void Test_VmRESTParseAndPopulateConfigFileTest2(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    VM_REST_CONFIG*                  pRESTConfig = NULL;
    FILE                             *fp = NULL;
    char                             buffer[2048] = {0};

    memset(buffer, '\0', 2048);
    strcpy(buffer, "SSL-Certificate /root/mycert.pemdddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd\nSSL-Key /root/mycert.pem\nPort 81\nLog-File /tmp/restServer.log\nClient-Count 5\nWorker-Thread-Count 5");
    fp  = fopen("/tmp/testConfig.txt", "w");
    if (fp)
    {
        fprintf(fp, "%s",buffer);
        fclose(fp);

        dwError = VmRESTParseAndPopulateConfigFile(
                  "/tmp/testConfig.txt",
                  &pRESTConfig);
        CuAssertTrue(tc, !dwError);

        CuAssertStrEquals(tc, "/root/mycert.pem", pRESTConfig->ssl_certificate);
        CuAssertStrEquals(tc, "/root/mycert.pem", pRESTConfig->ssl_key);
        CuAssertStrEquals(tc, "81", pRESTConfig->server_port);
        CuAssertStrEquals(tc, "/tmp/restServer.log", pRESTConfig->debug_log_file);
        CuAssertStrEquals(tc, "5", pRESTConfig->client_count);
        CuAssertStrEquals(tc, "5", pRESTConfig->worker_thread_count);
    }

    VmRESTFreeMemory(pRESTConfig);
}

void Test_VmRESTValidateConfigTest1(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    VM_REST_CONFIG*                  pRESTConfig = NULL;

    dwError = VmRESTAllocateMemory(
              sizeof(VM_REST_CONFIG),
              (void**)&pRESTConfig
              );
    CuAssertTrue(tc, !dwError);

    strcpy(pRESTConfig->server_port, "81");
    strcpy(pRESTConfig->worker_thread_count, "10");
    strcpy(pRESTConfig->client_count, "9");
    strcpy(pRESTConfig->debug_log_file, "/tmp/xxx.txt");
    
    /**** TEST 1 : Valid case ****/

    dwError = VmRESTValidateConfig(pRESTConfig);
    CuAssertTrue(tc, !dwError);
    CuAssertStrEquals(tc, "81", pRESTConfig->server_port);
    CuAssertStrEquals(tc, "9", pRESTConfig->client_count);
    CuAssertStrEquals(tc, "/tmp/xxx.txt", pRESTConfig->debug_log_file);
    CuAssertStrEquals(tc, "10", pRESTConfig->worker_thread_count);

    VmRESTFreeMemory(pRESTConfig);

}

void Test_VmRESTValidateConfigTest2(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    VM_REST_CONFIG*                  pRESTConfig = NULL;
    
    dwError = VmRESTAllocateMemory(
              sizeof(VM_REST_CONFIG),
              (void**)&pRESTConfig
              );
    CuAssertTrue(tc, !dwError);
    /**** TEST 2 : No Port specified ****/

    dwError = VmRESTValidateConfig(pRESTConfig);
    CuAssertTrue(tc, dwError);

    VmRESTFreeMemory(pRESTConfig);

}

void Test_VmRESTValidateConfigTest3(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    VM_REST_CONFIG*                  pRESTConfig = NULL;
    
    dwError = VmRESTAllocateMemory(
              sizeof(VM_REST_CONFIG),
              (void**)&pRESTConfig
              );
    CuAssertTrue(tc, !dwError);

    strcpy(pRESTConfig->server_port, "81s");

    /**** TEST 3 : SSL Port but not SSL key and cert paths ****/

    dwError = VmRESTValidateConfig(pRESTConfig);
    CuAssertTrue(tc, dwError);
  
    VmRESTFreeMemory(pRESTConfig);

}

void Test_VmRESTValidateConfigTest4(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    VM_REST_CONFIG*                  pRESTConfig = NULL;

    dwError = VmRESTAllocateMemory(
              sizeof(VM_REST_CONFIG),
              (void**)&pRESTConfig
              );
    CuAssertTrue(tc, !dwError);

    strcpy(pRESTConfig->server_port, "81s");
    strcpy(pRESTConfig->ssl_certificate, "/root/mycert.pem");
    strcpy(pRESTConfig->ssl_key, "/root/mycert.pem");

    /**** TEST 4 : valid SSL config ****/

    dwError = VmRESTValidateConfig(pRESTConfig);
    CuAssertTrue(tc, !dwError);
    CuAssertStrEquals(tc, "/root/mycert.pem", pRESTConfig->ssl_certificate);
    CuAssertStrEquals(tc, "/root/mycert.pem", pRESTConfig->ssl_key);

    /**** Defaults value for missing configs ****/
    CuAssertStrEquals(tc, "5", pRESTConfig->client_count);
    CuAssertStrEquals(tc, "/tmp/restServer.log", pRESTConfig->debug_log_file);
    CuAssertStrEquals(tc, "5", pRESTConfig->worker_thread_count);

    VmRESTFreeMemory(pRESTConfig);

}

void Test_VmRESTValidateConfigTest5(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    VM_REST_CONFIG*                  pRESTConfig = NULL;

    dwError = VmRESTAllocateMemory(
              sizeof(VM_REST_CONFIG),
              (void**)&pRESTConfig
              );
    CuAssertTrue(tc, !dwError);
    if (pRESTConfig != NULL)
    {
        memset(pRESTConfig->server_port, '\0', MAX_SERVER_PORT_LEN);
        strncpy(pRESTConfig->server_port, "blahbal",(MAX_SERVER_PORT_LEN - 1));
    }
    /**** TEST 5 : Invalid Port number ****/

    dwError = VmRESTValidateConfig(pRESTConfig);
    CuAssertTrue(tc, dwError);

    VmRESTFreeMemory(pRESTConfig);
}

void Test_VmRESTValidateConfigTest6(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    VM_REST_CONFIG*                  pRESTConfig = NULL;

    dwError = VmRESTAllocateMemory(
              sizeof(VM_REST_CONFIG),
              (void**)&pRESTConfig
              );
    CuAssertTrue(tc, !dwError);

    memset(pRESTConfig, '\0', sizeof(VM_REST_CONFIG));

    strncpy(pRESTConfig->server_port, "81", (MAX_SERVER_PORT_LEN - 1));
    strncpy(pRESTConfig->client_count, "blah", (MAX_CLIENT_ALLOWED_LEN - 1));

    /**** TEST 6 :client count ****/

    dwError = VmRESTValidateConfig(pRESTConfig);
    CuAssertTrue(tc, !dwError);
    /**** Defaults to 5 *****/
    CuAssertStrEquals(tc, "5", pRESTConfig->client_count);
    VmRESTFreeMemory(pRESTConfig);

}

void Test_VmRESTValidateConfigTest7(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    VM_REST_CONFIG*                  pRESTConfig = NULL;

    dwError = VmRESTAllocateMemory(
              sizeof(VM_REST_CONFIG),
              (void**)&pRESTConfig
              );
    CuAssertTrue(tc, !dwError);

    strcpy(pRESTConfig->server_port, "81");
    strcpy(pRESTConfig->worker_thread_count, "blah");

    /**** TEST 7 : Wrong thread count ****/

    dwError = VmRESTValidateConfig(pRESTConfig);
    CuAssertTrue(tc, !dwError);
    /**** Defaults to 5 *****/
    CuAssertStrEquals(tc, "5", pRESTConfig->worker_thread_count);
    VmRESTFreeMemory(pRESTConfig);

}

void Test_VmRESTValidateConfigTest8(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    VM_REST_CONFIG*                  pRESTConfig = NULL;

    dwError = VmRESTAllocateMemory(
              sizeof(VM_REST_CONFIG),
              (void**)&pRESTConfig
              );
    CuAssertTrue(tc, !dwError);

    strcpy(pRESTConfig->server_port, "81");

    /**** TEST 8 : Missing log file****/

    dwError = VmRESTValidateConfig(pRESTConfig);
    CuAssertTrue(tc, !dwError);
    /**** Defaults to /tmp/resSErver.log *****/
    CuAssertStrEquals(tc, "/tmp/restServer.log", pRESTConfig->debug_log_file);
    VmRESTFreeMemory(pRESTConfig);

}

void Test_VmRESTCopyConfigTest1(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    VM_REST_CONFIG*                  pRESTConfig = NULL;
    REST_CONF                        conf = {0};

    conf.pSSLCertificate = "/root/mycert.pem";
    conf.pSSLKey = "/root/mycert.pem";
    conf.pServerPort = "81";
    conf.pDebugLogFile = "/tmp/restServer.log";
    conf.pClientCount = "8";
    conf.pMaxWorkerThread = "9";

    /**** TEST 1: Valid case ****/
    dwError = VmRESTCopyConfig(
                  &conf,
                  &pRESTConfig);
    CuAssertTrue(tc, !dwError);

    CuAssertStrEquals(tc, "/root/mycert.pem", pRESTConfig->ssl_certificate);
    CuAssertStrEquals(tc, "/root/mycert.pem", pRESTConfig->ssl_key);
    CuAssertStrEquals(tc, "81", pRESTConfig->server_port);
    CuAssertStrEquals(tc, "/tmp/restServer.log", pRESTConfig->debug_log_file);
    CuAssertStrEquals(tc, "8", pRESTConfig->client_count);
    CuAssertStrEquals(tc, "9", pRESTConfig->worker_thread_count);

    VmRESTFreeMemory(pRESTConfig);

}

void Test_VmRESTCopyConfigTest2(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    VM_REST_CONFIG*                  pRESTConfig = NULL;
    REST_CONF                        conf = {0};

    conf.pServerPort = "81s";
    conf.pDebugLogFile = "/tmp/restServer.log";
    conf.pClientCount = "8";
    conf.pMaxWorkerThread = "9";

    /**** TEST 2: SSL Config but missing SSL files ****/
    dwError = VmRESTCopyConfig(
                  &conf,
                  &pRESTConfig);
    CuAssertTrue(tc, dwError);

    VmRESTFreeMemory(pRESTConfig);

}

void Test_VmRESTCopyConfigTest3(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    VM_REST_CONFIG*                  pRESTConfig = NULL;
    REST_CONF                        conf = {0};

    
    conf.pServerPort = "81";
    
    /**** TEST 3: Configure all default values ****/
    dwError = VmRESTCopyConfig(
                  &conf,
                  &pRESTConfig);
    CuAssertTrue(tc, !dwError);
    CuAssertStrEquals(tc, "81", pRESTConfig->server_port);
    CuAssertStrEquals(tc, "/tmp/restServer.log", pRESTConfig->debug_log_file);
    CuAssertStrEquals(tc, "5", pRESTConfig->client_count);
    CuAssertStrEquals(tc, "5", pRESTConfig->worker_thread_count);
    
    VmRESTFreeMemory(pRESTConfig);

}

void Test_VmRESTCopyConfigTest4(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    VM_REST_CONFIG*                  pRESTConfig = NULL;
    REST_CONF                        conf = {0};

    conf.pServerPort = "blah";

    /**** TEST 4: Wrong port ****/
    dwError = VmRESTCopyConfig(
                  &conf,
                  &pRESTConfig);
    CuAssertTrue(tc, dwError);

    VmRESTFreeMemory(pRESTConfig);

}

void Test_VmRESTCopyConfigTest5(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    VM_REST_CONFIG*                  pRESTConfig = NULL;
    REST_CONF                        conf = {0};

    conf.pServerPort = NULL;

    /**** TEST 5: Null Port ****/
    dwError = VmRESTCopyConfig(
                  &conf,
                  &pRESTConfig);
    CuAssertTrue(tc, dwError);

    VmRESTFreeMemory(pRESTConfig);

}

void Test_VmRESTCopyConfigTest6(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    VM_REST_CONFIG*                  pRESTConfig = NULL;
    REST_CONF                        conf = {0};

    conf.pServerPort = "81";
    conf.pClientCount = "blah";
    conf.pMaxWorkerThread = "blah";

    /**** TEST 6: Wrong info ****/
    dwError = VmRESTCopyConfig(
                  &conf,
                  &pRESTConfig);
    CuAssertTrue(tc, !dwError);

    CuAssertStrEquals(tc, "5", pRESTConfig->client_count);
    CuAssertStrEquals(tc, "5", pRESTConfig->worker_thread_count);

    VmRESTFreeMemory(pRESTConfig);

}

void Test_VmRESTCopyConfigTest7(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    VM_REST_CONFIG*                  pRESTConfig = NULL;
    REST_CONF                        conf = {0};

    conf.pServerPort = "81123213213123123123123123123123123123123123123123123123123123123123123";
    conf.pClientCount = "4555555555555555555555555555555555555555555555555555555555555555555555";
    conf.pMaxWorkerThread = "555555555555555555555555555555555555555555555555555555555555555555";
    conf.pSSLCertificate = "/root/mycert.pemdsdasssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssdsdddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd";
    conf.pSSLKey = "/root/mycert.pemdsssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssdsssssssssssssssdsdddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd";
    conf.pDebugLogFile = "/tmp/restServer.logdsfdsfffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffdfdsssssssssssssssdsdddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd";

    /**** TEST 7: Large data length ****/
    dwError = VmRESTCopyConfig(
                  &conf,
                  &pRESTConfig);
    CuAssertTrue(tc, dwError);

    VmRESTFreeMemory(pRESTConfig);

}

void Test_VmRESTGetChunkSizeTest1(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             line[MAX_DATA_BUFFER_LEN] = {0};
    uint32_t                         skipBytes = 0;
    uint32_t                         chunkSize = 0;

    memset(line, '\0', MAX_DATA_BUFFER_LEN);
    strcpy(line, "16\r\n");

    /**** TEST 1: Valid Case ****/

    dwError = VmRESTGetChunkSize(
                  line,
                  &skipBytes,
                  &chunkSize);
    CuAssertTrue(tc, !dwError);

    CuAssertIntEquals(tc, 22, chunkSize);
    CuAssertIntEquals(tc, 4, skipBytes);

}

void Test_VmRESTGetChunkSizeTest2(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             line[MAX_DATA_BUFFER_LEN] = {0};
    uint32_t                         skipBytes = 0;
    uint32_t                         chunkSize = 0;

    memset(line, '\0', MAX_DATA_BUFFER_LEN);
    strcpy(line, "16");

    /**** TEST 2: CRLF missinh ****/

    dwError = VmRESTGetChunkSize(
                  line,
                  &skipBytes,
                  &chunkSize);
    CuAssertTrue(tc, dwError);

}

void Test_VmRESTGetChunkSizeTest3(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             line[MAX_DATA_BUFFER_LEN] = {0};
    uint32_t                         skipBytes = 0;
    uint32_t                         chunkSize = 0;

    memset(line, '\0', MAX_DATA_BUFFER_LEN);
    strcpy(line, "0x16\r\n");

    /**** TEST 3: Valid Case ****/

    dwError = VmRESTGetChunkSize(
                  line,
                  &skipBytes,
                  &chunkSize);
    CuAssertTrue(tc, !dwError);

    CuAssertIntEquals(tc, 22, chunkSize);
    CuAssertIntEquals(tc, 6, skipBytes);

}

void Test_VmRESTGetChunkSizeTest4(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             line[MAX_DATA_BUFFER_LEN] = {0};
    uint32_t                         skipBytes = 0;
    uint32_t                         chunkSize = 0;

    memset(line, '\0', MAX_DATA_BUFFER_LEN);
    strcpy(line, "fadsffs\r\n");

    /**** TEST 4: Invalid ****/

    dwError = VmRESTGetChunkSize(
                  line,
                  &skipBytes,
                  &chunkSize);
    CuAssertTrue(tc, dwError);

}

void Test_VmRESTGetChunkSizeTest5(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             line[MAX_DATA_BUFFER_LEN] = {0};
    uint32_t                         skipBytes = 0;
    uint32_t                         chunkSize = 0;

    memset(line, '\0', MAX_DATA_BUFFER_LEN);
    strcpy(line, "\r\n16\r\n");

    /**** TEST 5: Invalid Case ****/

    dwError = VmRESTGetChunkSize(
                  line,
                  &skipBytes,
                  &chunkSize);
    CuAssertTrue(tc, dwError);

}

void Test_VmRESTGetChunkSizeTest6(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             line[MAX_DATA_BUFFER_LEN] = {0};
    uint32_t                         skipBytes = 0;
    uint32_t                         chunkSize = 0;

    memset(line, '\0', MAX_DATA_BUFFER_LEN);
    strcpy(line, "16\r\n");

    /**** TEST 6: NULL line ****/

    dwError = VmRESTGetChunkSize(
                  NULL,
                  &skipBytes,
                  &chunkSize);
    CuAssertTrue(tc, dwError);
}

void Test_VmRESTGetChunkSizeTest7(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             line[MAX_DATA_BUFFER_LEN] = {0};
    uint32_t                         skipBytes = 0;
    uint32_t                         chunkSize = 0;

    memset(line, '\0', MAX_DATA_BUFFER_LEN);
    strcpy(line, " 16\r\n");

    /**** TEST 7: Valid Case ****/

    dwError = VmRESTGetChunkSize(
                  line,
                  &skipBytes,
                  &chunkSize);
    CuAssertTrue(tc, !dwError);

    CuAssertIntEquals(tc, 22, chunkSize);
    CuAssertIntEquals(tc, 4, skipBytes);

}

void Test_VmRESTCopyDataWithoutCRLFTest1(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             src[MAX_DATA_BUFFER_LEN] = {0};
    char                             des[MAX_DATA_BUFFER_LEN] = {0};
    uint32_t                         actualBytes = 0;

    memset(src, '\0', MAX_DATA_BUFFER_LEN);
    memset(des, '\0', MAX_DATA_BUFFER_LEN);
    strcpy(src, "Simple data with \r\n at last");

    /**** TEST 1: Valid Case ****/

    dwError = VmRESTCopyDataWithoutCRLF(
                  strlen(src),
                  src,
                  des,
                  &actualBytes);
    CuAssertTrue(tc, !dwError);

    CuAssertIntEquals(tc, 25, actualBytes);
    CuAssertStrEquals(tc, "Simple data with  at last", des);

}

void Test_VmRESTCopyDataWithoutCRLFTest2(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             src[MAX_DATA_BUFFER_LEN] = {0};
    char                             des[MAX_DATA_BUFFER_LEN] = {0};
    uint32_t                         actualBytes = 0;

    memset(src, '\0', MAX_DATA_BUFFER_LEN);
    memset(des, '\0', MAX_DATA_BUFFER_LEN);
    strcpy(src, "Simple data with at last");

    /**** TEST 2: Valid Case ****/

    dwError = VmRESTCopyDataWithoutCRLF(
                  strlen(src),
                  src,
                  des,
                  &actualBytes);
    CuAssertTrue(tc, !dwError);

    CuAssertIntEquals(tc, 24, actualBytes);
    CuAssertStrEquals(tc, "Simple data with at last", des);

}

void Test_VmRESTCopyDataWithoutCRLFTest3(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             src[MAX_DATA_BUFFER_LEN] = {0};
    char                             des[MAX_DATA_BUFFER_LEN] = {0};
    uint32_t                         actualBytes = 0;

    memset(src, '\0', MAX_DATA_BUFFER_LEN);
    memset(des, '\0', MAX_DATA_BUFFER_LEN);
    strcpy(src, "\r\n");

    /**** TEST 3: Valid Case ****/

    dwError = VmRESTCopyDataWithoutCRLF(
                  strlen(src),
                  src,
                  des,
                  &actualBytes);
    CuAssertTrue(tc, !dwError);

    CuAssertIntEquals(tc, 0, actualBytes);
    CuAssertStrEquals(tc, "", des);

}

void Test_VmRESTCopyDataWithoutCRLFTest4(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             src[MAX_DATA_BUFFER_LEN] = {0};
    char                             des[MAX_DATA_BUFFER_LEN] = {0};
    uint32_t                         actualBytes = 0;

    memset(src, '\0', MAX_DATA_BUFFER_LEN);
    memset(des, '\0', MAX_DATA_BUFFER_LEN);
    strcpy(src, "\r\nThis\r\n is \r\n payload\r\n");

    /**** TEST 4: Valid Case ****/

    dwError = VmRESTCopyDataWithoutCRLF(
                  strlen(src),
                  src,
                  des,
                  &actualBytes);
    CuAssertTrue(tc, !dwError);

    CuAssertIntEquals(tc, 16, actualBytes);
    CuAssertStrEquals(tc, "This is payload", des);

}

void Test_VmRESTCopyDataWithoutCRLFTest5(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             src[MAX_DATA_BUFFER_LEN] = {0};
    char                             des[MAX_DATA_BUFFER_LEN] = {0};
    uint32_t                         actualBytes = 0;

    memset(src, '\0', MAX_DATA_BUFFER_LEN);
    memset(des, '\0', MAX_DATA_BUFFER_LEN);
    strcpy(src, "xx");

    /**** TEST 5: wrong length ****/

    dwError = VmRESTCopyDataWithoutCRLF(
                  (strlen(src)- 4),
                  src,
                  des,
                  &actualBytes);
    CuAssertTrue(tc, dwError);
}

void Test_VmRESTCopyDataWithoutCRLFTest6(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             src[MAX_DATA_BUFFER_LEN] = {0};
    char                             des[MAX_DATA_BUFFER_LEN] = {0};
    uint32_t                         actualBytes = 0;

    memset(src, '\0', MAX_DATA_BUFFER_LEN);
    memset(des, '\0', MAX_DATA_BUFFER_LEN);
    strcpy(src, "\r X \n X\n");

    /**** TEST 6: Valid case ****/

    dwError = VmRESTCopyDataWithoutCRLF(
                  strlen(src),
                  src,
                  des,
                  &actualBytes);
    CuAssertTrue(tc, !dwError);

    CuAssertIntEquals(tc, 8, actualBytes);
    CuAssertStrEquals(tc, "\r X \n X\n", des);

}


CuSuite* CuGetTestHttpUtilsInternalSuite(void)
{
    CuSuite* suite = CuSuiteNew();

    SUITE_ADD_TEST(suite, Test_VmRESTCopyStringTest1);
    SUITE_ADD_TEST(suite, Test_VmRESTCopyStringTest2);
    SUITE_ADD_TEST(suite, Test_VmRESTCopyStringTest3);
    SUITE_ADD_TEST(suite, Test_VmRESTCopyStringTest4);
    SUITE_ADD_TEST(suite, Test_VmRESTGetHttpResponseHeaderTest1);
    SUITE_ADD_TEST(suite, Test_VmRESTGetHttpResponseHeaderTest2);
    SUITE_ADD_TEST(suite, Test_VmRESTGetHttpResponseHeaderTest3);
    SUITE_ADD_TEST(suite, Test_VmRESTGetHttpResponseHeaderTest4);
    SUITE_ADD_TEST(suite, Test_VmRESTGetHttpResponseHeaderTest5);
    SUITE_ADD_TEST(suite, Test_VmRESTMapStatusCodeToEnumAndReasonPhraseTest1);
    SUITE_ADD_TEST(suite, Test_VmRESTMapStatusCodeToEnumAndReasonPhraseTest2);
    SUITE_ADD_TEST(suite, Test_VmRESTMapStatusCodeToEnumAndReasonPhraseTest3);
    SUITE_ADD_TEST(suite, Test_VmRESTMapStatusCodeToEnumAndReasonPhraseTest4);
    SUITE_ADD_TEST(suite, Test_VmRESTMapStatusCodeToEnumAndReasonPhraseTest5);
    SUITE_ADD_TEST(suite, Test_VmRESTSetHttpRequestHeaderTest1);
    SUITE_ADD_TEST(suite, Test_VmRESTSetHttpRequestHeaderTest2);
    SUITE_ADD_TEST(suite, Test_VmRESTSetHttpRequestHeaderTest3);
    SUITE_ADD_TEST(suite, Test_VmRESTSetHttpRequestHeaderTest4);
    SUITE_ADD_TEST(suite, Test_VmRESTSetHttpRequestHeaderTest5);
    SUITE_ADD_TEST(suite, Test_VmRESTParseAndPopulateConfigFileTest1);
    /***** CRASHING TEST: This feature will be eliminated: No need to fix *****
    SUITE_ADD_TEST(suite, Test_VmRESTParseAndPopulateConfigFileTest2);
    **************************************************************************/
    SUITE_ADD_TEST(suite, Test_VmRESTValidateConfigTest1);
    SUITE_ADD_TEST(suite, Test_VmRESTValidateConfigTest2);
    SUITE_ADD_TEST(suite, Test_VmRESTValidateConfigTest3);
    SUITE_ADD_TEST(suite, Test_VmRESTValidateConfigTest4);
    SUITE_ADD_TEST(suite, Test_VmRESTValidateConfigTest5);
    SUITE_ADD_TEST(suite, Test_VmRESTValidateConfigTest6);
    SUITE_ADD_TEST(suite, Test_VmRESTValidateConfigTest7);
    SUITE_ADD_TEST(suite, Test_VmRESTValidateConfigTest8);
    SUITE_ADD_TEST(suite, Test_VmRESTCopyConfigTest1);
    SUITE_ADD_TEST(suite, Test_VmRESTCopyConfigTest2);
    SUITE_ADD_TEST(suite, Test_VmRESTCopyConfigTest3);
    SUITE_ADD_TEST(suite, Test_VmRESTCopyConfigTest4);
    SUITE_ADD_TEST(suite, Test_VmRESTCopyConfigTest5);
    SUITE_ADD_TEST(suite, Test_VmRESTCopyConfigTest6);
    /**** CRASHING TEST: FIX ASAP **********************
    SUITE_ADD_TEST(suite, Test_VmRESTCopyConfigTest7);
    ***************************************************/
    SUITE_ADD_TEST(suite, Test_VmRESTGetChunkSizeTest1);
    SUITE_ADD_TEST(suite, Test_VmRESTGetChunkSizeTest2);
    SUITE_ADD_TEST(suite, Test_VmRESTGetChunkSizeTest3);
    SUITE_ADD_TEST(suite, Test_VmRESTGetChunkSizeTest4);
    SUITE_ADD_TEST(suite, Test_VmRESTGetChunkSizeTest5);
    SUITE_ADD_TEST(suite, Test_VmRESTGetChunkSizeTest6);
    SUITE_ADD_TEST(suite, Test_VmRESTGetChunkSizeTest7);
    SUITE_ADD_TEST(suite, Test_VmRESTCopyDataWithoutCRLFTest1);
    SUITE_ADD_TEST(suite, Test_VmRESTCopyDataWithoutCRLFTest2);
    SUITE_ADD_TEST(suite, Test_VmRESTCopyDataWithoutCRLFTest3);
    SUITE_ADD_TEST(suite, Test_VmRESTCopyDataWithoutCRLFTest4);
    /**** CRASHING TEST: FIX ASAP *****************************
    SUITE_ADD_TEST(suite, Test_VmRESTCopyDataWithoutCRLFTest5);
    ***********************************************************/
    SUITE_ADD_TEST(suite, Test_VmRESTCopyDataWithoutCRLFTest6);


    return suite;
}

