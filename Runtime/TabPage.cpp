// TabPage.cpp : implementation file
//

#include "stdafx.h"
#include "TabPage.h"


/////////////////////////////////////////////////////////////////////////////
// CTabPage dialog

BEGIN_MESSAGE_MAP(CTabPage, CDialog)
	ON_WM_SIZE()
END_MESSAGE_MAP()

CTabPage::CTabPage( TDclFormPtr pSourceForm, CTabCtrl* pTabCtrl, CRect rectPane, UINT& nId )
: CDialog(CTabPage::IDD, pTabCtrl)
, CDialogObject( pSourceForm, &mControlPane, this )
, mControlPane( pSourceForm, this )
{
	mbIgnoreSizing = true;
	CDialog::Create( CTabPage::IDD, pTabCtrl );
	ShowWindow( SW_HIDE );
	MoveWindow( &rectPane) ;
	mbIgnoreSizing = false;
	mpTemplate->SetLongProperty( Prop::Width, rectPane.Width() );
	mpTemplate->SetLongProperty( Prop::Height, rectPane.Height() );
	ApplyPropertiesEnum();
	mControlPane.CreateControls( nId );
	mControlPane.RecalcLayout();
}

CTabPage::~CTabPage()
{
}

/////////////////////////////////////////////////////////////////////////////
// CTabPage message handlers

BOOL CTabPage::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch( LOWORD(wParam) )
	{
	case IDCANCEL:
	case IDOK:
		return FALSE;
	}
	return __super::OnCommand( wParam, lParam );
}

void CTabPage::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);
	if( mbIgnoreSizing )
		return;
	mpTemplate->SetLongProperty( Prop::Width, cx );
	mpTemplate->SetLongProperty( Prop::Height, cy );
	mpControlPane->RecalcLayout();
}
