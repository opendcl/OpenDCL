// HtmlCtrl.cpp : implementation file
//

#include "StdAfx.h"
#include "HtmlCtrl.h" 
#include "ControlPane.h"
#include "Workspace.h"
#include "SharedRes.Local.h"
#include "ColorService.h"
#include "HostThemeHelper.h"

#include <mshtml.h>


/////////////////////////////////////////////////////////////////////////////
// CHtmlCtrl

CHtmlCtrl::CHtmlCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CDialogControl( pTemplate, pPane, this )
, mbAuthorHtmlBg( false )
, mbAuthorHtmlText( false )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CHtmlCtrl::~CHtmlCtrl()
{
}

bool CHtmlCtrl::Create( CWnd* pParentWnd, UINT nID ) 
{
	bool bSuccess = (__super::Create( NULL, NULL, GetWndStyle(), GetWndRect(), pParentWnd, nID ) != FALSE);

	if( bSuccess )
		SetSilent( FALSE );

	if( bSuccess && !ApplyPropertiesEnum() )
		bSuccess = false;

	return bSuccess;
}

DWORD CHtmlCtrl::GetWndStyle() const
{
	DWORD dwStyle = __super::GetWndStyle();

	switch( mpTemplate->GetLongProperty( Prop::Justification ) )
	{
	case 0:
		dwStyle |= SS_LEFT;
		break;
	case 1:
		dwStyle |= SS_CENTER;
		break;
	case 2:
		dwStyle |= SS_RIGHT;
		break;
	}
	return dwStyle;
}

void CHtmlCtrl::HandleDpiChanged()
{
	__super::HandleDpiChanged();
	SetOpticalZoom( FromDIP( 100 ) );
}

bool CHtmlCtrl::ApplyProperty( TPropertyPtr pProp )
{
	if( !__super::ApplyProperty( pProp ) )
		return false;
	bool bFailed = false;
	switch( pProp->GetID() )
	{
	case Prop::Justification:
		{
			switch( pProp->GetLongValue() )
			{
			case 0:
				ModifyStyle( (SS_CENTER | SS_RIGHT), SS_LEFT, 0 );
				break;
			case 1:
				ModifyStyle( (SS_LEFT | SS_RIGHT), SS_CENTER, 0 );
				break;
			case 2:
				ModifyStyle( (SS_LEFT | SS_CENTER), SS_RIGHT, 0 );
				break;
			}
			OnNeedRepaint();
		}
		break;
	}
	return !bFailed;
}

static CString HtmlRgb( COLORREF cr )
{
	CString s;
	s.Format( _T("#%02X%02X%02X"), GetRValue( cr ), GetGValue( cr ), GetBValue( cr ) );
	return s;
}

static bool IsHtmlColorSpecified( const VARIANT& v )
{
	if( v.vt == VT_BSTR && v.bstrVal && *v.bstrVal )
	{
		CString s( v.bstrVal );
		s.Trim();
		if( s.IsEmpty() )
			return false;
		if( s.CompareNoCase( _T("transparent") ) == 0 )
			return false;
		if( s.CompareNoCase( _T("inherit") ) == 0 )
			return false;
		return true;
	}
	if( v.vt == VT_I4 || v.vt == VT_UI4 )
		return true;
	return false;
}

void CHtmlCtrl::HandleHostThemeChanged()
{
	ApplyHostDocumentColors( false );
	OnNeedRepaint( true );
}

void CHtmlCtrl::ApplyHostDocumentColors( bool bDetectAuthor )
{
	LPDISPATCH pDisp = GetHtmlDocument();
	if( !pDisp )
		return;
	CComQIPtr< IHTMLDocument2 > pDoc( pDisp );
	pDisp->Release();
	if( !pDoc )
		return;
	CComPtr< IHTMLElement > pBodyEl;
	if( FAILED( pDoc->get_body( &pBodyEl ) ) || !pBodyEl )
		return;
	CComQIPtr< IHTMLBodyElement > pBody( pBodyEl );
	CComPtr< IHTMLStyle > pStyle;
	pBodyEl->get_style( &pStyle );

	if( bDetectAuthor )
	{
		mbAuthorHtmlBg = false;
		mbAuthorHtmlText = false;
		if( pBody )
		{
			CComVariant vBg;
			if( SUCCEEDED( pBody->get_bgColor( &vBg ) ) && IsHtmlColorSpecified( vBg ) )
				mbAuthorHtmlBg = true;
			CComVariant vText;
			if( SUCCEEDED( pBody->get_text( &vText ) ) && IsHtmlColorSpecified( vText ) )
				mbAuthorHtmlText = true;
		}
		if( pStyle )
		{
			CComVariant vStyleBg;
			if( !mbAuthorHtmlBg && SUCCEEDED( pStyle->get_backgroundColor( &vStyleBg ) ) && IsHtmlColorSpecified( vStyleBg ) )
				mbAuthorHtmlBg = true;
			CComVariant vStyleFg;
			if( !mbAuthorHtmlText && SUCCEEDED( pStyle->get_color( &vStyleFg ) ) && IsHtmlColorSpecified( vStyleFg ) )
				mbAuthorHtmlText = true;
		}
	}

	const CComVariant vHostBg( HtmlRgb( OdclSysColor( COLOR_WINDOW ) ) );
	const CComVariant vHostFg( HtmlRgb( OdclSysColor( COLOR_WINDOWTEXT ) ) );
	if( !mbAuthorHtmlBg )
	{
		if( pBody )
			pBody->put_bgColor( vHostBg );
		if( pStyle )
			pStyle->put_backgroundColor( vHostBg );
	}
	if( !mbAuthorHtmlText )
	{
		if( pBody )
			pBody->put_text( vHostFg );
		if( pStyle )
			pStyle->put_color( vHostFg );
	}
}


BEGIN_MESSAGE_MAP(CHtmlCtrl, CHtmlBrowser)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_ERASEBKGND()
	ON_MESSAGE(WM_DPICHANGED_AFTERPARENT, &CHtmlCtrl::OnDpiChanged)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CHtmlCtrl message handlers

LRESULT CHtmlCtrl::OnDpiChanged(WPARAM wParam, LPARAM lParam)
{
	HandleDpiChanged();
	return 0;
}

HRESULT CHtmlCtrl::OnGetHostInfo(DOCHOSTUIINFO *pInfo)
{
	HRESULT hr = __super::OnGetHostInfo( pInfo );
	if( FAILED(hr) )
		return hr;
	if( pInfo )
		pInfo->dwFlags |= DOCHOSTUIFLAG_NO3DOUTERBORDER;
	return S_OK;
}
void CHtmlCtrl::OnDocumentComplete(LPCTSTR lpszURL)
{
	__super::OnDocumentComplete(lpszURL);
	SetOpticalZoom( FromDIP( 100 ) );
	ApplyHostDocumentColors( true );
}

BOOL CHtmlCtrl::PreTranslateMessage(MSG* pMsg) 
{
	GetToolTipCtrl().RelayEvent(pMsg);
	return __super::PreTranslateMessage(pMsg);
}

HBRUSH CHtmlCtrl::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	return HandleCtlColor( pDC, nCtlColor );
}

BOOL CHtmlCtrl::OnEraseBkgnd(CDC* pDC)
{
	if( pDC )
	{
		CRect rc;
		GetClientRect( &rc );
		pDC->FillSolidRect( &rc, OdclSysColor( COLOR_WINDOW ) );
		return TRUE;
	}
	if( HandleEraseBkgnd( pDC ) )
		return TRUE;
	return __super::OnEraseBkgnd(pDC);
}

void CHtmlCtrl::PostNcDestroy() 
{
	__super::PostNcDestroy();
	delete this;
}
