// OptionListBox.cpp : implementation file
//

#include "stdafx.h"
#include "OptionListBox.h"
#include "InvokeMethod.h"
#include "SharedRes.h"
#include "AcadColorTable.h"
#include "PropertyIds.h"
#include "DclControlObject.h"
#include "PropertyObject.h"
#include "ToolTips.h"

#define GLYPH_WIDTH 17 


/////////////////////////////////////////////////////////////////////////////
// COptionListBox

COptionListBox::COptionListBox()
{
	m_RowHeight = 20;	
	m_NextHeight = m_RowHeight;
}

COptionListBox::~COptionListBox()
{
	m_ImageList.DeleteImageList();
}


BEGIN_MESSAGE_MAP(COptionListBox, CClrListBox)
	//{{AFX_MSG_MAP(COptionListBox)
	ON_WM_CREATE()
	ON_CONTROL_REFLECT(LBN_SELCHANGE, OnSelchange)
	ON_CONTROL_REFLECT(LBN_DBLCLK, OnDblclk)
	ON_CONTROL_REFLECT(LBN_KILLFOCUS, OnKillfocus)
	ON_CONTROL_REFLECT(LBN_SETFOCUS, OnSetfocus)
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)   	
	ON_WM_CAPTURECHANGED()
  ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void COptionListBox::SetRowHeight(int nNewHeight)
{
	m_RowHeight = nNewHeight;
	m_NextHeight = m_RowHeight;
}

/////////////////////////////////////////////////////////////////////////////
// COptionListBox message handlers

BOOL COptionListBox::Create(CDclControlObject* pControl, CWnd* pParentWnd, UINT nID ) 
{
	CRect ArxRect;

	// set the arx control pointer
	m_ArxControl = pControl;
	
	// get the rectangle of the new control
	ArxRect.top = pControl->m_pTop->GetLongValue();
	ArxRect.left = pControl->m_pLeft->GetLongValue();
	ArxRect.bottom = pControl->m_pHeight->GetLongValue() + ArxRect.top;
	ArxRect.right = pControl->m_pWidth->GetLongValue() + ArxRect.left;
	
	
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | LBS_OWNERDRAWVARIABLE
		| LBS_HASSTRINGS | LBS_NOTIFY |WS_CLIPSIBLINGS;
	
	if (pControl->GetBoolProperty(nNoIntegralHeight) == TRUE)
		dwStyle = dwStyle | LBS_NOINTEGRALHEIGHT;
	
	if (pControl->GetBoolProperty(nSorted) == TRUE)
		dwStyle = dwStyle | LBS_SORT;		


	if (pControl->GetBoolProperty(nVScrollBar) == TRUE)
		dwStyle = dwStyle | WS_VSCROLL;	

	if (pControl->GetBoolProperty(nIsTabStop) != FALSE)
		dwStyle = dwStyle | WS_TABSTOP;
	else
		dwStyle = dwStyle | WS_GROUP;

	BOOL RetVal = CClrListBox::Create(dwStyle,ArxRect, pParentWnd, nID);

	m_ToolTip.Create(this);
	//SetToolTipEx(this, m_ToolTip, pControl);

	switch (m_ArxControl->GetLngProperty(nEventInvoke))
	{
	case 1:
		m_bInvokeWithSendString = true;
		break;
	default:
		m_bInvokeWithSendString = false;
		break;
	}

	m_ImageList.Create(13, 13, ILC_COLOR8|ILC_MASK, 0, 1);
	
	CBitmap bitmap1;
	bitmap1.LoadBitmap(IDB_OPBTNNON);
	m_ImageList.Add(&bitmap1, RGB(255,0,255));
	
	CBitmap bitmap2;
	bitmap2.LoadBitmap(IDB_OPBTNNONH);
	m_ImageList.Add(&bitmap2, RGB(255,0,255));
	
	CBitmap bitmap3;
	bitmap3.LoadBitmap(IDB_OPBTNSEL);
	m_ImageList.Add(&bitmap3, RGB(255,0,255));
	
	CBitmap bitmap4;
	bitmap4.LoadBitmap(IDB_OPBTNSELH);
	m_ImageList.Add(&bitmap4, RGB(255,0,255));

	ResetTooltips();
	return RetVal;
}

void COptionListBox::ResetTooltips()
{
	m_ToolTip.RemoveAllTools();
	RefCountedPtr< CPropertyObject > pTTTProperty = m_ArxControl->GetPropertyObject( nBtnTTText );
	if( !pTTTProperty )
		return;
	RefCountedPtr< CPropertyObject > pToolTipBalloon = m_ArxControl->GetPropertyObject( nToolTipBalloon );
	m_ToolTip.SetDefaultSizes( !pToolTipBalloon || pToolTipBalloon->GetBooleanValue() );
	size_t nBtnQty = pTTTProperty->size();
	for (size_t i = 0; i < GetCount(); i++)
	{
		assert( i < nBtnQty );
		if( i >= nBtnQty )
			break;
		CString sToolTipTitle = m_ArxControl->GetPropertyListItem( nBtnTTText, i );
		if( !sToolTipTitle.IsEmpty() )
		{
			CRect rectItem;
			GetItemRect( i, &rectItem );
			m_ToolTip.AddTool( this, sToolTipTitle, &rectItem, i );
		}
	}
}


void COptionListBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	DrawItem(lpDrawItemStruct, -1);
}

void COptionListBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct, int nHighlight) 
{
	// Lets make a CDC for ease of use
	CDC *pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	
	pDC->SetBkColor(m_BackColor);
	
	ASSERT(pDC); // Attached failed
	
	CRect rc(lpDrawItemStruct->rcItem);
	
	// Draw focus rectangle
	if (lpDrawItemStruct->itemState & ODS_FOCUS)
		pDC->DrawFocusRect(rc);
	
	// Save off context attributes
	int nIndexDC = pDC->SaveDC();

	pDC->SetBkMode(TRANSPARENT);

	pDC->FillRect(rc, m_pStaticBrush);

	CString strCurFont,strNextFont;
	
	GetText(lpDrawItemStruct->itemID, strCurFont);

	int nX = rc.left; // Save for lines
	int nY = rc.top + 1; // Save for lines

	rc.left += GLYPH_WIDTH; // Text Position
	
	CFont *pOldHeadingFont = pDC->SelectObject(GetFont());
	
	pDC->SetTextColor(m_ForeColor);
	
	// draw the text
	LPCTSTR lpszString;
	lpszString = strCurFont;
	CRect rcText = rc;
	rcText.top = rcText.top + 2;
	
	if (lpDrawItemStruct->itemData < 2)
		// draw the text as normal
		m_NextHeight = ::DrawText(pDC->m_hDC, lpszString, strCurFont.GetLength(), &rcText, DT_TOP|DT_WORDBREAK|DT_LEFT);	
	else
	{
		// draw the text as disabled.
		::DrawState(pDC->m_hDC, 
			(HBRUSH)NULL,
			NULL,
			(LPARAM)lpszString, 
			(WPARAM)strCurFont.GetLength(),
			rcText.left, rcText.top, 
			rcText.Width(), rcText.Height(),
			DSS_DISABLED|(TRUE ? DST_PREFIXTEXT : DST_TEXT)); 
	}
	if (GetFocus() == this && (lpDrawItemStruct->itemState & ODS_FOCUS) == ODS_FOCUS )
	{	
		// setup the CRect for Focus Rectangle
		CRect rcCell;
		rcCell.left = rc.left - 2;
		rcCell.top = rc.top;
		rcCell.right = rc.right;
		rcCell.bottom = rcCell.top + m_NextHeight + 3;

		if (lpDrawItemStruct->itemData < 2)	
			// draw the solid rectangle
			::DrawFocusRect(pDC->m_hDC, &rcCell);
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

	m_ImageList.Draw(pDC, nImageIndex, CPoint(2, rc.top+2), ILD_NORMAL);//ILD_TRANSPARENT);
	
	// Restore State of context
	pDC->RestoreDC(nIndexDC);
	
	return;
}

void COptionListBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	lpMeasureItemStruct->itemHeight = m_RowHeight;
}

BOOL COptionListBox::PreTranslateMessage(MSG* pMsg) 
{
	m_ToolTip.RelayEvent(pMsg);
	
  //On a return, do a double-click if one is defined. Otherwise, do a tab.
  if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN)
		{
			if (m_ArxControl->GetBoolProperty(nReturnAsTab))
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
			//Invalidate();
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
			//Invalidate();
			int nCurSel = GetCurSel();
			if( nCurSel < GetCount() - 1 )
				++nCurSel;
			else
				nCurSel = 0;
			SetCurSel(nCurSel);
			return TRUE;
		}
	}
	
	return CClrListBox::PreTranslateMessage(pMsg);
}



void COptionListBox::OnSelchange() 
{
	int nCurSel = GetCurSel();
	
	//if (GetItemData(nCurSel) == 2)
	//{
	//	SetCurSel(m_CurSel);
	//	return;
	//}
	//if (m_CurSel != nCurSel)
	//{
	//	// we must redraw the default selected item because it will not redraw itself.
	//	CRect rc;
	//	GetItemRect(m_CurSel, rc);
	//	InvalidateRect(rc);		
	//}
	//
	//m_CurSel = nCurSel;

	if (nCurSel > -1)
	{
		CString sString;
	
		int n = GetTextLen(nCurSel);      
		GetText(nCurSel, sString.GetBuffer(n));
		sString.ReleaseBuffer();

		// call methods to invoke the event
		InvokeMethodIntString(m_ArxControl->GetStrProperty(nEventSelChanged), nCurSel, sString, m_bInvokeWithSendString);
	}   

}
void COptionListBox::OnDblclk() 
{
	// call methods to invoke the event
	InvokeMethod(m_ArxControl->GetStrProperty(nEventDblClicked), m_bInvokeWithSendString);
}

void COptionListBox::OnKillfocus() 
{
	// call methods to invoke the event
	InvokeMethod(m_ArxControl->GetStrProperty(nEventKillFocus), m_bInvokeWithSendString);
}


void COptionListBox::OnSetfocus() 
{
	// call methods to invoke the event
	InvokeMethod(m_ArxControl->GetStrProperty(nEventSetFocus), m_bInvokeWithSendString);
}

int COptionListBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CClrListBox::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	RefCountedPtr< CPropertyObject > pListProperty = m_ArxControl->GetPropertyObject(nBtnCaption);
	m_RowHeight = (short)m_ArxControl->GetLngProperty(nRowHeight);
	int nCurSel = (short)m_ArxControl->GetLngProperty(nDefSelIndex);
	CRect rc;
	GetClientRect(&rc);
	size_t nCount = pListProperty->size();
	for (int i = 0; i < nCount; i++)
	{
		int nIndex = AddString(pListProperty->GetStringItem(i));		
		if (i == nCount-1)
		{
			int nRemainder = rc.Height() - ((nCount -1 )* m_RowHeight);
			SetItemHeight(nIndex, nRemainder-2);
		}
		else
			SetItemHeight(nIndex, m_RowHeight);
	}
	SetCurSel(nCurSel);
	SetCaretIndex(nCurSel);
	return 0;
}

LRESULT COptionListBox::OnMouseLeave(WPARAM wParam, LPARAM lParam) 
{
	RedrawWindow();
	return FALSE;
}

void COptionListBox::OnMouseMove(UINT nFlags, CPoint point) 
{
	CClrListBox::OnMouseMove(nFlags, point);

	InvokeMethodIntIntInt(
		m_ArxControl->GetStrProperty(nEventMouseMove),
		nFlags,
		point.x,
		point.y,
		m_bInvokeWithSendString);

	CRect rc;

	CDC *pDC = GetDC();
	int nSel = GetCurSel();

	// here we need to find the item the mouse is over
  int i;
	for (i=0; i<GetCount(); i++)
	{		
		GetItemRect(i, rc);
		rc.bottom = rc.top + m_RowHeight;
		if (rc.PtInRect(point))		
		{
			if (nSel == i)
				m_ImageList.Draw(pDC, 3, CPoint(2, rc.top+2), ILD_NORMAL);//ILD_TRANSPARENT);
			else
				m_ImageList.Draw(pDC, 1, CPoint(2, rc.top+2), ILD_NORMAL);//ILD_TRANSPARENT);
		}
		else
		{
			if (nSel == i)
				m_ImageList.Draw(pDC, 2, CPoint(2, rc.top+2), ILD_NORMAL);//ILD_TRANSPARENT);
			else
				m_ImageList.Draw(pDC, 0, CPoint(2, rc.top+2), ILD_NORMAL);//ILD_TRANSPARENT);
		}
	}
	pDC->Detach();
}

void COptionListBox::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
  if (nChar == VK_RETURN) {
    InvokeMethod(m_ArxControl->GetStrProperty(nEventDblClicked), m_bInvokeWithSendString);
  }
}
