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


void Test_VmRESTTrimSpacesTest1(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             src[64] = {0};
    char*                            ignoreSpace = NULL;

    memset(src, '\0', 64);
    strcpy(src, " Header:Value");

    /**** TEST 1: Valid Case ****/
    dwError = VmRESTTrimSpaces(
                  src,
                  &ignoreSpace);
    CuAssertTrue(tc, !dwError);
    CuAssertStrEquals(tc, "Header:Value", ignoreSpace);
              
}

void Test_VmRESTTrimSpacesTest2(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             src[64] = {0};
    char*                            ignoreSpace = NULL;

    memset(src, '\0', 64);
    strcpy(src, "      Header:Value");

    /**** TEST 2: Valid Case ****/
    dwError = VmRESTTrimSpaces(
                  src,
                  &ignoreSpace);
    CuAssertTrue(tc, !dwError);
    CuAssertStrEquals(tc, "Header:Value", ignoreSpace);

}

void Test_VmRESTTrimSpacesTest3(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             src[64] = {0};
    char*                            ignoreSpace = NULL;

    memset(src, '\0', 64);
    strcpy(src, "  Header : Valu e ");

    /**** TEST 3: Valid Case ****/
    dwError = VmRESTTrimSpaces(
                  src,
                  &ignoreSpace);
    CuAssertTrue(tc, !dwError);
    CuAssertStrEquals(tc, "Header : Valu e", ignoreSpace);

}

void Test_VmRESTTrimSpacesTest4(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             src[64] = {0};
    char*                            ignoreSpace = NULL;

    memset(src, 'B', 64);

    /**** TEST 4: Valid Case ****/
    dwError = VmRESTTrimSpaces(
                  src,
                  &ignoreSpace);
    CuAssertTrue(tc, !dwError);
    CuAssertStrEquals(tc, src, ignoreSpace);

}

void Test_VmRESTTrimSpacesTest5(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             src[64] = {0};
    char*                            ignoreSpace = NULL;

    memset(src, '\0', 64);
    strcpy(src, "    ");

    /**** TEST 5: Valid case ****/
    dwError = VmRESTTrimSpaces(
                  src,
                  &ignoreSpace);
    CuAssertTrue(tc, !dwError);
    CuAssertStrEquals(tc, "", ignoreSpace);

}

void Test_VmRESTTrimSpacesTest6(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char*                            ignoreSpace = NULL;

    /**** TEST 6: Invalid case  ****/
    dwError = VmRESTTrimSpaces(
                  NULL,
                  &ignoreSpace);
    CuAssertTrue(tc, dwError);

}

void Test_VmRESTTrimSpacesTest7(
    CuTest* tc
    )
{
    uint32_t                         dwError = 0;
    char                             src[64] = {0};

    memset(src, '\0', 64);
    strcpy(src, " bhaj ");

    /**** TEST 7: Invalid case  ****/
    dwError = VmRESTTrimSpaces(
                  src,
                  NULL);
    CuAssertTrue(tc, dwError);
}





CuSuite* CuGetHttpValidateSuite(void)
{
    CuSuite* suite = CuSuiteNew();

    SUITE_ADD_TEST(suite, Test_VmRESTTrimSpacesTest1);
    SUITE_ADD_TEST(suite, Test_VmRESTTrimSpacesTest2);
    SUITE_ADD_TEST(suite, Test_VmRESTTrimSpacesTest3);
    SUITE_ADD_TEST(suite, Test_VmRESTTrimSpacesTest4);
    SUITE_ADD_TEST(suite, Test_VmRESTTrimSpacesTest5);
    SUITE_ADD_TEST(suite, Test_VmRESTTrimSpacesTest6);
    SUITE_ADD_TEST(suite, Test_VmRESTTrimSpacesTest7);

    return suite;
}

