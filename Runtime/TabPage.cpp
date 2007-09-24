// TabPage.cpp : implementation file
//

#include "stdafx.h"
#include "TabPage.h"


/////////////////////////////////////////////////////////////////////////////
// CTabPage property page

CTabPage::CTabPage( TDclFormPtr pSourceForm, CTabCtrl* pTabCtrl, CRect rectPane, UINT& nId )
: CDialog(CTabPage::IDD, pTabCtrl)
, mpSourceForm( pSourceForm )
, mControlPane( pSourceForm, this )
{
	Create(CTabPage::IDD, pTabCtrl);
	MoveWindow(rectPane);
	mControlPane.SetPanePos( CRect( 0, 0, rectPane.Width(), rectPane.Height() ), false );
	mControlPane.CreateControls( nId );
}

CTabPage::~CTabPage()
{

}


BEGIN_MESSAGE_MAP(CTabPage, CDialog)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabPage message handlers

BOOL CTabPage::OnEraseBkgnd(CDC* pDC)
{
	return CDialog::OnEraseBkgnd(pDC);
	//return TRUE;
}
