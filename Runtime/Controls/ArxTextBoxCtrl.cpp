#include "stdafx.h"
#include "ArxTextBoxCtrl.h"
#include "DclControlObject.h"
#include "ControlPane.h"
#include "InvokeMethod.h"


/////////////////////////////////////////////////////////////////////////////
// CArxTextBoxCtrl

CArxTextBoxCtrl::CArxTextBoxCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID,
																	CInputFilter* pFilter /*= NULL*/, DWORD dwAcUiStyle /*= 0*/,
																	bool bCreate /*= true*/ )
: CTextBoxCtrl( pTemplate, pPane, nID, pFilter, false )
, mArxServices( pTemplate )
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

bool CArxTextBoxCtrl::OnApplyProperty( TPropertyPtr pProp )
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

void CArxTextBoxCtrl::SetDragnDrop(BOOL bRegister)
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
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CArxTextBoxCtrl message handlers

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
	InvokeMethod( mpTemplate->GetStringProperty( Prop::EventSetFocus ), IsAsyncEvents() );
	mbFocusClick = false;
}

void CArxTextBoxCtrl::OnKillFocus( CWnd* pFocus )
{
	mbFocusClick = false;
	__super::OnKillFocus( pFocus );
	if( mpTemplate->m_bEventsAsAction )
	{
		GetParent()->GetParent()->EnableWindow(TRUE);
		struct resbuf *result = NULL, *list;    
		CString sVal = mpTemplate->GetStringProperty( Prop::Text );
    struct resbuf *VarVal = acutBuildList(RTSTR, (LPCTSTR)sVal, 0);
		int stat = acedPutSym(_T("$value"), VarVal);
    acutRelRb(VarVal);
		list = acutBuildList(RTSTR, (LPCTSTR)mpTemplate->GetStringProperty( Prop::EventEditChanged ), 0);
		if (list != NULL) 
		{ 
			stat = acedInvoke(list, &result); 
			acutRelRb(list); 
			if(result != NULL) 
				acutRelRb(result); 
		}
		GetParent()->GetParent()->EnableWindow(FALSE);
		GetParent()->EnableWindow(TRUE);
	}
	else
		InvokeMethod( mpTemplate->GetStringProperty( Prop::EventKillFocus ), IsAsyncEvents() );
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

void CArxTextBoxCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	__super::OnLButtonUp(nFlags, point);
	if( !mbFocusClick )
	{
		InvokeMethod( mpTemplate->GetStringProperty( Prop::EventSetFocus ), IsAsyncEvents() );
		mbFocusClick = true;
	}
}

void CArxTextBoxCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	if( mpTemplate->GetBooleanProperty( Prop::DragnDropAllowBegin ) && nFlags == 1 )
		BeginDragnDrop( mpTemplate, point, IsAsyncEvents() );
	__super::OnLButtonDown(nFlags, point);
}
