// Copyright (c) 2013, Luoxizhi. All rights reserved.
//
// History:
// Date         Reference       Person          Descriptions
// ---------- 	-------------- 	--------------  ---------------------------
// 2013/10/01                   Luoxizhi        Initial Implementation
//
//-------------------------------------------------------------------------
#pragma once

#ifndef JFileTest_H
#define JFileTest_H

#include <JString.h>

class JFileTest
{
public:
    static bool NormalTest();

private:
    static bool RunFunction(bool result, string strFunc);

    static bool TestDirectorySome();
    static bool TestIsFileExist();
};

#endif