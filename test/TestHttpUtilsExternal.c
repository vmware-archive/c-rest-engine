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


void Test_VmRESTGetHttpMethodTest1(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    PREST_REQUEST                    pRequest = NULL;
    char*                            method = NULL;

    dwError = VmRESTAllocateHTTPRequestPacket(&pRequest);
    CuAssertTrue(tc, !dwError);

    strcpy(pRequest->requestLine->method, "GET");

    /**** TEST 1: Valid case ****/
    dwError = VmRESTGetHttpMethod(
                  pRequest,
                  &method);
    CuAssertTrue(tc, !dwError);
    CuAssertStrEquals(tc, "GET", method);

    VmRESTFreeHTTPRequestPacket(&pRequest);
}

void Test_VmRESTGetHttpMethodTest2(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    PREST_REQUEST                    pRequest = NULL;
    char*                            method = NULL;

    dwError = VmRESTAllocateHTTPRequestPacket(&pRequest);
    CuAssertTrue(tc, !dwError);

    strcpy(pRequest->requestLine->method, "JUNK");

    /**** TEST 2: Junk name case - Valid case Set should fail  ****/
    dwError = VmRESTGetHttpMethod(
                  pRequest,
                  &method);
    CuAssertTrue(tc, !dwError);

    VmRESTFreeHTTPRequestPacket(&pRequest);
}

void Test_VmRESTGetHttpMethodTest3(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    PREST_REQUEST                    pRequest = NULL;
    char*                            method = NULL;

    dwError = VmRESTAllocateHTTPRequestPacket(&pRequest);
    CuAssertTrue(tc, !dwError);
    
    /**** TEST 3: method name not set ****/
    dwError = VmRESTGetHttpMethod(
                  pRequest,
                  &method);
    CuAssertTrue(tc, dwError);

    VmRESTFreeHTTPRequestPacket(&pRequest);
}

void Test_VmRESTGetHttpURITest1(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    PREST_REQUEST                    pRequest = NULL;
    char*                            uri = NULL;

    dwError = VmRESTAllocateHTTPRequestPacket(&pRequest);
    CuAssertTrue(tc, !dwError);

    strcpy(pRequest->requestLine->uri, "/v1/pkg?version=1.0");

    /**** TEST 1: Valid case ****/
    dwError = VmRESTGetHttpURI(
                  pRequest,
                  &uri);
    CuAssertTrue(tc, !dwError);
    CuAssertStrEquals(tc, "/v1/pkg?version=1.0", uri);

    VmRESTFreeHTTPRequestPacket(&pRequest);
}

void Test_VmRESTGetHttpURITest2(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    PREST_REQUEST                    pRequest = NULL;
    char*                            uri = NULL;

    dwError = VmRESTAllocateHTTPRequestPacket(&pRequest);
    CuAssertTrue(tc, !dwError);

    /**** TEST 2: Uri not present  ****/
    dwError = VmRESTGetHttpURI(
                  pRequest,
                  &uri);
    CuAssertTrue(tc, dwError);

    VmRESTFreeHTTPRequestPacket(&pRequest);
}

void Test_VmRESTGetHttpVersionTest1(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    PREST_REQUEST                    pRequest = NULL;
    char*                            version = NULL;

    dwError = VmRESTAllocateHTTPRequestPacket(&pRequest);
    CuAssertTrue(tc, !dwError);

    strcpy(pRequest->requestLine->version, "HTTP/1.1");

    /**** TEST 1: Valid case ****/
    dwError = VmRESTGetHttpVersion(
                  pRequest,
                  &version);
    CuAssertTrue(tc, !dwError);
    CuAssertStrEquals(tc, "HTTP/1.1", version);

    VmRESTFreeHTTPRequestPacket(&pRequest);
}

void Test_VmRESTGetHttpVersionTest2(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    PREST_REQUEST                    pRequest = NULL;
    char*                            version = NULL;

    dwError = VmRESTAllocateHTTPRequestPacket(&pRequest);
    CuAssertTrue(tc, !dwError);
    
    /**** TEST 2: version not present  ****/
    dwError = VmRESTGetHttpVersion(
                  pRequest,
                  &version);
    CuAssertTrue(tc, dwError);

    VmRESTFreeHTTPRequestPacket(&pRequest);
}

void Test_VmRESTGetHttpVersionTest3(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    PREST_REQUEST                    pRequest = NULL;
    char*                            version = NULL;

    dwError = VmRESTAllocateHTTPRequestPacket(&pRequest);
    CuAssertTrue(tc, !dwError);

    strcpy(pRequest->requestLine->version, "HTTP/0.9");

    /**** TEST 3: Wrong version -valid case****/
    dwError = VmRESTGetHttpVersion(
                  pRequest,
                  &version);
    CuAssertTrue(tc, !dwError);
    CuAssertStrEquals(tc, "HTTP/0.9", version);

    VmRESTFreeHTTPRequestPacket(&pRequest);
}

void Test_VmRESTGetHttpHeaderTest1(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    PREST_REQUEST                    pRequest = NULL;
    char*                            result = NULL;
    uint32_t                         resStatus = 200;

    dwError = VmRESTAllocateHTTPRequestPacket(&pRequest);
    CuAssertTrue(tc, !dwError);

    /**** TEST 1: Valid Test ****/
    dwError = VmRESTSetHttpRequestHeader(
                  pRequest,
                  "Connection",
                  "close",
                  &resStatus);
    CuAssertTrue(tc, !dwError);
    CuAssertIntEquals(tc, 200, resStatus);

    dwError = VmRESTSetHttpRequestHeader(
                  pRequest,
                  "Accept",
                  "*/*",
                  &resStatus);
    CuAssertTrue(tc, !dwError);
    CuAssertIntEquals(tc, 200, resStatus);

    dwError = VmRESTGetHttpHeader(
                  pRequest,
                  "Accept",
                  &result);
    CuAssertTrue(tc, !dwError);
    CuAssertStrEquals(tc, "*/*", result);

    dwError = VmRESTGetHttpHeader(
                  pRequest,
                  "Connection",
                  &result);
    CuAssertTrue(tc, !dwError);
    CuAssertStrEquals(tc, "close", result);

    VmRESTFreeHTTPRequestPacket(&pRequest);
}

void Test_VmRESTGetHttpHeaderTest2(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    PREST_REQUEST                    pRequest = NULL;
    char*                            result = NULL;
    uint32_t                         resStatus = 200;

    dwError = VmRESTAllocateHTTPRequestPacket(&pRequest);
    CuAssertTrue(tc, !dwError);

    /**** TEST 2: Header not present ****/
    dwError = VmRESTSetHttpRequestHeader(
                  pRequest,
                  "Connection",
                  "close",
                  &resStatus);
    CuAssertTrue(tc, !dwError);
    CuAssertIntEquals(tc, 200, resStatus);

    dwError = VmRESTSetHttpRequestHeader(
                  pRequest,
                  "Accept",
                  "*/*",
                  &resStatus);
    CuAssertTrue(tc, !dwError);
    CuAssertIntEquals(tc, 200, resStatus);
    
    dwError = VmRESTGetHttpHeader(
                  pRequest,
                  "Content-Length",
                  &result);
    CuAssertTrue(tc, !dwError);
    CuAssertTrue(tc, !result);

    dwError = VmRESTGetHttpHeader(
                  pRequest,
                  "Connection",
                  &result);
    CuAssertTrue(tc, !dwError);
    CuAssertStrEquals(tc, "close", result);

    VmRESTFreeHTTPRequestPacket(&pRequest);
}

void Test_VmRESTGetHttpHeaderTest3(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    PREST_REQUEST                    pRequest = NULL;
    char*                            result = NULL;
    uint32_t                         resStatus = 200;

    dwError = VmRESTAllocateHTTPRequestPacket(&pRequest);
    CuAssertTrue(tc, !dwError);

    /**** TEST 3: Randon string as header ****/
    dwError = VmRESTSetHttpRequestHeader(
                  pRequest,
                  "Connection",
                  "close",
                  &resStatus);
    CuAssertTrue(tc, !dwError);
    CuAssertIntEquals(tc, 200, resStatus);

    dwError = VmRESTSetHttpRequestHeader(
                  pRequest,
                  "junkHeader",
                  "junkValue",
                  &resStatus);
    CuAssertTrue(tc, !dwError);
    CuAssertIntEquals(tc, 200, resStatus);

    dwError = VmRESTGetHttpHeader(
                  pRequest,
                  "junkHeader",
                  &result);
    CuAssertTrue(tc, !dwError);
    CuAssertStrEquals(tc, "junkValue", result);

    dwError = VmRESTGetHttpHeader(
                  pRequest,
                  "Connection",
                  &result);
    CuAssertTrue(tc, !dwError);
    CuAssertStrEquals(tc, "close", result);

    VmRESTFreeHTTPRequestPacket(&pRequest);
}

void Test_VmRESTSetHttpHeaderTest1(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    PREST_RESPONSE                   pResponse = NULL;
    char*                            result = NULL;

    dwError = VmRESTAllocateHTTPResponsePacket(&pResponse);
    CuAssertTrue(tc, !dwError);

    /**** TEST 1: Valid case ****/
    dwError = VmRESTSetHttpHeader(
                  &pResponse,
                  "Connection",
                  "close");
    CuAssertTrue(tc, !dwError);

    dwError = VmRESTGetHttpResponseHeader(
                  pResponse,
                  "Connection", 
                  &result);
    CuAssertTrue(tc, !dwError);
    CuAssertStrEquals(tc, "close", result);

    VmRESTFreeHTTPResponsePacket(&pResponse);
}

void Test_VmRESTSetHttpHeaderTest2(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    PREST_RESPONSE                   pResponse = NULL;

    dwError = VmRESTAllocateHTTPResponsePacket(&pResponse);
    CuAssertTrue(tc, !dwError);

    /**** TEST 2: Empty Header ****/
    dwError = VmRESTSetHttpHeader(
                  &pResponse,
                  "User-Agent",
                  "");
    CuAssertTrue(tc, dwError);
    VmRESTFreeHTTPResponsePacket(&pResponse);
}

void Test_VmRESTSetHttpHeaderTest3(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    PREST_RESPONSE                   pResponse = NULL;

    dwError = VmRESTAllocateHTTPResponsePacket(&pResponse);
    CuAssertTrue(tc, !dwError);

    /**** TEST 3: Empty Value ****/
    dwError = VmRESTSetHttpHeader(
                  &pResponse,
                  "",
                  "close");
    CuAssertTrue(tc, dwError);
    VmRESTFreeHTTPResponsePacket(&pResponse);
}

void Test_VmRESTSetHttpHeaderTest4(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    PREST_RESPONSE                   pResponse = NULL;

    dwError = VmRESTAllocateHTTPResponsePacket(&pResponse);
    CuAssertTrue(tc, !dwError);

    /**** TEST 4: Both Empty ****/
    dwError = VmRESTSetHttpHeader(
                  &pResponse,
                  "",
                  "");
    CuAssertTrue(tc, dwError);
    VmRESTFreeHTTPResponsePacket(&pResponse);
}

void Test_VmRESTSetHttpHeaderTest5(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    PREST_RESPONSE                   pResponse = NULL;

    dwError = VmRESTAllocateHTTPResponsePacket(&pResponse);
    CuAssertTrue(tc, !dwError);

    /**** TEST 5: Check with NULL ****/
    dwError = VmRESTSetHttpHeader(
                  &pResponse,
                  NULL,
                  NULL);
    CuAssertTrue(tc, dwError);
    VmRESTFreeHTTPResponsePacket(&pResponse);
}

void Test_VmRESTSetHttpHeaderTest6(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    PREST_RESPONSE                   pResponse = NULL;

    dwError = VmRESTAllocateHTTPResponsePacket(&pResponse);
    CuAssertTrue(tc, !dwError);

    /**** TEST 6: too large header length ****/
    dwError = VmRESTSetHttpHeader(
                  &pResponse,
                  "Connectionddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd",
                  "close");
    CuAssertTrue(tc, dwError);

    VmRESTFreeHTTPResponsePacket(&pResponse);
}

void Test_VmRESTSetHttpHeaderTest7(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    PREST_RESPONSE                   pResponse = NULL;
    char                             value[10000] = {0};
   
    dwError = VmRESTAllocateHTTPResponsePacket(&pResponse);
    CuAssertTrue(tc, !dwError);
    
    memset(value, '\0', 10000);
    memset(value,'A',9999);

    /**** TEST 7: too large header value length ****/
    dwError = VmRESTSetHttpHeader(
                  &pResponse,
                  "Connection",
                  value);
    CuAssertTrue(tc, dwError);

    VmRESTFreeHTTPResponsePacket(&pResponse);
}

void Test_VmRESTSetHttpStatusCodeTest1(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    PREST_RESPONSE                   pResponse = NULL;

    dwError = VmRESTAllocateHTTPResponsePacket(&pResponse);
    CuAssertTrue(tc, !dwError);

    /**** TEST 1: Valid case ****/
    dwError = VmRESTSetHttpStatusCode(
                  &pResponse,
                  "200");
    CuAssertTrue(tc, !dwError);
    CuAssertStrEquals(tc, "200", pResponse->statusLine->statusCode);

    VmRESTFreeHTTPResponsePacket(&pResponse);
}

void Test_VmRESTSetHttpStatusCodeTest2(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    PREST_RESPONSE                   pResponse = NULL;

    dwError = VmRESTAllocateHTTPResponsePacket(&pResponse);
    CuAssertTrue(tc, !dwError);

    /**** TEST 2: Empty Status code****/
    dwError = VmRESTSetHttpStatusCode(
                  &pResponse,
                  "");
    CuAssertTrue(tc, dwError);

    VmRESTFreeHTTPResponsePacket(&pResponse);
}

void Test_VmRESTSetHttpStatusCodeTest3(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    PREST_RESPONSE                   pResponse = NULL;

    dwError = VmRESTAllocateHTTPResponsePacket(&pResponse);
    CuAssertTrue(tc, !dwError);

    /**** TEST 3: Invalid status code ****/
    dwError = VmRESTSetHttpStatusCode(
                  &pResponse,
                  "700");
    CuAssertTrue(tc, dwError);

    VmRESTFreeHTTPResponsePacket(&pResponse);
}

void Test_VmRESTSetHttpStatusCodeTest4(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    PREST_RESPONSE                   pResponse = NULL;

    dwError = VmRESTAllocateHTTPResponsePacket(&pResponse);
    CuAssertTrue(tc, !dwError);
    
    /**** TEST 4: Large Status code ****/
    dwError = VmRESTSetHttpStatusCode(
                  &pResponse,
                  "7000000000000000000000000000000000000000000000000000000000000000000000000");
    CuAssertTrue(tc, dwError);

    VmRESTFreeHTTPResponsePacket(&pResponse);
}

void Test_VmRESTSetHttpStatusCodeTest5(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    PREST_RESPONSE                   pResponse = NULL;

    dwError = VmRESTAllocateHTTPResponsePacket(&pResponse);
    CuAssertTrue(tc, !dwError);
    
    /**** TEST 5: NULL status code ****/
    dwError = VmRESTSetHttpStatusCode(
                  &pResponse,
                  NULL);
    CuAssertTrue(tc, dwError);

    VmRESTFreeHTTPResponsePacket(&pResponse);
}

void Test_VmRESTSetHttpReasonPhraseTest1(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    PREST_RESPONSE                   pResponse = NULL;

    dwError = VmRESTAllocateHTTPResponsePacket(&pResponse);
    CuAssertTrue(tc, !dwError);

    /**** TEST 1: Valid case ****/
    dwError = VmRESTSetHttpReasonPhrase(
                  &pResponse,
                  "OK");
    CuAssertTrue(tc, !dwError);
    CuAssertStrEquals(tc, "OK", pResponse->statusLine->reason_phrase);

    VmRESTFreeHTTPResponsePacket(&pResponse);
}

void Test_VmRESTSetHttpReasonPhraseTest2(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    PREST_RESPONSE                   pResponse = NULL;
    char                             phrase[10000] = {0};

    dwError = VmRESTAllocateHTTPResponsePacket(&pResponse);
    CuAssertTrue(tc, !dwError);

    memset(phrase,'B', 10000);
    /**** TEST 2: Not null terminated string ****/
    dwError = VmRESTSetHttpReasonPhrase(
                  &pResponse,
                  phrase);
    CuAssertTrue(tc, dwError);

    VmRESTFreeHTTPResponsePacket(&pResponse);
}

void Test_VmRESTSetHttpReasonPhraseTest3(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    PREST_RESPONSE                   pResponse = NULL;

    dwError = VmRESTAllocateHTTPResponsePacket(&pResponse);
    CuAssertTrue(tc, !dwError);

    /**** TEST 3: Large Reason Phrase ****/
    dwError = VmRESTSetHttpReasonPhrase(
                  &pResponse,
                  "This reason phrase length is way bigger than 32 bytes which is allocated for this.......blahhhhhhhhhhhhh");
    CuAssertTrue(tc, dwError);

    VmRESTFreeHTTPResponsePacket(&pResponse);
}

void Test_VmRESTSetHttpReasonPhraseTest4(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    PREST_RESPONSE                   pResponse = NULL;

    dwError = VmRESTAllocateHTTPResponsePacket(&pResponse);
    CuAssertTrue(tc, !dwError);

    /**** TEST 5: NULL Reason Phrase ****/
    dwError = VmRESTSetHttpReasonPhrase(
                  &pResponse,
                  NULL);
    CuAssertTrue(tc, dwError);

    VmRESTFreeHTTPResponsePacket(&pResponse);
}


/*********************/

void Test_VmRESTSetHttpStatusVersionTest1(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    PREST_RESPONSE                   pResponse = NULL;

    dwError = VmRESTAllocateHTTPResponsePacket(&pResponse);
    CuAssertTrue(tc, !dwError);

    /**** TEST 1: Valid case ****/
    dwError = VmRESTSetHttpStatusVersion(
                  &pResponse,
                  "HTTP/1.1");
    CuAssertTrue(tc, !dwError);
    CuAssertStrEquals(tc, "HTTP/1.1", pResponse->statusLine->version);

    VmRESTFreeHTTPResponsePacket(&pResponse);
}

void Test_VmRESTSetHttpStatusVersionTest2(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    PREST_RESPONSE                   pResponse = NULL;

    dwError = VmRESTAllocateHTTPResponsePacket(&pResponse);
    CuAssertTrue(tc, !dwError);

    /**** TEST 2: Empty version ****/
    dwError = VmRESTSetHttpStatusVersion(
                  &pResponse,
                  "");
    CuAssertTrue(tc, dwError);

    VmRESTFreeHTTPResponsePacket(&pResponse);
}

void Test_VmRESTSetHttpStatusVersionTest3(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    PREST_RESPONSE                   pResponse = NULL;

    dwError = VmRESTAllocateHTTPResponsePacket(&pResponse);
    CuAssertTrue(tc, !dwError);

    /**** TEST 3: Invalid version ****/
    dwError = VmRESTSetHttpStatusVersion(
                  &pResponse,
                  "HTTP/0.9");
    CuAssertTrue(tc, dwError);

    VmRESTFreeHTTPResponsePacket(&pResponse);
}

void Test_VmRESTSetHttpStatusVersionTest4(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    PREST_RESPONSE                   pResponse = NULL;

    dwError = VmRESTAllocateHTTPResponsePacket(&pResponse);
    CuAssertTrue(tc, !dwError);

    /**** TEST 4: Large version ****/
    dwError = VmRESTSetHttpStatusVersion(
                  &pResponse,
                  "JUNKVERSION>>>>>>>>>>>>>>>MMMMMMMMM<<<<<<<<<<<<<00000000000000000000000000000000000000000000000000000");
    CuAssertTrue(tc, dwError);

    VmRESTFreeHTTPResponsePacket(&pResponse);
}

void Test_VmRESTSetHttpStatusVersionTest5(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    PREST_RESPONSE                   pResponse = NULL;

    dwError = VmRESTAllocateHTTPResponsePacket(&pResponse);
    CuAssertTrue(tc, !dwError);

    /**** TEST 5: NULL version information ****/
    dwError = VmRESTSetHttpStatusVersion(
                  &pResponse,
                  NULL);
    CuAssertTrue(tc, dwError);

    VmRESTFreeHTTPResponsePacket(&pResponse);
}






CuSuite* CuGetTestHttpUtilsExternalSuite(void)
{
    CuSuite* suite = CuSuiteNew();

    SUITE_ADD_TEST(suite, Test_VmRESTGetHttpMethodTest1);
    SUITE_ADD_TEST(suite, Test_VmRESTGetHttpMethodTest2);
    SUITE_ADD_TEST(suite, Test_VmRESTGetHttpMethodTest3);
    SUITE_ADD_TEST(suite, Test_VmRESTGetHttpURITest1);
    SUITE_ADD_TEST(suite, Test_VmRESTGetHttpURITest2);
    SUITE_ADD_TEST(suite, Test_VmRESTGetHttpVersionTest1);
    SUITE_ADD_TEST(suite, Test_VmRESTGetHttpVersionTest2);
    SUITE_ADD_TEST(suite, Test_VmRESTGetHttpVersionTest3);
    SUITE_ADD_TEST(suite, Test_VmRESTGetHttpHeaderTest1);
    SUITE_ADD_TEST(suite, Test_VmRESTGetHttpHeaderTest2);
    SUITE_ADD_TEST(suite, Test_VmRESTGetHttpHeaderTest3);
    SUITE_ADD_TEST(suite, Test_VmRESTSetHttpHeaderTest1);
    SUITE_ADD_TEST(suite, Test_VmRESTSetHttpHeaderTest2);
    SUITE_ADD_TEST(suite, Test_VmRESTSetHttpHeaderTest3);
    SUITE_ADD_TEST(suite, Test_VmRESTSetHttpHeaderTest4);
    SUITE_ADD_TEST(suite, Test_VmRESTSetHttpHeaderTest5);
    SUITE_ADD_TEST(suite, Test_VmRESTSetHttpHeaderTest6);
    SUITE_ADD_TEST(suite, Test_VmRESTSetHttpHeaderTest7);
    SUITE_ADD_TEST(suite, Test_VmRESTSetHttpStatusCodeTest1);
    SUITE_ADD_TEST(suite, Test_VmRESTSetHttpStatusCodeTest2);
    SUITE_ADD_TEST(suite, Test_VmRESTSetHttpStatusCodeTest3);
    SUITE_ADD_TEST(suite, Test_VmRESTSetHttpStatusCodeTest4);
    SUITE_ADD_TEST(suite, Test_VmRESTSetHttpStatusCodeTest5);
    SUITE_ADD_TEST(suite, Test_VmRESTSetHttpReasonPhraseTest1);
    SUITE_ADD_TEST(suite, Test_VmRESTSetHttpReasonPhraseTest2);
    SUITE_ADD_TEST(suite, Test_VmRESTSetHttpReasonPhraseTest3);
    SUITE_ADD_TEST(suite, Test_VmRESTSetHttpReasonPhraseTest4);
    SUITE_ADD_TEST(suite, Test_VmRESTSetHttpStatusVersionTest1);
    SUITE_ADD_TEST(suite, Test_VmRESTSetHttpStatusVersionTest2);
    SUITE_ADD_TEST(suite, Test_VmRESTSetHttpStatusVersionTest3);
    SUITE_ADD_TEST(suite, Test_VmRESTSetHttpStatusVersionTest4);
    SUITE_ADD_TEST(suite, Test_VmRESTSetHttpStatusVersionTest5);

    return suite;
}

