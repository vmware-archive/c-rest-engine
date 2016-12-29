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

/************************************************
* NOTE: All function using socket layer API calls
* are not tested here. They will be tested during 
* end-to-end tested of module.
************************************************/

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

void Test_VmRESTHTTPGetReqMethodTest5(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             line[MAX_REQ_LIN_LEN] = {0};
    char                             methodName[10] = {0};
    uint32_t                         resStatus = 200;

    memset(line, '\0', MAX_REQ_LIN_LEN);
    memset(methodName, '\0', 10);

    /**** TEST 5:Negative line length ****/
    strcpy(line, "");

    dwError = VmRESTHTTPGetReqMethod(
                  line,
                  (strlen(line) - 1),
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

void Test_VmRESTHTTPGetReqURITest5(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             line[MAX_REQ_LIN_LEN] = {0};
    char                             uri[MAX_REQ_LIN_LEN] = {0};
    uint32_t                         resStatus = 200;

    memset(line, '\0', MAX_REQ_LIN_LEN);
    memset(uri, '\0', MAX_REQ_LIN_LEN);

    /**** TEST 5: Negative line length ****/
    strcpy(line, "");

    dwError = VmRESTHTTPGetReqURI(
                  line,
                  (strlen(line) - 2),
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

void Test_VmRESTHTTPGetReqVersionTest5(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             line[MAX_REQ_LIN_LEN] = {0};
    char                             version[10] = {0};
    uint32_t                         resStatus = 200;

    memset(line, '\0', MAX_REQ_LIN_LEN);
    memset(version, '\0', 10);

    /**** TEST 5: Negative line length ****/
    strcpy(line, "");

    dwError = VmRESTHTTPGetReqURI(
                  line,
                  (strlen(line) - 2),
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

void Test_VmRESTHTTPPopulateHeaderTest8(
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

    /**** TEST 8: Negative line length ****/
    strcpy(line, "");

    dwError = VmRESTHTTPPopulateHeader(
                  line,
                  (strlen(line) - 2),
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

void Test_VmRESTParseHTTPReqLineTest6(
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

    /**** TEST 6: Negative line length****/
    strcpy(line, "");

    dwError = VmRESTParseHTTPReqLine(
                  0,
                  line,
                  (strlen(line) - 2),
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

void Test_VMRESTWriteChunkedMessageInResponseStreamTest1(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             buffer[MAX_DATA_BUFFER_LEN] = {0};
    char                             src[MAX_DATA_BUFFER_LEN] = {0};
    uint32_t                         bytesWritten = 0;

    memset(buffer, '\0', MAX_DATA_BUFFER_LEN);
    memset(src, '\0', MAX_DATA_BUFFER_LEN);
    

    /**** TEST 1: Valid Case ****/
    strcpy(src, "This is one chunked data");

    dwError = VMRESTWriteChunkedMessageInResponseStream(
                  src,
                  strlen(src),
                  buffer,
                  &bytesWritten
                  );
    CuAssertTrue(tc, !dwError);
    CuAssertIntEquals(tc, 30, bytesWritten);
    CuAssertStrEquals(tc, "18\r\nThis is one chunked data\r\n", buffer);
}

void Test_VMRESTWriteChunkedMessageInResponseStreamTest2(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             buffer[MAX_DATA_BUFFER_LEN] = {0};
    char                             src[MAX_DATA_BUFFER_LEN] = {0};
    uint32_t                         bytesWritten = 0;

    memset(buffer, '\0', MAX_DATA_BUFFER_LEN);
    memset(src, '\0', MAX_DATA_BUFFER_LEN);


    /**** TEST 2: Valid Case with 0 size ****/
    strcpy(src, "");

    dwError = VMRESTWriteChunkedMessageInResponseStream(
                  src,
                  strlen(src),
                  buffer,
                  &bytesWritten
                  );
    CuAssertTrue(tc, !dwError);
    CuAssertIntEquals(tc, 5, bytesWritten);
    CuAssertStrEquals(tc, "0\r\n\r\n", buffer);
}

void Test_VMRESTWriteChunkedMessageInResponseStreamTest3(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             buffer[MAX_DATA_BUFFER_LEN] = {0};
    char                             src[MAX_DATA_BUFFER_LEN] = {0};
    uint32_t                         bytesWritten = 0;

    memset(buffer, '\0', MAX_DATA_BUFFER_LEN);
    memset(src, '\0', MAX_DATA_BUFFER_LEN);
    
    
    /**** TEST 3: Wrong length of source data****/
    strcpy(src, "This is one chunked data");
    
    dwError = VMRESTWriteChunkedMessageInResponseStream(
                  src,
                  (strlen(src) + 2),
                  buffer,
                  &bytesWritten
                  );
    CuAssertTrue(tc, dwError);
}

void Test_VMRESTWriteChunkedMessageInResponseStreamTest4(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             buffer[MAX_DATA_BUFFER_LEN] = {0};
    char                             src[MAX_DATA_BUFFER_LEN] = {0};
    uint32_t                         bytesWritten = 0;
    
    memset(buffer, '\0', MAX_DATA_BUFFER_LEN);
    memset(src, '\0', MAX_DATA_BUFFER_LEN);

    
    /**** TEST 4: Wrong length of source data****/
    strcpy(src, "This is one chunked data");
    
    dwError = VMRESTWriteChunkedMessageInResponseStream(
                  src,
                  (strlen(src) - 2),
                  buffer,
                  &bytesWritten
                  );
    CuAssertTrue(tc, dwError);
}

void Test_VMRESTWriteChunkedMessageInResponseStreamTest5(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             buffer[MAX_DATA_BUFFER_LEN] = {0};
    char                             src[MAX_DATA_BUFFER_LEN] = {0};
    uint32_t                         bytesWritten = 0;

    memset(buffer, '\0', MAX_DATA_BUFFER_LEN);
    memset(src, '\0', MAX_DATA_BUFFER_LEN);


    /**** TEST 5: Negative data length ****/
    strcpy(src, "");

    dwError = VMRESTWriteChunkedMessageInResponseStream(
                  src,
                  (strlen(src) - 2),
                  buffer,
                  &bytesWritten
                  );
    CuAssertTrue(tc, dwError);
}

void Test_VMRESTWriteMessageBodyInResponseStreamTest1(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             buffer[MAX_DATA_BUFFER_LEN] = {0};
    uint32_t                         bytesWritten = 0;
    PREST_RESPONSE                   pResponse = NULL;

    memset(buffer, '\0', MAX_DATA_BUFFER_LEN);

    dwError = VmRESTAllocateHTTPResponsePacket(&pResponse);
    CuAssertTrue(tc, !dwError);

    /**** TEST 1: Valid Case ****/
    dwError = VmRESTSetHttpHeader(
                  &pResponse,
                  "Content-Length",
                  "20");
    CuAssertTrue(tc, !dwError);

    strcpy(pResponse->messageBody->buffer, "This is message body");

    dwError = VMRESTWriteMessageBodyInResponseStream(
                  pResponse,
                  buffer,
                  &bytesWritten
                  );
    CuAssertTrue(tc, !dwError);
    CuAssertIntEquals(tc, 20, bytesWritten);
    CuAssertStrEquals(tc, "This is message body", buffer);

    VmRESTFreeHTTPResponsePacket(&pResponse);
}

void Test_VMRESTWriteMessageBodyInResponseStreamTest2(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             buffer[MAX_DATA_BUFFER_LEN] = {0};
    uint32_t                         bytesWritten = 0;
    PREST_RESPONSE                   pResponse = NULL;

    memset(buffer, '\0', MAX_DATA_BUFFER_LEN);

    dwError = VmRESTAllocateHTTPResponsePacket(&pResponse);
    CuAssertTrue(tc, !dwError);

    /**** TEST 2: Valid Case with 0 content length****/
    dwError = VmRESTSetHttpHeader(
                  &pResponse,
                  "Content-Length",
                  "0");
    CuAssertTrue(tc, !dwError);

    strcpy(pResponse->messageBody->buffer, "");

    dwError = VMRESTWriteMessageBodyInResponseStream(
                  pResponse,
                  buffer,
                  &bytesWritten
                  );
    CuAssertTrue(tc, !dwError);
    CuAssertIntEquals(tc, 0, bytesWritten);
    CuAssertStrEquals(tc, "", buffer);

    VmRESTFreeHTTPResponsePacket(&pResponse);
}

void Test_VMRESTWriteMessageBodyInResponseStreamTest3(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             buffer[MAX_DATA_BUFFER_LEN] = {0};
    uint32_t                         bytesWritten = 0;
    PREST_RESPONSE                   pResponse = NULL;

    memset(buffer, '\0', MAX_DATA_BUFFER_LEN);

    dwError = VmRESTAllocateHTTPResponsePacket(&pResponse);
    CuAssertTrue(tc, !dwError);

    /**** TEST 3: Missing content length ****/

    strcpy(pResponse->messageBody->buffer, "");

    dwError = VMRESTWriteMessageBodyInResponseStream(
                  pResponse,
                  buffer,
                  &bytesWritten
                  );
    CuAssertTrue(tc, dwError);

    VmRESTFreeHTTPResponsePacket(&pResponse);
}

void Test_VMRESTWriteMessageBodyInResponseStreamTest4(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             buffer[MAX_DATA_BUFFER_LEN] = {0};
    uint32_t                         bytesWritten = 0;
    PREST_RESPONSE                   pResponse = NULL;

    memset(buffer, '\0', MAX_DATA_BUFFER_LEN);

    dwError = VmRESTAllocateHTTPResponsePacket(&pResponse);
    CuAssertTrue(tc, !dwError);

    /**** TEST 4: Negative content length****/
    dwError = VmRESTSetHttpHeader(
                  &pResponse,
                  "Content-Length",
                  "-2");
    CuAssertTrue(tc, !dwError);

    strcpy(pResponse->messageBody->buffer, "");

    dwError = VMRESTWriteMessageBodyInResponseStream(
                  pResponse,
                  buffer,
                  &bytesWritten
                  );
    CuAssertTrue(tc, dwError);

    VmRESTFreeHTTPResponsePacket(&pResponse);
}

void Test_VMRESTWriteMessageBodyInResponseStreamTest5(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             buffer[MAX_DATA_BUFFER_LEN] = {0};
    uint32_t                         bytesWritten = 0;
    PREST_RESPONSE                   pResponse = NULL;

    memset(buffer, '\0', MAX_DATA_BUFFER_LEN);

    dwError = VmRESTAllocateHTTPResponsePacket(&pResponse);
    CuAssertTrue(tc, !dwError);

    /**** TEST 5: Larger content length than data****/
    dwError = VmRESTSetHttpHeader(
                  &pResponse,
                  "Content-Length",
                  "20");
    CuAssertTrue(tc, !dwError);

    strcpy(pResponse->messageBody->buffer, "Small data");

    dwError = VMRESTWriteMessageBodyInResponseStream(
                  pResponse,
                  buffer,
                  &bytesWritten
                  );
    CuAssertTrue(tc, dwError);

    VmRESTFreeHTTPResponsePacket(&pResponse);
}

void Test_VMRESTWriteStatusLineInResponseStreamTest1(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             buffer[MAX_DATA_BUFFER_LEN] = {0};
    uint32_t                         bytesWritten = 0;
    PREST_RESPONSE                   pResponse = NULL;

    memset(buffer, '\0', MAX_DATA_BUFFER_LEN);

    dwError = VmRESTAllocateHTTPResponsePacket(&pResponse);
    CuAssertTrue(tc, !dwError);

    /**** TEST 1: Valid case ****/
    strcpy(pResponse->statusLine->version, "HTTP/1.1");
    strcpy(pResponse->statusLine->statusCode, "200");
    strcpy(pResponse->statusLine->reason_phrase, "OK");
    

    dwError = VMRESTWriteStatusLineInResponseStream(
                  pResponse,
                  buffer,
                  &bytesWritten
                  );
    CuAssertTrue(tc, !dwError);
    CuAssertIntEquals(tc, 17, bytesWritten);
    CuAssertStrEquals(tc, "HTTP/1.1 200 OK\r\n", buffer);

    VmRESTFreeHTTPResponsePacket(&pResponse);
}

void Test_VMRESTWriteStatusLineInResponseStreamTest2(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             buffer[MAX_DATA_BUFFER_LEN] = {0};
    uint32_t                         bytesWritten = 0;
    PREST_RESPONSE                   pResponse = NULL;

    memset(buffer, '\0', MAX_DATA_BUFFER_LEN);

    dwError = VmRESTAllocateHTTPResponsePacket(&pResponse);
    CuAssertTrue(tc, !dwError);

    /**** TEST 2: Missing version ****/
    strcpy(pResponse->statusLine->statusCode, "200");
    strcpy(pResponse->statusLine->reason_phrase, "OK");

    dwError = VMRESTWriteStatusLineInResponseStream(
                  pResponse,
                  buffer,
                  &bytesWritten
                  );
    CuAssertTrue(tc, dwError);

    VmRESTFreeHTTPResponsePacket(&pResponse);
}

void Test_VMRESTWriteStatusLineInResponseStreamTest3(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             buffer[MAX_DATA_BUFFER_LEN] = {0};
    uint32_t                         bytesWritten = 0;
    PREST_RESPONSE                   pResponse = NULL;

    memset(buffer, '\0', MAX_DATA_BUFFER_LEN);

    dwError = VmRESTAllocateHTTPResponsePacket(&pResponse);
    CuAssertTrue(tc, !dwError);

    /**** TEST 3: Missing status code ****/
    strcpy(pResponse->statusLine->version, "HTTP/1.1");
    strcpy(pResponse->statusLine->reason_phrase, "OK");


    dwError = VMRESTWriteStatusLineInResponseStream(
                  pResponse,
                  buffer,
                  &bytesWritten
                  );
    CuAssertTrue(tc, dwError);
    VmRESTFreeHTTPResponsePacket(&pResponse);
}

void Test_VMRESTWriteStatusLineInResponseStreamTest4(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             buffer[MAX_DATA_BUFFER_LEN] = {0};
    uint32_t                         bytesWritten = 0;
    PREST_RESPONSE                   pResponse = NULL;

    memset(buffer, '\0', MAX_DATA_BUFFER_LEN);

    dwError = VmRESTAllocateHTTPResponsePacket(&pResponse);
    CuAssertTrue(tc, !dwError);

    /**** TEST 4:  Missing reason phrase****/
    strcpy(pResponse->statusLine->version, "HTTP/1.1");
    strcpy(pResponse->statusLine->statusCode, "200");


    dwError = VMRESTWriteStatusLineInResponseStream(
                  pResponse,
                  buffer,
                  &bytesWritten
                  );
    CuAssertTrue(tc, dwError);
    VmRESTFreeHTTPResponsePacket(&pResponse);
}

void Test_VmRESTAddAllHeaderInResponseStreamTest1(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             buffer[MAX_DATA_BUFFER_LEN] = {0};
    uint32_t                         bytesWritten = 0;
    PREST_RESPONSE                   pResponse = NULL;

    memset(buffer, '\0', MAX_DATA_BUFFER_LEN);

    dwError = VmRESTAllocateHTTPResponsePacket(&pResponse);
    CuAssertTrue(tc, !dwError);

    /**** TEST 1: Valid Case****/
    dwError = VmRESTSetHttpHeader(
                  &pResponse,
                  "Content-Length",
                  "10");
    CuAssertTrue(tc, !dwError);

    dwError = VmRESTSetHttpHeader(
                  &pResponse,
                  "Connection",
                  "close");
    CuAssertTrue(tc, !dwError);

    dwError = VmRESTSetHttpHeader(
                  &pResponse,
                  "Accept",
                  "*/*");
    CuAssertTrue(tc, !dwError);

    dwError = VmRESTSetHttpHeader(
                  &pResponse,
                  "header",
                  "value");
    CuAssertTrue(tc, !dwError);

    dwError = VmRESTAddAllHeaderInResponseStream(
                  pResponse,
                  buffer,
                  &bytesWritten
                  );
    CuAssertTrue(tc, !dwError);
    CuAssertIntEquals(tc, strlen("Content-Length:10\r\nConnection:close\r\nAccept:*/*\r\nheader:value\r\n\r\n"), bytesWritten);
    CuAssertStrEquals(tc, "Content-Length:10\r\nConnection:close\r\nAccept:*/*\r\nheader:value\r\n\r\n", buffer);
    
    VmRESTFreeHTTPResponsePacket(&pResponse);
}

void Test_VmRESTAddAllHeaderInResponseStreamTest2(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             buffer[MAX_DATA_BUFFER_LEN] = {0};
    uint32_t                         bytesWritten = 0;
    PREST_RESPONSE                   pResponse = NULL;

    memset(buffer, '\0', MAX_DATA_BUFFER_LEN);

    dwError = VmRESTAllocateHTTPResponsePacket(&pResponse);
    CuAssertTrue(tc, !dwError);

    /**** TEST 2: No headers ****/

    dwError = VmRESTAddAllHeaderInResponseStream(
                  pResponse,
                  buffer,
                  &bytesWritten
                  );
    CuAssertTrue(tc, !dwError);
    CuAssertIntEquals(tc, 2, bytesWritten);
    CuAssertStrEquals(tc, "\r\n", buffer);

    VmRESTFreeHTTPResponsePacket(&pResponse);
}

void Test_VmRESTAddAllHeaderInResponseStreamTest3(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             buffer[MAX_DATA_BUFFER_LEN] = {0};
    uint32_t                         bytesWritten = 0;
    PREST_RESPONSE                   pResponse = NULL;

    memset(buffer, '\0', MAX_DATA_BUFFER_LEN);

    dwError = VmRESTAllocateHTTPResponsePacket(&pResponse);
    CuAssertTrue(tc, !dwError);

    /**** TEST 3: Valid Case with space ****/
    dwError = VmRESTSetHttpHeader(
                  &pResponse,
                  " Content-Length",
                  "10");
    CuAssertTrue(tc, !dwError);

    dwError = VmRESTAddAllHeaderInResponseStream(
                  pResponse,
                  buffer,
                  &bytesWritten
                  );
    CuAssertTrue(tc, !dwError);
    CuAssertIntEquals(tc, strlen("Content-Length:10\r\n\r\n"), bytesWritten);
    CuAssertStrEquals(tc, "Content-Length:10\r\n\r\n", buffer);

    VmRESTFreeHTTPResponsePacket(&pResponse);
}

void Test_VmRESTAddAllHeaderInResponseStreamTest4(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             buffer[MAX_DATA_BUFFER_LEN] = {0};
    uint32_t                         bytesWritten = 0;
    PREST_RESPONSE                   pResponse = NULL;
    
    memset(buffer, '\0', MAX_DATA_BUFFER_LEN);
    
    dwError = VmRESTAllocateHTTPResponsePacket(&pResponse);
    CuAssertTrue(tc, !dwError);

    /**** TEST 4: Valid Case with space ****/
    dwError = VmRESTSetHttpHeader(
                  &pResponse,
                  "Content-Length ",
                  "10");
    CuAssertTrue(tc, !dwError);

    dwError = VmRESTAddAllHeaderInResponseStream(
                  pResponse,
                  buffer,
                  &bytesWritten
                  );
    CuAssertTrue(tc, !dwError);
    CuAssertIntEquals(tc, strlen("Content-Length:10\r\n\r\n"), bytesWritten);
    CuAssertStrEquals(tc, "Content-Length:10\r\n\r\n", buffer);

    VmRESTFreeHTTPResponsePacket(&pResponse);
}

void Test_VmRESTAddAllHeaderInResponseStreamTest5(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             buffer[MAX_DATA_BUFFER_LEN] = {0};
    uint32_t                         bytesWritten = 0;
    PREST_RESPONSE                   pResponse = NULL;
    
    memset(buffer, '\0', MAX_DATA_BUFFER_LEN);
    
    dwError = VmRESTAllocateHTTPResponsePacket(&pResponse);
    CuAssertTrue(tc, !dwError);

    /**** TEST 5: Valid Case with space ****/
    dwError = VmRESTSetHttpHeader(
                  &pResponse,
                  "Content-Length",
                  " 10");
    CuAssertTrue(tc, !dwError);

    dwError = VmRESTAddAllHeaderInResponseStream(
                  pResponse,
                  buffer,
                  &bytesWritten
                  );
    CuAssertTrue(tc, !dwError);
    CuAssertIntEquals(tc, strlen("Content-Length:10\r\n\r\n"), bytesWritten);
    CuAssertStrEquals(tc, "Content-Length:10\r\n\r\n", buffer);

    VmRESTFreeHTTPResponsePacket(&pResponse);
}

void Test_VmRESTAddAllHeaderInResponseStreamTest6(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             buffer[MAX_DATA_BUFFER_LEN] = {0};
    uint32_t                         bytesWritten = 0;
    PREST_RESPONSE                   pResponse = NULL;
    
    memset(buffer, '\0', MAX_DATA_BUFFER_LEN);
    
    dwError = VmRESTAllocateHTTPResponsePacket(&pResponse);
    CuAssertTrue(tc, !dwError);

    /**** TEST 6: Valid Case with space ****/
    dwError = VmRESTSetHttpHeader(
                  &pResponse,
                  "Content-Length",
                  "10 ");
    CuAssertTrue(tc, !dwError);

    dwError = VmRESTAddAllHeaderInResponseStream(
                  pResponse,
                  buffer,
                  &bytesWritten
                  );
    CuAssertTrue(tc, !dwError);
    CuAssertIntEquals(tc, strlen("Content-Length:10\r\n\r\n"), bytesWritten);
    CuAssertStrEquals(tc, "Content-Length:10\r\n\r\n", buffer);

    VmRESTFreeHTTPResponsePacket(&pResponse);
}

void Test_VmRESTAddAllHeaderInResponseStreamTest7(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             buffer[MAX_DATA_BUFFER_LEN] = {0};
    uint32_t                         bytesWritten = 0;
    PREST_RESPONSE                   pResponse = NULL;

    memset(buffer, '\0', MAX_DATA_BUFFER_LEN);

    dwError = VmRESTAllocateHTTPResponsePacket(&pResponse);
    CuAssertTrue(tc, !dwError);

    /**** TEST 7: Missing value ****/
    dwError = VmRESTSetHttpHeader(
                  &pResponse,
                  "Content-Length",
                  "");
    CuAssertTrue(tc, !dwError);

    dwError = VmRESTAddAllHeaderInResponseStream(
                  pResponse,
                  buffer,
                  &bytesWritten
                  );
    CuAssertTrue(tc, dwError);

    VmRESTFreeHTTPResponsePacket(&pResponse);
}

void Test_VmRESTAddAllHeaderInResponseStreamTest8(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             buffer[MAX_DATA_BUFFER_LEN] = {0};
    uint32_t                         bytesWritten = 0;
    PREST_RESPONSE                   pResponse = NULL;

    memset(buffer, '\0', MAX_DATA_BUFFER_LEN);

    dwError = VmRESTAllocateHTTPResponsePacket(&pResponse);
    CuAssertTrue(tc, !dwError);

    /**** TEST 8: Missing header ****/
    dwError = VmRESTSetHttpHeader(
                  &pResponse,
                  "",
                  "10");
    CuAssertTrue(tc, !dwError);

    dwError = VmRESTAddAllHeaderInResponseStream(
                  pResponse,
                  buffer,
                  &bytesWritten
                  );
    CuAssertTrue(tc, dwError);

    VmRESTFreeHTTPResponsePacket(&pResponse);
}

void Test_VmRESTCloseClientTest1(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    PREST_REQUEST                    pRequest = NULL;
    PREST_RESPONSE                   pResponse = NULL;

    dwError = VmRESTAllocateHTTPRequestPacket(&pRequest);
    CuAssertTrue(tc, !dwError);

    dwError = VmRESTAllocateHTTPResponsePacket(&pResponse);
    CuAssertTrue(tc, !dwError);

    pResponse->requestPacket = pRequest;

    strcpy(pResponse->statusLine->statusCode, "200");

    /**** TEST 1: Valid case ****/
    dwError = VmRESTCloseClient(
                  pResponse);
    CuAssertTrue(tc, !dwError);

}

void Test_VmRESTCloseClientTest2(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    PREST_RESPONSE                   pResponse = NULL;

    dwError = VmRESTAllocateHTTPResponsePacket(&pResponse);
    CuAssertTrue(tc, !dwError);

    pResponse->requestPacket = NULL;

    strcpy(pResponse->statusLine->statusCode, "200");

    /**** TEST 2: Valid case ****/
    dwError = VmRESTCloseClient(
                  pResponse);
    CuAssertTrue(tc, !dwError);
}

void Test_VmRESTCloseClientTest3(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    PREST_REQUEST                    pRequest = NULL;
    PREST_RESPONSE                   pResponse = NULL;

    dwError = VmRESTAllocateHTTPRequestPacket(&pRequest);
    CuAssertTrue(tc, !dwError);

    dwError = VmRESTAllocateHTTPResponsePacket(&pResponse);
    CuAssertTrue(tc, !dwError);

    pResponse->requestPacket = pRequest;

    /**** TEST 3: Missing Status code  ****/
    dwError = VmRESTCloseClient(
                  pResponse);
    CuAssertTrue(tc, !dwError);

}

void Test_VmRESTCloseClientTest4(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    PREST_RESPONSE                   pResponse = NULL;

    dwError = VmRESTAllocateHTTPResponsePacket(&pResponse);
    CuAssertTrue(tc, !dwError);

    pResponse->requestPacket = NULL;

    strcpy(pResponse->statusLine->statusCode, "400");

    /**** TEST 4: Failed status code  ****/
    dwError = VmRESTCloseClient(
                  pResponse);
    CuAssertTrue(tc, !dwError);
}



CuSuite* CuGetTestHTTPProtocolHeadSuite(void)
{
    CuSuite* suite = CuSuiteNew();

    SUITE_ADD_TEST(suite, Test_VmRESTHTTPGetReqMethodTest1);
    SUITE_ADD_TEST(suite, Test_VmRESTHTTPGetReqMethodTest2);
    SUITE_ADD_TEST(suite, Test_VmRESTHTTPGetReqMethodTest3);
    SUITE_ADD_TEST(suite, Test_VmRESTHTTPGetReqMethodTest4);
    SUITE_ADD_TEST(suite, Test_VmRESTHTTPGetReqMethodTest5);
    SUITE_ADD_TEST(suite, Test_VmRESTHTTPGetReqURITest1);
    SUITE_ADD_TEST(suite, Test_VmRESTHTTPGetReqURITest2);
    SUITE_ADD_TEST(suite, Test_VmRESTHTTPGetReqURITest3);
    SUITE_ADD_TEST(suite, Test_VmRESTHTTPGetReqURITest4);
    SUITE_ADD_TEST(suite, Test_VmRESTHTTPGetReqMethodTest5);
    SUITE_ADD_TEST(suite, Test_VmRESTHTTPGetReqVersionTest1);
    SUITE_ADD_TEST(suite, Test_VmRESTHTTPGetReqVersionTest2);
    SUITE_ADD_TEST(suite, Test_VmRESTHTTPGetReqVersionTest3);
    SUITE_ADD_TEST(suite, Test_VmRESTHTTPGetReqVersionTest4);
    SUITE_ADD_TEST(suite, Test_VmRESTHTTPGetReqVersionTest5);
    SUITE_ADD_TEST(suite, Test_VmRESTHTTPPopulateHeaderTest1);
    SUITE_ADD_TEST(suite, Test_VmRESTHTTPPopulateHeaderTest2);
    SUITE_ADD_TEST(suite, Test_VmRESTHTTPPopulateHeaderTest3);
    SUITE_ADD_TEST(suite, Test_VmRESTHTTPPopulateHeaderTest4);
    SUITE_ADD_TEST(suite, Test_VmRESTHTTPPopulateHeaderTest5);
    SUITE_ADD_TEST(suite, Test_VmRESTHTTPPopulateHeaderTest6);
    SUITE_ADD_TEST(suite, Test_VmRESTHTTPPopulateHeaderTest7);
    SUITE_ADD_TEST(suite, Test_VmRESTHTTPPopulateHeaderTest8);
    SUITE_ADD_TEST(suite, Test_VmRESTParseHTTPReqLineTest1);
    SUITE_ADD_TEST(suite, Test_VmRESTParseHTTPReqLineTest2);
    SUITE_ADD_TEST(suite, Test_VmRESTParseHTTPReqLineTest3);
    SUITE_ADD_TEST(suite, Test_VmRESTParseHTTPReqLineTest4);
    SUITE_ADD_TEST(suite, Test_VmRESTParseHTTPReqLineTest5);
    SUITE_ADD_TEST(suite, Test_VmRESTParseHTTPReqLineTest6);
    SUITE_ADD_TEST(suite, Test_VMRESTWriteChunkedMessageInResponseStreamTest1);
    SUITE_ADD_TEST(suite, Test_VMRESTWriteChunkedMessageInResponseStreamTest2);
    SUITE_ADD_TEST(suite, Test_VMRESTWriteChunkedMessageInResponseStreamTest3);
    SUITE_ADD_TEST(suite, Test_VMRESTWriteChunkedMessageInResponseStreamTest4); 
    /**** CRASHING TEST :: MUST FIX ASAP ****/
    SUITE_ADD_TEST(suite, Test_VMRESTWriteChunkedMessageInResponseStreamTest5);
    SUITE_ADD_TEST(suite, Test_VMRESTWriteMessageBodyInResponseStreamTest1);
    SUITE_ADD_TEST(suite, Test_VMRESTWriteMessageBodyInResponseStreamTest2);
    SUITE_ADD_TEST(suite, Test_VMRESTWriteMessageBodyInResponseStreamTest3);
    SUITE_ADD_TEST(suite, Test_VMRESTWriteMessageBodyInResponseStreamTest4);
    SUITE_ADD_TEST(suite, Test_VMRESTWriteMessageBodyInResponseStreamTest5); 
    SUITE_ADD_TEST(suite, Test_VMRESTWriteStatusLineInResponseStreamTest1);
    SUITE_ADD_TEST(suite, Test_VMRESTWriteStatusLineInResponseStreamTest2);
    SUITE_ADD_TEST(suite, Test_VMRESTWriteStatusLineInResponseStreamTest3);
    SUITE_ADD_TEST(suite, Test_VMRESTWriteStatusLineInResponseStreamTest4);
    SUITE_ADD_TEST(suite, Test_VmRESTAddAllHeaderInResponseStreamTest1);
    SUITE_ADD_TEST(suite, Test_VmRESTAddAllHeaderInResponseStreamTest2);
    SUITE_ADD_TEST(suite, Test_VmRESTAddAllHeaderInResponseStreamTest3);
    SUITE_ADD_TEST(suite, Test_VmRESTAddAllHeaderInResponseStreamTest4);
    SUITE_ADD_TEST(suite, Test_VmRESTAddAllHeaderInResponseStreamTest5);
    SUITE_ADD_TEST(suite, Test_VmRESTAddAllHeaderInResponseStreamTest6);
    SUITE_ADD_TEST(suite, Test_VmRESTAddAllHeaderInResponseStreamTest7);
    SUITE_ADD_TEST(suite, Test_VmRESTAddAllHeaderInResponseStreamTest8);
    SUITE_ADD_TEST(suite, Test_VmRESTCloseClientTest1);
    SUITE_ADD_TEST(suite, Test_VmRESTCloseClientTest2);
    SUITE_ADD_TEST(suite, Test_VmRESTCloseClientTest3);
    SUITE_ADD_TEST(suite, Test_VmRESTCloseClientTest4);

    return suite;
}


