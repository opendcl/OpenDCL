// ArxCheckBoxCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ArxCheckBoxCtrl.h"
#include "DclControlTemplate.h"
#include "PropertyObject.h"
#include "ControlPane.h"
#include "InvokeMethod.h"
#include "PropertyIds.h"


/////////////////////////////////////////////////////////////////////////////
// CArxCheckBoxCtrl

CArxCheckBoxCtrl::CArxCheckBoxCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CCheckBoxCtrl( pTemplate, pPane, nID, false )
, mArxServices( this )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CArxCheckBoxCtrl::~CArxCheckBoxCtrl()
{
}

bool CArxCheckBoxCtrl::Create( CWnd* pParentWnd, UINT nID ) 
{
	bool bSuccess =
		__super::Create( pParentWnd, nID );

	return bSuccess;
}


BEGIN_MESSAGE_MAP(CArxCheckBoxCtrl, CCheckBoxCtrl)
	ON_WM_MOUSEMOVE()
	ON_CONTROL_REFLECT(BN_CLICKED, OnClicked)
	ON_CONTROL_REFLECT(BN_DOUBLECLICKED, OnDoubleclicked)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CArxCheckBoxCtrl message handlers

void CArxCheckBoxCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	GetArxServices()->HandleEvent( Prop::EventMouseMove, args_NNN( nFlags, point.x, point.y ) );
	__super::OnMouseMove(nFlags, point);
}

void CArxCheckBoxCtrl::OnClicked()
{
	__super::OnClicked();
	int nValue = GetCheck();
	mpTemplate->SetLongProperty( Prop::Value, nValue );

  CString sEvent = mpTemplate->GetStringProperty( Prop::EventClicked );
	if( sEvent.SpanExcluding( _T("_") ) == _T("c:OnActionEvent") )
	{
		GetParent()->GetParent()->EnableWindow( TRUE );
		CString sVal;
		sVal.Format( _T("%d"), nValue );
		resbuf rbValue = { NULL, RTSTR };
		rbValue.resval.rstring = sVal.LockBuffer();
    acedPutSym( _T("$value"), &rbValue );
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
		GetArxServices()->HandleEvent( sEvent, args_N( nValue ) );
}

void CArxCheckBoxCtrl::OnDoubleclicked()
{
	__super::OnDoubleclicked();
	CString sDblClickEvent = mpTemplate->GetStringProperty( Prop::EventDblClicked );
	if( sDblClickEvent.IsEmpty() )
	{
		//treat it as a second single click when there's no double-click event handler
		GetArxServices()->HandleEvent( Prop::EventClicked, args_N( GetCheck() ) );
	}
	else
		GetArxServices()->HandleEvent( sDblClickEvent );
}
