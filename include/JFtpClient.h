// Copyright (c) 2012, Luoxizhi. All rights reserved.
//
// History:
// Date         Reference       Person          Descriptions
// ---------- 	-------------- 	--------------  ---------------------------
// 2012/03/01                   Luoxizhi        Initial Implementation
//
//-------------------------------------------------------------------------
#pragma once
#include "JSocket.h"

#ifndef JFtpClient_H
#define JFtpClient_H
/************************************************************************/
/* JFtpCtrlSocket                                                       */
/************************************************************************/
class JFtpCtrlSocket
{
public:
	__declspec(property(get=GetIP,		put=SetIP))		string	IP;
	__declspec(property(get=GetPort,	put=SetPort))	short	Port;
	__declspec(property(get=GetConnected))				bool	IsConnected;
	string	GetIP(){ return m_IP; }
	void	SetIP(string ip){ m_IP = ip; }
	short	GetPort(){ return m_Port; }
	void	SetPort(short port){ m_Port = port; }
	bool	GetConnected();

public:
	JFtpCtrlSocket();
	~JFtpCtrlSocket();
	void	Open();
	void	Close();
	void	SendCommand(string command);
	bool	GetResponse(vector<string>& replyVec, long& retCode, string& msg, long timeout);
	void	ClearReadBuffer();
	void	SendData(const char* buffer, long bufferSize);
	bool	IsDataArrived(long millSeconds = 0);
	string	GetLocalIP();

private:
	SOCKET	m_sock;
	string	m_IP;
	short	m_Port;
};

/************************************************************************/
/* CFtpClient                                                           */
/************************************************************************/
class JFtpClient
{
public:
	__declspec(property(get=GetIP,				put=SetIP))					string	IP;
	__declspec(property(get=GetPort,			put=SetPort))				short	Port;
	__declspec(property(get=GetResponseTimeout, put=SetResponseTimeout))	long	ResponseTimeout;
	__declspec(property(get=GetUser,			put=SetUser))				string	User;
	__declspec(property(get=GetPassword,		put=SetPassword))			string	Password;
	__declspec(property(get=GetPassiveMode,		put=SetPassiveMode))		bool	PassiveMode;
	__declspec(property(get=GetDataSockConnect))							bool	IsDataSockConnect;
	__declspec(property(get=GetCtrlSockConnect))							bool	IsCtrlSockConnect;
	string	GetIP(){ return m_IP; }
	void	SetIP(string ip){ m_IP = ip; }
	short	GetPort(){ return m_Port; }
	void	SetPort(short port){ m_Port = port; }
	long	GetResponseTimeout(){ return m_ResponseTimeout; }
	void	SetResponseTimeout(long timeout){ m_ResponseTimeout = timeout; }
	string	GetUser(){ return m_User; }
	void	SetUser(string user){ m_User = user; }
	string	GetPassword(){ return m_Password; }
	void	SetPassword(string pwd){ m_Password = pwd; }
	bool	GetPassiveMode(){ return m_PassiveMode; }
	void	SetPassiveMode(bool flag){ m_PassiveMode = flag; }
	bool    GetDataSockConnect(){ return m_dataSock.IsConnected; }
	bool	GetCtrlSockConnect(){ return m_ctrlSock.IsConnected; }

public:
	JFtpClient();
	~JFtpClient();

	void	Close();
	bool	Login(string user, string pwd);
	bool	Login();
	bool	IsConnected();

	bool	MakeDir(string dirName, bool bCreateFullPath);
	bool	RemoveDir(string dirName);
	bool	DelFile(string fileName);
	bool	DelFiles(vector<string> fileVec);

	bool	UploadFile(string localFileName, string serverFileName, bool bBinary = true, bool bAutoCreatePath = false);
	bool	DownloadFile(string serverFileName, string localFileName, bool bBinary = true, bool bAutoCreatePath = false);
	bool	Dir(string serverPath, vector<string>& folderList, vector<string>& fileList);
	bool	IsFileExist(string serverFileName);
	bool	Rename(string serverFileNameFrom, string serverFileNameTo);
	bool	ChangeDir(string serverPath);
	bool	Restart(long index);

private:
	bool	Login(long& rtCode, string& msg);
	void	Close(long& rtCode, string& msg);
	bool	MakeDir(string dirName, bool bCreateFullPath, long& rtCode, string& msg);
	bool	RemoveDir(string dirName, long& rtCode, string& msg);
	bool	DelFile(string fileName, long& rtCode, string& msg);
	bool	UploadFile(string localFileName, string serverFileName, bool bBinary, bool bAutoCreatePath, long& rtCode, string& msg);
	bool	DownloadFile(string serverFileName, string localFileName, bool bBinary, bool bAutoCreatePath, long& rtCode, string& msg);
	bool	OpenDataSocket(bool bBinaryMode);
	bool	Dir(string serverPath, vector<string>& folderList, vector<string>& fileList, long& rtCode, string& msg);	
	bool	IsFileExist(string serverFileName, long& rtCode, string& msg);
	bool	Rename(string serverFileNameFrom, string serverFileNameTo, long& rtCode, string& msg);
	bool	ChangeDir(string serverPath, long& rtCode, string& msg);
	bool	Restart(long index, long& rtCode, string& msg);
	
private:
	string				m_IP;
	short				m_Port;
	long				m_ResponseTimeout;
	string				m_User;
	string				m_Password;
	bool				m_PassiveMode;
	JFtpCtrlSocket		m_ctrlSock;
	JBlockTcpSocket		m_dataSock;
};

#endif // JFtpClient_H