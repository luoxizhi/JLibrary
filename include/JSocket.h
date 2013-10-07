// Copyright (c) 2012, Luoxizhi. All rights reserved.
//
// History:
// Date         Reference       Person          Descriptions
// ---------- 	-------------- 	--------------  ---------------------------
// 2012/03/01                   Luoxizhi        Initial Implementation
//
//-------------------------------------------------------------------------
#pragma once

#ifndef JSocket_H
#define JSocket_H

#include "JThread.h"
#include "winsock2.h"
#include "JFunction.h"
#include "JLogFile.h"
#include "JUtils.h"
#include <map>
#include <string>
using std::string;
using std::map;

/************************************************************************/
/* JPeerSocket: use for listener mode                                   */
/************************************************************************/
class JPeerSocket : public JThread
{
public:
	__declspec(property(get=GetConnected))	bool IsConnected;
	bool	GetConnected(){ return m_bConnected; }

public:
	JPeerSocket(SOCKET sock);
	virtual ~JPeerSocket();
	void	Open();
	void	Close();
	JThreeArguFunction<ULONG, const char*, long> OnRecvData;
	void	SendData(const char*, long);

private:
	JPeerSocket();
	void	Execute();
	bool	IsDataArrived(long millSeconds = 0);

private:
	SOCKET		m_socket;
	bool		m_bConnected;
};

/************************************************************************/
/* JSingleSocket  unblock type	support caller & listener mode          */
/************************************************************************/
class JTcpSocket : public JThread
{
public:
	__declspec(property(get=GetIP,			put=SetIP))				string	IP;
	__declspec(property(get=GetPeerIP))								string	IPFrom;
	__declspec(property(get=GetPort,		put=SetPort))			long	Port;
	__declspec(property(get=GetListener,	put=SetListener))		bool	IsListener;
	__declspec(property(get=GetOpen))								bool	IsOpen;
	__declspec(property(get=GetConnected))							bool	IsConnected;
	__declspec(property(get=GetAutoPort))							long	AutoPort;
	string	GetIP(){ return m_ip; }
	void	SetIP(string ip){ m_ip = ip; }
	bool	GetListener(){ return m_bListener; }
	void	SetListener(bool flag){ m_bListener = flag; }
	bool	GetOpen(){ return m_bOpen; }
	long	GetPort(){ return m_port; }
	void	SetPort(short port){ m_port = port; }
	bool	GetConnected(){ return m_bConnected; }
	long	GetAutoPort(){ return m_portAuto; }
	string	GetPeerIp(){ return m_ipFrom; }

public:
	JTcpSocket();
	virtual ~JTcpSocket();
	void	Open();
	void	Close();
	void	Reopen();
	void	SendData(const char* buffer, long bufferSize);
	string	GetLocalIP();
	JTwoArguFunction<const char*, long> OnRecvData;	

private:
	bool	IsDataArrived(long millSeconds = 0);	
	void	Execute();
	void	ReadData(ULONG threadId, const char* buffer, long bufferSize);

private:
	enum JSocketState{ JIdle, JListener,JListening, JCaller, JCallFailed, JConnected };
	SOCKET					m_socket;
	JPeerSocket*			m_sockAccept;
	string					m_ip;
	string					m_ipFrom;
	short					m_port;
	long					m_portAuto;
	bool					m_bOpen;
	bool					m_bConnected;
	bool					m_bListener;
	JSocketState			m_sockState;
	JCriticalSection		m_cs;
};

/************************************************************************/
/* JBlockTcpSocket                                                      */
/************************************************************************/
class JBlockTcpSocket: public JThread
{
public:
	__declspec(property(get=GetIP,			put=SetIP))				string	IP;
	__declspec(property(get=GetPort,		put=SetPort))			long	Port;
	__declspec(property(get=GetListener,	put=SetListener))		bool	Listener;
	__declspec(property(get=GetOpen))								bool	IsOpen;
	__declspec(property(get=GetConnected))							bool	IsConnected;
	__declspec(property(get=GetAutoPort))							USHORT	AutoPort;
	string	GetIP(){ return m_ip; }
	void	SetIP(string ip){ m_ip = ip; }
	bool	GetListener(){ return m_bListener; }
	void	SetListener(bool flag){ m_bListener = flag; }
	bool	GetOpen(){ return m_bOpen; }
	long	GetPort(){ return m_port; }
	void	SetPort(short port){ m_port = port; }
	bool	GetConnected(){ return m_bConnected; }
	USHORT	GetAutoPort(){ return m_portAuto; }

public:
	JBlockTcpSocket();
	~JBlockTcpSocket();
	void	Open();
	void	Close();
	void	SendData(const char* buffer, long bufferSize);
	long	ReadData(char* buffer, long bufferSize, long timeout);
	bool	IsDataArrived(long millSeconds = 0);
	string	GetLocalIP();

private:
	void	Execute();	
	void	Reopen();

private:
	enum JSocketState{ JIdle, JListener,JListening, JCaller, JCallFailed, JConnected };
	SOCKET					m_socket;
	SOCKET					m_sockAccept;
	string					m_ip;
	USHORT					m_port;
	USHORT					m_portAuto;
	bool					m_bOpen;
	bool					m_bConnected;
	bool					m_bListener;
	JSocketState			m_sockState;
	JCriticalSection		m_cs;
};

/************************************************************************/
/* JTcpServer                                                           */
/************************************************************************/
class JTcpServer: public JThread
{
public:
	__declspec(property(get=GetAutoPort,	put=SetAutoPort))	long	PortAuto;
	__declspec(property(get=GetOpen))							bool	IsOpen;
	__declspec(property(get=GetListening))						bool	IsListening;
	bool GetListening(){ m_sockState == JListening ? true: false; }

public:
	JTcpServer();
	~JTcpServer();	
	void		Open();
	void		Close();
	void		SendData(ULONG threadId, const char* buffer, long bufferSize);
	string		GetLocalIP();
	void		ReadData(ULONG threadId, const char* buffer, long bufferSize);
	JThreeArguFunction<ULONG, const char*, long> OnRecvData;

private:
	void		Reopen();
	void		Execute();	

private:
	enum JSocketState{ JIdle, JListener, JListening };
	SOCKET						m_socket;
	map<ULONG, JPeerSocket*>	m_sockAcpMap;
	string						m_ip;
	short						m_port;
	long						m_portAuto;
	bool						m_bOpen;
	JSocketState				m_sockState;
	JCriticalSection			m_cs;
};

/************************************************************************/
/* JUdpSocket                                                           */
/************************************************************************/
class JUdpSocket : public JThread
{
public:
	__declspec(property(get=GetOpen))							bool	IsOpen;
	__declspec(property(get=GetRemoteIp,	put=SetRemoteIp))	string	IP;
	__declspec(property(get=GetRemotePort,	put=SetRemoteIp))	long	PortRemote;
	__declspec(property(get=GetLocalPort,	put=SetLocalIp))	long	PortLocal;
	bool	GetOpen(){ return m_bOpen; }
	string	GetRemoteIp(){ return m_IpRemote; }
	void	SetRemoteIp(string ip);
	long	GetRemotePort(){ return m_PortRemote; }
	void	SetRemotePort(USHORT port);
	long	GetLocalPort(){ return m_PortLocal; }
	void	SetLocalPort(USHORT port);

public:
	JUdpSocket();
	~JUdpSocket();	
	JTwoArguFunction<const char*, long> OnRecvData;
	void	SendData(const char *buffer, long bufferSize);
	void	Open();
	void	Close();

private:
	bool	IsDataArrived(long millSeconds = 0);
	void	ReadData(const char* buffer, long bufferSize);
	void	Execute();

private:
	bool		m_bOpen;
	bool		m_bLocalBinded;
	bool		m_bRemoteBinded;
	string		m_IpRemote;
	USHORT		m_PortRemote;
	USHORT		m_PortLocal;
	sockaddr_in m_SockAddrRemote;
	SOCKET		m_SockLocal;
};

#endif