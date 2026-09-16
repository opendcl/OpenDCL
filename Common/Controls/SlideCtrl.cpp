// SlideCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "SlideCtrl.h"
#include "ControlPane.h"
#include "HostThemeHelper.h"
#include "ColorService.h"
#include "MemDC.h"


/////////////////////////////////////////////////////////////////////////////
// CSlideCtrl

CSlideCtrl::CSlideCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CDialogControl( pTemplate, pPane, this )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CSlideCtrl::~CSlideCtrl()
{
}

bool CSlideCtrl::Create( CWnd* pParentWnd, UINT nID ) 
{
	bool bSuccess = (__super::Create( GetWndStyle(), GetWndRect(), pParentWnd, nID ) != FALSE);
	if( bSuccess )
	{
	#ifdef _UNICODE
		BOOL bUnicode = TRUE;
	#else
		BOOL bUnicode = FALSE;
	#endif
		SendMessage( CCM_SETUNICODEFORMAT, (WPARAM)bUnicode, 0 );
	}

	if( bSuccess && !ApplyPropertiesEnum() )
		bSuccess = false;

	if( bSuccess )
		SyncHostSliderTheme();

	return bSuccess;
}

DWORD CSlideCtrl::GetWndStyle() const
{
	DWORD dwStyle = CDialogControl::GetWndStyle();

	dwStyle |= (TBS_AUTOTICKS);

	if( mpTemplate->GetLongProperty( Prop::Orientation ) == 1 )
		dwStyle |= TBS_VERT;
	else
		dwStyle |= TBS_HORZ;
	
	if( !mpTemplate->GetBooleanProperty( Prop::ShowTicks ) )
		dwStyle |= TBS_NOTICKS;

	return dwStyle;
}

bool CSlideCtrl::ApplyProperty( TPropertyPtr pProp )
{
	if( !__super::ApplyProperty( pProp ) )
		return false;
	bool bFailed = false;
	switch( pProp->GetID() )
	{
	case Prop::Value:
		{
			long lPos = pProp->GetLongValue();
			SetPos( lPos );
			OnPositionChanged( lPos, false );
		}
		break;
	case Prop::MinValue:
		SetRangeMin( pProp->GetLongValue() );
		break;
	case Prop::MaxValue:
		SetRangeMax( pProp->GetLongValue() );
		break;
	case Prop::SmallChange:
		SetLineSize( pProp->GetLongValue() );
		break;
	case Prop::LargeChange:
		SetPageSize( pProp->GetLongValue() );
		break;
	case Prop::ShowTicks:
		if( pProp->GetBooleanValue() )
			ModifyStyle( TBS_NOTICKS, 0 );
		else
			ModifyStyle( 0, TBS_NOTICKS );
		OnNeedRepaint();
		break;
	case Prop::TickFrequency:
		SetTicFreq( pProp->GetLongValue() );
		OnNeedRepaint();
		break;
	case Prop::Orientation:
		if( pProp->GetLongValue() == 0 )
			ModifyStyle( TBS_VERT, TBS_HORZ, SWP_FRAMECHANGED );
		else
			ModifyStyle( TBS_HORZ, TBS_VERT, SWP_FRAMECHANGED );
		break;
	}
	return !bFailed;
}

bool CSlideCtrl::OnApplyBackgroundColor( TPropertyPtr pProp )
{
	bool bSuccess = __super::OnApplyBackgroundColor( pProp );
	ClearSel( TRUE ); //force it to repaint its background
	return bSuccess;
}

bool CSlideCtrl::UseHostOwnerDraw() const
{
	return CHostThemeHelper::HostMaps() || !mpTemplate->GetBooleanProperty( Prop::UseVisualStyle );
}

void CSlideCtrl::HandleHostThemeChanged()
{
	SyncHostSliderTheme();
}

bool CSlideCtrl::OnApplyUseVisualStyle( TPropertyPtr pProp )
{
	if( !__super::OnApplyUseVisualStyle( pProp ) )
		return false;
	SyncHostSliderTheme();
	return true;
}

void CSlideCtrl::SyncHostSliderTheme()
{
	if( !m_hWnd )
		return;
	LPCWSTR pszTheme = CHostThemeHelper::ThemeClass( mpTemplate->GetBooleanProperty( Prop::UseVisualStyle ) );
	GetTheme().SetWindowTheme( pszTheme, pszTheme );
	CHostThemeHelper::Apply( m_hWnd, pszTheme );
	OnNeedRepaint( true );
}

static void NormalizeRect( CRect& rc )
{
	if( rc.left > rc.right )
	{
		const LONG nTmp = rc.left;
		rc.left = rc.right;
		rc.right = nTmp;
	}
	if( rc.top > rc.bottom )
	{
		const LONG nTmp = rc.top;
		rc.top = rc.bottom;
		rc.bottom = nTmp;
	}
}

static void DrawSliderTick( CDC* pDC, const CRect& rcChannel, bool bVert, bool bTicksAfter, int nPos, int nLen, int nGap )
{
	if( !pDC || nPos < 0 || nLen <= 0 )
		return;
	if( bVert )
	{
		const int x0 = bTicksAfter ? (rcChannel.right + nGap) : (rcChannel.left - nGap - nLen);
		pDC->MoveTo( x0, nPos );
		pDC->LineTo( x0 + nLen, nPos );
	}
	else
	{
		const int y0 = bTicksAfter ? (rcChannel.bottom + nGap) : (rcChannel.top - nGap - nLen);
		pDC->MoveTo( nPos, y0 );
		pDC->LineTo( nPos, y0 + nLen );
	}
}

void CSlideCtrl::PaintHostSlider( CDC* pDC )
{
	if( !pDC )
		return;

	CRect rcClient;
	GetClientRect( &rcClient );
	pDC->FillSolidRect( &rcClient, OdclSysColor( COLOR_BTNFACE ) );

	const bool bVert = (GetStyle() & TBS_VERT) != 0;
	CRect rcChannel;
	GetChannelRect( &rcChannel );
	// TBM_GETCHANNELRECT stores a vertical channel as a transposed RECT.
	if( bVert )
		rcChannel = CRect( rcChannel.top, rcChannel.left, rcChannel.bottom, rcChannel.right );
	NormalizeRect( rcChannel );

	const bool bEnabled = (IsWindowEnabled() != FALSE);
	const COLORREF crTrough = OdclSysColor( bEnabled ? COLOR_3DSHADOW : COLOR_INACTIVEBORDER );
	pDC->FillSolidRect( &rcChannel, crTrough );

	const bool bNoTicks = (GetStyle() & TBS_NOTICKS) != 0;
	if( !bNoTicks )
	{
		const int nTick = (int)FromDIP( 4 );
		const int nEndTick = (int)FromDIP( 6 );
		const int nGap = (int)FromDIP( 2 );
		CPen pen( PS_SOLID, 1, OdclSysColor( bEnabled ? COLOR_BTNTEXT : COLOR_GRAYTEXT ) );
		CPen* pOldPen = pDC->SelectObject( &pen );
		const bool bTicksAfter = bVert
			? (rcChannel.right + nGap + nEndTick <= rcClient.right)
			: (rcChannel.bottom + nGap + nEndTick <= rcClient.bottom);

		CRect rcThumbTic;
		GetThumbRect( &rcThumbTic );
		NormalizeRect( rcThumbTic );
		const int nThumbHalf = bVert ? (rcThumbTic.Height() / 2) : (rcThumbTic.Width() / 2 );
		const int nTravel0 = (bVert ? rcChannel.top : rcChannel.left) + nThumbHalf;
		const int nTravel1 = (bVert ? rcChannel.bottom : rcChannel.right) - nThumbHalf;
		const int nMin = GetRangeMin();
		const int nMax = GetRangeMax();
		int nFreq = mpTemplate ? (int)mpTemplate->GetLongProperty( Prop::TickFrequency ) : 1;
		if( nFreq <= 0 )
			nFreq = 1;
		const int nSpan = nMax - nMin;
		if( nSpan > 0 && nTravel1 >= nTravel0 )
		{
			int nValue = nMin;
			while( nValue <= nMax )
			{
				const int nPos = nTravel0 + MulDiv( nValue - nMin, nTravel1 - nTravel0, nSpan );
				const int nLen = (nValue == nMin || nValue == nMax) ? nEndTick : nTick;
				DrawSliderTick( pDC, rcChannel, bVert, bTicksAfter, nPos, nLen, nGap );
				if( nValue >= nMax )
					break;
				nValue += nFreq;
				if( nValue > nMax )
					nValue = nMax;
			}
		}
		pDC->SelectObject( pOldPen );
	}

	CRect rcThumb;
	GetThumbRect( &rcThumb );
	NormalizeRect( rcThumb );
	pDC->FillSolidRect( &rcThumb, OdclSysColor( COLOR_BTNFACE ) );
	CPen penThumb( PS_SOLID, 1, OdclSysColor( COLOR_3DLIGHT ) );
	CPen* pOldPen = pDC->SelectObject( &penThumb );
	CBrush* pOldBrush = (CBrush*)pDC->SelectStockObject( NULL_BRUSH );
	pDC->Rectangle( rcThumb.left, rcThumb.top, rcThumb.right, rcThumb.bottom );
	pDC->SelectObject( pOldBrush );
	pDC->SelectObject( pOldPen );

	if( GetFocus() == this )
	{
		CRect rcFocus = rcThumb;
		rcFocus.DeflateRect( 2, 2 );
		pDC->DrawFocusRect( &rcFocus );
	}
}


BEGIN_MESSAGE_MAP(CSlideCtrl, CSliderCtrl)
	ON_WM_HSCROLL_REFLECT()
	ON_WM_VSCROLL_REFLECT()
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_MESSAGE(WM_DPICHANGED_AFTERPARENT, &CSlideCtrl::OnDpiChanged)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSlideCtrl message handlers

LRESULT CSlideCtrl::OnDpiChanged(WPARAM wParam, LPARAM lParam)
{
	HandleDpiChanged();
	return 0;
}

void CSlideCtrl::HScroll(UINT nSBCode, UINT nPos)
{
	int nNewPos = GetPos();
	mpTemplate->SetLongProperty( Prop::Value, nNewPos );
	OnPositionChanged( nNewPos );
}

void CSlideCtrl::VScroll(UINT nSBCode, UINT nPos)
{
	int nNewPos = GetPos();
	mpTemplate->SetLongProperty( Prop::Value, nNewPos );
	OnPositionChanged( nNewPos );
}

void CSlideCtrl::PostNcDestroy() 
{
	__super::PostNcDestroy();
	delete this;
}

BOOL CSlideCtrl::PreTranslateMessage(MSG* pMsg) 
{
	GetToolTipCtrl().RelayEvent(pMsg);
	return __super::PreTranslateMessage(pMsg);
}

HBRUSH CSlideCtrl::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	return HandleCtlColor( pDC, nCtlColor );
}

BOOL CSlideCtrl::OnEraseBkgnd(CDC* pDC)
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

void CSlideCtrl::OnPaint()
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
	PaintHostSlider( &dc );
}
