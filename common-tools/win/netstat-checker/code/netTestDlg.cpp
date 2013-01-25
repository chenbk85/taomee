// netTestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "netTest.h"
#include "netTestDlg.h"
#include "Net.h"
#include "st.h"
#include "SendMailDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNetTestDlg dialog

CNetTestDlg::CNetTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNetTestDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNetTestDlg)
	m_strOutEdit = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CNetTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNetTestDlg)
	DDX_Control(pDX, IDC_EDIT_OUT, m_ctrOutEdit);
	DDX_Text(pDX, IDC_EDIT_OUT, m_strOutEdit);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CNetTestDlg, CDialog)
	//{{AFX_MSG_MAP(CNetTestDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_TEST, OnButtonTest)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_SEND_MAIL, OnButtonSendMail)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR_INTERNET, OnButtonClearInternet)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_MSG_TEST_DONE,TestDone)
	ON_MESSAGE(WM_MSG_SHOW_OUT_EDIT,ShowOutEdit)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNetTestDlg message handlers

BOOL CNetTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	m_iNet.Init(this);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CNetTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CNetTestDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CNetTestDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CNetTestDlg::OnButtonTest() 
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_BUTTON_TEST)->EnableWindow(FALSE);
	SetTimer(1,1000,NULL);
	m_strOutEdit.Empty();
	m_ctrOutEdit.UpdateData(FALSE);
	m_iNet.BeginTest();
}

void CNetTestDlg::TestDone(WPARAM wParam,LPARAM lParam)
{
	m_iNet.WaitThread();
	KillTimer(1);
	GetDlgItem(IDC_STATIC_WAIT)->SetWindowText("测试完毕!点击发送至结果处理邮箱--->>>");
	GetDlgItem(IDC_BUTTON_SEND_MAIL)->ShowWindow(SW_SHOWNORMAL);
	MessageBox("测试完毕!");
	GetDlgItem(IDC_BUTTON_TEST)->EnableWindow(TRUE);
}

void CNetTestDlg::ShowOutEdit(WPARAM wParam,LPARAM lParam)
{
	CString strS = "\r\n";
	st_ShowOutEdit* pst = (st_ShowOutEdit*)wParam;
	if (0 == lParam)
	{//成功
		if ((NULL == pst->pszData) || (0 == pst->nDataLen))
		{
			m_strOutEdit += pst->pszMsg + strS;
			CString   str;  
		//	m_ctrOutEdit.GetWindowText(str);  
			m_ctrOutEdit.SetWindowText(m_strOutEdit);
			m_ctrOutEdit.UpdateData(FALSE);
		}
		else
		{
			m_strOutEdit += pst->pszMsg + strS + pst->pszData + strS;
			m_ctrOutEdit.SetWindowText(m_strOutEdit);
			m_ctrOutEdit.UpdateData(FALSE);
		}
	}
	else
	{
		//失败
		m_strOutEdit += pst->pszMsg + strS;
		m_ctrOutEdit.UpdateData(FALSE);
	}
}

void CNetTestDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	static int n = 1;
	if (n < 6)
	{
		n++;
	}
	else
	{
		n = 1;
	}
	char sz[64] = {0};
	strcpy(sz,"测试中,请耐心等待.");
	for (int i = 0;i < n;i++)
	{
		sprintf(sz,"%s%s",sz,"..");
	}
	GetDlgItem(IDC_STATIC_WAIT)->SetWindowText(sz);

	CDialog::OnTimer(nIDEvent);
}

void CNetTestDlg::OnButtonSendMail() 
{
	// TODO: Add your control notification handler code here
	CSendMailDlg dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
		CDialog::OnOK();
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}
//	CDialog::OnOK();

}
void CNetTestDlg::OnButtonClearInternet() 
{
	// TODO: Add your control notification handler code here
	m_iDustman.ClearInternetTempFile();
	MessageBox("IE InternetTempFile 清理完毕.",CAPTION_NAME, MB_OK );
}


