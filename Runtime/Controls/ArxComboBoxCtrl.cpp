#include "stdafx.h"
#include "ArxComboBoxCtrl.h"
#include "DclControlTemplate.h"
#include "ControlPane.h"
#include "InvokeMethod.h"


/////////////////////////////////////////////////////////////////////////////
// CArxComboBoxCtrl

CArxComboBoxCtrl::CArxComboBoxCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID,
																		CComboHandler* pHandler /*= NULL*/, bool bCreate /*= true*/ )
: CComboBoxCtrl( pTemplate, pPane, nID, pHandler, false )
, mArxServices( this )
, mDragDropService( this )
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

BEGIN_MESSAGE_MAP(CArxComboBoxCtrl, CComboBoxCtrl)
	ON_CONTROL_REFLECT(CBN_SELCHANGE, &CArxComboBoxCtrl::OnCbnSelchange)
	ON_CONTROL_REFLECT(CBN_DROPDOWN, &CArxComboBoxCtrl::OnCbnDropdown)
	ON_CONTROL_REFLECT(CBN_KILLFOCUS, &CArxComboBoxCtrl::OnCbnKillfocus)
	ON_CONTROL_REFLECT(CBN_SETFOCUS, &CArxComboBoxCtrl::OnCbnSetfocus)
	ON_CONTROL_REFLECT(CBN_EDITCHANGE, &CArxComboBoxCtrl::OnCbnEditchange)
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CArxComboBoxCtrl message handlers

void CArxComboBoxCtrl::OnCbnSelchange() 
{
	CString sText;
	int nCurSel = GetCurSel();
	if( nCurSel >= 0 )
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
		GetArxServices()->HandleEvent( sEvent, args_NS( nCurSel, sText ) );
}

void CArxComboBoxCtrl::OnCbnDropdown() 
{
	__super::OnCbnDropdown();
	GetArxServices()->HandleEvent( Prop::EventDropDown );	
}

void CArxComboBoxCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	__super::OnMouseMove(nFlags, point);
	GetArxServices()->HandleEvent( Prop::EventMouseMove, args_NNN( nFlags, point.x, point.y ) );
}

void CArxComboBoxCtrl::OnCbnKillfocus() 
{
	GetArxServices()->HandleEvent( Prop::EventKillFocus );
}

void CArxComboBoxCtrl::OnCbnSetfocus() 
{
	GetArxServices()->HandleEvent( Prop::EventSetFocus );
}

void CArxComboBoxCtrl::OnCbnEditchange()
{
	CString sText;
	GetWindowText( sText );
	mpTemplate->SetStringProperty( Prop::Text, sText );
	GetArxServices()->HandleEvent( Prop::EventEditChanged, args_S( sText ) );
}
