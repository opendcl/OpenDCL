// ColorButton.cpp : implementation file
//

#include "stdafx.h"
#include "ColorButton.h"
#include "AcadColorTable.h"


/////////////////////////////////////////////////////////////////////////////
// CClrButton

CClrButton::CClrButton()
: mbrushBackground( GetRGBColor(-16) )
{
	m_BackColor = GetRGBColor(-16);
	m_ForeColor = GetRGBColor(-19);
}

CClrButton::~CClrButton()
{
	mbrushBackground.DeleteObject();
}


BEGIN_MESSAGE_MAP(CClrButton, CButton)
	ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClrButton message handlers


void CClrButton::SetAcadColor(long nColor)
{
	m_BackColor = GetRGBColor(nColor);
	mbrushBackground.DeleteObject();
	mbrushBackground.CreateSolidBrush(m_BackColor);
}


void CClrButton::SetForeColor(long nColor)
{
	m_ForeColor = GetRGBColor(nColor);
}


HBRUSH CClrButton::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	if (!IsWindowEnabled())
		return NULL;
	
	pDC->SetBkMode(TRANSPARENT);	
	pDC->SetBkColor(m_BackColor);	
	pDC->SetTextColor(m_ForeColor);
	return mbrushBackground;
}
