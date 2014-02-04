// ArxTextButtonCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ArxTextButtonCtrl.h"
#include "DclControlTemplate.h"
#include "InvokeMethod.h"
#include "ControlPane.h"


/////////////////////////////////////////////////////////////////////////////
// CArxTextButtonCtrl

CArxTextButtonCtrl::CArxTextButtonCtrl( TDclControlPtr pTemplate,
																				CControlPane* pPane,
																				UINT nID,
																				bool bCreate /*= true*/ )
: CTextButtonCtrl( pTemplate, pPane, nID, false )
, mArxServices( this )
, mDragDropService( this )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CArxTextButtonCtrl::~CArxTextButtonCtrl()
{
}

bool CArxTextButtonCtrl::Create( CWnd* pParentWnd, UINT nID )
{
	bool bSuccess =
		__super::Create( pParentWnd, nID );

	return bSuccess;
}

BEGIN_MESSAGE_MAP(CArxTextButtonCtrl, CTextButtonCtrl)
	ON_WM_MOUSEMOVE()
	ON_CONTROL_REFLECT(BN_CLICKED, OnClicked)
	ON_CONTROL_REFLECT(BN_DOUBLECLICKED, OnDoubleclicked)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// VdclTextButton message handlers

void CArxTextButtonCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	GetArxServices()->HandleEvent( Prop::EventMouseMove, args_NNN( nFlags, point.x, point.y ) );
	__super::OnMouseMove( nFlags, point );
}

void CArxTextButtonCtrl::OnDoubleclicked() 
{
	GetArxServices()->HandleEvent( Prop::EventDblClicked );
}

void CArxTextButtonCtrl::OnClicked() 
{	
	CString sEvent = mpTemplate->GetStringProperty(Prop::EventClicked);
	if( sEvent.SpanExcluding( _T("_") ) == _T("c:OnActionEvent") )
	{
		GetParent()->GetParent()->EnableWindow( TRUE );
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

void CArxTextButtonCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	__super::OnLButtonDown( nFlags, point );

	if( mpTemplate->GetBooleanProperty( Prop::DragnDropAllowBegin ) && nFlags == MK_LBUTTON )
	{
		DROPEFFECT dwDropEffect = BeginDragDrop( point );
		SendMessage( WM_LBUTTONUP, nFlags, MAKELPARAM(point.x, point.y) );	
		if( dwDropEffect != DROPEFFECT_NONE )
			return;
		GetParent()->SendMessage( WM_COMMAND, MAKEWPARAM( GetControlId(), BN_CLICKED ), (LPARAM)m_hWnd );
	}
}
