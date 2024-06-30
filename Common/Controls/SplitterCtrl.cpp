// SplitterCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "SplitterCtrl.h"
#include "ControlPane.h"
#include "DialogObject.h"

#define Splitter_Raised 0
#define Splitter_DoubleRaised 1
#define Splitter_Etched 2
#define Splitter_Flat 3
#define Splitter_Sunken 4
#define Splitter_Bump 5


/////////////////////////////////////////////////////////////////////////////
// CSplitterCtrl

CSplitterCtrl::CSplitterCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CDialogControl( pTemplate, pPane, this )
, mbVertical( pTemplate->GetLongProperty( Prop::Width ) <= pTemplate->GetLongProperty( Prop::Height ) )
, mptDragStart( 0, 0 )
, mbIgnoreSizing( true )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CSplitterCtrl::~CSplitterCtrl()
{
}

bool CSplitterCtrl::Create( CWnd* pParentWnd, UINT nID ) 
{
	CRect rcWnd = GetWndRect();
	FromDIP( rcWnd );
	bool bSuccess = (__super::Create( NULL, GetWndStyle(), rcWnd, pParentWnd, nID ) != FALSE);
	if( bSuccess && mpControlPane->IsInvisibleControlAllowed( TDialogControlLockedPtr( this ) ) )
		bSuccess = (ModifyStyleEx( 0, WS_EX_TRANSPARENT ) != FALSE);
	mbIgnoreSizing = false;

	if( bSuccess && !ApplyPropertiesEnum() )
		bSuccess = false;

	return bSuccess;
}

CRect CSplitterCtrl::ValidatePosition( const CRect& rcProposed ) const
{
	CRect rcValid = __super::ValidatePosition( rcProposed );
	CRect rcParent = mpControlPane->GetControlArea();
	long nMin = mpTemplate->GetLongProperty( Prop::SplitterMin );
	long nMax = mpTemplate->GetLongProperty( Prop::SplitterMax );
	if( mbVertical )
	{
		if( rcProposed.left - rcParent.left < nMin )
			rcValid.MoveToX( nMin + rcParent.left );
		else if( rcParent.right - rcProposed.left < nMax )
		{
			long lLeft = rcParent.right - nMax;
			if( lLeft - rcParent.left >= nMin )
				rcValid.MoveToX( lLeft );
			else
				rcValid.MoveToX( nMin + rcParent.left );
		}
	}
	else
	{
		if( rcProposed.top - rcParent.top < nMin )
			rcValid.MoveToY( nMin + rcParent.top );
		else if( rcParent.bottom - rcProposed.top < nMax )
		{
			long lTop = rcParent.bottom - nMax;
			if( lTop - rcParent.top >= nMin )
				rcValid.MoveToY( lTop );
			else
				rcValid.MoveToY( nMin + rcParent.top );
		}
	}
	return rcValid;
}

bool CSplitterCtrl::ApplyProperty( TPropertyPtr pProp )
{
	if( !__super::ApplyProperty( pProp ) )
		return false;
	bool bFailed = false;
	switch( pProp->GetID() )
	{
	case Prop::SplitterStyle:
		OnNeedRepaint(true);
		break;
	}
	return !bFailed;
}

bool CSplitterCtrl::OnApplyLeft( TPropertyPtr pProp )
{
	if( mbVertical )
	{
		CRect rcNew = ValidatePosition( GetWndRect() );
		if( pProp->GetLongValue() != rcNew.left )
		{
			pProp->SetLongValue( rcNew.left );
			SyncPosLeft();
		}
	}
	return __super::OnApplyLeft( pProp );
}

bool CSplitterCtrl::OnApplyTop( TPropertyPtr pProp )
{
	if( !mbVertical )
	{
		CRect rcNew = ValidatePosition( GetWndRect() );
		if( pProp->GetLongValue() != rcNew.top )
		{
			pProp->SetLongValue( rcNew.top );
			SyncPosTop();
		}
	}
	return __super::OnApplyTop( pProp );
}

bool CSplitterCtrl::OnApplyWidth( TPropertyPtr pProp )
{
	bool bSuccess = __super::OnApplyWidth( pProp );
	mbVertical = (pProp->GetLongValue() <= mpTemplate->GetLongProperty( Prop::Height ));
	return bSuccess;
}

bool CSplitterCtrl::OnApplyHeight( TPropertyPtr pProp )
{
	bool bSuccess = __super::OnApplyHeight( pProp );
	mbVertical = (mpTemplate->GetLongProperty( Prop::Width ) <= pProp->GetLongValue());
	return bSuccess;
}


BEGIN_MESSAGE_MAP(CSplitterCtrl, CStatic)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_NCHITTEST()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_MOVE()
	ON_WM_NCLBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_ERASEBKGND()
	ON_MESSAGE(WM_DPICHANGED_AFTERPARENT, &CSplitterCtrl::OnDpiChanged)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSplitterCtrl message handlers

LRESULT CSplitterCtrl::OnDpiChanged(WPARAM wParam, LPARAM lParam)
{
	HandleDpiChanged();
	return 0;
}

BOOL CSplitterCtrl::PreTranslateMessage(MSG* pMsg) 
{
	GetToolTipCtrl().RelayEvent(pMsg);
	return __super::PreTranslateMessage(pMsg);
}

HBRUSH CSplitterCtrl::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	return HandleCtlColor( pDC, nCtlColor );
}

BOOL CSplitterCtrl::OnEraseBkgnd(CDC* pDC)
{
	if( HandleEraseBkgnd( pDC ) )
		return TRUE;
	return __super::OnEraseBkgnd(pDC);
}

void CSplitterCtrl::PostNcDestroy() 
{
	__super::PostNcDestroy();
	delete this;
}

__UINT_LRESULT CSplitterCtrl::OnNcHitTest(CPoint point)
{
	if ( !mpControlPane->IsInvisibleControlAllowed( TDialogControlLockedPtr( this ) ) )
		return HTCLIENT;
	return (mbVertical? HTLEFT : HTTOP);
}

void CSplitterCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect rcPaint;
	GetWindowRect( &rcPaint );
	ScreenToClient( &rcPaint );

	switch( mpTemplate->GetLongProperty( Prop::SplitterStyle ) )
	{
	case Splitter_DoubleRaised:
		dc.FillSolidRect( &rcPaint, GetSysColor( COLOR_BTNFACE ) );
		if( mbVertical )
		{
			CRect rc1 = rcPaint;
			rc1.right = rc1.left + 3;
			CRect rc2 = rc1;
			rc2.left = rc1.left + 3;
			rc2.right = rc2.left + 3;
			dc.DrawEdge( &rc1, BDR_RAISEDINNER, BF_RECT );
			dc.DrawEdge( &rc2, BDR_RAISEDINNER, BF_RECT );
		}
		else
		{
			CRect rc1 = rcPaint;
			rc1.bottom = rc1.top + 3;
			CRect rc2 = rc1;
			rc2.top = rc1.top + 3;
			rc2.bottom = rc2.top + 3;
			dc.DrawEdge( &rc1, BDR_RAISEDINNER, BF_RECT );
			dc.DrawEdge( &rc2, BDR_RAISEDINNER, BF_RECT );
		}
		break;
	case Splitter_Raised:
		dc.DrawEdge( &rcPaint, EDGE_RAISED, BF_RECT | BF_MIDDLE );
		break;
	case Splitter_Sunken:
		dc.DrawEdge( &rcPaint, EDGE_SUNKEN, BF_RECT | BF_MIDDLE );
		break;
	case Splitter_Etched:
		dc.DrawEdge( &rcPaint, EDGE_ETCHED, BF_RECT | BF_MIDDLE );
		break;
	case Splitter_Bump:
		dc.DrawEdge( &rcPaint, EDGE_BUMP, BF_RECT | BF_MIDDLE );
		break;
	default:
		dc.FillSolidRect( &rcPaint, GetSysColor( COLOR_BTNFACE ) );
		break;
	}
}

void CSplitterCtrl::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);
	if( !mbIgnoreSizing )
	{
		SetPosWidth( ToDIP( cx ) );
		SetPosHeight( ToDIP( cy ) );
	}
}

void CSplitterCtrl::OnMove(int x, int y)
{
	__super::OnMove(x, y);
	if( !mbIgnoreSizing )
	{
		CRect rcThis = GetEffectiveWindowRect();
		ToDIP( rcThis );
		SetPosLeft( rcThis.left );
		SetPosTop( rcThis.top );
		mpControlPane->RecalcLayout();
	}
}

void CSplitterCtrl::OnNcLButtonDown(UINT nHitTest, CPoint point)
{
	__super::OnNcLButtonDown(nHitTest, point);
	if( nHitTest >= HTSIZEFIRST && nHitTest <= HTSIZELAST )
	{
		mptDragStart = point;
		ScreenToClient( &mptDragStart );
		SetCapture();
	}
}

void CSplitterCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	__super::OnMouseMove(nFlags, point);
	CPoint ptNew = point - mptDragStart;
	ClientToScreen( &ptNew );
	mpControlPane->GetHostDialog()->ScreenToClient( &ptNew );
	ToDIP( ptNew );
	CRect rcNew = ValidatePosition( CRect( ptNew, CSize( 0, 0 ) ) );
	if( mbVertical )
	{
		if( rcNew.left == mpTemplate->GetLongProperty( Prop::Left ) )
			return;
		SetPosLeft( rcNew.left );
	}
	else
	{
		if( rcNew.top == mpTemplate->GetLongProperty( Prop::Top ) )
			return;
		SetPosTop( rcNew.top );
	}
	ApplyPosition();
}

void CSplitterCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();
	__super::OnLButtonUp(nFlags, point);
}
