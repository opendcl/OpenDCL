// PreviewFileDlg.h : header file
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

//#include "DIB.h"
#include "DIBStatic.h"


/////////////////////////////////////////////////////////////////////////////
// CPreviewFileDlg dialog

class CPreviewFileDlg : public CFileDialog
{
	DECLARE_DYNAMIC(CPreviewFileDlg)

public:
	CPreviewFileDlg(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
		LPCTSTR lpszDefExt = NULL,
		LPCTSTR lpszFileName = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCTSTR lpszFilter = NULL,
		CWnd* pParentWnd = NULL);

// Attributes
public:
  BOOL m_bPreview;
  CDIBStatic m_DIBStaticCtrl;

protected:
	//{{AFX_MSG(CPreviewFileDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPreview();
	afx_msg BOOL OnQueryNewPalette();
	afx_msg void OnPaletteChanged(CWnd* pFocusWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG
	virtual void OnFileNameChange();
	virtual void OnFolderChange();

// Implementation
#ifdef _DEBUG
	virtual void Dump(CDumpContext& dc) const;
#endif
	DECLARE_MESSAGE_MAP()
};
