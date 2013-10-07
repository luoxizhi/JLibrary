// Copyright (c) 2013, Luoxizhi. All rights reserved.
//
// History:
// Date         Reference       Person          Descriptions
// ---------- 	-------------- 	--------------  ---------------------------
// 2013/10/01                   Luoxizhi        Initial Implementation
//
//-------------------------------------------------------------------------
#include "stdafx.h"
#include "JFile.h"
#include "JUtilsTest.h"
#include "JUtils.h"
#include <iostream>
using std::wcout;
using std::cout;

#ifdef _DEBUG
#pragma comment(lib, "..\\Debug\\JLibrary.lib")
#else
#pragma comment(lib, "..\\Release\\JLibrary.lib")
#endif

bool JUtilsTest::RunFunction(bool result, string strFunc)
{
    if(result == false)
    {
        cout << "Failed  " << strFunc << "\n";
    }
    else
    {
        cout << "Pass    " <<strFunc << "\n";
    }
    return result;
}

bool JUtilsTest::NormalTest()
{
    cout << "---------------------------------------\n";
    cout << "     File JUtils.h starting...\n";    
    bool all_pass = RunFunction(TestJUtilsClass(), "JUtils class")
                 && RunFunction(TestJLogUtilsClass(), "JLogUtils class");
    cout << "     File JUtils.h " << (all_pass? "PASS" : "FAILED") << "\n";
    cout << "---------------------------------------\n";
    return all_pass;
}

bool JUtilsTest::TestJUtilsClass()
{
    return JUtils::ConvertTime(L"1s") == 1000
        && JUtils::ConvertTime(L"1M") == 60000
        && JUtils::ConvertTime("1H") == 60*60*1000
        && JUtils::ConvertTime("1h1m1s") == 3600000+60000+1000
        && JUtils::ConvertTime("1d") == 24*60*60*1000
        && JUtils::ConvertSize("1k") == 1024
        && JUtils::ConvertSize("1m") == 1024*1024
        && JUtils::ConvertSize("1g") == 1024*1024*1024
        && JUtils::ConvertSize(L"15m") == 15*1024*1024
        && JUtils::AsciiToUnicodeString("hello") == L"hello"
        && JUtils::UnicodeToAsciiString(L"hello") == "hello";
}

bool JUtilsTest::TestJLogUtilsClass()
{
    LogData data;
    data.dt = JDateTime::Now();
    data.text = L"hello world";

    wstring log = JLogUtils::LogData2String(data);
    LogData newdata = JLogUtils::String2LogData(log);
    if(data.dt != newdata.dt || data.text != newdata.text)
        return false;

    return true;
}