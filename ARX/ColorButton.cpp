// ColorButton.cpp : implementation file
//

#include "stdafx.h"
#include "ColorButton.h"
#include "AcadColorTable.h"


/////////////////////////////////////////////////////////////////////////////
// CClrButton

CClrButton::CClrButton()
{
	m_pStaticBrush = new CBrush();
	m_pStaticBrush->CreateSolidBrush(GetRGBColor(-16));
	m_ForeColor = GetRGBColor(-19);
}

CClrButton::~CClrButton()
{
	if (m_pStaticBrush)
		delete m_pStaticBrush;
	
}


BEGIN_MESSAGE_MAP(CClrButton, CButton)
	//{{AFX_MSG_MAP(CClrButton)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClrButton message handlers


void CClrButton::SetAcadColor(long nColor)
{
	if (m_pStaticBrush)
		delete m_pStaticBrush;
	m_pStaticBrush = new CBrush();
	m_BackColor = GetRGBColor(nColor);
	m_pStaticBrush->CreateSolidBrush(m_BackColor);
	
}


void CClrButton::SetForeColor(long nColor)
{
	m_ForeColor = GetRGBColor(nColor);
}


HBRUSH CClrButton::CtlColor(CDC* pDC, UINT nCtlColor) 
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

void CClrButton::OnDestroy() 
{
	CButton::OnDestroy();
	
	
}
