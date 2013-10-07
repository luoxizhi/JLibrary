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
#include <string>
using std::wstring;

#ifndef JMemStream_H
#define JMemStream_H

class JMemStream
{
public:
	__declspec(property(get=GetSize,		put=SetSize))		long Size;
	__declspec(property(get=GetPosition,	put=SetPosition))	long Position;
	long GetSize(){ return m_size; }
	void SetSize(long size);
	long GetPosition(){ return m_lPostion; }
	void SetPosition(long size);

public:
	JMemStream();
	~JMemStream();

	enum PosType{ Begin = 0, Current, End };
	void	Clear();
	long	Read(char *buffer, long count);
	long	Copy(char *buffer, long count);
	long	Write(const char *buffer, long count);
	long	Seek(long offset, PosType type);
	char*	GetBuffer();
	void	LoadFromFile(wstring fileName);
	void	SaveToFile(wstring fileName);

private:
	long	m_lPostion;
	char*	m_buffer;
	long	m_size;
	long	m_realSize;
	JCriticalSection	m_cs;
};

#endif