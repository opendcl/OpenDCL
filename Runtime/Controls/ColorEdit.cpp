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
	m_BackColor = GetRGBColor(-6);
	m_DefBackColor = m_BackColor;
	m_pStaticBrush->CreateSolidBrush(m_BackColor);
	m_ForeColor = GetRGBColor(-19);
	m_DefForeColor = m_ForeColor;
	m_UseBackColor = true;

	m_DefaultBackColor = m_BackColor;
}

CColorEdit::~CColorEdit()
{	
	if (m_pStaticBrush)
		delete m_pStaticBrush;
	
}


BEGIN_MESSAGE_MAP(CColorEdit, CAcUiEdit)
	//{{AFX_MSG_MAP(CColorEdit)
	ON_WM_CTLCOLOR_REFLECT()	
	ON_WM_CTLCOLOR()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorEdit message handlers

void CColorEdit::SetAcadColor(long nColor)
{
	if (m_pStaticBrush)
		delete m_pStaticBrush;
	m_pStaticBrush = new CBrush();
	m_pStaticBrush->CreateSolidBrush(GetRGBColor(nColor));
	m_BackColor = GetRGBColor(nColor);
	
	if (m_DefBackColor == m_BackColor)
		m_UseBackColor = false;
	else
		m_UseBackColor = true;
}

void CColorEdit::SetBkColor(COLORREF lColor)
{
	if (m_pStaticBrush)
		delete m_pStaticBrush;
	m_pStaticBrush = new CBrush();
	m_pStaticBrush->CreateSolidBrush(lColor);
	m_BackColor = lColor;
	
	if (m_DefBackColor == m_BackColor)
		m_UseBackColor = false;
	else
		m_UseBackColor = true;
}

void CColorEdit::SetForeColor(COLORREF lColor)
{
	m_ForeColor = lColor;
	if (m_DefBackColor == m_BackColor)
		m_UseBackColor = false;
	else
		m_UseBackColor = true;
}

void CColorEdit::SetForeColor(long nColor)
{
	m_ForeColor = GetRGBColor(nColor);
	if (m_DefBackColor == m_BackColor)
		m_UseBackColor = false;
	else
		m_UseBackColor = true;
}

HBRUSH CColorEdit::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	if (!IsWindowEnabled() )
	{		
		return NULL;
	}
	
	//if (m_UseBackColor)
	pDC->SetBkColor(m_BackColor);	
	pDC->SetTextColor(m_ForeColor);
		
	pDC->SelectObject(m_pStaticBrush);
	return (HBRUSH)(m_pStaticBrush->GetSafeHandle());

	
}



HBRUSH CColorEdit::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	if(pWnd->m_hWnd == m_hWnd)
		return OnCtlColor(pDC, pWnd, nCtlColor);	// for dialogs
	pDC->SetBkColor(m_BackColor);	
	
	return CEdit::OnCtlColor(pDC, this, nCtlColor);		// send reflect message
}

void CColorEdit::OnDestroy() 
{
	CAcUiEdit::OnDestroy();
}