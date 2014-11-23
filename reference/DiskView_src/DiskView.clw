; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CDiskViewView
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "DiskView.h"
LastPage=0

ClassCount=6
Class1=CDiskViewApp
Class2=CDiskViewDoc
Class3=CDiskViewView
Class4=CMainFrame

ResourceCount=3
Resource1=IDD_ABOUTBOX
Class5=CAboutDlg
Resource2=IDR_MAINFRAME
Class6=CGotoDlg
Resource3=IDD_GOTO_DIALOG

[CLS:CDiskViewApp]
Type=0
HeaderFile=DiskView.h
ImplementationFile=DiskView.cpp
Filter=N

[CLS:CDiskViewDoc]
Type=0
HeaderFile=DiskViewDoc.h
ImplementationFile=DiskViewDoc.cpp
Filter=N
LastObject=CDiskViewDoc

[CLS:CDiskViewView]
Type=0
HeaderFile=DiskViewView.h
ImplementationFile=DiskViewView.cpp
Filter=C
BaseClass=CView
VirtualFilter=VWC
LastObject=CDiskViewView


[CLS:CMainFrame]
Type=0
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
Filter=T
LastObject=CMainFrame
BaseClass=CFrameWnd
VirtualFilter=fWC




[CLS:CAboutDlg]
Type=0
HeaderFile=DiskView.cpp
ImplementationFile=DiskView.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=5
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1350696961
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342242816
Control5=IDC_STATIC,button,1342177287

[MNU:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_APP_EXIT
Command2=ID_VIEW_TOOLBAR
Command3=ID_VIEW_STATUS_BAR
Command4=ID_APP_ABOUT
CommandCount=4

[ACL:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_FILE_PRINT
Command5=ID_EDIT_UNDO
Command6=ID_EDIT_CUT
Command7=ID_EDIT_COPY
Command8=ID_EDIT_PASTE
Command9=ID_EDIT_UNDO
Command10=ID_EDIT_CUT
Command11=ID_EDIT_COPY
Command12=ID_EDIT_PASTE
Command13=ID_NEXT_PANE
Command14=ID_PREV_PANE
CommandCount=14

[TB:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_LEFT_BUTTON
Command2=ID_RIGHT_BUTTON
Command3=ID_GOTO_BUTTON
CommandCount=3

[DLG:IDD_GOTO_DIALOG]
Type=1
Class=CGotoDlg
ControlCount=4
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_LOGSEC_EDIT,edit,1350639744
Control4=IDC_STATIC,static,1342308352

[CLS:CGotoDlg]
Type=0
HeaderFile=GotoDlg.h
ImplementationFile=GotoDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=CGotoDlg
VirtualFilter=dWC

