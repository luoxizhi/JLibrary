// Copyright (c) 2013, Luoxizhi. All rights reserved.
//
// History:
// Date         Reference       Person          Descriptions
// ---------- 	-------------- 	--------------  ---------------------------
// 2013/10/01                   Luoxizhi        Initial Implementation
//
//-------------------------------------------------------------------------
#include "stdafx.h"
#include "JFileTest.h"
#include "JFile.h"
#include "windows.h"
#include <iostream>
using std::wcout;
using std::cout;

#ifdef _DEBUG
#pragma comment(lib, "..\\Debug\\JLibrary.lib")
#else
#pragma comment(lib, "..\\Release\\JLibrary.lib")
#endif

bool JFileTest::RunFunction(bool result, string strFunc)
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

bool JFileTest::NormalTest()
{
    cout << "---------------------------------------\n";
    cout << "     File JFile.h starting...\n";    
    bool all_pass = RunFunction(TestDirectorySome(), "JFileUtils::ForceCreateDirectory+IsDirExist+DelDir")
                 && RunFunction(TestIsFileExist(), "JFileUtils::IsFileExist+ForceCreateDirectory+DelFile");
    cout << "     File JFile.h " << (all_pass? "PASS" : "FAILED") << "\n";
    cout << "---------------------------------------\n";
    return all_pass;
}

bool JFileTest::TestDirectorySome()
{
    wstring wstr = L"c:\\sample\\test4forcecreatedirectory\\";
    JFileUtils::ForceCreateDirectory(wstr);
    if( JFileUtils::IsDirExist(wstr) ==  false )
        return false;
    JFileUtils::DelDir(wstr);
    if( JFileUtils::IsDirExist(wstr) == true )
        return false;

    return true;
}

bool JFileTest::TestIsFileExist()
{
    wstring wstr = L"c:\\sample\\1.txt";
    JFileUtils::ForceCreateDirectory(JFileUtils::ExtractFileDir(wstr));
    CloseHandle(CreateFileW(wstr.c_str(), GENERIC_READ|GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL));
    if( JFileUtils::IsFileExist(wstr) == false )
        return false;
    
    JFileUtils::DelFile(wstr);
    if( JFileUtils::IsFileExist(wstr) == true )
        return false;

    JFileUtils::DelDir(JFileUtils::ExtractFileDir(wstr));

    return true;
}
