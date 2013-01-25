; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CSendMailDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "netTest.h"

ClassCount=4
Class1=CNetTestApp
Class2=CNetTestDlg
Class3=CAboutDlg

ResourceCount=4
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Resource3=IDD_NETTEST_DIALOG
Class4=CSendMailDlg
Resource4=IDD_DIALOG_SENDMAIL

[CLS:CNetTestApp]
Type=0
HeaderFile=netTest.h
ImplementationFile=netTest.cpp
Filter=N

[CLS:CNetTestDlg]
Type=0
HeaderFile=netTestDlg.h
ImplementationFile=netTestDlg.cpp
Filter=D
BaseClass=CDialog
VirtualFilter=dWC
LastObject=CNetTestDlg

[CLS:CAboutDlg]
Type=0
HeaderFile=netTestDlg.h
ImplementationFile=netTestDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_NETTEST_DIALOG]
Type=1
Class=CNetTestDlg
ControlCount=7
Control1=IDOK,button,1073807361
Control2=IDCANCEL,button,1073807360
Control3=IDC_BUTTON_TEST,button,1342242816
Control4=IDC_EDIT_OUT,edit,1353777156
Control5=IDC_STATIC_WAIT,static,1342308352
Control6=IDC_BUTTON_SEND_MAIL,button,1073807360
Control7=IDC_BUTTON_CLEAR_INTERNET,button,1342242816

[DLG:IDD_DIALOG_SENDMAIL]
Type=1
Class=CSendMailDlg
ControlCount=9
Control1=IDOK,button,1073807361
Control2=IDCANCEL,button,1073807360
Control3=IDC_BUTTON_SEND,button,1342242816
Control4=IDC_EDIT_MIMI_ID,edit,1350631552
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,static,1342308352
Control7=IDC_EDIT_QQ_ID,edit,1350631552
Control8=IDC_STATIC,button,1342177287
Control9=IDC_STATIC,static,1342308352

[CLS:CSendMailDlg]
Type=0
HeaderFile=SendMailDlg.h
ImplementationFile=SendMailDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=IDC_EDIT_MIMI_ID
VirtualFilter=dWC

