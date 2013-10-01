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
#include "io.h"
#include "JString.h"

#pragma comment(lib, "Version.lib")

void JFileUtils::ForceCreateDirectory(const wstring path)
{
    wstring parent = ExtractFileDir(path);
    if( parent == path ){
        return;
    }
    if( !IsDirExist(parent) ){
        ForceCreateDirectory(parent);
    }
    ::CreateDirectoryW(path.c_str(), NULL);
}

bool JFileUtils::IsFileExist(const wstring fileName)
{
    return _waccess(fileName.c_str(), 0) == 0;
}

bool JFileUtils::IsDirExist(const wstring dirName)
{
    return _waccess(dirName.c_str(), 0) == 0;
}

JDateTime JFileUtils::GetFileCreateTime(const wstring fileName)
{
    WIN32_FILE_ATTRIBUTE_DATA faData;
    FILETIME local_file_time;
    if( GetFileAttributesExW(fileName.c_str(), GetFileExInfoStandard, &faData) ){
        FileTimeToLocalFileTime(&faData.ftCreationTime, &local_file_time);
        return JDateTime(local_file_time);
    }
    return JDateTime();
}

JDateTime JFileUtils::GetFileModifyTime(const wstring fileName)
{
    WIN32_FILE_ATTRIBUTE_DATA faData;
    FILETIME local_file_time;

    if( GetFileAttributesExW(fileName.c_str(), GetFileExInfoStandard, &faData) ){
        FileTimeToLocalFileTime(&faData.ftLastWriteTime, &local_file_time);
        return JDateTime(local_file_time);
    }
    return JDateTime();
}

JDateTime JFileUtils::GetFileAccessTime(const wstring fileName)
{
    WIN32_FILE_ATTRIBUTE_DATA faData;
    FILETIME local_file_time;

    if( GetFileAttributesExW(fileName.c_str(), GetFileExInfoStandard, &faData) ){
        FileTimeToLocalFileTime(&faData.ftLastAccessTime, &local_file_time);
        return JDateTime(local_file_time);
    }
    return JDateTime();
}

long JFileUtils::GetFileSize(const wstring fileName)
{
    WIN32_FILE_ATTRIBUTE_DATA faData;
    if( GetFileAttributesExW(fileName.c_str(), GetFileExInfoStandard, &faData) ){
        return faData.nFileSizeLow;
    }
    return -1;
}

wstring JFileUtils::SuffixFileName(wstring fileName, wstring suffix)
{
    size_t pos = fileName.find_last_of(_T('\\'));
    if( pos > 0 ){
        fileName.insert(pos-1, suffix, 0, suffix.size());
    }else{
        fileName += suffix;
    }
    return fileName;
}

wstring JFileUtils::ExtractFilePath(const wstring fileName)
{
    wstring new_str;
    int pos1 = static_cast<int>(fileName.find_last_of(_T("\\")));
    int pos2 = static_cast<int>(fileName.find_last_of(_T("/")));
    if( pos1 >= 0 && pos2 >=0 ){
        if( pos1 > pos2 ){
            new_str = fileName.substr(0, pos1+1);
        }else{
            new_str = fileName.substr(0, pos2+1);
        }
    }else if( pos1 >= 0 ){
        new_str = fileName.substr(0, pos1+1);
    }else if( pos2 >= 0 ){
        new_str = fileName.substr(0, pos2+1);
    }else{
        new_str = _T("");
    }
    return new_str;
}

wstring JFileUtils::ExtractFileDir(const wstring fileName)
{
    wstring new_str;
    size_t pos1 = fileName.find_last_of(_T("\\"));
    size_t pos2 = fileName.find_last_of(_T("/"));

    if( pos1 >= 0 && pos2 >= 0 ){
        new_str = pos1 > pos2? fileName.substr(0, pos1) : fileName.substr(0, pos2);
    }else if( pos1 >= 0 ){
        new_str = fileName.substr(0, pos1);
    }else if( pos2 >= 0 ){
        new_str = fileName.substr(0, pos2);
    }else{
        new_str = _T("");
    }
    return new_str;
}

wstring JFileUtils::ExtractFileName(const wstring fileName)
{
    wstring new_str;
    int pos1 = static_cast<int>(fileName.find_last_of(_T("\\")));
    int pos2 = static_cast<int>(fileName.find_last_of(_T("/")));

    if( pos1 >= 0 && pos2 >=0 ){
        new_str = pos1 > pos2? fileName.substr(pos1+1, fileName.length()-pos1-1) 
                             : fileName.substr(pos2+1, fileName.length()-pos2-1);
    }else if( pos1 >= 0 ){
        new_str = fileName.substr(pos1+1, fileName.length()-pos1-1);
    }else if( pos2 >= 0 ){
        new_str = fileName.substr(pos2+1, fileName.length()-pos2-1);
    }else{
        new_str = fileName;
    }
    return new_str;
}

wstring JFileUtils::ExtractFileNameNoExt(const wstring fileName)
{
    wstring name = ExtractFileName(fileName);
    size_t pos = name.find_last_of(_T('.'));

    return pos != wstring::npos? name.substr(0, pos) : name;
}

wstring JFileUtils::ExtractFileExt(const wstring fileName)
{
    int pos = static_cast<int>(fileName.find_last_of(_T(".")));
    return pos >= 0? fileName.substr(pos, fileName.length()-pos) : _T("");
}

wstring JFileUtils::ModifyFileName(wstring fileName, wstring newFileName)
{
    wstring path = ExtractFilePath(fileName);
    wstring name = ExtractFileName(fileName);
    wstring ext = ExtractFileExt(fileName);
    size_t pos = name.find(_T('.'));

    if( pos != wstring::npos ){
        name = name.substr(0, pos);
        return path + newFileName + ext;
    }else{
        return path + newFileName;
    }				
}

wstring JFileUtils::GetExeName()
{
    WCHAR name[MAX_PATH+1];
    ::GetModuleFileNameW(NULL, name, MAX_PATH);

    return wstring(name);
}

wstring JFileUtils::GetFileVersion()
{
    return GetFileVersion(GetExeName());
}

wstring JFileUtils::GetFileVersion(wstring fileName)
{
    wstring verStr;
    DWORD info_size = ::GetFileVersionInfoSizeW(fileName.c_str(), &info_size);
    WCHAR *buffer = new WCHAR[info_size];

    if( (buffer != NULL) && ::GetFileVersionInfoW(fileName.c_str(), 0, info_size, buffer) ){
        VS_FIXEDFILEINFO *vs_info;
        UINT vs_info_size;

        if (::VerQueryValue(buffer, _T("\\"), (void**)&vs_info, &vs_info_size)){
            int major  = HIWORD(vs_info->dwFileVersionMS);
            int minor   = LOWORD(vs_info->dwFileVersionMS);
            int release = HIWORD(vs_info->dwFileVersionLS);
            int build   = LOWORD(vs_info->dwFileVersionLS);
            verStr = JStringUtils::ToWString<int>(major) 
                     + _T(".") + JStringUtils::ToWString<int>(minor) 
                     + _T(".") + JStringUtils::ToWString<int>(release) 
                     + _T(".") + JStringUtils::ToWString<int>(build);
        }
    }

    delete buffer;
    return verStr;
}

void JFileUtils::WriteFile(const wstring fileName, vector<wstring>& fileContent, wstring suffix)
{
    if( IsFileExist(fileName) ){
        ::DeleteFileW(fileName.c_str());
    }

    ForceCreateDirectory(ExtractFilePath(fileName));
    HANDLE file_handle = ::CreateFileW(fileName.c_str(), GENERIC_READ|GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if( file_handle != INVALID_HANDLE_VALUE ){
        wstring content;
        for( vector<wstring>::const_iterator it = fileContent.begin(); it != fileContent.end(); ++it ){
            content += (*it + suffix);
        }
        DWORD count;
        ::WriteFile(file_handle, content.c_str(), (DWORD)(content.size()*sizeof(WCHAR)), &count, NULL);
        CloseHandle(file_handle);
    }
}

void JFileUtils::WriteFile(const wstring fileName, wstring& fileContent)
{
    if( IsFileExist(fileName) ){
        ::DeleteFileW(fileName.c_str());
    }

    ForceCreateDirectory(ExtractFilePath(fileName));
    HANDLE file_handle = ::CreateFileW(fileName.c_str(), GENERIC_READ|GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if( file_handle != INVALID_HANDLE_VALUE ){
        DWORD count;
        ::WriteFile(file_handle, fileContent.c_str(), (DWORD)(fileContent.size()*sizeof(WCHAR)), &count, NULL);
        CloseHandle(file_handle);
    }
}

void JFileUtils::AppendFile(const wstring fileName, vector<wstring>& fileContent, wstring suffix)
{
    ForceCreateDirectory(ExtractFilePath(fileName));

    HANDLE file_handle = ::CreateFileW(fileName.c_str(), GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if( file_handle == INVALID_HANDLE_VALUE ){
        file_handle = ::CreateFileW(fileName.c_str(), GENERIC_READ|GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    }else{
        ::SetFilePointer(file_handle, 0, NULL, FILE_END);
    }
    if( file_handle >= 0 ){     
        wstring line;
        for( vector<wstring>::const_iterator it = fileContent.begin(); it != fileContent.end(); ++it ){
            DWORD write_count;
            line = *it + suffix;
            ::WriteFile(file_handle, line.c_str(), (DWORD)(line.size()*sizeof(WCHAR)), &write_count, NULL);
        }
        CloseHandle(file_handle);
    }
}

void JFileUtils::AppendFile(const wstring fileName, wstring fileContent, wstring suffix)
{
    wstring line;
    ForceCreateDirectory(ExtractFilePath(fileName));

    HANDLE file_handle = ::CreateFileW(fileName.c_str(), GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if( file_handle == INVALID_HANDLE_VALUE ){
        file_handle = ::CreateFileW(fileName.c_str(), GENERIC_READ|GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    }else{
        ::SetFilePointer(file_handle, 0, NULL, FILE_END);
    }
    if( file_handle >= 0 ){            
        DWORD write_count;
        line = fileContent + suffix;
        ::WriteFile(file_handle, line.c_str(), (DWORD)(line.size()*sizeof(WCHAR)), &write_count, NULL);
        CloseHandle(file_handle);
    }
}

void JFileUtils::AppendFile2File(const wstring fileName, wstring contentFileName)
{		
    HANDLE append_file_handle = ::CreateFileW(contentFileName.c_str(), GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if( append_file_handle != INVALID_HANDLE_VALUE ){
        ForceCreateDirectory(ExtractFilePath(fileName));
        HANDLE file_handle = ::CreateFileW(fileName.c_str(), GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if( file_handle == INVALID_HANDLE_VALUE ){
            file_handle = ::CreateFileW(fileName.c_str(), GENERIC_READ|GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        }else{
            ::SetFilePointer(file_handle, 0, NULL, FILE_END);
        }
        __int64 size = GetFileSize(contentFileName);
        char* buffer = new char[65535];
        if( buffer ){
            DWORD real_read_out, real_write_out;
            while( size > 0 ){
                if(::ReadFile(append_file_handle, buffer, 65535, &real_read_out, NULL)
                    && ::WriteFile(file_handle, buffer, real_read_out, &real_write_out, NULL) )
                {
                    size -= real_write_out;
                }
                else{
                    break;
                }
            }
            delete buffer;
        }
        ::CloseHandle(append_file_handle);
        ::CloseHandle(file_handle);
    }		
}

void JFileUtils::AppendData2File(const wstring fileName, char* buffer, long length)
{
    ForceCreateDirectory(ExtractFilePath(fileName));
    HANDLE file_handle = ::CreateFileW(fileName.c_str(), GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if( file_handle == INVALID_HANDLE_VALUE ){
        file_handle = ::CreateFileW(fileName.c_str(), GENERIC_READ|GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    }else{
        ::SetFilePointer(file_handle, 0, NULL, FILE_END);
    }
    DWORD real_write;
    ::WriteFile(file_handle, buffer, length*sizeof(char), &real_write, NULL);
    ::CloseHandle(file_handle);
}

void JFileUtils::CopyFolder2Folder(wstring srcDirName, wstring tarDirName)
{
    vector<wstring> fileList;
    GetFileNameList(srcDirName, fileList);
    CopyFile2Folder(fileList, tarDirName);
}

void JFileUtils::CopyFile2Folder(const vector<wstring>& vecFiles, wstring tarDirName)
{
    if( tarDirName[tarDirName.size()-1] != _T('\\') && tarDirName[tarDirName.size()-1] != _T('/') ){
        tarDirName += _T("\\");
    }
    wstring tarFileName;
    for( vector<wstring>::const_iterator it = vecFiles.begin(); it != vecFiles.end(); ++it ){
        tarFileName = tarDirName + ExtractFileName(*it);
        if( IsFileExist(tarFileName) == false ){	
            ::CopyFile(it->c_str(), tarFileName.c_str(), false);
        }
    }
}

void JFileUtils::CopyFile2File(const wstring srcFileName, const wstring tarFileName)
{
    if( JStringUtils::TrimString(srcFileName) == _T("") 
       || JStringUtils::TrimString(tarFileName) == _T("") ){
        return;
    }
    if( !IsFileExist(srcFileName) ){
        return;
    }
    ::CopyFileW(srcFileName.c_str(), tarFileName.c_str(), false);
}

void JFileUtils::MoveFolder2Folder(wstring srcDirName, wstring tarDirName)
{
    vector<wstring> fileList;
    GetFileNameList(srcDirName, fileList);
    MoveFile2Folder(fileList, tarDirName);
}

void JFileUtils::MoveFile2Folder(const vector<wstring>& vecFiles, wstring tarDirName)
{
    if( JStringUtils::TrimString(tarDirName) == _T("") ){
        return;
    }
    if( tarDirName[tarDirName.size()-1] != _T('\\') && tarDirName[tarDirName.size()-1] != _T('/') ){
        tarDirName += _T("\\");
    }
    wstring tarFileName;
    for( vector<wstring>::const_iterator it = vecFiles.begin(); it != vecFiles.end(); ++it ){
        tarFileName = tarDirName + ExtractFileName(*it);
        if( IsFileExist(tarFileName) == false ){	
            if( ::CopyFileW(it->c_str(), tarFileName.c_str(), false) ){
                DelFile(*it);
            }
        }
    }
}

void JFileUtils::MoveFile2File(const wstring srcFileName, const wstring tarFileName)
{
    if( JStringUtils::TrimString(srcFileName) == _T("") || JStringUtils::TrimString(tarFileName) == _T("") ){
        return;
    }
    if( !IsFileExist(srcFileName) ){
        return;
    }
    ::MoveFileW(srcFileName.c_str(), tarFileName.c_str());
}

/************************************************************************/
/* directory operation                                                  */
/************************************************************************/
void JFileUtils::GetFileNameList(wstring dirName, vector<wstring>& fileList)
{
    if( JStringUtils::TrimString(dirName) == _T("") ){
        return;
    }

    if( dirName[dirName.size()-1] != _T('\\') && dirName[dirName.size()-1] != _T('/') ){
        dirName += _T("\\");
    }
    fileList.clear();

    WIN32_FIND_DATAW sr;
    HANDLE handle = ::FindFirstFileW((dirName+_T("*")).c_str(), &sr);
    if( handle != INVALID_HANDLE_VALUE ){
        wstring fileName;
        do{
            fileName = sr.cFileName;
            if( !(sr.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ){
                fileList.push_back(dirName + sr.cFileName);
            }
        }while(FindNextFileW(handle, &sr));
        ::FindClose(handle);
    }
}

void JFileUtils::GetFileNameList(wstring dirName, vector<wstring>& fileList, wstring keyword)
{
    if( JStringUtils::TrimString(dirName) == _T("") ){
        return;
    }
    if( dirName[dirName.size()-1] != _T('\\') && dirName[dirName.size()-1] != _T('/') ){
        dirName += _T("\\");
    }
    fileList.clear();
    WIN32_FIND_DATAW sr;
    HANDLE handle = ::FindFirstFileW(( (dirName+_T("*")).c_str()), &sr);
    if( handle != INVALID_HANDLE_VALUE ){
        wstring fileName;
        do{
            fileName = sr.cFileName;
            if( !(sr.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ){
                if( HasKeyWord(sr.cFileName, keyword) ){
                    fileList.push_back(dirName + sr.cFileName);
                }					
            }
        }while(FindNextFileW(handle, &sr));
        ::FindClose(handle);
    }
}

void JFileUtils::GetFileNameList(wstring dirName, list<wstring>& fileList, bool bSubDir)
{
    if( JStringUtils::TrimString(dirName) == _T("") )
        return;

    if( dirName[dirName.size()-1] != _T('\\') && dirName[dirName.size()-1] != _T('/') ){
        dirName += _T("\\");
    }

    WIN32_FIND_DATAW sr;
    HANDLE handle = ::FindFirstFileW((dirName+_T("*")).c_str(), &sr);
    if( handle != INVALID_HANDLE_VALUE ){
        wstring fileName;
        do{
            fileName = sr.cFileName;
            if( !(sr.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ){
                fileList.push_back(dirName + sr.cFileName);
            }else{
                if( bSubDir && wstring(sr.cFileName) != _T(".") && wstring(sr.cFileName) != _T("..") )
                    GetFileNameList(dirName+sr.cFileName, fileList, true);
            }
        }while(FindNextFileW(handle, &sr));
        ::FindClose(handle);
    }
}

void JFileUtils::GetDirNameList(wstring dirName, vector<wstring>& dirList)
{
    dirList.clear();
    if( !dirName.empty() && dirName[dirName.size()-1] != _T('\\') && dirName[dirName.size()-1] != _T('/') ){
        dirName += _T("\\");
    }
    WIN32_FIND_DATAW sr;
    HANDLE handle = ::FindFirstFileW((dirName+_T("*")).c_str(), &sr);
    if( handle != INVALID_HANDLE_VALUE )
    {
        wstring fileName;
        do
        {
            fileName = sr.cFileName;
            if( (sr.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && fileName != _T(".") && fileName != _T("..") ){
                dirList.push_back(dirName + sr.cFileName);
            }
        }while(FindNextFileW(handle, &sr));
        ::FindClose(handle);
    }
}

void JFileUtils::DelOutDateFile(wstring dirName, __int64 seconds, bool bSubDir)
{
    vector<wstring> fileList, dirList;
    GetFileNameList(dirName, fileList);
    for( vector<wstring>::iterator it = fileList.begin(); it != fileList.end(); ++it ){
        if( (JDateTime::Now() - GetFileModifyTime(*it)) > seconds ){
            DelFile(*it);
        }
    }
    if( bSubDir ){
        GetDirNameList(dirName, dirList);
        for( vector<wstring>::iterator it = dirList.begin(); it != dirList.end(); ++it ){
            DelOutDateFile(*it, seconds, bSubDir);
        }
    }		
    GetFileNameList(dirName, fileList);
    GetDirNameList(dirName, dirList);
    if( fileList.empty() && dirList.empty() ){
        ::RemoveDirectory(dirName.c_str());
    }
}

bool CompareFileModifyTimeAsc(wstring file1, wstring file2){
    return JFileUtils::GetFileModifyTime(file1) > JFileUtils::GetFileModifyTime(file2);
}
void JFileUtils::DelOutSizeFile(wstring dirName, __int64 totalSize, bool bSubDir /* = true */)
{
    if( totalSize == -1 ){
        return;
    }
    list<wstring> fileList;
    GetFileNameList(dirName, fileList, bSubDir);
    __int64 real_total_size = 0;
    for( list<wstring>::iterator it = fileList.begin(); it != fileList.end(); ++it ){
        real_total_size += GetFileSize(*it);
    }
    fileList.sort(CompareFileModifyTimeAsc);
    for( list<wstring>::iterator it = fileList.begin(); it != fileList.end(); ++it ){
        if( real_total_size < totalSize ){
            break;
        }
        real_total_size -= GetFileSize(*it);
        DelFile(*it);
    }
}

void JFileUtils::DelDir(wstring dirName, bool keepFolder)
{
    vector<wstring> dirList, fileList;
    GetFileNameList(dirName, fileList);
    for( vector<wstring>::iterator it = fileList.begin(); it != fileList.end(); ++it ){
        DelFile(*it);
    }
    GetDirNameList(dirName, dirList);
    for( vector<wstring>::iterator it = dirList.begin(); it != dirList.end(); ++it ){
        DelDir(*it, keepFolder);
    }
    if( !keepFolder ){
        ::RemoveDirectoryW(dirName.c_str());
    }
}

void JFileUtils::DelFile(const wstring fileName)
{
    ::DeleteFileW(fileName.c_str());
}

bool JFileUtils::HasKeyWord(const wstring content, const wstring keyword)
{
    return content.find(keyword, 0) != wstring::npos;
}

wstring JFileUtils::MakeDirNameValid(wstring& dirName)
{
    set<wstring> invalidSet;
    WCHAR* invalidChr[] = { _T("\\"), _T("/"), _T(":"), _T("*"), _T("?"), _T("\""), _T("<"), _T(">"), _T("|") };
    for( int i = 0; i < sizeof(invalidChr)/sizeof(invalidChr[0]); i++){
        invalidSet.insert(wstring(invalidChr[i]));
    }
    return JStringUtils::StringReplaceAll<wstring>(dirName, invalidSet, _T(""));
}