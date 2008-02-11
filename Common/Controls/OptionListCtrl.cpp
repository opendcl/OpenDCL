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
: CListBoxCtrl( pTemplate, pPane, nID, false )
, mnRowHeight( 20 )
{
	mImageList.Create(13, 13, ILC_COLOR8 | ILC_MASK, 0, 1);
	
	CBitmap bitmap1;
	bitmap1.LoadBitmap(IDB_OPTBTN);
	mImageList.Add( &bitmap1, RGB(255,0,255) );
	
	CBitmap bitmap2;
	bitmap2.LoadBitmap(IDB_OPTBTNH);
	mImageList.Add( &bitmap2, RGB(255,0,255) );
	
	CBitmap bitmap3;
	bitmap3.LoadBitmap(IDB_OPTBTNSEL);
	mImageList.Add( &bitmap3, RGB(255,0,255) );
	
	CBitmap bitmap4;
	bitmap4.LoadBitmap(IDB_OPTBTNSELH);
	mImageList.Add( &bitmap4, RGB(255,0,255) );

	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

COptionListCtrl::~COptionListCtrl()
{
}

bool COptionListCtrl::Create( CWnd* pParentWnd, UINT nID ) 
{
	bool bSuccess =
		__super::Create( pParentWnd, nID );

	SetCaretIndex( GetCurSel() );
	ResetTooltips();

	return bSuccess;
}

DWORD COptionListCtrl::GetWndStyle() const
{
	DWORD dwStyle = __super::GetWndStyle();
	dwStyle &= ~(LBS_USETABSTOPS);
	dwStyle |= LBS_OWNERDRAWFIXED;
	return dwStyle;
}

bool COptionListCtrl::OnApplyProperty( TPropertyPtr pProp )
{
	if( !__super::OnApplyProperty( pProp ) )
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
	case Prop::DefSelIndex:
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
			OnApplyProperty( mpTemplate->GetPropertyObject( Prop::RowHeight ) );
			OnApplyProperty( mpTemplate->GetPropertyObject( Prop::FontName ) );
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
	TPropertyPtr pTTTProperty = mpTemplate->GetPropertyObject( Prop::BtnTTText );
	if( !pTTTProperty )
		return;
	TPropertyPtr pToolTipBalloon = mpTemplate->GetPropertyObject( Prop::ToolTipBalloon );
	GetToolTipCtrl().SetDefaultSizes( !pToolTipBalloon || pToolTipBalloon->GetBooleanValue() );
	size_t nTTQty = pTTTProperty->size();
	for (size_t i = 0; i < (size_t)GetCount(); i++)
	{
		if( i >= nTTQty )
			break;
		CString sToolTipTitle = mpTemplate->GetPropertyListItem( Prop::BtnTTText, i );
		if( !sToolTipTitle.IsEmpty() )
		{
			CRect rectItem;
			GetItemRect( i, &rectItem );
			GetToolTipCtrl().AddTool( this, sToolTipTitle, &rectItem, i );
		}
	}
}


BEGIN_MESSAGE_MAP(COptionListCtrl, CListBoxCtrl)
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)   	
	ON_WM_MEASUREITEM_REFLECT()
	ON_WM_WINDOWPOSCHANGING()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// COptionListCtrl message handlers

void COptionListCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	if( lpDrawItemStruct->itemID >= (UINT)GetCount() )
		return;

	// Lets make a CDC for ease of use
	CDC *pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	ASSERT(pDC); // Attached failed
	
	// Save off context attributes
	int nIndexDC = pDC->SaveDC();
	
	pDC->SetBkColor(GetColorService()->GetBackgroundColor());
	
	CRect rc(lpDrawItemStruct->rcItem);
	
	// Draw focus rectangle
	if (lpDrawItemStruct->itemState & ODS_FOCUS)
		pDC->DrawFocusRect(rc);

	pDC->SetBkMode(TRANSPARENT);
	pDC->FillRect(rc, GetColorService()->GetBackgroundCBrush());

	CString strCurFont,strNextFont;
	GetText(lpDrawItemStruct->itemID, strCurFont);

	int nX = rc.left; // Save for lines
	int nY = rc.top + 1; // Save for lines

	rc.left += GLYPH_WIDTH; // Text Position
	
	CFont *pOldHeadingFont = pDC->SelectObject(GetFont());
	
	pDC->SetTextColor(GetColorService()->GetForegroundColor());
	
	// draw the text
	CRect rcText = rc;
	rcText.top = rcText.top + 2;
	if (lpDrawItemStruct->itemData < 2)
	{
		// draw the text as normal
		int nHeight = ::DrawText(pDC->m_hDC, strCurFont, strCurFont.GetLength(), &rcText, DT_TOP|DT_WORDBREAK|DT_LEFT);	
		if (GetFocus() == this && (lpDrawItemStruct->itemState & ODS_FOCUS) == ODS_FOCUS )
		{	
			// setup the CRect for Focus Rectangle
			CRect rcCell;
			rcCell.left = rc.left - 2;
			rcCell.top = rc.top;
			rcCell.right = rc.right;
			rcCell.bottom = rcCell.top + nHeight + 3;
			::DrawFocusRect(pDC->m_hDC, &rcCell); // draw the solid rectangle
		}
	}
	else
	{
		// draw the text as disabled.
		::DrawState(pDC->m_hDC, 
			(HBRUSH)NULL,
			NULL,
			(LPARAM)(LPCTSTR)strCurFont, 
			(WPARAM)strCurFont.GetLength(),
			rcText.left, rcText.top, 
			rcText.Width(), rcText.Height(),
			DSS_DISABLED|(TRUE ? DST_PREFIXTEXT : DST_TEXT)); 
	}
	pDC->SelectObject(pOldHeadingFont);

	// lets draw the left side dark gray lines
	COLORREF rgb = GetSysColor(COLOR_BTNSHADOW);
	HGDIOBJ pen = ::CreatePen(PS_SOLID, 1, rgb);
	HGDIOBJ OldPen = SelectObject(pDC->m_hDC, pen);
	CPoint point;

	int nImageIndex = 0;
	if (lpDrawItemStruct->itemState & ODS_SELECTED)
		nImageIndex = 2;
	//if (m_nLastHighlighted == lpDrawItemStruct->itemID)
	//	nImageIndex++;

	mImageList.Draw(pDC, nImageIndex, CPoint(2, rc.top+2), ILD_NORMAL);//ILD_TRANSPARENT);
	
	::DeleteObject(::SelectObject(pDC->m_hDC, OldPen));
	// Restore State of context
	pDC->RestoreDC(nIndexDC);
	
	return;
}

void COptionListCtrl::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	lpMeasureItemStruct->itemHeight = mnRowHeight;
}

BOOL COptionListCtrl::PreTranslateMessage(MSG* pMsg) 
{
	GetToolTipCtrl().RelayEvent(pMsg);
	
  //On a return, do a double-click if one is defined. Otherwise, do a tab.
  if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN)
		{
			if (mpTemplate->GetBooleanProperty(Prop::ReturnAsTab))
				pMsg->wParam = VK_TAB;
			else
			{
				SetCurSel(GetCaretIndex());
				return TRUE;
			}
		}
		else if (pMsg->wParam == VK_SPACE)
		{
			SetCurSel(GetCaretIndex());
			return TRUE;
		}
		else if (pMsg->wParam == VK_LEFT || pMsg->wParam == VK_UP)
		{
			//int nCaret = GetCaretIndex();
			//if( nCaret > 0 )
			//	--nCaret;
			//else
			//	nCaret = GetCount() - 1;
			//SetCaretIndex(nCaret);
			//OnNeedRepaint();
			int nCurSel = GetCurSel();
			if( nCurSel > 0 )
				--nCurSel;
			else
				nCurSel = GetCount() - 1;
			SetCurSel(nCurSel);
			return TRUE;
		}
		else if (pMsg->wParam == VK_RIGHT || pMsg->wParam == VK_DOWN)
		{
			//int nCaret = GetCaretIndex();
			//if( nCaret < GetCount() )
			//	++nCaret;
			//else
			//	nCaret = 0;
			//SetCaretIndex(nCaret);
			//OnNeedRepaint();
			int nCurSel = GetCurSel();
			if( nCurSel < GetCount() - 1 )
				++nCurSel;
			else
				nCurSel = 0;
			SetCurSel(nCurSel);
			return TRUE;
		}
	}
	
	return __super::PreTranslateMessage(pMsg);
}

LRESULT COptionListCtrl::OnMouseLeave(WPARAM wParam, LPARAM lParam) 
{
	RedrawWindow();
	return FALSE;
}

void COptionListCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	__super::OnMouseMove(nFlags, point);

	CDC *pDC = GetDC();
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
		if( rcItem.PtInRect( point ) )
		{
			if( nSel == idxItem )
				mImageList.Draw( pDC, 3, CPoint( 2, rcItem.top + 2), ILD_NORMAL );//ILD_TRANSPARENT);
			else
				mImageList.Draw( pDC, 1, CPoint( 2, rcItem.top + 2), ILD_NORMAL );//ILD_TRANSPARENT);
		}
		else
		{
			if( nSel == idxItem )
				mImageList.Draw( pDC, 2, CPoint( 2, rcItem.top + 2), ILD_NORMAL );//ILD_TRANSPARENT);
			else
				mImageList.Draw( pDC, 0, CPoint( 2, rcItem.top + 2), ILD_NORMAL );//ILD_TRANSPARENT);
		}
	}
	ReleaseDC(pDC);
}
