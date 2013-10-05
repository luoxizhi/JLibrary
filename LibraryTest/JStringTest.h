// Copyright (c) 2013, Luoxizhi. All rights reserved.
//
// History:
// Date         Reference       Person          Descriptions
// ---------- 	-------------- 	--------------  ---------------------------
// 2013/10/01                   Luoxizhi        Initial Implementation
//
//-------------------------------------------------------------------------
#pragma once

#ifndef JStringTest_H
#define JStringTest_H

#include <string>
using std::string;

class JStringTest
{
public:
    static bool NormalTest();

private:
    static bool TestStringCutLeft_Size();
    static bool TestStringCutLeft_String();
    static bool TestStringCutRight_Size();
    static bool TestStringCutRight_String();
    static bool TestTrimString();
    static bool TestTrimLeft();
    static bool TestTrimRight();
    static bool TestStringSplit_Single();
    static bool TestStringSplit_Multi();
    static bool TestUpperCase();
    static bool TestLowerCase();
    static bool TestStringLengthFixed();
    static bool TestSubStrPairs();
    static bool TestStringReplaceAll_Single();
    static bool TestStringReplaceAll_Multi();
    static bool TestStringReverse();
    static bool TestToString();
    static bool TestToWString();
    static bool TestToWStringHex();
    static bool TestToStringHex();

    static bool RunFunction(bool result, string strFunc);
};

#endif