// SpreadSheetHdr.cpp : implementation file
//

#include "stdafx.h"
#include "SpreadSheetHdr.h"
#include "SpreadSheet.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CSpreadSheetHdr

CSpreadSheetHdr::CSpreadSheetHdr()
{
	m_pParent = NULL;
}

CSpreadSheetHdr::~CSpreadSheetHdr()
{
}


BEGIN_MESSAGE_MAP(CSpreadSheetHdr, CHeaderCtrl)
	//{{AFX_MSG_MAP(CSpreadSheetHdr)
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSpreadSheetHdr message handlers

void CSpreadSheetHdr::OnLButtonDown(UINT nFlags, CPoint point) 
{
	try
	{
		if (m_pParent != NULL)
			m_pParent->HideEditControls();
	}
	catch(...)
	{
	}

	CHeaderCtrl::OnLButtonDown(nFlags, point);
}
