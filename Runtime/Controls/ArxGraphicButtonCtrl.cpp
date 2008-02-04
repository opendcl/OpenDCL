// ArxGraphicButtonCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ArxGraphicButtonCtrl.h"
#include "DclControlObject.h"
#include "InvokeMethod.h"
#include "ControlPane.h"


/////////////////////////////////////////////////////////////////////////////
// CArxGraphicButtonCtrl

CArxGraphicButtonCtrl::CArxGraphicButtonCtrl( TDclControlPtr pTemplate,
																							CControlPane* pPane,
																							UINT nID,
																							bool bCreate /*= true*/ )
: CGraphicButtonCtrl( pTemplate, pPane, nID, false )
, mArxServices( this )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CArxGraphicButtonCtrl::~CArxGraphicButtonCtrl()
{
}

bool CArxGraphicButtonCtrl::Create( CWnd* pParentWnd, UINT nID )
{
	bool bSuccess =
		__super::Create( pParentWnd, nID );

	return bSuccess;
}

bool CArxGraphicButtonCtrl::OnApplyProperty( TPropertyPtr pProp )
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

void CArxGraphicButtonCtrl::SetDragnDrop(BOOL bRegister)
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

BEGIN_MESSAGE_MAP(CArxGraphicButtonCtrl, CGraphicButtonCtrl)
	ON_WM_MOUSEMOVE()
	ON_CONTROL_REFLECT(BN_CLICKED, OnClicked)
	ON_CONTROL_REFLECT(BN_DOUBLECLICKED, OnDoubleclicked)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// VdclTextButton message handlers

void CArxGraphicButtonCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	InvokeMethodIntIntInt( mpTemplate->GetStringProperty( Prop::EventMouseMove ),
												 nFlags,
												 point.x,
												 point.y,
												 IsAsyncEvents() );
	__super::OnMouseMove( nFlags, point );
}

void CArxGraphicButtonCtrl::OnDoubleclicked() 
{
	InvokeMethod( mpTemplate->GetStringProperty( Prop::EventDblClicked ), false );
}

void CArxGraphicButtonCtrl::OnClicked() 
{	
	CString sEvent = mpTemplate->GetStringProperty(Prop::EventClicked);
	if( sEvent.SpanExcluding( _T("_") ) == _T("c:OnActionEvent") )
	{
		GetParent()->GetParent()->EnableWindow( TRUE );
		resbuf rbEvent = { NULL, RTSTR };
		rbEvent.resval.rstring = sEvent.LockBuffer();
		resbuf* prbResult = NULL;
		acedInvoke( &rbEvent, &prbResult ); 
		if( prbResult ) 
			acutRelRb( prbResult ); 
		GetParent()->GetParent()->EnableWindow( FALSE );
		GetParent()->EnableWindow( TRUE );
	}
	else
		InvokeMethod( sEvent, IsAsyncEvents() );
}

void CArxGraphicButtonCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	__super::OnLButtonDown( nFlags, point );

	if( mpTemplate->GetBooleanProperty( Prop::DragnDropAllowBegin ) && nFlags == MK_LBUTTON )
		BeginDragnDrop( mpTemplate, point, IsAsyncEvents() );
}
