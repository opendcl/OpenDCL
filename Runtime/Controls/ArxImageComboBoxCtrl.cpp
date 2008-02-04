#include "stdafx.h"
#include "ArxImageComboBoxCtrl.h"
#include "DclControlObject.h"
#include "ControlPane.h"
#include "InvokeMethod.h"


/////////////////////////////////////////////////////////////////////////////
// CArxImageComboBoxCtrl

CArxImageComboBoxCtrl::CArxImageComboBoxCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID,
																							CComboHandler* pHandler /*= NULL*/, bool bCreate /*= true*/ )
: CImageComboBoxCtrl( pTemplate, pPane, nID, pHandler, false )
, mArxServices( this )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CArxImageComboBoxCtrl::~CArxImageComboBoxCtrl()
{
}

bool CArxImageComboBoxCtrl::Create( CWnd* pParentWnd, UINT nID )
{
	bool bSuccess =
		__super::Create( pParentWnd, nID );

	return bSuccess;
}

bool CArxImageComboBoxCtrl::OnApplyProperty( TPropertyPtr pProp )
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

void CArxImageComboBoxCtrl::SetDragnDrop(BOOL bRegister)
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

BEGIN_MESSAGE_MAP(CArxImageComboBoxCtrl, CImageComboBoxCtrl)
	ON_CONTROL_REFLECT(CBN_SELCHANGE, OnSelchange)
	ON_CONTROL_REFLECT(CBN_DROPDOWN, OnDropdown)
	ON_CONTROL_REFLECT(CBN_KILLFOCUS, OnKillfocus)
	ON_CONTROL_REFLECT(CBN_SETFOCUS, OnSetfocus)
	ON_CONTROL_REFLECT(CBN_EDITCHANGE, &CArxImageComboBoxCtrl::OnEditchange)
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CArxImageComboBoxCtrl message handlers

BOOL CArxImageComboBoxCtrl::PreTranslateMessage(MSG* pMsg) 
{
	if( pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN )
	{
		if( mpTemplate->GetBooleanProperty( Prop::ReturnAsTab ) )
			pMsg->wParam = VK_TAB;		
	}
	return __super::PreTranslateMessage(pMsg);
}

void CArxImageComboBoxCtrl::OnSelchange() 
{
	CString sText;
	int nCurSel = -1;
	CEdit* pEditCtrl = GetEditCtrl();
	if( pEditCtrl )
		pEditCtrl->GetWindowText( sText );
	else
	{
		CComboBox* pComboCtrl = GetComboBoxCtrl();
		ASSERT( pComboCtrl != NULL );
		if( pComboCtrl )
		{
			nCurSel = pComboCtrl->GetCurSel();
			if( nCurSel >= 0 )
				pComboCtrl->GetLBText( nCurSel, sText );
		}
	}
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
		InvokeMethodIntString( sEvent, GetCurSel(), sText, IsAsyncEvents() );
}

void CArxImageComboBoxCtrl::OnDropdown() 
{
	InvokeMethod(mpTemplate->GetStringProperty(Prop::EventDropDown), IsAsyncEvents());	
}

void CArxImageComboBoxCtrl::OnKillfocus() 
{
	InvokeMethod(mpTemplate->GetStringProperty(Prop::EventKillFocus), IsAsyncEvents());
}

void CArxImageComboBoxCtrl::OnSetfocus() 
{
	InvokeMethod(mpTemplate->GetStringProperty(Prop::EventSetFocus), IsAsyncEvents());
}

void CArxImageComboBoxCtrl::OnEditchange()
{
	CString sText;
	CEdit* pEditCtrl = GetEditCtrl();
	if( pEditCtrl )
		pEditCtrl->GetWindowText( sText );
	InvokeMethodString(mpTemplate->GetStringProperty(Prop::EventEditChanged), sText, IsAsyncEvents());
}

void CArxImageComboBoxCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	CComboBox::OnMouseMove(nFlags, point);
	InvokeMethodIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventMouseMove),
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());
}
