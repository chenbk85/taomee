// TaoMee.h: interface for the CTaoMee class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TAOMEE_H__56B0A89D_D1B2_489B_8D3C_A8A980A03A57__INCLUDED_)
#define AFX_TAOMEE_H__56B0A89D_D1B2_489B_8D3C_A8A980A03A57__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Log.h"
#include "NetSock.h"
#include "config.h"
#include "st.h"
#include "NetTestDlg.h"
#include <time.h>
#include <afxinet.h>
#include "md5.h"
class CTaoMee  
{
public:
	CTaoMee();
	virtual ~CTaoMee();
public:
	int Init(CNetTestDlg* p);
	//获取hostname
	int GetHostName();
	//获取IP配置信息 ipconfig /all
	int GetIpconfig();
	//获取IE版本号
	int GetIEVersion();
	//获取Flash版本号
	int GetFlashVersion();
	//获取WEB_FILE文件
	virtual int GetWebFile() = 0;
	//尝试登陆
	virtual int TryLogin();
	//尝试连接
	virtual int TryOnline(unsigned char* session,int len) = 0;
	//获取DNS
	int GetDNS();
	//获取tracert主机路径
	int GetTracertInfo();
	//获取ONLINE主机路径
	int GetOnlineTracertInfo();
	//获取Login主机路径
	int GetLoginTracertInfo();
	//获取登陆服务器Policy File 
	int GetLoginPolicyFile();
	//Telnet登陆服务器
	int TelnetLogin();
	//获取在线服务器Policy File 
	int GetOnlinePolicyFile();
	//Telnet在线服务器
	int TelnetOnline();
protected:
	int ExecDosCmd( char* pszCmd,char* pszOut,int nLen,unsigned long& nOutLen );
	int GetPolicyFile(char* pszIP,int nPort,char* pszOut,int nInLen,unsigned long& nOutLen);
	int Telnet(char* pszIP,int nPort);

	void SendShowOutEdit_S(char* pszMsg,unsigned long nMsgLen,char* pszData = NULL,unsigned long nDataLen = 0);// 成功
	void SendShowOutEdit_F(char* pszMsg,unsigned long nMsgLen);// 失败
	void MakeMD5( unsigned char* session,int sessionlen);
	unsigned char m_szMD5[16];
protected:
	CLog m_log;
	CNetSock m_s;
private:
	CNetTestDlg* m_piNetTestDlg;

	
};

#endif // !defined(AFX_TAOMEE_H__56B0A89D_D1B2_489B_8D3C_A8A980A03A57__INCLUDED_)
