// ListBoxCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ListBoxCtrl.h"
#include "DclControlObject.h"
#include "ControlPane.h"
#include "PropertyObject.h"
#include "PropertyIds.h"


/////////////////////////////////////////////////////////////////////////////
// CListBoxCtrl

CListBoxCtrl::CListBoxCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CDialogControl( pTemplate, pPane, this )
, mrcDropInsertMark( 0, 0, 0, 0 )
, mbIgnoreChange( false )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CListBoxCtrl::~CListBoxCtrl()
{
}

bool CListBoxCtrl::Create( CWnd* pParentWnd, UINT nID ) 
{
	bool bSuccess = (__super::Create( GetWndStyle(), GetWndRect(), pParentWnd, nID ) != FALSE);

	if( bSuccess && !ApplyPropertiesEnum() )
		bSuccess = false;

	return bSuccess;
}

DWORD CListBoxCtrl::GetWndStyle() const
{
	DWORD dwStyle = CDialogControl::GetWndStyle();

	dwStyle |= (/*WS_CLIPSIBLINGS | */LBS_HASSTRINGS | LBS_NOTIFY | LBS_USETABSTOPS);
	TPropertyPtr pPropNoIntegralHeight = mpTemplate->GetPropertyObject( Prop::NoIntegralHeight );
	if( !pPropNoIntegralHeight || pPropNoIntegralHeight->GetBooleanValue() )
		dwStyle |= LBS_NOINTEGRALHEIGHT;
	if( mpTemplate->GetBooleanProperty( Prop::DisableNoScroll ) )
		dwStyle |= LBS_DISABLENOSCROLL;
	if( mpTemplate->GetBooleanProperty( Prop::MultiColumn ) )
		dwStyle |= LBS_MULTICOLUMN;
	if( mpTemplate->GetBooleanProperty( Prop::Sorted ) )
		dwStyle |= LBS_SORT;
	switch( mpTemplate->GetLongProperty( Prop::SelectionStyle ) )
	{
	case 1:
		dwStyle |= LBS_EXTENDEDSEL;
		break;
	case 2:
		dwStyle |= LBS_MULTIPLESEL;
		break;
	}
	return dwStyle;
}

bool CListBoxCtrl::ApplyPropertiesEnum()
{
	bool bSuccess = __super::ApplyPropertiesEnum();

	//The automatic vertical scroll bar doesn't get initialized correctly unless the listbox window is
	//first resized small enough that the scroll bar would be needed
	CRect rc;
	GetWindowRect( &rc );
	GetParent()->ScreenToClient( &rc );
	MoveWindow( &CRect( 0, 0, 0, 0 ), FALSE );
	MoveWindow( &rc );

	return bSuccess;
}

void CListBoxCtrl::ApplyPropertiesOrder( std::vector< Prop::Id >& ridFirst, std::vector< Prop::Id >& ridLast )
{
	__super::ApplyPropertiesOrder( ridFirst, ridLast );
	ridFirst.push_back( Prop::List );
}

bool CListBoxCtrl::OnApplyProperty( TPropertyPtr pProp )
{
	if( !__super::OnApplyProperty( pProp ) )
		return false;
	bool bFailed = false;
	switch( pProp->GetID() )
	{
	case Prop::Sorted:
		{
			DWORD dwOldStyle = GetStyle();
			if( pProp->GetBooleanValue() )
				ModifyStyle( 0, LBS_SORT, 0 );
			else
				ModifyStyle( LBS_SORT, 0, 0 );
			if( dwOldStyle != GetStyle() )
				OnListChanged();
			OnNeedRepaint();
		}
		break;
	case Prop::ColumnWidth:
		{
			long lWidth = pProp->GetLongValue();
			if( lWidth <= 0 )
				lWidth = 1;
			SetColumnWidth( lWidth );
		}
		break;
	case Prop::DisableNoScroll:
		if( pProp->GetBooleanValue() )
			ModifyStyle( 0, LBS_DISABLENOSCROLL, SWP_FRAMECHANGED );
		else
			ModifyStyle( LBS_DISABLENOSCROLL, 0, SWP_FRAMECHANGED );
		break;
	case Prop::MultiColumn:
		if( pProp->GetBooleanValue() )
			ModifyStyle( 0, LBS_MULTICOLUMN, SWP_FRAMECHANGED );
		else
			ModifyStyle( LBS_MULTICOLUMN, 0, SWP_FRAMECHANGED );
		break;
	case Prop::SelectionStyle:
		switch( pProp->GetLongValue() )
		{
		case 0:
			ModifyStyle( LBS_MULTIPLESEL | LBS_EXTENDEDSEL, 0, SWP_FRAMECHANGED );
			break;
		case 1:
			ModifyStyle( LBS_MULTIPLESEL, LBS_EXTENDEDSEL, SWP_FRAMECHANGED );
			break;
		case 2:
			ModifyStyle( LBS_EXTENDEDSEL, LBS_MULTIPLESEL, SWP_FRAMECHANGED );
			break;
		}
		break;
	case Prop::List:
		mbIgnoreChange = true;
		ResetContent();
		for (size_t idx = 0; idx < pProp->size(); idx++)
		{
			int idxNewItem = AddString( pProp->GetStringItem( idx ) );
			if( idxNewItem < 0 )
				continue;
			if( IsEnumeratingProperties() )
			{
				const PropVal::TIntArray* prInt = mpTemplate->GetPropertyObject( Prop::ItemData )->GetConstIntArrayPtr();
				if( prInt )
					SetItemData( idxNewItem, (DWORD_PTR)prInt->at( idx ) );
			}
		}
		mbIgnoreChange = false;
		if( (GetStyle() & LBS_SORT) )
			OnListChanged(); //in case the list is sorted, to update the List property
		break;
	case Prop::ItemData:
		if( !IsEnumeratingProperties() )
		{
			const PropVal::TIntArray* prInt = pProp->GetConstIntArrayPtr();
			if( prInt )
			{
				for( int idx = 0; (size_t)idx < prInt->size(); ++idx )
					SetItemData( idx, (DWORD_PTR)prInt->at( idx ) );
			}
		}
		break;
	}
	return !bFailed;
}

DROPEFFECT CListBoxCtrl::OnBeginDrag( const CPoint& point, COleDataSource& SourceData )
{
	DROPEFFECT dwEffect = __super::OnBeginDrag( point, SourceData );
	CString sDragText;
	CString sDragTextPrefix = GetDragTextPrefix();
	if( IsMultiSelect() )
	{
		int ctSel = GetSelCount();
		if( ctSel <= 0 )
			return dwEffect;
		int* rnSel = new int[ctSel];
		GetSelItems( ctSel, rnSel );
		for( int idx = 0; idx < ctSel; ++idx )
		{
			CString sText;
			GetText( rnSel[idx], sText );
			if( !sDragText.IsEmpty() )
				sDragText += _T('\n');
			if( !sDragTextPrefix.IsEmpty() )
				sText = sDragTextPrefix + sText;
			sDragText += sText;
		}
		delete[] rnSel;
	}
	else
	{
		int nCurSel = GetCurSel();
		if( nCurSel < 0 )
			return dwEffect;
		GetText( nCurSel, sDragText );
		if( !sDragTextPrefix.IsEmpty() )
			sDragText = sDragTextPrefix + sDragText;
	}
	CStringA sTextA( sDragText );
	SIZE_T cchText = sTextA.GetLength() + 1;
	HGLOBAL hData = GlobalAlloc( GHND, cchText );
	if( !hData )
		return dwEffect;
	lstrcpynA( (char*)GlobalLock( hData ), sTextA, cchText );
	GlobalUnlock( hData );
	SourceData.CacheGlobalData( CF_TEXT, hData );
	return (dwEffect | DROPEFFECT_MOVE | DROPEFFECT_COPY);
}

DROPEFFECT CListBoxCtrl::OnDragEnter( const CPoint& point, COleDataObject* pSourceData,
																			DWORD dwKeyState )
{
	return __super::OnDragEnter( point, pSourceData, dwKeyState );
}

DROPEFFECT CListBoxCtrl::OnDragOver( const CPoint& point, COleDataObject* pSourceData,
																		 DWORD dwKeyState )
{
	BOOL bOutside = TRUE;
	UINT idxItem = ItemFromPoint( point, bOutside );
	CRect rcItem;
	if( idxItem < 0 || GetCount() == 0 )
	{
		GetClientRect( &rcItem );
		rcItem.bottom = rcItem.top + 2;
	}
	else
	{
		GetItemRect( idxItem, &rcItem );
		if( point.y > ((rcItem.top + rcItem.bottom) / 2) )
		{
			if( ++idxItem >= (UINT)GetCount() )
				rcItem.top = rcItem.bottom - 2;
			else
			{
				CRect rcNextItem;
				GetItemRect( idxItem, &rcNextItem );
				CRect rcClient;
				GetClientRect( &rcClient );
				if( rcNextItem.top + 2 > rcClient.bottom )
					rcItem.top = rcItem.bottom - 2;
				else
				{
					rcItem = rcNextItem;
					rcItem.bottom = rcItem.top + 2;
				}
			}
		}
		else
			rcItem.bottom = rcItem.top + 2;
	}
	if( mrcDropInsertMark != rcItem )
	{
		if( !mrcDropInsertMark.IsRectNull() )
			InvalidateRect( &mrcDropInsertMark );
		CDC* pDC = GetDC();
		mrcDropInsertMark = rcItem;
		pDC->FillSolidRect( &mrcDropInsertMark, RGB(0,0,0) );
	}
	return __super::OnDragOver( point, pSourceData, dwKeyState );
}

void CListBoxCtrl::OnDragLeave()
{
	if( !mrcDropInsertMark.IsRectNull() )
		InvalidateRect( &mrcDropInsertMark );
	mrcDropInsertMark.SetRectEmpty();
	__super::OnDragLeave();
}

bool CListBoxCtrl::OnDrop( const CPoint& point, COleDataObject* pSourceData,
													 DROPEFFECT dropEffect )
{
	OnDragLeave(); //to make sure everything gets cleaned up
	HGLOBAL hData = pSourceData->GetGlobalData( CF_TEXT );
	if( !hData )
		return false;
	CStringA sTextA = (char*)GlobalLock( hData );
	GlobalUnlock( hData );
	GlobalFree( hData );
	BOOL bOutside = TRUE;
	UINT idxItem = ItemFromPoint( point, bOutside );
	if( (short)idxItem < 0 )
		idxItem = 0;
	else
	{
		CRect rcItem;
		GetItemRect( idxItem, &rcItem );
		if( point.y > ((rcItem.top + rcItem.bottom) / 2) )
			++idxItem;
	}
	if( !setnDragSource.empty() && dropEffect == DROPEFFECT_MOVE )
	{
		for( std::set< UINT >::reverse_iterator iter = setnDragSource.rbegin();
				 iter != setnDragSource.rend();
				 ++iter )
		{
			UINT nIdx = *iter;
			DeleteString( nIdx );
			if( nIdx < idxItem )
				--idxItem;
		}
		setnDragSource.clear();
	}
	int idxInsert = idxItem;
	CString sInsText( sTextA );
	int nIdxToken = 0;
	bool bFirst = true;
	while( nIdxToken >= 0 )
	{
		CString sText = sInsText.Tokenize( _T("\r\n"), nIdxToken );
		if( sText.IsEmpty() && nIdxToken < 0 )
			break;
		idxInsert = InsertString( idxInsert, sText );
		if( bFirst )
		{
			SetCurSel( idxInsert );
			bFirst = false;
		}
		SetSel( idxInsert );
		++idxInsert;
	}
	GetParent()->SendMessage( WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), LBN_SELCHANGE), (LPARAM)m_hWnd );
	return true;
}

void CListBoxCtrl::OnListChanged()
{
	if( mbIgnoreChange )
		return;
	TPropertyPtr pItemList = mpTemplate->GetPropertyObject( Prop::List );
	TPropertyPtr pItemDataList = mpTemplate->GetPropertyObject( Prop::ItemData );
	if( !pItemList && !pItemDataList )
		return;
	if( pItemList )
		pItemList->clear();
	if( pItemDataList )
		pItemDataList->clear();
	int ctItems = GetCount();
	for( int idx = 0; idx < ctItems; ++idx )
	{
		if( pItemList )
		{
			CString sItem;
			GetText( idx, sItem );
			pItemList->AddStringItem( sItem );
		}
		if( pItemDataList )
			pItemDataList->GetIntArrayPtr()->push_back( GetItemData( idx ) );
	}
}


BEGIN_MESSAGE_MAP(CListBoxCtrl, CListBox)
	ON_WM_LBUTTONDOWN()
	ON_WM_CTLCOLOR_REFLECT()
	ON_MESSAGE(LB_ADDSTRING, &CListBoxCtrl::OnModifyContent)
	ON_MESSAGE(LB_DELETESTRING, &CListBoxCtrl::OnModifyContent)
	ON_MESSAGE(LB_DIR, &CListBoxCtrl::OnModifyContent)
	ON_MESSAGE(LB_INSERTSTRING, &CListBoxCtrl::OnModifyContent)
	ON_MESSAGE(LB_RESETCONTENT, &CListBoxCtrl::OnModifyContent)
	ON_MESSAGE(LB_ADDFILE, &CListBoxCtrl::OnModifyContent)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListBoxCtrl message handlers

void CListBoxCtrl::PostNcDestroy() 
{
	__super::PostNcDestroy();
	delete this;
}

BOOL CListBoxCtrl::PreTranslateMessage(MSG* pMsg) 
{
	GetToolTipCtrl().RelayEvent(pMsg);
	return __super::PreTranslateMessage(pMsg);
}

void CListBoxCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{	
	if( !mpTemplate->GetBooleanProperty( Prop::DragnDropAllowBegin ) )
	{
		__super::OnLButtonDown( nFlags, point );
		return;
	}

	bool bSuperMessage = true;
	BOOL bOutside = TRUE;
	UINT idxItem = ItemFromPoint( point, bOutside );
	if( idxItem == LB_ERR || bOutside || !IsMultiSelect() || GetSel( idxItem ) <= 0 )
	{
		__super::OnLButtonDown( nFlags, point );
		bSuperMessage = false;
	}
	if( IsMultiSelect() )
	{
		int ctSel = GetSelCount();
		if( ctSel > 0 )
		{
			int* rnSel = new int[ctSel];
			GetSelItems( ctSel, rnSel );
			for( int idx = 0; idx < ctSel; ++idx )
				setnDragSource.insert( rnSel[idx] );
			delete[] rnSel;
		}
	}
	else
	{
		int nCurSel = GetCurSel();
		if( nCurSel >= 0 )
			setnDragSource.insert( nCurSel );
	}
	if( setnDragSource.empty() )
		return;

	if( !bSuperMessage )
		GetParent()->SendMessage( WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), LBN_SELCHANGE), (LPARAM)m_hWnd );

	DWORD dwDropEffect = BeginDragDrop( point );
	if( bSuperMessage && dwDropEffect == DROPEFFECT_NONE )
		__super::OnLButtonDown( nFlags, point );
	if( !setnDragSource.empty() ) //if drop was on this control, mnDragSource gets reset to -1
	{
		// We need to send WM_LBUTTONUP to control or else the selection rectangle 
		// will "follow" the mouse (like when you hold the left mouse down and 
		// scroll through a regular listbox). WM_LBUTTONUP was sent to window that 
		// received the drag/drop, not the one that initiated it, so we simulate
		// an WM_LBUTTONUP to the initiating window.
		SendMessage( WM_LBUTTONUP, 0, MAKELPARAM(point.x,point.y) );	
		if( dwDropEffect == DROPEFFECT_MOVE )
		{
			for( std::set< UINT >::reverse_iterator iter = setnDragSource.rbegin();
					 iter != setnDragSource.rend();
					 ++iter )
			{
				DeleteString( *iter );
			}
			GetParent()->SendMessage( WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), LBN_SELCHANGE), (LPARAM)m_hWnd );
		}
		setnDragSource.clear();
	}
}

HBRUSH CListBoxCtrl::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	if( !IsWindowEnabled() )
		return NULL;
	return HandleCtlColor( pDC, nCtlColor );
}

BOOL CListBoxCtrl::OnEraseBkgnd(CDC* pDC)
{
	if( HandleEraseBkgnd( pDC ) )
		return TRUE;
	return __super::OnEraseBkgnd(pDC);
}

LRESULT CListBoxCtrl::OnModifyContent( WPARAM wParam, LPARAM lParam )
{
	LRESULT lResult = Default();
	OnListChanged();
	return lResult;
}
