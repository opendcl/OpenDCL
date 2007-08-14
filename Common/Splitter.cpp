// Splitter.cpp : implementation file
//

#include "stdafx.h"
#include "Splitter.h"


/////////////////////////////////////////////////////////////////////////////
// CSplitter

CSplitter::CSplitter()
{
	m_nStyle = 0;
}

CSplitter::~CSplitter()
{
}


BEGIN_MESSAGE_MAP(CSplitter, CStatic)
	//{{AFX_MSG_MAP(CSplitter)
	ON_WM_PAINT()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSplitter message handlers

BOOL CSplitter::Create(int nStyle, const RECT& rect, CWnd* pParentWnd, UINT nID) 
{	
	return CStatic::Create(
		CString(),
		WS_CHILD|WS_VISIBLE,
		rect,
		pParentWnd,
		nID);
}

void CSplitter::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	CBrush CellBrush;	
	CellBrush.CreateSysColorBrush(COLOR_BTNFACE);

	CRect rcThis;
	GetWindowRect(&rcThis);
	CRect rcCell(0,0,rcThis.Width(), rcThis.Height());

	switch (m_nStyle)
	{
	case Splitter_DoubleRaised:
		{
		// draw the Window background for the cell				
		dc.FillRect(rcCell, &CellBrush);
		
		if (rcCell.Width() >= rcCell.Height())
		{
			CRect rc1 = rcCell;
			rc1.bottom = rc1.top + 3;
			CRect rc2 = rc1;
			rc2.top = rc1.top + 3;
			rc2.bottom = rc2.top + 3;

			// draw the solid rectangle
			::DrawEdge(dc.m_hDC, &rc1, BDR_RAISEDINNER, BF_RECT);		
			// draw the solid rectangle
			::DrawEdge(dc.m_hDC, &rc2, BDR_RAISEDINNER, BF_RECT);		
		}
		else
		{
			CRect rc1 = rcCell;
			rc1.right = rc1.left + 3;
			CRect rc2 = rc1;
			rc2.left = rc1.left + 3;
			rc2.right = rc2.left + 3;

			// draw the solid rectangle
			::DrawEdge(dc.m_hDC, &rc1, BDR_RAISEDINNER, BF_RECT);		
			// draw the solid rectangle
			::DrawEdge(dc.m_hDC, &rc2, BDR_RAISEDINNER, BF_RECT);		
		}
		break;
		
		}
	case Splitter_Raised:
		{
		// draw the solid rectangle
		::DrawEdge(dc.m_hDC, &rcCell, BDR_RAISEDINNER, BF_RECT);
		
		rcCell.DeflateRect(1,1);
		
		// draw the Window background for the cell				
		dc.FillRect(rcCell, &CellBrush);
		break;
		}
	case Splitter_Sunken:
		{
		// draw the Window background for the cell				
		dc.FillRect(rcCell, &CellBrush);
		
		CRect rc1 = rcCell;
		if (rcCell.Width() >= rcCell.Height())
		{
			rc1.top++; 
			rc1.top++; 
			rc1.left--;
			rc1.left--; 
			rc1.right++;
			rc1.right++;
			rc1.bottom++; 
			rc1.bottom++; 
		}
		else
		{
			rc1.top--; 
			rc1.top--; 
			rc1.left++;
			rc1.left++; 
			rc1.right++;
			rc1.right++;
			rc1.bottom++; 
			rc1.bottom++; 
		}
		
		// draw the solid rectangle
		::DrawEdge(dc.m_hDC, &rc1, EDGE_ETCHED, BF_RECT);
		

		rcCell.top = rc1.bottom+1;
		
		// draw the Window background for the cell				
		dc.FillRect(rcCell, &CellBrush);
		break;
		}
	default:
		// draw the Window background for the cell				
		dc.FillRect(rcCell, &CellBrush);
		break;
	}
	
	CellBrush.DeleteObject();

	// Do not call CWnd::OnPaint() for painting messages
}

int CSplitter::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CStatic::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	return 0;
}
