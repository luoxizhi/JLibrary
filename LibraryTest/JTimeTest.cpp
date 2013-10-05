// Copyright (c) 2013, Luoxizhi. All rights reserved.
//
// History:
// Date         Reference       Person          Descriptions
// ---------- 	-------------- 	--------------  ---------------------------
// 2013/10/01                   Luoxizhi        Initial Implementation
//
//-------------------------------------------------------------------------
#include "stdafx.h"
#include "JTimeTest.h"
#include "JTime.h"
#include <iostream>
using std::wcout;
using std::cout;

#ifdef _DEBUG
#pragma comment(lib, "..\\Debug\\JLibrary.lib")
#else
#pragma comment(lib, "..\\Release\\JLibrary.lib")
#endif

bool JTimeTest::RunFunction(bool result, string strFunc)
{
    if(result == false)
    {
        cout << "Failed    " << strFunc << "\n";
    }
    else
    {
        cout << "Pass    " <<strFunc << "\n";
    }
    return result;
}

bool JTimeTest::NormalTest()
{
    cout << "---------------------------------------\n";
    cout << "     File JTime.h starting...\n";    
    bool all_pass = RunFunction(TestJTime(), "JTime class")
        && RunFunction(TestJDateTime(), "JDateTime class");
    cout << "     File JTime.h " << (all_pass? "PASS" : "FAILED") << "\n";
    cout << "---------------------------------------\n";
    return all_pass;
}

bool JTimeTest::TestJTime()
{
    // test construction function
    JTime time(8, 5, 30, 50);
    string str = time.ToString();
    wstring wstr = time.ToWString();
    if( str != "08:05:30.050" && wstr != L"08:05:30.050")
        return false;

    JTime time_less(8, 5, 30, 49);
    if( time_less >= time || time <= time_less || time_less > time || time < time_less)
        return false;

    return true;
}

bool JTimeTest::TestJDateTime()
{
    string str = "2013/10/05 21:12:18.439";
    wstring wstr = L"2013/10/05 21:12:18.438";

    if(JDateTime(str).ToString() != str || JDateTime(wstr).ToWString() != wstr || JDateTime(str) == JDateTime(wstr)
        || JDateTime(str) <= JDateTime(wstr) || JDateTime(wstr) >= JDateTime(str))
        return false;

    JDateTime time1;
    JDateTime time2;
    str = time1.ToNumberString();
    wstr = time2.ToNumberWString();
    if( str != time2.ToNumberString() || wstr != time1.ToNumberWString() || time1 != time2 || time1 > time2
        || time1 < time2 || time1 - time2 != 0)
        return false;

    return true;
}