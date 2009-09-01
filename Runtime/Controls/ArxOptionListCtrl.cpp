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
, mArxServices( this )
, mDragDropService( this )
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
	ON_CONTROL_REFLECT(LBN_SELCHANGE, OnLbnSelchange)
	ON_CONTROL_REFLECT(LBN_DBLCLK, OnDblclk)
	ON_CONTROL_REFLECT(LBN_KILLFOCUS, OnKillfocus)
	ON_CONTROL_REFLECT(LBN_SETFOCUS, OnSetfocus)
	ON_WM_MOUSEMOVE()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CArxOptionListCtrl message handlers

void CArxOptionListCtrl::OnLbnSelchange()
{
	__super::OnLbnSelchange();
	int nCurSel = GetCurSel();
	CString sString;
	GetText(nCurSel, sString);
	GetArxServices()->HandleEvent( Prop::EventSelChanged, args_NS( nCurSel, sString ) );
}

void CArxOptionListCtrl::OnDblclk() 
{
	GetArxServices()->HandleEvent( Prop::EventDblClicked );
}

void CArxOptionListCtrl::OnKillfocus() 
{
	GetArxServices()->HandleEvent( Prop::EventKillFocus );
}

void CArxOptionListCtrl::OnSetfocus() 
{
	GetArxServices()->HandleEvent( Prop::EventSetFocus );
}

void CArxOptionListCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	__super::OnMouseMove(nFlags, point);
	GetArxServices()->HandleEvent( Prop::EventMouseMove, args_NNN( nFlags, point.x, point.y ) );
}

void CArxOptionListCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
  if( nChar == VK_RETURN )
	{
    if( GetArxServices()->HandleEvent( Prop::EventDblClicked ) )
			return;
	}
	__super::OnKeyDown(nChar,nRepCnt,nFlags);
}
