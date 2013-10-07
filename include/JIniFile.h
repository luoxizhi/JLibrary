// Copyright (c) 2012, Luoxizhi. All rights reserved.
//
// History:
// Date         Reference       Person          Descriptions
// ---------- 	-------------- 	--------------  ---------------------------
// 2012/03/01                   Luoxizhi        Initial Implementation
//
//-------------------------------------------------------------------------
#pragma once
#ifndef JIniFile_H
#define JIniFile_H

#include "JString.h"
#include <map>
using std::map;

class JIniFile
{
public:
	JIniFile(wstring fileName);
	~JIniFile();

	wstring	ReadString(const wstring strSection, const wstring strKey, const wstring strDefault);
	void	WriteString(const wstring strSection, const wstring strKey, const wstring strValue);
	void	ReadSection(const wstring strSection, map<wstring, wstring>& section);

private:
	JIniFile(){}

private:
	wstring	m_fileName;
};

#endif