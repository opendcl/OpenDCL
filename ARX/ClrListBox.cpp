// ClrListBox.cpp : implementation file
//

#include "stdafx.h"
#include "ClrListBox.h"
#include "AcadColorTable.h"


/////////////////////////////////////////////////////////////////////////////
// CClrListBox

CClrListBox::CClrListBox()
{
	m_pStaticBrush = new CBrush();
	//m_pStaticBrush->CreateStockObject(HOLLOW_BRUSH);	
	m_pStaticBrush->CreateSolidBrush(GetRGBColor(-6));
	m_ForeColor = GetRGBColor(-19);

}

CClrListBox::~CClrListBox()
{
	if (m_pStaticBrush)
		delete m_pStaticBrush;
	
}


BEGIN_MESSAGE_MAP(CClrListBox, CAcUiListBox)
	//{{AFX_MSG_MAP(CClrListBox)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClrListBox message handlers

void CClrListBox::SetAcadColor(long nColor)
{
	if (m_pStaticBrush)
		delete m_pStaticBrush;
	m_pStaticBrush = new CBrush();
	m_BackColor = GetRGBColor(nColor);
	m_pStaticBrush->CreateSolidBrush(m_BackColor);
	
}


void CClrListBox::SetForeColor(long nColor)
{
	m_ForeColor = GetRGBColor(nColor);
}



HBRUSH CClrListBox::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	if (!IsWindowEnabled())
	{		
		return NULL;
	}
	
	pDC->SetBkMode(TRANSPARENT);	
	pDC->SetBkColor(m_BackColor);	
	pDC->SetTextColor(m_ForeColor);
	pDC->SelectObject(m_pStaticBrush);
	return (HBRUSH)(m_pStaticBrush->GetSafeHandle());
}

void CClrListBox::OnDestroy() 
{
	CAcUiListBox::OnDestroy();
	
	
}
