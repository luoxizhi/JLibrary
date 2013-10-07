// Copyright (c) 2012, Luoxizhi. All rights reserved.
//
// History:
// Date         Reference       Person          Descriptions
// ---------- 	-------------- 	--------------  ---------------------------
// 2012/03/01                   Luoxizhi        Initial Implementation
//
//-------------------------------------------------------------------------
#include "stdafx.h"
#include "JSyncObject.h"
#pragma comment(lib, "Kernel32.lib")

/************************************************************************/
/* JCriticalSection                                                     */
/************************************************************************/
JCriticalSection::JCriticalSection()
{
	::InitializeCriticalSection(&m_cs);
}

JCriticalSection::~JCriticalSection()
{
	::DeleteCriticalSection(&m_cs);
}

void JCriticalSection::Enter()
{
	::EnterCriticalSection(&m_cs);
}

void JCriticalSection::Leave()
{
	return ::LeaveCriticalSection(&m_cs);
}

/************************************************************************/
/* JEvent                                                               */
/************************************************************************/
JEvent::JEvent() : m_handle(NULL)
{
	Create(NULL, false, false, "");
}

JEvent::JEvent(const string evName)
{
	m_handle = ::OpenEventA(EVENT_ALL_ACCESS, false, evName.c_str());
}

JEvent::JEvent(LPSECURITY_ATTRIBUTES attr, bool bManSet, bool bInit, const string evName)
{
	Create(attr, bManSet, bInit, evName);
}

JEvent::~JEvent()
{
	::CloseHandle(m_handle);
	m_handle = NULL;
}

void JEvent::Create(LPSECURITY_ATTRIBUTES attr, bool bManSet, bool bInit, const string evName)
{
	if( m_handle ){
		::CloseHandle(m_handle);
	}
    m_handle = evName.empty()? ::CreateEventA(attr,bManSet, bInit, NULL) : ::CreateEventA(attr,bManSet, bInit, evName.c_str());
}

void JEvent::SetEvent()
{
	if( m_handle ){
		::SetEvent(m_handle);
	}
}

void JEvent::ResetEvent()
{
	if( m_handle ){
		::ResetEvent(m_handle);
	}
}

long JEvent::WaitFor(long timeout /* = INFINITE */)
{
	if( m_handle ){
		long rt = ::WaitForSingleObject(m_handle, timeout);
		switch( rt){
		case WAIT_OBJECT_0:		return JWRSignaled;		break;
		case WAIT_TIMEOUT:		return JWRTimeOut;		break;
		case WAIT_ABANDONED:	return JWRAbandoned;	break;
		default:				return JWRError;		break;
		}
	}
    return JWRAbandoned;
}

/************************************************************************/
/* JSemaphore                                                           */
/************************************************************************/
JSemaphore::JSemaphore() : m_handle(NULL)
{
	m_handle = ::CreateSemaphoreA(NULL, 1, 1, NULL);
}

JSemaphore::JSemaphore(const string spName)
{
	m_handle = ::OpenSemaphoreA(SEMAPHORE_ALL_ACCESS, false, spName.c_str());
}

JSemaphore::JSemaphore(LPSECURITY_ATTRIBUTES attr, long initCount, long maxCount, const string spName)
{
	Create(attr, initCount, maxCount, spName);
}

JSemaphore::~JSemaphore()
{
    ::CloseHandle(m_handle);
    m_handle = NULL;
}

void JSemaphore::Create(LPSECURITY_ATTRIBUTES attr, long initCount, long maxCount, const string spName)
{
	if( m_handle ){
		::CloseHandle(m_handle);
		m_handle = NULL;
	}
    m_handle = spName.empty()? ::CreateSemaphoreA(attr, initCount, maxCount, NULL) : ::CreateSemaphoreA(attr, initCount, maxCount, spName.c_str());
}

long JSemaphore::WaitFor(long timeout /* = INFINITE */)
{
	if( m_handle ){
		long rt = ::WaitForSingleObject(m_handle, timeout);	
		switch( rt){
		case WAIT_OBJECT_0:		return JWRSignaled;		break;
		case WAIT_TIMEOUT:		return JWRTimeOut;		break;
		case WAIT_ABANDONED:	return JWRAbandoned;	break;
		default:				return JWRError;		break;
		}
	}
    return JWRAbandoned;
}

void JSemaphore::Release(long spCount /* = 1 */)
{
	if( m_handle ){
		::ReleaseSemaphore(m_handle, spCount, NULL);
	}
}

/************************************************************************/
/* JMutex                                                               */
/************************************************************************/
JMutex::JMutex() :m_handle(NULL)
{
	m_handle = ::CreateMutex(NULL, false, NULL);
}

JMutex::JMutex(const string muName)
{
	m_handle = ::OpenMutexA(MUTEX_ALL_ACCESS, false, muName.c_str());
}

JMutex::JMutex(LPSECURITY_ATTRIBUTES attr, const string muName)
{
	Create(attr, muName);
}

JMutex::~JMutex()
{
	::CloseHandle(m_handle);
	m_handle = NULL;
}

void JMutex::Create(LPSECURITY_ATTRIBUTES attr, const string muName)
{
	if( m_handle ){
		::CloseHandle(m_handle);
		m_handle = NULL;
	}
    m_handle = muName.empty()? ::CreateMutexA(attr, false, NULL) : ::CreateMutexA(attr, false, muName.c_str());
}

long JMutex::WaitFor(long timeout /* = INFINITE */)
{
	if( m_handle ){
		long rt = ::WaitForSingleObject(m_handle, timeout);
		switch( rt){
		case WAIT_OBJECT_0:		return JWRSignaled;		break;
		case WAIT_TIMEOUT:		return JWRTimeOut;		break;
		case WAIT_ABANDONED:	return JWRAbandoned;	break;
		default:				return JWRError;		break;
		}
	}
    return JWRAbandoned;
}

void JMutex::Release()
{
	if( m_handle ){
		::ReleaseMutex(m_handle);
	}
}

