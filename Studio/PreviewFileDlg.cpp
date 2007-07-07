// PreviewFileDlg.cpp : implementation file
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

#include "stdafx.h"
#include "PreviewFileDlg.h"
#include "Resource.h"


/////////////////////////////////////////////////////////////////////////////
// CPreviewFileDlg

IMPLEMENT_DYNAMIC(CPreviewFileDlg, CFileDialog)

CPreviewFileDlg::CPreviewFileDlg(BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName,
		DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd) :
		CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
{
	m_ofn.Flags |= (OFN_EXPLORER | OFN_ENABLETEMPLATE);
	m_ofn.lpTemplateName = MAKEINTRESOURCE(IDD_FILEOPENPREVIEW);

	m_bPreview = TRUE;
}


BEGIN_MESSAGE_MAP(CPreviewFileDlg, CFileDialog)
	//{{AFX_MSG_MAP(CPreviewFileDlg)
	ON_BN_CLICKED(IDC_PREVIEW, OnPreview)
	ON_WM_QUERYNEWPALETTE()
	ON_WM_PALETTECHANGED()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CPreviewFileDlg::OnInitDialog() 
{
	CFileDialog::OnInitDialog();

	m_DIBStaticCtrl.SubclassDlgItem(IDC_IMAGE, this);
// if exception problem still exists whan selecting picture files change this next line to get the CWnd and modify that way
	GetDlgItem(IDC_PREVIEW)->SendMessage(BM_SETCHECK, (m_bPreview) ? 1 : 0);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX PropertyObject Pages should return FALSE
}

void CPreviewFileDlg::OnFileNameChange() 
{
	CFileDialog::OnFileNameChange();
	if (m_bPreview)
		m_DIBStaticCtrl.LoadDib(GetPathName()); // the control will handle errors
}

void CPreviewFileDlg::OnFolderChange() 
{
	CFileDialog::OnFolderChange();
	m_DIBStaticCtrl.RemoveDib();
}

void CPreviewFileDlg::OnPreview() 
{
	m_bPreview = !m_bPreview;
	if (!m_bPreview)
		m_DIBStaticCtrl.RemoveDib(); // no preview
	else
		m_DIBStaticCtrl.LoadDib(GetPathName()); // the control will handle errors
}

BOOL CPreviewFileDlg::OnQueryNewPalette() 
{
	m_DIBStaticCtrl.SendMessage(WM_QUERYNEWPALETTE);	// redo the palette if necessary
	return CFileDialog::OnQueryNewPalette();
}

void CPreviewFileDlg::OnPaletteChanged(CWnd* pFocusWnd) 
{
	CFileDialog::OnPaletteChanged(pFocusWnd);
	m_DIBStaticCtrl.SendMessage(WM_PALETTECHANGED, (WPARAM)pFocusWnd->GetSafeHwnd());	// redo the palette if necessary
}

void CPreviewFileDlg::OnSetFocus(CWnd* pOldWnd) 
{
	CFileDialog::OnSetFocus(pOldWnd);
	m_DIBStaticCtrl.SendMessage(WM_QUERYNEWPALETTE);	// redo the palette if necessary
}

#ifdef _DEBUG
void CPreviewFileDlg::Dump(CDumpContext& dc) const
{
	CFileDialog::Dump(dc);
	if (m_bPreview)
		dc << "preview is enabled\n";
	else
		dc << "preview is disabled\n";
}
#endif //_DEBUG
