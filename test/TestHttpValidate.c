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


void Test_VmRESTRemovePreSpaceTest1(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             src[64] = {0};
    char                             des[64] = {0}; 

    memset(src, '\0', 64);
    memset(des, '\0', 64);

    strcpy(src, " Header:Value");

    /**** TEST 1: Valid Case ****/
    dwError = VmRESTRemovePreSpace(
                  src,
                  des);
    CuAssertTrue(tc, !dwError);
    CuAssertStrEquals(tc, "Header:Value", des);
              
}

void Test_VmRESTRemovePreSpaceTest2(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             src[64] = {0};
    char                             des[64] = {0};

    memset(src, '\0', 64);
    memset(des, '\0', 64);

    strcpy(src, "       Header:Value");

    /**** TEST 2: Valid Case ****/
    dwError = VmRESTRemovePreSpace(
                  src,
                  des);
    CuAssertTrue(tc, !dwError);
    CuAssertStrEquals(tc, "Header:Value", des);
              
}

void Test_VmRESTRemovePreSpaceTest3(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             src[64] = {0};
    char                             des[64] = {0};

    memset(src, '\0', 64);
    memset(des, '\0', 64);

    strcpy(src, "Header:Value");

    /**** TEST 3: Valid Case ****/
    dwError = VmRESTRemovePreSpace(
                  src,
                  des);
    CuAssertTrue(tc, !dwError);
    CuAssertStrEquals(tc, "Header:Value", des);

}

void Test_VmRESTRemovePreSpaceTest4(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             src[64] = {0};
    char                             des[64] = {0};

    memset(src, '\0', 64);
    memset(des, '\0', 64);

    strcpy(src, "");

    /**** TEST 4: Valid Case ****/
    dwError = VmRESTRemovePreSpace(
                  src,
                  des);
    CuAssertTrue(tc, !dwError);
    CuAssertStrEquals(tc, "", des);

}

void Test_VmRESTRemovePostSpaceTest1(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             src[64] = {0};
    char                             des[64] = {0};

    memset(src, '\0', 64);
    memset(des, '\0', 64);

    strcpy(src, "Header:Value ");

    /**** TEST 1: Valid Case ****/
    dwError = VmRESTRemovePreSpace(
                  src,
                  des);
    CuAssertTrue(tc, !dwError);
    CuAssertStrEquals(tc, "Header:Value", des);

}

void Test_VmRESTRemovePostSpaceTest2(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             src[64] = {0};
    char                             des[64] = {0};

    memset(src, '\0', 64);
    memset(des, '\0', 64);

    strcpy(src, "Header:Value       ");

    /**** TEST 2: Valid Case ****/
    dwError = VmRESTRemovePreSpace(
                  src,
                  des);
    CuAssertTrue(tc, !dwError);
    CuAssertStrEquals(tc, "Header:Value", des);

}

void Test_VmRESTRemovePostSpaceTest3(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             src[64] = {0};
    char                             des[64] = {0};

    memset(src, '\0', 64);
    memset(des, '\0', 64);

    strcpy(src, "Header:Value");

    /**** TEST 3: Valid Case ****/
    dwError = VmRESTRemovePreSpace(
                  src,
                  des);
    CuAssertTrue(tc, !dwError);
    CuAssertStrEquals(tc, "Header:Value", des);
}

void Test_VmRESTRemovePostSpaceTest4(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             src[64] = {0};
    char                             des[64] = {0};

    memset(src, '\0', 64);
    memset(des, '\0', 64);

    strcpy(src, "");

    /**** TEST 4: Valid Case ****/
    dwError = VmRESTRemovePreSpace(
                  src,
                  des);
    CuAssertTrue(tc, !dwError);
    CuAssertStrEquals(tc, "", des);
}

CuSuite* CuGetHttpValidateSuite(void)
{
    CuSuite* suite = CuSuiteNew();

    SUITE_ADD_TEST(suite, Test_VmRESTRemovePreSpaceTest1);
    SUITE_ADD_TEST(suite, Test_VmRESTRemovePreSpaceTest2);
    SUITE_ADD_TEST(suite, Test_VmRESTRemovePreSpaceTest3);
    SUITE_ADD_TEST(suite, Test_VmRESTRemovePreSpaceTest4);
    SUITE_ADD_TEST(suite, Test_VmRESTRemovePostSpaceTest1);
    SUITE_ADD_TEST(suite, Test_VmRESTRemovePostSpaceTest2);
    SUITE_ADD_TEST(suite, Test_VmRESTRemovePostSpaceTest3);
    SUITE_ADD_TEST(suite, Test_VmRESTRemovePostSpaceTest4);
        
    return suite;
}

