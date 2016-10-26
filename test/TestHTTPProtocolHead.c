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

void Test_VmRESTHTTPGetReqMethodTest1(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             line[MAX_REQ_LIN_LEN] = {0};
    char                             methodName[10] = {0};
    uint32_t                         resStatus = 200;

    memset(line, '\0', MAX_REQ_LIN_LEN);
    memset(methodName, '\0', 10);

    /**** TEST 1: Valid HTTP syntax in proper format ****/
    strcpy(line, "GET /v1/pkg?x=y HTTP/1.1");

    dwError = VmRESTHTTPGetReqMethod(
                  line,
                  strlen(line),
                  methodName,
                  &resStatus
                  );
    CuAssertIntEquals(tc, 0, dwError);
    CuAssertStrEquals(tc, "GET", methodName);
    CuAssertIntEquals(tc, 200, resStatus);
}

void Test_VmRESTHTTPGetReqMethodTest2(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             line[MAX_REQ_LIN_LEN] = {0};
    char                             methodName[10] = {0};
    uint32_t                         resStatus = 200;

    memset(line, '\0', MAX_REQ_LIN_LEN);
    memset(methodName, '\0', 10);

    /**** TEST 2: Line starts with space ****/
    strcpy(line, " GET /v1/pkg?x=y HTTP/1.1");

    dwError = VmRESTHTTPGetReqMethod(
                  line,
                  strlen(line),
                  methodName,
                  &resStatus
                  );
    CuAssertTrue(tc, dwError);
}

void Test_VmRESTHTTPGetReqMethodTest3(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             line[MAX_REQ_LIN_LEN] = {0};
    char                             methodName[10] = {0};
    uint32_t                         resStatus = 200;

    memset(line, '\0', MAX_REQ_LIN_LEN);
    memset(methodName, '\0', 10);

    /**** TEST 3:Extra Space after method name ****/
    strcpy(line, "POST  /v1/pkg?x=y HTTP/1.1");

    dwError = VmRESTHTTPGetReqMethod(
                  line,
                  strlen(line),
                  methodName,
                  &resStatus
                  );
    CuAssertIntEquals(tc, 0, dwError);
    CuAssertStrEquals(tc, "POST", methodName);
    CuAssertIntEquals(tc, 200, resStatus);
}

void Test_VmRESTHTTPGetReqMethodTest4(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             line[MAX_REQ_LIN_LEN] = {0};
    char                             methodName[10] = {0};
    uint32_t                         resStatus = 200;

    memset(line, '\0', MAX_REQ_LIN_LEN);
    memset(methodName, '\0', 10);

    /**** TEST 4:No method name ****/
    strcpy(line, "/v1/pkg?x=y HTTP/1.1");

    dwError = VmRESTHTTPGetReqMethod(
                  line,
                  strlen(line),
                  methodName,
                  &resStatus
                  );
    CuAssertTrue(tc, dwError);
}

void Test_VmRESTHTTPGetReqURITest1(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             line[MAX_REQ_LIN_LEN] = {0};
    char                             uri[MAX_REQ_LIN_LEN] = {0};
    uint32_t                         resStatus = 200;

    memset(line, '\0', MAX_REQ_LIN_LEN);
    memset(uri, '\0', MAX_REQ_LIN_LEN);

    /**** TEST 1: Valid test ****/
    strcpy(line, "POST /v1/pkg?x=y HTTP/1.1");

    dwError = VmRESTHTTPGetReqURI(
                  line,
                  strlen(line),
                  uri,
                  &resStatus
                  );
    CuAssertIntEquals(tc, 0, dwError);
    CuAssertStrEquals(tc, "/v1/pkg?x=y", uri);
    CuAssertIntEquals(tc, 200, resStatus);
    
}

void Test_VmRESTHTTPGetReqURITest2(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             line[MAX_REQ_LIN_LEN] = {0};
    char                             uri[MAX_REQ_LIN_LEN] = {0};
    uint32_t                         resStatus = 200;

    memset(line, '\0', MAX_REQ_LIN_LEN);
    memset(uri, '\0', MAX_REQ_LIN_LEN);

    /**** TEST 2: No URI present ****/
    strcpy(line, "POST   HTTP/1.1");

    dwError = VmRESTHTTPGetReqURI(
                  line,
                  strlen(line),
                  uri,
                  &resStatus
                  );
    CuAssertTrue(tc, dwError);
}

void Test_VmRESTHTTPGetReqURITest3(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             line[MAX_REQ_LIN_LEN] = {0};
    char                             uri[MAX_REQ_LIN_LEN] = {0};
    uint32_t                         resStatus = 200;

    memset(line, '\0', MAX_REQ_LIN_LEN);
    memset(uri, '\0', MAX_REQ_LIN_LEN);

    /**** TEST 3: just the / uri ****/
    strcpy(line, "POST / HTTP/1.1");

    dwError = VmRESTHTTPGetReqURI(
                  line,
                  strlen(line),
                  uri,
                  &resStatus
                  );
    CuAssertIntEquals(tc, 0, dwError);
    CuAssertStrEquals(tc, "/", uri);
    CuAssertIntEquals(tc, 200, resStatus);

}

void Test_VmRESTHTTPGetReqURITest4(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             line[MAX_REQ_LIN_LEN] = {0};
    char                             uri[MAX_REQ_LIN_LEN] = {0};
    uint32_t                         resStatus = 200;

    memset(line, '\0', MAX_REQ_LIN_LEN);
    memset(uri, '\0', MAX_REQ_LIN_LEN);

    /**** TEST 4: Single string line ****/
    strcpy(line, "SINGLESTRING");

    dwError = VmRESTHTTPGetReqURI(
                  line,
                  strlen(line),
                  uri,
                  &resStatus
                  );
    CuAssertTrue(tc, dwError);
}

void Test_VmRESTHTTPGetReqVersionTest1(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             line[MAX_REQ_LIN_LEN] = {0};
    char                             version[10] = {0};
    uint32_t                         resStatus = 200;

    memset(line, '\0', MAX_REQ_LIN_LEN);
    memset(version, '\0', 10);

    /**** TEST 1: valid case ****/
    strcpy(line, "POST /v1/pkg?x=y HTTP/1.1");

    dwError = VmRESTHTTPGetReqURI(
                  line,
                  strlen(line),
                  version,
                  &resStatus
                  );
    CuAssertIntEquals(tc, 0, dwError);
    CuAssertStrEquals(tc, "HTTP/1.1", version);
    CuAssertIntEquals(tc, 200, resStatus);
}

void Test_VmRESTHTTPGetReqVersionTest2(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             line[MAX_REQ_LIN_LEN] = {0};
    char                             version[10] = {0};
    uint32_t                         resStatus = 200;

    memset(line, '\0', MAX_REQ_LIN_LEN);
    memset(version, '\0', 10);

    /**** TEST 2: Wrong version ****/
    strcpy(line, "POST /v1/pkg?x=y HTTP/0.9");

    dwError = VmRESTHTTPGetReqURI(
                  line,
                  strlen(line),
                  version,
                  &resStatus
                  );
    CuAssertTrue(tc, dwError);
}

void Test_VmRESTHTTPGetReqVersionTest3(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             line[MAX_REQ_LIN_LEN] = {0};
    char                             version[10] = {0};
    uint32_t                         resStatus = 200;

    memset(line, '\0', MAX_REQ_LIN_LEN);
    memset(version, '\0', 10);

    /**** TEST 3: No Version ****/
    strcpy(line, "POST /v1/pkg?x=y");

    dwError = VmRESTHTTPGetReqURI(
                  line,
                  strlen(line),
                  version,
                  &resStatus
                  );
    CuAssertTrue(tc, dwError);
}

void Test_VmRESTHTTPGetReqVersionTest4(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             line[MAX_REQ_LIN_LEN] = {0};
    char                             version[10] = {0};
    uint32_t                         resStatus = 200;

    memset(line, '\0', MAX_REQ_LIN_LEN);
    memset(version, '\0', 10);

    /**** TEST 4: empty sring ****/
    strcpy(line, "POST /v1/pkg?x=y ");

    dwError = VmRESTHTTPGetReqURI(
                  line,
                  strlen(line),
                  version,
                  &resStatus
                  );
    CuAssertTrue(tc, dwError);
}

void Test_VmRESTHTTPPopulateHeaderTest1(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             line[MAX_REQ_LIN_LEN] = {0};
    PREST_REQUEST                    pRequest = NULL;
    uint32_t                         resStatus = 200;
    char*                            value = NULL;

    dwError = VmRESTAllocateHTTPRequestPacket(&pRequest);
    CuAssertTrue(tc, !dwError); 

    memset(line, '\0', MAX_REQ_LIN_LEN);

    /**** TEST 1: Valid header ****/
    strcpy(line, "User-Agent:curl/7.35.0");

    dwError = VmRESTHTTPPopulateHeader(
                  line,
                  strlen(line),
                  pRequest,
                  &resStatus
                  );
    CuAssertTrue(tc, !dwError);

    dwError = VmRESTGetHttpHeader(
                  pRequest,
                  "User-Agent",
                  &value);
    CuAssertTrue(tc, !dwError);
    CuAssertStrEquals(tc, "curl/7.35.0", value);
    
    VmRESTFreeHTTPRequestPacket(&pRequest);
}

void Test_VmRESTHTTPPopulateHeaderTest2(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             line[MAX_REQ_LIN_LEN] = {0};
    PREST_REQUEST                    pRequest = NULL;
    uint32_t                         resStatus = 200;
    char*                            value = NULL;

    dwError = VmRESTAllocateHTTPRequestPacket(&pRequest);
    CuAssertTrue(tc, !dwError);

    memset(line, '\0', MAX_REQ_LIN_LEN);

    /**** TEST 2: host name with port number ****/
    strcpy(line, "Host: 172.16.127.132:81");

    dwError = VmRESTHTTPPopulateHeader(
                  line,
                  strlen(line),
                  pRequest,
                  &resStatus
                  );
    CuAssertTrue(tc, !dwError);

    dwError = VmRESTGetHttpHeader(
                  pRequest,
                  "Host",
                  &value);
    CuAssertTrue(tc, !dwError);
    CuAssertStrEquals(tc, "172.16.127.132:81", value);

    VmRESTFreeHTTPRequestPacket(&pRequest);
}

void Test_VmRESTHTTPPopulateHeaderTest3(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             line[MAX_REQ_LIN_LEN] = {0};
    PREST_REQUEST                    pRequest = NULL;
    uint32_t                         resStatus = 200;

    dwError = VmRESTAllocateHTTPRequestPacket(&pRequest);
    CuAssertTrue(tc, !dwError);

    memset(line, '\0', MAX_REQ_LIN_LEN);

    /**** TEST 3: Header Value Missing ****/
    strcpy(line, "User-Agent:");

    dwError = VmRESTHTTPPopulateHeader(
                  line,
                  strlen(line),
                  pRequest,
                  &resStatus
                  );
    CuAssertTrue(tc, dwError);
    VmRESTFreeHTTPRequestPacket(&pRequest);
}

void Test_VmRESTHTTPPopulateHeaderTest4(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             line[MAX_REQ_LIN_LEN] = {0};
    PREST_REQUEST                    pRequest = NULL;
    uint32_t                         resStatus = 200;

    dwError = VmRESTAllocateHTTPRequestPacket(&pRequest);
    CuAssertTrue(tc, !dwError);

    memset(line, '\0', MAX_REQ_LIN_LEN);

    /**** TEST 3: Both Header and value missing ****/
    strcpy(line, " : ");

    dwError = VmRESTHTTPPopulateHeader(
                  line,
                  strlen(line),
                  pRequest,
                  &resStatus
                  );
    CuAssertTrue(tc, dwError);
    VmRESTFreeHTTPRequestPacket(&pRequest);
}

void Test_VmRESTHTTPPopulateHeaderTest5(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             line[MAX_REQ_LIN_LEN] = {0};
    PREST_REQUEST                    pRequest = NULL;
    uint32_t                         resStatus = 200;
    char*                            value = NULL;

    dwError = VmRESTAllocateHTTPRequestPacket(&pRequest);
    CuAssertTrue(tc, !dwError);

    memset(line, '\0', MAX_REQ_LIN_LEN);

    /**** TEST 5: Space after header and befor value ****/
    strcpy(line, "Connection : close");

    dwError = VmRESTHTTPPopulateHeader(
                  line,
                  strlen(line),
                  pRequest,
                  &resStatus
                  );
    CuAssertTrue(tc, !dwError);

    dwError = VmRESTGetHttpHeader(
                  pRequest,
                  "Connection",
                  &value);
    CuAssertTrue(tc, !dwError);
    CuAssertStrEquals(tc, "close", value);

    VmRESTFreeHTTPRequestPacket(&pRequest);
}

void Test_VmRESTHTTPPopulateHeaderTest6(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             line[MAX_REQ_LIN_LEN] = {0};
    PREST_REQUEST                    pRequest = NULL;
    uint32_t                         resStatus = 200;

    dwError = VmRESTAllocateHTTPRequestPacket(&pRequest);
    CuAssertTrue(tc, !dwError);

    memset(line, '\0', MAX_REQ_LIN_LEN);

    /**** TEST 6: Empty Line ****/
    strcpy(line, "    ");

    dwError = VmRESTHTTPPopulateHeader(
                  line,
                  strlen(line),
                  pRequest,
                  &resStatus
                  );
    CuAssertTrue(tc, dwError);
    VmRESTFreeHTTPRequestPacket(&pRequest);
}

void Test_VmRESTHTTPPopulateHeaderTest7(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             line[MAX_REQ_LIN_LEN] = {0};
    PREST_REQUEST                    pRequest = NULL;
    uint32_t                         resStatus = 200;

    dwError = VmRESTAllocateHTTPRequestPacket(&pRequest);
    CuAssertTrue(tc, !dwError);
    
    memset(line, '\0', MAX_REQ_LIN_LEN);

    /**** TEST 7: Header missing but value present ****/
    strcpy(line, " :keep-alive");

    dwError = VmRESTHTTPPopulateHeader(
                  line,
                  strlen(line),
                  pRequest,
                  &resStatus
                  );
    CuAssertTrue(tc, dwError);
    VmRESTFreeHTTPRequestPacket(&pRequest);
}

void Test_VmRESTParseHTTPReqLineTest1(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             line[MAX_REQ_LIN_LEN] = {0};
    PREST_REQUEST                    pRequest = NULL;
    uint32_t                         resStatus = 200;

    dwError = VmRESTAllocateHTTPRequestPacket(&pRequest);
    CuAssertTrue(tc, !dwError);

    memset(line, '\0', MAX_REQ_LIN_LEN);

    /**** TEST 1: Valid case ****/
    strcpy(line, "GET /v1/pkg?x=y HTTP/1.1");

    dwError = VmRESTParseHTTPReqLine(
                  1,
                  line,
                  strlen(line),
                  pRequest,
                  &resStatus
                  );
    CuAssertTrue(tc, !dwError);
    CuAssertStrEquals(tc, "GET", pRequest->requestLine->method);
    CuAssertStrEquals(tc, "/v1/pkg?x=y", pRequest->requestLine->uri);
    CuAssertStrEquals(tc, "HTTP/1.1", pRequest->requestLine->version);

    VmRESTFreeHTTPRequestPacket(&pRequest);
}

void Test_VmRESTParseHTTPReqLineTest2(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             line[MAX_REQ_LIN_LEN] = {0};
    PREST_REQUEST                    pRequest = NULL;
    uint32_t                         resStatus = 200;

    dwError = VmRESTAllocateHTTPRequestPacket(&pRequest);
    CuAssertTrue(tc, !dwError);

    memset(line, '\0', MAX_REQ_LIN_LEN);

    /**** TEST 2: With wrong line number ****/
    strcpy(line, "GET /v1/pkg?x=y HTTP/1.1");

    dwError = VmRESTParseHTTPReqLine(
                  2,
                  line,
                  strlen(line),
                  pRequest,
                  &resStatus
                  );
    CuAssertTrue(tc, dwError);
    VmRESTFreeHTTPRequestPacket(&pRequest);
}

void Test_VmRESTParseHTTPReqLineTest3(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             line[MAX_REQ_LIN_LEN] = {0};
    PREST_REQUEST                    pRequest = NULL;
    uint32_t                         resStatus = 200;
    char*                            value = NULL;

    dwError = VmRESTAllocateHTTPRequestPacket(&pRequest);
    CuAssertTrue(tc, !dwError);

    memset(line, '\0', MAX_REQ_LIN_LEN);

    /**** TEST 3: Valid case ****/
    strcpy(line, "Connection:close");

    dwError = VmRESTParseHTTPReqLine(
                  2,
                  line,
                  strlen(line),
                  pRequest,
                  &resStatus
                  );
    CuAssertTrue(tc, !dwError);

    memset(line, '\0', MAX_REQ_LIN_LEN);
    strcpy(line, "Content-Length:43");

    dwError = VmRESTParseHTTPReqLine(
                  2,
                  line,
                  strlen(line),
                  pRequest,
                  &resStatus
                  );
    CuAssertTrue(tc, !dwError);

    dwError = VmRESTGetHttpHeader(
                  pRequest,
                  "Connection",
                  &value);
    CuAssertTrue(tc, !dwError);
    CuAssertStrEquals(tc, "close", value);

    dwError = VmRESTGetHttpHeader(
                  pRequest,
                  "Content-Length",
                  &value);
    CuAssertTrue(tc, !dwError);
    CuAssertStrEquals(tc, "43", value);

    VmRESTFreeHTTPRequestPacket(&pRequest);
}

void Test_VmRESTParseHTTPReqLineTest4(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             line[MAX_REQ_LIN_LEN] = {0};
    PREST_REQUEST                    pRequest = NULL;
    uint32_t                         resStatus = 200;

    dwError = VmRESTAllocateHTTPRequestPacket(&pRequest);
    CuAssertTrue(tc, !dwError);

    memset(line, '\0', MAX_REQ_LIN_LEN);

    /**** TEST 4: With wrong line number ****/
    strcpy(line, "Content-Length:43");

    dwError = VmRESTParseHTTPReqLine(
                  1,
                  line,
                  strlen(line),
                  pRequest,
                  &resStatus
                  );
    CuAssertTrue(tc, dwError);
    VmRESTFreeHTTPRequestPacket(&pRequest);
}

void Test_VmRESTParseHTTPReqLineTest5(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             line[MAX_REQ_LIN_LEN] = {0};
    PREST_REQUEST                    pRequest = NULL;
    uint32_t                         resStatus = 200;

    dwError = VmRESTAllocateHTTPRequestPacket(&pRequest);
    CuAssertTrue(tc, !dwError);
    
    memset(line, '\0', MAX_REQ_LIN_LEN);

    /**** TEST 5: With 0 line number ****/
    strcpy(line, "Content-Length:43");

    dwError = VmRESTParseHTTPReqLine(
                  0,
                  line,
                  strlen(line),
                  pRequest,
                  &resStatus
                  );
    CuAssertTrue(tc, dwError);
    VmRESTFreeHTTPRequestPacket(&pRequest);
}

#if 0

/*****************************************************************
* This test cannot be added here as the function tested
* here is dependent on tranport API's. Keeping it for 
* integration testing.
*****************************************************************/

void Test_VmRESTParseAndPopulateHTTPHeadersTest1(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             buffer[MAX_DATA_BUFFER_LEN] = {0};
    PREST_REQUEST                    pRequest = NULL;
    uint32_t                         resStatus = 200;
    char*                            value = NULL;

    dwError = VmRESTAllocateHTTPRequestPacket(&pRequest);
    CuAssertTrue(tc, !dwError);

    memset(buffer, '\0', MAX_DATA_BUFFER_LEN);

    /**** TEST 1: Valid case ****/
    strcpy(buffer, "GET http://SITE/foobar.html HTTP/1.1\r\nHost: SITE\r\nConnection: Keep-Alive\r\nTransfer-Encoding:chunked\r\n\r\n6\r\nThis i\r\n9\r\ns payload\r\n0\r\n");

    dwError = VmRESTParseAndPopulateHTTPHeaders(
                  buffer,
                  strlen(buffer),
                  pRequest,
                  &resStatus
                  );
    CuAssertTrue(tc, !dwError);
    CuAssertStrEquals(tc, "GET", pRequest->requestLine->method);
    CuAssertStrEquals(tc, "http://SITE/foobar.html", pRequest->requestLine->uri);
    CuAssertStrEquals(tc, "HTTP/1.1", pRequest->requestLine->version);

    dwError = VmRESTGetHttpHeader(
                  pRequest,
                  "Host",
                  &value);
    CuAssertTrue(tc, !dwError);
    CuAssertStrEquals(tc, "SITE", value);

    dwError = VmRESTGetHttpHeader(
                  pRequest,
                  "Connection",
                  &value);
    CuAssertTrue(tc, !dwError);
    CuAssertStrEquals(tc, "Keep-Alive", value);

    dwError = VmRESTGetHttpHeader(
                  pRequest,
                  "Transfer-Encoding",
                  &value);
    CuAssertTrue(tc, !dwError);
    CuAssertStrEquals(tc, "chunked", value);


    VmRESTFreeHTTPRequestPacket(&pRequest);
}

#endif


CuSuite* CuGetTestHTTPProtocolHeadSuite(void)
{
    CuSuite* suite = CuSuiteNew();

    SUITE_ADD_TEST(suite, Test_VmRESTHTTPGetReqMethodTest1);
    SUITE_ADD_TEST(suite, Test_VmRESTHTTPGetReqMethodTest2);
    SUITE_ADD_TEST(suite, Test_VmRESTHTTPGetReqMethodTest3);
    SUITE_ADD_TEST(suite, Test_VmRESTHTTPGetReqMethodTest4);
    SUITE_ADD_TEST(suite, Test_VmRESTHTTPGetReqURITest1);
    SUITE_ADD_TEST(suite, Test_VmRESTHTTPGetReqURITest2);
    SUITE_ADD_TEST(suite, Test_VmRESTHTTPGetReqURITest3);
    SUITE_ADD_TEST(suite, Test_VmRESTHTTPGetReqURITest4);
    SUITE_ADD_TEST(suite, Test_VmRESTHTTPGetReqVersionTest1);
    SUITE_ADD_TEST(suite, Test_VmRESTHTTPGetReqVersionTest2);
    SUITE_ADD_TEST(suite, Test_VmRESTHTTPGetReqVersionTest3);
    SUITE_ADD_TEST(suite, Test_VmRESTHTTPGetReqVersionTest4);
    SUITE_ADD_TEST(suite, Test_VmRESTHTTPPopulateHeaderTest1);
    SUITE_ADD_TEST(suite, Test_VmRESTHTTPPopulateHeaderTest2);
    SUITE_ADD_TEST(suite, Test_VmRESTHTTPPopulateHeaderTest3);
    SUITE_ADD_TEST(suite, Test_VmRESTHTTPPopulateHeaderTest4);
    SUITE_ADD_TEST(suite, Test_VmRESTHTTPPopulateHeaderTest5);
    SUITE_ADD_TEST(suite, Test_VmRESTHTTPPopulateHeaderTest6);
    SUITE_ADD_TEST(suite, Test_VmRESTHTTPPopulateHeaderTest7);
    SUITE_ADD_TEST(suite, Test_VmRESTParseHTTPReqLineTest1);
    SUITE_ADD_TEST(suite, Test_VmRESTParseHTTPReqLineTest2);
    SUITE_ADD_TEST(suite, Test_VmRESTParseHTTPReqLineTest3);
    SUITE_ADD_TEST(suite, Test_VmRESTParseHTTPReqLineTest4);
    SUITE_ADD_TEST(suite, Test_VmRESTParseHTTPReqLineTest5);

    return suite;
}


