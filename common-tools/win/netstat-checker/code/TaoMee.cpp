// TaoMee.cpp: implementation of the CTaoMee class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "netTest.h"
#include "TaoMee.h"

#include <atlbase.h>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


extern CTaoMee* g_pTaoMee;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTaoMee::CTaoMee()
{
	memset(m_szMD5,0,sizeof(m_szMD5));
}

CTaoMee::~CTaoMee()
{

}

int CTaoMee::GetHostName()
{
	int nRes = 0;
	char szOut[MAX_MSG_LEN] = {0};
	int nOutLen = sizeof(szOut);
	nRes = gethostname(szOut,sizeof(szOut));
	if (0 == nRes)
	{
		char szMsg[] = "get host name success";
		m_log.Log(szOut,strlen(szOut),szMsg);
		SendShowOutEdit_S(szMsg,sizeof(szMsg),szOut,strlen(szOut));
	}
	else
	{
		char szMsg[] = "get host name false!";
		m_log.FormatLog(szMsg);
		SendShowOutEdit_F(szMsg,sizeof(szMsg));
		return -1;
	}
	return 0;
}

int CTaoMee::GetIpconfig()
{
	char szOut[MAX_MSG_LEN] = {0};
	unsigned long nOutLen = sizeof(szOut);
	
	if(0 == ExecDosCmd(IPCONFIG_ALL_CMD,szOut,sizeof(szOut),nOutLen))
	{
		char szMsg[] = "get ipconfig /all success.";
		m_log.Log(szOut,nOutLen,szMsg);
		SendShowOutEdit_S(szMsg,sizeof(szMsg),szOut,nOutLen);
	}
	else
	{
		char szMsg[] = "get ipconfig /all false!";
		m_log.FormatLog(szMsg);
		SendShowOutEdit_F(szMsg,sizeof(szMsg));
		return -1;
	}
	return 0;
}

int CTaoMee::ExecDosCmd( char* pszCmd,char* pszOut,int nLen,unsigned long& nOutLen )
{
	int nRes;
	SECURITY_ATTRIBUTES	sa;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;
	
	HANDLE	hRead;  
	HANDLE	hWrite;
	if(!CreatePipe(&hRead,&hWrite,&sa,0))    
	{  
		return -1;
	}    
	
	STARTUPINFO	si;
	PROCESS_INFORMATION	pi;
	si.cb = sizeof(STARTUPINFO);
	GetStartupInfo(&si);
	si.hStdError = hWrite;
	si.hStdOutput = hWrite;
	si.wShowWindow  = SW_HIDE;
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	
	if(!CreateProcess(NULL,pszCmd,NULL,NULL,TRUE,NULL,NULL,NULL,&si,&pi))
	{  
		return -1;
	}
	if (NULL != hWrite)
	{
		nRes = CloseHandle(hWrite);
		hWrite = NULL;
	}
	nOutLen = 0;
	unsigned long n = 0;
	while(true)   
	{  
		Sleep(500);
		if (0 == strcmp(TRACERT_CMD,pszCmd) || 0 == strcmp(TRACERT_ONLINE_CMD,pszCmd)
			|| 0 == strcmp(TRACERT_LOGIN_CMD,pszCmd))
		{
			if(ReadFile(hRead,pszOut + nOutLen,nLen,&n,NULL) == NULL)
			{
				nOutLen += n;
				break;
			}
		}
		else 
			if (0 == strcmp(IPCONFIG_ALL_CMD,pszCmd) || 0 == strcmp(NSLOOKUP_CMD,pszCmd))
			{
				if(ReadFile(hRead,pszOut,nLen,&n,NULL) == NULL)
				{
					nOutLen += n;
					break;
				}
			}
			nOutLen += n;
	}
	
	if (NULL != hWrite)
	{
		nRes = CloseHandle(hWrite);
		hWrite = NULL;
	}
	if (NULL != hRead)
	{
		nRes = CloseHandle(hRead);
		hRead = NULL;
	}
	return 0;	
}

int CTaoMee::GetIEVersion()
{
	int nRes = 0;
	char szOut[MAX_MSG_LEN] = {0};
	unsigned long nOutLen = sizeof(szOut);
	CRegKey keyVersion;

	long lResult = keyVersion.Open(HKEY_LOCAL_MACHINE,
		"SOFTWARE\\Microsoft\\Internet Explorer\\Version Vector");
	if (lResult == ERROR_SUCCESS)
	{  
		long lResultQuery = keyVersion.QueryValue(szOut,"IE",&nOutLen);
		keyVersion.Close();

		char szMsg[] = "IE Version";
		m_log.Log(szOut,nOutLen,szMsg);
		SendShowOutEdit_S(szMsg,sizeof(szMsg),szOut,nOutLen);
	}
	else
	{
		char szMsg[] = "get IE Version false!";
		m_log.FormatLog(szMsg);
		SendShowOutEdit_F(szMsg,sizeof(szMsg));
		return -1;
	}
	return 0;
}

int CTaoMee::GetFlashVersion()
{
	int nRes = 0;
	char szOut[MAX_MSG_LEN] = {0};
	unsigned long nOutLen = sizeof(szOut);
	CRegKey keyVersion;
	
	long lResult = keyVersion.Open(HKEY_LOCAL_MACHINE,
		"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Adobe Flash Player ActiveX");
	if (lResult == ERROR_SUCCESS)
	{  
		long lResultQuery = keyVersion.QueryValue(szOut,"DisplayVersion",&nOutLen);
		keyVersion.Close();
		
		char szMsg[] = "Adobe Flash Player ActiveX Version";
		m_log.Log(szOut,nOutLen,szMsg);
		SendShowOutEdit_S(szMsg,sizeof(szMsg),szOut,nOutLen);
	}
	else
	{
		char szMsg[] = "get Adobe Flash Player ActiveX Version false!";
		m_log.FormatLog(szMsg);
		SendShowOutEdit_F(szMsg,sizeof(szMsg));
		return -1;
	}
	return 0;
}

void CTaoMee::SendShowOutEdit_S( char* pszMsg,unsigned long nMsgLen,char* pszData /*= NULL*/,unsigned long nDataLen /*= 0*/ )
{
	st_ShowOutEdit st;
	st.Init(pszMsg,nMsgLen,pszData,nDataLen);
	SendMessage(m_piNetTestDlg->m_hWnd,WM_MSG_SHOW_OUT_EDIT,(WPARAM)&st,0);
}

void CTaoMee::SendShowOutEdit_F( char* pszMsg,unsigned long nMsgLen )
{
	st_ShowOutEdit st;
	st.Init(pszMsg,nMsgLen,NULL,0);
	SendMessage(m_piNetTestDlg->m_hWnd,WM_MSG_SHOW_OUT_EDIT,(WPARAM)&st,1);	
}

int CTaoMee::GetPolicyFile( char* pszIP,int nPort,char* pszOut,int nInLen,unsigned long& nOutLen )
{
	char request[] = "<policy-file-request/>";
	
	int nRet = 0;
	
	m_s.Init();
	if (0 != m_s.CreateSock(nPort,pszIP,SOCK_STREAM))
	{
		return -1;
	}
	if(0 != m_s.Connect())
	{
		return -1;
	}
	
	if(0 > m_s.Send(request,strlen(request)+1))//这里strlen(request) + 1是根据协议定的
	{
		return -1;
	}
	
	nOutLen = m_s.Recv(pszOut,nInLen);
	if(0 > nOutLen)
	{
		return -1;
	}
	
	return 0;
}

int CTaoMee::Telnet( char* pszIP,int nPort )
{
	int nRet = 0;
	
	m_s.Init();
	if (0 != m_s.CreateSock(nPort,pszIP,SOCK_STREAM))
	{
		return -1;
	}
	if(0 != m_s.Connect())
	{
		return -1;
	}
	
	return 0;	
}

int CTaoMee::TelnetLogin()
{
	if(0 == Telnet(LOGIN_SERVER,LOGIN_SERVER_TELNET_PORT))
	{
		char szMsg[] = "login server telnet success.";
		m_log.FormatLog(szMsg);
		SendShowOutEdit_S(szMsg,sizeof(szMsg));
	}
	else
	{
		char szMsg[] = "login server telnet false!";
		m_log.FormatLog(szMsg);
		SendShowOutEdit_F(szMsg,sizeof(szMsg));
		return -1;
	}
	return 0;
}

int CTaoMee::GetTracertInfo()
{
	char szOut[MAX_MSG_LEN] = {0};
	unsigned long nOutLen = sizeof(szOut);
	if(0 == ExecDosCmd(TRACERT_CMD,szOut,sizeof(szOut),nOutLen))
	{		
		char szMsg[] = "do tracert complete.";
		m_log.Log(szOut,nOutLen,szMsg);
		SendShowOutEdit_S(szMsg,sizeof(szMsg),szOut,nOutLen);
	}
	else
	{
		char szMsg[] = "tracert false!";
		m_log.FormatLog(szMsg);
		SendShowOutEdit_F(szMsg,sizeof(szMsg));
		return -1;
	}
	return 0;	
}

int CTaoMee::GetDNS()
{
 	char szOut[MAX_MSG_LEN] = {0};
 	unsigned long nOutLen = sizeof(szOut);
 	
 	if(0 == ExecDosCmd(NSLOOKUP_CMD,szOut,sizeof(szOut),nOutLen))
 	{
 		char szMsg[] = "get nslookup success.";
 		m_log.Log(szOut,nOutLen,szMsg);
 		SendShowOutEdit_S(szMsg,sizeof(szMsg),szOut,nOutLen);
 	}
 	else
 	{
 		char szMsg[] = "get nslookup false!";
 		m_log.FormatLog(szMsg);
 		SendShowOutEdit_F(szMsg,sizeof(szMsg));
 		return -1;
 	}
 	return 0;	
}

int CTaoMee::GetLoginPolicyFile()
{
	char szOut[MAX_MSG_LEN] = {0};
	unsigned long nOutLen = 0;
	if (0 == GetPolicyFile(LOGIN_SERVER,LOGIN_SERVER_POLICYFILE_PORT,szOut,sizeof(szOut),nOutLen))
	{
		char szMsg[] = "get login server policyfile success.";
		m_log.Log(szOut,nOutLen,szMsg);
		SendShowOutEdit_S(szMsg,sizeof(szMsg),szOut,nOutLen);
	}
	else
	{
		char szMsg[] = "get login server policyfile false!";
		m_log.FormatLog(szMsg);
		SendShowOutEdit_F(szMsg,sizeof(szMsg));
		return -1;
	}
	return 0;
}

int CTaoMee::GetOnlinePolicyFile()
{
	char szOut[MAX_MSG_LEN] = {0};
	unsigned long nOutLen = 0;
	if (0 == GetPolicyFile(ONLINE_SERVER,ONLINE_SERVER_POLICYFILE_PORT,szOut,sizeof(szOut),nOutLen))
	{
		char szMsg[] = "get online server policyfile success.";
		m_log.Log(szOut,nOutLen,szMsg);
		SendShowOutEdit_S(szMsg,sizeof(szMsg),szOut,nOutLen);
	}
	else
	{
		char szMsg[] = "get online server policyfile false!";
		m_log.FormatLog(szMsg);
		SendShowOutEdit_F(szMsg,sizeof(szMsg));
		return -1;
	}
	return 0;
}

int CTaoMee::TelnetOnline()
{
	if(0 == Telnet(ONLINE_SERVER,ONLINE_SERVER_TELNET_PORT))
	{
		char szMsg[] = "online server telnet success.";
		m_log.FormatLog(szMsg);
		SendShowOutEdit_S(szMsg,sizeof(szMsg));
	}
	else
	{
		char szMsg[] = "online server telnet false!";
		m_log.FormatLog(szMsg);
		SendShowOutEdit_F(szMsg,sizeof(szMsg));
		return -1;
	}
	return 0;
}

int CTaoMee::TryLogin()
{
	char szOut[MAX_MSG_LEN] = {0};
	unsigned long nOutLen = sizeof(szOut);
	st_LoginPack stLoginPack(TRYLOGIN_WHICHGAME);
	int u = sizeof(stLoginPack);
	char request[sizeof(stLoginPack)] = {0};
	memcpy(request,&stLoginPack,sizeof(stLoginPack));
	
	int nRet = 0;
	
	m_s.Init();
	if (0 != m_s.CreateSock(LOGIN_SERVER_TELNET_PORT,LOGIN_SERVER,SOCK_STREAM))
	{
		return -1;
	}
	if(0 != m_s.Connect())
	{
		return -1;
	}
	
	if(0 > m_s.Send(request,sizeof(request)))
	{
		return -1;
	}
	
	int nRecvLen = m_s.Recv(szOut,nOutLen);
	if(0 < nRecvLen)
	{
		st_LoginPackAck* p = (st_LoginPackAck*)szOut;
		if (p->stHead.Result == 0)
		{
			if (1 == ntohl(p->RoleCreated))
			{
				if (0 == g_pTaoMee->TryOnline(p->Session,sizeof(p->Session)))
				{
					char szMsg[] = "try login success.online sucess.";
					m_log.FormatLog(szMsg);
					SendShowOutEdit_S(szMsg,sizeof(szMsg),szOut,nOutLen);
				}
				else
				{
					char szMsg[] = "try login success.online false!";
					m_log.FormatLog(szMsg);
					SendShowOutEdit_S(szMsg,sizeof(szMsg),szOut,nOutLen);
				}
			}
			else
			{
				char szMsg[] = "try login success.--id no exist!";
				m_log.FormatLog(szMsg);
				SendShowOutEdit_S(szMsg,sizeof(szMsg),szOut,nOutLen);
			}
		}
		else
		{
			char szMsg[] = "try login false!Result != 0";
			m_log.FormatLog(szMsg);
			SendShowOutEdit_S(szMsg,sizeof(szMsg),szOut,nOutLen);
		}

	}
	else
	{
		int i = WSAGetLastError();
		char szMsg[] = "try login false!RecvLen < 0";
		m_log.FormatLog(szMsg);
 		SendShowOutEdit_F(szMsg,sizeof(szMsg));
		return -1;
	}
	
	return 0;
}

int CTaoMee::Init( CNetTestDlg* p )
{
	m_piNetTestDlg = p;
	return 0;
}

int CTaoMee::GetOnlineTracertInfo()
{
	char szOut[MAX_MSG_LEN] = {0};
	unsigned long nOutLen = sizeof(szOut);
	if(0 == ExecDosCmd(TRACERT_ONLINE_CMD,szOut,sizeof(szOut),nOutLen))
	{		
		char szMsg[] = "do online tracert complete.";
		m_log.Log(szOut,nOutLen,szMsg);
		SendShowOutEdit_S(szMsg,sizeof(szMsg),szOut,nOutLen);
	}
	else
	{
		char szMsg[] = "online tracert false!";
		m_log.FormatLog(szMsg);
		SendShowOutEdit_F(szMsg,sizeof(szMsg));
		return -1;
	}
	return 0;
}

int CTaoMee::GetLoginTracertInfo()
{
	char szOut[MAX_MSG_LEN] = {0};
	unsigned long nOutLen = sizeof(szOut);
	if(0 == ExecDosCmd(TRACERT_ONLINE_CMD,szOut,sizeof(szOut),nOutLen))
	{		
		char szMsg[] = "do login tracert complete.";
		m_log.Log(szOut,nOutLen,szMsg);
		SendShowOutEdit_S(szMsg,sizeof(szMsg),szOut,nOutLen);
	}
	else
	{
		char szMsg[] = "login tracert false!";
		m_log.FormatLog(szMsg);
		SendShowOutEdit_F(szMsg,sizeof(szMsg));
		return -1;
	}
	return 0;
}


void CTaoMee::MakeMD5( unsigned char* sess,int sessionlen )
{
	char szNewSession[16] = {0};
	memcpy(szNewSession,sess,sizeof(szNewSession));
	char md5key[256];
	unsigned char md_out[16], md[32];
	const char* str = "fREd hAo crAzy BAby in Our ProgRAm?";
	int len = sprintf(md5key, "%u%.11s%u", 
		ntohl(*(unsigned int*)(sess + 10)), str + 5,
		ntohl(*(unsigned int*)(sess + 3)));

	MD5 md5;
	md5.update(md5key,len);
	memcpy(md_out,md5.digest(),sizeof(md_out));
	int i;
	for (i = 0; i != 16; ++i) 
	{
		sprintf((char*)md + i * 2, "%.2x", md_out[i]);
	}
	memcpy(m_szMD5,md+6,sizeof(m_szMD5));
	

/*检查md5
	static inline int
	verify_user_md5(const char* md5, const char* sess)
	{
	static char md5key[256];
	
	  unsigned char md_out[16], md[32];
	  const char* str = "fREd hAo crAzy BAby in Our ProgRAm?";
	  int len = snprintf(md5key, sizeof md5key, "%u%.11s%u", 
	  ntohl(*(uint32_t*)(sess + 10)), str + 5, ntohl(*(uint32_t*)(sess + 3)));
	  MakeMD5((unsigned char*)md5key, len, md_out);
	  
		int i;
		for (i = 0; i != 16; ++i)
		{
			sprintf((char*)md + i * 2, "%.2x", md_out[i]);
		}
		
		  return memcmp(md + 6, md5, 16);
	}

	*/
}