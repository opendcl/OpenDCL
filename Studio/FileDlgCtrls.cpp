// FileDlgCtrls.cpp : implementation file
//

#include "stdafx.h"
#include "FileDlgCtrls.h"
#include "Workspace.h"
#include "Resource.h"


/////////////////////////////////////////////////////////////////////////////
// CFileDlgCtrls

CFileDlgCtrls::CFileDlgCtrls()
: CStatic()
, mszBitmap( -1, -1 )
{
}

CFileDlgCtrls::~CFileDlgCtrls()
{
}

bool CFileDlgCtrls::Initialize()
{
	HBITMAP hbmpFileDlgCtrl = ::LoadBitmap( theWorkspace.GetResourceModule(), MAKEINTRESOURCE(IDB_FILEDLGCTRL) );
	if( !hbmpFileDlgCtrl )
		return false;
	CBitmap bmpScreen;
	bmpScreen.Attach( hbmpFileDlgCtrl );
	BITMAP bmpInfo;
	bmpScreen.GetBitmap( &bmpInfo );
	SetBitmap( (HBITMAP)bmpScreen.Detach() );
	MoveWindow( 0, 0, bmpInfo.bmWidth, bmpInfo.bmHeight );
	mszBitmap.SetSize( bmpInfo.bmWidth, bmpInfo.bmHeight );
	return true;
}

BEGIN_MESSAGE_MAP(CFileDlgCtrls, CStatic)
	ON_WM_SIZE()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CFileDlgCtrls message handlers

void CFileDlgCtrls::OnSize(UINT nType, int cx, int cy) 
{
	if ((mszBitmap.cx > 0 && cx != mszBitmap.cx) || (mszBitmap.cy > 0 && cy != mszBitmap.cy))
	{
		CRect rcThis(0, 0, mszBitmap.cx, cy);
		MoveWindow( 0, 0, mszBitmap.cx, mszBitmap.cy, TRUE);
		return;
	}
	CStatic::OnSize(nType, cx, cy);
}

BOOL CFileDlgCtrls::CreateEx(DWORD dwExStyle, LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, LPVOID lpParam)
{
	dwStyle |= (SS_BITMAP | SS_CENTERIMAGE);
	BOOL bResult = __super::CreateEx(dwExStyle, lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, lpParam);
	if( !bResult )
		return FALSE;
	return Initialize()? TRUE : FALSE;
}

BOOL CFileDlgCtrls::Create(LPCTSTR lpszText, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	dwStyle |= (SS_BITMAP | SS_CENTERIMAGE);
	BOOL bResult = __super::Create(lpszText, dwStyle, rect, pParentWnd, nID);
	if( !bResult )
		return FALSE;
	return Initialize()? TRUE : FALSE;
}
