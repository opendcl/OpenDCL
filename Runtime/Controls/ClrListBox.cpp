// ClrListBox.cpp : implementation file
//

#include "stdafx.h"
#include "ClrListBox.h"
#include "AcadColorTable.h"


/////////////////////////////////////////////////////////////////////////////
// CClrListBox

CClrListBox::CClrListBox()
: mbrushBackground( GetRGBColor(-6) )
{
	m_BackColor = GetRGBColor(-6);
	m_ForeColor = GetRGBColor(-19);
}

CClrListBox::~CClrListBox()
{
	mbrushBackground.DeleteObject();
}


BEGIN_MESSAGE_MAP(CClrListBox, CAcUiListBox)
	ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClrListBox message handlers

void CClrListBox::SetAcadColor(long nColor)
{
	m_BackColor = GetRGBColor(nColor);
	mbrushBackground.DeleteObject();
	mbrushBackground.CreateSolidBrush(m_BackColor);
}

void CClrListBox::SetForeColor(long nColor)
{
	m_ForeColor = GetRGBColor(nColor);
}

HBRUSH CClrListBox::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	if (!IsWindowEnabled())
		return NULL;
	
	pDC->SetBkMode(TRANSPARENT);	
	pDC->SetBkColor(m_BackColor);	
	pDC->SetTextColor(m_ForeColor);
	return mbrushBackground;
}
