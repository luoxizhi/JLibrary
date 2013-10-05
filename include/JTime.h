// Copyright (c) 2013, Luoxizhi. All rights reserved.
//
// History:
// Date         Reference       Person          Descriptions
// ---------- 	-------------- 	--------------  ---------------------------
// 2013/10/01                   Luoxizhi        Initial Implementation
//
//-------------------------------------------------------------------------
#pragma once

#ifndef JTime_H
#define JTime_H

#include "ATLComTime.h"
#include "JString.h"

#define JDateTime_Sec			(1000)
#define JDateTime_Minute_Sec	(1000*60)
#define JDateTime_Hour_Sec		(1000*60*60)
#define JDateTime_Day_Sec		(1000*60*60*24)

class JTime{
public:
	JTime(short hour, short minute, short second, short milisecond)
        :m_hour(hour), m_minute(minute), m_second(second), m_milisecond(milisecond){}
	JTime(const JTime& time){ 
        m_hour = time.m_hour; 
        m_minute = time.m_minute; 
        m_second = time.m_second; 
        m_milisecond = time.m_milisecond; 
    }
	JTime(wstring time);
    JTime(string time);
	~JTime(){}

	JTime& operator = (const JTime& rightVlule){
		m_hour = rightVlule.m_hour; 
        m_minute = rightVlule.m_minute; 
        m_second = rightVlule.m_second; 
        m_milisecond = rightVlule.m_milisecond;
		return *this;
	}
	bool operator > (const JTime& rightValue){
		__int64 v1 = m_hour*JDateTime_Hour_Sec + m_minute*JDateTime_Minute_Sec + m_second*JDateTime_Sec + m_milisecond;
		__int64 v2 = rightValue.m_hour*JDateTime_Hour_Sec + rightValue.m_minute*JDateTime_Minute_Sec 
                     + rightValue.m_second*JDateTime_Sec + rightValue.m_milisecond;
		return v1 > v2;
	}
	bool operator < (const JTime& rightValue){
		__int64 v1 = m_hour*JDateTime_Hour_Sec + m_minute*JDateTime_Minute_Sec + m_second*JDateTime_Sec + m_milisecond;
		__int64 v2 = rightValue.m_hour*JDateTime_Hour_Sec + rightValue.m_minute*JDateTime_Minute_Sec 
                     + rightValue.m_second*JDateTime_Sec + rightValue.m_milisecond;
		return v1 < v2;
	}
	wstring ToWString();
    string  ToString();

public:
	short m_hour;
	short m_minute;
	short m_second;
	short m_milisecond;
};

class JDateTime
{
	// property declare
public:
	__declspec(property(get=GetYear,        put=SetYear))       unsigned int Year;
	__declspec(property(get=GetMonth,       put=SetMonth))      unsigned int Month;
	__declspec(property(get=GetDay,         put=SetDay))        unsigned int Day;
	__declspec(property(get=GetHour,        put=SetHour))       unsigned int Hour;
	__declspec(property(get=GetMinute,      put=SetMinute))     unsigned int Minute;
	__declspec(property(get=GetSecond,      put=SetSecond))     unsigned int Second;
	__declspec(property(get=GetMSec,        put=SetMSec))       unsigned int MSec;
	__declspec(property(get=GetTime,		put=SetTime))		JTime Time;

	unsigned int GetYear();
	unsigned int GetMonth();
	unsigned int GetDay();
	unsigned int GetHour();
	unsigned int GetMinute();
	unsigned int GetSecond();
	unsigned int GetMSec();
	JTime		 GetTime();
	void SetYear(unsigned int year);
	void SetMonth(unsigned int month);
	void SetDay(unsigned int day);
	void SetHour(unsigned int hour);
	void SetMinute(unsigned int minute);
	void SetSecond(unsigned int second);
	void SetMSec(unsigned int milisecond);
	void SetTime(JTime time);

public:
	JDateTime(void);
	JDateTime(wstring time);
    JDateTime(string time);
	JDateTime(SYSTEMTIME& time);
	JDateTime(COleDateTime& time);
	JDateTime(FILETIME time);
	~JDateTime(void);

    //%04d/%02d/%02d %02d:%02d:%02d.%03d
	string  ToString(void); 
    wstring ToWString(void);
    //%04d%02d%02d%02d%02d%02d
	string  ToNumberString(void);
    wstring ToNumberWString(void);
	enum { DataTimeStringLength = 23 };

    static JDateTime Now();

public:
	__int64		operator -  (const JDateTime& rightValue);
	bool		operator >  (const JDateTime& rightValue);
	bool		operator >= (const JDateTime& rightValue);
	bool		operator <  (const JDateTime& rightValue);
	bool		operator == (const JDateTime& rightValue);
	bool		operator != (const JDateTime& rightValue);
	JDateTime&	operator =  (const JDateTime& rightValue);
	JDateTime&	operator =  (const SYSTEMTIME& rightValue);
	JDateTime&	operator =  (const COleDateTime& rightValue);
	JDateTime&	operator =  (const FILETIME& rightValue);

private:
	SYSTEMTIME m_time;
};

#endif