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

CArxFolderComboCtrl::CArxFolderComboCtrl( CDclControlObject* pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CFolderComboCtrl( pTemplate, pPane, nID, false )
, mArxServices( pTemplate )
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

	if( GetTemplate()->GetLongProperty(Prop::EventInvoke) == 1 )
		m_bInvokeWithSendString = true;
	else
		m_bInvokeWithSendString = false;

	POSITION pos = mpTemplate->GetOwnerForm()->GetControlList().GetHeadPosition();
	while( pos )
	{
		CDclControlObject* pDclControl = mpTemplate->GetOwnerForm()->GetControlList().GetNext( pos );
		if( pDclControl->GetType() == CtlDwgList && pDclControl->GetWindow() )
		{
			CArxDwgListCtrl* pDwgList = (CArxDwgListCtrl*)pDclControl->GetWindow();
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
	//GetWindowText( sPath );

	if( mpDwgList )
		mpDwgList->Dir( sPath );

	CString sEventName = mpTemplate->GetStringProperty(Prop::EventSelChanged);
	if( !sEventName.IsEmpty() )
	{
		if( mpTemplate->m_bEventsAsAction )
		{
			GetParent()->GetParent()->EnableWindow(TRUE);
			CString sVal;
			sVal.Format( _T("%d"), GetCurSel() );
			resbuf rbVal = { NULL, RTSTR };
			rbVal.resval.rstring = (LPTSTR)sVal.LockBuffer();
      acedPutSym( _T("$value"), &rbVal );
			resbuf rbEvent = { NULL, RTSTR };
			rbEvent.resval.rstring = (LPTSTR)sEventName.LockBuffer();
			struct resbuf* prbResult = NULL;
			acedInvoke( &rbEvent, &prbResult );
			if( prbResult )
				acutRelRb( prbResult );
			GetParent()->GetParent()->EnableWindow(FALSE);
			GetParent()->EnableWindow(TRUE);
		}
		else
		{
			CString sLispSafePath = sPath;
			sLispSafePath.Replace( _T("\\"), _T("\\\\") );
			InvokeMethodIntString( sEventName, GetCurSel(), sLispSafePath, m_bInvokeWithSendString );
			mpTemplate->SetStringProperty( Prop::Text, sPath );
		}
	}
	return 0;
}

void CArxFolderComboCtrl::OnDropdown() 
{
	InvokeMethod(mpTemplate->GetStringProperty(Prop::EventDropDown), m_bInvokeWithSendString);	
}

void CArxFolderComboCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	__super::OnMouseMove(nFlags, point);
	InvokeMethodIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventMouseMove),
		nFlags,
		point.x,
		point.y,
		m_bInvokeWithSendString);
}

void CArxFolderComboCtrl::OnKillfocus() 
{
	InvokeMethod(mpTemplate->GetStringProperty(Prop::EventKillFocus), m_bInvokeWithSendString);
}

void CArxFolderComboCtrl::OnSetfocus() 
{
	InvokeMethod(mpTemplate->GetStringProperty(Prop::EventSetFocus), m_bInvokeWithSendString);
}
