// Copyright (c) 2012, Luoxizhi. All rights reserved.
//
// History:
// Date         Reference       Person          Descriptions
// ---------- 	-------------- 	--------------  ---------------------------
// 2012/03/01                   Luoxizhi        Initial Implementation
//
//-------------------------------------------------------------------------
#pragma once
#include "JSyncObject.h"
#include "JFunction.h"

#ifndef JThread_H
#define JThread_H

class JThread
{
public:
	__declspec(property(get=GetThreadId))					long ThreadId;
	__declspec(property(get=GetTerminateFlag))				bool TermFlag;
	__declspec(property(get=GetSuspend))					bool IsSuspend;
	long	GetThreadId(void){ return m_threadId; }
	bool	GetTerminateFlag(){ return m_bTerminate; }
	bool	GetSuspend(){ return m_bSuspend; }	

public:
	JThread();
	virtual ~JThread();
	void Suspend();
	void Resume();
	void Terminate();
	void WaitForThreadEnd();	
	virtual void Execute() = 0;

private:
	static unsigned long __stdcall ExecuteHandler(PVOID param);

private:
	HANDLE				m_handle;
	bool                m_bSuspend;
	JCriticalSection    m_csThread;
	ULONG				m_threadId;
	bool				m_bTerminate;
	bool				m_bEnd;
};

class JTimer : public JThread
{
public:
	__declspec(property(get=GetInterval,	put=SetInterval))	ULONG	Interval;
	__declspec(property(get=GetEnable,		put=SetEnable))		bool	Enable;
	ULONG GetInterval(){ return m_interval; }
	void SetInterval(ULONG interval){ m_interval = interval; m_event.SetEvent(); }
	bool GetEnable(){ return m_enable; }
	void SetEnable(bool flag){ m_enable = flag; }


public:
	JNullArguFunction OnTimer;

public:
	JTimer();
	~JTimer();

private:
	void Execute();

private:
	ULONG	m_interval;
	JEvent  m_event;
	bool	m_enable;
};

#endif // JThread_H