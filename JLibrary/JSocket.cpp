// Copyright (c) 2012, Luoxizhi. All rights reserved.
//
// History:
// Date         Reference       Person          Descriptions
// ---------- 	-------------- 	--------------  ---------------------------
// 2012/03/01                   Luoxizhi        Initial Implementation
//
//-------------------------------------------------------------------------
#include "stdafx.h"
#include "JSocket.h"
#include "nspapi.h"

#pragma comment(lib, "Ws2_32.lib")

#define JSocketBufferSize 0x80000

/************************************************************************/
/* JSocket                                                              */
/************************************************************************/
JTcpSocket::JTcpSocket() 
:m_bListener(true)
,m_sockState(JIdle)
,m_bOpen(false)
,m_bConnected(false)
,m_ip("127.0.0.1")
,m_port(5000)
,m_portAuto(0)
,m_sockAccept(NULL)
{
	WSADATA wsData;
	::WSAStartup(0x202, &wsData);
	Resume();
}

JTcpSocket::~JTcpSocket()
{	
	WaitForThreadEnd();
	Close();
	::WSACleanup();	
}

string JTcpSocket::GetLocalIP()
{
	sockaddr_in addr;
	int len = sizeof(sockaddr_in);
	if( ::getsockname(m_socket, LPSOCKADDR(&addr), &len) != SOCKET_ERROR ){
		char outIP[32];
		unsigned char* ip = reinterpret_cast<unsigned char*>(&addr.sin_addr);
		sprintf_s(outIP, 32, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
		return string(outIP);
	}
	return string();
}

void JTcpSocket::Open()
{
	if( !m_bOpen ){
		m_cs.Enter();
		m_bListener? m_sockState = JListener : m_sockState = JCaller;
		m_bOpen = true;
		m_cs.Leave();
	}
}

void JTcpSocket::Close()
{
	m_cs.Enter();
	if( m_socket ){
		::shutdown(m_socket, SD_SEND);
		::closesocket(m_socket);
		m_socket = NULL;
	}
	if( m_sockAccept ){
		delete m_sockAccept;
		m_sockAccept = NULL;
	}
	m_bConnected = false;
	m_sockState = JIdle;
	m_bOpen = false;
	m_cs.Leave();
}

void JTcpSocket::Reopen()
{
	Close();
	Sleep(50);
	Open();
}

void JTcpSocket::Execute()
{
	while(1)
	{
		if( GetTerminateFlag() ){
			break;
		}
		if( m_sockState == JIdle ){
			Sleep(50);
			continue;
		}

		m_cs.Enter();
		JSocketState state = m_sockState;
		bool isOpen = m_bOpen;
		m_cs.Leave();

		if( !isOpen ){
			Sleep(50);
			continue;
		}
		if( state == JCaller ){			
			m_socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
			if( m_socket != NULL && m_socket != SOCKET_ERROR ){
				BOOL bRsa = TRUE;
				::setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, (const char*)(&bRsa), sizeof(BOOL));
				long BufferSize = JSocketBufferSize;
				::setsockopt(m_socket, SOL_SOCKET, SO_RCVBUF, (const char*)(&BufferSize), sizeof(long));
				::setsockopt(m_socket, SOL_SOCKET, SO_SNDBUF, (const char*)(&BufferSize), sizeof(long));
				sockaddr_in addr;
				addr.sin_family = AF_INET;
				HOSTENT* hostinfo = NULL; 
				if( m_ip.empty() )
					hostinfo = ::gethostbyname("127.0.0.1");
				else
					hostinfo = ::gethostbyname(m_ip.c_str());
				
				if( hostinfo ){
					addr.sin_addr.s_addr = ((struct in_addr *)hostinfo->h_addr)->s_addr;
					addr.sin_port = ::htons(m_port);
				}
				if( ::connect(m_socket, (const sockaddr*)(&addr), sizeof(addr)) != SOCKET_ERROR ){
					m_cs.Enter();
					m_sockState = JConnected;
					m_bConnected = true;
					m_cs.Leave();
				}else{
					Reopen();
				}
			}else{
				Reopen();
			}
		}
		else if( state == JListener ){
			m_socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
			if( m_socket == NULL && m_socket == SOCKET_ERROR ){
				Reopen();
			}
			else{
				BOOL bRsa = TRUE;
				::setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, (const char*)(&bRsa), sizeof(BOOL));
				long bufferSize = JSocketBufferSize;
				::setsockopt(m_socket, SOL_SOCKET, SO_RCVBUF, (const char*)(&bufferSize), sizeof(long));
				::setsockopt(m_socket, SOL_SOCKET, SO_SNDBUF, (const char*)(&bufferSize), sizeof(long));
				sockaddr_in addr;
				addr.sin_family = AF_INET;
				addr.sin_port = ::htons(m_port);
				addr.sin_addr.s_addr = ::htonl(ADDR_ANY);
				if( ::bind(m_socket, (LPSOCKADDR)&addr, sizeof(sockaddr_in)) == SOCKET_ERROR ){
					Reopen();
				}
				else{
					sockaddr_in addr;
					int addrSize = sizeof(sockaddr_in);
					if( m_port == 0 && ::getsockname(m_socket, (LPSOCKADDR)&addr, &addrSize) != SOCKET_ERROR )
						m_portAuto = ::ntohs(addr.sin_port);
									
					if( ::listen(m_socket, SOMAXCONN) != SOCKET_ERROR ){
						m_cs.Enter();
						m_sockState = JListening;
						m_cs.Leave();
					}else{
						Reopen();
					}
				}
			}
		}
		else if( state == JListening ){
			sockaddr_in addr;
			int addrSize = sizeof(sockaddr_in);
			SOCKET accept_sock = ::accept(m_socket, (struct sockaddr*)&addr, &addrSize);
			if( accept_sock == INVALID_SOCKET || accept_sock == NULL ){
				::shutdown(accept_sock, SD_SEND);
				::closesocket(accept_sock);
			}else{
				// close listening socket
				::shutdown(m_socket, SD_SEND);
				::closesocket(m_socket);
				m_socket = NULL;

				long bufferSize = JSocketBufferSize;
				::setsockopt(accept_sock, SOL_SOCKET, SO_RCVBUF, (const char*)(&bufferSize), sizeof(long));
				::setsockopt(accept_sock, SOL_SOCKET, SO_SNDBUF, (const char*)(&bufferSize), sizeof(long));
				m_sockAccept = new JPeerSocket(accept_sock);
				if( m_sockAccept != NULL ){
					m_sockAccept->OnRecvData.Attach<JTcpSocket>(this, &JTcpSocket::ReadData);
					m_sockAccept->Open();
					m_cs.Enter();
					m_sockState = JConnected;
					m_bConnected = true;
					m_cs.Leave();
				}
			}
		}
		else if( state == JConnected ){
			if( !m_bListener ){
				char buffer[65535];
				if( IsDataArrived(10) ){
					long recvSize = 0;
					while( IsDataArrived(0) ){
						recvSize = ::recv(m_socket, buffer, 65535*sizeof(TCHAR), 0);
						if( recvSize <= 0 )
							break;
						
						if( OnRecvData && m_sockState == JConnected)
							OnRecvData(buffer, recvSize);
					}
				}else{
					//Sleep(50);
				}
			}else{
				Sleep(50);
			}
		}else{
			Sleep(50);
		}
	}
	//Close();
}

void JTcpSocket::SendData(const char* buffer, long bufferSize)
{
	if( m_sockState != JConnected || bufferSize <= 0 ){
		return;
	}
	if( !m_bListener ){
		if( m_socket ){
			long sendSize = 0, sendedSize = 0;
			do 
			{
				sendSize = ::send(m_socket, buffer+sendedSize, bufferSize-sendedSize, 0);
				if( sendSize <= 0 ){
					break;
				}
				sendedSize += sendSize;
			} while ( sendedSize < bufferSize );
		}
	}else{
		if( m_sockAccept ){
			m_sockAccept->SendData(buffer, bufferSize);
		}
	}
}

void JTcpSocket::ReadData(ULONG threadId, const char* buffer, long bufferSize)
{
	if( m_sockAccept->IsConnected && OnRecvData ){
		OnRecvData(buffer, bufferSize);
	}
}

bool JTcpSocket::IsDataArrived(long millSeconds)
{
	if( !m_bOpen || m_sockState != JConnected )
		return false;

	FD_SET read_set;
	FD_ZERO(&read_set);
	FD_SET(m_socket, &read_set);

	timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 1;

	bool result = false;
	long rtSelect;
	if( millSeconds <= 0 ){
		rtSelect = ::select(FD_SETSIZE, &read_set, NULL, NULL, &timeout);
		result = (rtSelect != 0 && rtSelect != SOCKET_ERROR);
	}else{
		for(long i = 0; i < millSeconds; i++){
			rtSelect = ::select(FD_SETSIZE, &read_set, NULL, NULL, &timeout);
			if( rtSelect != 0 && rtSelect != SOCKET_ERROR ){
				result = true;
				break;
			}
		}
	}
	return result;
}

/************************************************************************/
/* JPeerSocket                                                          */
/************************************************************************/
JPeerSocket::JPeerSocket(SOCKET sock) 
: m_socket(sock)
, m_bConnected(false)
{
	WSAData wsa_data;
	::WSAStartup(0x202, &wsa_data);
}

JPeerSocket::~JPeerSocket()
{
	WaitForThreadEnd();
	Close();
	::WSACleanup();	
}

void JPeerSocket::Open()
{
	Resume();
}

void JPeerSocket::Close()
{
	if(m_socket){
		::shutdown(m_socket, SD_SEND);
		::closesocket(m_socket);
		m_socket = NULL;
		m_bConnected = false;
	}
}

void JPeerSocket::Execute()
{
	char buffer[65535];
	while(1){
		if( GetTerminateFlag() || m_socket == NULL )
			break;

		if( IsDataArrived(10) ){
			long recvSize = 0;
			while( IsDataArrived(0) ){
				recvSize = ::recv(m_socket, buffer, 65535*sizeof(TCHAR), 0);
				if( recvSize <= 0 )
					break;

				if( !m_bConnected )
					m_bConnected = true;
				if( OnRecvData )
					OnRecvData(ThreadId, buffer, recvSize);
			}
		}else{
			Sleep(25);
		}
	}
	Close();
}

bool JPeerSocket::IsDataArrived(long millSeconds)
{
	FD_SET read_set;
	FD_ZERO(&read_set);
	FD_SET(m_socket, &read_set);

	timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 1;

	bool result = false;
	long rtSelect;
	if( millSeconds <= 0 ){
		rtSelect = ::select(FD_SETSIZE, &read_set, NULL, NULL, &timeout);
		result = (rtSelect != 0 && rtSelect != SOCKET_ERROR);
	}else{
		for(long i = 0; i < millSeconds; i++){
			rtSelect = ::select(FD_SETSIZE, &read_set, NULL, NULL, &timeout);
			if( rtSelect != 0 && rtSelect != SOCKET_ERROR ){
				result = true;
				break;
			}
		}
	}
	return result;
}

void JPeerSocket::SendData(const char *buffer, long bufferSize)
{
	if( bufferSize <= 0 )
		return;

	if( m_socket ){
		long sendSize = 0, sendedSize = 0;
		do 
		{
			sendSize = ::send(m_socket, buffer+sendedSize, bufferSize-sendedSize, 0);
			if( sendSize <= 0 )
				break;

			sendedSize += sendSize;
		} while ( sendedSize < bufferSize );
	}
}

/************************************************************************/
/* JTcpServer                                                           */
/************************************************************************/
JTcpServer::JTcpServer()
: m_bOpen(false)
, m_port(0)
, m_portAuto(0)
, m_sockState(JIdle)
{
	WSADATA data;
	::WSAStartup(MAKEWORD(2,2), &data);
}

JTcpServer::~JTcpServer()
{
	WaitForThreadEnd();
	::WSACleanup();
}

void JTcpServer::Open()
{
	if( !m_bOpen ){
		m_cs.Enter();
		m_bOpen = true;
		m_sockState = JListener;
		m_cs.Leave();
	}
}

void JTcpServer::Close()
{
	if( m_bOpen ){
		m_cs.Enter();
		if( m_socket ){
			::shutdown(m_socket, SD_SEND);
			::closesocket(m_socket);
		}
		m_cs.Enter();
		for ( map<ULONG, JPeerSocket*>::iterator it = m_sockAcpMap.begin(); it != m_sockAcpMap.end(); ++it ){
			delete it->second;
			m_sockAcpMap.erase(it);
		}
		m_bOpen = false;		
		m_sockState = JIdle;
		m_portAuto = 0;
		m_cs.Leave();
	}
}

void JTcpServer::Reopen()
{
	Close();
	Sleep(50);
	Open();
}

void JTcpServer::SendData(ULONG threadId, const char *buffer, long bufferSize)
{
	if( m_sockAcpMap.find(threadId) != m_sockAcpMap.end() )
		m_sockAcpMap[threadId]->SendData(buffer, bufferSize);
}

void JTcpServer::ReadData(ULONG threadId, const char* buffer, long bufferSize)
{
	if( OnRecvData )
		OnRecvData(threadId, buffer, bufferSize);
}

void JTcpServer::Execute()
{
	while(1){
		if( GetTerminateFlag() )
			break;
		
		if( !m_bOpen ){
			Sleep(50);
			continue;
		}
		if( m_sockState == JListener ){
			m_socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
			if( m_socket != SOCKET_ERROR ){
				BOOL bRsa = TRUE;
				::setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, (const char*)(&bRsa), sizeof(bRsa));
				sockaddr_in sockAddr;
				sockAddr.sin_family = AF_INET;
				sockAddr.sin_addr.s_addr = ::htonl(ADDR_ANY);
				sockAddr.sin_port = ::htons(m_port);
				if( ::bind(m_socket, (sockaddr*)&sockAddr, sizeof(sockAddr)) == SOCKET_ERROR ){
					Reopen();
				}else{
					if( m_port == 0 ){
						sockaddr_in sockAddr;
						int len = sizeof(sockAddr);
						if( ::getsockname(m_socket, (sockaddr*)&sockAddr, &len) != SOCKET_ERROR )
							m_portAuto = ::ntohs(sockAddr.sin_port);
					}
					if( ::listen(m_socket, SOMAXCONN) != SOCKET_ERROR ){
						m_cs.Enter();
						m_sockState = JListening;
						m_cs.Leave();
					}else{
						Reopen();
					}
				}
			}else{
				Reopen();
			}
		}else if( m_sockState == JListening ){
			sockaddr_in sockIn;
			int len = sizeof(sockIn);
			SOCKET sockAccept = ::accept(m_socket, (sockaddr*)&sockIn, &len);
			if( sockAccept != INVALID_SOCKET && sockAccept != NULL ){
				long bufferSize = JSocketBufferSize;
				::setsockopt(sockAccept, SOL_SOCKET, SO_RCVBUF, (const char*)(&bufferSize), sizeof(bufferSize));
				::setsockopt(sockAccept, SOL_SOCKET, SO_SNDBUF, (const char*)(&bufferSize), sizeof(bufferSize));
				JPeerSocket* newSock = new JPeerSocket(sockAccept);
				if( newSock != NULL ){
					newSock->OnRecvData.Attach<JTcpServer>(this, &JTcpServer::ReadData);
					newSock->Open();
					m_cs.Enter();
					m_sockAcpMap[newSock->ThreadId] = newSock;
					m_cs.Leave();
				}				
			}else{
				::closesocket(sockAccept);
			}
		}
	}
	Close();
}

string JTcpServer::GetLocalIP()
{
	sockaddr_in addr;
	int len = sizeof(sockaddr_in);
	if( ::getsockname(m_socket, LPSOCKADDR(&addr), &len) != SOCKET_ERROR ){
		char outIP[32];
		unsigned char* ip = reinterpret_cast<unsigned char*>(&addr.sin_addr);
		sprintf_s(outIP, 32, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
		return string(outIP);
	}
	return string();
}

/************************************************************************/
/* JBlockTcpSocket                                                      */
/************************************************************************/
JBlockTcpSocket::JBlockTcpSocket()
: m_bOpen(false)
, m_port(0)
, m_portAuto(0)
, m_sockState(JIdle)
, m_sockAccept(NULL)
, m_bConnected(false)
, m_bListener(false)
{
	::WSAStartup(MAKEWORD(2,2), &WSAData());
	Resume();
}

JBlockTcpSocket::~JBlockTcpSocket()
{
	::WSACleanup();
}

void JBlockTcpSocket::Open()
{
	if( !m_bOpen ){
		m_cs.Enter();
		m_bOpen = true;
		m_bListener? m_sockState = JListener : m_sockState = JCaller;
		m_cs.Leave();
	}
}

void JBlockTcpSocket::Close()
{
	if( m_bOpen ){
		m_cs.Enter();
		if( m_socket ){
			::shutdown(m_socket, SD_SEND);
			::closesocket(m_socket);
			m_socket = NULL;
		}
		if( m_sockAccept ){
			::shutdown(m_sockAccept, SD_SEND);
			::closesocket(m_sockAccept);
			m_sockAccept = NULL;
		}
		m_bConnected = false;
		m_sockState = JIdle;
		m_cs.Leave();
	}
}

void JBlockTcpSocket::Reopen()
{
	Close();
	Sleep(50);
	Open();
}

string JBlockTcpSocket::GetLocalIP()
{
	sockaddr_in addr;
	int len = sizeof(sockaddr_in);
	if( ::getsockname(m_socket, LPSOCKADDR(&addr), &len) != SOCKET_ERROR ){
		char outIP[32];
		unsigned char* ip = reinterpret_cast<unsigned char*>(&addr.sin_addr);
		sprintf_s(outIP, 32, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
		return string(outIP);
	}
	return string();
}

void JBlockTcpSocket::Execute()
{
	while(1){
		if( GetTerminateFlag() ){
			break;
		}
		if( !m_bOpen ){
			Sleep(50);
			continue;
		}
		if( m_sockState == JCaller ){
			m_socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
			if( m_socket != NULL && m_socket != SOCKET_ERROR ){
				BOOL bRsa = TRUE;
				::setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&bRsa, sizeof(bRsa));
				long bufferSize = JSocketBufferSize;
				::setsockopt(m_socket, SOL_SOCKET, SO_RCVBUF, (const char*)&bufferSize, sizeof(bufferSize));
				::setsockopt(m_socket, SOL_SOCKET, SO_SNDBUF, (const char*)&bufferSize, sizeof(bufferSize));
				sockaddr_in sockAddrRemote;
				sockAddrRemote.sin_family = AF_INET;
				HOSTENT* hostInfo;
				if( m_ip.empty() )
					hostInfo = ::gethostbyname("127.0.0.1");	
				else
					hostInfo = ::gethostbyname(m_ip.c_str());
				
				if( hostInfo != NULL ){
					sockAddrRemote.sin_addr.s_addr = ((in_addr*)hostInfo->h_addr)->s_addr;
					sockAddrRemote.sin_port = ::htons(m_port);
					if( ::connect(m_socket, (sockaddr*)&sockAddrRemote, sizeof(sockAddrRemote)) != SOCKET_ERROR ){
						m_cs.Enter();
						m_bConnected = true;
						m_sockState = JConnected;
						m_cs.Leave();
					}else{
						Reopen();
					}
				}
			}else{
				Reopen();
			}
		}else if( m_sockState == JListener ){
			m_socket = ::socket(AF_INET, SOL_SOCKET, IPPROTO_IP);
			if( m_socket == NULL || m_socket == SOCKET_ERROR ){
				Reopen();
			}else{
				BOOL bRsa = TRUE;
				::setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&bRsa, sizeof(bRsa));
				long bufferSize = JSocketBufferSize;
				::setsockopt(m_socket, SOL_SOCKET, SO_RCVBUF, (const char*)&bufferSize, sizeof(bufferSize));
				::setsockopt(m_socket, SOL_SOCKET, SO_SNDBUF, (const char*)&bufferSize, sizeof(bufferSize));
				sockaddr_in sockAddrLocal;
				sockAddrLocal.sin_family = AF_INET;
				sockAddrLocal.sin_addr.s_addr = ::htonl(ADDR_ANY);
				sockAddrLocal.sin_port = ::htons(m_port);
				if( ::bind(m_socket, (sockaddr*)&sockAddrLocal, sizeof(sockAddrLocal)) != SOCKET_ERROR ){
					sockaddr_in addrLocal;
					int len;
					if( ::getsockname(m_socket, (sockaddr*)&addrLocal, &len) != SOCKET_ERROR ){
						m_portAuto = ::ntohs(addrLocal.sin_port);
					}
					if( ::listen(m_socket, SOMAXCONN) == SOCKET_ERROR ){
						Reopen();
					}else{
						m_cs.Enter();
						m_sockState = JListening;
						m_cs.Leave();
					}					
				}else{
					Reopen();
				}
			}
		}else if( m_sockState == JListening ){
			sockaddr_in sockRemote;
			int len;
			SOCKET sockAccept = ::accept(m_socket, (sockaddr*)&sockRemote, &len);
			if( sockAccept == INVALID_SOCKET || sockAccept == SOCKET_ERROR ){
				::closesocket(sockAccept);
			}else{				
				::shutdown(m_socket, SD_SEND);
				::closesocket(m_socket);
				BOOL bRsa = TRUE;
				::setsockopt(sockAccept, SOL_SOCKET, SO_REUSEADDR, (const char*)&bRsa, sizeof(bRsa));
				long bufferSize = JSocketBufferSize;
				::setsockopt(sockAccept, SOL_SOCKET, SO_RCVBUF, (const char*)&bufferSize, sizeof(bufferSize));
				::setsockopt(sockAccept, SOL_SOCKET, SO_SNDBUF, (const char*)&bufferSize, sizeof(bufferSize));
				m_sockAccept = sockAccept;
				m_cs.Enter();
				m_bConnected = true;
				m_sockState = JConnected;
				m_cs.Leave();
			}
		}
	}
	Close();
}

void JBlockTcpSocket::SendData(const char* buffer, long bufferSize)
{
	if( !m_bOpen || bufferSize <= 0  ){
		return;
	}

	if( m_sockState == JConnected ){
		SOCKET sockSend = m_bListener? m_sockAccept: m_socket;
		long sendedSize = 0;
		do 
		{
			long sendSize = 0;
			sendSize = ::send(sockSend, buffer+sendSize, bufferSize-sendedSize, 0);
			if( sendSize <= 0 ){
				break;
			}
			sendedSize += sendSize;		
		} while (sendedSize < bufferSize);
	}
}

long JBlockTcpSocket::ReadData(char* buffer, long bufferSize, long timeout)
{
	long recvSize = 0;
	if( m_sockState == JConnected && m_bOpen && IsDataArrived(timeout) ){
		SOCKET sock = m_bListener? m_sockAccept: m_socket;
		recvSize = ::recv(sock, buffer, bufferSize, 0);
		if( recvSize <= 0 ){
			Reopen();
		}
	}
	return recvSize;
}

bool JBlockTcpSocket::IsDataArrived(long millSeconds /* = 0 */)
{
	SOCKET sock = m_bListener? m_sockAccept: m_socket;
	FD_SET read_set;
	FD_ZERO(&read_set);
	FD_SET(sock, &read_set);

	timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 1;

	bool result = false;
	long rtSelect;
	if( millSeconds <= 0 ){
		rtSelect = ::select(FD_SETSIZE, &read_set, NULL, NULL, &timeout);
		result = (rtSelect != 0 && rtSelect != SOCKET_ERROR);
	}else{
		for(long i = 0; i < millSeconds; i++){
			rtSelect = ::select(FD_SETSIZE, &read_set, NULL, NULL, &timeout);
			if( rtSelect != 0 && rtSelect != SOCKET_ERROR ){
				result = true;
				break;
			}
		}
	}
	return result;
}


/************************************************************************/
/* JUdpSocket                                                           */
/************************************************************************/
JUdpSocket::JUdpSocket()
: m_bOpen(false)
, m_IpRemote("127.0.0.1")
, m_PortLocal(0)
, m_PortRemote(0)
, m_bLocalBinded(false)
, m_bRemoteBinded(false)
{
	WSAData wsa_data;
	::WSAStartup(0x202, &wsa_data);
	Resume();
}

JUdpSocket::~JUdpSocket()
{
	WaitForThreadEnd();
	Close();
	::WSACleanup();
}

void JUdpSocket::Open()
{
	if( !m_bOpen )
		m_bOpen = true;
}

void JUdpSocket::Close()
{
	if( m_bOpen ){
		if( m_SockLocal ){
			::shutdown(m_SockLocal, SD_SEND);
			::closesocket(m_SockLocal);
			m_SockLocal = NULL;
		}
		m_bRemoteBinded = false;
		m_bLocalBinded = false;
		m_bOpen = false;
	}	
}

void JUdpSocket::Execute()
{
	while(1)
	{
		if( GetTerminateFlag() )
			break;
		
		if( !m_bOpen ){
			Sleep(50);
			continue;
		}

		if( !m_bLocalBinded && m_PortLocal != 0 ){
			m_SockLocal = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
			if( m_SockLocal != SOCKET_ERROR ){
				sockaddr_in sockAddrLocal;
				sockAddrLocal.sin_family = AF_INET;
				sockAddrLocal.sin_addr.s_addr = ::htonl(ADDR_ANY); 
				sockAddrLocal.sin_port = ::htons(m_PortLocal);
				BOOL enable = TRUE;
				::setsockopt(m_SockLocal, SOL_SOCKET, SO_REUSEADDR|SO_BROADCAST, (const char*)(&enable), sizeof(BOOL));
				long bufferSize = JSocketBufferSize;
				::setsockopt(m_SockLocal, SOL_SOCKET, SO_RCVBUF, (const char*)(&bufferSize), sizeof(bufferSize));
				::setsockopt(m_SockLocal, SOL_SOCKET, SO_SNDBUF, (const char*)(&bufferSize), sizeof(bufferSize));

				if( ::bind(m_SockLocal, (sockaddr*)(&sockAddrLocal), sizeof(sockAddrLocal)) != SOCKET_ERROR )
					m_bLocalBinded = true;				
			}
		}
		if( !m_bRemoteBinded && m_PortRemote != 0 ){
			HOSTENT* hostInfo;
			if( m_IpRemote == "" )
				hostInfo = ::gethostbyname("127.0.0.1");
			else
				hostInfo = ::gethostbyname(m_IpRemote.c_str());
			
			if( hostInfo ){
				m_SockAddrRemote.sin_family = AF_INET;
				m_SockAddrRemote.sin_addr.s_addr = ::htonl(((struct in_addr *)hostInfo->h_addr)->s_addr);
				m_SockAddrRemote.sin_port = ::htons(m_PortRemote);
				m_bRemoteBinded = true;
			}
		}
		if( m_bLocalBinded ){
			if( m_SockLocal && IsDataArrived(10) ){
				char buffer[65535];
				int addrSize = sizeof(m_SockAddrRemote);
				while( IsDataArrived(0) ){
					long rcvSize = ::recvfrom(m_SockLocal, buffer, 65535*sizeof(TCHAR), 0, (sockaddr*)(&m_SockAddrRemote), (int*)(&addrSize));
					if( rcvSize <= 0 )
						break;
					// callback function
					if( OnRecvData )
						OnRecvData(buffer, rcvSize);
				}
			}
		}
	}
	Close();
}

bool JUdpSocket::IsDataArrived(long millSeconds /* = 0 */)
{
	FD_SET read_set;
	FD_ZERO(&read_set);
	FD_SET(m_SockLocal, &read_set);

	timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 1;

	bool result = false;
	long rtSelect;
	if( millSeconds <= 0 ){
		rtSelect = ::select(FD_SETSIZE, &read_set, NULL, NULL, &timeout);
		rtSelect = (rtSelect != 0 && rtSelect != SOCKET_ERROR);
	}else{
		for(long i = 0; i < millSeconds; i++){
			rtSelect = ::select(FD_SETSIZE, &read_set, NULL, NULL, &timeout);
			if( rtSelect != 0 && rtSelect != SOCKET_ERROR ){
				result = true;
				break;
			}
		}
	}
	return result;
}

void JUdpSocket::SendData(const char *buffer, long bufferSize)
{
	if( !m_bOpen || !m_bLocalBinded || !m_bRemoteBinded ){
		return;
	}

	if( m_SockLocal ){
		long dataSizeSent = 0;
		do 
		{
			long sendSize = ::sendto(m_SockLocal, buffer, bufferSize, 0, (sockaddr*)(&m_SockAddrRemote), sizeof(m_SockAddrRemote));
			if( sendSize <= 0 ){
				break;
			}
			dataSizeSent += sendSize;
		} while (dataSizeSent < bufferSize);		
	}
}

void JUdpSocket::SetLocalPort(USHORT port)
{
	if( port!= m_PortLocal ){
		if( m_SockLocal ){
			::shutdown(m_SockLocal, SD_SEND);
			::closesocket(m_SockLocal);
			m_SockLocal = NULL;			
		}
		m_bLocalBinded = false;
		m_PortLocal = port;
	}
}

void JUdpSocket::SetRemoteIp(string ip)
{
	if( ip != m_IpRemote ){
		m_bRemoteBinded = false;
		m_IpRemote = ip;
	}
}

void JUdpSocket::SetRemotePort(USHORT port)
{
	if( port != m_PortRemote ){
		m_bRemoteBinded = false;
		m_PortRemote = port;
	}
}


