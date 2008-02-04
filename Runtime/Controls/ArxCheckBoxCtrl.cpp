// ArxCheckBoxCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ArxCheckBoxCtrl.h"
#include "DclControlObject.h"
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
	InvokeMethodIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventMouseMove),
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());
	__super::OnMouseMove(nFlags, point);
}

void CArxCheckBoxCtrl::OnClicked() 
{
	int nValue = GetCheck();
	mpTemplate->SetLongProperty( Prop::Value, nValue );

  CString sEvent = mpTemplate->GetStringProperty(Prop::EventClicked);
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
		InvokeMethodInt( sEvent, nValue, IsAsyncEvents());
}

void CArxCheckBoxCtrl::OnDoubleclicked() 
{
	InvokeMethod(mpTemplate->GetStringProperty(Prop::EventDblClicked), IsAsyncEvents());
}
