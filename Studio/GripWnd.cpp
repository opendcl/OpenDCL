// GripWnd.cpp : implementation file
//

#include "StdAfx.h"
#include "GripWnd.h"

const UINT WM_GRIP_STARTDRAG = RegisterWindowMessage( _T("OpenDCL.Grip.StartDrag") );
const UINT WM_GRIP_DRAGGING = RegisterWindowMessage( _T("OpenDCL.Grip.Dragging") );


/////////////////////////////////////////////////////////////////////////////
// CGripWnd


CGripWnd::CGripWnd( CWnd* pParentWnd, int nQuadrant, LPCTSTR pszCursor, bool bHot /*= false*/ )
: mpParentWnd( pParentWnd )
, mbHot( bHot )
, mhCursor( AfxGetApp()->LoadStandardCursor( pszCursor ) )
, mbResizing( false )
, mnQuadrant( nQuadrant )
{
	CreateGripWindow();
}

CGripWnd::~CGripWnd()
{
	DestroyWindow();
	DeleteObject( mhCursor );
}

bool CGripWnd::CreateGripWindow()
{
	if( m_hWnd )
		return true;
	if( !mpParentWnd->m_hWnd )
		return false; //parent window hasn't been created yet!
	static CString sClassName = AfxRegisterWndClass( (CS_VREDRAW | CS_HREDRAW), NULL, NULL, NULL );
	if( !Create( sClassName, _T(""), WS_CHILD, CRect(), mpParentWnd, (UINT)mnQuadrant + 80, NULL ) )
		return false;
	return (m_hWnd != NULL);
}

bool CGripWnd::IsPointInside( const CPoint& ptTest /*window coordinates*/ )
{
	CreateGripWindow();
	CRect rcThis;
	GetWindowRect( &rcThis );
	return (rcThis.PtInRect( ptTest ) != FALSE);
}

BEGIN_MESSAGE_MAP(CGripWnd, CWnd)
	ON_WM_PAINT()	
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()	
	ON_WM_MOUSEMOVE()
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CGripWnd message handlers

void CGripWnd::OnPaint() 
{
	CPaintDC dcPaint(this);
	CRect rcThis;
	GetClientRect( &rcThis );
	if( mbHot )
		dcPaint.FillSolidRect( &rcThis, RGB(255,0,0) );
	else
		dcPaint.FillSolidRect( &rcThis, RGB(0,0,255) );
}

void CGripWnd::OnMouseMove(UINT nFlags, CPoint point) 
{
	if( mhCursor && IsWindowVisible() )
		::SetCursor( mhCursor );					

	if (mbResizing)
	{
		CWnd* pParent = GetParent();
		CRect rcThis;
		GetWindowRect( &rcThis );
		pParent->ScreenToClient( &rcThis );
		pParent->SendMessage( WM_GRIP_DRAGGING, (WPARAM)mnQuadrant, MAKELPARAM(rcThis.left + point.x, rcThis.top + point.y) );
	}
	
	__super::OnMouseMove(nFlags, point);
}

void CGripWnd::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CWnd* pParent = GetParent();
	CRect rcThis;
	GetWindowRect( &rcThis );
	pParent->ScreenToClient( &rcThis );
	pParent->SendMessage( WM_LBUTTONUP, (WPARAM)nFlags, MAKELPARAM(rcThis.left + point.x, rcThis.top + point.y) );
	mbResizing = false;
}

void CGripWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CWnd* pParent = GetParent();
	pParent->SendMessage( WM_GRIP_STARTDRAG, (WPARAM)mnQuadrant, (LPARAM)this );
	mbResizing = true;
}

BOOL CGripWnd::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if( mhCursor && IsWindowVisible() )
	{
		::SetCursor( mhCursor );					
		return FALSE;
	}
	return __super::OnSetCursor( pWnd, nHitTest, message );
}
