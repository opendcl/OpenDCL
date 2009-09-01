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
	{
		if( GetArxServices()->HandleEvent( Prop::EventReturnPressed ) )
			return TRUE;
	}
	return __super::PreTranslateMessage(pMsg);
}

void CArxTextBoxCtrl::OnChange() 
{
	CString sOldValue = mpTemplate->GetStringProperty( Prop::Text );
	__super::OnChange();
	CString sNewValue = mpTemplate->GetStringProperty( Prop::Text );
	if( sNewValue != sOldValue )
		GetArxServices()->HandleEvent( Prop::EventEditChanged, args_S( sNewValue ) );
}

void CArxTextBoxCtrl::OnErrspace() 
{
	GetArxServices()->HandleEvent( Prop::EventOutOfMemory );
}

void CArxTextBoxCtrl::OnMaxtext() 
{
	GetArxServices()->HandleEvent( Prop::EventMaxText );
}

void CArxTextBoxCtrl::OnUpdate() 
{
	if( IsWindowVisible() )
	{
		GetArxServices()->HandleEvent( Prop::EventEditChanged, args_S( mpTemplate->GetStringProperty( Prop::Text ) ) );
	}
}

void CArxTextBoxCtrl::OnSetFocus( CWnd* pFocus )
{
	__super::OnSetFocus( pFocus );
	if( !mbFocusClick )
		GetArxServices()->HandleEvent( Prop::EventSetFocus );
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
		GetArxServices()->HandleEvent( sEvent );
}

void CArxTextBoxCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	GetArxServices()->HandleEvent( Prop::EventMouseMove, args_NNN( nFlags, point.x, point.y ) );
	__super::OnMouseMove(nFlags, point);
}


void CArxTextBoxCtrl::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	GetArxServices()->HandleEvent( Prop::EventKeyUp, args_CNN( (TCHAR)nChar, nRepCnt, nFlags ) );
	__super::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CArxTextBoxCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	GetArxServices()->HandleEvent( Prop::EventKeyDown, args_CNN( (TCHAR)nChar, nRepCnt, nFlags ) );
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
	GetArxServices()->HandleEvent( Prop::EventSetFocus );
	return 0;
}
