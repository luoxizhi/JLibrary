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
                 && RunFunction(TestIsFileExist(), "JFileUtils::IsFileExist+ForceCreateDirectory+DelFile+ModifyFileName")
                 && RunFunction(TestExtractFilePath(), "JFileUtils::ExtractFilePath+ExtractFileDir+ExtractFileExt+ExtractFileName+ExtractFileNameNoExt")
                 && RunFunction(TestFileInfo(), "JFileUtils::GetExeName+GetFileSize+GetFileCreateTime+GetFileAccessTime+GetFileModifyTime+GetFileVersion")
                 && RunFunction(TestFileOperation(), "JFileUtils::WriteStringFile+ReadStringFile+AppendStringFile")
                 && RunFunction(TestFolderOperation(), "JFileUtils::MoveFile2Folder+CopyFile2Folder+MakeDirNameValid");
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
    JFileUtils::ModifyFileName(wstr, L"2");
    wstring newWStr = L"c:\\sample\\2.txt";
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

bool JFileTest::TestExtractFilePath()
{
    wstring wstr = L"c:\\中国\\1.txt";
    if( JFileUtils::ExtractFileDir(wstr) != L"c:\\中国" 
        && JFileUtils::ExtractFilePath(wstr) != L"c:\\中国\\" 
        && JFileUtils::ExtractFileExt(wstr) != L".txt"
        && JFileUtils::ExtractFileName(wstr) != L"1.txt"
        && JFileUtils::ExtractFileNameNoExt(wstr) != L"1" )
        return false;

    return true;
}

bool JFileTest::TestFileInfo()
{
    wstring wstr = JFileUtils::GetExeName();
    cout << "---------TestLibrary FileInfo-----------\n";
    wcout << wstr << L"\n";
    wcout << L"FileSize = " << JFileUtils::GetFileSize(wstr) << "\n";
    wcout << L"Create Time = " << JFileUtils::GetFileCreateTime(wstr).ToWString() << "\n";
    wcout << L"Access Time = " << JFileUtils::GetFileAccessTime(wstr).ToWString() << "\n";
    wcout << L"Modify Time = " << JFileUtils::GetFileModifyTime(wstr).ToWString() << "\n";
    wcout << L"File Version = " << JFileUtils::GetFileVersion(wstr) << "\n";
    return true;
}

bool JFileTest::TestFileOperation()
{
    wstring wstr = L"c:\\sample\\1.txt";
    wstring wstrContent = L"hello world";
    wstring wstrAppend = L", china";
    JFileUtils::WriteStringFile(wstr, wstrContent);
    if( JFileUtils::IsFileExist(wstr) == false )
        return false;

    wstring wstrRead = (WCHAR*)JFileUtils::ReadStringFile(wstr);
    if( wstrRead != wstrContent )
        return false;

    JFileUtils::AppendStringFile(wstr, wstrAppend);
    wstrRead = (WCHAR*)JFileUtils::ReadStringFile(wstr);
    if( wstrRead != wstrContent + wstrAppend )
        return false;

    JFileUtils::DelFile(wstr);

    return true;
}

bool JFileTest::TestFolderOperation()
{
    wstring wstrFolder = L"c:\\sample";
    wstring wstrNewFolder = L"c:\\sample two\\";
    wstring wstrFile = L"c:\\sample\\test\\new.txt";
    wstring wstrFileNew = L"c:\\sample two\\new.txt";

    JFileUtils::WriteStringFile(wstrFile, wstring(L"hello world"));
    JFileUtils::MoveFile2Folder(wstrFile, wstrNewFolder);
    if( JFileUtils::IsFileExist(wstrFileNew) == false || JFileUtils::IsFileExist(wstrFile) == true )
        return false;

    JFileUtils::CopyFile2Folder(wstrFileNew, wstrFolder+L"\\test");
    if( JFileUtils::IsFileExist(wstrFileNew) == false || JFileUtils::IsFileExist(wstrFile) == false )
        return false;

    JFileUtils::DelDir(wstrFolder);
    JFileUtils::DelDir(wstrNewFolder);
    if( JFileUtils::IsDirExist(wstrFolder) || JFileUtils::IsDirExist(wstrNewFolder) )
        return false;

    WCHAR invalidChr[] = { L'/', L':', L'*', L'?', L'"', L'<', L'>', L'|' };
    wstring invalidFolder = wstring(L"c:\\testinvalidfoldername\\")+invalidChr;
    JFileUtils::ForceCreateDirectory(invalidFolder);
    if( JFileUtils::IsDirExist(L"c:\\testinvalidfoldername\\") == false)
        return false;
    JFileUtils::DelDir(L"c:\\testinvalidfoldername\\");

    return true;
}
