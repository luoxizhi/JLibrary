#include "StdAfx.h"
#include "JPing.h"

#pragma comment(lib,"wsock32.lib")
#pragma comment(lib,"Ws2_32.lib")
string JPing::errMsg = "";

JPing::JPing(void)
: m_IpRemote("127.0.0.1")
, m_Timeout(3000)
{
	WSADATA data;
	::WSAStartup(MAKEWORD(2, 2), &data);
	timer.OnTimer.Attach<JPing>(this, &JPing::OnTimer);
}

JPing::~JPing(void)
{
	::WSACleanup();
}

void JPing::OnTimer()
{
	if( OnPing ){
		OnPing(Ping(m_IpRemote, m_Timeout));
	}
}

USHORT JPing::CheckSum(USHORT *buffer, int size)
{
	unsigned long cksum = 0;
	while(size > 1)
	{
		cksum += *(buffer++);
		size -= sizeof(USHORT);

	}
	if(size)
		cksum += *(UCHAR*)buffer;
	cksum = (cksum >> 16) + (cksum & 0xffff);
	cksum += (cksum >> 16);

	return (USHORT)(~cksum);
}

bool JPing::Ping(string IpRemote, long timeout)
{
	SOCKET sock = ::socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if( sock == SOCKET_ERROR ){
		return false;
	}
	::setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,sizeof(long));
	char name[256];
	if( ::gethostname(name, 256) == SOCKET_ERROR ){
		return false;
	}
	HOSTENT* hostInfo;
	if( (hostInfo = ::gethostbyname(name)) == NULL ){
		return false;
	}
	sockaddr_in sockLocal;
	sockLocal.sin_family = AF_INET;
	sockLocal.sin_addr.s_addr = ::htonl(ADDR_ANY);
	sockLocal.sin_port = ::htons(0);
	if( ::bind(sock, (sockaddr*)(&sockLocal), sizeof(sockLocal)) == SOCKET_ERROR ){
		return false;
	}
	sockaddr_in sockRemote;
	sockRemote.sin_family = AF_INET;
	sockRemote.sin_addr.s_addr = inet_addr(IpRemote.c_str());
	PingHeader ping, *ping_hdr;
	char recvBuffer[1024];
	char szDestIP[16];
	long totalRecv = 0;
	for (int i = 0; i < 4; i++){
		ping.i_type = 8; 
		ping.i_code = 0;
		ping.i_seqnum = (USHORT)i;
		ping.i_identify = (unsigned short)GetCurrentProcessId(); 
		ping.i_timestamp = (unsigned long)::GetTickCount();
		for(int j=0;j < 32; j++){
			ping.i_data[i] = (UCHAR)('a'+j);
		}
		ping.i_chksum = 0;		
		ping.i_chksum = CheckSum((unsigned short*)&ping,sizeof(ping));
		if(::sendto(sock, (char*)&ping, sizeof(ping),0, (struct sockaddr*)&sockRemote, sizeof(sockRemote)) == SOCKET_ERROR){
			errMsg = "sendto function err.";
			return false;
		}		
		ZeroMemory(recvBuffer, 1024);
		int len = sizeof(sockRemote);
		long recvLen = 0;
		if( (recvLen = ::recvfrom(sock, recvBuffer, sizeof(recvBuffer), 0, (struct sockaddr*)&sockRemote, &len)) == SOCKET_ERROR ){
			errMsg = "recvfrom function err";
			return false;
		}
		if( recvLen > 0 ){
			pIpHeader ip_hdr = (pIpHeader)recvBuffer;
			memcpy(szDestIP, inet_ntoa(ip_hdr->SourIp), 16);
			if( ip_hdr->IpProto == IPPROTO_ICMP && IpRemote == string(szDestIP) ){
				ping_hdr = (pPingHeader)(recvBuffer + sizeof(unsigned long)*ip_hdr->IpHlen);
				if(ping_hdr->i_type == 0){					
					__int64 timestamp = ::GetTickCount() - ping_hdr->i_timestamp; //¼ÆËãÑÓ³ÙÊ±¼ä
					totalRecv++;
				}				
			}
		}
		if(ping_hdr->i_type == 3){
			switch(ping_hdr->i_code){
			case 0: errMsg = "network unreachable";	break;
			case 1: errMsg = "host unreachable";	break;
			case 2: errMsg = "protocol unreachable"; break;
			case 3: errMsg = "port unreachable";	break;
			default: errMsg = "";					break;
			}
		}
		if(ping_hdr->i_type == 5){
			if(ping_hdr->i_code == 0){
				errMsg = "network redirection";
			}
			if(ping_hdr->i_code == 1){
				errMsg = "host redirection";
			}
		}
	}
	if( totalRecv != 0 ){
		return true;
	}else{
		return false;
	}
}