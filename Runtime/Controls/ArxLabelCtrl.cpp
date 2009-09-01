// ArxLabelCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ArxLabelCtrl.h"
#include "DclControlObject.h"
#include "InvokeMethod.h"
#include "ControlPane.h"


/////////////////////////////////////////////////////////////////////////////
// CArxLabelCtrl

CArxLabelCtrl::CArxLabelCtrl( TDclControlPtr pTemplate,
															CControlPane* pPane,
															UINT nID,
															bool bCreate /*= true*/ )
: CLabelCtrl( pTemplate, pPane, nID, false )
, mArxServices( this )
, mDragDropService( this )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CArxLabelCtrl::~CArxLabelCtrl()
{
}

BEGIN_MESSAGE_MAP(CArxLabelCtrl, CLabelCtrl)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CArxLabelCtrl message handlers

void CArxLabelCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	GetArxServices()->HandleEvent( Prop::EventMouseMove, args_NNN( nFlags, point.x, point.y ) );
	__super::OnMouseMove( nFlags, point );
}

void CArxLabelCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	__super::OnLButtonDown( nFlags, point );

	GetArxServices()->HandleEvent( Prop::EventClicked );
	if( mpTemplate->GetBooleanProperty( Prop::DragnDropAllowBegin ) && nFlags == MK_LBUTTON )
		BeginDragDrop( point );
}

__UINT_LRESULT CArxLabelCtrl::OnNcHitTest(CPoint point)
{
	if( !mpTemplate->GetStringProperty(Prop::EventMouseMove).IsEmpty() ||
			!mpTemplate->GetStringProperty(Prop::EventClicked).IsEmpty() ||
			mpTemplate->GetBooleanProperty(Prop::DragnDropAllowBegin) )
		return HTCLIENT;
	return __super::OnNcHitTest( point );
}
