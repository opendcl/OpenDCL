// VdclStatic.cpp : implementation file
//

#include "stdafx.h"
#include "VdclStatic.h"
#include "AcadColorTable.h"


/////////////////////////////////////////////////////////////////////////////
// VdclStatic

VdclStatic::VdclStatic()
{
	m_pStaticBrush = new CBrush();
	m_pStaticBrush->CreateSolidBrush(GetRGBColor(nButtonFace));
	m_ForeColor = GetRGBColor(nButtonText);
}

VdclStatic::~VdclStatic()
{

}


BEGIN_MESSAGE_MAP(VdclStatic, CStatic)
	//{{AFX_MSG_MAP(VdclStatic)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void VdclStatic::SetAcadColor(long nColorArg)
{
	if (m_pStaticBrush)
		delete m_pStaticBrush;
	m_pStaticBrush = new CBrush();
	m_pStaticBrush->CreateSolidBrush(GetRGBColor(nColorArg));
}


void VdclStatic::SetForeColor(long nColorArg)
{
	m_ForeColor = GetRGBColor(nColorArg);
}

/////////////////////////////////////////////////////////////////////////////
// VdclStatic message handlers

HBRUSH VdclStatic::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	if (nCtlColor == CTLCOLOR_STATIC)
	{	
		pDC->SetBkMode(TRANSPARENT);	
		pDC->SetTextColor(m_ForeColor);
		pDC->SelectObject(m_pStaticBrush);
		return *m_pStaticBrush;	
	}

	return NULL;
}

void VdclStatic::OnDestroy() 
{
	CStatic::OnDestroy();
	
	if (m_pStaticBrush)
	delete m_pStaticBrush;	
}
