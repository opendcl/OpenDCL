// ListHeader.cpp : implementation file
//

#include "stdafx.h"
#include "ListHeader.h"
#include "ColumnsPage.h"


/////////////////////////////////////////////////////////////////////////////
// CListHeader

CListHeader::CListHeader()
{
}

CListHeader::~CListHeader()
{
}


BEGIN_MESSAGE_MAP(CListHeader, CHeaderCtrl)
	//{{AFX_MSG_MAP(CListHeader)
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListHeader message handlers

void CListHeader::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CHeaderCtrl::OnLButtonUp(nFlags, point);

	CColumnsPage* pPage = (CColumnsPage*)GetParent()->GetParent();

	pPage->ResetWidths();
	
	
}

void CListHeader::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CHeaderCtrl::OnLButtonDown(nFlags, point);
	CColumnsPage* pPage = (CColumnsPage*)GetParent()->GetParent();
	
	for (int i=0; i<GetItemCount(); i++)
	{
		CRect rc;
		CHeaderCtrl::GetItemRect(i, rc);

		if (rc.PtInRect(point) == TRUE)
		{
			if (point.x >= rc.left && point.x >= rc.left+6)
				pPage->OnColumnHeaderClicked(i);
			
			if (point.x >= rc.right && point.x >= rc.right-6)
				pPage->OnColumnHeaderClicked(i);
			
			break;
		}
	}

	

}
