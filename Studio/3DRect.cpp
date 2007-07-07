// 3DRect.cpp : implementation file
//

#include "stdafx.h"
#include "3DRect.h"


/////////////////////////////////////////////////////////////////////////////
// C3DRect

C3DRect::C3DRect()
{
	m_ClassName = AfxRegisterWndClass(CS_VREDRAW | CS_HREDRAW,
      NULL,
      NULL,
      NULL);

}

C3DRect::~C3DRect()
{
}


BEGIN_MESSAGE_MAP(C3DRect, CWnd)
	//{{AFX_MSG_MAP(C3DRect)
	ON_WM_PAINT()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// C3DRect message handlers

BOOL C3DRect::Create(int nStyle, const RECT& rect, CWnd* pParentWnd, UINT nID) 
{	
	m_BrdStyle = nStyle;
	return CWnd::Create(
		m_ClassName,
		m_ClassName,
		WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS,
		rect,
		pParentWnd,
		nID,
		NULL);
}

void C3DRect::OnPaint() 
{
	//CPaintDC dc(this); // device context for painting
	PAINTSTRUCT ps; 
	
	
	CDC* pdc = BeginPaint(&ps);
    
	
	CBrush *pCellBrush = new CBrush;	
	pCellBrush->CreateSysColorBrush(COLOR_BTNFACE);
			
	
	CRect rcThis;
	CWnd::GetWindowRect(&rcThis);

	
	CRect rcCell(0,0,rcThis.Width(), rcThis.Height());
	// draw the Window background for the cell				
	pdc->FillRect(rcCell, pCellBrush);
	// delete the brush
	pCellBrush->DeleteObject();
	delete pCellBrush;

	EndPaint(&ps);
	
	// Do not call CWnd::OnPaint() for painting messages
}

int C3DRect::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	//CWnd::GetDC()->SetBkMode(TRANSPARENT);	
	return 0;
}
