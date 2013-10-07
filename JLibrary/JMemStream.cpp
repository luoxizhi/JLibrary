// Copyright (c) 2012, Luoxizhi. All rights reserved.
//
// History:
// Date         Reference       Person          Descriptions
// ---------- 	-------------- 	--------------  ---------------------------
// 2012/03/01                   Luoxizhi        Initial Implementation
//
//-------------------------------------------------------------------------
#include "stdafx.h"
#include "JMemStream.h"
#include "JFile.h"

#define JMemStreamDefaultSize (65536) 

JMemStream::JMemStream()
: m_buffer(NULL)
, m_realSize(JMemStreamDefaultSize)
, m_size(0)
, m_lPostion(0)
{
	m_buffer = new char[m_realSize];
}

JMemStream::~JMemStream()
{
	if( m_buffer ){
		try
		{
			delete m_buffer;
		}
		catch(...){}
		m_buffer = NULL;
	}
}

void JMemStream::Clear()
{
	m_cs.Enter();
	if( m_buffer ){
		delete m_buffer;
		m_buffer = NULL;
	}
	m_realSize = JMemStreamDefaultSize;
	m_size = 0;
	m_buffer = new char[m_realSize];
	m_lPostion = 0;
	m_cs.Leave();
}

long JMemStream::Read(char *buffer, long count)
{
	if( buffer == NULL || m_buffer == NULL || count <= 0 ){
		return 0;
	}
	long readLen;
	m_cs.Enter();
	long available = m_size - m_lPostion;
	if( count >= available ){
		readLen = available;
	}else{
		readLen = count;
	}
	memcpy(buffer, m_buffer+m_lPostion, readLen);
	m_lPostion += readLen;
	m_cs.Leave();
	return readLen;
}

long JMemStream::Copy(char *buffer, long count)
{
	if( buffer == NULL || m_buffer == NULL || count <= 0 ){
		return 0;
	}
	long readLen;
	m_cs.Enter();
	long available = m_size - m_lPostion;
	if( count >= available ){
		readLen = available;
	}else{
		readLen = count;
	}
	memcpy(buffer, m_buffer+m_lPostion, readLen);
	m_cs.Leave();
	return readLen;
}

long JMemStream::Write(const char *buffer, long count)
{
	if( buffer == NULL || m_buffer == NULL || count <= 0 ){
		return 0;
	}
	m_cs.Enter();
	long available = m_realSize - m_lPostion;
	if( available >= count ){
		memcpy(m_buffer+m_lPostion, buffer, count);
		m_lPostion += count;
		if( m_lPostion > m_size ){
			m_size = m_lPostion;
		}
	}else{
		long scaler = ((m_lPostion + count)/JMemStreamDefaultSize) + 1;
		long newRealSize = JMemStreamDefaultSize * scaler;
		char *newBuffer = new char[newRealSize];
		if( newBuffer != NULL ){
			memcpy(newBuffer, m_buffer, m_lPostion);
			memcpy(newBuffer+m_lPostion, buffer, count);
			m_lPostion += count;
			m_size = m_lPostion;
			m_realSize = newRealSize;
			delete m_buffer;
			m_buffer = newBuffer;
		}
	}
	m_cs.Leave();
	return count;
}

long JMemStream::Seek(long offset, PosType type)
{
	m_cs.Enter();
	if( type == Begin ){
		if( offset <= 0 ){
			m_lPostion = 0;
		}else{
			m_lPostion = (offset < m_size) ? offset : m_size;
		}
	}else if( type == Current ){
		if( offset >= 0 ){
			if( m_lPostion + offset >= m_size ){
				m_lPostion = m_size;
			}else{
				m_lPostion += offset;
			}
		}else{
			if( m_lPostion + offset >= 0 ){
				m_lPostion += offset;
			}else{
				m_lPostion = 0;
			}
		}
	}else if( type == End ){
		if( offset >= 0 ){
			m_lPostion = m_size;
		}else{
			if( (m_size + offset) >= 0){
				m_lPostion = m_size + offset;
			}else{
				m_lPostion = 0;
			}			
		}
	}
	m_cs.Leave();
	return m_lPostion;
}

char* JMemStream::GetBuffer()
{
	return m_buffer;
}

void JMemStream::LoadFromFile(wstring fileName)
{
	if( !JFileUtils::IsFileExist(fileName) ){
		return;
	}
	HANDLE handle = ::CreateFileW(fileName.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if( handle == INVALID_HANDLE_VALUE ){
		return;
	}
	long fileSize = JFileUtils::GetFileSize(fileName);
	if( fileSize <= 0 ){
		return;
	}	
	m_cs.Enter();
	if( m_buffer ){
		delete m_buffer;
		m_buffer = NULL;
	}
	long scaler = (fileSize/JMemStreamDefaultSize) + 1;
	long newRealSize = scaler*JMemStreamDefaultSize;
	if( newRealSize > m_realSize ){
		m_buffer = new char[newRealSize];
		if( m_buffer == NULL ){
			return;
		}
		m_realSize = newRealSize;
	}
	ULONG readBytes;
	::ReadFile(handle, m_buffer, fileSize, &readBytes, NULL);
	m_lPostion = 0;
	m_size = fileSize;
	m_cs.Leave();
	::CloseHandle(handle);
}

void JMemStream::SaveToFile(wstring fileName)
{
	JFileUtils::ForceCreateDirectory(JFileUtils::ExtractFilePath(fileName));
	HANDLE handle = ::CreateFileW(fileName.c_str(), GENERIC_READ|GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if( handle == INVALID_HANDLE_VALUE ){
		return;
	}
	ULONG writeBytes;
	::WriteFile(handle, m_buffer, m_size, &writeBytes, NULL);
	::CloseHandle(handle);
}

