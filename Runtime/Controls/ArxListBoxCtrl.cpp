// ArxListBoxCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ArxListBoxCtrl.h"
#include "PropertyObject.h"
#include "DclControlTemplate.h"
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

bool CArxListBoxCtrl::OnDrop( const CPoint& point, COleDataObject* pSourceData, DROPEFFECT& dropEffect )
{
	if( mpTemplate->GetBooleanProperty( Prop::DragnDropAllowDrop ) )
	{
		CString sDropControlEvent = mpTemplate->GetStringProperty( Prop::DragnDropFromControl );
		if( !sDropControlEvent.IsEmpty() && pSourceData->IsDataAvailable( CDragDropService::GetDclControlClipboardFormat() ) )
		{
			HGLOBAL hData = pSourceData->GetGlobalData( CDragDropService::GetDclControlClipboardFormat() );
			if( !hData )
				return false;
			CDclControlTemplate* pSourceDclControl = *(CDclControlTemplate**)GlobalLock( hData );
			GlobalUnlock( hData );
			GlobalFree( hData );
			if( !pSourceDclControl )
				return false;
			CString sProject = pSourceDclControl->GetOwnerProject()->GetKeyName();
			CString sForm = pSourceDclControl->GetOwnerForm()->GetKeyName();
			CString sControl;
			if( pSourceDclControl->GetType() != _CtlForm )
				sControl = pSourceDclControl->GetKeyName();
			GetArxServices()->HandleEvent( sDropControlEvent, args_SSSP( sProject, sForm, sControl, point ) );
			return true;
		}

		CString sDropAcadWndPointEvent = mpTemplate->GetStringProperty( Prop::DragnDropFromOther );
		if( !sDropAcadWndPointEvent.IsEmpty() )
		{
			GetArxServices()->HandleEvent( sDropAcadWndPointEvent, args_P( point ) );
			return true;
		}
	}
	return __super::OnDrop( point, pSourceData, dropEffect )  ;
}


BEGIN_MESSAGE_MAP(CArxListBoxCtrl, CListBoxCtrl)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
	ON_CONTROL_REFLECT(LBN_SELCHANGE, OnCbnSelchange)
	ON_CONTROL_REFLECT(LBN_DBLCLK, OnDblclk)
	ON_CONTROL_REFLECT(LBN_KILLFOCUS, OnKillfocus)
	ON_CONTROL_REFLECT(LBN_SETFOCUS, OnSetfocus)
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CArxListBoxCtrl message handlers

void CArxListBoxCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	GetArxServices()->HandleEvent( Prop::EventMouseMove, args_NNN( nFlags, point.x, point.y ) );
	__super::OnMouseMove(nFlags, point);
}

void CArxListBoxCtrl::OnCbnSelchange() 
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
			GetArxServices()->HandleEvent( sEvent, args_NS( nSelCount, _T("") ) );
		else if (nSelCount == -1)
		{
			CString sSelText;
			int nCurSel = GetCurSel();
			if (nCurSel > -1)
				GetText( nCurSel, sSelText );
			GetArxServices()->HandleEvent( sEvent, args_NS( nCurSel, sSelText ) );
		}
	}	   
}

void CArxListBoxCtrl::OnDblclk() 
{
	GetArxServices()->HandleEvent( Prop::EventDblClicked );
}

void CArxListBoxCtrl::OnKillfocus() 
{
	GetArxServices()->HandleEvent( Prop::EventKillFocus );
}

void CArxListBoxCtrl::OnSetfocus() 
{
	GetArxServices()->HandleEvent( Prop::EventSetFocus );	
}

BOOL CArxListBoxCtrl::PreTranslateMessage(MSG* pMsg) 
{
	if( pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN )
	{
		CString sEvent = mpTemplate->GetStringProperty(Prop::EventDblClicked);
		if( !sEvent.IsEmpty() )
		{
			GetArxServices()->HandleEvent( sEvent );
			return TRUE;
		}
	}
	return __super::PreTranslateMessage(pMsg);
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
		GetArxServices()->HandleEvent( sEventName );
	}
	__super::OnRButtonUp( nFlags, point );
}
