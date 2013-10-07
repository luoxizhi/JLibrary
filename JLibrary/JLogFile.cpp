// Copyright (c) 2012, Luoxizhi. All rights reserved.
//
// History:
// Date         Reference       Person          Descriptions
// ---------- 	-------------- 	--------------  ---------------------------
// 2012/03/01                   Luoxizhi        Initial Implementation
//
//-------------------------------------------------------------------------
#include "stdafx.h"
#include "JLogFile.h"
#include "JIniFile.h"
#include "JFile.h"
#include "JTime.h"
#include <algorithm>

#define DEFAULTFILESIZE 1024*1024*64

JLogFile::JLogFile()
: m_LogNameAppendType(atDateTime)
, m_FileSize(DEFAULTFILESIZE)
, m_fileTotalSize(DEFAULTFILESIZE)
, m_OutDateDays(7)
, m_DelOutDateDaysLog(false)
, m_LogCathe(true)
, m_CreateSubFolder(false)
{
	m_timerDelFiles.OnTimer.Attach<JLogFile>(this, &JLogFile::DelOutFileLogs);
	m_timerDelFiles.Interval = JDateTime_Sec*60;
	m_timerWriteLog.OnTimer.Attach<JLogFile>(this, &JLogFile::WriteLogAction);
	m_timerWriteLog.Interval = JDateTime_Sec;
}

JLogFile::~JLogFile()
{
}

wstring JLogFile::GetFileNameAppendText(JDateTime dt)
{
	WCHAR pathAppend[128] = {0};
	switch( m_LogNameAppendType ){
	case atDay:		swprintf_s(pathAppend, 127, L"%04d%02d%02d", dt.Year, dt.Month, dt.Day); break;
	case atMonth:	swprintf_s(pathAppend, 127, L"%04d%02d", dt.Year, dt.Month); break;
	case atYear:	swprintf_s(pathAppend, 127, L"%04d", dt.Year); break;
	default:		break;
	}
	return pathAppend;
}

void JLogFile::SetLogFileName(wstring fileName)
{
	m_cs.Enter();
	m_timeConfirmFileName = JDateTime::Now(); 
	wstring path_append_name = GetFileNameAppendText(m_timeConfirmFileName);
	wstring path = JFileUtils::ExtractFilePath(fileName);
	wstring ext = JFileUtils::ExtractFileExt(fileName);
	wstring name = JFileUtils::ExtractFileName(fileName);
	m_FileName = path+name+L".log";
	if( !path_append_name.empty() ){
		name += L"_";
	}
	m_currentFileName = path+name+path_append_name+L".log";
	if( JFileUtils::IsFileExist(m_currentFileName) ){
		if( JFileUtils::GetFileSize(m_currentFileName) < m_FileSize ){
			return;
		}else{
			long index = 1;
			while(index > 0){
				m_currentFileName = path+name+path_append_name+L"_"+JStringUtils::ToWString(index)+L".log";
				if( !JFileUtils::IsFileExist(m_currentFileName)){
					JFileUtils::AppendStringFile(m_currentFileName, m_LogHeader);
					break;
				}else{
					if( JFileUtils::GetFileSize(m_currentFileName) < m_FileSize ){
						break;
					}else{
						index++;
					}
				}
			}
		}
	}else{
		JFileUtils::AppendStringFile(m_currentFileName, m_LogHeader);
	}
	m_timerDelFiles.Enable = true;
	m_timerWriteLog.Enable = true;
	m_cs.Leave();
}

void JLogFile::WriteLog(const WCHAR* buffer, long bufferSize)
{
	WriteLog(wstring().append(buffer, bufferSize));
}

void JLogFile::WriteLog(wstring logText)
{		
	LogData data;
	data.dt = JDateTime::Now();
	data.text = logText;
	if( m_LogCathe ){
		m_cs.Enter();
		m_logList.push_back(data);
		m_cs.Leave();
	}else{
		CheckFileName();
		m_cs.Enter();
		JFileUtils::AppendStringFile(m_currentFileName, JLogUtils::LogData2String(data));
		m_cs.Leave();
	}	
}

void JLogFile::WriteLog(list<wstring> logList)
{	
	wstring logText;
	LogData data;
	vector<wstring> logVec;
	data.dt = JDateTime::Now();
	for ( list<wstring>::iterator it = logList.begin(); it != logList.end(); ++it ){
		data.text = *it;
		if( m_LogCathe ){
			m_cs.Enter();
			m_logList.push_back(data);
			m_cs.Leave();
		}else{
			logVec.push_back(JLogUtils::LogData2String(data));
		}
	}
	if( !m_LogCathe ){
		CheckFileName();
		m_cs.Enter();
        for(size_t i = 0; i < logVec.size(); i++)
		    JFileUtils::AppendStringFile(m_currentFileName, logVec[i]);
		m_cs.Leave();
	}	
}

void JLogFile::WriteLogAction()
{
	m_cs.Enter();
	vector<wstring> logVec;
	for ( list<LogData>::iterator it = m_logList.begin(); it != m_logList.end(); ++it ){
		logVec.push_back(JLogUtils::LogData2String(*it));
	}
	m_logList.clear();
	m_cs.Leave();
	CheckFileName();
	m_cs.Enter();
    for(size_t i = 0; i < logVec.size(); i++)
	    JFileUtils::AppendStringFile(m_currentFileName, logVec[i]);
	m_cs.Leave();
}

void JLogFile::CheckFileName()
{
	if( GetFileNameAppendText(JDateTime::Now()) != GetFileNameAppendText(m_timeConfirmFileName) 
        || JFileUtils::GetFileSize(m_currentFileName) >= m_FileSize )
    {
		SetLogFileName(m_FileName);
	}
}

void JLogFile::DelOutFileLogs()
{
    JFileUtils::DelOutDateFile(JFileUtils::ExtractFilePath(m_FileName), m_OutDateDays * JDateTime_Day_Sec);
    JFileUtils::DelOutSizeFile(JFileUtils::ExtractFilePath(m_FileName), m_fileTotalSize);
}

void JLogFile::SaveToIni(wstring iniFileName, wstring iniSectionName)
{
	JIniFile ini(iniFileName);
	ini.WriteString(iniSectionName, _T("LogFileName"), m_FileName);
	ini.WriteString(iniSectionName, _T("LogFileSize"), JStringUtils::ToWString(m_FileSize));
	ini.WriteString(iniSectionName, _T("LogNameAppendType"), JStringUtils::ToWString((long)m_LogNameAppendType));
	ini.WriteString(iniSectionName, _T("LogCathe"), m_LogCathe? L"1": L"0");
	ini.WriteString(iniSectionName, _T("LogCreateSubFolder"), m_CreateSubFolder? L"1": L"0");
	ini.WriteString(iniSectionName, _T("DelOutDateDaysLog"), m_DelOutDateDaysLog? L"1": L"0");
	ini.WriteString(iniSectionName, _T("LogOutDateDays"), JStringUtils::ToWString(m_OutDateDays));
}

void JLogFile::LoadFromIni(wstring iniFileName, wstring iniSectionName)
{
	JIniFile ini(iniFileName);
	m_FileName = ini.ReadString(iniSectionName, L"LogFileName", L"");
	m_FileSize = _wtol(ini.ReadString(iniSectionName, L"LogFileSize", L"1024*1024*64").c_str());
	m_LogNameAppendType = (JLogNameAppendType)_wtol(ini.ReadString(iniSectionName, L"LogNameAppendType", L"1").c_str());
	m_LogCathe = ini.ReadString(iniSectionName, L"LogCathe", L"1") == L"1"? true: false;
	m_CreateSubFolder = ini.ReadString(iniSectionName, L"LogCreateSubFolder", L"0") == L"1"? true: false;
	m_DelOutDateDaysLog = ini.ReadString(iniSectionName, L"DelOutDateDaysLog", L"1") == L"1"? true: false;
	m_OutDateDays = _wtol(ini.ReadString(iniSectionName, L"LogOutDateDays", L"7").c_str());
}

void JLogFile::SetLogHeader(wstring header)
{
	if( JFileUtils::IsFileExist(m_currentFileName) ){
		JFileUtils::AppendStringFile(m_currentFileName, header);
	}
}
