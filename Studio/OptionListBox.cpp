// OptionListBox.cpp : implementation file
//

#include "stdafx.h"
#include "OptionListBox.h"
#include "AcadColorTable.h"

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
	
}


BEGIN_MESSAGE_MAP(COptionListBox, CAcadColorListBox)
	//{{AFX_MSG_MAP(COptionListBox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void COptionListBox::SetRowHeight(int nNewHeight)
{
	m_RowHeight = nNewHeight;
	m_NextHeight = m_RowHeight;
}
/////////////////////////////////////////////////////////////////////////////
// COptionListBox message handlers

void COptionListBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	ASSERT(lpDrawItemStruct->CtlType == ODT_LISTBOX); // We've gotta be a combo

	// Lets make a CDC for ease of use
	CDC *pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	
	pDC->SetBkColor(GetRGBColor(nButtonFace));
	
	ASSERT(pDC); // Attached failed
	
	CRect rc(lpDrawItemStruct->rcItem);
	
	// Draw focus rectangle
	if (lpDrawItemStruct->itemState & ODS_FOCUS)
		pDC->DrawFocusRect(rc);
	
	// Save off context attributes
	int nIndexDC = pDC->SaveDC();

	pDC->SetBkMode(TRANSPARENT);

	CBrush brBack(GetRGBColor(nButtonFace));
	pDC->FillRect(rc, &brBack);
	brBack.DeleteObject();

	CString strCurFont,strNextFont;
	
	GetText(lpDrawItemStruct->itemID,strCurFont);

	DWORD dwData = GetItemData(lpDrawItemStruct->itemID);
	
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
	
	if (lpDrawItemStruct->itemState & ODS_SELECTED && GetFocus() == this)
	{	
		// setup the CRect for Focus Rectangle
		CRect rcCell;
		rcCell.left = rc.left - 2;
		rcCell.top = rc.top;
		rcCell.right = rc.right;
		rcCell.bottom = rcCell.top + m_NextHeight + 3;

		// draw the solid rectangle
		::DrawFocusRect(pDC->m_hDC, &rcCell);
	}
	pDC->SelectObject(pOldHeadingFont);

	// lets draw the left side dark gray lines
	COLORREF rgb = GetSysColor(COLOR_BTNSHADOW);
	HGDIOBJ pen = ::CreatePen(PS_SOLID, 1, rgb);
	HGDIOBJ OldPen = SelectObject(pDC->m_hDC, pen);
	CPoint point;

	// here we have to recalculate the nY variable to center the option circle on the text just drawn.
	CSize size = pDC->GetTextExtent(_T("Test"), 4);
	nY = rc.top + (size.cy / 2) - 6 + 1;

	MoveToEx(pDC->m_hDC,nX+2, nY+5, &point);
	LineTo(pDC->m_hDC,	nX+2, nY+9);		
	
	MoveToEx(pDC->m_hDC,nX+3, nY+3, &point);
	LineTo(pDC->m_hDC,	nX+3, nY+5);		
	
	MoveToEx(pDC->m_hDC,nX+3, nY+9, &point);
	LineTo(pDC->m_hDC,	nX+3, nY+11);		
	
	MoveToEx(pDC->m_hDC,nX+4, nY+2, &point);
	LineTo(pDC->m_hDC,	nX+6, nY+2);		
	
	MoveToEx(pDC->m_hDC,nX+6, nY+1, &point);
	LineTo(pDC->m_hDC,	nX+10, nY+1);		
	
	MoveToEx(pDC->m_hDC,nX+10, nY+2, &point);
	LineTo(pDC->m_hDC,	nX+12, nY+2);		
	
	SelectObject(pDC->m_hDC, OldPen);			
	DeleteObject(pen);


	// lets draw the left side black lines
	rgb = GetSysColor(COLOR_3DDKSHADOW);
	pen = ::CreatePen(PS_SOLID, 1, rgb);
	OldPen = SelectObject(pDC->m_hDC, pen);
	
	MoveToEx(pDC->m_hDC,nX+3, nY+5, &point);
	LineTo(pDC->m_hDC,	nX+3, nY+9);		
	
	MoveToEx(pDC->m_hDC,nX+4, nY+3, &point);
	LineTo(pDC->m_hDC,	nX+4, nY+5);		
	
	SetPixel(pDC->m_hDC, nX+4, nY+9, rgb);
	
	MoveToEx(pDC->m_hDC,nX+4, nY+3, &point);
	LineTo(pDC->m_hDC,	nX+6, nY+3);		
	
	MoveToEx(pDC->m_hDC,nX+6, nY+2, &point);
	LineTo(pDC->m_hDC,	nX+10, nY+2);		
	
	MoveToEx(pDC->m_hDC,nX+10, nY+3, &point);
	LineTo(pDC->m_hDC,	nX+12, nY+3);		
	
	SelectObject(pDC->m_hDC, OldPen);			
	DeleteObject(pen);
	

	// lets draw the inside light grey lines
	rgb = GetSysColor(COLOR_BTNHILIGHT);
	pen = ::CreatePen(PS_SOLID, 1, rgb);
	OldPen = SelectObject(pDC->m_hDC, pen);
	
	MoveToEx(pDC->m_hDC,nX+4, nY+10, &point);
	LineTo(pDC->m_hDC,	nX+6, nY+10);		
	
	MoveToEx(pDC->m_hDC,nX+6, nY+11, &point);
	LineTo(pDC->m_hDC,	nX+10,nY+11);		
	
	MoveToEx(pDC->m_hDC,nX+10, nY+10, &point);
	LineTo(pDC->m_hDC,	nX+12,nY+10);		
	
	SetPixel(pDC->m_hDC, nX+11, nY+9, rgb);

	MoveToEx(pDC->m_hDC,nX+12, nY+5, &point);
	LineTo(pDC->m_hDC,	nX+12,nY+9);	
	
	SetPixel(pDC->m_hDC, nX+11, nY+4, rgb);
	
	SelectObject(pDC->m_hDC, OldPen);			
	DeleteObject(pen);
	
	
	// lets draw the outside white lines
	rgb = GetSysColor(COLOR_WINDOW);
	pen = ::CreatePen(PS_SOLID, 1, rgb);
	OldPen = SelectObject(pDC->m_hDC, pen);
	
	MoveToEx(pDC->m_hDC,nX+12, nY+3, &point);
	LineTo(pDC->m_hDC,	nX+12,nY+5);	
	
	MoveToEx(pDC->m_hDC,nX+13, nY+5, &point);
	LineTo(pDC->m_hDC,	nX+13,nY+9);	
	
	MoveToEx(pDC->m_hDC,nX+12, nY+9, &point);
	LineTo(pDC->m_hDC,	nX+12,nY+11);	
	
	MoveToEx(pDC->m_hDC,nX+10, nY+11, &point);
	LineTo(pDC->m_hDC,	nX+12,nY+11);	
	
	MoveToEx(pDC->m_hDC,nX+6, nY+12, &point);
	LineTo(pDC->m_hDC,	nX+10,nY+12);	
	
	MoveToEx(pDC->m_hDC,nX+4, nY+11, &point);
	LineTo(pDC->m_hDC,	nX+6,nY+11);	

	MoveToEx(pDC->m_hDC,nX+6, nY+3, &point);
	LineTo(pDC->m_hDC,	nX+10,nY+3);	

	MoveToEx(pDC->m_hDC,nX+5, nY+4, &point);
	LineTo(pDC->m_hDC,	nX+11,nY+4);	

	MoveToEx(pDC->m_hDC,nX+4, nY+5, &point);
	LineTo(pDC->m_hDC,	nX+12,nY+5);	

	MoveToEx(pDC->m_hDC,nX+4, nY+6, &point);
	LineTo(pDC->m_hDC,	nX+12,nY+6);	

	MoveToEx(pDC->m_hDC,nX+4, nY+7, &point);
	LineTo(pDC->m_hDC,	nX+12,nY+7);	

	MoveToEx(pDC->m_hDC,nX+4, nY+8, &point);
	LineTo(pDC->m_hDC,	nX+12,nY+8);	

	MoveToEx(pDC->m_hDC,nX+5, nY+9, &point);
	LineTo(pDC->m_hDC,	nX+11,nY+9);	

	MoveToEx(pDC->m_hDC,nX+6, nY+10, &point);
	LineTo(pDC->m_hDC,	nX+10,nY+10);	


	SelectObject(pDC->m_hDC, OldPen);			
	DeleteObject(pen);

	if (lpDrawItemStruct->itemState & ODS_SELECTED || lpDrawItemStruct->itemData == 1)
	{
		// lets draw the black dot
		rgb = m_ForeColor;
		pen = ::CreatePen(PS_SOLID, 1, rgb);
		OldPen = SelectObject(pDC->m_hDC, pen);
		
		MoveToEx(pDC->m_hDC,nX+7, nY+5, &point);
		LineTo(pDC->m_hDC,	nX+9, nY+5);		
		
		MoveToEx(pDC->m_hDC,nX+6, nY+6, &point);
		LineTo(pDC->m_hDC,	nX+10,nY+6);		
				
		MoveToEx(pDC->m_hDC,nX+6, nY+7, &point);
		LineTo(pDC->m_hDC,	nX+10,nY+7);		
		
		MoveToEx(pDC->m_hDC,nX+7, nY+8, &point);
		LineTo(pDC->m_hDC,	nX+9, nY+8);		
		
		
		SelectObject(pDC->m_hDC, OldPen);			
		DeleteObject(pen);
	}
	
	// Restore State of context
	pDC->RestoreDC(nIndexDC);
	
}

void COptionListBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	lpMeasureItemStruct->itemHeight = m_RowHeight;
}
