#include "stdafx.h"
#include "ArxComboBoxCtrl.h"
#include "DclControlObject.h"
#include "ControlPane.h"
#include "InvokeMethod.h"


/////////////////////////////////////////////////////////////////////////////
// CArxComboBoxCtrl

CArxComboBoxCtrl::CArxComboBoxCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID,
																		CComboHandler* pHandler /*= NULL*/, bool bCreate /*= true*/ )
: CComboBoxCtrl( pTemplate, pPane, nID, pHandler, false )
, mArxServices( this )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CArxComboBoxCtrl::~CArxComboBoxCtrl()
{
}

bool CArxComboBoxCtrl::Create( CWnd* pParentWnd, UINT nID )
{
	bool bSuccess =
		__super::Create( pParentWnd, nID );

	return bSuccess;
}

bool CArxComboBoxCtrl::OnApplyProperty( TPropertyPtr pProp )
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

void CArxComboBoxCtrl::SetDragnDrop(BOOL bRegister)
{
	if (bRegister == TRUE)
	{
		BOOL success = m_DropTarget.Register(this);
		m_DropTarget.m_pThisArxControl = mpTemplate;
		m_DropTarget.m_pParent = this;
	}
	else
		m_DropTarget.Revoke();
}

BEGIN_MESSAGE_MAP(CArxComboBoxCtrl, CComboBoxCtrl)
	ON_CONTROL_REFLECT(CBN_SELCHANGE, OnSelchange)
	ON_CONTROL_REFLECT(CBN_DROPDOWN, OnDropdown)
	ON_CONTROL_REFLECT(CBN_KILLFOCUS, OnKillfocus)
	ON_CONTROL_REFLECT(CBN_SETFOCUS, OnSetfocus)
	ON_CONTROL_REFLECT(CBN_EDITCHANGE, &CArxComboBoxCtrl::OnEditchange)
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CArxComboBoxCtrl message handlers

void CArxComboBoxCtrl::OnSelchange() 
{
	int nCurSel = GetCurSel();
	CString sText;
	GetLBText( nCurSel, sText );
	mpTemplate->SetStringProperty( Prop::Text, sText );
	CString sEvent = mpTemplate->GetStringProperty(Prop::EventSelChanged);
	if( sEvent.SpanExcluding( _T("_") ) == _T("c:OnActionEvent") )
	{
		GetParent()->GetParent()->EnableWindow( TRUE );
		CString sVal;
		sVal.Format( _T("%d"), nCurSel );
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
		InvokeMethodIntString( sEvent, nCurSel, sText, IsAsyncEvents() );
}

void CArxComboBoxCtrl::OnDropdown() 
{
	__super::OnDropdown();
	InvokeMethod(mpTemplate->GetStringProperty(Prop::EventDropDown), IsAsyncEvents());	
}

void CArxComboBoxCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	__super::OnMouseMove(nFlags, point);
	InvokeMethodIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventMouseMove),
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());
}

void CArxComboBoxCtrl::OnKillfocus() 
{
	InvokeMethod(mpTemplate->GetStringProperty(Prop::EventKillFocus), IsAsyncEvents());
}

void CArxComboBoxCtrl::OnSetfocus() 
{
	InvokeMethod(mpTemplate->GetStringProperty(Prop::EventSetFocus), IsAsyncEvents());
}

void CArxComboBoxCtrl::OnEditchange()
{
	CString sText;
	GetWindowText( sText );
	InvokeMethodString(mpTemplate->GetStringProperty(Prop::EventEditChanged), sText, IsAsyncEvents());
}
