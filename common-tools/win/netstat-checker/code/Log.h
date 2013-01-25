// Log.h: interface for the CLog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOG_H__ACD53A60_9BAD_4C81_A14F_6E2A0FEDAE9E__INCLUDED_)
#define AFX_LOG_H__ACD53A60_9BAD_4C81_A14F_6E2A0FEDAE9E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CLog  
{
public:
	CLog();
	virtual ~CLog();
public:
	int Log(const char* psz,int nLen,char* pszHead = NULL,int nHeadLen = 0);
	int FormatLog(const char* pszFormat,...);
private:
	int Open();
	int Close();
	FILE* m_pFile;
};

#endif // !defined(AFX_LOG_H__ACD53A60_9BAD_4C81_A14F_6E2A0FEDAE9E__INCLUDED_)
