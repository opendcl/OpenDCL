// ColorPatch.cpp : implementation file
//

#include "stdafx.h"
#include "ColorPatch.h"


/////////////////////////////////////////////////////////////////////////////
// CColorPatch

CColorPatch::CColorPatch()
{
	m_color = RGB(0,0,0);
}

CColorPatch::~CColorPatch()
{
}


BEGIN_MESSAGE_MAP(CColorPatch, CStatic)
	//{{AFX_MSG_MAP(CColorPatch)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorPatch message handlers

void CColorPatch::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	

	// setup the CRect for FillRect
	CRect rc;
	
	GetClientRect(&rc);

	// draw the solid rectangle
	::SetBkColor(dc.m_hDC, m_color);
	::ExtTextOut(dc.m_hDC, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);	
	// Do not call CStatic::OnPaint() for painting messages
}
