// CheckBoxCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "CheckBoxCtrl.h"
#include "DclControlTemplate.h"
#include "PropertyObject.h"
#include "ControlPane.h"
#include "PropertyIds.h"
#include "HostThemeHelper.h"
#include "ColorService.h"
#include "MemDC.h"


//from CommCtrl.h
#define BCN_FIRST               (0U-1250U)
#define BCN_HOTITEMCHANGE       (BCN_FIRST + 0x0001)


/////////////////////////////////////////////////////////////////////////////
// CCheckBoxCtrl

CCheckBoxCtrl::CCheckBoxCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CDialogControl( pTemplate, pPane, this )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CCheckBoxCtrl::~CCheckBoxCtrl()
{
}

bool CCheckBoxCtrl::Create( CWnd* pParentWnd, UINT nID ) 
{
	bool bSuccess = (__super::Create( NULL, GetWndStyle(), GetWndRect(), pParentWnd, nID ) != FALSE);

	if( bSuccess && !ApplyPropertiesEnum() )
		bSuccess = false;

	if( bSuccess )
		SyncHostCheckTheme();

	return bSuccess;
}

DWORD CCheckBoxCtrl::GetWndStyle() const
{
	DWORD dwStyle = CDialogControl::GetWndStyle();

	dwStyle |= (/*WS_CLIPSIBLINGS | */BS_3STATE | BS_NOTIFY);
	return dwStyle;
}

bool CCheckBoxCtrl::ApplyProperty( TPropertyPtr pProp )
{
	if( !__super::ApplyProperty( pProp ) )
		return false;
	bool bFailed = false;
	switch( pProp->GetID() )
	{
	case Prop::Caption:
		SetWindowText( pProp->GetStringValue() );
		break;
	case Prop::Value:
		SetCheck( pProp->GetLongValue() );
		break;
	}
	return !bFailed;
}

bool CCheckBoxCtrl::UseHostOwnerDraw() const
{
	return CHostThemeHelper::HostMaps() || !mpTemplate->GetBooleanProperty( Prop::UseVisualStyle );
}

void CCheckBoxCtrl::HandleHostThemeChanged()
{
	SyncHostCheckTheme();
}

bool CCheckBoxCtrl::OnApplyUseVisualStyle( TPropertyPtr pProp )
{
	if( !__super::OnApplyUseVisualStyle( pProp ) )
		return false;
	SyncHostCheckTheme();
	return true;
}

void CCheckBoxCtrl::SyncHostCheckTheme()
{
	if( !m_hWnd )
		return;
	LPCWSTR pszTheme = CHostThemeHelper::ThemeClass( mpTemplate->GetBooleanProperty( Prop::UseVisualStyle ) );
	GetTheme().SetWindowTheme( pszTheme, pszTheme );
	CHostThemeHelper::Apply( m_hWnd, pszTheme );
	OnNeedRepaint( true );
}

void CCheckBoxCtrl::PaintHostCheck( CDC* pDC )
{
	if( !pDC )
		return;

	CRect rcClient;
	GetClientRect( &rcClient );

	COLORREF crBk = OdclSysColor( COLOR_BTNFACE );
	CAcadColorService* pColorService = GetColorService();
	if( pColorService && !pColorService->IsBackgroundNotSet() && !pColorService->IsBackgroundTransparent() )
		crBk = pColorService->GetBackgroundColor();
	pDC->FillSolidRect( &rcClient, crBk );

	const bool bEnabled = (IsWindowEnabled() != FALSE);
	const int nCheck = GetCheck();
	const int nBox = (int)FromDIP( 13 );
	const int nGap = (int)FromDIP( 4 );
	const int nPad = (int)FromDIP( 1 );
	int yBox = rcClient.top + (rcClient.Height() - nBox) / 2;
	if( yBox < rcClient.top )
		yBox = rcClient.top;
	CRect rcBox( rcClient.left + nPad, yBox, rcClient.left + nPad + nBox, yBox + nBox );

	pDC->FillSolidRect( &rcBox, OdclSysColor( COLOR_WINDOW ) );
	CPen penBox( PS_SOLID, 1, OdclSysColor( bEnabled ? COLOR_BTNTEXT : COLOR_GRAYTEXT ) );
	CPen* pOldPen = pDC->SelectObject( &penBox );
	CBrush* pOldBrush = (CBrush*)pDC->SelectStockObject( NULL_BRUSH );
	pDC->Rectangle( rcBox.left, rcBox.top, rcBox.right, rcBox.bottom );
	pDC->SelectObject( pOldBrush );
	pDC->SelectObject( pOldPen );

	const COLORREF crMark = OdclSysColor( bEnabled ? COLOR_BTNTEXT : COLOR_GRAYTEXT );
	if( nCheck == BST_INDETERMINATE )
	{
		CRect rcMix = rcBox;
		const int nBar = max( 2, nBox / 5 );
		rcMix.DeflateRect( nBox / 5, 0 );
		rcMix.top = rcBox.top + (rcBox.Height() - nBar) / 2;
		rcMix.bottom = rcMix.top + nBar;
		pDC->FillSolidRect( &rcMix, crMark );
	}
	else if( nCheck == BST_CHECKED )
	{
		const int nThick = max( 1, (int)FromDIP( 2 ) );
		CPen penMark( PS_SOLID, nThick, crMark );
		pOldPen = pDC->SelectObject( &penMark );
		const int x0 = rcBox.left + nBox * 3 / 13;
		const int y0 = rcBox.top + nBox * 7 / 13;
		const int x1 = rcBox.left + nBox * 5 / 13;
		const int y1 = rcBox.top + nBox * 10 / 13;
		const int x2 = rcBox.left + nBox * 10 / 13;
		const int y2 = rcBox.top + nBox * 4 / 13;
		pDC->MoveTo( x0, y0 );
		pDC->LineTo( x1, y1 );
		pDC->LineTo( x2, y2 );
		pDC->SelectObject( pOldPen );
	}

	CString sCaption;
	GetWindowText( sCaption );
	CRect rcText = rcClient;
	rcText.left = rcBox.right + nGap;
	COLORREF crText = crMark;
	if( bEnabled && pColorService )
		crText = pColorService->GetForegroundColor();
	const COLORREF crOld = pDC->SetTextColor( crText );
	const int nOldBk = pDC->SetBkMode( TRANSPARENT );
	CFont* pFont = GetFont();
	CFont* pOldFont = pFont ? pDC->SelectObject( pFont ) : NULL;
	pDC->DrawText( sCaption, &rcText, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS );
	if( GetFocus() == this )
	{
		CRect rcFocus = rcText;
		pDC->DrawText( sCaption, &rcFocus, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_CALCRECT );
		rcFocus.InflateRect( (int)FromDIP( 1 ), (int)FromDIP( 1 ) );
		pDC->DrawFocusRect( &rcFocus );
	}
	if( pOldFont )
		pDC->SelectObject( pOldFont );
	pDC->SetBkMode( nOldBk );
	pDC->SetTextColor( crOld );
}

BEGIN_MESSAGE_MAP(CCheckBoxCtrl, CButton)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_CONTROL_REFLECT(BN_CLICKED, &CCheckBoxCtrl::OnClicked)
	ON_CONTROL_REFLECT(BN_DOUBLECLICKED, &CCheckBoxCtrl::OnDoubleclicked)
	ON_MESSAGE(WM_DPICHANGED_AFTERPARENT, &CCheckBoxCtrl::OnDpiChanged)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CCheckBoxCtrl message handlers

LRESULT CCheckBoxCtrl::OnDpiChanged(WPARAM wParam, LPARAM lParam)
{
	HandleDpiChanged();
	return 0;
}

BOOL CCheckBoxCtrl::PreTranslateMessage(MSG* pMsg) 
{
	GetToolTipCtrl().RelayEvent(pMsg);
	if (pMsg->message == WM_KEYDOWN )
	{
		if( pMsg->wParam == VK_RETURN )
			pMsg->wParam = VK_TAB;
	}
	return __super::PreTranslateMessage(pMsg);
}

HBRUSH CCheckBoxCtrl::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	HBRUSH hbrBackground = HandleCtlColor( pDC, nCtlColor );
	if( hbrBackground )
		return hbrBackground;
	if( GetTheme().IsThemeActive() )
		return NULL; //when using visual style, transparent brush causes class background to be used
	return CAcadColorService::GetTransparentBrush();
}

BOOL CCheckBoxCtrl::OnEraseBkgnd(CDC* pDC)
{
	if( UseHostOwnerDraw() && pDC )
	{
		CRect rc;
		GetClientRect( &rc );
		pDC->FillSolidRect( &rc, OdclSysColor( COLOR_BTNFACE ) );
		return TRUE;
	}
	if( HandleEraseBkgnd( pDC ) )
		return TRUE;
	return __super::OnEraseBkgnd(pDC);
}

void CCheckBoxCtrl::OnPaint()
{
	if( !UseHostOwnerDraw() )
	{
		Default();
		return;
	}
	CPaintDC dcPaint( this );
	CRect rcClient;
	GetClientRect( &rcClient );
	CMemDCx dc( &dcPaint, rcClient );
	PaintHostCheck( &dc );
}

void CCheckBoxCtrl::PostNcDestroy() 
{
	__super::PostNcDestroy();
	delete this;
}

void CCheckBoxCtrl::OnClicked() 
{
	int nState = (mpTemplate->GetLongProperty( Prop::Value ) != BST_CHECKED? BST_CHECKED : BST_UNCHECKED);
	mpTemplate->SetLongProperty( Prop::Value, nState );
	SetCheck( nState );
}

void CCheckBoxCtrl::OnDoubleclicked() 
{
	int nState = (mpTemplate->GetLongProperty( Prop::Value ) != BST_CHECKED? BST_CHECKED : BST_UNCHECKED);
	mpTemplate->SetLongProperty( Prop::Value, nState );
	SetCheck( nState );
}

void CCheckBoxCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	OnNeedRepaint(); //must erase background here or focus rectangle gets erased afterward
	__super::OnLButtonDown(nFlags, point);
}
