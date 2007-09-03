// ListCtrlEx.cpp : implementation file
//

#include "stdafx.h"
#include "ListCtrlEx.h"
#include "PropertyIds.h"
#include "PropertyObject.h"
#include "DclControlObject.h"


/////////////////////////////////////////////////////////////////////////////
// CListCtrlEx

CListCtrlEx::CListCtrlEx()
{
}

CListCtrlEx::~CListCtrlEx()
{
}


BEGIN_MESSAGE_MAP(CListCtrlEx, CPictureBox)
	//{{AFX_MSG_MAP(CListCtrlEx)
	ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListCtrlEx message handlers

int CListCtrlEx::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CPictureBox::OnCreate(lpCreateStruct) == -1)
		return -1;
	CRect rc(0,0,10,10);

	m_Child.Create(
		WS_CHILD|WS_VISIBLE |WS_CLIPSIBLINGS|HDS_BUTTONS,
		rc, 
		this,
		99);
	
	return 0;
}

void CListCtrlEx::OnSize(UINT nType, int cx, int cy) 
{
	CPictureBox::OnSize(nType, cx, cy);
	
	CRect rc;
	GetClientRect(&rc);

	rc.bottom = 17;

	m_Child.MoveWindow(rc, TRUE);
	
}


void CListCtrlEx::SetupColumns(CDclControlObject *pControl)
{	
	while (m_Child.GetItemCount() > 0)
		m_Child.DeleteItem(0);

	TPropertyPtr pColCaptions = pControl->GetPropertyObject(Prop::ColumnCaptions);
	TPropertyPtr pColWidths = pControl->GetPropertyObject(Prop::ColumnWidths);
	TPropertyPtr pColAlign = pControl->GetPropertyObject(Prop::ColumnAlignments);
	TPropertyPtr pColImage = pControl->GetPropertyObject(Prop::ColumnImages);

	for (size_t i = 0; i < pColCaptions->size(); i++)
	{				
		HDITEM  hdi;
		hdi.mask = HDI_TEXT | HDI_WIDTH | HDI_FORMAT ;
		hdi.cxy = pColWidths->GetIntArrayPtr()->at(i);
		hdi.fmt = HDF_STRING;

		if (pColAlign->GetIntArrayPtr()->at(i) == 0)
			hdi.fmt = hdi.fmt | HDF_LEFT;
		else if (pColAlign->GetIntArrayPtr()->at(i) == 2)
			hdi.fmt = hdi.fmt | HDF_CENTER;
		else if (pColAlign->GetIntArrayPtr()->at(i) == 1)
			hdi.fmt = hdi.fmt | HDF_RIGHT;

		if (pColImage->GetIntArrayPtr()->at(i) > -1)
		{
			hdi.mask = hdi.mask | HDI_IMAGE;
			hdi.fmt = hdi.fmt | HDF_IMAGE;
			hdi.iImage = pColImage->GetIntArrayPtr()->at(i);			
		}
		CString sCaption = pColCaptions->GetStringItem(i);
		hdi.pszText = sCaption.LockBuffer();
		m_Child.InsertItem(i, &hdi);
	}
}

