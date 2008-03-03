// ListViewCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ListViewCtrl.h"
#include "DclControlObject.h"
#include "ControlPane.h"
#include "AcadColorService.h"
#include "PropertyIds.h"
#include "Resource.h"
#include "Workspace.h"

#undef SubclassWindow


/////////////////////////////////////////////////////////////////////////////
// CLVEdit

BEGIN_MESSAGE_MAP(CLVEdit, CEdit)
	ON_WM_WINDOWPOSCHANGING()
	ON_CONTROL_REFLECT(EN_SETFOCUS, OnSetfocus)
END_MESSAGE_MAP()

void CLVEdit::OnWindowPosChanging(WINDOWPOS FAR* lpwndpos) 
{
	m_rcOldPos.SetRect(lpwndpos->x, lpwndpos->y, lpwndpos->cx + lpwndpos->x, lpwndpos->cy + lpwndpos->y);
	lpwndpos->x=m_x;
	lpwndpos->y=m_y;
	CEdit::OnWindowPosChanging(lpwndpos);
	m_pParent->InvalidateRect(m_rcOldPos);
}

void CLVEdit::OnSetfocus() 
{
	CRect rcThis(m_x, m_y, m_x+ m_rcOldPos.Width(), m_y + m_rcOldPos.Height());
	MoveWindow(m_rcOldPos, TRUE);
	MoveWindow(rcThis, TRUE);
	GetParent()->InvalidateRect(m_rcOldPos);
	m_pParent->InvalidateRect(m_rcOldPos);
}


/////////////////////////////////////////////////////////////////////////////
// CListViewCtrl

CListViewCtrl::CListViewCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CDialogControl( pTemplate, pPane, this )
, mbBlockList( pTemplate->GetType() == CtlBlockList )
, mnDragSource( -1 )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CListViewCtrl::~CListViewCtrl()
{
}

bool CListViewCtrl::Create( CWnd* pParentWnd, UINT nID )
{
	bool bSuccess = (__super::Create( GetWndStyle(), GetWndRect(), pParentWnd, nID ) != FALSE);

	if( bSuccess && !IsBlockList() )
		SetExtendedStyle( GetExtendedStyle() | LVS_EX_SUBITEMIMAGES );

	if( bSuccess && !OnApplyProperty( mpTemplate->GetPropertyObject( Prop::ImageList ) ) )
		bSuccess = false;
	if( bSuccess && !ApplyPropertiesEnum() )
		bSuccess = false;

	return bSuccess;
}

bool CListViewCtrl::OnApplyProperty( TPropertyPtr pProp )
{
	if( !__super::OnApplyProperty( pProp ) )
		return false;
	bool bFailed = false;
	switch( pProp->GetID() )
	{
	case Prop::ImageList:
		{
			RefCountedPtr< CImageListObject > pImageList = mpTemplate->GetImageList();
			if (pImageList && pImageList->GetImageList().GetSafeHandle())
			{
				pImageList->GetImageList().SetBkColor( ::GetSysColor( COLOR_BTNFACE ) );
				SetImageList( &pImageList->GetImageList(), TVSIL_NORMAL );
				SetImageList( &pImageList->GetImageList(), LVSIL_SMALL );
			}
			else
			{
				if( !mDefaultImageList.m_hImageList )
				{
					CRect rcIcon( 0, 0, 16, 16 );
					GetItemRect( 0, &rcIcon, LVIR_ICON );
					mDefaultImageList.Create( rcIcon.Width(), rcIcon.Height(), ILC_COLOR, 1, 1 );
				}
				mDefaultImageList.SetBkColor( ::GetSysColor( COLOR_WINDOW ) );
				SetImageList( &mDefaultImageList, TVSIL_NORMAL );
				SetImageList( &mDefaultImageList, LVSIL_SMALL );
			}
		}
		break;
	case Prop::BlockListStyle:
		{
			assert( mbBlockList == true );
			//create a default icon for blocks without a preview image
			mBlockViewImageList.Create( 32, 32, ILC_COLOR4, 1, 1 );
			mBlockViewImageList.SetBkColor( GetBkColor() );

			HICON hIcon = (HICON)::LoadImage( theWorkspace.GetLocalResourceModule(), MAKEINTRESOURCE(IDI_LARGEBLOCK), IMAGE_ICON, 0, 0, 0 );
			mBlockViewImageList.Add( hIcon );
			DestroyIcon( hIcon );
			SetImageList( &mBlockViewImageList, LVSIL_NORMAL );
			SetImageList( &mBlockViewImageList, LVSIL_SMALL );
			break;
		}
	case Prop::ListViewStyle:
		{
			break;
		}
	case Prop::IconXSpacing:
		{
			int nImageListIconSizeX = 0;
			if( mpTemplate->GetPropertyObject( Prop::BlockListStyle ) )
				nImageListIconSizeX = 32;
			else
			{
				RefCountedPtr< CImageListObject > pImageList = mpTemplate->GetImageList();
				if( pImageList )
					nImageListIconSizeX = pImageList->GetSize().cx;
			}
			CSize sizeIconSpacing( GetControlWnd()->SendMessage( LVM_GETITEMSPACING ) );
			sizeIconSpacing.cx = pProp->GetLongValue() + nImageListIconSizeX;
			SetIconSpacing( sizeIconSpacing );
			break;
		}
	case Prop::IconYSpacing:
		{
			int nImageListIconSizeY = 0;
			if( mpTemplate->GetPropertyObject( Prop::BlockListStyle ) )
				nImageListIconSizeY = 32;
			else
			{
				RefCountedPtr< CImageListObject > pImageList = mpTemplate->GetImageList();
				if( pImageList )
					nImageListIconSizeY = pImageList->GetSize().cy;
			}
			CSize sizeIconSpacing( GetControlWnd()->SendMessage( LVM_GETITEMSPACING ) );
			sizeIconSpacing.cy = pProp->GetLongValue() + nImageListIconSizeY;
			SetIconSpacing( sizeIconSpacing );
			break;
		}
	case Prop::GridLines:
		{
			if( pProp->GetBooleanValue() )
				SetExtendedStyle( GetExtendedStyle() | LVS_EX_GRIDLINES );
			else
				SetExtendedStyle( GetExtendedStyle() & ~LVS_EX_GRIDLINES );
			break;
		}
	case Prop::FullRowSelect:
		{
			if( pProp->GetBooleanValue() )
				SetExtendedStyle( GetExtendedStyle() | LVS_EX_FULLROWSELECT );
			else
				SetExtendedStyle( GetExtendedStyle() & ~LVS_EX_FULLROWSELECT );
			break;
		}
	case Prop::MultiSelect:
		{
			if( pProp->GetBooleanValue() )
				ModifyStyle( LVS_SINGLESEL, 0 );
			else
				ModifyStyle( 0, LVS_SINGLESEL );
			break;
		}
	case Prop::ColumnCaptions:
		{
			CHeaderCtrl* pHdrCtrl = GetHeaderCtrl();
			const PropVal::TCStringArray* prsCaption = pProp->GetConstStringArrayPtr();
			size_t idx = prsCaption->size();
			while( idx > 0 )
			{
				CString sCaption = prsCaption->at(--idx);
				HDITEM hdi;
				hdi.mask = HDI_TEXT;
				hdi.pszText = sCaption.LockBuffer();
				if( !pHdrCtrl->SetItem( idx, &hdi ) )
					bFailed = true;
			}
			break;
		}
	case Prop::ColumnWidths:
		{
			CHeaderCtrl* pHdrCtrl = GetHeaderCtrl();
			const PropVal::TIntArray* prInt = pProp->GetConstIntArrayPtr();
			size_t idx = pProp->size();
			while( idx > 0 )
			{
				HDITEM hdi;
				hdi.mask = HDI_WIDTH;
				hdi.cxy = prInt->at(--idx);
				if( !pHdrCtrl->SetItem( idx, &hdi ) )
					bFailed = true;
			}
			break;
		}
	case Prop::ColumnAlignments:
		{
			CHeaderCtrl* pHdrCtrl = GetHeaderCtrl();
			const PropVal::TIntArray* prInt = pProp->GetConstIntArrayPtr();
			size_t idx = pProp->size();
			while( idx > 0 )
			{
				HDITEM hdi;
				hdi.mask = HDI_FORMAT;
				hdi.fmt = prInt->at(--idx);
				if( !pHdrCtrl->SetItem( idx, &hdi ) )
					bFailed = true;
			}
			break;
		}
	case Prop::ColumnImages:
		{
			CHeaderCtrl* pHdrCtrl = GetHeaderCtrl();
			const PropVal::TIntArray* prInt = pProp->GetConstIntArrayPtr();
			size_t idx = pProp->size();
			while( idx > 0 )
			{
				HDITEM hdi;
				hdi.mask = HDI_IMAGE;
				hdi.iImage = prInt->at(--idx);
				if( !pHdrCtrl->SetItem( idx, &hdi ) )
					bFailed = true;
			}
			break;
		}
	}
	return !bFailed;
}

DWORD CListViewCtrl::GetWndStyle() const
{
	DWORD dwStyle = __super::GetWndStyle();

	switch( mpTemplate->GetLongProperty( Prop::BlockListStyle ) ) //returns -1 if not found
	{
	case 0:
		dwStyle |= (LVS_ICON | LVS_NOCOLUMNHEADER);
		break;
	case 1:
		dwStyle |= (LVS_LIST| LVS_NOCOLUMNHEADER);
		break;
	case -1:
		{ //block list style property not found, so it is not a blockview list
			dwStyle |= LVS_SHAREIMAGELISTS; //normal listview always uses a shared image list
			LONG fListViewStyle = mpTemplate->GetLongProperty( Prop::ListViewStyle ); //returns -1 if not found
			switch( fListViewStyle )
			{
			case 0:
				dwStyle |= (LVS_ICON | LVS_NOCOLUMNHEADER);
				break;
			case 1:
				dwStyle |= (LVS_SMALLICON | LVS_NOCOLUMNHEADER);
				break;
			case 2:
				dwStyle |= (LVS_LIST | LVS_NOCOLUMNHEADER);
				break;
			default:
				dwStyle |= (LVS_REPORT);
				if( !mpTemplate->GetBooleanProperty( Prop::ColHeader ) )
					dwStyle |= (LVS_NOCOLUMNHEADER);
				break;
			}
			if( fListViewStyle < 4 && mpTemplate->GetBooleanProperty( Prop::EditLabels ) )
				dwStyle |= (LVS_EDITLABELS);			
			break;
		}
	}

	switch( mpTemplate->GetLongProperty( Prop::ListViewSort ) )
	{
	case 1:
		dwStyle |= (LVS_SORTDESCENDING);
		break;
	case 2:
		dwStyle |= (LVS_SORTASCENDING);
		break;
	}
	
	switch( mpTemplate->GetLongProperty( Prop::ListViewIconAlign ) )
	{
	case 0:
		// I know that 0 = left in the property list box in the editor, but the top must be set so
		// the list scrolls top to bottom, this would make better sense to the user
		dwStyle |= (LVS_ALIGNTOP);
		break;
	case 1:
		// I know that 1 = top in the property list box in the editor, but the top must be set so
		// the list scrolls left to right, this would make better sense to the user
		dwStyle |= (LVS_ALIGNLEFT);
		break;
	}

	if( mpTemplate->GetBooleanProperty( Prop::AutoArrange ) )
		dwStyle |= (LVS_AUTOARRANGE);	
	if( !mpTemplate->GetBooleanProperty( Prop::LabelWrap ) )
		dwStyle |= (LVS_NOLABELWRAP);
	if( mpTemplate->GetBooleanProperty( Prop::ShowSelectAlways ) )
		dwStyle |= (LVS_SHOWSELALWAYS);
	if( !mpTemplate->GetBooleanProperty( Prop::MultiSelect ) )
		dwStyle |= (LVS_SINGLESEL);

	return dwStyle;
}

bool CListViewCtrl::OnApplyForegroundColor( TPropertyPtr pProp )
{
	if( !__super::OnApplyForegroundColor( pProp ) )
		return false;
	CAcadColorService* pColorService = GetColorService();
	if( pColorService )
		SetTextColor( pColorService->GetForegroundColor() );
	return true;
}

bool CListViewCtrl::OnApplyBackgroundColor( TPropertyPtr pProp )
{
	if( !__super::OnApplyBackgroundColor( pProp ) )
		return false;
	CAcadColorService* pColorService = GetColorService();
	if( pColorService )
	{
		SetBkColor( pColorService->GetBackgroundColor() );
		SetTextBkColor( pColorService->GetBackgroundColor() );
	}
	return true;
}

DROPEFFECT CListViewCtrl::OnBeginDrag( const CPoint& point, COleDataSource& SourceData )
{
	DROPEFFECT dwEffect = __super::OnBeginDrag( point, SourceData );
	UINT nFlags = 0;
	int idxItem = HitTest( point, &nFlags );
	if( idxItem < 0 )
		return dwEffect;
	CString sText = GetItemText( idxItem, 0 );
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


void CListViewCtrl::SetItemImage( int nRow, int nCol, int nImage)  
{
	CString sText = GetItemText(nRow, nCol);
	
	LVITEM lvItem;
	lvItem.mask = LVIF_TEXT|LVIF_IMAGE;
	lvItem.iItem = nRow;
	lvItem.iSubItem = nCol;	
	lvItem.iImage = nImage;	
	TCHAR sValue [256];
	_tcscpy(sValue, sText);		
	lvItem.pszText = sValue;

	SetItem(&lvItem);
}

int CListViewCtrl::GetItemImage( int nRow, int nCol)  
{
	LV_ITEM lvitem;
				
	lvitem.mask = LVIF_IMAGE | LVIF_PARAM | LVIF_STATE;
	lvitem.iItem = nRow;
	lvitem.iSubItem = nCol;				
	lvitem.stateMask = LVIS_CUT | LVIS_DROPHILITED | 
			LVIS_FOCUSED |  LVIS_SELECTED | 
			LVIS_OVERLAYMASK | LVIS_STATEIMAGEMASK;

	GetItem( &lvitem );

	return lvitem.iImage;
}

void CListViewCtrl::SetItemTextImage( int nRow,int nCol, CString sText, int nImage)  
{
	sText = GetItemText(nRow, nCol);
	// we do not update the image item in the first column
	if (nCol == 0) return;

	LVITEM lvItem;
	if (nImage == -1)
		lvItem.mask = LVIF_TEXT|LVIF_IMAGE;
	else
		lvItem.mask = LVIF_TEXT|LVIF_IMAGE;
	lvItem.iItem = nRow;
	lvItem.iSubItem = nCol;	
	lvItem.iImage = nImage;	
	TCHAR sValue [256];
	_tcscpy(sValue, sText);		
	lvItem.pszText = sValue;

	SetItem(&lvItem);
}

bool CListViewCtrl::SortTextItems( int nCol, bool bAscending )
{
	CHeaderCtrl* pHeaderCtrl = GetHeaderCtrl();
	if( !pHeaderCtrl )
		return false;
	size_t ctColumns = (size_t)pHeaderCtrl->GetItemCount();
	if( nCol < 0 || (size_t)nCol >= ctColumns )
		return false;
	size_t ctRows = GetItemCount();
	if( ctRows <= 1 )
		return true;
	std::vector< size_t > rnSortXForm;
	rnSortXForm.resize( ctRows );
	size_t idx = ctRows;
	while( idx-- > 0 )
		rnSortXForm[idx] = idx;
	bool bAlreadySorted = true;
	bool bDoneSorting = false;
	while( !bDoneSorting )
	{
		bDoneSorting = true;
		for( size_t idxRow = 1; idxRow < ctRows; ++idxRow )
		{
			size_t idxLo = rnSortXForm[idxRow - 1];
			size_t idxHi = rnSortXForm[idxRow];
			CString sHi = GetItemText( idxHi, nCol );
			CString sLo = GetItemText( idxLo, nCol );
			bool bOrdered = true;
			if( bAscending )
			{
				if( sLo > sHi )
					bOrdered = false;
			}
			else
			{
				if( sHi > sLo )
					bOrdered = false;
			}
			if( !bOrdered )
			{
				bDoneSorting = false;
				bAlreadySorted = false;
				rnSortXForm[idxRow] = idxLo;
				rnSortXForm[idxRow - 1] = idxHi;
			}
		}
	}
	if( bAlreadySorted )
		return true; //nothing to do
	SetRedraw( FALSE );
	typedef std::pair< UINT, std::vector< CString > > TRowState;
	std::vector< TRowState > rGridState;
	rGridState.resize( ctRows );
	for( size_t idxRow = 0; idxRow < ctRows; ++idxRow )
	{
		TRowState& RowState = rGridState[idxRow];
		RowState.second.resize( ctColumns );
		RowState.first = GetItemState( idxRow, ~UINT(0) );
		for( size_t idxCol = 0; idxCol < ctColumns; ++idxCol )
			RowState.second[idxCol] = GetItemText( idxRow, idxCol );
	}
	for( size_t idxRow = 0; idxRow < ctRows; ++idxRow )
	{
		size_t idxOldRow = rnSortXForm[idxRow];
		TRowState& RowState = rGridState[idxOldRow];
		for( size_t idxCol = 0; idxCol < ctColumns; ++idxCol )
			SetItemText( idxRow, idxCol, RowState.second[idxCol] );
		SetItemState( idxRow, RowState.first, ~UINT(0) );
	}
	SetRedraw( TRUE );
	OnNeedRepaint();
	return true;
}

bool CListViewCtrl::SortNumericItems( int nCol, bool bAscending )
{
	CHeaderCtrl* pHeaderCtrl = GetHeaderCtrl();
	if( !pHeaderCtrl )
		return false;
	size_t ctColumns = pHeaderCtrl->GetItemCount();
	if( nCol < 0 || (size_t)nCol >= ctColumns )
		return false;
	size_t ctRows = GetItemCount();
	if( ctRows <= 1 )
		return true;
	std::vector< size_t > rnSortXForm;
	rnSortXForm.resize( ctRows );
	size_t idx = ctRows;
	while( idx-- > 0 )
		rnSortXForm[idx] = idx;
	bool bAlreadySorted = true;
	bool bDoneSorting = false;
	while( !bDoneSorting )
	{
		bDoneSorting = true;
		for( size_t idxRow = 1; idxRow < ctRows; ++idxRow )
		{
			size_t idxLo = rnSortXForm[idxRow - 1];
			size_t idxHi = rnSortXForm[idxRow];
			long nLo = _tstol( GetItemText( idxLo, nCol ) );
			long nHi = _tstol( GetItemText( idxHi, nCol ) );
			bool bOrdered = true;
			if( bAscending )
			{
				if( nLo > nHi )
					bOrdered = false;
			}
			else
			{
				if( nHi > nLo )
					bOrdered = false;
			}
			if( !bOrdered )
			{
				bDoneSorting = false;
				bAlreadySorted = false;
				rnSortXForm[idxRow] = idxLo;
				rnSortXForm[idxRow - 1] = idxHi;
			}
		}
	}
	if( bAlreadySorted )
		return true; //nothing to do
	SetRedraw( FALSE );
	typedef std::pair< UINT, std::vector< CString > > TRowState;
	std::vector< TRowState > rGridState;
	rGridState.resize( ctRows );
	for( size_t idxRow = 0; idxRow < ctRows; ++idxRow )
	{
		TRowState& RowState = rGridState[idxRow];
		RowState.second.resize( ctColumns );
		RowState.first = GetItemState( idxRow, ~UINT(0) );
		for( size_t idxCol = 0; idxCol < ctColumns; ++idxCol )
			RowState.second[idxCol] = GetItemText( idxRow, idxCol );
	}
	for( size_t idxRow = 0; idxRow < ctRows; ++idxRow )
	{
		size_t idxOldRow = rnSortXForm[idxRow];
		TRowState& RowState = rGridState[idxOldRow];
		for( size_t idxCol = 0; idxCol < ctColumns; ++idxCol )
			SetItemText( idxRow, idxCol, RowState.second[idxCol] );
		SetItemState( idxRow, RowState.first, ~UINT(0) );
	}
	SetRedraw( TRUE );
	OnNeedRepaint();
	return true;
}


BEGIN_MESSAGE_MAP(CListViewCtrl, CListCtrl)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnEndlabeledit)
	ON_NOTIFY_REFLECT(LVN_BEGINLABELEDIT, OnBeginlabeledit)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CListViewCtrl message handlers


void CListViewCtrl::OnDestroy() 
{
	DeleteAllItems();
	SetImageList(NULL, TVSIL_NORMAL);
	SetImageList(NULL, LVSIL_SMALL);
	__super::OnDestroy();
}

void CListViewCtrl::OnSize(UINT nType, int cx, int cy) 
{
	__super::OnSize(nType, cx, cy);
	//Arrange(LVA_DEFAULT);
	//return;
	int nPropIconStyle = mpTemplate->GetLongProperty(Prop::ListViewStyle);
	if (nPropIconStyle == 0 || nPropIconStyle == 1 || mpTemplate->GetLongProperty(Prop::BlockListStyle) > -1)
	{	
		GetWorkAreas(0, NULL);
		CRect rcThis;
		GetClientRect(&rcThis);
		CRect rcWorkArea[1];
		rcWorkArea[0].SetRect(0,0,rcThis.Width(), 36000);
		SetWorkAreas(1, rcWorkArea);
	}
}

void CListViewCtrl::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	LV_ITEM		*plvItem = &pDispInfo->item;
	if (pDispInfo->item.mask > 0)
		SetItemText(plvItem->iItem, plvItem->iSubItem, plvItem->pszText);
	*pResult = 0;
}

void CListViewCtrl::OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;

	int nItem = pDispInfo->item.iItem;

	if (m_nEditSubItem > -1)
	{
		CRect rcItem;
		GetSubItemRect(nItem,m_nEditSubItem, LVIR_LABEL, rcItem);

		HWND hWnd=(HWND)SendMessage(LVM_GETEDITCONTROL);
		ASSERT(hWnd!=NULL);
		VERIFY(m_LVEdit.SubclassWindow(hWnd));

		m_LVEdit.m_x=rcItem.left;
		m_LVEdit.m_y=rcItem.top-1;
		m_LVEdit.m_pParent = this;
		m_LVEdit.SetWindowText(GetItemText(nItem,m_nEditSubItem));
	}
	*pResult = 0;
}

void CListViewCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	__super::OnLButtonDblClk(nFlags, point);

	POSITION pos = GetFirstSelectedItemPosition();
	if (pos == NULL)
	{
		if (mpTemplate->GetLongProperty(Prop::ListViewStyle) > -1)
		{
			LVHITTESTINFO lvhti;
			lvhti.pt = point;
			SubItemHitTest(&lvhti);
			int nRow = lvhti.iItem;
			if (nRow > -1)
			{			
				SetFocus();
				UINT flag = LVIS_SELECTED | LVIS_FOCUSED;
				SetItemState(nRow, flag, flag);
			}
		}
	}
}

void CListViewCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if( GetFocus() != this ) SetFocus();
	__super::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CListViewCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if( GetFocus() != this ) SetFocus();
	__super::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CListViewCtrl::PostNcDestroy() 
{
	__super::PostNcDestroy();
	delete this;
}

BOOL CListViewCtrl::PreTranslateMessage(MSG* pMsg) 
{
	GetToolTipCtrl().RelayEvent(pMsg);
	return __super::PreTranslateMessage(pMsg);
}

HBRUSH CListViewCtrl::CtlColor(CDC* pDC, UINT nCtlColor)
{
	return mColorService.CtlColor( pDC, nCtlColor );
}

void CListViewCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	if( mpTemplate->GetBooleanProperty( Prop::DragnDropAllowBegin ) )
	{
		UINT nHTFlags = 0;
		int mnDragSource = HitTest( point, &nHTFlags );
		if( mnDragSource >= 0 )
		{
			if( (nFlags & MK_CONTROL) == 0 )
			{
				POSITION pos = GetFirstSelectedItemPosition();
				while( pos )
				{
					int idxItem = GetNextSelectedItem( pos );
					if( idxItem != mnDragSource )
						SetItemState( mnDragSource, 0, LVIS_SELECTED );
				}
			}
			SetItemState( mnDragSource, LVIS_SELECTED, LVIS_SELECTED );
			DWORD dwDropEffect = BeginDragDrop( point );
			if( dwDropEffect == DROPEFFECT_MOVE )
				DeleteItem( mnDragSource );
			PostMessage( WM_LBUTTONUP, 0, MAKELPARAM(point.x,point.y) );	
		}
	}

	__super::OnLButtonDown(nFlags, point);
}
