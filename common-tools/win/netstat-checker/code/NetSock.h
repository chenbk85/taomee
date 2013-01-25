// NetSock.h: interface for the CNetSock class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NETSOCK_H__38AA255E_6C4C_4630_86C4_D8A8F6FE510F__INCLUDED_)
#define AFX_NETSOCK_H__38AA255E_6C4C_4630_86C4_D8A8F6FE510F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma comment (lib,"ws2_32")
#include <WINSOCK2.H>

class CNetSock  
{
public:
	CNetSock();
	virtual ~CNetSock();
public:
	void Init();
	int CreateSock(int nPORT,char* pszIP,int nType);
	int Connect();
	int Bind();
	//返回:1.成功:发送个数. 2.失败:-1
	int Send(char* pszData,int nLen);
	//返回:1.成功:接收个数. 2.失败:-1
	int Recv(char* pszData,int nLen);
private:
	int InitWSA();
	void CleanWSA();
	void CloseSocket();
	void InvalidSocket();
	SOCKET m_s;
	sockaddr_in m_sa;
};

#pragma pack(1)
struct  st_Head
{
	unsigned long PkgLen;//协议包总长度，网络字节序，不超过4k
	char Version;//版本号 1
	unsigned long CommandID;//消息命令字 104
	unsigned long UserID;//用户ID号 55505237
	int Result;//处理结果,在Client->Server类型中,该字段不起作用.
};//17

#pragma pack(1)
struct st_LoginPack
{
	st_Head stHead;
	char Password[32];//用户密码，MD5加密  142857 -> a420384997c8a1a93d5a84046117c2aa
	unsigned int Channel;//登陆渠道,9
	unsigned int WhichGame;//1：摩尔庄园；2：赛尔号；3：大玩国
	unsigned int IP;//IP(如果是大玩国等Php网页就填用户外网地址，其他的游戏填0)
	st_LoginPack(int nWhichGame)
	{
		stHead.PkgLen = htonl(sizeof(*this));
		stHead.Version = '1';
		stHead.CommandID = htonl(104);
		stHead.UserID = htonl(55505237);
		stHead.Result = 0;
		strcpy(Password,"a420384997c8a1a93d5a84046117c2aa");
		Channel = htonl(9);
		WhichGame = htonl(nWhichGame);
		IP = 0;
	}
};//61

#pragma pack(1)
struct st_LoginPackAck 
{
	st_Head stHead;
	unsigned char Session[16];//登陆Online用的Session
	unsigned int RoleCreated;//该游戏的角色是否已创建0-没创建 1-已创建
};//37
#pragma pack(1)
struct st_OnlinePack
{
	st_Head stHead;
	short unsigned int OnlineID;//3
	unsigned char MD5[16];
	unsigned int SessionLen;
	unsigned char Session[16];//用于验证的session
	st_OnlinePack(unsigned char* md5,int md5len,unsigned char* session)
	{
		stHead.PkgLen = htonl(sizeof(*this));
		stHead.Version = '1';
		stHead.CommandID = htonl(201);
		stHead.UserID = htonl(55505237);
		stHead.Result = 0;

		this->OnlineID = htons(3);
		memset(this->MD5,0,sizeof(this->MD5));
		memcpy(this->MD5,md5,md5len);

		this->SessionLen = htonl(sizeof(Session));
		memset(this->Session,0,sizeof(this->Session));
		memcpy(this->Session,session,sizeof(Session));
	}
	
};//39+16=55

#pragma pack(1)
struct st_SeerOnlinePack
{
	st_Head stHead;
	unsigned char Session[16];//用于验证的session
	st_SeerOnlinePack(unsigned char* session,int len)
	{
		stHead.PkgLen = htonl(sizeof(*this));
		stHead.Version = '1';
		stHead.CommandID = htonl(1001);
		stHead.UserID = htonl(55505237);
		stHead.Result = 0;//?登陆时填写什么

		memset(this->Session,0,sizeof(this->Session));
		memcpy(this->Session,session,sizeof(Session));
	}
};//17+16=33

#endif // !defined(AFX_NETSOCK_H__38AA255E_6C4C_4630_86C4_D8A8F6FE510F__INCLUDED_)
