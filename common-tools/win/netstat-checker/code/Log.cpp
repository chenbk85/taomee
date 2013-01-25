// Log.cpp: implementation of the CLog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "netTest.h"
#include "Log.h"
#include "config.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
extern char g_szFilePath[MAX_PATH];
CLog::CLog()
{
	m_pFile = NULL;
	Open();
}

CLog::~CLog()
{
	Close();
}

int CLog::Open()
{
	char szFileName[MAX_PATH] = {0};

	time_t t;
	struct tm *ptm;
	time(&t);
	ptm = localtime(&t);	
	sprintf(szFileName,"%s_%d_%02d_%02d_%02d_%02d_%02d%s",
		TEST_NAME,ptm->tm_year+1900,ptm->tm_mon+1,ptm->tm_mday,
		ptm->tm_hour,ptm->tm_min,ptm->tm_sec,".txt");

	m_pFile = fopen(szFileName,"w+a");
	
	if (NULL != m_pFile)
	{
		char ss[MAX_PATH];
		DWORD dw = GetFullPathName(
			szFileName,  // file name
			MAX_PATH, // size of path buffer
			ss,     // path buffer
			NULL);
		memcpy(g_szFilePath,ss,sizeof(ss));
		return 0;
	}
	return -1;
}

int CLog::Close()
{
	if (NULL != m_pFile)
	{
		fclose(m_pFile);
		m_pFile = NULL;
	}
	return 0;
}

int CLog::Log( const char* psz,int nLen,char* pszHead,int nHeadLen)
{
	int nRes = 0;
	char tbuf[128] = {0};
	
	time_t t;
	struct tm *ptm;
	time(&t);
	ptm = localtime(&t);	
	sprintf(tbuf,"%d-%02d-%02d %02d:%02d:%02d\r\n%s:\r\n",
		ptm->tm_year+1900,ptm->tm_mon+1,ptm->tm_mday,ptm->tm_hour,ptm->tm_min,ptm->tm_sec,pszHead);
	if (NULL != m_pFile)
	{
		fwrite(tbuf,sizeof(char),strlen(tbuf),m_pFile);
		fwrite(psz,sizeof(char),nLen,m_pFile);
		fprintf(m_pFile,"\r\n\r\n");
		nRes = fflush(m_pFile);
	}
	
	return nRes;
}

int CLog::FormatLog( const char* pszFormat,... )
{
	int nRes = 0;
	char msg[MAX_MSG_LEN];
	char tbuf[64] = {0};
	
	va_list marker;
	va_start(marker,pszFormat);
	vsprintf(msg, pszFormat, marker);	
	va_end(marker);
	
	time_t t;
	struct tm *ptm;
	time(&t);
	ptm = localtime(&t);	
	sprintf(tbuf,"%d-%02d-%02d %02d:%02d:%02d",
		ptm->tm_year+1900,ptm->tm_mon+1,ptm->tm_mday,ptm->tm_hour,ptm->tm_min,ptm->tm_sec);
	
	if (NULL != m_pFile)
	{
		fprintf(m_pFile,"%s\r\n%s\r\n\r\n\r\n",tbuf,msg);
		nRes = fflush(m_pFile);
	}
	
	return nRes;
}

