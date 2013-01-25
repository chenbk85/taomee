#if !defined(AFX_SENDMAILDLG_H__FF3147D3_FF62_484E_9BEF_ED28C79A0FF3__INCLUDED_)
#define AFX_SENDMAILDLG_H__FF3147D3_FF62_484E_9BEF_ED28C79A0FF3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendMailDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSendMailDlg dialog

class CSendMailDlg : public CDialog
{
// Construction
public:
	CSendMailDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSendMailDlg)
	enum { IDD = IDD_DIALOG_SENDMAIL };
	CString	m_strMimiID;
	CString	m_strQqID;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSendMailDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSendMailDlg)
	afx_msg void OnButtonSend();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDMAILDLG_H__FF3147D3_FF62_484E_9BEF_ED28C79A0FF3__INCLUDED_)
