//---------------------------------------------------------------------//
//
// Author: Sreejith S { http://paradiseprogramming.tripod.com }
// Please send bugs or fixes to sreejithsin@hotpop.com
//
//---------------------------------------------------------------------//
// DiskViewView.cpp : implementation of the CDiskViewView class
//

#include "stdafx.h"
#include "DiskView.h"

#include "DiskViewDoc.h"
#include "DiskViewView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDiskViewView

IMPLEMENT_DYNCREATE(CDiskViewView, CView)

BEGIN_MESSAGE_MAP(CDiskViewView, CView)
	//{{AFX_MSG_MAP(CDiskViewView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDiskViewView construction/destruction

CDiskViewView::CDiskViewView()
{
	// TODO: add construction code here

}

CDiskViewView::~CDiskViewView()
{
}

BOOL CDiskViewView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CDiskViewView drawing

void CDiskViewView::OnDraw(CDC* pDC)
{
	CDiskViewDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// init CBrush
	CBrush brush;
	brush.CreateSolidBrush (RGB(212,208,200));
	// set font
	CFont font;
	font.CreateFont (16, 0, 0, 0, FW_NORMAL, 
		FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, 
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH |
		FF_DONTCARE, "Terminal");
	pDC->SelectObject (&font);
	// Display sector contents in the view area
	// m_strSectorContents of document contains sector data
	CRect rect;
	GetClientRect (&rect);
	rect.NormalizeRect ();
	// Display sector contents
	if (pDoc->m_boolViewHex)
	{
		pDC->SetTextColor (RGB (0, 0, 255));
		pDC->DrawText (pDoc->m_strSectorContentsHex, strlen (pDoc->m_strSectorContentsHex), &rect, DT_WORDBREAK | DT_NOPREFIX);
	}
	else
	{
		pDC->SetTextColor (RGB (255, 0, 0));
		pDC->DrawText (pDoc->m_strSectorContents, 512, &rect, DT_WORDBREAK | DT_NOPREFIX);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDiskViewView printing

BOOL CDiskViewView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CDiskViewView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CDiskViewView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CDiskViewView diagnostics

#ifdef _DEBUG
void CDiskViewView::AssertValid() const
{
	CView::AssertValid();
}

void CDiskViewView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CDiskViewDoc* CDiskViewView::GetDocument() // non-debug version is inline
{ 
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDiskViewDoc)));
	return (CDiskViewDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDiskViewView message handlers

void CDiskViewView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();
	// Initialise document data
	CDiskViewDoc *pDOC = GetDocument ();
	pDOC->m_intDrive = 2;  // drive C:
	pDOC->m_dwStartingLogicalSector = 0;
	pDOC->m_intNumberOfSectors = 1;
	pDOC->SetTitle ("Paradise Programming presents");
	pDOC->m_boolViewHex = 0;
	pDOC->LoadSectorBuffer ();
}



