// GridCtrlHdr.cpp : implementation file
//

#include "stdafx.h"
#include "GridCtrlHdr.h"
#include "GridCtrl.h"


/////////////////////////////////////////////////////////////////////////////
// CGridCtrlHdr

CGridCtrlHdr::CGridCtrlHdr( CGridCtrl* pParent )
: mpParent( pParent )
{
}

CGridCtrlHdr::~CGridCtrlHdr()
{
}


BEGIN_MESSAGE_MAP(CGridCtrlHdr, CHeaderCtrl)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CGridCtrlHdr message handlers

void CGridCtrlHdr::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (mpParent != NULL)
		mpParent->HideEditControls();
	__super::OnLButtonDown(nFlags, point);
}
