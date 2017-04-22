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

#include <CuTest.h>
#include "includes.h"


void Test_RestProtoHeadInitServer(
    CuTest* tc
    )
{
    uint32_t  dwError = 0;

    /**** TEST 1:  Init the library with config file ****/
    dwError = VmRESTInit(NULL,"/tmp/restconfig.txt");
    CuAssertTrue(tc, !dwError);
}

void Test_RestProtoHeadRegisterHandler(
    CuTest* tc
    )
{
    uint32_t  dwError = 0;

    /**** TEST 1: Register the dummy callback(Endpoint) ****/
    dwError = VmRESTRegisterHandler(
                  "/v1/pkg",
                  gpVmRestHandlers,
                  NULL
                  );
    CuAssertTrue(tc, !dwError);
}

void Test_VmRestEngineHandlerTest1(
    CuTest* tc
    )
{
    uint32_t                        dwError = 0;
    PREST_REQUEST                   pRequest = NULL;
    PREST_RESPONSE                  pResponse = NULL;

    dwError = VmRESTAllocateHTTPRequestPacket(&pRequest);
    CuAssertTrue(tc, !dwError);

    dwError = VmRESTAllocateHTTPResponsePacket(&pResponse);
    CuAssertTrue(tc, !dwError);

 
    /**** TEST 1: Valid case ****/
    strcpy(pRequest->requestLine->method, "GET");
    strcpy(pRequest->requestLine->uri, "/v1/pkg?x=y");
    
    dwError = VmRestEngineHandler(
                  pRequest,
                  &pResponse
                  );
    CuAssertTrue(tc, !dwError);

    VmRESTFreeHTTPRequestPacket(&pRequest);
    VmRESTFreeHTTPResponsePacket(&pResponse);
}

void Test_VmRestEngineHandlerTest2(
    CuTest* tc
    )
{
    uint32_t                        dwError = 0;
    PREST_REQUEST                   pRequest = NULL;
    PREST_RESPONSE                  pResponse = NULL;

    dwError = VmRESTAllocateHTTPRequestPacket(&pRequest);
    CuAssertTrue(tc, !dwError);

    dwError = VmRESTAllocateHTTPResponsePacket(&pResponse);
    CuAssertTrue(tc, !dwError);

 
    /**** TEST2: Invalid URI *****/
    strcpy(pRequest->requestLine->method, "POST");
    strcpy(pRequest->requestLine->uri, "blah");
    dwError = VmRestEngineHandler(
                  pRequest,
                  &pResponse
                  );
    CuAssertTrue(tc, dwError);

    VmRESTFreeHTTPRequestPacket(&pRequest);
    VmRESTFreeHTTPResponsePacket(&pResponse);
}

void Test_VmRestEngineHandlerTest3(
    CuTest* tc
    )
{
    uint32_t                        dwError = 0;
    PREST_REQUEST                   pRequest = NULL;
    PREST_RESPONSE                  pResponse = NULL;

    dwError = VmRESTAllocateHTTPRequestPacket(&pRequest);
    CuAssertTrue(tc, !dwError);

    dwError = VmRESTAllocateHTTPResponsePacket(&pResponse);
    CuAssertTrue(tc, !dwError);

    /**** TEST 3: Invalid method Name ****/
    strcpy(pRequest->requestLine->method, "BINGO");
    strcpy(pRequest->requestLine->uri, "/v1/pkg?x=y");

    strcpy(pRequest->requestLine->uri, "blah");
    dwError = VmRestEngineHandler(
                  pRequest,
                  &pResponse
                  );
    CuAssertTrue(tc, dwError);

    VmRESTFreeHTTPRequestPacket(&pRequest);
    VmRESTFreeHTTPResponsePacket(&pResponse);
}

void Test_VmRestEngineHandlerTest4(
    CuTest* tc
    )
{
    uint32_t                        dwError = 0;
    PREST_REQUEST                   pRequest = NULL;
    PREST_RESPONSE                  pResponse = NULL;

    dwError = VmRESTAllocateHTTPRequestPacket(&pRequest);
    CuAssertTrue(tc, !dwError);

    dwError = VmRESTAllocateHTTPResponsePacket(&pResponse);
    CuAssertTrue(tc, !dwError);

    /**** TEST 4: Empty method Name ****/
    strcpy(pRequest->requestLine->method, "");
    strcpy(pRequest->requestLine->uri, "/v1/pkg?x=y");

    dwError = VmRestEngineHandler(
                  pRequest,
                  &pResponse
                  );
    CuAssertTrue(tc, dwError);

    VmRESTFreeHTTPRequestPacket(&pRequest);
    VmRESTFreeHTTPResponsePacket(&pResponse);
}

void Test_VmRestEngineHandlerTest5(
    CuTest* tc
    )
{
    uint32_t                        dwError = 0;
    PREST_REQUEST                   pRequest = NULL;
    PREST_RESPONSE                  pResponse = NULL;

    dwError = VmRESTAllocateHTTPRequestPacket(&pRequest);
    CuAssertTrue(tc, !dwError);

    dwError = VmRESTAllocateHTTPResponsePacket(&pResponse);
    CuAssertTrue(tc, !dwError);

    /**** TEST 5: Empty URI Name ****/
    strcpy(pRequest->requestLine->method, "PUT");
    strcpy(pRequest->requestLine->uri, "");

    dwError = VmRestEngineHandler(
                  pRequest,
                  &pResponse
                  );
    CuAssertTrue(tc, dwError);

    VmRESTFreeHTTPRequestPacket(&pRequest);
    VmRESTFreeHTTPResponsePacket(&pResponse);
}

void Test_VmRestEngineHandlerTest6(
    CuTest* tc
    )
{
    uint32_t                        dwError = 0;
    PREST_REQUEST                   pRequest = NULL;
    PREST_RESPONSE                  pResponse = NULL;

    dwError = VmRESTAllocateHTTPRequestPacket(&pRequest);
    CuAssertTrue(tc, !dwError);

    dwError = VmRESTAllocateHTTPResponsePacket(&pResponse);
    CuAssertTrue(tc, !dwError);

    /**** TEST 6: Large invalid method name ****/
    strcpy(pRequest->requestLine->method, "THIS IS INVALID METHOD NAME");
    strcpy(pRequest->requestLine->uri, "");

    dwError = VmRestEngineHandler(
                  pRequest,
                  &pResponse
                  );
    CuAssertTrue(tc, dwError);

    VmRESTFreeHTTPRequestPacket(&pRequest);
    VmRESTFreeHTTPResponsePacket(&pResponse);
}

void Test_VmRestEngineTESTEndPointsAddDeleteTest1(
    CuTest* tc
    )
{
    uint32_t                        dwError = 0;

    /**** TEST 1: Init the system ****/
    dwError = VmRestEngineInitEndPointRegistration();
    CuAssertIntEquals(tc, 0, dwError);
}

void Test_VmRestEngineTESTEndPointsAddDeleteTest2(
    CuTest* tc
    )
{
    uint32_t                        dwError = 0;
    PREST_ENDPOINT                  pEndPoint = NULL;

    /**** TEST 2: Valid URL - ADD and Delete ****/
    dwError = VmRestEngineAddEndpoint(
                  "/v1/tdnf",
                  gpVmRestHandlers
                  );
    CuAssertIntEquals(tc, 0, dwError);

    dwError = VmRestEngineGetEndPoint(
                  "/v1/tdnf",
                  &pEndPoint);
    CuAssertIntEquals(tc, 0, dwError);
    CuAssertStrEquals(tc, "/v1/tdnf", pEndPoint->pszEndPointURI);

    dwError = VmRestEngineRemoveEndpoint(
                  "/v1/tdnf"
                  );
    CuAssertIntEquals(tc, 0, dwError);

    dwError = VmRestEngineGetEndPoint(
                  "/v1/tdnf",
                  &pEndPoint);
    CuAssertTrue(tc, dwError);
}

void Test_VmRestEngineTESTEndPointsAddDeleteTest3(
    CuTest* tc
    )
{
    uint32_t                        dwError = 0;

    /**** TEST 3: Duplicate URL ****/
    dwError = VmRestEngineAddEndpoint(
                  "/v1/restServer",
                  gpVmRestHandlers
                  );
    CuAssertIntEquals(tc, 0, dwError);

    dwError = VmRestEngineAddEndpoint(
                  "/v1/restServer",
                  gpVmRestHandlers
                  );
    CuAssertTrue(tc, dwError);
}

void Test_VmRestEngineTESTEndPointsAddDeleteTest4(
    CuTest* tc
    )
{
    uint32_t                        dwError = 0;
  
    /**** TEST 4: Empty URL ****/
    dwError = VmRestEngineAddEndpoint(
                  "",
                  gpVmRestHandlers
                  );
    CuAssertTrue(tc, dwError);
}

void Test_VmRestEngineTESTEndPointsAddDeleteTest5(
    CuTest* tc
    )
{
    uint32_t                        dwError = 0;
    /**** TEST 5: Space in URL ****/
    dwError = VmRestEngineAddEndpoint(
                  "/v1 /vrrr",
                  gpVmRestHandlers
                  );
    CuAssertTrue(tc, dwError);

}

void Test_VmRestEngineTESTEndPointsAddDeleteTest6(
    CuTest* tc
    )
{
    /**** This will delete all added endpoints ****/
    VmRestEngineShutdownEndPointRegistration();
}

void Test_EndPointURIManupulationTest1(
    CuTest* tc
    )
{
    uint32_t  dwError = 0;
    char*     endPointURI = NULL;
    uint32_t  paramsCount = 0;

    memset(endPointURI, '\0', 128);

    /**** TEST 1: Valid Endpoint URI ****/
    dwError = VmRestGetEndPointURIfromRequestURI(
                  "/v1/pkg?x=y",
                  &endPointURI
                  );
    CuAssertIntEquals(tc, 0, dwError);
    CuAssertStrEquals(tc, "/v1/pkg", endPointURI);

    dwError = VmRestGetParamsCountInReqURI(
                  "/v1/pkg?x=y",
                  &paramsCount);
    CuAssertIntEquals(tc, 0, dwError);
    CuAssertIntEquals(tc, 1, paramsCount);
}

void Test_EndPointURIManupulationTest2(
    CuTest* tc
    )
{
    uint32_t  dwError = 0;
    char*     endPointURI = NULL;
    uint32_t  paramsCount = 0;

    /**** TEST 2: Valid Endpoint URI with multiple params****/
    memset(endPointURI, '\0', 128);
    dwError = VmRestGetEndPointURIfromRequestURI(
                  "/v1/tdnf/version?x=y&PkgName=flex&Version=5",
                  &endPointURI
                  );
    CuAssertIntEquals(tc, 0, dwError);
    CuAssertStrEquals(tc, "/v1/tdnf/version", endPointURI);

    dwError = VmRestGetParamsCountInReqURI(
                  "/v1/tdnf/version?x=y&PkgName=flex&Version=5",
                  &paramsCount);
    CuAssertIntEquals(tc, 0, dwError);
    CuAssertIntEquals(tc, 3, paramsCount);
}

void Test_EndPointURIManupulationTest3(
    CuTest* tc
    )
{
    uint32_t  dwError = 0;
    char*     endPointURI = NULL;
    uint32_t  paramsCount = 0;


    /**** TEST 3: No params in valid URL ****/
    memset(endPointURI, '\0', 128);
    dwError = VmRestGetEndPointURIfromRequestURI(
                  "/v1/restserver",
                  &endPointURI
                  );
    CuAssertIntEquals(tc, 0, dwError);
    CuAssertStrEquals(tc, "/v1/restserver", endPointURI);

    dwError = VmRestGetParamsCountInReqURI(
                  "/v1/restserver",
                  &paramsCount);
    CuAssertIntEquals(tc, 0, dwError);
    CuAssertIntEquals(tc, 0, paramsCount);
}

void Test_EndPointURIManupulationTest4(
    CuTest* tc
    )
{
    uint32_t  dwError = 0;
    char*     endPointURI = NULL;
    uint32_t  paramsCount = 0;


    /**** TEST 4: Invalid URL ****/
    memset(endPointURI, '\0', 128);
    dwError = VmRestGetEndPointURIfromRequestURI(
                  "/v1   restserver",
                  &endPointURI
                  );
    CuAssertTrue(tc, dwError);

    dwError = VmRestGetParamsCountInReqURI(
                  "invalid url",
                  &paramsCount);
    CuAssertTrue(tc, dwError);
}

void Test_EndPointURIManupulationTest5(
    CuTest* tc
    )
{
    uint32_t  dwError = 0;
    char*     endPointURI = NULL;
    uint32_t  paramsCount = 0;

    /**** TEST 5: Empty URL ****/
    memset(endPointURI, '\0', 128);
    dwError = VmRestGetEndPointURIfromRequestURI(
                  "",
                  &endPointURI
                  );
    CuAssertTrue(tc, dwError);

    dwError = VmRestGetParamsCountInReqURI(
                  "",
                  &paramsCount);
    CuAssertTrue(tc, dwError); 
}

void Test_SetURLParamsInRequestObjectTest1(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char*                            key = NULL;
    char*                            value = NULL;
    PREST_REQUEST                    pRequest = NULL;

    /**** TEST 1: Valid Case with single params *****/
    dwError = VmRESTAllocateHTTPRequestPacket(&pRequest);
    CuAssertTrue(tc, !dwError);

    dwError = VmRestParseParams(
                  "/v1/tdnf?MyName=MyValue",
                  1,
                  pRequest
                  );
    CuAssertIntEquals(tc, 0, dwError);

    dwError = VmRESTGetParamsByIndex(
                  pRequest,
                  1,
                  1,
                  &key,
                  &value);
    CuAssertIntEquals(tc, 0, dwError);
    CuAssertStrEquals(tc, "MyName", key);
    CuAssertStrEquals(tc, "MyValue", value);
    
    VmRESTFreeHTTPRequestPacket(&pRequest);
}

void Test_SetURLParamsInRequestObjectTest2(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char*                            key = NULL;
    char*                            value = NULL;
    PREST_REQUEST                    pRequest = NULL;

    /**** TEST 2: Valid Case with multiple params *****/
    dwError = VmRESTAllocateHTTPRequestPacket(&pRequest);
    CuAssertTrue(tc, !dwError);

    dwError = VmRestParseParams(
                  "/v1/tdnf?MyName1=MyValue1&MyName2=MyValue2",
                  2,
                  pRequest
                  );
    CuAssertIntEquals(tc, 0, dwError);

    dwError = VmRESTGetParamsByIndex(
                  pRequest,
                  2,
                  1,
                  &key,
                  &value);
    CuAssertIntEquals(tc, 0, dwError);
    CuAssertStrEquals(tc, "MyName1", key);
    CuAssertStrEquals(tc, "MyValue1", value);

    dwError = VmRESTGetParamsByIndex(
                  pRequest,
                  2,
                  2,
                  &key,
                  &value);
    CuAssertIntEquals(tc, 0, dwError);
    CuAssertStrEquals(tc, "MyName2", key);
    CuAssertStrEquals(tc, "MyValue2", value);

    VmRESTFreeHTTPRequestPacket(&pRequest);
}

void Test_SetURLParamsInRequestObjectTest3(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char*                            key = NULL;
    char*                            value = NULL;
    PREST_REQUEST                    pRequest = NULL;

    /**** TEST 3: Valid Case with No Params *****/
    dwError = VmRESTAllocateHTTPRequestPacket(&pRequest);
    CuAssertTrue(tc, !dwError);

    dwError = VmRestParseParams(
                  "/v1/tdnf",
                  0,
                  pRequest
                  );
    CuAssertIntEquals(tc, 0, dwError);

    dwError = VmRESTGetParamsByIndex(
                  pRequest,
                  1,
                  1,
                  &key,
                  &value);
    CuAssertTrue(tc, dwError);

    VmRESTFreeHTTPRequestPacket(&pRequest);
}

void Test_SetURLParamsInRequestObjectTest4(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char*                            key = NULL;
    char*                            value = NULL;
    PREST_REQUEST                    pRequest = NULL;

    /**** TEST 4: Bad URL *****/
    dwError = VmRESTAllocateHTTPRequestPacket(&pRequest);
    CuAssertTrue(tc, !dwError);

    dwError = VmRestParseParams(
                  "/v1/tdnf?",
                  0,
                  pRequest
                  );
    CuAssertIntEquals(tc, 0, dwError);

    dwError = VmRESTGetParamsByIndex(
                  pRequest,
                  1,
                  1,
                  &key,
                  &value);
    CuAssertTrue(tc, dwError);

    VmRESTFreeHTTPRequestPacket(&pRequest);
}

void Test_SetURLParamsInRequestObjectTest5(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char*                            key = NULL;
    char*                            value = NULL;
    PREST_REQUEST                    pRequest = NULL;

    /**** TEST 5: Query by bad index *****/
    dwError = VmRESTAllocateHTTPRequestPacket(&pRequest);
    CuAssertTrue(tc, !dwError);

    dwError = VmRestParseParams(
                  "/v1/tdnf?x=y",
                  1,
                  pRequest
                  );
    CuAssertIntEquals(tc, 0, dwError);

    dwError = VmRESTGetParamsByIndex(
                  pRequest,
                  1,
                  2,
                  &key,
                  &value);
    CuAssertTrue(tc, dwError);

    VmRESTFreeHTTPRequestPacket(&pRequest);
}

void Test_SetURLParamsInRequestObjectTest6(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char*                            key = NULL;
    char*                            value = NULL;
    PREST_REQUEST                    pRequest = NULL;

    /**** TEST 6: No Value just key *****/
    dwError = VmRESTAllocateHTTPRequestPacket(&pRequest);
    CuAssertTrue(tc, !dwError);

    dwError = VmRestParseParams(
                  "/v1/tdnf?EmptyKey=",
                  1,
                  pRequest
                  );
    CuAssertIntEquals(tc, 0, dwError);

    dwError = VmRESTGetParamsByIndex(
                  pRequest,
                  1,
                  1,
                  &key,
                  &value);
    CuAssertIntEquals(tc, 0, dwError);
    CuAssertStrEquals(tc, "EmptyKey", key);
    CuAssertStrEquals(tc, "", value);
    
    VmRESTFreeHTTPRequestPacket(&pRequest);
}

void Test_SetURLParamsInRequestObjectTest7(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char*                            key = NULL;
    char*                            value = NULL;
    PREST_REQUEST                    pRequest = NULL;

    /**** TEST 7: Extra & in URL *****/
    dwError = VmRESTAllocateHTTPRequestPacket(&pRequest);
    CuAssertTrue(tc, !dwError);

    dwError = VmRestParseParams(
                  "/v1/tdnf?Key=val&key2=val2&",
                  1,
                  pRequest
                  );
    CuAssertIntEquals(tc, 0, dwError);

    dwError = VmRESTGetParamsByIndex(
                  pRequest,
                  1,
                  1,
                  &key,
                  &value);
    CuAssertIntEquals(tc, 0, dwError);
    CuAssertStrEquals(tc, "Key", key);
    CuAssertStrEquals(tc, "val", value);

    VmRESTFreeHTTPRequestPacket(&pRequest);
}

CuSuite* CuGetRestProtocolHeadSuite(void)
{
	CuSuite* suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, Test_RestProtoHeadInitServer);
        SUITE_ADD_TEST(suite, Test_RestProtoHeadRegisterHandler);
        SUITE_ADD_TEST(suite, Test_VmRestEngineHandlerTest1);
        SUITE_ADD_TEST(suite, Test_VmRestEngineHandlerTest2);
        SUITE_ADD_TEST(suite, Test_VmRestEngineHandlerTest3);
        SUITE_ADD_TEST(suite, Test_VmRestEngineHandlerTest4);
        SUITE_ADD_TEST(suite, Test_VmRestEngineHandlerTest5);
        SUITE_ADD_TEST(suite, Test_VmRestEngineHandlerTest6);
        SUITE_ADD_TEST(suite, Test_VmRestEngineTESTEndPointsAddDeleteTest1);
        SUITE_ADD_TEST(suite, Test_VmRestEngineTESTEndPointsAddDeleteTest2);
        SUITE_ADD_TEST(suite, Test_VmRestEngineTESTEndPointsAddDeleteTest3);
        SUITE_ADD_TEST(suite, Test_VmRestEngineTESTEndPointsAddDeleteTest4);
        SUITE_ADD_TEST(suite, Test_VmRestEngineTESTEndPointsAddDeleteTest5);
        SUITE_ADD_TEST(suite, Test_VmRestEngineTESTEndPointsAddDeleteTest6);
        SUITE_ADD_TEST(suite, Test_EndPointURIManupulationTest1);
        SUITE_ADD_TEST(suite, Test_EndPointURIManupulationTest2);
        SUITE_ADD_TEST(suite, Test_EndPointURIManupulationTest3);
        SUITE_ADD_TEST(suite, Test_EndPointURIManupulationTest4);
        SUITE_ADD_TEST(suite, Test_EndPointURIManupulationTest5);
        SUITE_ADD_TEST(suite, Test_SetURLParamsInRequestObjectTest1);
        SUITE_ADD_TEST(suite, Test_SetURLParamsInRequestObjectTest2);
        SUITE_ADD_TEST(suite, Test_SetURLParamsInRequestObjectTest3);
        SUITE_ADD_TEST(suite, Test_SetURLParamsInRequestObjectTest4);
        SUITE_ADD_TEST(suite, Test_SetURLParamsInRequestObjectTest5);
        SUITE_ADD_TEST(suite, Test_SetURLParamsInRequestObjectTest6);
        SUITE_ADD_TEST(suite, Test_SetURLParamsInRequestObjectTest7);

	return suite;
}

