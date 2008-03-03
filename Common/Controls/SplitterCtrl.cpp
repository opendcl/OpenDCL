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
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CSplitterCtrl::~CSplitterCtrl()
{
}

bool CSplitterCtrl::Create( CWnd* pParentWnd, UINT nID ) 
{
	bool bSuccess = (__super::Create( NULL, GetWndStyle(), GetWndRect(), pParentWnd, nID ) != FALSE);
	if( bSuccess )
		bSuccess = (ModifyStyleEx( 0, WS_EX_TRANSPARENT ) != FALSE);

	if( bSuccess && !ApplyPropertiesEnum() )
		bSuccess = false;

	return bSuccess;
}

bool CSplitterCtrl::OnApplyProperty( TPropertyPtr pProp )
{
	if( !__super::OnApplyProperty( pProp ) )
		return false;
	bool bFailed = false;
	switch( pProp->GetID() )
	{
	case Prop::SplitterStyle:
		OnNeedRepaint();
		break;
	}
	return !bFailed;
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
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSplitterCtrl message handlers

BOOL CSplitterCtrl::PreTranslateMessage(MSG* pMsg) 
{
	GetToolTipCtrl().RelayEvent(pMsg);
	return __super::PreTranslateMessage(pMsg);
}

HBRUSH CSplitterCtrl::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	if( !IsWindowEnabled() )
		return NULL;
	return mColorService.CtlColor( pDC, nCtlColor );
}

void CSplitterCtrl::PostNcDestroy() 
{
	__super::PostNcDestroy();
	delete this;
}

__UINT_LRESULT CSplitterCtrl::OnNcHitTest(CPoint point)
{
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
	mpTemplate->SetLongProperty( Prop::Width, cx );
	mpTemplate->SetLongProperty( Prop::Height, cy );
}

void CSplitterCtrl::OnMove(int x, int y)
{
	__super::OnMove(x, y);
	CRect rcThis = GetEffectiveWindowRect();
	mpTemplate->SetLongProperty( Prop::Left, rcThis.left );
	mpTemplate->SetLongProperty( Prop::Top, rcThis.top );
	mpControlPane->RecalcLayout();
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
	GetParent()->ScreenToClient( &ptNew );
	CRect rcParent;
	GetParent()->GetClientRect( &rcParent );
	long nMin = mpTemplate->GetLongProperty( Prop::SplitterMin );
	long nMax = mpTemplate->GetLongProperty( Prop::SplitterMax );
	if( mbVertical )
	{
		if( ptNew.x - rcParent.left < nMin )
			ptNew.x = nMin;
		else if( rcParent.right - ptNew.x < nMax )
			ptNew.x = rcParent.right - nMax;
		mpTemplate->SetLongProperty( Prop::Left, ptNew.x );
	}
	else
	{
		if( ptNew.y - rcParent.top < nMin )
			ptNew.y = nMin;
		else if( rcParent.bottom - ptNew.y < nMax )
			ptNew.y = rcParent.bottom - nMax;
		mpTemplate->SetLongProperty( Prop::Top, ptNew.y );
	}
	ApplyPosition();
}

void CSplitterCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();
	__super::OnLButtonUp(nFlags, point);
}
