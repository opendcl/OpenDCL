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


BEGIN_MESSAGE_MAP(CArxListViewCtrl, CListCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_NOTIFY_REFLECT(NM_CLICK, OnClick)
	ON_NOTIFY_REFLECT(NM_KILLFOCUS, OnKillfocus)
	ON_NOTIFY_REFLECT(NM_RCLICK, OnRclick)
	ON_NOTIFY_REFLECT(NM_RDBLCLK, OnRdblclk)
	ON_NOTIFY_REFLECT(NM_RETURN, OnReturn)
	ON_NOTIFY_REFLECT(NM_SETFOCUS, OnSetfocus)
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnEndlabeledit)
	ON_NOTIFY_REFLECT(LVN_BEGINLABELEDIT, OnBeginlabeledit)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnclick)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_CONTEXTMENU()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CArxListViewCtrl message handlers

void CArxListViewCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	__super::OnLButtonDown(nFlags, point);
	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventMouseDown),
		1,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());
/*
	bool bDrag = mpTemplate->GetBooleanProperty(Prop::DragnDropAllowBegin);
	if (!bDrag) //dragging not allowed
		return;

	// a -1 will be returned if not found
	if (mpTemplate->GetLongProperty(Prop::ListViewStyle) > -1)
		BeginDragnDrop(mpTemplate, point, IsAsyncEvents());
	else
	{
		CStringArray saBlockNames;
		saBlockNames.SetSize(1,1);
		POSITION pos = GetFirstSelectedItemPosition();
		if (pos)
		{
			while (pos)
				saBlockNames.Add(GetItemText(GetNextSelectedItem(pos), 0));
			//!CHANGED! 10-5-04 SRM
			//didnt allow user to drag and drop blocks from external dwgs
			//if (m_pLoadedDwg == NULL)
			BeginDragnDropToInsertBlocks(mpTemplate, point, IsAsyncEvents(), saBlockNames);
		}
	}
*/
}

void CArxListViewCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventMouseUp),
		1,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());
	__super::OnLButtonUp(nFlags, point);
}

void CArxListViewCtrl::OnClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LPNMITEMACTIVATE plvia = (LPNMITEMACTIVATE)pNMHDR;
	int nRow = plvia->iItem;
	int nCol = plvia->iSubItem;
	if( nRow < 0 )
	{
		LVHITTESTINFO lvhti = 
		{
			plvia->ptAction.x,
			plvia->ptAction.y,
			0,
			0,
			0,
		};
		if( SubItemHitTest( &lvhti ) >= 0 )
		{
			nRow = lvhti.iItem;
			nCol = lvhti.iSubItem;
		}
	}
	if( nRow >= 0 && nRow < GetItemCount() )
	{
		InvokeMethodIntInt(
			mpTemplate->GetStringProperty( Prop::EventClicked ),  
			nRow,
			nCol,
			IsAsyncEvents() );			
	}
	*pResult = 0;
}

void CArxListViewCtrl::OnKillfocus(NMHDR* pNMHDR, LRESULT* pResult) 
{
	InvokeMethod(mpTemplate->GetStringProperty(Prop::EventKillFocus), IsAsyncEvents());
	*pResult = 0;
}

void CArxListViewCtrl::OnRclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* plvdi = (LV_DISPINFO*)pNMHDR;
	int nItem = plvdi->item.iItem;
	if( nItem < GetItemCount() && nItem >= 0 )
	{
		InvokeMethodIntInt(
			mpTemplate->GetStringProperty( Prop::EventRClick ),  
			nItem,
			plvdi->item.iSubItem,
			IsAsyncEvents());			
	}
	*pResult = 0;
}

void CArxListViewCtrl::OnRdblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* plvdi = (LV_DISPINFO*)pNMHDR;
	int nItem = plvdi->item.iItem;
	if( nItem < GetItemCount() && nItem >= 0 )
	{
		InvokeMethodIntInt(
			mpTemplate->GetStringProperty( Prop::EventRDblClick ),  
			nItem,
			plvdi->item.iSubItem,
			IsAsyncEvents());			
	}
	*pResult = 0;
}

void CArxListViewCtrl::OnReturn(NMHDR* pNMHDR, LRESULT* pResult) 
{
	InvokeMethod(mpTemplate->GetStringProperty(Prop::EventReturnPressed), IsAsyncEvents());
	*pResult = 0;
}

void CArxListViewCtrl::OnSetfocus(NMHDR* pNMHDR, LRESULT* pResult) 
{
	InvokeMethod(mpTemplate->GetStringProperty(Prop::EventSetFocus), IsAsyncEvents());
	*pResult = 0;
}

void CArxListViewCtrl::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	LV_ITEM		*plvItem = &pDispInfo->item;

	if (pDispInfo->item.iItem >= 0)
	{
		InvokeMethodStringInt(
			mpTemplate->GetStringProperty(Prop::EventEndLabelEdit),
			plvItem->pszText,
			plvItem->iItem,
			IsAsyncEvents());
	}
	__super::OnEndlabeledit( pNMHDR, pResult );
	*pResult = 0;
}

void CArxListViewCtrl::OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	InvokeMethodInt(		
		mpTemplate->GetStringProperty(Prop::EventBeginLabelEdit),
		pDispInfo->item.iItem,
		IsAsyncEvents());
	*pResult = 0;
}

void CArxListViewCtrl::OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	InvokeMethodInt(mpTemplate->GetStringProperty(Prop::EventColumnClick), pNMListView->iSubItem, IsAsyncEvents());
	*pResult = 0;
}

void CArxListViewCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	__super::OnLButtonDblClk(nFlags, point);

	int nRow = -1, nCol = -1;
	POSITION pos = GetFirstSelectedItemPosition();
	if (pos == NULL)
	{
		LVHITTESTINFO lvhti;
		lvhti.pt = point;
		SubItemHitTest(&lvhti);
		nRow = lvhti.iItem;
		if (nRow > -1)
			nCol = lvhti.iSubItem;
	}
	else
	{
		nRow = GetNextSelectedItem(pos);			
		nCol = 0;
	}
	
	InvokeMethodIntInt(
		mpTemplate->GetStringProperty(Prop::EventDblClicked),  
		nRow, 
		nCol, 
		IsAsyncEvents());
}

void CArxListViewCtrl::OnMButtonDown(UINT nFlags, CPoint point) 
{
	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventMouseDown),
		4,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());
	__super::OnMButtonDown(nFlags, point);
}

void CArxListViewCtrl::OnMButtonUp(UINT nFlags, CPoint point) 
{
	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventMouseUp),
		4,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());
	__super::OnMButtonUp(nFlags, point);
}

void CArxListViewCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{
	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventMouseDown),
		2,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());
	__super::OnRButtonDown(nFlags, point);
}

void CArxListViewCtrl::OnRButtonUp(UINT nFlags, CPoint point) 
{
	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventMouseUp),
		2,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());
	__super::OnRButtonUp(nFlags, point);
}

void CArxListViewCtrl::OnContextMenu( CWnd* pTarget, CPoint point )
{
	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventMouseDown),
		2,
		MK_RBUTTON,
		point.x,
		point.y,
		IsAsyncEvents());
	__super::OnContextMenu(pTarget, point);
}

void CArxListViewCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	TCHAR sChar = nChar;
	InvokeMethodStringIntInt(mpTemplate->GetStringProperty(Prop::EventKeyDown), sChar, nRepCnt, nFlags, IsAsyncEvents());
	__super::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CArxListViewCtrl::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	TCHAR sChar = nChar;
	InvokeMethodStringIntInt(mpTemplate->GetStringProperty(Prop::EventKeyUp), sChar, nRepCnt, nFlags, IsAsyncEvents());
	__super::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CArxListViewCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	InvokeMethodIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventMouseMove),
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());
	__super::OnMouseMove(nFlags, point);
}
