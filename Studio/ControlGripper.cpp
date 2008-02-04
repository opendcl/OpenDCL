// ControlGripper.cpp : implementation file
//

#include "stdafx.h"
#include "ControlGripper.h"
#include "ControlManager.h"
#include "StudioWorkspace.h"


// CControlGripper

CControlGripper::CControlGripper( CControlManager* pManager )
: mpManager( pManager )
, mbThickFrame( false )
{
}

CControlGripper::~CControlGripper()
{
}


BEGIN_MESSAGE_MAP(CControlGripper, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CControlGripper message handlers


void CControlGripper::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CStatic::OnPaint() for painting messages
	CDC* pDC = &dc;
	CRect rcManager;
	GetClientRect( &rcManager );
	static const COLORREF crBG = RGB(220, 220, 220);
	//pDC->FillSolidRect( 0, 0, 4, rcManager.bottom, crBG );
	//pDC->FillSolidRect( rcManager.right - 4, 0, 4, rcManager.bottom, crBG );
	//pDC->FillSolidRect( 4, 0, rcManager.right - 8, 4, crBG );
	//pDC->FillSolidRect( 4, rcManager.bottom - 4, rcManager.right - 8, 4, crBG );
	pDC->DrawFocusRect( &rcManager );
	rcManager.DeflateRect( 1, 1 );
	pDC->DrawFocusRect( &rcManager );
	if( mbThickFrame )
	{
		rcManager.DeflateRect( 1, 1 );
		pDC->DrawFocusRect( &rcManager );
		rcManager.DeflateRect( 1, 1 );
		pDC->DrawFocusRect( &rcManager );
	}
}
