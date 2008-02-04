// ImageTreeHolderCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ImageTreeHolderCtrl.h"
#include "ImageTreeCtrl.h"


/////////////////////////////////////////////////////////////////////////////
// CImageTreeHolderCtrl

CImageTreeHolderCtrl::CImageTreeHolderCtrl( CImageTreeCtrl* pImageTreeCtrl,
																						const CRect& rcWnd,
																						CWnd* pParentWnd )
: mpImageTreeCtrl( pImageTreeCtrl )
{
	Create( NULL, WS_VISIBLE | WS_CHILD | WS_TABSTOP, rcWnd, pParentWnd );
}

CImageTreeHolderCtrl::~CImageTreeHolderCtrl()
{
}


BEGIN_MESSAGE_MAP(CImageTreeHolderCtrl, CStatic)
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CImageTreeHolderCtrl message handlers

void CImageTreeHolderCtrl::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize( nType, cx, cy );
	if( mpImageTreeCtrl->m_hWnd )
		mpImageTreeCtrl->SetWindowPos( NULL, 0, 0, cx, cy, SWP_NOMOVE | SWP_NOZORDER );
}

void CImageTreeHolderCtrl::OnSetFocus(CWnd* pOldWnd)
{
	__super::OnSetFocus( pOldWnd );
	mpImageTreeCtrl->SetFocus();
}
