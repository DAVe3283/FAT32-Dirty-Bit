//---------------------------------------------------------------------//
//
// Author: Sreejith S { http://paradiseprogramming.tripod.com }
// Please send bugs or fixes to sreejithsin@hotpop.com
//
//---------------------------------------------------------------------//
// DiskViewDoc.cpp : implementation of the CDiskViewDoc class
//

#include "stdafx.h"
#include "DiskView.h"

#include "DiskViewDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDiskViewDoc

IMPLEMENT_DYNCREATE(CDiskViewDoc, CDocument)

BEGIN_MESSAGE_MAP(CDiskViewDoc, CDocument)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_SEC, OnUpdateIndicatorSec)
	//{{AFX_MSG_MAP(CDiskViewDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDiskViewDoc construction/destruction

CDiskViewDoc::CDiskViewDoc()
{
	// TODO: add one-time construction code here

}

CDiskViewDoc::~CDiskViewDoc()
{
}

BOOL CDiskViewDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CDiskViewDoc serialization

void CDiskViewDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDiskViewDoc diagnostics

#ifdef _DEBUG
void CDiskViewDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CDiskViewDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDiskViewDoc commands

char * CDiskViewDoc::ReadSector(int drive, DWORD startinglogicalsector, int numberofsectors)
{
 // All msdos data structures must be packed on a 1 byte boundary
 #pragma pack (1)	
 struct 
 { 
	DWORD StartingSector ;
	WORD NumberOfSectors ;
	DWORD pBuffer;
 }ControlBlock;
 #pragma pack ()

 #pragma pack (1)
 typedef struct _DIOC_REGISTERS 
 {
    DWORD reg_EBX;
    DWORD reg_EDX;
    DWORD reg_ECX;
    DWORD reg_EAX;
    DWORD reg_EDI;
    DWORD reg_ESI;
    DWORD reg_Flags;
 } DIOC_REGISTERS ;
 #pragma pack ()

 char* buffer = (char*)malloc (512*numberofsectors);
 HANDLE hDevice ;
 DIOC_REGISTERS reg ;
 BOOL  fResult ;
 DWORD cb ;

 // Creating handle to vwin32.vxd (win 9x)
 hDevice = CreateFile ( "\\\\.\\vwin32",
		 0,
		 0,
		 NULL,
		 0,
		 FILE_FLAG_DELETE_ON_CLOSE,
		 NULL );

 if ( hDevice == INVALID_HANDLE_VALUE )
 {

	// win 2k code 
	HANDLE hDevice; 
	DWORD bytesread;
    
	// Creating a handle to drive a: using CreateFile () function ..
	char _devicename[] = "\\\\.\\A:";
	_devicename[4] += drive;
	hDevice = CreateFile(_devicename, 
        GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, 
        NULL, OPEN_EXISTING, 0, NULL); 
 
    if (hDevice == INVALID_HANDLE_VALUE) 
        return NULL;

	// Setting the pointer to point to the start of the sector we want to read ..
	SetFilePointer (hDevice, (startinglogicalsector*512), NULL, FILE_BEGIN); 

	if (!ReadFile (hDevice, buffer, 512*numberofsectors, &bytesread, NULL) )
		 return NULL;
 }
 else
 {
	 // code for win 95/98
	 ControlBlock.StartingSector = (DWORD)startinglogicalsector;
	 ControlBlock.NumberOfSectors = (DWORD)numberofsectors ;
	 ControlBlock.pBuffer =  (DWORD)buffer ;

	//-----------------------------------------------------------
	// SI contains read/write mode flags
	// SI=0h for read and SI=1h for write
	// CX must be equal to ffffh for
	// int 21h's 7305h extention
	// DS:BX -> base addr of the
	// control block structure
	// DL must contain the drive number
	// (01h=A:, 02h=B: etc)
	//-----------------------------------------------------------

	 reg.reg_ESI = 0x00 ;
	 reg.reg_ECX = -1 ; 
	 reg.reg_EBX = (DWORD)(&ControlBlock);
	 reg.reg_EDX = drive+1;
	 reg.reg_EAX = 0x7305 ;

	 //  6 == VWIN32_DIOC_DOS_DRIVEINFO
	 fResult = DeviceIoControl ( hDevice, 
		 6,
		 &(reg),
		 sizeof (reg),
 		 &(reg),
		 sizeof (reg),
		 &cb, 
		 0);
 
	 if (!fResult || (reg.reg_Flags & 0x0001)) return NULL; 		
 }

 CloseHandle(hDevice); 
 return buffer;
}


int CDiskViewDoc::LoadSectorBuffer()
{
	char hexcode[10];
	m_strSectorContents = (char *)malloc (512);
	m_strSectorContentsHex = (char *)malloc (512*5);
	m_strSectorContents = ReadSector (m_intDrive, m_dwStartingLogicalSector, m_intNumberOfSectors);
	// return if NULL
	if (!m_strSectorContents) return -1;
	// load HEX buffer
	sprintf (hexcode, "%X ", m_strSectorContents[0] & 0xff);
	strcpy (m_strSectorContentsHex, hexcode);
	for (int i=1; i<512; i++)
	{
		sprintf (hexcode, "%02X ", m_strSectorContents[i] & 0xff);
		strcat (m_strSectorContentsHex, hexcode);
	}
	UpdateAllViews (NULL);
	return 0;
}


void CDiskViewDoc::OnUpdateIndicatorSec(CCmdUI *pCmdUI)
{
	char strStatus[100];
	sprintf (strStatus, "Logical Sector %ld", m_dwStartingLogicalSector);
	pCmdUI->Enable (TRUE);
	pCmdUI->SetText (strStatus);
}
