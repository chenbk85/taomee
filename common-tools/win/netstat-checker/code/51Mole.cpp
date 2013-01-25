// 51Mole.cpp: implementation of the C51Mole class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "netTest.h"
#include "51Mole.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

C51Mole::C51Mole()
{

}

C51Mole::~C51Mole()
{

}

#define FILE_PATH_NUM_51MOLE 3
int C51Mole::GetWebFile()
{
	char szFilePath[FILE_PATH_NUM_51MOLE][128]=
	{
		"http://www.51mole.com/Login.swf",
		"http://www.51mole.com/resource/entertainment/newCoreDLL.swf",
		"http://www.51mole.com/config/ext.xml",
	};
	int i = 0;
	char szOut[MAX_MSG_LEN] = {0};
	unsigned long nOutLen = sizeof(szOut);
	char szMsg[] = "get 51Mole web file success.";
	char szMsg_f[] = "get 51Mole web file false!";
	char szTime[32] = "cost time:";

	CString strData;
	CInternetSession mySession(NULL,0);
	CHttpFile  *pmyHttpFile = NULL;
	
	double   elapsed_time;
	time_t   start, finish;
	time( &start );

	try
	{
		for(i = 0;i < FILE_PATH_NUM_51MOLE;i++)
		{
			if(NULL == (pmyHttpFile=(CHttpFile *)mySession.OpenURL(szFilePath[i])))
			{
				goto err_51mole;
			}
			while(pmyHttpFile->ReadString(strData))
			{//按行获取文件内容
			}
			if (NULL != pmyHttpFile)
			{ 
				pmyHttpFile->Close();
				delete pmyHttpFile;
				pmyHttpFile = NULL;
				mySession.Close();

			}
		}
	}
	catch(...)
	{
		goto err_51mole;
	}

	time( &finish );
	elapsed_time = difftime( finish, start );

	m_log.FormatLog(szMsg);
	SendShowOutEdit_S(szMsg,sizeof(szMsg));
	for(i = 0;i < FILE_PATH_NUM_51MOLE;i++)
	{
		m_log.FormatLog(szFilePath[i]);
			SendShowOutEdit_S(szFilePath[i],sizeof(szFilePath[i]));
	}
	sprintf(szTime,"%s %6.0f",szTime,elapsed_time);
	m_log.FormatLog(szTime);
	SendShowOutEdit_S(szTime,sizeof(szTime));

	return 0;

err_51mole:
	if (NULL != pmyHttpFile)
	{ 
		pmyHttpFile->Close();
		delete pmyHttpFile;
		pmyHttpFile = NULL;
		mySession.Close();
	}
	m_log.FormatLog(szMsg_f);
	SendShowOutEdit_F(szMsg_f,sizeof(szMsg_f));
	return -1;	
}

int C51Mole::TryOnline( unsigned char* session,int len )
{
	int nRet = 0;
	char szOut[MAX_MSG_LEN] = {0};
	unsigned long nOutLen = sizeof(szOut);
	
	MakeMD5(session,len);
	st_OnlinePack stOnlinePack(m_szMD5,sizeof(m_szMD5),session);//初始化发送包
	int msglen = sizeof(stOnlinePack);
	
	m_s.Init();
	if (0 != m_s.CreateSock(ONLINE_SERVER_TELNET_PORT,ONLINE_SERVER,SOCK_STREAM))
	{
		return -1;
	}
	if(0 != m_s.Connect())
	{
		return -1;
	}
	
	if(0 > m_s.Send((char*)&stOnlinePack,msglen))
	{
		return -1;
	}
	
	int nRecvLen = m_s.Recv(szOut,nOutLen);
	if(0 < nRecvLen)
	{
		st_LoginPackAck* p = (st_LoginPackAck*)szOut;
		if (p->stHead.Result == 0)
		{
			return 0;
		}
	}
	return -1;
}
