// Copyright (c) 2012, Luoxizhi. All rights reserved.
//
// History:
// Date         Reference       Person          Descriptions
// ---------- 	-------------- 	--------------  ---------------------------
// 2012/03/01                   Luoxizhi        Initial Implementation
//
//-------------------------------------------------------------------------
#include "stdafx.h"
#include "JThread.h"
#include "windows.h"
#pragma comment(lib, "Kernel32.lib")

/************************************************************************/
/* JThread                                                              */
/************************************************************************/
JThread::JThread() 
: m_handle(NULL)
, m_bSuspend(true)
, m_threadId(NULL)
, m_bTerminate(false)
, m_bEnd(false)
{
	m_handle = ::CreateThread(NULL, 0, &(JThread::ExecuteHandler), (PVOID)this, CREATE_SUSPENDED, &m_threadId);
}

JThread::~JThread()
{
	if( m_handle ){
		WaitForThreadEnd();
		::CloseHandle(m_handle);
		m_handle = NULL;
	}
}

void JThread::Resume()
{
	m_csThread.Enter();
	if( m_bSuspend ){
		::ResumeThread(m_handle);
		m_bSuspend = false;
	}
	m_csThread.Leave();
}

void JThread::Suspend()
{
	m_csThread.Enter();
	if( !m_bSuspend ){
		::SuspendThread(m_handle);
		m_bSuspend = true;
	}	
	m_csThread.Leave();
}

void JThread::Terminate()
{
	m_bTerminate = true;
}

unsigned long __stdcall JThread::ExecuteHandler(PVOID param)
{
	JThread* pThread = (JThread*)param;
	if( pThread ){
		pThread->Execute();
	}
	if( !pThread->m_bEnd ){
		pThread->m_bEnd = true;
	}
	return 0;
}

void JThread::WaitForThreadEnd()
{
	Resume();
	Terminate();
	__int64 starttime = ::GetTickCount();
	while(1){
		if(m_bEnd){
			break;
		}
		__int64 now = ::GetTickCount();
		if( now < starttime ){
			starttime = now;
		}
		if( (now - starttime) > 100 ){
			break;
		}
	}
	if( !m_bEnd ){
		for(long i = 0; i < 2000; i++){
			if( m_bEnd ){
				break;
			}
			Sleep(1);
		}
	}
	m_bEnd = true;
}

/************************************************************************/
/* JTimer                                                               */
/************************************************************************/
JTimer::JTimer()
: m_enable(false)
, m_interval(1000)
{
	Resume();
}

JTimer::~JTimer()
{
	m_event.SetEvent();
	WaitForThreadEnd();
}

void JTimer::Execute()
{
	while(1)
	{
		if( GetTerminateFlag() ){
			break;
		}

		if( m_interval <= 0 ){
			Sleep(100);
			continue;
		}

		if( !m_enable ){
			Sleep(100);
			continue;
		}

		m_event.WaitFor(m_interval);

		if( GetTerminateFlag() ){
			break;
		}

		if( m_interval <= 0 ){
			Sleep(100);
			continue;
		}

		if( !m_enable ){
			Sleep(100);
			continue;
		}
			
		if( OnTimer ){
			OnTimer();
		}
	}
}