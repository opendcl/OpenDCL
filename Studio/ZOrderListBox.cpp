// CZOrderListBox.cpp : implementation file
//

#include "stdafx.h"
#include "ZOrderListBox.h"
#include "StudioDialogObject.h"
#include "DclControlObject.h"
#include "DclFormObject.h"
#include "DclFormView.h"
#include "StudioWorkspace.h"
#include "ControlManager.h"
#include "ControlTypeIcons.h"
#include "Resource.h"


static bool IsControlSelected( TDclControlPtr pDclControl )
{
	assert( pDclControl != NULL );
	const CDialogControl* pDlgControl = pDclControl->GetControlInstance();
	if( !pDlgControl )
		return false;
	CControlManager* pManager = pDlgControl->GetControlManager();
	if( !pManager )
		return false;
	return pManager->IsSelected();
}

static void SetControlSelected( TDclControlPtr pDclControl, bool bSelected = true )
{
	assert( pDclControl != NULL );
	const CDialogControl* pDlgControl = pDclControl->GetControlInstance();
	if( !pDlgControl )
		return;
	CControlManager* pManager = pDlgControl->GetControlManager();
	if( !pManager )
		return;
	if( bSelected == pManager->IsSelected() )
		return; //no-op
	pManager->SetSelected( bSelected );
	if( bSelected )
		theStudioWorkspace.ActivateDclControl( pDclControl );
}

static void ToggleControlSelected( TDclControlPtr pDclControl )
{
	assert( pDclControl != NULL );
	const CDialogControl* pDlgControl = pDclControl->GetControlInstance();
	if( !pDlgControl )
		return;
	CControlManager* pManager = pDlgControl->GetControlManager();
	if( !pManager )
		return;
	pManager->SetSelected( !pManager->IsSelected() );
}


/////////////////////////////////////////////////////////////////////////////
// CZOrderListBox

CZOrderListBox::CZOrderListBox()
: mpDlgObject( NULL )
, mbNotifying( false )
, mnItemUnselectPending( -1 )
, mbDragging( false )
, mptDragStart( -1, -1 )
, midxInsertAt( -1 )
{
	mImageList.Create( 16, 16, ILC_COLOR4 | ILC_MASK, _CtlMax - _CtlFirst + 1, 1 );
	for( ControlType nCtrlType = _CtlFirst; nCtrlType <= _CtlMax; ++(int&)nCtrlType )
		mImageList.Add( GetControlTypeIcon( nCtrlType ) );
	if( !mFont.CreatePointFont( 80, theStudioWorkspace.GetDefaultFontName() ) )
		mFont.CreateStockObject( DEFAULT_GUI_FONT );
}

CZOrderListBox::~CZOrderListBox()
{
}

BOOL CZOrderListBox::Create( DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID )
{
	dwStyle &= ~(LBS_SORT | LBS_HASSTRINGS);
	dwStyle |= (WS_VSCROLL | LBS_MULTIPLESEL | LBS_OWNERDRAWFIXED | LBS_NOTIFY);
	if( !__super::Create( dwStyle, rect, pParentWnd, nID ) )
		return FALSE;
	SetFont( &mFont );
	return TRUE;
}

void CZOrderListBox::OnActivateDlgObject( CStudioDialogObject* pDlgObject )
{
	mpDlgObject = pDlgObject;
	mControls.clear();
	if( !m_hWnd )
		return;
	ResetContent();
	if( !pDlgObject )
		return;
	const TDclControlList& Controls = pDlgObject->GetSourceForm()->GetControlList();
	TDclControlList::const_reverse_iterator iter = Controls.rbegin();
	while( iter != Controls.rend() )
	{
		TDclControlPtr pDclControl = *(iter++);
		if( !pDclControl->IsZOrderAllowed() )
			continue;
		mControls.push_back( pDclControl );
		int idxItem = AddString( pDclControl->GetKeyName() );
		if( IsControlSelected( pDclControl ) )
			SetSel( idxItem, TRUE );
	}
}

void CZOrderListBox::OnActivateDclControl( TDclControlPtr pDclControl )
{
	if( pDclControl )
	{
		assert( !mpDlgObject || pDclControl->GetOwnerForm() == mpDlgObject->GetSourceForm() );
		if( !pDclControl->IsZOrderAllowed() )
			return;
		int idxItem = 0;
		for( TDclControlList::const_iterator iter = mControls.begin();
				 iter != mControls.end();
				 ++iter, ++idxItem )
		{
			if( (*iter) == pDclControl )
			{
				SetSel( idxItem, TRUE );
				SetCaretIndex( idxItem );
				CRect rcItem;
				GetItemRect( idxItem, &rcItem );
				InvalidateRect( &rcItem );
				return;
			}
		}
		OnActivateDlgObject( (CStudioDialogObject*)pDclControl->GetOwnerForm()->GetFormInstance() );
	}
	else if( !mbNotifying )
		SelItemRange( FALSE, 0, GetCount() - 1 );
	Invalidate();
}

void CZOrderListBox::OnZOrderChanged()
{
	mbNotifying = true;
	TDclFormPtr pSourceForm = mpDlgObject->GetSourceForm();
	AutoUndoGroup UndoGroup( pSourceForm->GetUndoManager(), IDS_UNDO_ZORDER );
	int idxItem = 0;
	TDclControlList::iterator iter = mControls.begin();
	while( iter != mControls.end() )
	{
		TDclControlPtr pDclControl = (*iter++);
		SetSel( idxItem++, IsControlSelected( pDclControl ) );
		pSourceForm->ReorderControl( pDclControl, true );
	}
	mpDlgObject->OnUpdateZOrder();
	mbNotifying = false;
}

void CZOrderListBox::OnMovezToIndex( int idxInsertAt ) 
{
	if( mControls.empty() )
		return;
	TDclControlList Selection;
	TDclControlList::iterator iter = mControls.begin();
	int idxIter = 0;
	while( iter != mControls.end() )
	{
		TDclControlList::iterator iterCurrent = iter;
		TDclControlPtr pDclControl = (*iter++);
		if( IsControlSelected( pDclControl ) )
		{
			Selection.push_front( pDclControl );
			mControls.erase( iterCurrent );
			if( idxIter < idxInsertAt )
				--idxInsertAt;
		}
		else
			++idxIter;
	}
	TDclControlList::iterator iterInsert = mControls.begin();
	while( idxInsertAt-- > 0 )
		++iterInsert;
	while( !Selection.empty() )
	{
		mControls.insert( iterInsert, Selection.back() );
		Selection.pop_back();
	}
	OnZOrderChanged();
	Invalidate();
}


BEGIN_MESSAGE_MAP(CZOrderListBox, CListBox)
	ON_WM_DRAWITEM_REFLECT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_CAPTURECHANGED()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_SENDTOFRONT, OnSendtofront)
	ON_COMMAND(ID_SENDTOBACK, OnSendtoback)
	ON_COMMAND(ID_MOVEZBACKBY1, OnMovezbackby1)
	ON_COMMAND(ID_MOVEZFRONTBY1, OnMovezfrontby1)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CZOrderListBox message handlers


void CZOrderListBox::OnSendtofront() 
{
	TDclControlList Selection;
	TDclControlList::iterator iter = mControls.begin();
	while( iter != mControls.end() )
	{
		TDclControlList::iterator iterCurrent = iter;
		TDclControlPtr pDclControl = (*iter++);
		if( IsControlSelected( pDclControl ) )
		{
			Selection.push_back( pDclControl );
			mControls.erase( iterCurrent );
		}
	}
	while( !Selection.empty() )
	{
		mControls.push_front( Selection.back() );
		Selection.pop_back();
	}
	OnZOrderChanged();
	Invalidate();
}

void CZOrderListBox::OnSendtoback() 
{
	TDclControlList Selection;
	TDclControlList::iterator iter = mControls.begin();
	while( iter != mControls.end() )
	{
		TDclControlList::iterator iterCurrent = iter;
		TDclControlPtr pDclControl = (*iter++);
		if( IsControlSelected( pDclControl ) )
		{
			Selection.push_front( pDclControl );
			mControls.erase( iterCurrent );
		}
	}
	while( !Selection.empty() )
	{
		mControls.push_back( Selection.back() );
		Selection.pop_back();
	}
	OnZOrderChanged();
	Invalidate();
}

void CZOrderListBox::OnMovezbackby1() 
{
	if( mControls.empty() )
		return;
	int idxItem = mControls.size();
	bool bFoundHole = false;
	TDclControlList::reverse_iterator iter = mControls.rbegin();
	while( iter != mControls.rend() )
	{
		--idxItem;
		TDclControlList::reverse_iterator iterCurrent = iter++;
		if( !IsControlSelected( (*iterCurrent) ) )
		{
			bFoundHole = true;
			continue;
		}
		if( !bFoundHole )
			continue;
		TDclControlList::reverse_iterator iterOld = iterCurrent--;
		std::swap( *iterCurrent, *iterOld );
		CRect rcOldItem;
		GetItemRect( idxItem, &rcOldItem );
		InvalidateRect( &rcOldItem );
		CRect rcNewItem;
		GetItemRect( idxItem + 1, &rcNewItem );
		InvalidateRect( &rcNewItem );
	}
	OnZOrderChanged();
}

void CZOrderListBox::OnMovezfrontby1() 
{
	if( mControls.empty() )
		return;
	int idxItem = -1;
	bool bFoundHole = false;
	TDclControlList::iterator iter = mControls.begin();
	while( iter != mControls.end() )
	{
		++idxItem;
		TDclControlList::iterator iterCurrent = iter++;
		if( !IsControlSelected( (*iterCurrent) ) )
		{
			bFoundHole = true;
			continue;
		}
		if( !bFoundHole )
			continue;
		TDclControlList::iterator iterOld = iterCurrent--;
		std::swap( *iterCurrent, *iterOld );
		CRect rcOldItem;
		GetItemRect( idxItem, &rcOldItem );
		InvalidateRect( &rcOldItem );
		CRect rcNewItem;
		GetItemRect( idxItem - 1, &rcNewItem );
		InvalidateRect( &rcNewItem );
	}
	OnZOrderChanged();
}

BOOL CZOrderListBox::PreTranslateMessage(MSG* pMsg) 
{
	if( pMsg->message == WM_KEYDOWN || pMsg->message == WM_CHAR )
	{
		if( AfxGetMainWnd()->PreTranslateMessage(pMsg) )
			return TRUE;
	}
	return CListBox::PreTranslateMessage(pMsg);
}

void CZOrderListBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	lpMeasureItemStruct->itemHeight = 20;
}

void CZOrderListBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC* pDC = CDC::FromHandle( lpDrawItemStruct->hDC );
	UINT idxItem = lpDrawItemStruct->itemID;
	if( idxItem < mControls.size() )
	{
		CRect rcDraw = lpDrawItemStruct->rcItem;
		TDclControlList::const_iterator iterControl = mControls.begin();
		for( UINT counter = 0; counter < idxItem; ++counter )
			++iterControl;
		const TDclControlPtr pDclControl = (*iterControl);
		bool bSelected = (IsControlSelected( pDclControl ) || (GetSel(idxItem ) > 0));
		pDC->SaveDC();
		if( bSelected )
		{
			pDC->FillSolidRect( &rcDraw, GetSysColor( COLOR_HIGHLIGHT ) );
			pDC->SetBkColor( GetSysColor( COLOR_HIGHLIGHT ) );
			pDC->SetTextColor( GetSysColor( COLOR_HIGHLIGHTTEXT ) );
		}
		else
			pDC->FillSolidRect( &rcDraw, pDC->GetBkColor() );
		pDC->SetBkMode( TRANSPARENT );
		CPoint ptIcon = rcDraw.TopLeft();
		ptIcon.Offset( 2, 2 );
		mImageList.Draw( pDC, pDclControl->GetType() - 2, ptIcon, bSelected? ILD_SELECTED : ILD_NORMAL );
		rcDraw.left += rcDraw.Height() + 5;
		if( rcDraw.left > rcDraw.right )
			return;
		CString sControlName = pDclControl->GetKeyName();
		pDC->DrawText( sControlName, sControlName.GetLength(), &rcDraw, DT_NOPREFIX | DT_SINGLELINE );
		pDC->RestoreDC( -1 );
	}
	if( idxItem == GetAnchorIndex() )
		pDC->DrawFocusRect( &lpDrawItemStruct->rcItem );
}

void CZOrderListBox::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if( !(nFlags & MK_CONTROL) )
	{
		SelItemRange( FALSE, 0, GetCount() - 1 );
		theStudioWorkspace.ActivateDclControl( NULL );
	}
	mnItemUnselectPending = -1;
	BOOL bOutside;
	int idxSel = ItemFromPoint( point, bOutside );
	if( !bOutside )
	{
		int idxAnchor = GetAnchorIndex();
		if( (nFlags & MK_SHIFT) && idxAnchor >= 0 )
		{
			int idxStart = (idxAnchor < idxSel)? idxAnchor : idxSel;
			int idxEnd = (idxAnchor < idxSel)? idxSel : idxAnchor;
			SelItemRange( TRUE, idxStart, idxEnd );
			TDclControlList::iterator iter = mControls.begin();
			int ctIter = idxStart;
			while( ctIter-- > 0 && iter != mControls.end() )
				++iter;
			ctIter = idxEnd - idxStart;
			while( ctIter-- >= 0 && iter != mControls.end() )
			{
				SetControlSelected( *iter, true );
				theStudioWorkspace.ActivateDclControl( *iter );
				++iter;
			}
			SetAnchorIndex( idxAnchor );
		}
		else
		{
			TDclControlList::iterator iter = mControls.begin();
			int ctIter = idxSel;
			while( ctIter-- > 0 && iter != mControls.end() )
				++iter;
			if( IsControlSelected( *iter ) )
				mnItemUnselectPending = idxSel;
			SetAnchorIndex( idxSel );
			SetControlSelected( *iter, true );
			theStudioWorkspace.ActivateDclControl( *iter );
		}
	}
	mptDragStart = point;

	//__super::OnLButtonDown( nFlags, point );
}

void CZOrderListBox::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if( mbDragging )
		ReleaseCapture();
	__super::OnLButtonDblClk(nFlags, point);
	AfxGetMainWnd()->SendMessage( WM_COMMAND, ID_PROPERTIES, (LPARAM)m_hWnd );
}

void CZOrderListBox::OnLButtonUp(UINT nFlags, CPoint point)
{
	if( mbDragging )
	{
		int idxInsertAt = midxInsertAt;
		ReleaseCapture();
		OnMovezToIndex( idxInsertAt );
		return;
	}
	if( mnItemUnselectPending >= 0 )
	{ //the item was selected, and no dragging was initiated, so unselect it now
		TDclControlList::iterator iter = mControls.begin();
		while( mnItemUnselectPending-- > 0 && iter != mControls.end() )
			++iter;
		TDclControlPtr pDclControlToUnselect = *iter;
		mnItemUnselectPending = -1;
		TDclControlList SelectedControls;
		mpDlgObject->GetSelectedControls( SelectedControls );
		theStudioWorkspace.ActivateDclControl( NULL );
		bool bSelectedOne = false;
		TDclControlList::iterator iterSelected = SelectedControls.begin();
		while( iterSelected != SelectedControls.end() )
		{
			TDclControlPtr pDclControl = *iterSelected++;
			if( pDclControl != pDclControlToUnselect )
			{
				bSelectedOne = true;
				theStudioWorkspace.ActivateDclControl( pDclControl );
			}
		}
		if( !bSelectedOne )
			theStudioWorkspace.ActivateDclControl( mpDlgObject->GetTemplate() );
	}
	__super::OnLButtonUp(nFlags, point);
}

void CZOrderListBox::OnMouseMove(UINT nFlags, CPoint point)
{
	__super::OnMouseMove(nFlags, point);
	if( nFlags & MK_LBUTTON && !mbDragging )
	{ //should we begin dragging?
		CRect rcDragRegion( mptDragStart, CSize( 0, 0 ) );
		rcDragRegion.InflateRect( GetSystemMetrics( SM_CXDOUBLECLK ) / 2,
															GetSystemMetrics( SM_CYDOUBLECLK ) / 2 );
		mbDragging = !rcDragRegion.PtInRect( point );
		if( mbDragging )
			SetCapture();
	}
	if( mbDragging )
	{
		CRect rcClient;
		GetClientRect( &rcClient );
		int nItemHeight = GetItemHeight( 0 );
		int nClientHeight = rcClient.Height();
		int ctVisibleItems = (nClientHeight / nItemHeight);
		int idxFirstVisible = GetTopIndex();
		bool bScroll = false;
		CRect rcCaret;
		if( point.y <= 0 )
		{
			midxInsertAt = idxFirstVisible;
			GetItemRect( midxInsertAt, &rcCaret );
			rcCaret.bottom = rcCaret.top + 2;
			bScroll = true;
		}
		else if( point.y >= rcClient.bottom - (nClientHeight % nItemHeight) - (nItemHeight / 2) )
		{
			midxInsertAt = idxFirstVisible + ctVisibleItems;
			GetItemRect( midxInsertAt - 1, &rcCaret );
			rcCaret.top = rcCaret.bottom - 2;
			bScroll = (point.y > rcClient.bottom);
			if( !bScroll )
				int n = 1;
		}
		else
		{
			BOOL bOutside;
			midxInsertAt = ItemFromPoint( point, bOutside );
			GetItemRect( midxInsertAt, &rcCaret );
			if( point.y > ((rcCaret.bottom + rcCaret.top) / 2) )
			{
				++midxInsertAt;
				rcCaret.top = rcCaret.bottom - 2;
			}
			else
				rcCaret.bottom = rcCaret.top + 2;
		}
		bool bFirstTime = (mrcInsertCaret.IsRectNull() != FALSE);
		CDC* pDC = GetDC();
		if( !bFirstTime && rcCaret.top != -1 && rcCaret.top != mrcInsertCaret.top )
			InvalidateRect( &mrcInsertCaret, TRUE );
		UpdateWindow();
		mrcInsertCaret = rcCaret;
		if( !bFirstTime )
			pDC->FillSolidRect( &rcCaret, RGB(0, 0, 0) );
		ReleaseDC( pDC );
		if( bScroll )
		{
			Invalidate();
			PostMessage( WM_VSCROLL, (WPARAM)((point.y <= 0)? SB_LINEUP : SB_LINEDOWN), 0 );
		}
	}
}

void CZOrderListBox::OnCaptureChanged(CWnd *pWnd)
{
	mbDragging = false;
	midxInsertAt = -1;
	mrcInsertCaret.SetRect( 0, 0, 0, 0 );

	__super::OnCaptureChanged(pWnd);
}

void CZOrderListBox::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if( !mrcInsertCaret.IsRectNull() )
	{
		InvalidateRect( &mrcInsertCaret, TRUE );
		UpdateWindow();
		mrcInsertCaret.top = -1;
	}
	__super::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CZOrderListBox::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	CPoint ptClient( point );
	ScreenToClient( &ptClient );
	BOOL bOutside;
	UINT idxItem = ItemFromPoint( ptClient, bOutside );
	if( idxItem < 0 || idxItem >= mControls.size() )
		return;

	TDclControlList::iterator iter = mControls.begin();
	while( idxItem-- > 0 )
		++iter;
	theStudioWorkspace.ActivateDclControl( *iter );
	CMenu menu;
	if( menu.LoadMenu( IDR_MAINFRAME ) )
	{
		CMenu* pEditMenu = menu.GetSubMenu( 1 );
		pEditMenu->TrackPopupMenu( TPM_LEFTALIGN, point.x, point.y, AfxGetMainWnd() );
	}
}
