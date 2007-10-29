// ArxTextButtonCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ArxTextButtonCtrl.h"
#include "DclControlObject.h"
#include "InvokeMethod.h"
#include "ControlPane.h"


/////////////////////////////////////////////////////////////////////////////
// CArxTextButtonCtrl

CArxTextButtonCtrl::CArxTextButtonCtrl( TDclControlPtr pTemplate,
																				CControlPane* pPane,
																				UINT nID,
																				bool bCreate /*= true*/ )
: CTextButtonCtrl( pTemplate, pPane, nID, false )
, mArxServices( pTemplate )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CArxTextButtonCtrl::~CArxTextButtonCtrl()
{
}

bool CArxTextButtonCtrl::Create( CWnd* pParentWnd, UINT nID )
{
	bool bSuccess =
		__super::Create( pParentWnd, nID );

	return bSuccess;
}

bool CArxTextButtonCtrl::OnApplyProperty( TPropertyPtr pProp )
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

void CArxTextButtonCtrl::SetDragnDrop(BOOL bRegister)
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

BEGIN_MESSAGE_MAP(CArxTextButtonCtrl, CTextButtonCtrl)
	ON_WM_MOUSEMOVE()
	ON_CONTROL_REFLECT(BN_CLICKED, OnClicked)
	ON_CONTROL_REFLECT(BN_DOUBLECLICKED, OnDoubleclicked)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// VdclTextButton message handlers

void CArxTextButtonCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	InvokeMethodIntIntInt( mpTemplate->GetStringProperty( Prop::EventMouseMove ),
												 nFlags,
												 point.x,
												 point.y,
												 IsAsyncEvents() );
	__super::OnMouseMove( nFlags, point );
}

void CArxTextButtonCtrl::OnDoubleclicked() 
{
	InvokeMethod( mpTemplate->GetStringProperty( Prop::EventDblClicked ), false );
}

void CArxTextButtonCtrl::OnClicked() 
{	
	if( mpTemplate->m_bEventsAsAction )
	{
		GetParent()->GetParent()->EnableWindow( TRUE );
		struct resbuf *result = NULL, *list;    

		CString sText = mpTemplate->GetStringProperty( Prop::EventClicked );
		list = acutBuildList( RTSTR, sText, 0 );
		if( list != NULL ) 
		{ 
			int stat = acedInvoke( list, &result ); 
			acutRelRb( list ); 
			if( result ) 
				acutRelRb( result ); 
		}
		GetParent()->GetParent()->EnableWindow( FALSE );
		GetParent()->EnableWindow( TRUE );
	}
	else
		InvokeMethod( mpTemplate->GetStringProperty( Prop::EventClicked ), IsAsyncEvents() );
}

void CArxTextButtonCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	__super::OnLButtonDown( nFlags, point );

	if( mpTemplate->GetBooleanProperty( Prop::DragnDropAllowBegin ) && nFlags == MK_LBUTTON )
		BeginDragnDrop( mpTemplate, point, IsAsyncEvents() );
}
