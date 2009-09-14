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
	__if_exists(m_bVistaStyle)
	{
		CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd, 0, FALSE)
	}
	__if_not_exists(m_bVistaStyle)
	{
		CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
	}
{
	m_ofn.Flags |= (OFN_EXPLORER | OFN_ENABLETEMPLATE);
	m_ofn.lpTemplateName = MAKEINTRESOURCE(IDD_FILEOPENPREVIEW);
	m_bPreview = TRUE;
}


BEGIN_MESSAGE_MAP(CPreviewFileDlg, CFileDialog)
	ON_BN_CLICKED(IDC_PREVIEW, OnPreview)
	ON_WM_QUERYNEWPALETTE()
	ON_WM_PALETTECHANGED()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()


BOOL CPreviewFileDlg::OnInitDialog() 
{
	CFileDialog::OnInitDialog();

	m_Preview.SubclassDlgItem(IDC_IMAGE, this);
// if exception problem still exists whan selecting picture files change this next line to get the CWnd and modify that way
	GetDlgItem(IDC_PREVIEW)->SendMessage(BM_SETCHECK, (m_bPreview) ? 1 : 0);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX PropertyObject Pages should return FALSE
}

void CPreviewFileDlg::OnFileNameChange() 
{
	CFileDialog::OnFileNameChange();
	if( m_bPreview && m_Preview.m_hWnd )
		m_Preview.LoadPictureFile( GetPathName(), true );
}

void CPreviewFileDlg::OnFolderChange() 
{
	CFileDialog::OnFolderChange();
	m_Preview.Clear();
}

void CPreviewFileDlg::OnPreview() 
{
	m_bPreview = !m_bPreview;
	if( !m_bPreview )
		m_Preview.Clear(); // no preview
	else if( m_Preview.m_hWnd )
		m_Preview.LoadPictureFile( GetPathName(), true );
}

BOOL CPreviewFileDlg::OnQueryNewPalette() 
{
	m_Preview.SendMessage(WM_QUERYNEWPALETTE);	// redo the palette if necessary
	return CFileDialog::OnQueryNewPalette();
}

void CPreviewFileDlg::OnPaletteChanged(CWnd* pFocusWnd) 
{
	CFileDialog::OnPaletteChanged(pFocusWnd);
	m_Preview.SendMessage(WM_PALETTECHANGED, (WPARAM)pFocusWnd->GetSafeHwnd());	// redo the palette if necessary
}

void CPreviewFileDlg::OnSetFocus(CWnd* pOldWnd) 
{
	CFileDialog::OnSetFocus(pOldWnd);
	m_Preview.SendMessage(WM_QUERYNEWPALETTE);	// redo the palette if necessary
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
