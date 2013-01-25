// SendMailDlg.cpp : implementation file
//

#include "stdafx.h"
#include "netTest.h"
#include "SendMailDlg.h"
#include "SMTP.h"
#include "config.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


extern char g_szFilePath[MAX_PATH];
/////////////////////////////////////////////////////////////////////////////
// CSendMailDlg dialog


CSendMailDlg::CSendMailDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSendMailDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSendMailDlg)
	m_strMimiID = _T("");
	m_strQqID = _T("");
	//}}AFX_DATA_INIT
}


void CSendMailDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendMailDlg)
	DDX_Text(pDX, IDC_EDIT_MIMI_ID, m_strMimiID);
	DDV_MaxChars(pDX, m_strMimiID, 64);
	DDX_Text(pDX, IDC_EDIT_QQ_ID, m_strQqID);
	DDV_MaxChars(pDX, m_strQqID, 64);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSendMailDlg, CDialog)
	//{{AFX_MSG_MAP(CSendMailDlg)
	ON_BN_CLICKED(IDC_BUTTON_SEND, OnButtonSend)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSendMailDlg message handlers

void CSendMailDlg::OnButtonSend() 
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_EDIT_MIMI_ID)->GetWindowText(m_strMimiID);
	GetDlgItem(IDC_EDIT_QQ_ID)->GetWindowText(m_strQqID);

	if(m_strMimiID.IsEmpty())
	{
		MessageBox("请填写米米号!");
		return;
	}


	char szSubject[128] = {0};
	if (!m_strMimiID.IsEmpty())
	{
		sprintf(szSubject,"%smimi id:%s    ",szSubject,m_strMimiID.GetBuffer(m_strMimiID.GetLength()));
		m_strMimiID.ReleaseBuffer();
	}

	if (!m_strQqID.IsEmpty())
	{
		sprintf(szSubject,"%sqq id:%s",szSubject,m_strQqID.GetBuffer(m_strQqID.GetLength()));
		m_strQqID.ReleaseBuffer();
	}
	CSmtp mail;
	
	if(mail.GetLastError() != CSMTP_NO_ERROR)
	{
		printf("Unable to initialise winsock2.\n");
		return;
	}
	
	mail.SetSMTPServer("smtp.yeah.net",25);
	mail.SetLogin("antigloss@yeah.net");
	mail.SetPassword("123asdqwezxc");
	mail.SetSenderName(szSubject);
	mail.SetSenderMail("antigloss@yeah.net");
	mail.SetReplyTo("antigloss@yeah.net");
	mail.SetSubject("netTest");
	mail.AddRecipient(SEND_TO_MAIL);
	mail.SetXPriority(XPRIORITY_NORMAL);
	mail.SetXMailer("The Bat! (v3.02) Professional");
	char szMessageBody[MAX_MSG_LEN]={0};
	sprintf(szMessageBody,"%s\r\n%s","This is netTest Mail.",szSubject);
	mail.SetMessageBody(szMessageBody);
	mail.AddAttachment(g_szFilePath);
	
	if( mail.Send())
	{
		MessageBox("邮件成功发送!问题处理完毕后,会致电给您!");
	}
	else
	{
		char szE[256]={0};
		sprintf(szE,"邮件发送失败:%s!\r\n请手动将%s文件发送至:%s邮箱,以待处理.",GetErrorText(mail.GetLastError()),
			g_szFilePath,SEND_TO_MAIL);
		MessageBox(szE);
	}
	CDialog::OnOK();
}
