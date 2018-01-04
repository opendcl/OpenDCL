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
, mbCancelLabelEdit( false )
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

DROPEFFECT CArxImageTreeCtrl::OnDragOver( const CPoint& point, COleDataObject* pSourceData,
																					DWORD dwKeyState )
{
	if( !mpTemplate->GetBooleanProperty( Prop::DragnDropAllowDrop ) )
		return DROPEFFECT_NONE;
	DWORD dwEffect = __super::OnDragOver( point, pSourceData, dwKeyState );
	CString sDragOverEvent = mpTemplate->GetStringProperty( Prop::DragOverFromControl );
	if( !sDragOverEvent.IsEmpty() && pSourceData->IsDataAvailable( CDragDropService::GetDclControlClipboardFormat() ) )
	{
		HGLOBAL hData = pSourceData->GetGlobalData( CDragDropService::GetDclControlClipboardFormat() );
		if( !hData )
			return dwEffect;
		CDclControlTemplate* pSourceDclControl = *(CDclControlTemplate**)GlobalLock( hData );
		GlobalUnlock( hData );
		GlobalFree( hData );
		if( !pSourceDclControl )
			return dwEffect;
		CString sProject = pSourceDclControl->GetOwnerProject()->GetKeyName();
		CString sForm = pSourceDclControl->GetOwnerForm()->GetKeyName();
		CString sControl;
		if( pSourceDclControl->GetType() != _CtlForm )
			sControl = pSourceDclControl->GetKeyName();
		UINT flags = 0;
		HTREEITEM	hItem = HitTest( point, &flags );
		CString sKey = GetItemKey( hItem );
		resbuf* prbResult = NULL;
		bool bCancel = false;
		if( !sKey.IsEmpty() )
			bCancel = GetArxServices()->HandleEvent( sDragOverEvent, prbResult, args_SSSS( sProject, sForm, sControl, sKey ) );
		else
			bCancel = GetArxServices()->HandleEvent( sDragOverEvent, prbResult, args_SSSH( sProject, sForm, sControl, (DWORD_PTR)hItem ) );
		if( bCancel )
			dwEffect = DROPEFFECT_NONE;
		else if( prbResult && prbResult->restype == RTSHORT )
		{
			switch( prbResult->resval.rint )
			{
			case DROPEFFECT_NONE:
				dwEffect = DROPEFFECT_NONE;
				break;
			case DROPEFFECT_COPY:
				dwEffect = DROPEFFECT_COPY;
				break;
			case DROPEFFECT_MOVE:
				dwEffect = DROPEFFECT_MOVE;
				break;
			case DROPEFFECT_LINK:
				dwEffect = DROPEFFECT_LINK;
				break;
			}
		}
		acutRelRb( prbResult );
	}
	return dwEffect;
}

bool CArxImageTreeCtrl::OnDrop( const CPoint& point, COleDataObject* pSourceData, DROPEFFECT& dropEffect )
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
			CDclControlTemplate* pSourceDclControl = *(CDclControlTemplate**)GlobalLock( hData );
			GlobalUnlock( hData );
			GlobalFree( hData );
			if( !pSourceDclControl )
				return false;
			CString sProject = pSourceDclControl->GetOwnerProject()->GetKeyName();
			CString sForm = pSourceDclControl->GetOwnerForm()->GetKeyName();
			CString sControl;
			if( pSourceDclControl->GetType() != _CtlForm )
				sControl = pSourceDclControl->GetKeyName();
			bool bExtendedHandler = sDropControlEvent.GetAt( sDropControlEvent.GetLength() - 1 ) == _T('+'); //drop effect as additional argument?
			bool bCancel = false;
			resbuf* prbResult = NULL;
			if( !sKey.IsEmpty() )
			{
				if( bExtendedHandler )
					bCancel = GetArxServices()->HandleEvent( sDropControlEvent, prbResult, args_SSSSN( sProject, sForm, sControl, sKey, dropEffect ) );
				else
					bCancel = GetArxServices()->HandleEvent( sDropControlEvent, prbResult, args_SSSS( sProject, sForm, sControl, sKey ) );
			}
			else
			{
				if( bExtendedHandler )
					bCancel = GetArxServices()->HandleEvent( sDropControlEvent, prbResult, args_SSSHN( sProject, sForm, sControl, (DWORD_PTR)hItem, dropEffect ) );
				else
					bCancel = GetArxServices()->HandleEvent( sDropControlEvent, prbResult, args_SSSH( sProject, sForm, sControl, (DWORD_PTR)hItem ) );
			}
			bool bUnhandled = false;
			if( bCancel )
				dropEffect = DROPEFFECT_NONE;
			else if( prbResult )
			{
				if( prbResult->restype == RTSHORT )
				{
					switch( prbResult->resval.rint )
					{
					case -1:
						bUnhandled = true;
						break;
					case DROPEFFECT_NONE:
					case DROPEFFECT_COPY:
					case DROPEFFECT_MOVE:
					case DROPEFFECT_LINK:
						dropEffect = (DROPEFFECT)prbResult->resval.rint;
						break;
					}
				}
				acutRelRb( prbResult );
			}
			if( !bUnhandled )
				return true;
		}

		CString sDropAcadWndPointEvent = mpTemplate->GetStringProperty( Prop::DragnDropFromOther );
		if( !sDropAcadWndPointEvent.IsEmpty() )
		{
			if( !sKey.IsEmpty() )
				GetArxServices()->HandleEvent( sDropAcadWndPointEvent, args_S( sKey ) );
			else
				GetArxServices()->HandleEvent( sDropAcadWndPointEvent, args_H( (DWORD_PTR)hItem ) );
			return true;
		}
	}
	return __super::OnDrop( point, pSourceData, dropEffect );
}

BOOL CArxImageTreeCtrl::SetItemText( HTREEITEM hItem, LPCTSTR lpszItem )
{
	mbCancelLabelEdit = true;
	return __super::SetItemText( hItem, lpszItem );
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
	GetArxServices()->HandleEvent( Prop::EventKeyDown, args_CNN( (TCHAR)nChar, nRepCnt, nFlags ) );
	__super::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CArxImageTreeCtrl::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	GetArxServices()->HandleEvent( Prop::EventKeyUp, args_CNN( (TCHAR)nChar, nRepCnt, nFlags ) );
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
	GetArxServices()->HandleEvent( Prop::EventRightClick );
	__super::OnRButtonUp(nFlags, point);
}

void CArxImageTreeCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	GetArxServices()->HandleEvent( Prop::EventMouseMove, args_NNN( nFlags, ToDIP( point.x ), ToDIP( point.y ) ) );
	__super::OnMouseMove(nFlags, point);
}

void CArxImageTreeCtrl::OnNMClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	GetArxServices()->HandleEvent( Prop::EventClicked );
	*pResult = 0;
}

void CArxImageTreeCtrl::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
{
	GetArxServices()->HandleEvent( Prop::EventDblClicked );
	*pResult = 0;
}

void CArxImageTreeCtrl::OnNMRclick(NMHDR *pNMHDR, LRESULT *pResult)
{
	GetArxServices()->HandleEvent( Prop::EventRightClick );
	*pResult = 0;
}

void CArxImageTreeCtrl::OnNMRdblclk(NMHDR *pNMHDR, LRESULT *pResult)
{
	GetArxServices()->HandleEvent( Prop::EventRightDblClick );
	*pResult = 0;
}

void CArxImageTreeCtrl::OnNMReturn(NMHDR *pNMHDR, LRESULT *pResult)
{
	GetArxServices()->HandleEvent( Prop::EventReturnPressed );
	*pResult = 0;
}

void CArxImageTreeCtrl::OnNMKillfocus(NMHDR *pNMHDR, LRESULT *pResult)
{
	GetArxServices()->HandleEvent( Prop::EventKillFocus );
	*pResult = 0;
}

void CArxImageTreeCtrl::OnNMSetfocus(NMHDR *pNMHDR, LRESULT *pResult)
{
	GetArxServices()->HandleEvent( Prop::EventSetFocus );
	*pResult = 0;
}

void CArxImageTreeCtrl::OnTvnDeleteitem(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	HTREEITEM hItem = pNMTreeView->itemOld.hItem;
	CString sKey = GetItemKey( hItem );
	if( !sKey.IsEmpty() )
		GetArxServices()->HandleEvent( Prop::EventDeleteItem, args_SS( GetItemText( hItem ), sKey ) );
	else if( hItem )
		GetArxServices()->HandleEvent( Prop::EventDeleteItem, args_SH( GetItemText( hItem ), (DWORD_PTR)hItem ) );
	__super::OnTvnDeleteitem( pNMHDR, pResult );
	*pResult = 0;
}

void CArxImageTreeCtrl::OnTvnBeginlabeledit(NMHDR *pNMHDR, LRESULT *pResult)
{
	//__super::OnTvnBeginlabeledit( pNMHDR, pResult );
	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
	HTREEITEM hItem = pTVDispInfo->item.hItem;
	CString sKey = GetItemKey( hItem );
	if( !sKey.IsEmpty() )
		GetArxServices()->HandleEvent( Prop::EventBeginLabelEdit, args_S( sKey ) );
	else if( hItem )
		GetArxServices()->HandleEvent( Prop::EventBeginLabelEdit, args_H( (DWORD_PTR)hItem ) );
	*pResult = 0;
}

void CArxImageTreeCtrl::OnTvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult)
{
	//__super::OnTvnEndlabeledit( pNMHDR, pResult );
	mbCancelLabelEdit = false;
	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
	HTREEITEM hItem = pTVDispInfo->item.hItem;
	CString sLabel =
		pTVDispInfo->item.pszText? pTVDispInfo->item.pszText : GetItemText( hItem );
	CString sKey = GetItemKey( hItem );
	if( !sKey.IsEmpty() )
		GetArxServices()->HandleEvent( Prop::EventEndLabelEdit, args_SS( sLabel, sKey ) );
	else if( hItem )
		GetArxServices()->HandleEvent( Prop::EventEndLabelEdit, args_SH( sLabel, (DWORD_PTR)hItem ) );
	if( mbCancelLabelEdit )
	{
		*pResult = FALSE;
		mbCancelLabelEdit = false;
	}
	else
		*pResult = TRUE;
}

void CArxImageTreeCtrl::OnTvnItemexpanding(NMHDR *pNMHDR, LRESULT *pResult)
{
	//__super::OnTvnItemexpanding( pNMHDR, pResult );
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	HTREEITEM hItem = pNMTreeView->itemNew.hItem;
	CString sKey = GetItemKey( hItem );
	if( !sKey.IsEmpty() )
		GetArxServices()->HandleEvent( Prop::EventItemExpanding, args_SS( GetItemText( hItem ), sKey ) );
	else if( hItem )
		GetArxServices()->HandleEvent( Prop::EventItemExpanding, args_SH( GetItemText( hItem ), (DWORD_PTR)hItem ) );
	*pResult = 0;
}

void CArxImageTreeCtrl::OnTvnItemexpanded(NMHDR *pNMHDR, LRESULT *pResult)
{
	//__super::OnTvnItemexpanded( pNMHDR, pResult );
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	HTREEITEM hItem = pNMTreeView->itemNew.hItem;
	CString sKey = GetItemKey( hItem );
	if( !sKey.IsEmpty() )
		GetArxServices()->HandleEvent( Prop::EventItemExpanded, args_SS( GetItemText( hItem ), sKey ) );
	else if( hItem )
		GetArxServices()->HandleEvent( Prop::EventItemExpanded, args_SH( GetItemText( hItem ), (DWORD_PTR)hItem ) );
	*pResult = 0;
}

void CArxImageTreeCtrl::OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	//__super::OnTvnSelchanged( pNMHDR, pResult );
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	HTREEITEM hItem = pNMTreeView->itemNew.hItem;
	CString sKey = GetItemKey( hItem );
	if( !sKey.IsEmpty() )
		GetArxServices()->HandleEvent( Prop::EventSelChanged, args_SS( GetItemText( hItem ), sKey ) );
	else if( hItem )
		GetArxServices()->HandleEvent( Prop::EventSelChanged, args_SH( GetItemText( hItem ), (DWORD_PTR)hItem ) );
	//SelectDropTarget( hItem ); //[ORW 2010-08-23] this causes the selection highlight to display always, thereby masking the ShowSelectAlways property]
	OnNeedRepaint();
	*pResult = 0;
}
