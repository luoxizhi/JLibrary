// Copyright (c) 2013, Luoxizhi. All rights reserved.
//
// History:
// Date         Reference       Person          Descriptions
// ---------- 	-------------- 	--------------  ---------------------------
// 2013/10/01                   Luoxizhi        Initial Implementation
//
//-------------------------------------------------------------------------
#pragma once

#ifndef JIniFileTest_H
#define JIniFileTest_H

class JIniFileTest
{
public:
    static bool NormalTest();

private:
    static bool RunFunction(bool result, string strFunc);

    static bool TestJIniFileClass();
};

#endif