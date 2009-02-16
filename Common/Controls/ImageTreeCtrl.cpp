// ImageTreeCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ImageTreeCtrl.h"
#include "ControlPane.h"


static UINT GetTreeItemClipboardFormat()
{
	static const UINT CF_TreeItem = RegisterClipboardFormat( _T("OpenDCL.TreeItem") );
	return CF_TreeItem;
}

/////////////////////////////////////////////////////////////////////////////
// CImageTreeCtrl

CImageTreeCtrl::CImageTreeCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CDialogControl( pTemplate, pPane, this )
, mbDeleting( false )
, mhtiDragSource( NULL )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CImageTreeCtrl::~CImageTreeCtrl()
{
}

bool CImageTreeCtrl::Create( CWnd* pParentWnd, UINT nID ) 
{
	bool bSuccess = (__super::Create( GetWndStyle(), GetWndRect(), pParentWnd, nID ) != FALSE);
	if( bSuccess )
	{
	#ifdef _UNICODE
		BOOL bUnicode = TRUE;
	#else
		BOOL bUnicode = FALSE;
	#endif
		SendMessage( CCM_SETUNICODEFORMAT, (WPARAM)bUnicode, 0 );
	}

	if( bSuccess && !ApplyPropertiesEnum() )
		bSuccess = false;

	return bSuccess;
}

DWORD CImageTreeCtrl::GetWndStyle() const
{
	DWORD dwStyle = CDialogControl::GetWndStyle();

	dwStyle |= (WS_VSCROLL | WS_HSCROLL);

	if( mpTemplate->GetBooleanProperty( Prop::ShowSelectAlways ) )
		dwStyle |= TVS_SHOWSELALWAYS;
	if( mpTemplate->GetBooleanProperty( Prop::HasLines ) )
		dwStyle |= TVS_HASLINES;
	if( mpTemplate->GetBooleanProperty( Prop::LinesAtRoot ) )
		dwStyle |= TVS_LINESATROOT;
	if( mpTemplate->GetBooleanProperty( Prop::HasButtons ) )
		dwStyle |= TVS_HASBUTTONS;
	if( mpTemplate->GetBooleanProperty( Prop::EditLabels ) )
		dwStyle |= TVS_EDITLABELS;
	if( mpTemplate->GetBooleanProperty( Prop::SingleClickExpand ) )
		dwStyle |= TVS_SINGLEEXPAND;

	return dwStyle;
}

bool CImageTreeCtrl::OnApplyProperty( TPropertyPtr pProp )
{
	if( !__super::OnApplyProperty( pProp ) )
		return false;
	bool bFailed = false;
	switch( pProp->GetID() )
	{
	case Prop::ImageList:
		{
			TImageListPtr pImageList = mpTemplate->GetImageList();
			if (pImageList && pImageList->GetImageList().GetSafeHandle())
			{
				CImageList& ImageList = pImageList->GetImageList();
				CAcadColorService* pColorService = GetColorService();
				ImageList.SetBkColor( pColorService? pColorService->GetBackgroundColor() : CLR_NONE );
				SetImageList( &ImageList, TVSIL_NORMAL );
			}
			else
				SetImageList( NULL, TVSIL_NORMAL );
		}
		break;
	case Prop::ShowSelectAlways:
		if( pProp->GetBooleanValue() )
			ModifyStyle( 0, TVS_SHOWSELALWAYS );
		else
			ModifyStyle( TVS_SHOWSELALWAYS, 0 );
		break;
	case Prop::Indent:
		SetIndent( pProp->GetLongValue() );
		break;
	case Prop::HasLines:
		if( pProp->GetBooleanValue() )
			ModifyStyle( 0, TVS_HASLINES );
		else
			ModifyStyle( TVS_HASLINES, 0 );
		break;
	case Prop::LinesAtRoot:
		if( pProp->GetBooleanValue() )
			ModifyStyle( 0, TVS_LINESATROOT );
		else
			ModifyStyle( TVS_LINESATROOT, 0 );
		break;
	case Prop::HasButtons:
		if( pProp->GetBooleanValue() )
			ModifyStyle( 0, TVS_HASBUTTONS );
		else
			ModifyStyle( TVS_HASBUTTONS, 0 );
		break;
	case Prop::EditLabels:
		if( pProp->GetBooleanValue() )
			ModifyStyle( 0, TVS_EDITLABELS );
		else
			ModifyStyle( TVS_EDITLABELS, 0 );
		break;
	case Prop::SingleClickExpand:
		if( pProp->GetBooleanValue() )
			ModifyStyle( 0, TVS_SINGLEEXPAND );
		else
			ModifyStyle( TVS_SINGLEEXPAND, 0 );
		break;
	}
	return !bFailed;
}

DROPEFFECT CImageTreeCtrl::OnBeginDrag( const CPoint& point, COleDataSource& SourceData )
{
	DROPEFFECT dwEffect = __super::OnBeginDrag( point, SourceData );
	UINT nFlags = 0;
	HTREEITEM hItem = HitTest( point, &nFlags );
	if( !hItem )
		return dwEffect;
	CString sText = GetItemText( hItem );
	if( sText.IsEmpty() )
		return dwEffect;
	CStringA sTextA( sText );
	SIZE_T cchText = sTextA.GetLength() + 1;
	HGLOBAL hData = GlobalAlloc( GHND, cchText );
	if( !hData )
		return dwEffect;
	lstrcpynA( (char*)GlobalLock( hData ), sTextA, cchText );
	GlobalUnlock( hData );
	SourceData.CacheGlobalData( CF_TEXT, hData );
	return (dwEffect | DROPEFFECT_MOVE | DROPEFFECT_COPY);
}

DROPEFFECT CImageTreeCtrl::OnDragOver( const CPoint& point, COleDataObject* pSourceData,
																			 DWORD dwKeyState )
{
	UINT flags = 0;
	HTREEITEM	hItem = HitTest( point, &flags );
	if( hItem && (!mhtiDragSource || mhtiDragSource != hItem || (dwKeyState & MK_CONTROL)) )
	{
		SelectDropTarget( hItem );
		return __super::OnDragOver( point, pSourceData, dwKeyState );
	}
	return DROPEFFECT_NONE;
}

bool CImageTreeCtrl::OnDrop( const CPoint& point, COleDataObject* pSourceData,
														 DROPEFFECT dropEffect )
{
	HGLOBAL hData = pSourceData->GetGlobalData( CF_TEXT );
	if( !hData )
		return __super::OnDrop( point, pSourceData, dropEffect )  ;
	UINT flags = 0;
	HTREEITEM	hItem = HitTest( point, &flags );
	if( !hItem )
		hItem = TVI_ROOT;
	CStringA sTextA = (char*)GlobalLock( hData );
	GlobalUnlock( hData );
	GlobalFree( hData );
	HTREEITEM hNewItem = InsertItem( CString( sTextA ), hItem, TVI_FIRST );
	CTreeItem TreeItem( hNewItem );
	const CTreeItem* pOriginalTreeItem = GetTreeItem( mhtiDragSource );
	if( pOriginalTreeItem )
	{
		TreeItem.mnImage = pOriginalTreeItem->mnImage;
		TreeItem.mnSelImage = pOriginalTreeItem->mnSelImage;
		TreeItem.mnExpImage = pOriginalTreeItem->mnExpImage;
		SetItemImage( hNewItem, TreeItem.mnImage, TreeItem.mnSelImage );
		if( mhtiDragSource && dropEffect == DROPEFFECT_MOVE )
			TreeItem.msKey = pOriginalTreeItem->msKey;
	}
	mTreeItems.push_back( TreeItem );
	SelectItem( hNewItem );
	return true;
}

CString CImageTreeCtrl::GetItemKey( HTREEITEM hItem ) const
{
	if( !hItem )
		return _T("");
	for( std::list< CTreeItem >::const_iterator iter = mTreeItems.begin();
			 iter != mTreeItems.end();
			 ++iter )
	{
		if( iter->mhItem == hItem )
			return iter->msKey;
	}
	return _T("");
}

HTREEITEM CImageTreeCtrl::FindItem( LPCTSTR pszKey ) const
{
	for( std::list< CTreeItem >::const_iterator iter = mTreeItems.begin();
			 iter != mTreeItems.end();
			 ++iter )
	{
		if( iter->msKey.CompareNoCase( pszKey ) == 0 )
			return iter->mhItem;
	}
	return NULL;
}

HTREEITEM CImageTreeCtrl::AddParent( LPCTSTR pszCaption, LPCTSTR pszKey /*= NULL*/,
																		 int nImage /*= -1*/, int nSelImage /*= -1*/,
																		 int nExpImage /*= -1*/ )
{
	if( pszKey && *pszKey && FindItem( pszKey ) )
		return NULL; //duplicate key!
	HTREEITEM hItem = InsertItem( pszCaption, I_IMAGECALLBACK, I_IMAGECALLBACK );
	if( !hItem )
		return NULL;
	mTreeItems.push_back( CTreeItem( hItem, pszKey, nImage, nSelImage, nExpImage ) );
	return hItem;
}

HTREEITEM CImageTreeCtrl::AddChild( HTREEITEM hParent, LPCTSTR pszCaption, LPCTSTR pszKey /*= NULL*/,
																		int nImage /*= -1*/, int nSelImage /*= -1*/,
																		int nExpImage /*= -1*/, HTREEITEM hAddAfter /*= TVI_LAST*/ )
{
	if( pszKey && *pszKey && FindItem( pszKey ) )
		return NULL; //duplicate key!
	if( hAddAfter && !hParent )
		hParent = GetParentItem( hAddAfter );
	HTREEITEM hItem = InsertItem( pszCaption, I_IMAGECALLBACK, I_IMAGECALLBACK, hParent, hAddAfter );
	if( !hItem )
		return NULL;
	mTreeItems.push_back( CTreeItem( hItem, pszKey, nImage, nSelImage, nExpImage ) );
	return hItem;
}

BOOL CImageTreeCtrl::GetItemImage( HTREEITEM hItem, int& nImage, int& nSelImage ) const
{
	const CTreeItem* pItem = GetTreeItem( hItem );
	if( !pItem )
		return FALSE;
	nImage = pItem->mnImage;
	nSelImage = pItem->mnSelImage;
	return TRUE;
}

BOOL CImageTreeCtrl::SetItemImage( HTREEITEM hItem, int nImage, int nSelImage )
{
	CTreeItem* pItem = GetTreeItem( hItem );
	if( !pItem )
		return FALSE;
	pItem->mnImage = nImage;
	pItem->mnSelImage = nSelImage;
	return __super::SetItemImage( hItem, I_IMAGECALLBACK, I_IMAGECALLBACK ); //force it to redraw
}

void CImageTreeCtrl::Clear()
{
	mTreeItems.clear();
	DeleteAllItems();
}

int CImageTreeCtrl::GetExpandedImage( HTREEITEM hItem ) const
{
	const CTreeItem* pItem = GetTreeItem( hItem );
	if( !pItem )
		return -1;
	return pItem->mnExpImage;
}

bool CImageTreeCtrl::SetExpandedImage( HTREEITEM hItem, int nExpImage )
{
	CTreeItem* pItem = GetTreeItem( hItem );
	if( !pItem )
		return false;
	pItem->mnExpImage = nExpImage;
	__super::SetItemImage( hItem, I_IMAGECALLBACK, I_IMAGECALLBACK ); //force it to redraw
	return true;
}

CTreeItem* CImageTreeCtrl::GetTreeItem( HTREEITEM hItem )
{
	for( std::list< CTreeItem >::iterator iter = mTreeItems.begin();
			 iter != mTreeItems.end();
			 ++iter )
	{
		if( iter->mhItem == hItem )
			return &(*iter);
	}
	return NULL;
}

const CTreeItem* CImageTreeCtrl::GetTreeItem( HTREEITEM hItem ) const
{
	for( std::list< CTreeItem >::const_iterator iter = mTreeItems.begin();
			 iter != mTreeItems.end();
			 ++iter )
	{
		if( iter->mhItem == hItem )
			return &(*iter);
	}
	return NULL;
}


BEGIN_MESSAGE_MAP(CImageTreeCtrl, CTreeCtrl)
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_NOTIFY_REFLECT(TVN_DELETEITEM, &CImageTreeCtrl::OnTvnDeleteitem)
	ON_NOTIFY_REFLECT(TVN_GETDISPINFO, &CImageTreeCtrl::OnTvnGetdispinfo)
	ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CImageTreeCtrl message handlers

BOOL CImageTreeCtrl::PreTranslateMessage(MSG* pMsg) 
{
	GetToolTipCtrl().RelayEvent(pMsg);
	return __super::PreTranslateMessage(pMsg);
}

void CImageTreeCtrl::PostNcDestroy() 
{
	__super::PostNcDestroy();
	delete this;
}

void CImageTreeCtrl::OnDestroy()
{
	SetImageList( NULL, TVSIL_NORMAL );
	__super::OnDestroy();
}

void CImageTreeCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	__super::OnLButtonDown(nFlags, point);
	HTREEITEM hItem = HitTest( point, &nFlags );
	if( hItem )
	{
		//Select( hItem, TVGN_CARET );
		mhtiDragSource = hItem;
	}
	DROPEFFECT dwDropEffect = BeginDragDrop( point );
	//if( dwDropEffect != DROPEFFECT_NONE )
	//	SendMessage( WM_LBUTTONUP, 0, MAKELPARAM(point.x,point.y) );	
	if( mhtiDragSource && dwDropEffect == DROPEFFECT_MOVE )
		DeleteItem( mhtiDragSource );
	mhtiDragSource = NULL;
}

void CImageTreeCtrl::OnTvnDeleteitem(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	HTREEITEM hItem = pNMTreeView->itemOld.hItem;
	for( std::list< CTreeItem >::iterator iter = mTreeItems.begin();
			 iter != mTreeItems.end();
			 ++iter )
	{
		if( iter->mhItem == hItem )
		{
			mTreeItems.erase( iter );
			break;
		}
	}
	*pResult = 0;
}

void CImageTreeCtrl::OnTvnGetdispinfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
	HTREEITEM hItem = pTVDispInfo->item.hItem;
	const CTreeItem* pItem = GetTreeItem( hItem );
	if( pItem )
	{
		UINT nState = pTVDispInfo->item.state;
		int nImage = ((nState & TVIS_EXPANDED) != 0)? pItem->mnExpImage : pItem->mnImage;
		pTVDispInfo->item.iImage = nImage;
		pTVDispInfo->item.iSelectedImage = (pItem->mnSelImage < 0)? nImage : pItem->mnSelImage;
		*pResult = 1;
	}
}

HBRUSH CImageTreeCtrl::CtlColor(CDC* pDC, UINT nCtlColor)
{
	if( !IsWindowEnabled() )
		return NULL;
	return NULL;
	//return mColorService.CtlColor( pDC, nCtlColor );
}
