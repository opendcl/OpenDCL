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
	ON_WM_NCPAINT()
END_MESSAGE_MAP()



// CControlGripper message handlers


void CControlGripper::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CStatic::OnPaint() for painting messages
}

void CControlGripper::OnNcPaint()
{
	// TODO: Add your message handler code here
	// Do not call CStatic::OnNcPaint() for painting messages
	CDC* pDC = GetWindowDC();
	CRect rcManager;
	GetClientRect( &rcManager );
	static const COLORREF crFocus = RGB(20, 20, 220);
	pDC->Draw3dRect( &rcManager, crFocus, crFocus );
	rcManager.DeflateRect( 1, 1 );
	pDC->Draw3dRect( &rcManager, crFocus, crFocus );
	if( mbThickFrame )
	{
		rcManager.DeflateRect( 1, 1 );
		pDC->Draw3dRect( &rcManager, crFocus, crFocus );
		rcManager.DeflateRect( 1, 1 );
		pDC->Draw3dRect( &rcManager, crFocus, crFocus );
	}
	ReleaseDC( pDC );
}
