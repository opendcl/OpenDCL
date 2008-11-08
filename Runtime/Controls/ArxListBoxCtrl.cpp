// ArxListBoxCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ArxListBoxCtrl.h"
#include "PropertyObject.h"
#include "DclControlObject.h"
#include "ControlPane.h"
#include "InvokeMethod.h"
#include "PropertyIds.h"


/////////////////////////////////////////////////////////////////////////////
// CArxListBoxCtrl

CArxListBoxCtrl::CArxListBoxCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CListBoxCtrl( pTemplate, pPane, nID, false )
, mArxServices( this )
, mDragDropService( this )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CArxListBoxCtrl::~CArxListBoxCtrl()
{
}

bool CArxListBoxCtrl::Create( CWnd* pParentWnd, UINT nID ) 
{
	bool bSuccess =
		__super::Create( pParentWnd, nID );

	return bSuccess;
}

bool CArxListBoxCtrl::OnDrop( const CPoint& point, COleDataObject* pSourceData,
															DROPEFFECT dropEffect )
{
	if( mpTemplate->GetBooleanProperty( Prop::DragnDropAllowDrop ) )
	{
		CString sDropControlEvent = mpTemplate->GetStringProperty( Prop::DragnDropFromControl );
		if( !sDropControlEvent.IsEmpty() && pSourceData->IsDataAvailable( CDragDropService::GetDclControlClipboardFormat() ) )
		{
			HGLOBAL hData = pSourceData->GetGlobalData( CDragDropService::GetDclControlClipboardFormat() );
			if( !hData )
				return FALSE;
			CDclControlObject* pSourceDclControl = *(CDclControlObject**)GlobalLock( hData );
			GlobalUnlock( hData );
			GlobalFree( hData );
			if( !pSourceDclControl )
				return FALSE;
			CString sProject = pSourceDclControl->GetOwnerProject()->GetKeyName();
			CString sForm = pSourceDclControl->GetOwnerForm()->GetKeyName();
			CString sControl;
			if( pSourceDclControl->GetType() != _CtlForm )
				sControl = pSourceDclControl->GetKeyName();
			InvokeMethod3StringsPoint( sDropControlEvent,
																 sProject,
																 sForm,
																 sControl,
																 point,
																 IsAsyncEvents() );
			return TRUE;
		}

		CString sDropAcadWndPointEvent = mpTemplate->GetStringProperty( Prop::DragnDropFromOther );
		if( !sDropAcadWndPointEvent.IsEmpty() )
		{
			InvokeMethodPoint( sDropAcadWndPointEvent,
												 point,
												 IsAsyncEvents() );
			return TRUE;
		}
	}
	return __super::OnDrop( point, pSourceData, dropEffect )  ;
}


BEGIN_MESSAGE_MAP(CArxListBoxCtrl, CListBoxCtrl)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
	ON_CONTROL_REFLECT(LBN_SELCHANGE, OnSelchange)
	ON_CONTROL_REFLECT(LBN_DBLCLK, OnDblclk)
	ON_CONTROL_REFLECT(LBN_KILLFOCUS, OnKillfocus)
	ON_CONTROL_REFLECT(LBN_SETFOCUS, OnSetfocus)
	ON_WM_RBUTTONUP()
  ON_WM_KEYDOWN()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CArxListBoxCtrl message handlers

void CArxListBoxCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	InvokeMethodIntIntInt( mpTemplate->GetStringProperty( Prop::EventMouseMove ),
												 nFlags,
												 point.x,
												 point.y,
												 IsAsyncEvents() );
	__super::OnMouseMove(nFlags, point);
}

void CArxListBoxCtrl::OnSelchange() 
{
	int nSelCount = GetSelCount();

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
	else
	{	
		if (nSelCount > -1)
			InvokeMethodIntString(sEvent, nSelCount, CString(), IsAsyncEvents());
		else if (nSelCount == -1)
		{
			CString sSelText;
			int nCurSel = GetCurSel();
			if (nCurSel > -1)
				GetText( nCurSel, sSelText );
			InvokeMethodIntString(sEvent, nCurSel, sSelText, IsAsyncEvents());
		}
	}	   
}

void CArxListBoxCtrl::OnDblclk() 
{
	InvokeMethod(mpTemplate->GetStringProperty(Prop::EventDblClicked), IsAsyncEvents());
}

void CArxListBoxCtrl::OnKillfocus() 
{
	InvokeMethod(mpTemplate->GetStringProperty(Prop::EventKillFocus), IsAsyncEvents());
}

void CArxListBoxCtrl::OnSetfocus() 
{
	InvokeMethod(mpTemplate->GetStringProperty(Prop::EventSetFocus), IsAsyncEvents());	
}

BOOL CArxListBoxCtrl::PreTranslateMessage(MSG* pMsg) 
{
	GetToolTipCtrl().RelayEvent(pMsg);
	return __super::PreTranslateMessage(pMsg);
}

void CArxListBoxCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
  if (nChar == VK_RETURN)
	{
			//Change return into a double-click
			InvokeMethod(mpTemplate->GetStringProperty(Prop::EventDblClicked), IsAsyncEvents());
  }
	else
    __super::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CArxListBoxCtrl::OnRButtonUp(UINT nFlags, CPoint point) 
{
	CString sEventName = mpTemplate->GetStringProperty(Prop::EventRightClick);
	if( !sEventName.IsEmpty() )
	{
		// see if the user has right clicked on a list item.
		for( int i = 0; i < GetCount(); ++i )
		{
			CRect rc;
			GetItemRect( i, rc );
			if( !rc.PtInRect( point ) )
				continue;
			__super::SetCurSel( i );
			break;
		}
		InvokeMethod( sEventName, IsAsyncEvents() );
	}
	__super::OnRButtonUp( nFlags, point );
}
