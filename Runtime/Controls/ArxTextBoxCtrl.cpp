#include "stdafx.h"
#include "ArxTextBoxCtrl.h"
#include "DclControlObject.h"
#include "ControlPane.h"
#include "InvokeMethod.h"

static const UINT& refWM_FOCUSCLICK()
{
	static const UINT WM_FOCUSCLICK = RegisterWindowMessage( _T("OpenDCL.FocusClick") );
	return WM_FOCUSCLICK;
}


/////////////////////////////////////////////////////////////////////////////
// CArxTextBoxCtrl

CArxTextBoxCtrl::CArxTextBoxCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID,
																	CInputFilter* pFilter /*= NULL*/, DWORD dwAcUiStyle /*= 0*/,
																	bool bCreate /*= true*/ )
: CTextBoxCtrl( pTemplate, pPane, nID, pFilter, false )
, mArxServices( this )
, mDragDropService( this )
, mbFocusClick( false )
{
	SetStyleMask( dwAcUiStyle );
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CArxTextBoxCtrl::~CArxTextBoxCtrl()
{
}

bool CArxTextBoxCtrl::Create( CWnd* pParentWnd, UINT nID )
{
	bool bSuccess =
		__super::Create( pParentWnd, nID );

	return bSuccess;
}

BEGIN_MESSAGE_MAP(CArxTextBoxCtrl, CTextBoxCtrl)
	ON_CONTROL_REFLECT(EN_CHANGE, &CArxTextBoxCtrl::OnChange)
	ON_CONTROL_REFLECT(EN_ERRSPACE, &CArxTextBoxCtrl::OnErrspace)
	ON_CONTROL_REFLECT(EN_MAXTEXT, &CArxTextBoxCtrl::OnMaxtext)
	ON_CONTROL_REFLECT(EN_UPDATE, &CArxTextBoxCtrl::OnUpdate)
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_MOUSEMOVE()
	ON_WM_KEYUP()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_REGISTERED_MESSAGE(refWM_FOCUSCLICK(),OnFocusClick)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CArxTextBoxCtrl message handlers

BOOL CArxTextBoxCtrl::PreTranslateMessage(MSG* pMsg) 
{
	if( pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN )
		InvokeMethod(mpTemplate->GetStringProperty(Prop::EventReturnPressed), IsAsyncEvents());
	return __super::PreTranslateMessage(pMsg);
}

void CArxTextBoxCtrl::OnChange() 
{
	CString sOldValue = mpTemplate->GetStringProperty( Prop::Text );
	__super::OnChange();
	CString sNewValue = mpTemplate->GetStringProperty( Prop::Text );
	if( sNewValue != sOldValue )
		InvokeMethodString( mpTemplate->GetStringProperty( Prop::EventEditChanged ),
												sNewValue,
												IsAsyncEvents());
}

void CArxTextBoxCtrl::OnErrspace() 
{
	InvokeMethod( mpTemplate->GetStringProperty( Prop::EventOutOfMemory ), IsAsyncEvents() );
}

void CArxTextBoxCtrl::OnMaxtext() 
{
	InvokeMethod( mpTemplate->GetStringProperty( Prop::EventMaxText ), IsAsyncEvents() );
}

void CArxTextBoxCtrl::OnUpdate() 
{
	if( IsWindowVisible() )
	{
		InvokeMethodString( mpTemplate->GetStringProperty( Prop::EventUpdate ),
												mpTemplate->GetStringProperty( Prop::Text ),
												IsAsyncEvents() );
	}
}

void CArxTextBoxCtrl::OnSetFocus( CWnd* pFocus )
{
	__super::OnSetFocus( pFocus );
	if( !mbFocusClick )
		InvokeMethod( mpTemplate->GetStringProperty( Prop::EventSetFocus ), IsAsyncEvents() );
	else
	{
		mbFocusClick = false;
		PostMessage( refWM_FOCUSCLICK() );
	}
}

void CArxTextBoxCtrl::OnKillFocus( CWnd* pFocus )
{
	mbFocusClick = false;
	__super::OnKillFocus( pFocus );
	CString sEvent = mpTemplate->GetStringProperty(Prop::EventKillFocus);
	if( sEvent.SpanExcluding( _T("_") ) == _T("c:OnActionEvent") )
	{
		GetParent()->GetParent()->EnableWindow( TRUE );
		resbuf rbValue = { NULL, RTSTR };
		CString sVal = mpTemplate->GetStringProperty( Prop::Text );
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
		InvokeMethod( sEvent, IsAsyncEvents() );
}

void CArxTextBoxCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	InvokeMethodIntIntInt( mpTemplate->GetStringProperty( Prop::EventMouseMove ),
												 nFlags,
												 point.x,
												 point.y,
												 IsAsyncEvents() );	
	__super::OnMouseMove(nFlags, point);
}


void CArxTextBoxCtrl::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	InvokeMethodStringIntInt( mpTemplate->GetStringProperty( Prop::EventKeyUp ),
														CString( (TCHAR)nChar ), (int)nRepCnt, (int)nFlags, IsAsyncEvents() );
	__super::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CArxTextBoxCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	InvokeMethodStringIntInt( mpTemplate->GetStringProperty( Prop::EventKeyDown ),
														CString( (TCHAR)nChar ), (int)nRepCnt, (int)nFlags, IsAsyncEvents() );
	__super::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CArxTextBoxCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	if( GetFocus() != this )
		mbFocusClick = true;
	__super::OnLButtonDown(nFlags, point);
}

void CArxTextBoxCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	mbFocusClick = false;
	__super::OnLButtonUp(nFlags, point);
}

LRESULT CArxTextBoxCtrl::OnFocusClick(WPARAM wParam, LPARAM lParam)
{
	InvokeMethod( mpTemplate->GetStringProperty( Prop::EventSetFocus ), IsAsyncEvents() );
	return 0;
}
