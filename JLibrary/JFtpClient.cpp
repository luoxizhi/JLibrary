// Copyright (c) 2012, Luoxizhi. All rights reserved.
//
// History:
// Date         Reference       Person          Descriptions
// ---------- 	-------------- 	--------------  ---------------------------
// 2012/03/01                   Luoxizhi        Initial Implementation
//
//-------------------------------------------------------------------------
#include "stdafx.h"
#include "JFtpClient.h"
#include "JUtils.h"
#include "JMemStream.h"
#include <string>
#include <set>
#include <vector>
using std::string;
using std::set;
using std::vector;

/************************************************************************/
/* JFtpCtrlSocket                                                       */
/************************************************************************/
JFtpCtrlSocket::JFtpCtrlSocket()
: m_Port(0)
, m_sock(NULL)
{
}

JFtpCtrlSocket::~JFtpCtrlSocket()
{
}

bool JFtpCtrlSocket::GetConnected()
{
	return (m_sock == NULL)? false : true;
}

void JFtpCtrlSocket::Open()
{
	m_sock = ::socket(AF_INET, SOCK_STREAM, 0);
	if( m_sock == INVALID_SOCKET || m_sock == NULL ){
		Close();
	}else{
		BOOL reuse_addr = TRUE;
		::setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse_addr, sizeof(int));
		int socket_size = 524288;
		setsockopt(m_sock, SOL_SOCKET, SO_RCVBUF, (char*)&socket_size, sizeof(int));
		setsockopt(m_sock, SOL_SOCKET, SO_SNDBUF, (char*)&socket_size, sizeof(int));

		sockaddr_in addr;
		HOSTENT *hostinfo;
		addr.sin_family = AF_INET;
		if( m_IP.empty() ){
			hostinfo = ::gethostbyname("127.0.0.1");
		}else{
			hostinfo = ::gethostbyname(m_IP.c_str());
		}

		if( hostinfo ){
			addr.sin_addr.s_addr = ((struct in_addr *)hostinfo->h_addr)->s_addr;
			addr.sin_port = htons(m_Port);
			if( ::connect( m_sock, (LPSOCKADDR)&addr, sizeof(addr) ) == SOCKET_ERROR ){
				Close();
			}
		}else{
			Close();
		}
	}
}

void JFtpCtrlSocket::Close()
{
	::shutdown(m_sock, SD_SEND);
	::closesocket(m_sock);
	m_sock = NULL;
}

void JFtpCtrlSocket::SendCommand(string command)
{
	ClearReadBuffer();
	command += "\r\n";
	SendData(command.c_str(), (long)command.length());
}

bool JFtpCtrlSocket::GetResponse(vector<string>& replyVec, long& retCode, string& msg, long timeout)
{
	retCode = 0;
	msg = "";
	replyVec.clear();

	if( m_sock == NULL ){
		return false;
	}

	fd_set fd;
	FD_ZERO(&fd);
	FD_SET((unsigned int)m_sock, &fd);

	struct timeval tmOut;
	tmOut.tv_sec = 0;
	tmOut.tv_usec = timeout*1000;
	if( ::select(FD_SETSIZE, &fd, NULL, NULL, &tmOut) == 0 ){
		return false;
	}

	int count;
	char inBuffer[65536];
	count = ::recv(m_sock, inBuffer, 65536, 0);
	if( count <= 0 ){
		Close();
		return false;
	}

	string reply;
	if( count > 0 ){
		inBuffer[count] = '\0';
		string str = Trim(inBuffer);
		size_t index = str.find("\n");
		if( index > 0 ){
			str = str.substr(0, index);
		}
		reply = str;
		replyVec.push_back(str);
	}

	if( replyVec.empty() ){
		return false;
	}

	if( reply.length() >= 5 ){
		string error_code = reply.substr(0, 3);
		retCode = atoi(error_code.c_str());
		msg = reply.substr(4);
	}

	if( reply.length() < 3 || ( reply[0] != '1' && reply[0] != '2' && reply[0] != '3' ) ){
		return false;
	}

	return true;
}

void JFtpCtrlSocket::ClearReadBuffer()
{
	fd_set fd;
	FD_ZERO(&fd);
	FD_SET((unsigned int)m_sock, &fd);
	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;
	int read_bytes;
	char in_buffer[65536];
	while( ::select(FD_SETSIZE, &fd, NULL, NULL, &timeout) > 0 )
	{
		read_bytes = ::recv(m_sock, in_buffer, 65536, 0);
		if( read_bytes <= 0 ){
			break;
		}	
	}
}

void JFtpCtrlSocket::SendData(const char* buffer, long bufferSize)
{
	if( m_sock ){
		int send_bytes = 0;
		int bytes;
		try
		{
			do
			{
				bytes = ::send( m_sock, buffer+send_bytes, bufferSize-send_bytes, 0);
				if( bytes <= 0 ){
					break;
				}
				send_bytes += bytes;
			}while( send_bytes < bufferSize );
		}
		catch(...){}
	}
}

bool JFtpCtrlSocket::IsDataArrived(long millSeconds /* = 0 */)
{
	bool result = false;
	fd_set fd;
	FD_ZERO(&fd);
	FD_SET((unsigned int)m_sock, &fd);
	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = millSeconds;
	if( ::select(FD_SETSIZE, &fd, NULL, NULL, &timeout) > 0 ){
		result = true;
	}
	return result;
}

string JFtpCtrlSocket::GetLocalIP()
{
	string localIp;
	sockaddr_in addr;
	int length = sizeof(SOCKADDR);
	if( m_sock ){
		try
		{
			if( ::getsockname(m_sock, (LPSOCKADDR)&addr, &length) != SOCKET_ERROR ){
				char ipStr[50];
				unsigned char *ip = (unsigned char *)&(addr.sin_addr);
				sprintf_s(ipStr, 50, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
				localIp = ipStr;
			}
		}
		catch(...){}
	}
	return localIp;
}

/************************************************************************/
/* JFtpClient                                                           */
/************************************************************************/
JFtpClient::JFtpClient()
: m_Port(21)
, m_IP("127.0.0.1")
, m_ResponseTimeout(5000)
, m_PassiveMode(false)
{
}

JFtpClient::~JFtpClient()
{
}

bool JFtpClient::IsConnected()
{
	return m_ctrlSock.IsConnected;
}

void JFtpClient::Close()
{
	string msg;
	long rtCode;
	Close(rtCode, msg);
}

void JFtpClient::Close(long& rtCode, string& msg)
{
	rtCode = 0;
	msg = "";
	vector<string> replyStrList;
	string reply;
	m_ctrlSock.SendCommand("quit");
	m_ctrlSock.GetResponse(replyStrList, rtCode, msg, m_ResponseTimeout);
	m_ctrlSock.Close();
	m_dataSock.Close();
}

bool JFtpClient::Login()
{
	string msg;
	long rtCode;
	return Login(rtCode, msg);
}

bool JFtpClient::Login(string user, string pwd)
{
	m_User = user;
	m_Password = pwd;
	return Login();
}

bool JFtpClient::Login(long& rtCode, string& msg)
{
	rtCode = 0;
	msg = "";
	vector<string> replyStrList;
	m_ctrlSock.Close();
	m_ctrlSock.IP = m_IP;
	m_ctrlSock.Port = m_Port;
	m_ctrlSock.Open();
	m_ctrlSock.GetResponse(replyStrList, rtCode, msg, m_ResponseTimeout); // ??

	string command, reply;
	for(int i = 0; i < 20; ++i ){
		if( IsConnected() ){
			break;
		}
		::Sleep(100);
	}
	if( IsConnected() )
	{
		replyStrList.clear();
		command = string("USER ") + m_User;
		m_ctrlSock.SendCommand(command);
		if( m_ctrlSock.GetResponse(replyStrList, rtCode, msg, m_ResponseTimeout) == false ){
			return false;
		}
		replyStrList.clear();
		command = string("PASS ") + m_Password;
		m_ctrlSock.SendCommand(command);
		if( m_ctrlSock.GetResponse(replyStrList, rtCode, msg, m_ResponseTimeout) == false ){
			return false;
		}
	}
	return true;
}

bool JFtpClient::MakeDir(string dirName, bool bCreateFullPath)
{
	string msg;
	long rtCode;
	return MakeDir(dirName, bCreateFullPath, rtCode, msg);
}

bool JFtpClient::MakeDir(string dirName, bool bCreateFullPath, long& rtCode, string& msg)
{
	rtCode = 0;
	msg = "";
	if( dirName.empty() ){
		return false;
	}
	bool result = true;
	vector<string> replyStrList;
	string command;
	if( bCreateFullPath ){
		set<string> tags;
		tags.insert("\\");
		tags.insert("/");
		string serverPath = dirName;
		vector<string> pathList;
		StringSplit(dirName, tags, pathList, true);
		if( !pathList.empty() )	{
			serverPath = "";
			vector<string>::iterator pos = pathList.begin();
			if( pos != pathList.end() ){
				const char* p = dirName.c_str();
				if( p[0] == '\\' || p[0] == '/' ){
					serverPath = string("/") + (*pos);
				}else{
					serverPath = (*pos);
				}
				replyStrList.clear();
				command = string("MKD ") + serverPath;
				m_ctrlSock.SendCommand(command);
				result = m_ctrlSock.GetResponse(replyStrList, rtCode, msg, m_ResponseTimeout);
				++pos;
				for( ; pos != pathList.end(); ++pos ){
					if( !pos->empty() ){
						serverPath += string("/") + (*pos);
						replyStrList.clear();
						command = string("MKD ") + serverPath;
						m_ctrlSock.SendCommand(command);
						result = m_ctrlSock.GetResponse(replyStrList, rtCode, msg, 6000);
					}
				}
			}
		}
	}else{
		replyStrList.clear();
		command = string("MKD ") + ExtractFileDir(dirName);
		m_ctrlSock.SendCommand(command);
		result = m_ctrlSock.GetResponse(replyStrList, rtCode, msg, m_ResponseTimeout);
	}
	return result;
}

bool JFtpClient::RemoveDir(string dirName)
{
	long rtCode;
	string msg;
	return RemoveDir(dirName, rtCode, msg);
}

bool JFtpClient::RemoveDir(string dirName, long& rtCode, string& msg)
{
	rtCode = 0;
	msg = "";
	if( dirName.empty() ){
		return false;
	}
	vector<string> replyStrList;
	string command = string("RMD ") + ExtractFileDir(dirName);
	m_ctrlSock.SendCommand(command);
	return m_ctrlSock.GetResponse(replyStrList, rtCode, msg, m_ResponseTimeout);
}

bool JFtpClient::DelFile(string fileName)
{
	long rtCode;
	string msg;
	return DelFile(fileName, rtCode, msg);
}

bool JFtpClient::DelFile(string fileName, long& rtCode, string& msg)
{
	if( fileName.empty() ){
		return false;
	}
	vector<string> replyStrList;
	string command;
	command = string("DELE ") + fileName;
	m_ctrlSock.SendCommand(command);
	return m_ctrlSock.GetResponse(replyStrList, rtCode, msg, m_ResponseTimeout);
}

bool JFtpClient::DelFiles(vector<string> fileVec)
{
	if( fileVec.empty() ){
		return false;
	}
	bool result = true;
	for (vector<string>::iterator it = fileVec.begin(); it != fileVec.end(); ++it){
		if( DelFile(*it) == false ){
			result = false;
		}
	}
	return result;
}

bool JFtpClient::OpenDataSocket(bool bBinaryMode)
{
	long rtCode;
	string msg;
	vector<string> replyStrList;
	string command, reply;
	if( IsConnected() ){
		replyStrList.clear();
		if( bBinaryMode ){
			command = "TYPE I";
		}else{
			command = "TYPE A";
		}
		m_ctrlSock.SendCommand(command);
		if( m_ctrlSock.GetResponse(replyStrList, rtCode, msg, m_ResponseTimeout) == false ){
			return false;
		}
	}

	if( m_PassiveMode ){
		replyStrList.clear();
		command = "PASV";
		m_ctrlSock.SendCommand(command);
		if( m_ctrlSock.GetResponse(replyStrList, rtCode, msg, m_ResponseTimeout) == false ){
			return false;
		}
		reply = replyStrList.front();
		size_t x1 = reply.find("(");
		size_t x2 = reply.find("("); // ??
		if( x1 >= 0 && x2 >= 0 ){
			string str = reply.substr(x1+1, x2-x1-1);
			vector<string> numbers;
			StringSplit(str, ",", numbers, true);
			if( numbers.size() < 6 ){
				return false;
			}
			USHORT port = (atoi(numbers[4].c_str()) << 8) + atoi(numbers[5].c_str());
			m_dataSock.Close();
			m_dataSock.Listener = false;
			m_dataSock.IP = m_IP;
			m_dataSock.Port = port;
			m_dataSock.Open();
		}
	}
	else
	{
		m_dataSock.Close();
		m_dataSock.Listener = true;
		m_dataSock.IP = "127.0.0.1";
		m_dataSock.Port = 0;
		m_dataSock.Open();
		USHORT port = m_dataSock.AutoPort;
		string ip = m_dataSock.GetLocalIP();
		string str = StringReplaceAll(ip, ".", ",");
		replyStrList.clear();
		command = string("PORT ") + str + "," + ToString(port>>8) + "," + ToString(port&0x00000FF);
		m_ctrlSock.SendCommand(command);
		if( m_ctrlSock.GetResponse(replyStrList, rtCode, msg, m_ResponseTimeout) == false ){
			return false;
		}
	}
	return true;
}

bool JFtpClient::UploadFile(string localFileName, string serverFileName, bool bBinary /* = true */, bool bAutoCreatePath /* = false */)
{
	long rtCode;
	string msg;
	return UploadFile(localFileName, serverFileName, bBinary, bAutoCreatePath, rtCode, msg);
}

bool JFtpClient::UploadFile(string localFileName, string serverFileName, bool bBinary, bool bAutoCreatePath, long &rtCode, std::string &msg)
{
	if( localFileName.empty() || serverFileName.empty() ){
		return false;
	}
	if( !OpenDataSocket(bBinary) ){
		return false;
	}

	if( bAutoCreatePath ){
		if( MakeDir(ExtractFilePath(serverFileName), true) == false ){
			return false;
		}		
	}
	string command = string("STOR ") + serverFileName;
	m_ctrlSock.SendCommand(command);

	JMemStream ms;
	ms.LoadFromFile(localFileName);
	char *buffer = ms.GetBuffer();
	int buffer_length = ms.Size;

	vector<string> replyStrList;
	bool result = m_ctrlSock.GetResponse(replyStrList, rtCode, msg, m_ResponseTimeout);

	__int64 diff, start_time;
	start_time = ::GetTickCount();
	while(1)
	{	
		if( m_dataSock.IsConnected ){
			break;
		}
		diff = ::GetTickCount() - start_time;
		if( diff < 0 ){
			start_time = 0;
		}
		if( diff > 1000 ){
			break;
		}
	}

	if( !m_dataSock.IsConnected ){
		return false;
	}else{
		m_dataSock.SendData(buffer, buffer_length);
	}
	m_dataSock.Close();

	replyStrList.clear();
	if( result = m_ctrlSock.GetResponse(replyStrList, rtCode, msg, m_ResponseTimeout) == false){
		return false;
	}
	return result;
};

bool JFtpClient::DownloadFile(string serverfileName, string localFileName, bool bBinary /* = true */, bool bAutoCreatePath /* = false */)
{
	long rtCode;
	string msg;
	return DownloadFile(serverfileName, localFileName, bBinary, bAutoCreatePath, rtCode, msg);
}

bool JFtpClient::DownloadFile(string serverFileName, string localFileName, bool bBinary, bool bAutoCreatePath, long& rtCode, string& msg)
{
	if( localFileName.empty() || serverFileName.empty() )
	{
		return false;
	}
	bool result = false;
	if( !OpenDataSocket(bBinary) ){
		AppendFile("c:\\ftp.log", "open data sock fail.");
		return false;
	}

	__int64 diff, start_time;
	vector<string> replyStrList;
	string command, reply;
	command = string("RETR ") + serverFileName;
	m_ctrlSock.SendCommand(command);
	result = m_ctrlSock.GetResponse(replyStrList, rtCode, msg, m_ResponseTimeout);
	if( result == false ){
		return false;
	}

	bool bDataArrived = false;
	start_time = ::GetTickCount();
	while(1)
	{
		if( m_dataSock.IsConnected ){
			AppendFile("c:\\ftp.log", "data sock connected.");
		}
		bDataArrived = m_dataSock.IsDataArrived(5);
		if( bDataArrived ){
			break;
		}
		diff = ::GetTickCount() - start_time;
		if( diff < 0 ){
			start_time = 0;
		}
		if( diff > 5000 ){
			AppendFile("c:\\ftp.log", "data sock: data arrive timeout.");
			break;
		}
	}

	JMemStream ms;
	if( bDataArrived ){
		AppendFile("c:\\ftp.log", "data sock: data arrived.");
		char in_buffer[65536];
		int read_count;
		while(1)
		{
			read_count = m_dataSock.ReadData(in_buffer, 65536, 1000);
			if( read_count <= 0 ){
				break;
			}
			ms.Write(in_buffer, read_count);
		}
		ms.SaveToFile(localFileName);
	}else{
		AppendFile("c:\\ftp.log", "data sock: no data arrived.");
	}
	m_dataSock.Close();

	replyStrList.clear();
	result = m_ctrlSock.GetResponse(replyStrList, rtCode, msg, m_ResponseTimeout);
	if( result == false ){
		return false;
	}
	return result;
}

bool JFtpClient::Dir(string serverPath, vector<string>& folderList, vector<string>& fileList)
{
	long rtCode;
	string msg;
	return Dir(serverPath, folderList, fileList, rtCode, msg);
}

bool JFtpClient::Dir(string serverPath, vector<string>& folderList, vector<string>& fileList, long& rtCode, string& msg)
{
	folderList.clear();
	fileList.clear();
	if( !this->OpenDataSocket(false) ){
		return false;
	}

	__int64 diff, start_time;
	vector<string> replyStrList;
	string command, reply;
	bool result;
	command = Trim(string("LIST ") + serverPath);
	m_ctrlSock.SendCommand(command);

	replyStrList.clear();
	result = m_ctrlSock.GetResponse(replyStrList, rtCode, msg, m_ResponseTimeout);
	bool is_data_exist;
	start_time = ::GetTickCount();
	while(1)
	{
		is_data_exist = m_dataSock.IsDataArrived(10);
		if( is_data_exist ){
			break;
		}
		diff = ::GetTickCount() - start_time;
		if( diff < 0 ){
			start_time = 0;
		}
		if( diff > 2000 ){
			break;
		}
	}

	JMemStream ms;
	if( is_data_exist ){
		char in_buffer[65536];
		int read_count;
		while(1)
		{
			read_count = m_dataSock.ReadData(in_buffer, 65536, 100);
			if( read_count <= 0 ){
				break;
			}
			ms.Write(in_buffer, read_count);
		}
	}
	m_dataSock.Close();

	replyStrList.clear();
	result = m_ctrlSock.GetResponse(replyStrList, rtCode, msg, m_ResponseTimeout);
	if( result == false ){
		return false;
	}
	if( ms.Size > 0 ){
		char tmp_str[2] = {'\0' , '\0'};
		ms.Write(tmp_str, 1);
		string str = ms.GetBuffer();

		vector<string>::iterator pos;
		vector<string> strList;
		set<string> split_tags;
		string name;
		split_tags.insert(" ");
		split_tags.insert("\t");
		StringSplit(str, "\n", strList);
		for( pos=strList.begin(); pos!=strList.end(); ++pos ){
			str = *pos;
			if( !str.empty() ){
				str	= Trim(str);				
				size_t index1 = str.rfind(" ");
				//index2 = str.rfind("\t");
				if( index1 != string::npos ){
					name = str.substr( str.rfind(" ") );
					name = Trim(name);
					if( str[0] == 'd' ){
						folderList.push_back(name);
					}else if( str[0] == '-' ){
						fileList.push_back(name);
					}
				}
			}
		}
	}
	return result;
}

bool JFtpClient::IsFileExist(string serverFileName)
{
	long rtCode;
	string msg;
	return IsFileExist(serverFileName, rtCode, msg);
}

bool JFtpClient::IsFileExist(string serverFileName, long& rtCode, string& msg)
{
	if( serverFileName.empty() ){
		return false;
	}
	bool result = true;
	vector<string> replyStrList;
	string command;
	command = string("RNFR ") + serverFileName;
	m_ctrlSock.SendCommand(command);
	result = m_ctrlSock.GetResponse(replyStrList, rtCode, msg, m_ResponseTimeout);
	if( result == true ){ // ??
		replyStrList.clear();
		command = string("ABOR");
		m_ctrlSock.SendCommand(command);
		result = m_ctrlSock.GetResponse(replyStrList, rtCode, msg, m_ResponseTimeout);
	}
	return result;
}

bool JFtpClient::Rename(string serverFileNameFrom, string serverFileNameTo)
{
	long rtCode;
	string msg;
	return Rename(serverFileNameFrom, serverFileNameTo, rtCode, msg);
}

bool JFtpClient::Rename(string serverFileNameFrom, string serverFileNameTo, long& rtCode, string& msg)
{
	if( serverFileNameFrom.empty() || serverFileNameTo.empty() ){
		return false;
	}
	bool result = true;
	vector<string> replyStrList;
	string command;
	command = string("RNFR ") + serverFileNameFrom;
	m_ctrlSock.SendCommand(command);
	result = m_ctrlSock.GetResponse(replyStrList, rtCode, msg, m_ResponseTimeout);
	if( result ){
		replyStrList.clear();
		command = string("RNTO ") + serverFileNameTo;
		m_ctrlSock.SendCommand(command);
		result = m_ctrlSock.GetResponse(replyStrList, rtCode, msg, m_ResponseTimeout);
	}
	return result;
}

bool JFtpClient::ChangeDir(string serverPath)
{
	long rtCode;
	string msg;
	return ChangeDir(serverPath, rtCode, msg);
}

bool JFtpClient::ChangeDir(string serverPath, long& rtCode, string& msg)
{
	if( serverPath.empty() ){
		return false;
	}
	bool result = true;
	vector<string> replyStrList;
	string command;
	command = string("CWD ") + ExtractFileDir(serverPath);
	m_ctrlSock.SendCommand(command);
	result = m_ctrlSock.GetResponse(replyStrList, rtCode, msg, 6000);
	return result;
}

bool JFtpClient::Restart(long index)
{
	long rtCode;
	string msg;
	return Restart(index, rtCode, msg);
}

bool JFtpClient::Restart(long index, long& rtCode, string& msg)
{
	if( index < 0 ){
		return false;
	}
	bool result = true;
	vector<string> replyStrList;
	string command;
	command = string("REST ") + ToString(index);
	m_ctrlSock.SendCommand(command);
	result = m_ctrlSock.GetResponse(replyStrList, rtCode, msg, m_ResponseTimeout);
	return result;
}

