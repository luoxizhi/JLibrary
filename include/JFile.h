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
    static void		    ForceCreateDirectory(wstring path);
    static bool			IsFileExist(const wstring fileName);
    static bool			IsDirExist(const wstring dirName);
    static JDateTime	GetFileCreateTime(const wstring fileName);
    static JDateTime	GetFileModifyTime(const wstring fileName);
    static JDateTime	GetFileAccessTime(const wstring fileName);
    static ULONG		GetFileSize(const wstring fileName);
    static wstring		ExtractFilePath(wstring fileName);
    static wstring		ExtractFileDir(wstring fileName);
    static wstring		ExtractFileName(wstring fileName);
    static wstring		ExtractFileExt(wstring fileName);
    static wstring		ExtractFileNameNoExt(wstring fileName);
    static wstring		ModifyFileName(wstring fullFileName, wstring newFileNameNoExt);
    static wstring		GetExeName();
    static wstring		GetFileVersion();
    static wstring		GetFileVersion(wstring fileName);

    // file operation
    static char*        ReadStringFile(wstring fileName);   // make sure free the memory if return is not NULL
    static DWORD        WriteStringFile(const wstring fileName, wstring& fileContent);
    static DWORD        AppendStringFile(const wstring fileName, wstring& fileContent);

    static void		    CopyFile2Folder(wstring fileName, wstring tarDirName);
    static void		    MoveFile2Folder(wstring fileName, wstring tarDirName);
    static void		    DelFile(const wstring fileName);

    // directory operation	
    static void			GetFileNameList(wstring dirName, vector<wstring>& fileList);
    static void			GetFileNameList(wstring dirName, list<wstring>& fileList, bool bSubDir = false);	// not clear output parameter fileList
    static void			GetDirNameList(wstring dirName, vector<wstring>& dirList);
    static void			DelOutDateFile(wstring dirName, __int64 seconds, bool bSubDir = true);	// search files by time limit, then delete them
    static void			DelOutSizeFile(wstring dirName, __int64 totalSize, bool bSubDir = true); // calculate total size of directory, if out limit, delete the files by time ASC.
    static void			DelDir(wstring dirName, bool keepFolder=false);
    static void			DelDir(wstring dirName, wstring timeout);
    static wstring		MakeDirNameValid(wstring& dirName); // clear { "\\", "/", ":", "*", "?", "\"", "<", ">", "|" }
};

#endif