// ArxListViewCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ArxListViewCtrl.h"
#include "DclControlObject.h"
#include "PropertyObject.h"
#include "PropertyIds.h"
#include "Resource.h"
#include "InvokeMethod.h"
#include "Workspace.h"
#include "ControlPane.h"


/////////////////////////////////////////////////////////////////////////////
// CArxListViewCtrl

CArxListViewCtrl::CArxListViewCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CListViewCtrl( pTemplate, pPane, nID, false )
, mArxServices( this )
, mDragDropService( this )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CArxListViewCtrl::~CArxListViewCtrl()
{
}


BEGIN_MESSAGE_MAP(CArxListViewCtrl, CListViewCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_NOTIFY_REFLECT(NM_CLICK, OnClick)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblClick)
	ON_NOTIFY_REFLECT(NM_KILLFOCUS, OnKillfocus)
	ON_NOTIFY_REFLECT(NM_RCLICK, OnRclick)
	ON_NOTIFY_REFLECT(NM_RDBLCLK, OnRdblclk)
	ON_NOTIFY_REFLECT(NM_RETURN, OnReturn)
	ON_NOTIFY_REFLECT(NM_SETFOCUS, OnSetfocus)
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnEndlabeledit)
	ON_NOTIFY_REFLECT(LVN_BEGINLABELEDIT, OnBeginlabeledit)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnclick)
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_MOUSEMOVE()
	ON_WM_GETDLGCODE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MBUTTONDBLCLK()
	ON_WM_RBUTTONDBLCLK()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CArxListViewCtrl message handlers

void CArxListViewCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	GetArxServices()->HandleEvent( Prop::EventMouseDown, args_NNNN( 1, nFlags, point.x, point.y ) );
	__super::OnLButtonDown(nFlags, point);
}

void CArxListViewCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	__super::OnLButtonUp(nFlags, point);
	GetArxServices()->HandleEvent( Prop::EventMouseUp, args_NNNN( 1, nFlags, point.x, point.y ) );
}

void CArxListViewCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	GetArxServices()->HandleEvent( Prop::EventMouseDown, args_NNNN( 1, nFlags, point.x, point.y ) );
	__super::OnLButtonDblClk(nFlags, point);
	if( GetCapture() )
		GetArxServices()->HandleEvent( Prop::EventDblClicked, args_NN( -1, -1 ) );
}

void CArxListViewCtrl::OnMButtonDblClk(UINT nFlags, CPoint point)
{
	GetArxServices()->HandleEvent( Prop::EventMouseDown, args_NNNN( 4, nFlags, point.x, point.y ) );
	__super::OnMButtonDblClk(nFlags, point);
}

void CArxListViewCtrl::OnRButtonDblClk(UINT nFlags, CPoint point)
{
	GetArxServices()->HandleEvent( Prop::EventMouseDown, args_NNNN( 2, nFlags, point.x, point.y ) );
	__super::OnRButtonDblClk(nFlags, point);
}

void CArxListViewCtrl::OnClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LPNMLISTVIEW plvia = (LPNMLISTVIEW)pNMHDR;
	int nRow = plvia->iItem;
	int nCol = plvia->iSubItem;
	if( nRow < 0 )
	{
		LVHITTESTINFO lvhti = { plvia->ptAction.x, plvia->ptAction.y, 0, 0, 0, };
		if( SubItemHitTest( &lvhti ) >= 0 )
		{
			nRow = lvhti.iItem;
			nCol = lvhti.iSubItem;
		}
	}
	if( nRow >= 0 && nRow < GetItemCount() )
		GetArxServices()->HandleEvent( Prop::EventClicked, args_NN( nRow, nCol ) );
	*pResult = 0;
}

void CArxListViewCtrl::OnDblClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LPNMLISTVIEW plvia = (LPNMLISTVIEW)pNMHDR;
	int nRow = plvia->iItem;
	int nCol = plvia->iSubItem;
	if( nRow < 0 )
	{
		LVHITTESTINFO lvhti = { plvia->ptAction.x, plvia->ptAction.y, 0, 0, 0, };
		if( SubItemHitTest( &lvhti ) >= 0 )
		{
			nRow = lvhti.iItem;
			nCol = lvhti.iSubItem;
		}
	}
	if( nRow >= 0 && nRow < GetItemCount() )
		GetArxServices()->HandleEvent( Prop::EventDblClicked, args_NN( nRow, nCol ) );
	*pResult = 0;
}

void CArxListViewCtrl::OnKillfocus(NMHDR* pNMHDR, LRESULT* pResult) 
{
	GetArxServices()->HandleEvent( Prop::EventKillFocus );
	*pResult = 0;
}

void CArxListViewCtrl::OnRclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LPNMLISTVIEW plvia = (LPNMLISTVIEW)pNMHDR;
	int nRow = plvia->iItem;
	int nCol = plvia->iSubItem;
	if( nRow < 0 )
	{
		LVHITTESTINFO lvhti = { plvia->ptAction.x, plvia->ptAction.y, 0, 0, 0, };
		if( SubItemHitTest( &lvhti ) >= 0 )
		{
			nRow = lvhti.iItem;
			nCol = lvhti.iSubItem;
		}
	}
	if( nRow >= 0 && nRow < GetItemCount() )
		GetArxServices()->HandleEvent( Prop::EventRightClick, args_NN( nRow, nCol ) );
	*pResult = 0;
}

void CArxListViewCtrl::OnRdblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LPNMLISTVIEW plvia = (LPNMLISTVIEW)pNMHDR;
	int nRow = plvia->iItem;
	int nCol = plvia->iSubItem;
	if( nRow < 0 )
	{
		LVHITTESTINFO lvhti = { plvia->ptAction.x, plvia->ptAction.y, 0, 0, 0, };
		if( SubItemHitTest( &lvhti ) >= 0 )
		{
			nRow = lvhti.iItem;
			nCol = lvhti.iSubItem;
		}
	}
	if( nRow >= 0 && nRow < GetItemCount() )
		GetArxServices()->HandleEvent( Prop::EventRightDblClick, args_NN( nRow, nCol ) );
	*pResult = 0;
}

void CArxListViewCtrl::OnReturn(NMHDR* pNMHDR, LRESULT* pResult) 
{
	GetArxServices()->HandleEvent( Prop::EventReturnPressed );
	*pResult = 0;
}

void CArxListViewCtrl::OnSetfocus(NMHDR* pNMHDR, LRESULT* pResult) 
{
	GetArxServices()->HandleEvent( Prop::EventSetFocus );
	*pResult = 0;
}

void CArxListViewCtrl::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	LV_ITEM& lvItem = pDispInfo->item;

	if( lvItem.iItem >= 0 )
	{
		CString sLabel = lvItem.pszText? lvItem.pszText : GetItemText( lvItem.iItem, 0 );
		GetArxServices()->HandleEvent( Prop::EventEndLabelEdit, args_SN( sLabel, lvItem.iItem ) );
	}
	*pResult = TRUE;
}

void CArxListViewCtrl::OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	GetArxServices()->HandleEvent( Prop::EventBeginLabelEdit, args_N( pDispInfo->item.iItem ) );
	__super::OnBeginlabeledit( pNMHDR, pResult );
	*pResult = 0;
}

void CArxListViewCtrl::OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	GetArxServices()->HandleEvent( Prop::EventColumnClick, args_N( pNMListView->iSubItem ) );
	*pResult = 0;
}

void CArxListViewCtrl::OnMButtonDown(UINT nFlags, CPoint point) 
{
	GetArxServices()->HandleEvent( Prop::EventMouseDown, args_NNNN( 4, nFlags, point.x, point.y ) );
	__super::OnMButtonDown(nFlags, point);
}

void CArxListViewCtrl::OnMButtonUp(UINT nFlags, CPoint point) 
{
	GetArxServices()->HandleEvent( Prop::EventMouseUp, args_NNNN( 4, nFlags, point.x, point.y ) );
	__super::OnMButtonUp(nFlags, point);
}

void CArxListViewCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{
	GetArxServices()->HandleEvent( Prop::EventMouseDown, args_NNNN( 2, nFlags, point.x, point.y ) );
	__super::OnRButtonDown(nFlags, point);
}

void CArxListViewCtrl::OnRButtonUp(UINT nFlags, CPoint point) 
{
	GetArxServices()->HandleEvent( Prop::EventMouseUp, args_NNNN( 2, nFlags, point.x, point.y ) );
	__super::OnRButtonUp(nFlags, point);
}

void CArxListViewCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	GetArxServices()->HandleEvent( Prop::EventKeyDown, args_CNN( (TCHAR)nChar, nRepCnt, nFlags ) );
	__super::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CArxListViewCtrl::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	GetArxServices()->HandleEvent( Prop::EventKeyUp, args_CNN( (TCHAR)nChar, nRepCnt, nFlags ) );
	__super::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CArxListViewCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	GetArxServices()->HandleEvent( Prop::EventMouseMove, args_NNN( nFlags, point.x, point.y ) );
	__super::OnMouseMove(nFlags, point);
}

UINT CArxListViewCtrl::OnGetDlgCode()
{
	UINT nResult = __super::OnGetDlgCode();
	if( !mpTemplate->GetStringProperty( Prop::EventReturnPressed ).IsEmpty() ||
			!mpTemplate->GetStringProperty( Prop::EventKeyDown ).IsEmpty() ||
			!mpTemplate->GetStringProperty( Prop::EventKeyUp ).IsEmpty() )
		nResult |= DLGC_WANTALLKEYS;
	return nResult;
}
