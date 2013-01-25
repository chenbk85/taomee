// 51Seer.cpp: implementation of the C51Seer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "netTest.h"
#include "51Seer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

C51Seer::C51Seer()
{

}

C51Seer::~C51Seer()
{

}

#define FILE_PATH_NUM_51SEER 6

int C51Seer::GetWebFile()
{
	char szFilePath[FILE_PATH_NUM_51SEER][128]=
	{
		"http://www.51seer.com/config/Server.xml",
		"http://www.51seer.com/config/dll.xml",
		"http://www.51seer.com/dll/TaomeeLibraryDLL.swf",
		"http://www.51seer.com/dll/RobotCoreDLL.swf",
		"http://www.51seer.com/dll/RobotAppDLL.swf",
		"http://www.51seer.com/resource/ui.swf",
	};
	int i = 0;
	char szTime[32] = "cost time:";
	char szOut[MAX_MSG_LEN] = {0};
	unsigned long nOutLen = sizeof(szOut);
	char szMsg[] = "get 51Seer web file success.";
	char szMsg_f[] = "get 51Seer web file false!";
	CString strData;
	CInternetSession mySession(NULL,0);
	CHttpFile  *pmyHttpFile = NULL;

	double   elapsed_time;
	time_t   start, finish;
	time( &start );

	try
	{
		for(i = 0;i < FILE_PATH_NUM_51SEER;i++)
		{
			if(NULL == (pmyHttpFile=(CHttpFile *)mySession.OpenURL(szFilePath[i])))
			{
				goto err_51seer;
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
		goto err_51seer;
	}

	time( &finish );
	elapsed_time = difftime( finish, start );

	//////////////////////////////////////////////////////////////////////////
	m_log.FormatLog(szMsg);
	SendShowOutEdit_S(szMsg,sizeof(szMsg));
	for(i = 0;i < FILE_PATH_NUM_51SEER;i++)
	{
		m_log.FormatLog(szFilePath[i]);
		SendShowOutEdit_S(szFilePath[i],sizeof(szFilePath[i]));
	}
	sprintf(szTime,"%s %6.0f",szTime,elapsed_time);
	m_log.FormatLog(szTime);
	SendShowOutEdit_S(szTime,sizeof(szTime));
	return 0;
	
err_51seer:
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

int C51Seer::TryOnline( unsigned char* session,int len )
{
	int nRet = 0;
	char szOut[MAX_MSG_LEN] = {0};
	unsigned long nOutLen = sizeof(szOut);
	

	st_SeerOnlinePack stSeerOnlinePack(session,len);
	int msglen = sizeof(stSeerOnlinePack);
	
	m_s.Init();
	if (0 != m_s.CreateSock(ONLINE_SERVER_TELNET_PORT,ONLINE_SERVER,SOCK_STREAM))
	{
		return -1;
	}
	if(0 != m_s.Connect())
	{
		return -1;
	}
	

	if(0 > m_s.Send((char*)&stSeerOnlinePack,msglen))
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