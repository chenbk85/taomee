// netTest.h : main header file for the NETTEST application
//

#if !defined(AFX_NETTEST_H__E77803C2_2737_4A35_99AC_458618F8E728__INCLUDED_)
#define AFX_NETTEST_H__E77803C2_2737_4A35_99AC_458618F8E728__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CNetTestApp:
// See netTest.cpp for the implementation of this class
//

class CNetTestApp : public CWinApp
{
public:
	CNetTestApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNetTestApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CNetTestApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NETTEST_H__E77803C2_2737_4A35_99AC_458618F8E728__INCLUDED_)
