// CAcadolorListBox.cpp : implementation file
//

#include "stdafx.h"
#include "AcadColorListBox.h"
#include "AcadColorTable.h"


/////////////////////////////////////////////////////////////////////////////
// CAcadColorListBox

CAcadColorListBox::CAcadColorListBox()
: mbrushBackground( GetRGBColor(nButtonFace) )
{
	m_BackColor = GetRGBColor(nButtonFace);
	m_ForeColor = GetRGBColor(nButtonText);
}

CAcadColorListBox::~CAcadColorListBox()
{
	mbrushBackground.DeleteObject();
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
	m_BackColor = GetRGBColor(nColorArg);
	mbrushBackground.DeleteObject();
	mbrushBackground.CreateSolidBrush(m_BackColor);
}


void CAcadColorListBox::SetForeColor(long nColorArg)
{
	m_ForeColor = GetRGBColor(nColorArg);
}



HBRUSH CAcadColorListBox::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	if (!IsWindowEnabled())
		return NULL;
	pDC->SetBkMode(TRANSPARENT);	
	pDC->SetTextColor(m_ForeColor);
	return mbrushBackground;	
}

void CAcadColorListBox::OnDestroy() 
{
	CListBox::OnDestroy();
}
