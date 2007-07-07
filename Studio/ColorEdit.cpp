// ColorEdit.cpp : implementation file
//

#include "stdafx.h"
#include "ColorEdit.h"
#include "AcadColorTable.h"


/////////////////////////////////////////////////////////////////////////////
// CColorEdit

CColorEdit::CColorEdit()
{
	m_pStaticBrush = new CBrush();
	
	m_BackColor = GetRGBColor(nButtonFace);
	m_ForeColor = GetRGBColor(nButtonText);
	m_pStaticBrush->CreateSolidBrush(m_BackColor);
	m_UseBackColor = true;
}

CColorEdit::~CColorEdit()
{
	if (m_pStaticBrush)
		delete m_pStaticBrush;
	
}


BEGIN_MESSAGE_MAP(CColorEdit, CEdit)
	//{{AFX_MSG_MAP(CColorEdit)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorEdit message handlers

void CColorEdit::SetAcadColor(long nColorArg)
{
	if (m_pStaticBrush)
		delete m_pStaticBrush;
	m_pStaticBrush = new CBrush();
	m_pStaticBrush->CreateSolidBrush(GetRGBColor(nColorArg));
	m_BackColor = GetRGBColor(nColorArg);
}



void CColorEdit::SetForeColor(long nColorArg)
{
	m_ForeColor = GetRGBColor(nColorArg);
}

HBRUSH CColorEdit::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	if (!IsWindowEnabled() || !m_UseBackColor)
	{
		return NULL;
	}

	pDC->SetBkColor(m_BackColor);
	pDC->SetTextColor(m_ForeColor);
	pDC->SelectObject(m_pStaticBrush);
	return *m_pStaticBrush;	
}

void CColorEdit::OnDestroy() 
{
	CEdit::OnDestroy();
	
	
}
