#include "stdafx.h"
#include "ArxImageTreeCtrl.h"
#include "ControlPane.h"
#include "InvokeMethod.h"


/////////////////////////////////////////////////////////////////////////////
// CArxImageTreeCtrl

CArxImageTreeCtrl::CArxImageTreeCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CImageTreeCtrl( pTemplate, pPane, nID, false )
, mArxServices( this )
, mDragDropService( this )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CArxImageTreeCtrl::~CArxImageTreeCtrl()
{
}

bool CArxImageTreeCtrl::Create( CWnd* pParentWnd, UINT nID )
{
	bool bSuccess =
		__super::Create( pParentWnd, nID );

	return bSuccess;
}

bool CArxImageTreeCtrl::OnDrop( const CPoint& point, COleDataObject* pSourceData,
																DROPEFFECT dropEffect )
{
	if( mpTemplate->GetBooleanProperty( Prop::DragnDropAllowDrop ) )
	{
		UINT flags = 0;
		HTREEITEM	hItem = HitTest( point, &flags );
		CString sKey = GetItemKey( hItem );
		CString sDropControlEvent = mpTemplate->GetStringProperty( Prop::DragnDropFromControl );
		if( !sDropControlEvent.IsEmpty() && pSourceData->IsDataAvailable( CDragDropService::GetDclControlClipboardFormat() ) )
		{
			HGLOBAL hData = pSourceData->GetGlobalData( CDragDropService::GetDclControlClipboardFormat() );
			if( !hData )
				return false;
			CDclControlObject* pSourceDclControl = *(CDclControlObject**)GlobalLock( hData );
			GlobalUnlock( hData );
			GlobalFree( hData );
			if( !pSourceDclControl )
				return false;
			CString sProject = pSourceDclControl->GetOwnerProject()->GetKeyName();
			CString sForm = pSourceDclControl->GetOwnerForm()->GetKeyName();
			CString sControl;
			if( pSourceDclControl->GetType() != _CtlForm )
				sControl = pSourceDclControl->GetKeyName();
			if( !sKey.IsEmpty() )
				InvokeMethod4Strings( sDropControlEvent,
															sProject,
															sForm,
															sControl,
															sKey,
															IsAsyncEvents() );
			else
				InvokeMethod3StringsLong( sDropControlEvent,
																	sProject,
																	sForm,
																	sControl,
																	(DWORD_PTR)hItem,
																	IsAsyncEvents() );
			return true;
		}

		CString sDropAcadWndPointEvent = mpTemplate->GetStringProperty( Prop::DragnDropFromAutoCAD );
		if( !sDropAcadWndPointEvent.IsEmpty() )
		{
			if( !sKey.IsEmpty() )
				InvokeMethodString( sDropAcadWndPointEvent,
														sKey,
														IsAsyncEvents() );
			else
				InvokeMethodLong( sDropAcadWndPointEvent,
													(DWORD_PTR)hItem,
													IsAsyncEvents() );
			return true;
		}
	}
	return __super::OnDrop( point, pSourceData, dropEffect )  ;
}


BEGIN_MESSAGE_MAP(CArxImageTreeCtrl, CImageTreeCtrl)
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_NOTIFY_REFLECT(NM_CLICK, &CArxImageTreeCtrl::OnNMClick)
	ON_NOTIFY_REFLECT(NM_DBLCLK, &CArxImageTreeCtrl::OnNMDblclk)
	ON_NOTIFY_REFLECT(NM_RCLICK, &CArxImageTreeCtrl::OnNMRclick)
	ON_NOTIFY_REFLECT(NM_RDBLCLK, &CArxImageTreeCtrl::OnNMRdblclk)
	ON_NOTIFY_REFLECT(NM_RETURN, &CArxImageTreeCtrl::OnNMReturn)
	ON_NOTIFY_REFLECT(NM_KILLFOCUS, &CArxImageTreeCtrl::OnNMKillfocus)
	ON_NOTIFY_REFLECT(NM_SETFOCUS, &CArxImageTreeCtrl::OnNMSetfocus)
	ON_NOTIFY_REFLECT(TVN_DELETEITEM, &CArxImageTreeCtrl::OnTvnDeleteitem)
	ON_NOTIFY_REFLECT(TVN_BEGINLABELEDIT, &CArxImageTreeCtrl::OnTvnBeginlabeledit)
	ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT, &CArxImageTreeCtrl::OnTvnEndlabeledit)
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDING, &CArxImageTreeCtrl::OnTvnItemexpanding)
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDED, &CArxImageTreeCtrl::OnTvnItemexpanded)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, &CArxImageTreeCtrl::OnTvnSelchanged)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CArxImageTreeCtrl message handlers

void CArxImageTreeCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	InvokeMethodStringIntInt( mpTemplate->GetStringProperty( Prop::EventKeyDown ),
														CString( (TCHAR)nChar ),
														nRepCnt,
														nFlags,
														IsAsyncEvents() );
	__super::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CArxImageTreeCtrl::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	InvokeMethodStringIntInt( mpTemplate->GetStringProperty( Prop::EventKeyUp ),
														CString( (TCHAR)nChar ),
														nRepCnt,
														nFlags,
														IsAsyncEvents() );
	__super::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CArxImageTreeCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
	HTREEITEM hItem = HitTest( point, &nFlags );
	SelectItem( hItem );
	__super::OnRButtonDown(nFlags, point);
}

void CArxImageTreeCtrl::OnRButtonUp(UINT nFlags, CPoint point)
{
	InvokeMethod( mpTemplate->GetStringProperty( Prop::EventRClick ), IsAsyncEvents() );
	__super::OnRButtonUp(nFlags, point);
}

void CArxImageTreeCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	InvokeMethodIntIntInt( mpTemplate->GetStringProperty( Prop::EventMouseMove ),
												 nFlags,
												 point.x,
												 point.y,
												 IsAsyncEvents() );
	__super::OnMouseMove(nFlags, point);
}

void CArxImageTreeCtrl::OnNMClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	InvokeMethod( mpTemplate->GetStringProperty( Prop::EventClicked ), IsAsyncEvents() );
	*pResult = 0;
}

void CArxImageTreeCtrl::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
{
	InvokeMethod( mpTemplate->GetStringProperty( Prop::EventDblClicked ), IsAsyncEvents() );
	*pResult = 0;
}

void CArxImageTreeCtrl::OnNMRclick(NMHDR *pNMHDR, LRESULT *pResult)
{
	InvokeMethod( mpTemplate->GetStringProperty( Prop::EventRClick ), IsAsyncEvents() );
	*pResult = 0;
}

void CArxImageTreeCtrl::OnNMRdblclk(NMHDR *pNMHDR, LRESULT *pResult)
{
	InvokeMethod( mpTemplate->GetStringProperty( Prop::EventRDblClick ), IsAsyncEvents() );
	*pResult = 0;
}

void CArxImageTreeCtrl::OnNMReturn(NMHDR *pNMHDR, LRESULT *pResult)
{
	InvokeMethod( mpTemplate->GetStringProperty( Prop::EventReturn ), IsAsyncEvents() );
	*pResult = 0;
}

void CArxImageTreeCtrl::OnNMKillfocus(NMHDR *pNMHDR, LRESULT *pResult)
{
	InvokeMethod( mpTemplate->GetStringProperty( Prop::EventKillFocus ), IsAsyncEvents() );
	*pResult = 0;
}

void CArxImageTreeCtrl::OnNMSetfocus(NMHDR *pNMHDR, LRESULT *pResult)
{
	InvokeMethod( mpTemplate->GetStringProperty( Prop::EventSetFocus ), IsAsyncEvents() );
	*pResult = 0;
}

void CArxImageTreeCtrl::OnTvnDeleteitem(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	HTREEITEM hItem = pNMTreeView->itemOld.hItem;
	CString sKey = GetItemKey( hItem );
	if( !sKey.IsEmpty() )
		InvokeMethodStringString( mpTemplate->GetStringProperty( Prop::EventDeleteItem ),
															GetItemText( hItem ),
															sKey,
															IsAsyncEvents() );
	else if( hItem )
		InvokeMethodStringLong( mpTemplate->GetStringProperty( Prop::EventDeleteItem ),
														GetItemText( hItem ),
														(DWORD_PTR)hItem,
														IsAsyncEvents() );
	*pResult = 0;
}

void CArxImageTreeCtrl::OnTvnBeginlabeledit(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
	HTREEITEM hItem = pTVDispInfo->item.hItem;
	CString sKey = GetItemKey( hItem );
	if( !sKey.IsEmpty() )
		InvokeMethodString( mpTemplate->GetStringProperty( Prop::EventBeginLabelEdit ),
												sKey,
												IsAsyncEvents() );
	else if( hItem )
		InvokeMethodLong( mpTemplate->GetStringProperty( Prop::EventBeginLabelEdit ),
											(DWORD_PTR)hItem,
											IsAsyncEvents() );
	*pResult = 0;
}

void CArxImageTreeCtrl::OnTvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
	HTREEITEM hItem = pTVDispInfo->item.hItem;
	CString sKey = GetItemKey( hItem );
	if( !sKey.IsEmpty() )
		InvokeMethodStringString( mpTemplate->GetStringProperty( Prop::EventEndLabelEdit ),
															GetItemText( hItem ),
															sKey,
															IsAsyncEvents() );
	else if( hItem )
		InvokeMethodStringLong( mpTemplate->GetStringProperty( Prop::EventEndLabelEdit ),
														GetItemText( hItem ),
														(DWORD_PTR)hItem,
														IsAsyncEvents() );
	*pResult = 0;
}

void CArxImageTreeCtrl::OnTvnItemexpanding(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	HTREEITEM hItem = pNMTreeView->itemNew.hItem;
	CString sKey = GetItemKey( hItem );
	if( !sKey.IsEmpty() )
		InvokeMethodStringString( mpTemplate->GetStringProperty( Prop::EventItemExpanding ),
															GetItemText( hItem ),
															sKey,
															IsAsyncEvents() );
	else if( hItem )
		InvokeMethodStringLong( mpTemplate->GetStringProperty( Prop::EventItemExpanding ),
														GetItemText( hItem ),
														(DWORD_PTR)hItem,
														IsAsyncEvents() );
	*pResult = 0;
}

void CArxImageTreeCtrl::OnTvnItemexpanded(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	HTREEITEM hItem = pNMTreeView->itemNew.hItem;
	CString sKey = GetItemKey( hItem );
	if( !sKey.IsEmpty() )
		InvokeMethodStringString( mpTemplate->GetStringProperty( Prop::EventItemExpanded ),
															GetItemText( hItem ),
															sKey,
															IsAsyncEvents() );
	else if( hItem )
		InvokeMethodStringLong( mpTemplate->GetStringProperty( Prop::EventItemExpanded ),
														GetItemText( hItem ),
														(DWORD_PTR)hItem,
														IsAsyncEvents() );
	*pResult = 0;
}

void CArxImageTreeCtrl::OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	HTREEITEM hItem = pNMTreeView->itemNew.hItem;
	CString sKey = GetItemKey( hItem );
	if( !sKey.IsEmpty() )
		InvokeMethodStringString( mpTemplate->GetStringProperty( Prop::EventSelChanged ),
															GetItemText( hItem ),
															sKey,
															IsAsyncEvents() );
	else if( hItem )
		InvokeMethodStringLong( mpTemplate->GetStringProperty( Prop::EventSelChanged ),
														GetItemText( hItem ),
														(DWORD_PTR)hItem,
														IsAsyncEvents() );
	SelectDropTarget( hItem );
	OnNeedRepaint();
	*pResult = 0;
}
