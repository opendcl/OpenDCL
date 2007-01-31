// CAcadolorListBox.cpp : implementation file
//

#include "stdafx.h"
#include "AcadColorListBox.h"
#include "AcadColorTable.h"


/////////////////////////////////////////////////////////////////////////////
// CAcadColorListBox

CAcadColorListBox::CAcadColorListBox()
{
	m_pStaticBrush = new CBrush();
	m_BackColor = GetRGBColor(nButtonFace);
	m_pStaticBrush->CreateSolidBrush(m_BackColor);
	m_ForeColor = GetRGBColor(nButtonText);

}

CAcadColorListBox::~CAcadColorListBox()
{
	if (m_pStaticBrush)
		delete m_pStaticBrush;
	
}


BEGIN_MESSAGE_MAP(CAcadColorListBox, CListBox)
	//{{AFX_MSG_MAP(CAcadColorListBox)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAcadColorListBox message handlers

void CAcadColorListBox::SetAcadColor(long nColorArg)
{
	if (m_pStaticBrush)
		delete m_pStaticBrush;

	m_BackColor = GetRGBColor(nColorArg);
	m_pStaticBrush = new CBrush();
	m_pStaticBrush->CreateSolidBrush(m_BackColor);
}


void CAcadColorListBox::SetForeColor(long nColorArg)
{
	m_ForeColor = GetRGBColor(nColorArg);
}



HBRUSH CAcadColorListBox::CtlColor(CDC* pDC, UINT nCtlColor) 
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

void CAcadColorListBox::OnDestroy() 
{
	CListBox::OnDestroy();
}
