// LabelCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "LabelCtrl.h"
#include "ControlPane.h"
#include "HostThemeHelper.h"
#include "ColorService.h"


/////////////////////////////////////////////////////////////////////////////
// CLabelCtrl

CLabelCtrl::CLabelCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CDialogControl( pTemplate, pPane, this )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CLabelCtrl::~CLabelCtrl()
{
}

bool CLabelCtrl::Create( CWnd* pParentWnd, UINT nID ) 
{
	bool bSuccess = (__super::Create( NULL, GetWndStyle(), GetWndRect(), pParentWnd, nID ) != FALSE);

	if( bSuccess && !ApplyPropertiesEnum() )
		bSuccess = false;

	if( bSuccess && ( !mpTemplate || !mpTemplate->GetPropertyObject( Prop::UseVisualStyle ) ) )
		GetTheme().SetWindowTheme( L"", L"" );

	return bSuccess;
}

DWORD CLabelCtrl::GetWndStyle() const
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

bool CLabelCtrl::ApplyProperty( TPropertyPtr pProp )
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
			OnNeedRepaint( true );
		}
		break;
	}
	return !bFailed;
}

bool CLabelCtrl::OnApplyBackgroundColor( TPropertyPtr pProp )
{
	CAcadColorService* pColorService = GetColorService();
	if( pColorService )
		pColorService->SetBackgroundColor( pProp->GetLongValue() );
	OnNeedRepaint( true );
	return true;
}


BEGIN_MESSAGE_MAP(CLabelCtrl, CStatic)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_MESSAGE(WM_DPICHANGED_AFTERPARENT, &CLabelCtrl::OnDpiChanged)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CLabelCtrl message handlers

LRESULT CLabelCtrl::OnDpiChanged(WPARAM wParam, LPARAM lParam)
{
	HandleDpiChanged();
	return 0;
}

BOOL CLabelCtrl::PreTranslateMessage(MSG* pMsg) 
{
	GetToolTipCtrl().RelayEvent(pMsg);
	return __super::PreTranslateMessage(pMsg);
}

void CLabelCtrl::HandleHostThemeChanged()
{
	if( !m_hWnd )
		return;
	const bool bVisual = (mpTemplate && mpTemplate->GetPropertyObject( Prop::UseVisualStyle )
		&& mpTemplate->GetBooleanProperty( Prop::UseVisualStyle ));
	LPCWSTR pszTheme = CHostThemeHelper::ThemeClass( bVisual );
	GetTheme().SetWindowTheme( pszTheme, pszTheme );
	CHostThemeHelper::Apply( m_hWnd, pszTheme );
	Invalidate();
}

HBRUSH CLabelCtrl::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	HBRUSH hbrBackground = HandleCtlColor( pDC, nCtlColor );
	if( hbrBackground )
		return hbrBackground;
	return CAcadColorService::GetTransparentBrush();
}

BOOL CLabelCtrl::OnEraseBkgnd(CDC* pDC)
{
	if( HandleEraseBkgnd( pDC ) )
		return TRUE;
#if defined(ODCL_HOST_COLORTHEME)
	if( CHostThemeHelper::HostMaps() && pDC )
	{
		CRect rc;
		GetClientRect( &rc );
		pDC->FillSolidRect( &rc, GetPaneFaceColor() );
		return TRUE;
	}
#endif
	return __super::OnEraseBkgnd(pDC);
}

void CLabelCtrl::OnPaint()
{
	if( !CHostThemeHelper::HostMaps() )
	{
		Default();
		return;
	}
	CPaintDC dc( this );
	CRect rc;
	GetClientRect( &rc );
	CAcadColorService* pColorService = GetColorService();
	const COLORREF crBk = GetPaneFaceColor();
	dc.FillSolidRect( &rc, crBk );
	CString sCaption;
	GetWindowText( sCaption );
	COLORREF crText = CHostThemeHelper::DisabledTextColor();
	if( IsWindowEnabled() && pColorService )
		crText = pColorService->GetForegroundColor();
	else if( IsWindowEnabled() )
		crText = OdclSysColor( COLOR_BTNTEXT );
	dc.SetTextColor( crText );
	dc.SetBkColor( crBk );
	dc.SetBkMode( OPAQUE );
	CFont* pFont = GetFont();
	CFont* pOldFont = pFont ? dc.SelectObject( pFont ) : NULL;
	UINT dt = DT_EXPANDTABS | DT_NOPREFIX;
	const DWORD dwStyle = GetStyle();
	if( dwStyle & SS_CENTER )
		dt |= DT_CENTER;
	else if( dwStyle & SS_RIGHT )
		dt |= DT_RIGHT;
	else
		dt |= DT_LEFT;
	if( dwStyle & SS_CENTERIMAGE )
		dt |= DT_SINGLELINE | DT_VCENTER;
	else
		dt |= DT_WORDBREAK;
	if( GetExStyle() & WS_EX_RTLREADING )
		dt |= DT_RTLREADING;
	dc.DrawText( sCaption, &rc, dt );
	if( pOldFont )
		dc.SelectObject( pOldFont );
}

void CLabelCtrl::PostNcDestroy() 
{
	__super::PostNcDestroy();
	delete this;
}
