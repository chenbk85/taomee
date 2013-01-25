// netTestDlg.h : header file
//

#if !defined(AFX_NETTESTDLG_H__CADFEDC8_3031_4417_B6EF_977B5FBD1BD6__INCLUDED_)
#define AFX_NETTESTDLG_H__CADFEDC8_3031_4417_B6EF_977B5FBD1BD6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CNetTestDlg dialog
#include "Net.h"
#include "Dustman.h"
#define WM_MSG_TEST_DONE WM_USER+100
#define WM_MSG_SHOW_OUT_EDIT WM_USER+101
#define CAPTION_NAME "ÌÔÃ×ÍøÂç¼ì²â¹¤¾ß"
class CNetTestDlg : public CDialog
{
// Construction
public:
	CNetTestDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CNetTestDlg)
	enum { IDD = IDD_NETTEST_DIALOG };
	CEdit	m_ctrOutEdit;
	CString	m_strOutEdit;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNetTestDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CNetTestDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonTest();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void TestDone(WPARAM wParam,LPARAM lParam);
	afx_msg void ShowOutEdit(WPARAM wParam,LPARAM lParam);
	afx_msg void OnButtonSendMail();
	afx_msg void OnButtonClearInternet();
	//}}AFX_MSG
	
	DECLARE_MESSAGE_MAP()
private:
	CNet m_iNet;
	CDustman m_iDustman;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NETTESTDLG_H__CADFEDC8_3031_4417_B6EF_977B5FBD1BD6__INCLUDED_)
