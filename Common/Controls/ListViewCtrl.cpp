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

#define WM_SETSELECTEDSTATE (WM_USER + 45)

#ifndef LVS_EX_LABELTIP
#define LVS_EX_LABELTIP 0x00004000
#endif


/////////////////////////////////////////////////////////////////////////////
// CLVEdit

BEGIN_MESSAGE_MAP(CLVEdit, CEdit)
	ON_WM_WINDOWPOSCHANGING()
	ON_CONTROL_REFLECT(EN_SETFOCUS, OnSetfocus)
	ON_WM_ERASEBKGND()
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
, mnEditSubItem( -1 )
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
	if( bSuccess )
	{
	#ifdef _UNICODE
		BOOL bUnicode = TRUE;
	#else
		BOOL bUnicode = FALSE;
	#endif
		SendMessage( CCM_SETUNICODEFORMAT, (WPARAM)bUnicode, 0 );
	}

	if( bSuccess )
	{
		DWORD dwExStyle = GetExtendedStyle();
		dwExStyle |= LVS_EX_LABELTIP;
		if( HasSubItemImages() )
			dwExStyle |= LVS_EX_SUBITEMIMAGES;
		SetExtendedStyle( dwExStyle );
	}
	if( bSuccess && !ApplyPropertiesEnum() )
		bSuccess = false;

	return bSuccess;
}

bool CListViewCtrl::ApplyProperty( TPropertyPtr pProp )
{
	if( !__super::ApplyProperty( pProp ) )
		return false;
	bool bFailed = false;
	switch( pProp->GetID() )
	{
	case Prop::ImageList:
		{
			TImageListPtr pImageList = mpTemplate->GetImageList();
			if (pImageList && pImageList->GetImageList().GetSafeHandle())
			{
				pImageList->GetImageList().SetBkColor( RGB(255,255,255) );
				SetImageList( &pImageList->GetImageList(), TVSIL_NORMAL );
				SetImageList( &pImageList->GetImageList(), LVSIL_SMALL );
			}
			//else
			//{
			//	if( !mDefaultImageList.m_hImageList )
			//	{
			//		CRect rcIcon( 0, 0, 16, 16 );
			//		GetItemRect( 0, &rcIcon, LVIR_ICON );
			//		mDefaultImageList.Create( rcIcon.Width(), rcIcon.Height(), ILC_COLOR, 1, 1 );
			//	}
			//	mDefaultImageList.SetBkColor( ::GetSysColor( COLOR_WINDOW ) );
			//	SetImageList( &mDefaultImageList, TVSIL_NORMAL );
			//	SetImageList( &mDefaultImageList, LVSIL_SMALL );
			//}
		}
		break;
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
				TImageListPtr pImageList = mpTemplate->GetImageList();
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
				TImageListPtr pImageList = mpTemplate->GetImageList();
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
	case Prop::EditLabels:
		{
			if( pProp->GetBooleanValue() && mpTemplate->GetLongProperty( Prop::ListViewStyle ) < 4 )
				ModifyStyle( 0, LVS_EDITLABELS );
			else
				ModifyStyle( LVS_EDITLABELS, 0 );
			break;
		}
	case Prop::ListViewSort:
		{
			switch( pProp->GetLongValue() )
			{
			case 1:
				ModifyStyle( LVS_SORTASCENDING, LVS_SORTDESCENDING );
				break;
			case 2:
				ModifyStyle( LVS_SORTDESCENDING, LVS_SORTASCENDING );
				break;
			}
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
	CHeaderCtrl* pHeaderCtrl = GetHeaderCtrl();
	if( pHeaderCtrl )
	{
		int idxColumn = pHeaderCtrl->GetItemCount();
		while( --idxColumn > 0 )
		{
			sText += _T('\t');
			sText += GetItemText( idxItem, idxColumn );
		}
	}
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

bool CListViewCtrl::OnDrop( const CPoint& point, COleDataObject* pSourceData,
														DROPEFFECT dropEffect )
{
	CStringA sTextA;
	HGLOBAL hData = pSourceData->GetGlobalData( CF_TEXT );
	if( hData )
	{
		sTextA = (char*)GlobalLock( hData );
		GlobalUnlock( hData );
		GlobalFree( hData );
	}
	else if( NULL != (hData = pSourceData->GetGlobalData( CF_HDROP )) )
	{
		DROPFILES* pdf = (DROPFILES*)GlobalLock( hData );
		if( pdf )
		{
			if( pdf->fWide )
			{
				CStringW sTextW = (LPCWSTR)((BYTE*)pdf + pdf->pFiles);
				sTextA = sTextW;
			}
			else
				sTextA = (LPCSTR)((BYTE*)pdf + pdf->pFiles);
		}
		GlobalUnlock( hData );
		GlobalFree( hData );
	}
	else
		return false;

	UINT nFlags = 0;
	int idxItem = HitTest( point, &nFlags );
	if( idxItem < 0 )
	{
		idxItem = 0;
		int idxLast = GetItemCount() - 1;
		if( idxLast >= 0 )
		{
			CRect rcLast;
			GetItemRect( idxLast, &rcLast, LVIR_BOUNDS );
			if( point.y > rcLast.bottom )
				idxItem = idxLast + 1;
		}
	}
	else
	{
		CRect rcItem;
		GetItemRect( idxItem, &rcItem, LVIR_BOUNDS );
		if( point.y > ((rcItem.top + rcItem.bottom) / 2) )
			++idxItem;
	}

	int nImage = -1;
	if( mnDragSource >= 0 )
		nImage = GetItemImage( mnDragSource, 0 );

	int idxInsert = idxItem;
	CString sInsText( sTextA );
	int nIdxToken = 0;
	while( nIdxToken >= 0 )
	{
		CString sText = sInsText.Tokenize( _T("\r\n"), nIdxToken );
		if( sText.IsEmpty() && nIdxToken < 0 )
			break;
		int nIdxTab = 0;
		CString sCellText = sText.Tokenize( _T("\t"), nIdxTab );
		idxInsert = InsertItem( idxInsert, sCellText, nImage );
		SetItemState( idxInsert, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );
		int idxCol = 1;
		while( nIdxTab >= 0 )
		{
			sCellText = sText.Tokenize( _T("\t"), nIdxTab );
			if( sCellText.IsEmpty() && nIdxToken < 0 )
				break;
			SetItemText( idxInsert, idxCol++, sCellText );
		}
		++idxInsert;
	}
	return true;
}

void CListViewCtrl::SetItemImage( int nRow, int nCol, int nImage )  
{
	LV_ITEM lvi = { LVIF_IMAGE, nRow, nCol, 0, 0, NULL, -1, nImage };
	SetItem( &lvi );
}

int CListViewCtrl::GetItemImage( int nRow, int nCol )  
{
	LV_ITEM lvi = { LVIF_IMAGE, nRow, nCol, 0, 0, NULL, -1, -1 };
	GetItem( &lvi );
	return lvi.iImage;
}

void CListViewCtrl::SetItemTextImage( int nRow, int nCol, LPCTSTR pszText, int nImage )  
{
	CString sText = pszText;
	LV_ITEM lvi = { LVIF_TEXT | LVIF_IMAGE, nRow, nCol, 0, 0, sText.LockBuffer(), -1, nImage };
	SetItem( &lvi );
}

CEdit* CListViewCtrl::EditLabel( int nItem )
{
	mnEditSubItem = 0;
	return __super::EditLabel( nItem );
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
	std::vector< UINT > rnSortXForm;
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
	typedef std::pair< int, CString > TItemState;
	typedef std::pair< UINT, std::vector< TItemState > > TRowState;
	std::vector< TRowState > rGridState;
	rGridState.resize( ctRows );
	for( size_t idxRow = 0; idxRow < ctRows; ++idxRow )
	{
		TRowState& RowState = rGridState[idxRow];
		RowState.second.resize( ctColumns );
		RowState.first = GetItemState( idxRow, ~UINT(0) );
		for( size_t idxCol = 0; idxCol < ctColumns; ++idxCol )
		{
			TCHAR szText[MAX_PATH];
			LV_ITEM lvi = { LVIF_TEXT | LVIF_IMAGE, idxRow, idxCol, 0, 0, szText, MAX_PATH, -1 };
			GetItem( &lvi );
			TItemState& ItemState = RowState.second[idxCol];
			ItemState.first = lvi.iImage;
			ItemState.second = lvi.pszText;
		}
	}
	for( size_t idxRow = 0; idxRow < ctRows; ++idxRow )
	{
		size_t idxOldRow = rnSortXForm[idxRow];
		TRowState& RowState = rGridState[idxOldRow];
		for( size_t idxCol = 0; idxCol < ctColumns; ++idxCol )
		{
			const TItemState& ItemState = RowState.second[idxCol];
			SetItemTextImage( idxRow, idxCol, ItemState.second, ItemState.first );
		}
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
	typedef std::pair< int, CString > TItemState;
	typedef std::pair< UINT, std::vector< TItemState > > TRowState;
	std::vector< TRowState > rGridState;
	rGridState.resize( ctRows );
	for( size_t idxRow = 0; idxRow < ctRows; ++idxRow )
	{
		TRowState& RowState = rGridState[idxRow];
		RowState.second.resize( ctColumns );
		RowState.first = GetItemState( idxRow, ~UINT(0) );
		for( size_t idxCol = 0; idxCol < ctColumns; ++idxCol )
		{
			TCHAR szText[MAX_PATH];
			LV_ITEM lvi = { LVIF_TEXT | LVIF_IMAGE, idxRow, idxCol, 0, 0, szText, MAX_PATH, -1 };
			GetItem( &lvi );
			TItemState& ItemState = RowState.second[idxCol];
			ItemState.first = lvi.iImage;
			ItemState.second = lvi.pszText;
		}
	}
	for( size_t idxRow = 0; idxRow < ctRows; ++idxRow )
	{
		size_t idxOldRow = rnSortXForm[idxRow];
		TRowState& RowState = rGridState[idxOldRow];
		for( size_t idxCol = 0; idxCol < ctColumns; ++idxCol )
		{
			const TItemState& ItemState = RowState.second[idxCol];
			SetItemTextImage( idxRow, idxCol, ItemState.second, ItemState.first );
		}
		SetItemState( idxRow, RowState.first, ~UINT(0) );
	}
	SetRedraw( TRUE );
	OnNeedRepaint();
	return true;
}


BEGIN_MESSAGE_MAP(CListViewCtrl, CListCtrl)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_NOTIFY_REFLECT(LVN_BEGINLABELEDIT, OnBeginlabeledit)
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_LBUTTONDOWN()
	ON_NOTIFY_REFLECT(LVN_INSERTITEM, &CListViewCtrl::OnLvnInsertitem)
	ON_NOTIFY_REFLECT(LVN_DELETEITEM, &CListViewCtrl::OnLvnDeleteitem)
	ON_NOTIFY_REFLECT(LVN_DELETEALLITEMS, &CListViewCtrl::OnLvnDeleteallitems)
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
	GetWorkAreas( 0, NULL );
	CRect rcThis;
	GetClientRect( &rcThis );
	CRect rcWorkArea[1];
	rcWorkArea[0].SetRect( 0, 0, rcThis.Width(), 36000 );
	SetWorkAreas( 1, rcWorkArea );
}

void CListViewCtrl::OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;

	int nItem = pDispInfo->item.iItem;

	if( mnEditSubItem > -1 )
	{
		CRect rcItem;
		GetSubItemRect( nItem, mnEditSubItem, LVIR_LABEL, rcItem );

		HWND hWnd = (HWND)SendMessage( LVM_GETEDITCONTROL );
		ASSERT(hWnd!=NULL);
		VERIFY(mLVEdit.SubclassWindow( hWnd ));

		mLVEdit.m_x=rcItem.left;
		mLVEdit.m_y=rcItem.top-1;
		mLVEdit.m_pParent = this;
		mLVEdit.SetWindowText( GetItemText( nItem, mnEditSubItem ) );
	}
	*pResult = 0;
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
	return HandleCtlColor( pDC, nCtlColor );
}

BOOL CListViewCtrl::OnEraseBkgnd(CDC* pDC)
{
	if( HandleEraseBkgnd( pDC ) )
		return TRUE;
	return __super::OnEraseBkgnd(pDC);
}

void CListViewCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	__super::OnLButtonDown(nFlags, point);
	if( mpTemplate->GetBooleanProperty( Prop::DragnDropAllowBegin ) )
	{
		UINT nHTFlags = 0;
		mnDragSource = HitTest( point, &nHTFlags );
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
			//bool bWasSelected = (GetItemState( mnDragSource, LVIS_SELECTED ) != 0);
			SetItemState( mnDragSource, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );
			DWORD dwDropEffect = BeginDragDrop( point );
			if( dwDropEffect == DROPEFFECT_MOVE )
				DeleteItem( mnDragSource );
			//else if( (nFlags & MK_CONTROL) != 0 )
			//	PostMessage( WM_SETSELECTEDSTATE, (WPARAM)mnDragSource, (LPARAM)bWasSelected );	
			PostMessage( WM_LBUTTONUP, nFlags, MAKELPARAM(point.x, point.y) );	
			mnDragSource = -1;
		}
	}
}

LRESULT CListViewCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_SETSELECTEDSTATE )
	{
		SetItemState( (int)wParam, lParam? LVIS_SELECTED : 0, LVIS_SELECTED );
		return 0;
	}

	return __super::WindowProc(message, wParam, lParam);
}

void CListViewCtrl::OnLvnInsertitem(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	int nRow = pNMLV->iItem;
	if( nRow >= 0 && nRow <= mnDragSource )
		++mnDragSource;
	*pResult = 0;
}

void CListViewCtrl::OnLvnDeleteitem(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	int nRow = pNMLV->iItem;
	if( nRow >= 0 )
	{
		if( nRow < mnDragSource )
			--mnDragSource;
		else if( nRow == mnDragSource )
			mnDragSource = -1;
	}
	*pResult = 0;
}

void CListViewCtrl::OnLvnDeleteallitems(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	mnDragSource = -1;
	*pResult = 0;
}
