// DiskViewDoc.h : interface of the CDiskViewDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_DISKVIEWDOC_H__086662FA_B7E2_47EA_8CCA_0073E0408341__INCLUDED_)
#define AFX_DISKVIEWDOC_H__086662FA_B7E2_47EA_8CCA_0073E0408341__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CDiskViewDoc : public CDocument
{
protected: // create from serialization only
	CDiskViewDoc();
	DECLARE_DYNCREATE(CDiskViewDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDiskViewDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	afx_msg void OnUpdateIndicatorSec (CCmdUI *pCmdUI);
	BOOL m_boolViewHex;
	char * m_strSectorContentsHex;
	int LoadSectorBuffer ();
	int m_intNumberOfSectors;
	DWORD m_dwStartingLogicalSector;
	int m_intDrive;
	char * ReadSector (int drive, DWORD startinglogicalsector, int numberofsectors);
	char * m_strSectorContents;
	virtual ~CDiskViewDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CDiskViewDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DISKVIEWDOC_H__086662FA_B7E2_47EA_8CCA_0073E0408341__INCLUDED_)
