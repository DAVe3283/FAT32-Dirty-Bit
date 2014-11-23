#if !defined(AFX_GOTODLG_H__4FBC7E81_7D5E_42A3_83EB_7D6AC12DCE6E__INCLUDED_)
#define AFX_GOTODLG_H__4FBC7E81_7D5E_42A3_83EB_7D6AC12DCE6E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GotoDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGotoDlg dialog

class CGotoDlg : public CDialog
{
// Construction
public:
	CGotoDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGotoDlg)
	enum { IDD = IDD_GOTO_DIALOG };
	DWORD	m_dwLogicalSector;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGotoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGotoDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GOTODLG_H__4FBC7E81_7D5E_42A3_83EB_7D6AC12DCE6E__INCLUDED_)
