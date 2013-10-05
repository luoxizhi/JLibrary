// Copyright (c) 2013, Luoxizhi. All rights reserved.
//
// History:
// Date         Reference       Person          Descriptions
// ---------- 	-------------- 	--------------  ---------------------------
// 2013/10/01                   Luoxizhi        Initial Implementation
//
//-------------------------------------------------------------------------
#pragma once

#ifndef JTimeTest_H
#define JTimeTest_H

#include <string>
using std::string;
using std::wstring;

class JTimeTest
{
public:
    static bool NormalTest();

private:
    static bool RunFunction(bool result, string strFunc);

    static bool TestJTime();
    static bool TestJDateTime();
};

#endif