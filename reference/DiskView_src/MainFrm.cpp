//---------------------------------------------------------------------//
//
// Author: Sreejith S { http://paradiseprogramming.tripod.com }
// Please send bugs or fixes to sreejithsin@hotpop.com
//
//---------------------------------------------------------------------//
// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "DiskView.h"
#include "DiskViewDoc.h"
#include "DiskViewView.h"

#include "MainFrm.h"

#include <direct.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_LEFT_BUTTON, OnLeftButton)
	ON_COMMAND(ID_RIGHT_BUTTON, OnRightButton)
	ON_COMMAND(ID_GOTO_BUTTON, OnGotoButton)
	//}}AFX_MSG_MAP
	ON_CBN_SELCHANGE(IDC_CBDRIVE, OnSelChangeDrive)
	ON_CBN_SELCHANGE(IDC_CBVIEW, OnSelChangeView)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_SEC
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	int intTBCindex = m_wndToolBar.CommandToIndex (ID_LEFT_BUTTON);
	if (intTBCindex >= 0)
	{
		for (int i=intTBCindex; i < intTBCindex+3; i++)
			m_wndToolBar.SetButtonStyle (i, TBSTYLE_BUTTON);
	}

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	// Add the {Drive Select} Combo Box 
	m_wndToolBar.SetButtonInfo (4, IDC_CBDRIVE, TBBS_SEPARATOR, 100);
	// Get the toolbar height
	CRect rect;
	m_wndToolBar.GetItemRect (4, &rect);
	rect.bottom = rect.top + 125;
	// Create the combo box
	m_comboDriveSelect.Create (WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST,
		rect, &m_wndToolBar, IDC_CBDRIVE);

	// Initialize the combo box entries
	m_comboDriveSelect.Dir (DDL_DRIVES | DDL_EXCLUSIVE, _T("*.*"));

	// Change drive to first drive (this one exists for sure **
	_chdrive (3);

	// Set combo string to current drive
	CString strdrive = "[-a-]";
	strdrive.SetAt (2, strdrive.GetAt (2) + _getdrive ()-1);
	m_comboDriveSelect.SetCurSel (m_comboDriveSelect.FindString (0, strdrive));

	// Add the {View Select} Combo Box 
	m_wndToolBar.SetButtonInfo (6, IDC_CBVIEW, TBBS_SEPARATOR, 180);
	// Get the toolbar height
	m_wndToolBar.GetItemRect (6, &rect);
	rect.bottom = rect.top + 125;
	// Create the combo box
	m_comboViewSelect.Create (WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST,
		rect, &m_wndToolBar, IDC_CBVIEW);

	// Initialize combo box entries
	m_comboViewSelect.AddString ("View as Character text");
	m_comboViewSelect.AddString ("View as HEX");
	m_comboViewSelect.SetCurSel (0);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers


void CMainFrame::OnLeftButton() 
{
	// TODO: Add your command handler code here
	if (((CDiskViewDoc *)((CDiskViewView *)(GetActiveView ()))->GetDocument ())->m_dwStartingLogicalSector)
		((CDiskViewDoc *)((CDiskViewView *)(GetActiveView ()))->GetDocument ())->m_dwStartingLogicalSector--;
	((CDiskViewDoc *)((CDiskViewView *)(GetActiveView ()))->GetDocument ())->LoadSectorBuffer ();
}

void CMainFrame::OnRightButton() 
{
	// TODO: Add your command handler code here
	((CDiskViewDoc *)((CDiskViewView *)(GetActiveView ()))->GetDocument ())->m_dwStartingLogicalSector++;
	((CDiskViewDoc *)((CDiskViewView *)(GetActiveView ()))->GetDocument ())->LoadSectorBuffer ();
}

void CMainFrame::OnGotoButton() 
{
	// TODO: Add your command handler code here
	if (m_dlgGoto.DoModal () == IDOK)
	{
		((CDiskViewDoc *)((CDiskViewView *)(GetActiveView ()))->GetDocument ())->LoadSectorBuffer ();
	}

}

void CMainFrame::OnSelChangeDrive()
{
	CString strdrive;
	m_comboDriveSelect.GetLBText (m_comboDriveSelect.GetCurSel (), strdrive);
	((CDiskViewDoc *)((CDiskViewView *)(GetActiveView ()))->GetDocument ())->m_intDrive = strdrive.GetAt (2)-'a';
	((CDiskViewDoc *)((CDiskViewView *)(GetActiveView ()))->GetDocument ())->m_dwStartingLogicalSector = 0;
	((CDiskViewDoc *)((CDiskViewView *)(GetActiveView ()))->GetDocument ())->LoadSectorBuffer ();
}


void CMainFrame::OnSelChangeView()
{
	if (m_comboViewSelect.GetCurSel () == 0)
		((CDiskViewDoc *)((CDiskViewView *)(GetActiveView ()))->GetDocument ())->m_boolViewHex = 0;
	else
		((CDiskViewDoc *)((CDiskViewView *)(GetActiveView ()))->GetDocument ())->m_boolViewHex = 1;

	((CDiskViewDoc *)((CDiskViewView *)(GetActiveView ()))->GetDocument ())->LoadSectorBuffer ();
}


