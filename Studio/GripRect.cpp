// GripRect.cpp : implementation file
//

#include "stdafx.h"
#include "OpenDCL.h"
#include "GripRect.h"
#include "OpenDCLView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGripRect

CGripRect::CGripRect()
{
	m_ClassName = AfxRegisterWndClass(CS_VREDRAW | CS_HREDRAW,
      NULL,
      NULL,
      NULL);

	m_hCursor = NULL;
	m_bShowAsSelected = false;
	m_bResizing = false;
	m_nQuadrant = -1;
}

CGripRect::~CGripRect()
{
	DeleteObject(m_hCursor);
}


BEGIN_MESSAGE_MAP(CGripRect, CWnd)
	//{{AFX_MSG_MAP(CGripRect)
	ON_WM_PAINT()	
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()	
	ON_WM_MOUSEMOVE()
	ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CGripRect message handlers

BOOL CGripRect::Create(const RECT& rect, CWnd* pParentWnd, UINT nID) 
{
	BOOL bReturn = CWnd::Create(
		m_ClassName,
		_T(""), 
		WS_VISIBLE|WS_CHILD|WS_CLIPSIBLINGS,
		rect,
		pParentWnd,
		nID,
		NULL);
	m_bResizing = false;
	
	return bReturn;
}
void CGripRect::SetGripCursor(LPCTSTR lpszCursorName)
{
	m_hCursor = AfxGetApp()->LoadStandardCursor(lpszCursorName);
	
}
void CGripRect::OnPaint() 
{
	PAINTSTRUCT ps; 
	CDC* pdc = BeginPaint(&ps);
	//CDC *pdc = GetDC();
	RePaint(pdc->m_hDC);
	
	EndPaint(&ps);
	//pdc->Detach();

}
void CGripRect::Refresh()
{	
	RePaint(GetDC()->m_hDC);
}
void CGripRect::RePaint(HDC hdc)
{
	CRect rcThis;
	COLORREF rgb;
	GetWindowRect(&rcThis);

	CRect rcRect(0, 0, rcThis.Width(), rcThis.Height());
	
	if (!m_bShowAsSelected)
		rgb = GetSysColor(COLOR_HIGHLIGHTTEXT);
	else
		rgb = GetSysColor(COLOR_HIGHLIGHT);

	HGDIOBJ pen = ::CreatePen(PS_SOLID, 1, rgb);
	HGDIOBJ OldPen = SelectObject(hdc, pen);
		
	::Rectangle(hdc, rcRect.left, rcRect.top, rcRect.right, rcRect.bottom);

	SelectObject(hdc, OldPen);			
	DeleteObject(pen);

	
	// draw the inside area

	if (!m_bShowAsSelected)
		::SetBkColor(hdc, GetSysColor(COLOR_HIGHLIGHT));
	else
		::SetBkColor(hdc, GetSysColor(COLOR_HIGHLIGHTTEXT));
	
	CRect rcCell(1, 1, rcThis.Width() - 1, rcThis.Height() - 1);
	// draw the Window background for the cell				
	::ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &rcCell, NULL, 0, NULL);
	

}

void CGripRect::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (m_hCursor != NULL && IsWindowVisible() == TRUE)
		::SetCursor(m_hCursor);					

	if (m_bResizing == true)
	{
		COpenDCLView *pParent = (COpenDCLView*)GetParent();
		CRect rcThis;
		GetWindowRect(&rcThis);
		pParent->ScreenToClient(rcThis);
		point.x += rcThis.left;
		point.y += rcThis.top;
		pParent->DragResized(m_nQuadrant, point);
	}
	
	CWnd::OnMouseMove(nFlags, point);
}

void CGripRect::OnLButtonUp(UINT nFlags, CPoint point) 
{
	COpenDCLView *pParent = (COpenDCLView*)GetParent();
	CRect rcThis;
	GetWindowRect(&rcThis);
	pParent->ScreenToClient(rcThis);

	CPoint ptThis(
		rcThis.left + point.x,
		rcThis.top + point.y);

	pParent->OnLButtonUp(nFlags, point);
	m_bResizing = false;
}
void CGripRect::OnLButtonDown(UINT nFlags, CPoint point) 
{
	COpenDCLView *pParent = (COpenDCLView*)GetParent();
	pParent->SetupDragResize(m_nQuadrant);
	m_bResizing = true;
}



BOOL CGripRect::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if (m_hCursor != NULL && IsWindowVisible() == TRUE)
	{
		::SetCursor(m_hCursor);					
		return FALSE;
	}
	
	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}
