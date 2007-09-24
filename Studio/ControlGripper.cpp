// ControlGripper.cpp : implementation file
//

#include "StdAfx.h"
#include "ControlGripper.h"


CControlGripper::CControlGripper( CWnd* pParent, bool bSelected /*= false*/ )
: mpParentWnd( pParent )
, mGrip1( pParent, 1, IDC_SIZENWSE, bSelected )
, mGrip2( pParent, 2, IDC_SIZENS, bSelected )
, mGrip3( pParent, 3, IDC_SIZENESW, bSelected )
, mGrip4( pParent, 4, IDC_SIZEWE, bSelected )
, mGrip5( pParent, 5, IDC_SIZEWE, bSelected )
, mGrip6( pParent, 6, IDC_SIZENESW, bSelected )
, mGrip7( pParent, 7, IDC_SIZENS, bSelected )
, mGrip8( pParent, 8, IDC_SIZENWSE, bSelected )
{
}

CControlGripper::~CControlGripper(void)
{
}

bool CControlGripper::EnsureCreated()
{
	return (mGrip1.EnsureCreated() &&
					mGrip2.EnsureCreated() &&
					mGrip3.EnsureCreated() &&
					mGrip4.EnsureCreated() &&
					mGrip5.EnsureCreated() &&
					mGrip6.EnsureCreated() &&
					mGrip7.EnsureCreated() &&
					mGrip8.EnsureCreated());
}

bool CControlGripper::IsVisible() const
{
	return (mGrip1.m_hWnd && mGrip1.IsWindowVisible() != FALSE);
}

void CControlGripper::Hide()
{
	EnsureCreated();
	mGrip1.ShowWindow( SW_HIDE );
	mGrip2.ShowWindow( SW_HIDE );
	mGrip3.ShowWindow( SW_HIDE );
	mGrip4.ShowWindow( SW_HIDE );
	mGrip5.ShowWindow( SW_HIDE );
	mGrip6.ShowWindow( SW_HIDE );
	mGrip7.ShowWindow( SW_HIDE );
	mGrip8.ShowWindow( SW_HIDE );
}

void CControlGripper::MoveToTop()
{
	EnsureCreated();
	mGrip1.SetWindowPos( &CWnd::wndTop, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOCOPYBITS );
	mGrip2.SetWindowPos( &CWnd::wndTop, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOCOPYBITS );
	mGrip3.SetWindowPos( &CWnd::wndTop, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOCOPYBITS );
	mGrip4.SetWindowPos( &CWnd::wndTop, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOCOPYBITS );
	mGrip5.SetWindowPos( &CWnd::wndTop, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOCOPYBITS );
	mGrip6.SetWindowPos( &CWnd::wndTop, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOCOPYBITS );
	mGrip7.SetWindowPos( &CWnd::wndTop, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOCOPYBITS );
	mGrip8.SetWindowPos( &CWnd::wndTop, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOCOPYBITS );
	mGrip1.Invalidate();
	mGrip2.Invalidate();
	mGrip3.Invalidate();
	mGrip4.Invalidate();
	mGrip5.Invalidate();
	mGrip6.Invalidate();
	mGrip7.Invalidate();
	mGrip8.Invalidate();
}

void CControlGripper::MoveTo( const CRect& rcPos, bool bShow /*= true*/ )
{
	EnsureCreated();
	int nLeftX = rcPos.left;
	int nMidX = rcPos.left + (rcPos.Width() - CGripWnd::Size) / 2;
	int nRightX = rcPos.right - CGripWnd::Size;
	int nTopY = rcPos.top;
	int nMidY = rcPos.top + (rcPos.Height() - CGripWnd::Size) / 2;
	int nBottomY = rcPos.bottom - CGripWnd::Size;
	
	mGrip1.MoveWindow( nLeftX, nTopY, CGripWnd::Size, CGripWnd::Size, TRUE );
	mGrip2.MoveWindow( nMidX, nTopY, CGripWnd::Size, CGripWnd::Size, TRUE );
	mGrip3.MoveWindow( nRightX, nTopY, CGripWnd::Size, CGripWnd::Size, TRUE );
	mGrip4.MoveWindow( nLeftX, nMidY, CGripWnd::Size, CGripWnd::Size, TRUE );
	mGrip5.MoveWindow( nRightX, nMidY, CGripWnd::Size, CGripWnd::Size, TRUE );
	mGrip6.MoveWindow( nLeftX, nBottomY, CGripWnd::Size, CGripWnd::Size, TRUE );
	mGrip7.MoveWindow( nMidX, nBottomY, CGripWnd::Size, CGripWnd::Size, TRUE );
	mGrip8.MoveWindow( nRightX, nBottomY, CGripWnd::Size, CGripWnd::Size, TRUE );

	if( (mGrip1.IsWindowVisible() != FALSE) != bShow )
	{
		mGrip1.ShowWindow( bShow? SW_SHOW : SW_HIDE );
		mGrip2.ShowWindow( bShow? SW_SHOW : SW_HIDE );
		mGrip3.ShowWindow( bShow? SW_SHOW : SW_HIDE );
		mGrip4.ShowWindow( bShow? SW_SHOW : SW_HIDE );
		mGrip5.ShowWindow( bShow? SW_SHOW : SW_HIDE );
		mGrip6.ShowWindow( bShow? SW_SHOW : SW_HIDE );
		mGrip7.ShowWindow( bShow? SW_SHOW : SW_HIDE );
		mGrip8.ShowWindow( bShow? SW_SHOW : SW_HIDE );
	}
	if( bShow )
		MoveToTop();
}

int CControlGripper::HitTest( const CPoint& ptTest, HCURSOR& hCursor /*out*/ )
{
	EnsureCreated();
	CPoint ptWindow( ptTest );
	mGrip1.GetParent()->ClientToScreen( &ptWindow );
	if( mGrip1.IsPointInside( ptWindow ) )
	{
		hCursor = mGrip1.GetGripCursor();
		return 1;
	}
	if( mGrip2.IsPointInside( ptWindow ) )
	{
		hCursor = mGrip2.GetGripCursor();
		return 2;
	}
	if( mGrip3.IsPointInside( ptWindow ) )
	{
		hCursor = mGrip3.GetGripCursor();
		return 3;
	}
	if( mGrip4.IsPointInside( ptWindow ) )
	{
		hCursor = mGrip4.GetGripCursor();
		return 4;
	}
	if( mGrip5.IsPointInside( ptWindow ) )
	{
		hCursor = mGrip5.GetGripCursor();
		return 5;
	}
	if( mGrip6.IsPointInside( ptWindow ) )
	{
		hCursor = mGrip6.GetGripCursor();
		return 6;
	}
	if( mGrip7.IsPointInside( ptWindow ) )
	{
		hCursor = mGrip7.GetGripCursor();
		return 7;
	}
	if( mGrip8.IsPointInside( ptWindow ) )
	{
		hCursor = mGrip8.GetGripCursor();
		return 8;
	}
	return -1;
}

void CControlGripper::CancelSizing()
{
	EnsureCreated();
	mGrip1.CancelSizing();
	mGrip2.CancelSizing();
	mGrip3.CancelSizing();
	mGrip4.CancelSizing();
	mGrip5.CancelSizing();
	mGrip6.CancelSizing();
	mGrip7.CancelSizing();
	mGrip8.CancelSizing();
}

void CControlGripper::SetHot( bool bHot /*= true*/ )
{
	EnsureCreated();
	mGrip1.SetHot( bHot );
	mGrip2.SetHot( bHot );
	mGrip3.SetHot( bHot );
	mGrip4.SetHot( bHot );
	mGrip5.SetHot( bHot );
	mGrip6.SetHot( bHot );
	mGrip7.SetHot( bHot );
	mGrip8.SetHot( bHot );
	MoveToTop();
}
