// Copyright (c) 2012, Luoxizhi. All rights reserved.
//
// History:
// Date         Reference       Person          Descriptions
// ---------- 	-------------- 	--------------  ---------------------------
// 2012/03/01                   Luoxizhi        Initial Implementation
//
//-------------------------------------------------------------------------
#pragma once
#ifndef JLogFile_H
#define JLogFile_H

#include "JSyncObject.h"
#include "JThread.h"
#include "JUtils.h"
#include <list>
#include <string>
using std::wstring;
using std::list;

enum JLogNameAppendType{ atNone = 0, atDateTime, atDay, atMonth, atYear };
class JLogFile
{
public:
	__declspec(property(get=GetLogFileName,		put=SetLogFileName))	wstring	LogFileName;
	__declspec(property(put=SetLogNameAppendType))	JLogNameAppendType	LogNameAppendType;	
	__declspec(property(put=SetFileSizeLimit))		long				LogFileSizeLimit;
	__declspec(property(put=SetFileTotalSizeLimit))	__int64				LogFileTotalSizeLimit;
	__declspec(property(put=SetOutDays))			long				OutDateDays;
	__declspec(property(put=SetDelOutDaysLog))		bool				DelOutDateDaysLog;
	__declspec(property(put=SetLogCathe))			bool				LogCathe;
	__declspec(property(put=SetCreateSubFolder))	bool				CreateSubFolder; // Make sure set this value before LogFileName, otherwise it is invalid.
	__declspec(property(put=SetLogHeader))			wstring				LogHeader;		// It is better to set this value before LogFileName.
	wstring	GetLogFileName(){ return m_FileName; }
	void SetLogNameAppendType(JLogNameAppendType type){ m_LogNameAppendType = type; }
	void SetLogFileName(wstring fileName);
	void SetFileSizeLimit(long size){ m_FileSize = size; }
	void SetFileTotalSizeLimit(__int64 size){ m_fileTotalSize = size; }
	void SetOutDays(long days){ m_OutDateDays = days; }
	void SetDelOutDaysLog(bool flag){ m_DelOutDateDaysLog = flag; }
	void SetLogCathe(bool flag){ m_LogCathe = flag; }
	void SetCreateSubFolder(bool flag){ m_CreateSubFolder = flag; }
	void SetLogHeader(wstring header);

private:
	JLogNameAppendType	m_LogNameAppendType;
	wstring				m_FileName;
	long				m_FileSize;
	long				m_OutDateDays;
	bool				m_DelOutDateDaysLog;
	bool				m_LogCathe;
	bool				m_CreateSubFolder;
	wstring				m_LogHeader;
	__int64				m_fileTotalSize;

public:
	JLogFile();
	~JLogFile();
	void	WriteLog(wstring logText);
	void	WriteLog(const WCHAR* buffer, long bufferSize);
	void	WriteLog(list<wstring> logList);
	void	SaveToIni(wstring iniFileName, wstring iniSectionName);
	void	LoadFromIni(wstring iniFileName, wstring iniSectionName);

private:
	void	WriteLogAction();
	void	CheckFileName();
	void	DelOutFileLogs();
	wstring	GetFileNameAppendText(JDateTime dt);

private:
	JTimer				m_timerDelFiles;
	JTimer				m_timerWriteLog;
	JCriticalSection	m_cs;
	list<LogData>		m_logList;
	wstring				m_currentFileName;
	JDateTime			m_timeLastWriteFile;
	JDateTime			m_timeConfirmFileName;
};

#endif