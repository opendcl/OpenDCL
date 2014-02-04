// ArxFolderComboCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ArxFolderComboCtrl.h"
#include "DclControlTemplate.h"
#include "ControlPane.h"
#include "InvokeMethod.h"
#include "ArxDwgListCtrl.h"


/////////////////////////////////////////////////////////////////////////////
// CArxFolderComboCtrl

CArxFolderComboCtrl::CArxFolderComboCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CFolderComboCtrl( pTemplate, pPane, nID, false )
, mArxServices( this )
, mpDwgList( NULL )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CArxFolderComboCtrl::~CArxFolderComboCtrl()
{
}

bool CArxFolderComboCtrl::Create( CWnd* pParentWnd, UINT nID )
{
	bool bSuccess =
		__super::Create( pParentWnd, nID );

	const TDclControlList& Controls = mpTemplate->GetOwnerForm()->GetControlList();
	for( TDclControlList::const_iterator iter = Controls.begin(); iter != Controls.end(); ++iter )
	{
		if( (*iter)->GetType() == CtlDwgList && (*iter)->GetWindow() )
		{
			CArxDwgListCtrl* pDwgList = (CArxDwgListCtrl*)(*iter)->GetWindow();
			pDwgList->m_pDirComboBox = this;
			mpDwgList = pDwgList;
			break;
		}
	}

	return bSuccess;
}

bool CArxFolderComboCtrl::ApplyProperty( TPropertyPtr pProp )
{
	if( !__super::ApplyProperty( pProp ) )
		return false;
	bool bFailed = false;
	//switch( pProp->GetID() )
	//{
	//case Prop::DragnDropAllowDrop:
	//	{
	//		SetDragnDrop( pProp->GetBooleanValue() );
	//		break;
	//	}
	//}
	return !bFailed;
}

BEGIN_MESSAGE_MAP(CArxFolderComboCtrl, CFolderComboCtrl)
	ON_CONTROL_REFLECT(CBN_SELCHANGE, OnCbnSelchange)
	ON_CONTROL_REFLECT(CBN_DROPDOWN, OnCbnDropdown)
	ON_CONTROL_REFLECT(CBN_CLOSEUP, OnCbnCloseup)
	ON_CONTROL_REFLECT(CBN_KILLFOCUS, OnCbnKillfocus)
	ON_CONTROL_REFLECT(CBN_SETFOCUS, OnCbnSetfocus)
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CArxFolderComboCtrl message handlers

BOOL CArxFolderComboCtrl::PreTranslateMessage(MSG* pMsg) 
{
	if( pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN )
	{
		if( mpTemplate->GetBooleanProperty( Prop::ReturnAsTab ) )
			pMsg->wParam = VK_TAB;		
	}
	return __super::PreTranslateMessage(pMsg);
}

void CArxFolderComboCtrl::OnCbnSelchange()
{
	CString sPath = GetSelectedPath();
	if( sPath.CompareNoCase( mpTemplate->GetStringProperty( Prop::Text ) ) != 0 )
	{
		mpTemplate->SetStringProperty( Prop::Text, sPath );

		if( mpDwgList )
			mpDwgList->Dir( sPath );
	}

	CString sEvent = mpTemplate->GetStringProperty(Prop::EventSelChanged);
	if( sEvent.SpanExcluding( _T("_") ) == _T("c:OnActionEvent") )
	{
		GetParent()->GetParent()->EnableWindow( TRUE );
		CString sVal;
		sVal.Format( _T("%d"), GetCurSel() );
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
	else if( !sEvent.IsEmpty() )
		GetArxServices()->HandleEvent( sEvent, args_NS( GetCurSel(), sPath ) );
}

void CArxFolderComboCtrl::OnCbnDropdown() 
{
	GetArxServices()->HandleEvent( Prop::EventDropDown );	
}

void CArxFolderComboCtrl::OnCbnCloseup() 
{
	CString sPath = GetSelectedPath();
	if( sPath != mpTemplate->GetStringProperty( Prop::Text ) )
	{
		mpTemplate->SetStringProperty( Prop::Text, sPath );
		if( mpDwgList )
			mpDwgList->Dir( sPath );
	}
}

void CArxFolderComboCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	__super::OnMouseMove(nFlags, point);
	GetArxServices()->HandleEvent( Prop::EventMouseMove, args_NNN( nFlags, point.x, point.y ) );
}

void CArxFolderComboCtrl::OnCbnKillfocus() 
{
	GetArxServices()->HandleEvent( Prop::EventKillFocus );
}

void CArxFolderComboCtrl::OnCbnSetfocus() 
{
	GetArxServices()->HandleEvent( Prop::EventSetFocus );
}
