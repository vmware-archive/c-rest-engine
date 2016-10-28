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
    /***** CRASHING TEST: FIX ASAP ***********************************
    SUITE_ADD_TEST(suite, Test_VmRESTParseAndPopulateConfigFileTest2);
    ******************************************************************/

    return suite;
}

