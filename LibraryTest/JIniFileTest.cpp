// Copyright (c) 2013, Luoxizhi. All rights reserved.
//
// History:
// Date         Reference       Person          Descriptions
// ---------- 	-------------- 	--------------  ---------------------------
// 2013/10/01                   Luoxizhi        Initial Implementation
//
//-------------------------------------------------------------------------
#include "stdafx.h"
#include "JIniFile.h"
#include "JIniFileTest.h"
#include <iostream>
using std::wcout;
using std::cout;

#ifdef _DEBUG
#pragma comment(lib, "..\\Debug\\JLibrary.lib")
#else
#pragma comment(lib, "..\\Release\\JLibrary.lib")
#endif

bool JIniFileTest::RunFunction(bool result, string strFunc)
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

bool JIniFileTest::NormalTest()
{
    cout << "---------------------------------------\n";
    cout << "     File JIniFile.h starting...\n";    
    bool all_pass = RunFunction(TestJIniFileClass(), "JIniFile class");
    cout << "     File JIniFile.h " << (all_pass? "PASS" : "FAILED") << "\n";
    cout << "---------------------------------------\n";
    return all_pass;
}

bool JIniFileTest::TestJIniFileClass()
{
    wstring wstr = L"c:\\sample.ini";
    JIniFile ini(wstr);
    ini.WriteString(L"config", L"keyword", L"hello world");
    wstring wstrData = ini.ReadString(L"config", L"keyword", L"");
    if( wstrData != L"hello world" )
        return false;

    ini.WriteString(L"config", L"setting", L"china");
    map<wstring, wstring> mapSection;
    ini.ReadSection(L"config", mapSection);
    if( mapSection[L"keyword"] != L"hello world" || mapSection[L"setting"] != L"china" )
        return false;

    return true;
}