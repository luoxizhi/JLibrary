// Copyright (c) 2013, Luoxizhi. All rights reserved.
//
// History:
// Date         Reference       Person          Descriptions
// ---------- 	-------------- 	--------------  ---------------------------
// 2013/10/01                   Luoxizhi        Initial Implementation
//
//-------------------------------------------------------------------------
#include "stdafx.h"
#include "time.h"
#include "JTime.h"
#include "stdio.h"
#include <vector>
#include <set>
using std::vector;
using std::set;

/************************************************************************/
/* JTime                                                                */
/************************************************************************/
JTime::JTime(wstring time)
{
    set<wstring> tags;
    tags.insert(_T(":"));
    tags.insert(_T("."));
    vector<wstring> vecStr;
    JStringUtils::StringSplit(time, tags, vecStr);
    if( time.find(_T('.')) == wstring::npos ){
        if( vecStr.size() == 3 ){
            m_hour = _wtoi(vecStr[0].c_str());
            m_minute = _wtoi(vecStr[1].c_str());
            m_second = _wtoi(vecStr[2].c_str());
        }
    }else{
        if( vecStr.size() == 4 ){
            m_hour = _wtoi(vecStr[0].c_str());
            m_minute = _wtoi(vecStr[1].c_str());
            m_second = _wtoi(vecStr[2].c_str());
            m_milisecond = _wtoi(vecStr[3].c_str());
        }
    }
}

JTime::JTime(string time)
{
    set<string> tags;
    tags.insert(":");
    tags.insert(".");
    vector<string> vecStr;
    JStringUtils::StringSplit(time, tags, vecStr);
    if( time.find(_T('.')) == wstring::npos ){
        if( vecStr.size() == 3 ){
            m_hour = atoi(vecStr[0].c_str());
            m_minute = atoi(vecStr[1].c_str());
            m_second = atoi(vecStr[2].c_str());
        }
    }else{
        if( vecStr.size() == 4 ){
            m_hour = atoi(vecStr[0].c_str());
            m_minute = atoi(vecStr[1].c_str());
            m_second = atoi(vecStr[2].c_str());
            m_milisecond = atoi(vecStr[3].c_str());
        }
    }
}

string JTime::ToString()
{
    char dt[128] = {0};
    sprintf_s(dt, 127, "%02d:%02d:%02d.%03d", m_hour, m_minute, m_second, m_milisecond);
    return string(dt);
}

wstring JTime::ToWString()
{
    WCHAR dt[128] = {0};
    swprintf_s(dt, 127, _T("%02d:%02d:%02d.%03d"), m_hour, m_minute, m_second, m_milisecond);
    return wstring(dt);
}

/************************************************************************/
/* JDateTime                                                                */
/************************************************************************/

JDateTime::JDateTime(void)
{
    SYSTEMTIME time;
    GetLocalTime(&time);
    m_time = time;
}

JDateTime::JDateTime(wstring time)
{
    if( time.size() != DataTimeStringLength ){
        return;
    }

    m_time.wYear            = _wtoi(time.substr(0, 4).c_str());
    m_time.wMonth           = _wtoi(time.substr(5, 2).c_str());
    m_time.wDay             = _wtoi(time.substr(8, 2).c_str());
    m_time.wHour            = _wtoi(time.substr(11, 2).c_str());
    m_time.wMinute          = _wtoi(time.substr(14, 2).c_str());
    m_time.wSecond          = _wtoi(time.substr(17, 2).c_str());
    m_time.wMilliseconds    = _wtoi(time.substr(20).c_str());
}

JDateTime::JDateTime(string time)
{
    if( time.size() != DataTimeStringLength ){
        return;
    }

    m_time.wYear            = atoi(time.substr(0, 4).c_str());
    m_time.wMonth           = atoi(time.substr(5, 2).c_str());
    m_time.wDay             = atoi(time.substr(8, 2).c_str());
    m_time.wHour            = atoi(time.substr(11, 2).c_str());
    m_time.wMinute          = atoi(time.substr(14, 2).c_str());
    m_time.wSecond          = atoi(time.substr(17, 2).c_str());
    m_time.wMilliseconds    = atoi(time.substr(20).c_str());
}

JDateTime::JDateTime(SYSTEMTIME& time)
{
    m_time = time;
}

JDateTime::JDateTime(COleDateTime& time)
{
    time.GetAsSystemTime(m_time);
}

JDateTime::JDateTime(FILETIME time)
{
    FileTimeToSystemTime(&time, &m_time);	
}

JDateTime::~JDateTime(void)
{
}

// property define
unsigned int JDateTime::GetYear()	{ return m_time.wYear; }
unsigned int JDateTime::GetMonth()	{ return m_time.wMonth; }
unsigned int JDateTime::GetDay()	{ return m_time.wDay; }
unsigned int JDateTime::GetHour()	{ return m_time.wHour; }
unsigned int JDateTime::GetMinute()	{ return m_time.wMinute; }
unsigned int JDateTime::GetSecond() { return m_time.wSecond; }
unsigned int JDateTime::GetMSec()	{ return m_time.wMilliseconds; }
JTime JDateTime::GetTime()			{ return JTime(m_time.wHour, m_time.wMinute, m_time.wSecond, m_time.wMilliseconds); }
void JDateTime::SetYear(unsigned int year)		{ m_time.wYear = year; }
void JDateTime::SetMonth(unsigned int month)	{ m_time.wMonth = month; }
void JDateTime::SetDay(unsigned int day)		{ m_time.wDay = day; }
void JDateTime::SetHour(unsigned int hour)		{ m_time.wHour = hour;}
void JDateTime::SetMinute(unsigned int minute)	{ m_time.wMinute = minute; }
void JDateTime::SetSecond(unsigned int second)	{ m_time.wSecond = second; }
void JDateTime::SetMSec(unsigned int milisecond){ m_time.wMilliseconds = milisecond; }
void JDateTime::SetTime(JTime time)				{ m_time.wHour = time.m_hour;
m_time.wMinute = time.m_minute;
m_time.wSecond = time.m_second; 
m_time.wMilliseconds = time.m_milisecond; }
// property end

LONGLONG JDateTime::operator - (const JDateTime &rightValue)
{
    CTimeSpan span = CTime(m_time.wYear, m_time.wMonth, m_time.wDay, m_time.wHour, m_time.wMinute, m_time.wSecond) 
        - CTime(rightValue.m_time.wYear, rightValue.m_time.wMonth, rightValue.m_time.wDay, rightValue.m_time.wHour, rightValue.m_time.wMinute, rightValue.m_time.wSecond);
    __int64 nseconds = span.GetTotalSeconds();
    __int64 nMillSeconds = m_time.wMilliseconds - rightValue.m_time.wMilliseconds;
    return nseconds*1000 + nMillSeconds;
}

bool JDateTime::operator > (const JDateTime& rightValue)
{
    CTimeSpan span = CTime(m_time.wYear, m_time.wMonth, m_time.wDay, m_time.wHour, m_time.wMinute, m_time.wSecond) 
        - CTime(rightValue.m_time.wYear, rightValue.m_time.wMonth, rightValue.m_time.wDay, rightValue.m_time.wHour, rightValue.m_time.wMinute, rightValue.m_time.wSecond);
    __int64 nseconds = span.GetTotalSeconds();
    if( nseconds != 0 ){
        return nseconds > 0;
    }else{
        return (m_time.wMilliseconds - rightValue.m_time.wMilliseconds) > 0;
    }	
}

bool JDateTime::operator >= (const JDateTime& rightValue)
{
    CTimeSpan span = CTime(m_time.wYear, m_time.wMonth, m_time.wDay, m_time.wHour, m_time.wMinute, m_time.wSecond) 
        - CTime(rightValue.m_time.wYear, rightValue.m_time.wMonth, rightValue.m_time.wDay, rightValue.m_time.wHour, rightValue.m_time.wMinute, rightValue.m_time.wSecond);
    __int64 nseconds = span.GetTotalSeconds();
    if( nseconds != 0 ){
        return nseconds > 0;
    }else{
        return (m_time.wMilliseconds - rightValue.m_time.wMilliseconds) >= 0;
    }	
}

bool JDateTime::operator == (const JDateTime& rightValue)
{
    CTimeSpan span = CTime(m_time.wYear, m_time.wMonth, m_time.wDay, m_time.wHour, m_time.wMinute, m_time.wSecond) 
        - CTime(rightValue.m_time.wYear, rightValue.m_time.wMonth, rightValue.m_time.wDay, rightValue.m_time.wHour, rightValue.m_time.wMinute, rightValue.m_time.wSecond);
    return (span.GetTotalSeconds() == 0 && (m_time.wMilliseconds - rightValue.m_time.wMilliseconds) == 0);
}

bool JDateTime::operator != (const JDateTime& rightValue)
{
    CTimeSpan span = CTime(m_time.wYear, m_time.wMonth, m_time.wDay, m_time.wHour, m_time.wMinute, m_time.wSecond) 
        - CTime(rightValue.m_time.wYear, rightValue.m_time.wMonth, rightValue.m_time.wDay, rightValue.m_time.wHour, rightValue.m_time.wMinute, rightValue.m_time.wSecond);
    return (span.GetTotalSeconds() != 0 || (m_time.wMilliseconds - rightValue.m_time.wMilliseconds) != 0);
}

bool JDateTime::operator < (const JDateTime& rightValue)
{
    CTimeSpan span = CTime(m_time.wYear, m_time.wMonth, m_time.wDay, m_time.wHour, m_time.wMinute, m_time.wSecond) 
        - CTime(rightValue.m_time.wYear, rightValue.m_time.wMonth, rightValue.m_time.wDay, rightValue.m_time.wHour, rightValue.m_time.wMinute, rightValue.m_time.wSecond);
    __int64 nseconds = span.GetTotalSeconds();
    if( nseconds != 0 ){
        return nseconds < 0;
    }else{
        return (m_time.wMilliseconds - rightValue.m_time.wMilliseconds) < 0;
    }
}

JDateTime& JDateTime::operator = (const JDateTime& rightValue)
{
    m_time = rightValue.m_time;
    return *this;
}

JDateTime& JDateTime::operator = (const SYSTEMTIME& rightValue)
{
    m_time = rightValue;
    return *this;
}

JDateTime& JDateTime::operator = (const COleDateTime& rightValue)
{
    rightValue.GetAsSystemTime(m_time);
    return *this;
}

JDateTime& JDateTime::operator =(const FILETIME &rightValue)
{
    FileTimeToSystemTime(&rightValue, &m_time);
    return *this;
}

string JDateTime::ToString(void)
{	
    char dt[128] = {0};
    sprintf_s(dt, 127, "%04d/%02d/%02d %02d:%02d:%02d.%03d", m_time.wYear, m_time.wMonth, m_time.wDay, m_time.wHour, m_time.wMinute, m_time.wSecond, m_time.wMilliseconds);
    return string(dt);
}

string JDateTime::ToNumberString()
{
    char dt[128] = {0};
    sprintf_s(dt, 127, "%04d%02d%02d%02d%02d%02d", m_time.wYear, m_time.wMonth, m_time.wDay, m_time.wHour, m_time.wMinute, m_time.wSecond);
    return string(dt);
}

wstring JDateTime::ToWString(void)
{	
    WCHAR dt[128] = {0};
    swprintf_s(dt, 127, _T("%04d/%02d/%02d %02d:%02d:%02d.%03d"), m_time.wYear, m_time.wMonth, m_time.wDay, m_time.wHour, m_time.wMinute, m_time.wSecond, m_time.wMilliseconds);
    return wstring(dt);
}

wstring JDateTime::ToNumberWString()
{
    WCHAR dt[128] = {0};
    swprintf_s(dt, 127, _T("%04d%02d%02d%02d%02d%02d"), m_time.wYear, m_time.wMonth, m_time.wDay, m_time.wHour, m_time.wMinute, m_time.wSecond);
    return wstring(dt);
}

JDateTime JDateTime::Now()
{
    SYSTEMTIME time;
    GetLocalTime(&time);

    return JDateTime(time);
}
