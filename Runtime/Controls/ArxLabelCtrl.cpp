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
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CArxLabelCtrl::~CArxLabelCtrl()
{
}

bool CArxLabelCtrl::OnApplyProperty( TPropertyPtr pProp )
{
	if( !__super::OnApplyProperty( pProp ) )
		return false;
	bool bFailed = false;
	switch( pProp->GetID() )
	{
	case Prop::DragnDropAllowDrop:
		{
			SetDragnDrop( pProp->GetBooleanValue() );
			break;
		}
	}
	return !bFailed;
}

void CArxLabelCtrl::SetDragnDrop(BOOL bRegister)
{
	if( bRegister )
	{
		BOOL success = mDropTarget.Register( this );
		mDropTarget.m_pThisArxControl = mpTemplate;
		mDropTarget.m_pParent = this;
	}
	else
		mDropTarget.Revoke();
}

BEGIN_MESSAGE_MAP(CArxLabelCtrl, CLabelCtrl)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// VdclTextButton message handlers

void CArxLabelCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	InvokeMethodIntIntInt( mpTemplate->GetStringProperty( Prop::EventMouseMove ),
												 nFlags,
												 point.x,
												 point.y,
												 IsAsyncEvents() );
	__super::OnMouseMove( nFlags, point );
}

void CArxLabelCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	__super::OnLButtonDown( nFlags, point );

	InvokeMethod( mpTemplate->GetStringProperty( Prop::EventClicked ), IsAsyncEvents() );
	if( mpTemplate->GetBooleanProperty( Prop::DragnDropAllowBegin ) && nFlags == MK_LBUTTON )
		BeginDragnDrop( mpTemplate, point, IsAsyncEvents() );
}

__UINT_LRESULT CArxLabelCtrl::OnNcHitTest(CPoint point)
{
	if( !mpTemplate->GetStringProperty(Prop::EventMouseMove).IsEmpty() ||
			!mpTemplate->GetStringProperty(Prop::EventClicked).IsEmpty() ||
			mpTemplate->GetBooleanProperty(Prop::DragnDropAllowBegin) )
		return HTCLIENT;
	return __super::OnNcHitTest( point );
}
