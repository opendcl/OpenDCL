// ArxOptionListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ArxOptionListCtrl.h"
#include "InvokeMethod.h"
#include "PropertyIds.h"
#include "DclControlObject.h"
#include "ControlPane.h"
#include "PropertyObject.h"


/////////////////////////////////////////////////////////////////////////////
// CArxOptionListCtrl

CArxOptionListCtrl::CArxOptionListCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: COptionListCtrl( pTemplate, pPane, nID, false )
, mArxServices( pTemplate )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CArxOptionListCtrl::~CArxOptionListCtrl()
{
}

bool CArxOptionListCtrl::Create( CWnd* pParentWnd, UINT nID ) 
{
	bool bSuccess =
		__super::Create( pParentWnd, nID );

	return bSuccess;
}


BEGIN_MESSAGE_MAP(CArxOptionListCtrl, COptionListCtrl)
	ON_CONTROL_REFLECT(LBN_SELCHANGE, OnSelchange)
	ON_CONTROL_REFLECT(LBN_DBLCLK, OnDblclk)
	ON_CONTROL_REFLECT(LBN_KILLFOCUS, OnKillfocus)
	ON_CONTROL_REFLECT(LBN_SETFOCUS, OnSetfocus)
	ON_WM_MOUSEMOVE()
  ON_WM_KEYDOWN()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CArxOptionListCtrl message handlers

void CArxOptionListCtrl::OnSelchange() 
{
	int nCurSel = GetCurSel();
	CString sString;
	GetText(nCurSel, sString);
	InvokeMethodIntString(mpTemplate->GetStringProperty(Prop::EventSelChanged), nCurSel, sString, IsAsyncEvents());
}

void CArxOptionListCtrl::OnDblclk() 
{
	InvokeMethod(mpTemplate->GetStringProperty(Prop::EventDblClicked), IsAsyncEvents());
}

void CArxOptionListCtrl::OnKillfocus() 
{
	InvokeMethod(mpTemplate->GetStringProperty(Prop::EventKillFocus), IsAsyncEvents());
}

void CArxOptionListCtrl::OnSetfocus() 
{
	InvokeMethod(mpTemplate->GetStringProperty(Prop::EventSetFocus), IsAsyncEvents());
}

void CArxOptionListCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	__super::OnMouseMove(nFlags, point);

	InvokeMethodIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventMouseMove),
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());
}

void CArxOptionListCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
  if (nChar == VK_RETURN)
    InvokeMethod(mpTemplate->GetStringProperty(Prop::EventDblClicked), IsAsyncEvents());
	__super::OnKeyDown(nChar,nRepCnt,nFlags);
}
