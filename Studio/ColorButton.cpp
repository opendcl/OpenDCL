// ColorButton.cpp : implementation file
//

#include "stdafx.h"
#include "ColorButton.h"
#include "AcadColorTable.h"


/////////////////////////////////////////////////////////////////////////////
// CColorButton

CColorButton::CColorButton()
{
	m_pStaticBrush = new CBrush();
	m_pStaticBrush->CreateSolidBrush(GetRGBColor(-16));
	m_ForeColor = GetRGBColor(-19);
	
}

CColorButton::~CColorButton()
{
	if (m_pStaticBrush)
		delete m_pStaticBrush;
	
}


BEGIN_MESSAGE_MAP(CColorButton, CButton)
	//{{AFX_MSG_MAP(CColorButton)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorButton message handlers


void CColorButton::SetAcadColor(long nColorArg)
{
	if (m_pStaticBrush)
		delete m_pStaticBrush;
	m_pStaticBrush = new CBrush();
	m_pStaticBrush->CreateSolidBrush(GetRGBColor(nColorArg));
}


void CColorButton::SetForeColor(long nColorArg)
{
	m_ForeColor = GetRGBColor(nColorArg);
}


HBRUSH CColorButton::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	if (!IsWindowEnabled())
	{
		return NULL;
	}

	pDC->SetBkMode(TRANSPARENT);	
	pDC->SetTextColor(m_ForeColor);
	pDC->SelectObject(m_pStaticBrush);
	return *m_pStaticBrush;	
}

void CColorButton::OnDestroy() 
{
	if (m_pStaticBrush)
	{
		delete m_pStaticBrush;
		m_pStaticBrush = NULL;
	}
	CButton::OnDestroy();
	
}
