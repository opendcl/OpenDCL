// TrueColorDialog.cpp : implementation file
//

#include "stdafx.h"
#include "TrueColorDialog.h"
#include "Workspace.h"

#ifndef _WIN32_WCE // CColorDialog is not supported for Windows CE.

// CTrueColorDialog

IMPLEMENT_DYNAMIC(CTrueColorDialog, CColorDialog)

CTrueColorDialog::CTrueColorDialog(COLORREF clrInit, DWORD dwFlags, CWnd* pParentWnd) :
	CColorDialog(clrInit, dwFlags, pParentWnd)
	, mbMustDeleteCustomColors( false )
{
	if( !m_cc.lpCustColors )
	{
		m_cc.lpCustColors = new COLORREF[16];
		mbMustDeleteCustomColors = true;
	}
	CWinApp* pApp = AfxGetApp();
	static const CString sSection = theWorkspace.GetAppKey();
	for( int idx = 0; idx < 16; ++idx )
	{
		CString sVal;
		sVal.Format( _T("CustomColor%d"), idx + 1 );
		m_cc.lpCustColors[idx] = pApp->GetProfileInt( sSection, sVal, m_cc.lpCustColors[idx] );
	}
}

CTrueColorDialog::~CTrueColorDialog()
{
	CWinApp* pApp = AfxGetApp();
	static const CString sSection = theWorkspace.GetAppKey();
	for( int idx = 0; idx < 16; ++idx )
	{
		CString sVal;
		sVal.Format( _T("CustomColor%d"), idx + 1 );
		pApp->WriteProfileInt( sSection, sVal, m_cc.lpCustColors[idx] );
	}
	if( mbMustDeleteCustomColors )
		delete[] m_cc.lpCustColors;
}


BEGIN_MESSAGE_MAP(CTrueColorDialog, CColorDialog)
END_MESSAGE_MAP()



// CTrueColorDialog message handlers


#endif // !_WIN32_WCE
