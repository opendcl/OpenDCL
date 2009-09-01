// ArxHtmlCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ArxHtmlCtrl.h"
#include "DclControlObject.h"
#include "InvokeMethod.h"
#include "ControlPane.h"


/////////////////////////////////////////////////////////////////////////////
// CArxHtmlCtrl

CArxHtmlCtrl::CArxHtmlCtrl( TDclControlPtr pTemplate,
														CControlPane* pPane,
														UINT nID,
														bool bCreate /*= true*/ )
: CHtmlCtrl( pTemplate, pPane, nID, false )
, mArxServices( this )
, mDragDropService( this )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CArxHtmlCtrl::~CArxHtmlCtrl()
{
}

void CArxHtmlCtrl::OnNavigateComplete2( LPCTSTR strURL ) 
{
	GetArxServices()->HandleEvent( Prop::EventNavigateComplete, args_S( strURL ) );
	__super::OnNavigateComplete2( strURL );
}
 
void CArxHtmlCtrl::OnAppCmd( LPCTSTR lpszWhere )
{
	// default: do nothing
}

BEGIN_MESSAGE_MAP(CArxHtmlCtrl, CHtmlCtrl)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CArxHtmlCtrl message handlers

void CArxHtmlCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	GetArxServices()->HandleEvent( Prop::EventMouseMove, args_NNN( nFlags, point.x, point.y ) );
	__super::OnMouseMove( nFlags, point );
}

void CArxHtmlCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	__super::OnLButtonDown( nFlags, point );

	if( (nFlags & MK_LBUTTON) != 0 && mpTemplate->GetBooleanProperty( Prop::DragnDropAllowBegin ) )
		BeginDragDrop( point );
}
