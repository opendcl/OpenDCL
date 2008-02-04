// ArxFolderComboCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ArxFolderComboCtrl.h"
#include "DclControlObject.h"
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

bool CArxFolderComboCtrl::OnApplyProperty( TPropertyPtr pProp )
{
	if( !__super::OnApplyProperty( pProp ) )
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
	ON_REGISTERED_MESSAGE(CFolderTreeCtrl::refWM_SELCHANGE(), OnSelchange)
	ON_CONTROL_REFLECT(CBN_DROPDOWN, OnDropdown)
	ON_CONTROL_REFLECT(CBN_KILLFOCUS, OnKillfocus)
	ON_CONTROL_REFLECT(CBN_SETFOCUS, OnSetfocus)
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

LRESULT CArxFolderComboCtrl::OnSelchange( WPARAM wParam, LPARAM lParam )
{
	__super::OnSelchange( wParam, lParam );

	CString sPath;
	CFolder* pFolderInfo = (CFolder*)lParam;
	if( pFolderInfo )
		sPath = (LPCTSTR)pFolderInfo->m_path;
	mpTemplate->SetStringProperty( Prop::Text, sPath );

	if( mpDwgList )
		mpDwgList->Dir( sPath );

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
	{
		CString sLispSafePath = sPath;
		sLispSafePath.Replace( _T("\\"), _T("\\\\") );
		InvokeMethodIntString( sEvent, GetCurSel(), sLispSafePath, IsAsyncEvents() );
	}
	return 0;
}

void CArxFolderComboCtrl::OnDropdown() 
{
	InvokeMethod(mpTemplate->GetStringProperty(Prop::EventDropDown), IsAsyncEvents());	
}

void CArxFolderComboCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	__super::OnMouseMove(nFlags, point);
	InvokeMethodIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventMouseMove),
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());
}

void CArxFolderComboCtrl::OnKillfocus() 
{
	InvokeMethod(mpTemplate->GetStringProperty(Prop::EventKillFocus), IsAsyncEvents());
}

void CArxFolderComboCtrl::OnSetfocus() 
{
	InvokeMethod(mpTemplate->GetStringProperty(Prop::EventSetFocus), IsAsyncEvents());
}
