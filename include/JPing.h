#pragma once
#include "JThread.h"
#include "JFunction.h"
#include "winsock2.h"

#ifndef JPing_H
#define JPing_H

class JPing
{
public:
	__declspec(property(get=GetRemoteIp,	put=SetRemoteIp))	string	IP;
	__declspec(property(get=GetInterval,	put=SetInterval))	long	Interval;
	__declspec(property(get=GetEnable,		put=SetEnable))		bool	Enable;
	string GetRemoteIp(){ return m_IpRemote; }
	void SetRemoteIp(string ip){ m_IpRemote = ip; }
	long GetInterval(){ return timer.Interval; }
	void SetInterval(long interval){ timer.Interval = interval; }
	bool GetEnable(){ return timer.Enable; }
	void SetEnable(bool flag){ timer.Enable = flag; }

public:
	JPing(void);
	~JPing(void);
	
	static bool Ping(string IpRemote, long timeout = 1000);
	static string GetErrMsg(){ return errMsg; }
	JOneArguFunction<bool> OnPing;

private:
	typedef struct _ping
	{
		UCHAR	i_type;		//8位类型
		UCHAR	i_code;		//8位代码
		USHORT	i_chksum;	//16位ICMP校验和
		USHORT	i_identify;	//16位标志位
		USHORT	i_seqnum;	//16位序号
		ULONG   i_timestamp;//32位时间戳
		UCHAR	i_data[32];	//32BYTE选项数据
	}PingHeader,*pPingHeader;

	typedef struct _ipHeader	//IP头部，总长度20字节
	{
	#if LITTLEENDIAN
		UCHAR	IpHlen:4,		//4位首部长度
				IpVer :4;		//4位IP版本号
	#else
		UCHAR   IpVer :4,       //4位IP版本号
				IpHlen:4;		//4位首部长度
	#endif
		UCHAR	IpTos;			//8服务类型
		USHORT	IpTlen;			//总长度
		USHORT	IpId;			//标志    
		USHORT	FlagsOff;		//分片偏移
		UCHAR	IpTtl;			//生存时间
		UCHAR	IpProto;		//协议    
		USHORT	ChkSum;			//检验和
		struct in_addr   SourIp;//源IP地址    
		struct in_addr   DestIp;//目的IP地址
	} IpHeader,*pIpHeader;

private:
	void	OnTimer();
	static USHORT	CheckSum(USHORT *buffer, int size);

private:
	JTimer	timer;
	string	m_IpRemote;
	long	m_Timeout;
	static string errMsg;
};

#endif


