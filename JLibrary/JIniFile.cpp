// Copyright (c) 2012, Luoxizhi. All rights reserved.
//
// History:
// Date         Reference       Person          Descriptions
// ---------- 	-------------- 	--------------  ---------------------------
// 2012/03/01                   Luoxizhi        Initial Implementation
//
//-------------------------------------------------------------------------
#include "stdafx.h"
#include "JIniFile.h"
#include "JUtils.h"

#pragma comment(lib, "Kernel32.lib")
#define BufferSize	65536

JIniFile::JIniFile(wstring fileName) :m_fileName(fileName)
{	
}

JIniFile::~JIniFile()
{
}

wstring JIniFile::ReadString(const wstring strSection, const wstring strKey, const wstring strDefault)
{
	WCHAR data[BufferSize];
	::GetPrivateProfileStringW(strSection.c_str(),  strKey.c_str(), strDefault.c_str(), data, BufferSize, m_fileName.c_str());
	return wstring(data);
}

void JIniFile::WriteString(const wstring strSection, const wstring strKey, const wstring strValue)
{
	::WritePrivateProfileStringW(strSection.c_str(), strKey.c_str(), strValue.c_str(), m_fileName.c_str());
}

void JIniFile::ReadSection(const wstring strSection, map<wstring, wstring>& section)
{
	section.clear();
	TCHAR buffer[BufferSize];
	ZeroMemory(buffer, BufferSize*sizeof(TCHAR));
	::GetPrivateProfileSectionW(strSection.c_str(), buffer, BufferSize, m_fileName.c_str());

	wstring line, key, val;
	size_t pos;
	TCHAR *p = buffer;
	while( (*p) != L'\0'){
		line = p;
		pos = line.find(L"=");
		if( pos >= 0 ){
			key = line.substr(0, pos);
			val = line.substr(pos+1, line.length());
		}else{
			key = line;
			val = L"";
		}
		if( !key.empty() ){
			section[JStringUtils::TrimString(key)] = JStringUtils::TrimString(val);
		}
		p += (line.size()+1);
	}
}