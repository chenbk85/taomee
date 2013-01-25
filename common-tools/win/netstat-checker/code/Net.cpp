// Net.cpp: implementation of the CNet class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <afxinet.h>
#include <atlbase.h>
#include <process.h>


#include "netTest.h"
#include "Net.h"
#include "config.h"
#include "netTestDlg.h"
#include "st.h"
#include "TaoMee.h"
#include "51Mole.h"
#include "51Seer.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CTaoMee* g_pTaoMee;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNet::CNet()
{
	m_hThreadTest = NULL;
}

CNet::~CNet()
{
	
}

int CNet::BeginTest()
{
	if (!m_hThreadTest)
	{
		m_hThreadTest = (HANDLE)_beginthreadex(NULL,0,ThreadTest,this,0,NULL);
	}
	return 0;
}

unsigned __stdcall CNet::ThreadTest( void* pData )
{
	CNet* pThis = (CNet*) pData;
	CTaoMee* pTaoMee = NULL;

#ifdef DO_51MOLE
	pTaoMee = new C51Mole;
#else
	pTaoMee = new C51Seer;
#endif // DO_51MOLE
	g_pTaoMee = pTaoMee;

	pTaoMee->Init(pThis->m_piNetTestDlg);
	
	pTaoMee->GetHostName();//获取hostname
	pTaoMee->GetIpconfig();//获取IP配置信息
	pTaoMee->GetIEVersion();//获取IE版本号
	pTaoMee->GetFlashVersion();
	pTaoMee->GetWebFile();//获取WEB_FILE文件_51MOLE
	pTaoMee->TryLogin();
	pTaoMee->GetDNS();
	pTaoMee->GetTracertInfo();//获取tracert主机路径
	pTaoMee->GetLoginTracertInfo();//获取login服务器路径
	pTaoMee->GetOnlineTracertInfo();//获取online服务器路径
	pTaoMee->GetLoginPolicyFile();//获取登陆服务器Policy File 
	pTaoMee->TelnetLogin();//Telnet登陆服务器
	pTaoMee->GetOnlinePolicyFile();//获取在线服务器Policy File 
	pTaoMee->TelnetOnline();//Telnet在线服务器

	if (pTaoMee)
	{
		delete pTaoMee;
		pTaoMee = NULL;
	}
	PostMessage(pThis->m_piNetTestDlg->m_hWnd,WM_MSG_TEST_DONE,0,0);
	return 0;
}

int CNet::Init( CNetTestDlg* piNetTestDlg )
{
	m_piNetTestDlg = piNetTestDlg;
	return 0;
}

int CNet::WaitThread()
{
	if (m_hThreadTest)
	{
		DWORD dw = WaitForSingleObject(m_hThreadTest,INFINITE);
		m_hThreadTest = NULL;
	}
	return 0;
}