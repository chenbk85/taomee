// Net.h: interface for the CNet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NET_H__F995A757_9493_4DCB_A5D4_7F095785DCBE__INCLUDED_)
#define AFX_NET_H__F995A757_9493_4DCB_A5D4_7F095785DCBE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <string>
using namespace std;
#include "NetSock.h"
#include "Log.h"
class CNetTestDlg;
class CNet  
{
public:
	CNet();
	virtual ~CNet();
public:
	int Init(CNetTestDlg* piNetTestDlg);
	int BeginTest();
	int WaitThread();
private:
	HANDLE m_hThreadTest;
	static unsigned __stdcall ThreadTest(void* pData);
	CNetTestDlg* m_piNetTestDlg;
};

#endif // !defined(AFX_NET_H__F995A757_9493_4DCB_A5D4_7F095785DCBE__INCLUDED_)
