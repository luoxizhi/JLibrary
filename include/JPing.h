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
		UCHAR	i_type;		//8λ����
		UCHAR	i_code;		//8λ����
		USHORT	i_chksum;	//16λICMPУ���
		USHORT	i_identify;	//16λ��־λ
		USHORT	i_seqnum;	//16λ���
		ULONG   i_timestamp;//32λʱ���
		UCHAR	i_data[32];	//32BYTEѡ������
	}PingHeader,*pPingHeader;

	typedef struct _ipHeader	//IPͷ�����ܳ���20�ֽ�
	{
	#if LITTLEENDIAN
		UCHAR	IpHlen:4,		//4λ�ײ�����
				IpVer :4;		//4λIP�汾��
	#else
		UCHAR   IpVer :4,       //4λIP�汾��
				IpHlen:4;		//4λ�ײ�����
	#endif
		UCHAR	IpTos;			//8��������
		USHORT	IpTlen;			//�ܳ���
		USHORT	IpId;			//��־    
		USHORT	FlagsOff;		//��Ƭƫ��
		UCHAR	IpTtl;			//����ʱ��
		UCHAR	IpProto;		//Э��    
		USHORT	ChkSum;			//�����
		struct in_addr   SourIp;//ԴIP��ַ    
		struct in_addr   DestIp;//Ŀ��IP��ַ
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


