// FileDlgCtrls.cpp : implementation file
//

#include "stdafx.h"
#include "FileDlgCtrls.h"
#include "Resource.h"


/////////////////////////////////////////////////////////////////////////////
// CFileDlgCtrls

CFileDlgCtrls::CFileDlgCtrls()
{
	HINSTANCE hInstResource = AfxFindResourceHandle(MAKEINTRESOURCE(IDI_OPENFOLDER), RT_GROUP_ICON);
    HMODULE hRes = AfxGetApp()->m_hInstance;
	// Set icon when the mouse is IN the button
	m_TopBitmap = (HBITMAP)::LoadImage(hRes, MAKEINTRESOURCE(IDB_FDTOP), IMAGE_BITMAP, 0, 0, 0);
	m_BottomBitmap = (HBITMAP)::LoadImage(hRes, MAKEINTRESOURCE(IDB_FDBOTTOM), IMAGE_BITMAP, 0, 0, 0);
  	
}

CFileDlgCtrls::~CFileDlgCtrls()
{
	if (m_TopBitmap != NULL)
	{
		DeleteObject(m_TopBitmap);
		m_TopBitmap = NULL;
	}
	if (m_BottomBitmap != NULL)
	{
		DeleteObject(m_BottomBitmap);
		m_BottomBitmap = NULL;
	}
}


BEGIN_MESSAGE_MAP(CFileDlgCtrls, CStatic)
	//{{AFX_MSG_MAP(CFileDlgCtrls)
	ON_WM_PAINT()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CFileDlgCtrls message handlers

void CFileDlgCtrls::OnPaint() 
{
	//CPaintDC dc(this); // device context for painting
	PAINTSTRUCT ps; 
	
	CDC* pdc = BeginPaint(&ps);
	
	CRect rcBounds;
	GetClientRect(&rcBounds);

	COLORREF BackColor = RGB(192,192,192);
	COLORREF FillColor = RGB(255,255,255);
	CBrush StaticBrush;
	StaticBrush.CreateSolidBrush(BackColor);
	
	pdc->FillRect(rcBounds, &StaticBrush);

	CRect rc;
	rc.left = 0;
	rc.top = 0;
	rc.right = 415;
	rc.bottom = 27;
	pdc->DrawState(rc.TopLeft(),
				rc.Size(), 
				m_TopBitmap, 
				DSS_NORMAL, 
				(HBRUSH)NULL);

	rc.left = 6;
	rc.top = 31;
	rc.right = 414;
	rc.bottom = 171;
	
	// draw the solid rectangle
	::DrawEdge(pdc->m_hDC, &rc, EDGE_SUNKEN, BF_RECT);

	CBrush FillBrush;
	FillBrush.CreateSolidBrush(FillColor);

	rc.DeflateRect(2,2,2,2);
	pdc->FillRect(rc, &FillBrush);

	rc.left = 0;
	rc.top = 175;
	rc.right = 411;
	rc.bottom = rc.top+84;
	pdc->DrawState(rc.TopLeft(),
				rc.Size(), 
				m_BottomBitmap, 
				DSS_NORMAL, 
				(HBRUSH)NULL);

	EndPaint(&ps);
	
}

void CFileDlgCtrls::OnSize(UINT nType, int cx, int cy) 
{
	CStatic::OnSize(nType, cx, cy);
	
	if (cx != 416 && cy != 258)
	{
		cx = 416;
		cy = 258;

		CRect rcThis(0,0,cx,cy);
		MoveWindow(rcThis, TRUE);
	
		return;
	}
}
