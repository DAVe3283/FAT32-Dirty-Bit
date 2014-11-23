//---------------------------------------------------------------------//
//
// Author: Sreejith S { http://paradiseprogramming.tripod.com }
// Please send bugs or fixes to sreejithsin@hotpop.com
//
//---------------------------------------------------------------------//
// GotoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DiskView.h"
#include "GotoDlg.h"
#include "DiskViewDoc.h"
#include "DiskViewView.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGotoDlg dialog


CGotoDlg::CGotoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGotoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGotoDlg)
	m_dwLogicalSector = 0;
	//}}AFX_DATA_INIT
}


void CGotoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGotoDlg)
	DDX_Text(pDX, IDC_LOGSEC_EDIT, m_dwLogicalSector);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGotoDlg, CDialog)
	//{{AFX_MSG_MAP(CGotoDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGotoDlg message handlers

BOOL CGotoDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_dwLogicalSector = ((CDiskViewDoc *)((CMainFrame *)AfxGetMainWnd ())->GetActiveDocument ())->m_dwStartingLogicalSector;
	UpdateData (FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CGotoDlg::OnOK() 
{
	UpdateData (TRUE);
	((CDiskViewDoc *)((CMainFrame *)AfxGetMainWnd ())->GetActiveDocument ())->m_dwStartingLogicalSector = m_dwLogicalSector;
	CDialog::OnOK();
}

