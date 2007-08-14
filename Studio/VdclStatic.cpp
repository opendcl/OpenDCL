// VdclStatic.cpp : implementation file
//

#include "stdafx.h"
#include "VdclStatic.h"
#include "AcadColorTable.h"


/////////////////////////////////////////////////////////////////////////////
// VdclStatic

VdclStatic::VdclStatic()
: mbrushBackground( GetRGBColor(nButtonFace) )
{
	m_ForeColor = GetRGBColor(nButtonText);
}

VdclStatic::~VdclStatic()
{
	mbrushBackground.DeleteObject();
}


BEGIN_MESSAGE_MAP(VdclStatic, CStatic)
	//{{AFX_MSG_MAP(VdclStatic)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void VdclStatic::SetAcadColor(long nColorArg)
{
	mbrushBackground.DeleteObject();
	mbrushBackground.CreateSolidBrush(GetRGBColor(nColorArg));
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
		return mbrushBackground;	
	}

	return NULL;
}

void VdclStatic::OnDestroy() 
{
	CStatic::OnDestroy();
}
