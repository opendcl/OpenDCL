// DIBStatic.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1998 by Jorge Lodos
// All rights reserved
//
// Distribute and use freely, except:
// 1. Don't alter or remove this notice.
// 2. Mark the changes you made
//
// Send bug reports, bug fixes, enhancements, requests, etc. to:
//    lodos@cigb.edu.cu
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "DIB.h"


/////////////////////////////////////////////////////////////////////////////
// CDIBStatic window

class CDIBStatic : public CStatic
{
// Construction
public:
	CDIBStatic();

// Attributes
public:
	CDib m_DIB;

public:
	BOOL IsValidDib() const { return (m_DIB.IsValid()); }

// Operations
public:
	BOOL LoadDib(LPCTSTR lpszFileName);
	BOOL LoadDib(CFile& file);
	void UpdateDib();
	void RemoveDib() { m_DIB.Invalidate(); UpdateDib(); }
	
	BOOL DoRealizePalette(BOOL bForceBackGround);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDIBStatic)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDIBStatic();

protected:
	void ClearDib();
	void PaintDib(BOOL bDibValid);

	// Generated message map functions
protected:
	//{{AFX_MSG(CDIBStatic)
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg BOOL OnQueryNewPalette();
	afx_msg void OnPaletteChanged(CWnd* pFocusWnd);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
