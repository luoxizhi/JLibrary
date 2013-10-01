// Copyright (c) 2013, Luoxizhi. All rights reserved.
//
// History:
// Date         Reference       Person          Descriptions
// ---------- 	-------------- 	--------------  ---------------------------
// 2013/10/01                   Luoxizhi        Initial Implementation
//
//-------------------------------------------------------------------------
#pragma once

#ifndef JFile_H
#define JFile_H

#include "JTime.h"
#include <string>
#include <vector>
#include <list>
using std::wstring;
using std::vector;
using std::list;

class JFileUtils
{
public:
    // file & directory
    static void		    ForceCreateDirectory(const wstring path);
    static bool			IsFileExist(const wstring fileName);
    static bool			IsDirExist(const wstring dirName);
    static JDateTime	GetFileCreateTime(const wstring fileName);
    static JDateTime	GetFileModifyTime(const wstring fileName);
    static JDateTime	GetFileAccessTime(const wstring fileName);
    static long			GetFileSize(const wstring fileName);
    static wstring		SuffixFileName(wstring fileName, wstring suffix = _T("bk"));
    static wstring		ExtractFilePath(const wstring fileName);
    static wstring		ExtractFileDir(const wstring fileName);
    static wstring		ExtractFileName(const wstring fileName);
    static wstring		ExtractFileExt(const wstring fileName);
    static wstring		ExtractFileNameNoExt(const wstring fileName);
    static wstring		ModifyFileName(wstring fileName, wstring newFileName);
    static wstring		GetExeName();
    static wstring		GetFileVersion();
    static wstring		GetFileVersion(wstring fileName);    

    // file operation
    template<class T> static bool ReadFile(const wstring fileName, vector<T>& fileContent)
    {
        T filestring;
        if( GetStringFromFile(fileName, filestring) ){
            JStringUtils::StringSplit<T>(filestring, _T("\n"), fileContent, false);
            return true;
        }        
        return false;
    }

    template<class T> static T ReadFile(const wstring fileName)
    {
        T filestring;
        GetStringFromFile(fileName, filestring);
        return filestring;
    }

    template<class T> static bool GetStringFromFile(const wstring fileName, T& fileContent)
    {
        fileContent.clear();
        if( !IsFileExist(fileName) ){
            return false;
        }

        long size = GetFileSize(fileName);
        HANDLE file_handle = ::CreateFileW(fileName.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if( file_handle == INVALID_HANDLE_VALUE ){
            return false;
        }else{
            char* buffer = NULL;
            DWORD bytes_read;
            buffer = new char[size+1];
            if( buffer ){
                ::ReadFile(file_handle, buffer, size, &bytes_read, NULL);
                buffer[size] = _T('\0');
                fileContent = buffer;
                delete buffer;
            }
            CloseHandle(file_handle);
        }
        return true;
    }

    static void     WriteFile(const wstring fileName, vector<wstring>& fileContent, wstring suffix = _T("\r\n"));
    static void     WriteFile(const wstring fileName, wstring& fileContent);
    static void     AppendFile(const wstring fileName, vector<wstring>& fileContent, wstring suffix = _T("\r\n"));
    static void     AppendFile(const wstring fileName, wstring fileContent, wstring suffix = _T("\r\n"));
    static void     AppendFile2File(const wstring fileName, wstring contentFileName);
    static void		AppendData2File(const wstring fileName, char* buffer, long length);
    static void		CopyFolder2Folder(wstring srcDirName, wstring tarDirName);
    static void		CopyFile2Folder(const vector<wstring>& vecFiles, wstring tarDirName);
    static void		CopyFile2File(const wstring srcFileName, const wstring tarFileName);
    static void		MoveFolder2Folder(wstring srcDirName, wstring tarDirName);
    static void		MoveFile2Folder(const vector<wstring>& vecFiles, wstring tarDirName);
    static void		MoveFile2File(const wstring srcFileName, const wstring tarFileName);
    static bool		IsWordsInFile(const wstring fileName, const wstring keyword);
    static bool		HasKeyWord(const wstring content, const wstring keyword);
    static void		DelFile(const wstring fileName);

    // directory operation	
    static void			GetFileNameList(wstring dirName, vector<wstring>& fileList);
    static void			GetFileNameList(wstring dirName, vector<wstring>& fileList, wstring keyword);
    static void			GetFileNameList(wstring dirName, list<wstring>& fileList, bool bSubDir = false);	// not clear output parameter fileList
    static void			GetDirNameList(wstring dirName, vector<wstring>& dirList);
    static void			DelOutDateFile(wstring dirName, __int64 seconds, bool bSubDir = true);	// search files by time limit, then delete them
    static void			DelOutSizeFile(wstring dirName, __int64 totalSize, bool bSubDir = true); // calculate total size of directory, if out limit, delete the files by time ASC.
    static void			DelDir(wstring dirName, bool keepFolder=false);
    static void			DelDir(wstring dirName, wstring timeout);
    static wstring		MakeDirNameValid(wstring& dirName); // clear { "\\", "/", ":", "*", "?", "\"", "<", ">", "|" }
};

#endif