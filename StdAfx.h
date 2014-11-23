// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__A83D3C43_7290_11E4_9F04_0003FFFAFFFF__INCLUDED_)
#define AFX_STDAFX_H__A83D3C43_7290_11E4_9F04_0003FFFAFFFF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#define SECTOR_SIZE 512			// Should be the case for Windows 98

#include <stdio.h>

#include "RawIO.h"
#include "Fat32Util.h"

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__A83D3C43_7290_11E4_9F04_0003FFFAFFFF__INCLUDED_)
