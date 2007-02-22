// TabPage.cpp : implementation file
//

#include "stdafx.h"
#include "TabPage.h"


/////////////////////////////////////////////////////////////////////////////
// CTabPage property page

CTabPage::CTabPage( CDclFormObject* pSourceForm, CTabCtrl* pTabCtrl, CRect rectPane, UINT& nId )
: CDialog(CTabPage::IDD)
, mpSourceForm( pSourceForm )
, mControlPane( pSourceForm, this )
{
	Create(CTabPage::IDD, pTabCtrl);
	MoveWindow(rectPane);
	mControlPane.SetPanePos( CRect( 0, 0, rectPane.Width(), rectPane.Height() ), false );
	mControlPane.CreateControls( pSourceForm, nId );
}

CTabPage::~CTabPage()
{

}

void CTabPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTabPage, CDialog)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabPage message handlers

BOOL CTabPage::PreTranslateMessage(MSG* pMsg) 
{
	// allow sheet to translate Ctrl+Tab, Shift+Ctrl+Tab,
	//  Ctrl+PageUp, and Ctrl+PageDown
	if (pMsg->message == WM_KEYDOWN &&
		(pMsg->wParam == VK_TAB || pMsg->wParam == VK_PRIOR || pMsg->wParam == VK_NEXT))
	{
		if (::IsDialogMessage(m_hWnd, pMsg))
			return TRUE;
	}

	return CWnd::PreTranslateMessage(pMsg);
}

BOOL CTabPage::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
	//return CDialog::OnEraseBkgnd(pDC);
}

void CTabPage::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialog::OnPaint() for painting messages
}
