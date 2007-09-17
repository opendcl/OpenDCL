// ArxOptionListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ArxOptionListCtrl.h"
#include "InvokeMethod.h"
#include "SharedRes.h"
#include "AcadColorTable.h"
#include "PropertyIds.h"
#include "DclControlObject.h"
#include "ControlPane.h"
#include "PropertyObject.h"
#include "ToolTips.h"

#define GLYPH_WIDTH 17 


/////////////////////////////////////////////////////////////////////////////
// CArxOptionListCtrl

CArxOptionListCtrl::CArxOptionListCtrl( CDclControlObject* pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
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

	if( GetTemplate()->GetLongProperty(Prop::EventInvoke) == 1 )
		m_bInvokeWithSendString = true;
	else
		m_bInvokeWithSendString = false;

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
	
	//if (GetItemData(nCurSel) == 2)
	//{
	//	SetCurSel(m_CurSel);
	//	return;
	//}
	//if (m_CurSel != nCurSel)
	//{
	//	// we must redraw the default selected item because it will not redraw itself.
	//	CRect rc;
	//	GetItemRect(m_CurSel, rc);
	//	InvalidateRect(rc);		
	//}
	//
	//m_CurSel = nCurSel;

	if (nCurSel > -1)
	{
		CString sString;
	
		int n = GetTextLen(nCurSel);      
		GetText(nCurSel, sString.GetBuffer(n));
		sString.ReleaseBuffer();

		// call methods to invoke the event
		InvokeMethodIntString(mpTemplate->GetStringProperty(Prop::EventSelChanged), nCurSel, sString, m_bInvokeWithSendString);
	}   
}

void CArxOptionListCtrl::OnDblclk() 
{
	// call methods to invoke the event
	InvokeMethod(mpTemplate->GetStringProperty(Prop::EventDblClicked), m_bInvokeWithSendString);
}

void CArxOptionListCtrl::OnKillfocus() 
{
	// call methods to invoke the event
	InvokeMethod(mpTemplate->GetStringProperty(Prop::EventKillFocus), m_bInvokeWithSendString);
}

void CArxOptionListCtrl::OnSetfocus() 
{
	// call methods to invoke the event
	InvokeMethod(mpTemplate->GetStringProperty(Prop::EventSetFocus), m_bInvokeWithSendString);
}

void CArxOptionListCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	__super::OnMouseMove(nFlags, point);

	InvokeMethodIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventMouseMove),
		nFlags,
		point.x,
		point.y,
		m_bInvokeWithSendString);
}

void CArxOptionListCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
  if (nChar == VK_RETURN)
    InvokeMethod(mpTemplate->GetStringProperty(Prop::EventDblClicked), m_bInvokeWithSendString);
	__super::OnKeyDown(nChar,nRepCnt,nFlags);
}
