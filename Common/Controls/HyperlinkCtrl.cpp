// HyperlinkCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "HyperlinkCtrl.h"
#include "ControlPane.h"
#include "Workspace.h"
#include "Resource.h"


/////////////////////////////////////////////////////////////////////////////
// CHyperlinkCtrl

COLORREF CHyperlinkCtrl::g_colorVisited   = RGB(128,0,128);		 // purple

CHyperlinkCtrl::CHyperlinkCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CDialogControl( pTemplate, pPane, this )
, mhHandCursor( LoadCursor( theWorkspace.GetLocalResourceModule(), MAKEINTRESOURCE(IDC_OHAND) ) )
{
	LOGFONT lfDefault;
	::GetObject( (HFONT)GetStockObject( DEFAULT_GUI_FONT ), sizeof(lfDefault), &lfDefault );
	mFont.CreateFontIndirect( &lfDefault );

	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CHyperlinkCtrl::~CHyperlinkCtrl()
{
}

bool CHyperlinkCtrl::Create( CWnd* pParentWnd, UINT nID )
{
	CString sWndClass = AfxRegisterWndClass( CS_HREDRAW | CS_VREDRAW, mhHandCursor );
	bool bSuccess = (__super::Create( sWndClass, GetWndCaption(), GetWndStyle(), GetWndRect(), pParentWnd, nID ) != FALSE);

	if( bSuccess && !ApplyPropertiesEnum() )
		bSuccess = false;

	return bSuccess;
}

DWORD CHyperlinkCtrl::GetWndStyle() const
{
	DWORD dwStyle = __super::GetWndStyle();

	return dwStyle;
}

CString CHyperlinkCtrl::GetWndCaption() const
{
	return mpTemplate->GetStringProperty( Prop::Caption );
}

bool CHyperlinkCtrl::ApplyProperty( TPropertyPtr pProp )
{
	if( !__super::ApplyProperty( pProp ) )
		return false;
	bool bFailed = false;
	switch( pProp->GetID() )
	{
	case Prop::Caption:
		SetWindowText( pProp->GetStringValue() );
		OnNeedRepaint();
		break;
	}
	return !bFailed;
}


BEGIN_MESSAGE_MAP(CHyperlinkCtrl, CWnd)
	ON_WM_NCHITTEST()
	ON_WM_LBUTTONDOWN()
	ON_WM_PAINT()
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_ERASEBKGND()
	ON_MESSAGE(WM_SETFONT, &CHyperlinkCtrl::OnSetFont)
	ON_MESSAGE(WM_DPICHANGED_AFTERPARENT, &CHyperlinkCtrl::OnDpiChanged)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CHyperlinkCtrl message handlers

LRESULT CHyperlinkCtrl::OnDpiChanged(WPARAM wParam, LPARAM lParam)
{
	HandleDpiChanged();
	return 0;
}

BOOL CHyperlinkCtrl::PreTranslateMessage(MSG* pMsg)
{
	GetToolTipCtrl().RelayEvent(pMsg);
	return __super::PreTranslateMessage(pMsg);
}

HBRUSH CHyperlinkCtrl::CtlColor(CDC* pDC, UINT nCtlColor)
{
	return HandleCtlColor( pDC, nCtlColor );
}

BOOL CHyperlinkCtrl::OnEraseBkgnd(CDC* pDC)
{
	if( mColorService.IsBackgroundTransparent() )
	{
		CWnd* pParent = mpControlWnd->GetParent();
		if( pParent )
		{
			HBRUSH hbrBackground = (HBRUSH)pParent->SendMessage( WM_CTLCOLORSTATIC, (WPARAM)pDC, (LPARAM)mpControlWnd->m_hWnd );
			if( hbrBackground )
			{
				CRect rcClip;
				pDC->GetClipBox( &rcClip );
				CRect rcClient;
				mpControlWnd->GetClientRect( &rcClient );
				rcClip.IntersectRect( &rcClip, &rcClient );
				pDC->FillRect( &rcClip, CBrush::FromHandle( hbrBackground ) );
				return TRUE;
			}
		}
	}
	if( HandleEraseBkgnd( pDC ) )
		return TRUE;
	return TRUE;
}

__UINT_LRESULT CHyperlinkCtrl::OnNcHitTest(CPoint point)
{
	return HTCLIENT;
}

void CHyperlinkCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	// Call ShellExecute to run the file. For a URL, this will run the browser.
	CString sHyperlink = mpTemplate->GetStringProperty( Prop::Hyperlink );
	if( sHyperlink.IsEmpty() )
	{
		MessageBeep( MB_ICONERROR );
		return;
	}
	HINSTANCE hShell = ShellExecute( 0, _T("open"), sHyperlink, 0, 0, SW_SHOWNORMAL );
	if ((UINT_PTR)hShell > 32)
	{ // success!
		mColorService.SetForegroundColor( g_colorVisited );
		OnNeedRepaint();
	}
	else
	{
		MessageBeep( MB_ICONERROR );
		TRACE( _T("*** WARNING: CHyperlinkCtrl: unable to open hyperlink [%s]\n"), (LPCTSTR)sHyperlink );
	}
}

void CHyperlinkCtrl::PostNcDestroy()
{
	__super::PostNcDestroy();
	delete this;
}

void CHyperlinkCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	dc.SaveDC();
	CString sText;
	GetWindowText( sText );
	if( !sText.IsEmpty() )
	{
		CRect rcClient;
		GetClientRect( &rcClient );
		CtlColor( &dc, CTLCOLOR_STATIC );
		dc.SelectObject( &mFont );
		dc.ExtTextOut( 0, 0, ETO_CLIPPED, &rcClient, sText, NULL );
	}
	dc.RestoreDC( -1 );
}

LRESULT CHyperlinkCtrl::OnSetFont(WPARAM wParam, LPARAM lParam)
{
	LOGFONT lf = { NULL };
	CFont::FromHandle( (HFONT)wParam )->GetLogFont( &lf );
	mFont.DeleteObject();
	mFont.CreateFontIndirect(&lf);
	if( LOWORD(lParam) )
		RedrawWindow();
	return 0;
}
