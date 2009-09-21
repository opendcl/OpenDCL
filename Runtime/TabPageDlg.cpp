// TabPageDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TabPageDlg.h"


/////////////////////////////////////////////////////////////////////////////
// CTabPageDlg dialog

BEGIN_MESSAGE_MAP(CTabPageDlg, CDialog)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

CTabPageDlg::CTabPageDlg( TDclFormPtr pSourceForm, CTabCtrl* pTabCtrl, CRect rectPane, UINT& nId )
: CDialog(CTabPageDlg::IDD, pTabCtrl)
, CDialogObject( pSourceForm, &mControlPane, this )
, mControlPane( pSourceForm, this )
, mbRecalcQueued( false )
{
	IgnoreSizing();
	CDialog::Create( CTabPageDlg::IDD, pTabCtrl );
	ShowWindow( SW_HIDE );
	MoveWindow( &rectPane) ;
	IgnoreSizing( false );
	mpTemplate->SetLongProperty( Prop::Width, rectPane.Width() );
	mpTemplate->SetLongProperty( Prop::Height, rectPane.Height() );
	ApplyPropertiesEnum();
	mControlPane.CreateControls( nId );
	mControlPane.RecalcLayout();
}

CTabPageDlg::~CTabPageDlg()
{
}

void CTabPageDlg::ApplyPosition()
{
	bool bIgnoreSizing = IgnoreSizing();
	GetTopLevelWnd()->SetWindowPos( NULL, 0, 0,
																	mpTemplate->GetLongProperty(Prop::Width) + GetNCWidth(),
																	mpTemplate->GetLongProperty(Prop::Height) + GetNCHeight(),
																	SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | /*SWP_NOCOPYBITS | */SWP_NOOWNERZORDER );
	if( GetTopLevelWnd()->IsWindowVisible() )
		mpControlPane->RecalcLayout();
	else
		mbRecalcQueued = true;
	IgnoreSizing( bIgnoreSizing );
}

/////////////////////////////////////////////////////////////////////////////
// CTabPageDlg message handlers

LRESULT CTabPageDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch( message )
	{
	case WM_PAINT:
		if( mbRecalcQueued )
		{
			mbRecalcQueued = false;
			mpControlPane->RecalcLayout();
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
	mpTemplate->SetLongProperty( Prop::Width, cx );
	mpTemplate->SetLongProperty( Prop::Height, cy );
	if( IsWindowVisible() )
		mpControlPane->RecalcLayout();
	else
		mbRecalcQueued = true;
}

BOOL CTabPageDlg::OnEraseBkgnd(CDC* pDC)
{
	if( mpControlPane->GetThemeHelper() )
	{
		TDclFormPtr pParentForm = mpSourceForm->GetParentForm();
		if( pParentForm )
		{
			TDclControlPtr pTabStrip = pParentForm->FindFirstControlOfType( CtlTabStrip );
			if( pTabStrip && pTabStrip->GetBooleanProperty( Prop::UseVisualStyle ) )
			{
				CRect rcClient;
				GetClientRect( &rcClient );
				pDC->FillSolidRect( &rcClient, GetSysColor( COLOR_WINDOW ) );
				return TRUE;
			}
		}
	}
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
