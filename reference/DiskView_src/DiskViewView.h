// DiskViewView.h : interface of the CDiskViewView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_DISKVIEWVIEW_H__C50F7168_717A_447A_8B52_B7E8EC328431__INCLUDED_)
#define AFX_DISKVIEWVIEW_H__C50F7168_717A_447A_8B52_B7E8EC328431__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CDiskViewView : public CView
{
protected: // create from serialization only
	CDiskViewView();
	DECLARE_DYNCREATE(CDiskViewView)

// Attributes
public:
	CDiskViewDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDiskViewView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDiskViewView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CDiskViewView)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in DiskViewView.cpp
inline CDiskViewDoc* CDiskViewView::GetDocument()
   { return (CDiskViewDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DISKVIEWVIEW_H__C50F7168_717A_447A_8B52_B7E8EC328431__INCLUDED_)
