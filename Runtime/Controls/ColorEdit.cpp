// ColorEdit.cpp : implementation file
//

#include "stdafx.h"
#include "ColorEdit.h"
#include "AcadColorTable.h"


/////////////////////////////////////////////////////////////////////////////
// CColorEdit

CColorEdit::CColorEdit()
: mbrushBackground( GetRGBColor(-16) )
{
	m_BackColor = GetRGBColor(-6);
	m_ForeColor = GetRGBColor(-19);
}

CColorEdit::~CColorEdit()
{	
	mbrushBackground.DeleteObject();
}


BEGIN_MESSAGE_MAP(CColorEdit, CAcUiEdit)
	//{{AFX_MSG_MAP(CColorEdit)
	ON_WM_CTLCOLOR_REFLECT()	
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorEdit message handlers

void CColorEdit::SetAcadColor(long nColor)
{
	SetBkColor(GetRGBColor(nColor));
}

void CColorEdit::SetBkColor(COLORREF lColor)
{
	m_BackColor = lColor;
	mbrushBackground.DeleteObject();
	mbrushBackground.CreateSolidBrush(m_BackColor);
}

void CColorEdit::SetForeColor(COLORREF lColor)
{
	m_ForeColor = lColor;
}

void CColorEdit::SetForeColor(long nColor)
{
	m_ForeColor = GetRGBColor(nColor);
}

HBRUSH CColorEdit::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	if (!IsWindowEnabled() )
		return NULL;
	
	pDC->SetBkColor(m_BackColor);	
	pDC->SetTextColor(m_ForeColor);
	return mbrushBackground;
}

HBRUSH CColorEdit::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	if(pWnd->m_hWnd == m_hWnd)
		return __super::OnCtlColor(pDC, pWnd, nCtlColor);	// for dialogs
	pDC->SetBkColor(m_BackColor);	
	return __super::OnCtlColor(pDC, this, nCtlColor);		// send reflect message
}
