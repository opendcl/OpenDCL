// UrlLinkCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "UrlLinkCtrl.h"
#include "ControlPane.h"
#include "Workspace.h"
#include "Resource.h"


/////////////////////////////////////////////////////////////////////////////
// CUrlLinkCtrl

COLORREF CUrlLinkCtrl::g_colorVisited   = RGB(128,0,128);		 // purple

CUrlLinkCtrl::CUrlLinkCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CDialogControl( pTemplate, pPane, this )
, mhHandCursor( LoadCursor( theWorkspace.GetLocalResourceModule(), MAKEINTRESOURCE(IDC_HAND) ) )
{
	LOGFONT lfDefault;
	::GetObject( (HFONT)GetStockObject( DEFAULT_GUI_FONT ), sizeof(lfDefault), &lfDefault );
	mFont.CreateFontIndirect( &lfDefault );

	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CUrlLinkCtrl::~CUrlLinkCtrl()
{
}

bool CUrlLinkCtrl::Create( CWnd* pParentWnd, UINT nID ) 
{
	CString sWndClass = AfxRegisterWndClass( CS_HREDRAW | CS_VREDRAW, mhHandCursor );
	bool bSuccess = (__super::Create( sWndClass, GetWndCaption(), GetWndStyle(), GetWndRect(), pParentWnd, nID ) != FALSE);

	if( bSuccess && !ApplyPropertiesEnum() )
		bSuccess = false;

	return bSuccess;
}

DWORD CUrlLinkCtrl::GetWndStyle() const
{
	DWORD dwStyle = __super::GetWndStyle();

	return dwStyle;
}

CString CUrlLinkCtrl::GetWndCaption() const
{
	return mpTemplate->GetStringProperty( Prop::URLAddress );
}

bool CUrlLinkCtrl::OnApplyProperty( TPropertyPtr pProp )
{
	if( !__super::OnApplyProperty( pProp ) )
		return false;
	bool bFailed = false;
	switch( pProp->GetID() )
	{
	case Prop::URLLinkType:
		break;
	case Prop::URLAddress:
		{
			SetWindowText( pProp->GetStringValue() );
			Invalidate();
		}
		break;
	}
	return !bFailed;
}


BEGIN_MESSAGE_MAP(CUrlLinkCtrl, CWnd)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_NCHITTEST()
	ON_WM_LBUTTONDOWN()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_MESSAGE(WM_SETFONT, &CUrlLinkCtrl::OnSetFont)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CUrlLinkCtrl message handlers

BOOL CUrlLinkCtrl::PreTranslateMessage(MSG* pMsg) 
{
	GetToolTipCtrl().RelayEvent(pMsg);
	return __super::PreTranslateMessage(pMsg);
}

HBRUSH CUrlLinkCtrl::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	if( !IsWindowEnabled() )
		return NULL;
	return mAcadColorService.CtlColor( pDC, nCtlColor );
}

__UINT_LRESULT CUrlLinkCtrl::OnNcHitTest(CPoint point) 
{
	return HTCLIENT;
}

void CUrlLinkCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// Call ShellExecute to run the file. For a URL, this will run the browser.
	CString sURL = mpTemplate->GetStringProperty( Prop::URLAddress );
	if( sURL.IsEmpty() )
	{
		MessageBeep( MB_ICONERROR );
		return;
	}
	CString sPrefix;
	switch( mpTemplate->GetLongProperty( Prop::URLLinkType ) )
	{
	case 1:
		sPrefix = _T("mailto:");
		break;
	default:
		sPrefix = _T("http://");
		break;
	}
	if( !sPrefix.IsEmpty() && sURL.Left( sPrefix.GetLength() ) != sPrefix )
		sURL = sPrefix + sURL;
	HINSTANCE hShell = ShellExecute( 0, _T("open"), sURL, 0, 0, SW_SHOWNORMAL );
	if ((UINT_PTR)hShell > 32) 
	{ // success!
		mAcadColorService.SetForegroundColor( g_colorVisited );
		Invalidate();
	}
	else 
	{
		MessageBeep( MB_ICONERROR );
		TRACE( _T("*** WARNING: CUrlLinkCtrl: unable to navigate link %s\n"), (LPCTSTR)CString(sPrefix + sURL) );
	}	
}

void CUrlLinkCtrl::PostNcDestroy() 
{
	__super::PostNcDestroy();
	delete this;
}

void CUrlLinkCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	dc.SaveDC();
	CString sText;
	GetWindowText( sText );
	if( !sText.IsEmpty() )
	{
		CRect rcClient;
		GetClientRect( &rcClient );
		mAcadColorService.CtlColor( &dc, CTLCOLOR_STATIC );
		dc.SelectObject( &mFont );
		dc.ExtTextOut( 0, 0, ETO_CLIPPED, &rcClient, sText, NULL );
	}
	dc.RestoreDC( -1 );
}

BOOL CUrlLinkCtrl::OnEraseBkgnd(CDC* pDC)
{
	if( !mAcadColorService.IsBackgroundTransparent() )
	{
		CRect rcClient;
		GetClientRect( &rcClient );
		pDC->FillSolidRect( &rcClient, mAcadColorService.GetBackgroundColor() );
	}
	return TRUE;
}

LRESULT CUrlLinkCtrl::OnSetFont(WPARAM wParam, LPARAM lParam)
{
	LOGFONT lf = { NULL };
	CFont::FromHandle( (HFONT)wParam )->GetLogFont( &lf );
	mFont.DeleteObject();
	mFont.CreateFontIndirect(&lf);
	if( LOWORD(lParam) )
		RedrawWindow();
	return 0;
}
