// Copyright (c) 2013, Luoxizhi. All rights reserved.
//
// History:
// Date         Reference       Person          Descriptions
// ---------- 	-------------- 	--------------  ---------------------------
// 2013/10/01                   Luoxizhi        Initial Implementation
//
//-------------------------------------------------------------------------
#pragma once

#ifndef JUtilsTest_H
#define JUtilsTest_H

class JUtilsTest
{
public:
    static bool NormalTest();

private:
    static bool RunFunction(bool result, string strFunc);

    static bool TestJUtilsClass();
    static bool TestJLogUtilsClass();
};

#endif