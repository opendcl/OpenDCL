// TabPageDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TabPageDlg.h"
#include "Resource.h"


/////////////////////////////////////////////////////////////////////////////
// CTabPageDlg dialog

BEGIN_MESSAGE_MAP(CTabPageDlg, CDialog)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_SHOWWINDOW()
	ON_WM_CTLCOLOR()
	ON_MESSAGE(WM_DPICHANGED_AFTERPARENT, &CTabPageDlg::OnDpiChanged)
END_MESSAGE_MAP()


CTabPageDlg::CTabPageDlg( TDclFormPtr pSourceForm, CTabCtrl* pTabCtrl, CRect rectPane, UINT& nId )
: CDialog( IDD_TABPAGE, pTabCtrl )
, CArxDialogObject( pSourceForm, this )
, mColorService()
, mbRecalcQueued( false )
{
	GetControlPane()->CheckDpiChanged(); //reset mnDPI to zero for tab page control pane
	IgnoreSizing();
	CDialog::Create( IDD_TABPAGE, pTabCtrl );
	ShowWindow( SW_HIDE );
	MoveWindow( &rectPane) ;
	IgnoreSizing( false );
	ToDIP( rectPane );
	mpTemplate->SetLongProperty( Prop::Width, rectPane.Width() );
	mpTemplate->SetLongProperty( Prop::Height, rectPane.Height() );
	ApplyPropertiesEnum();
	GetControlPane()->CreateControls( nId );
	GetControlPane()->RecalcLayout();
}

CTabPageDlg::~CTabPageDlg()
{
}

void CTabPageDlg::ApplyPosition()
{
	if( IsEnumeratingProperties() )
		return; //defer
	bool bIgnoreSizing = IgnoreSizing();
	long lWidth = FromDIP( mpTemplate->GetLongProperty( Prop::Width ) );
	long lHeight = FromDIP( mpTemplate->GetLongProperty( Prop::Height ) );
	GetTopLevelWnd()->SetWindowPos( NULL, 0, 0,
																	lWidth + GetNCWidth(),
																	lHeight + GetNCHeight(),
																	SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER | /*SWP_NOCOPYBITS | */SWP_NOOWNERZORDER );
	if( GetTopLevelWnd()->IsWindowVisible() )
		mpControlPane->RecalcLayout();
	else
		mbRecalcQueued = true;
	IgnoreSizing( bIgnoreSizing );
}

/////////////////////////////////////////////////////////////////////////////
// CTabPageDlg message handlers

LRESULT CTabPageDlg::OnDpiChanged(WPARAM wParam, LPARAM lParam)
{
	HandleDpiChanged();
	return 0;
}

LRESULT CTabPageDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch( message )
	{
	case WM_PAINT:
		if( mbRecalcQueued )
		{
			mbRecalcQueued = false;
			mpControlPane->RecalcLayout();
			return 0;
		}
		break;
	}
	return __super::WindowProc(message, wParam, lParam);
}

BOOL CTabPageDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch( LOWORD(wParam) )
	{
	case IDCANCEL:
	case IDOK:
		return FALSE;
	}
	return __super::OnCommand( wParam, lParam );
}

void CTabPageDlg::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);
	if( IsIgnoreSizing() )
		return;
	mpTemplate->SetLongProperty( Prop::Width, ToDIP( cx ) );
	mpTemplate->SetLongProperty( Prop::Height, ToDIP( cy ) );
	if( IsWindowVisible() )
		mpControlPane->RecalcLayout();
	else
		mbRecalcQueued = true;
}

BOOL CTabPageDlg::OnEraseBkgnd(CDC* pDC)
{
	if( !IsWindowVisible() )
		return TRUE;
/*
	CRect rcTarget;
	pDC->GetClipBox( &rcTarget );
	CRect rcClient;
	GetClientRect( &rcClient );
	if( rcTarget.IntersectRect( &rcTarget, rcClient ) )
	{
		for( CWnd* pChild = GetWindow( GW_CHILD ); pChild; pChild = pChild->GetWindow( GW_HWNDNEXT ) )
		{
			if( !pChild->IsWindowVisible() )
				continue;
			if( pChild->GetExStyle() & WS_EX_TRANSPARENT )
				continue;
			CRect rcChild;
			pChild->GetWindowRect( &rcChild );
			ScreenToClient( &rcChild );
			if( rcChild.IntersectRect( &rcChild, &rcTarget ) )
				ValidateRect( &rcChild );
		}
	}
*/
	//CDialogControl::HandleEraseBkgnd( pDC ); //bypass CDialogObject to get transparency
	//return TRUE;
	return __super::OnEraseBkgnd(pDC);
}

void CTabPageDlg::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	__super::OnWindowPosChanging(lpwndpos);

	if( (lpwndpos->flags & SWP_SHOWWINDOW) != 0 && mbRecalcQueued )
	{
		mbRecalcQueued = false;
		mpControlPane->RecalcLayout();
	}
}

BOOL CTabPageDlg::PreTranslateMessage(MSG* pMsg)
{
	return CWnd::PreTranslateMessage(pMsg); //bypass CDialog
}

void CTabPageDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	__super::OnShowWindow(bShow, nStatus);
	if( bShow && mbRecalcQueued )
	{
		mbRecalcQueued = false;
		mpControlPane->RecalcLayout();
	}
}

HBRUSH CTabPageDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	LRESULT lResult;
	if (pWnd && pWnd->SendChildNotifyLastMsg(&lResult))
		return (HBRUSH)lResult;     // eat it
	if( nCtlColor < CTLCOLOR_MAX )
	{
		HBRUSH hbrBackground =
			(HBRUSH)GetParent()->SendMessage( (WM_CTLCOLORMSGBOX + nCtlColor), (WPARAM)pDC->GetSafeHdc(), (LPARAM)m_hWnd );
		if( hbrBackground )
			return hbrBackground;
	}
	return (HBRUSH)Default();
}
