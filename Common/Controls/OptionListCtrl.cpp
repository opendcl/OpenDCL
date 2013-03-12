// OptionListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "OptionListCtrl.h"
#include "Resource.h"
#include "PropertyIds.h"
#include "DclControlObject.h"
#include "ControlPane.h"
#include "PropertyObject.h"
#include "ToolTips.h"

#define GLYPH_WIDTH 17 


/////////////////////////////////////////////////////////////////////////////
// COptionListCtrl

COptionListCtrl::COptionListCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CDialogControl( pTemplate, pPane, this )
, mnRowHeight( 20 )
, mbTrackingMouse( false )
, idxInitialFocusItem( -1 )
{
	mImageList.Create( 13, 13, ILC_COLOR8 | ILC_MASK, 4, 1 );
	
	CBitmap bitmap1;
	bitmap1.LoadBitmap(IDB_OPTBTN);
	mImageList.Add( &bitmap1, RGB(255,0,255) );
	
	CBitmap bitmap2;
	bitmap2.LoadBitmap(IDB_OPTBTNH);
	mImageList.Add( &bitmap2, RGB(255,0,255) );

	CBitmap bitmap3;
	bitmap3.LoadBitmap(IDB_OPTBTND);
	mImageList.Add( &bitmap3, RGB(255,0,255) );
	
	CBitmap bitmap4;
	bitmap4.LoadBitmap(IDB_OPTBTNSEL);
	mImageList.Add( &bitmap4, RGB(255,0,255) );
	
	CBitmap bitmap5;
	bitmap5.LoadBitmap(IDB_OPTBTNSELH);
	mImageList.Add( &bitmap5, RGB(255,0,255) );
	
	CBitmap bitmap6;
	bitmap6.LoadBitmap(IDB_OPTBTNSELD);
	mImageList.Add( &bitmap6, RGB(255,0,255) );
	
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

COptionListCtrl::~COptionListCtrl()
{
}

DWORD COptionListCtrl::GetWndStyle() const
{
	DWORD dwStyle = CDialogControl::GetWndStyle();
	dwStyle |= (LBS_HASSTRINGS | LBS_OWNERDRAWFIXED | LBS_NOTIFY | LBS_WANTKEYBOARDINPUT | LBS_NOINTEGRALHEIGHT);
	return dwStyle;
}

bool COptionListCtrl::Create( CWnd* pParentWnd, UINT nID ) 
{
	bool bSuccess = (__super::Create( GetWndStyle(), GetWndRect(), pParentWnd, nID ) != FALSE);
	if( bSuccess && !ApplyPropertiesEnum() )
		bSuccess = false;

	if( bSuccess )
	{
		ModifyStyleEx( WS_EX_TRANSPARENT, 0 );
		SetCaretIndex( GetCurSel() );

		//The automatic vertical scroll bar doesn't get initialized correctly unless the listbox window is
		//first resized small enough that the scroll bar would be needed
		CRect rc;
		GetWindowRect( &rc );
		GetParent()->ScreenToClient( &rc );
		MoveWindow( CRect( 0, 0, 0, 0 ), FALSE );
		MoveWindow( &rc );

		ResetTooltips();
	}

	return bSuccess;
}

bool COptionListCtrl::ApplyProperty( TPropertyPtr pProp )
{
	if( !__super::ApplyProperty( pProp ) )
		return false;
	bool bFailed = false;
	switch( pProp->GetID() )
	{
	case Prop::RowHeight:
		{
			long nNewHeight = pProp->GetLongValue();
			mnRowHeight = nNewHeight > 0? nNewHeight : 20;
			SetItemHeight( 0, mnRowHeight );
			break;
		}
	case Prop::CurSelIndex:
		{
			SetCurSel( pProp->GetLongValue() );
			break;
		}
	case Prop::Enabled:
		{
			int nData = pProp->GetBooleanValue()? 0 : 2;
			for( int i = 0; i < GetCount(); i++ )
				SetItemData( i, nData );
			OnNeedRepaint();				
			break;
		}
	case Prop::BtnCaption:
		{
			ApplyProperty( mpTemplate->GetPropertyObject( Prop::RowHeight ) );
			ApplyProperty( mpTemplate->GetPropertyObject( Prop::FontName ) );
			int nCurSel = GetCurSel();
			ResetContent();					
			size_t nMax = pProp->size();
			for( size_t idx = 0; idx < nMax; ++idx )
			{				
				CString sOption = pProp->GetStringItem( idx );
				int nNewItem = AddString( sOption );
				SetItemData( nNewItem, (idx == nCurSel)? 1 : 0 );
			}
			ResetTooltips();
			if( nCurSel >= 0 && nCurSel >= (int)nMax )
				nCurSel = (int)nMax - 1;
			SetCurSel( nCurSel );
			OnNeedRepaint();
			break;
		}
	}
	return !bFailed;
}

void COptionListCtrl::ResetTooltips()
{
	GetToolTipCtrl().RemoveAllTools();
	TPropertyPtr pTTTProperty = mpTemplate->GetPropertyObject( Prop::BtnToolTips );
	if( !pTTTProperty )
		return;
	TPropertyPtr pToolTipBalloon = mpTemplate->GetPropertyObject( Prop::ToolTipBalloon );
	GetToolTipCtrl().SetDefaultSizes( !pToolTipBalloon || pToolTipBalloon->GetBooleanValue() );
	size_t nTTQty = pTTTProperty->size();
	for (size_t i = 0; i < (size_t)GetCount(); i++)
	{
		if( i >= nTTQty )
			break;
		CString sToolTipTitle = mpTemplate->GetPropertyListItem( Prop::BtnToolTips, i );
		if( !sToolTipTitle.IsEmpty() )
		{
			CRect rectItem;
			GetItemRect( i, &rectItem );
			GetToolTipCtrl().AddTool( this, sToolTipTitle, &rectItem, i );
		}
	}
}


BEGIN_MESSAGE_MAP(COptionListCtrl, CListBox)
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE, &COptionListCtrl::OnMouseLeave)   
	ON_WM_MEASUREITEM_REFLECT()
	ON_CONTROL_REFLECT(LBN_SELCHANGE, &COptionListCtrl::OnLbnSelchange)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	ON_WM_ERASEBKGND()
	ON_WM_DRAWITEM_REFLECT()
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// COptionListCtrl message handlers

void COptionListCtrl::PostNcDestroy() 
{
	__super::PostNcDestroy();
	delete this;
}

void COptionListCtrl::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	lpMeasureItemStruct->itemHeight = mnRowHeight;
}

BOOL COptionListCtrl::PreTranslateMessage(MSG* pMsg) 
{
	GetToolTipCtrl().RelayEvent( pMsg );
	
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN)
		{
			if (mpTemplate->GetBooleanProperty(Prop::ReturnAsTab))
				pMsg->wParam = VK_TAB;
		}
		else if (pMsg->wParam == VK_LEFT || pMsg->wParam == VK_UP)
		{
			int nCurSel = GetCurSel();
			int idxNext = -1;
			for( int idxItem = nCurSel - 1; idxItem >= 0; --idxItem )
			{
				if( !(GetItemData( idxItem ) & 2) ) //enabled?
				{
					idxNext = idxItem;
					break;
				}
			}
			if( idxNext < 0 )
			{
				for( int idxItem = GetCount() - 1; idxItem > nCurSel; --idxItem )
				{
					if( !(GetItemData( idxItem ) & 2) ) //enabled?
					{
						idxNext = idxItem;
						break;
					}
				}
			}
			if( idxNext >= 0 )
			{
				SetCurSel(idxNext);
				SetCaretIndex(idxNext);
				GetParent()->SendMessage( WM_COMMAND, MAKEWPARAM(0,LBN_SELCHANGE), (LPARAM)m_hWnd );
			}
			return TRUE;
		}
		else if (pMsg->wParam == VK_RIGHT || pMsg->wParam == VK_DOWN)
		{
			int nCurSel = GetCurSel();
			int idxNext = -1;
			for( int idxItem = nCurSel + 1; idxItem < GetCount(); ++idxItem )
			{
				if( !(GetItemData( idxItem ) & 2) ) //enabled?
				{
					idxNext = idxItem;
					break;
				}
			}
			if( idxNext < 0 )
			{
				for( int idxItem = 0; idxItem < nCurSel; ++idxItem )
				{
					if( !(GetItemData( idxItem ) & 2) ) //enabled?
					{
						idxNext = idxItem;
						break;
					}
				}
			}
			if( idxNext >= 0 )
			{
				SetCurSel(idxNext);
				SetCaretIndex(idxNext);
				GetParent()->SendMessage( WM_COMMAND, MAKEWPARAM(0,LBN_SELCHANGE), (LPARAM)m_hWnd );
			}
			return TRUE;
		}
	}
	
	return __super::PreTranslateMessage(pMsg);
}

void COptionListCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	__super::OnMouseMove(nFlags, point);

	if( !mbTrackingMouse )
	{
		TRACKMOUSEEVENT tm = { sizeof(TRACKMOUSEEVENT), TME_LEAVE, m_hWnd, 0 };
		if( _TrackMouseEvent( &tm ) )
			mbTrackingMouse = true;
	}

	if( !IsWindowEnabled() )
		return;

	CDC *pDC = GetDC();
	pDC->SaveDC();
	int nSel = GetCurSel();

	CRect rcClient;
	GetClientRect( &rcClient );
	rcClient.InflateRect( 1, 1 );
	for( int idxItem = 0; idxItem < GetCount(); ++idxItem )
	{		
		CRect rcItem;
		if( LB_ERR == GetItemRect( idxItem, &rcItem ) )
			continue;
		rcItem.bottom = rcItem.top + mnRowHeight;
		if( !rcClient.PtInRect( rcItem.TopLeft() ) || !rcClient.PtInRect( rcItem.BottomRight() ) )
			continue;

		bool bSelected = (nSel == idxItem);
		bool bHighlighted = (rcItem.PtInRect( point ) != FALSE);
		int idxImage = bHighlighted? 1 : 0;
		if( bSelected )
			idxImage += 3;
		mImageList.Draw( pDC, idxImage, CPoint( 2, rcItem.top + 2), ILD_NORMAL );
	}
	pDC->RestoreDC( -1 );
	ReleaseDC(pDC);
}

LRESULT COptionListCtrl::OnMouseLeave(WPARAM wParam, LPARAM lParam) 
{
	mbTrackingMouse = false;        
	OnNeedRepaint( true );
	return FALSE;
}

void COptionListCtrl::OnLbnSelchange()
{
	mpTemplate->SetLongProperty( Prop::CurSelIndex, GetCurSel() );
}

HBRUSH COptionListCtrl::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	return HandleCtlColor( pDC, nCtlColor );
}

BOOL COptionListCtrl::OnEraseBkgnd(CDC* pDC)
{
	if( HandleEraseBkgnd( pDC ) )
		return TRUE;
	return __super::OnEraseBkgnd(pDC);
}

void COptionListCtrl::OnSetFocus(CWnd* pOldWnd) 
{
	__super::OnSetFocus( pOldWnd );
	OnNeedRepaint( true );
}

void COptionListCtrl::OnKillFocus(CWnd* pNewWnd)
{
	__super::OnKillFocus(pNewWnd);
	OnNeedRepaint( true );
}

void COptionListCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if( lpDrawItemStruct->itemID >= (UINT)GetCount() )
		return;

	CDC *pDC = CDC::FromHandle( lpDrawItemStruct->hDC );
	ASSERT(pDC); // Attached failed
	pDC->SaveDC();

	CRect rcItem( lpDrawItemStruct->rcItem );

	//calculate text area
	CRect rcText( rcItem );
	rcText.top += 2;
	rcText.left += (GLYPH_WIDTH + 3);
	CString sCaption;
	GetText( lpDrawItemStruct->itemID, sCaption );
	pDC->DrawText( sCaption, -1, &rcText, DT_TOP | DT_WORDBREAK | DT_LEFT | DT_CALCRECT | DT_NOPREFIX );	

	if( (lpDrawItemStruct->itemAction & ODA_DRAWENTIRE) )
	{
		pDC->SetTextColor( GetColorService()->GetForegroundColor() );
		pDC->SetBkMode( TRANSPARENT );
		
		if( !(lpDrawItemStruct->itemData & 2) )
			pDC->DrawText( sCaption, -1, &rcText, DT_TOP | DT_WORDBREAK | DT_LEFT | DT_NOPREFIX );	
		else
		{ // draw the text as disabled
			pDC->DrawState( rcText.TopLeft(), CSize( rcText.Width(), rcText.Height() ),
											sCaption, DSS_DISABLED, FALSE, 0, (HBRUSH)NULL );
		}
		pDC->SetBkMode( OPAQUE );
	}
	if( (lpDrawItemStruct->itemAction & (ODA_SELECT | ODA_DRAWENTIRE)) )
	{
		bool bSelected = ((lpDrawItemStruct->itemState & ODS_SELECTED) != 0);
		bool bDisabled = ((lpDrawItemStruct->itemData & 2) != 0);
		int idxImage = bDisabled? 2 : 0;
		if( bSelected )
			idxImage += 3;
		mImageList.Draw( pDC, idxImage, CPoint( rcItem.left + 2, rcItem.top + 2 ), ILD_NORMAL );
	}
	if( (lpDrawItemStruct->itemAction & ODA_FOCUS) )
	{
		bool bFocus = ((lpDrawItemStruct->itemState & ODS_FOCUS) != 0);
		if( bFocus || idxInitialFocusItem >= 0 )
		{
			if( idxInitialFocusItem < 0 )
				idxInitialFocusItem = lpDrawItemStruct->itemID;
			CRect rcFocus( rcText );
			rcFocus.InflateRect( 2, 2 );
			rcFocus &= rcItem;
			pDC->DrawFocusRect( &rcFocus );
		}
	}

	pDC->RestoreDC( -1 );
	return;
}

LRESULT COptionListCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// hack to handle unwanted background painting when scrolling transparent window
	if( message == WM_PRINTCLIENT )
	{
		Invalidate();
		return 0;
	}

	return __super::WindowProc(message, wParam, lParam);
}

__UINT_LRESULT COptionListCtrl::OnNcHitTest(CPoint point)
{
	LRESULT lResult = __super::OnNcHitTest(point);
	if( lResult == HTCLIENT )
	{
		CPoint ptClient = point;
		ScreenToClient( &ptClient );
		BOOL bOutside = TRUE;
		int idxItem = ItemFromPoint( ptClient, bOutside );
		if( !bOutside && idxItem >= 0 && (GetItemData( idxItem ) & 2) ) //is this item disabled?
			return HTNOWHERE; //prevent clicking
	}
	return lResult;
}
