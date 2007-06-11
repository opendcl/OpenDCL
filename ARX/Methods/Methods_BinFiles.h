#if !defined(AFX_METHODS_BINFILES_H__B067FCA1_F3B6_11D5_8AE5_FE55C959156E__INCLUDED_)
#define AFX_METHODS_BINFILES_H__B067FCA1_F3B6_11D5_8AE5_FE55C959156E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Methods_BinFiles.h : header file
//

#include "stdafx.h"

class CLispFileObject : public CObject
{
public:
	CLispFileObject()
	{
		m_pArchive = NULL;
		m_bFoundEnd = false;
		m_bFoundError = false;
	}
	~CLispFileObject(){}
	CArchive	*m_pArchive;
	CFile		m_File;
	CString		m_Style;
	bool		m_bFoundEnd;
	bool		m_bFoundError;
};

int OpenBinFile();
int WriteBinFile();
int ReadBinFile();
int CloseBinFile();
int CheckBinFile();




//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_METHODS_BINFILES_H__B067FCA1_F3B6_11D5_8AE5_FE55C959156E__INCLUDED_)
