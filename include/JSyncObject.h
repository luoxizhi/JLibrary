// Copyright (c) 2012, Luoxizhi. All rights reserved.
//
// History:
// Date         Reference       Person          Descriptions
// ---------- 	-------------- 	--------------  ---------------------------
// 2012/03/01                   Luoxizhi        Initial Implementation
//
//-------------------------------------------------------------------------
#pragma once

#ifndef JSyncObject_H
#define JSyncObject_H

#include "windows.h"
#include <string>
using std::string;

#define JWRSignaled		0
#define JWRTimeOut		(JWRSignaled+1)
#define JWRAbandoned	(JWRTimeOut+1)
#define JWRError		(JWRAbandoned+1)

class JCriticalSection
{
public:
	JCriticalSection();
	virtual ~JCriticalSection();

	void	Enter(void);
	void	Leave(void);

private:
	CRITICAL_SECTION m_cs;
};

class JEvent
{
public:
	__declspec(property(get = GetHandle))	HANDLE Handle;
	HANDLE	GetHandle(){ return m_handle; }

public:
	JEvent();
	JEvent(const string evName);
	JEvent(LPSECURITY_ATTRIBUTES attr, bool bManSet, bool bInit, const string evName);
	virtual ~JEvent();

	void	Create(LPSECURITY_ATTRIBUTES attr, bool bManSet, bool bInit, const string evName);
	void	SetEvent(void);
	void	ResetEvent(void);
	long	WaitFor(long timeout = INFINITE);

private:
	HANDLE m_handle;
};

class JSemaphore
{
public:
	__declspec(property(get = GetHandle))	HANDLE Handle;
	HANDLE	GetHandle(){ return m_handle; }

public:
	JSemaphore();
	JSemaphore(const string spName);
	JSemaphore(LPSECURITY_ATTRIBUTES attr, long initCount, long maxCount, const string spName);
	virtual ~JSemaphore();

	void Create(LPSECURITY_ATTRIBUTES attr, long initCount, long maxCount, const string spName);
	void Release(long spCount = 1);
	long WaitFor(long timeout = INFINITE);

private:
	HANDLE m_handle;
};

class JMutex
{
public:
	__declspec(property(get = GetHandle))	HANDLE Handle;
	HANDLE	GetHandle(){ return m_handle; }

public:
	JMutex();
	JMutex(const string muName);
	JMutex(LPSECURITY_ATTRIBUTES attr, const string muName);
	virtual ~JMutex();

	void Create(LPSECURITY_ATTRIBUTES attr, const string muName);
	void Release(void);
	long WaitFor(long timeout = INFINITE);

private:
	HANDLE m_handle;
};

#endif // JSyncObject_H
