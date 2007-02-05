// TabPage.cpp : implementation file
//

#include "stdafx.h"
#include "TabPage.h"


/////////////////////////////////////////////////////////////////////////////
// CTabPage property page

CTabPage::CTabPage( CDclFormObject* pSourceForm, CWnd* pHostDlg )
: CDialog(CTabPage::IDD)
, mpSourceForm( pSourceForm )
, mControlPane( pSourceForm, this )
{
	//{{AFX_DATA_INIT(CTabPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

}

CTabPage::~CTabPage()
{

}

void CTabPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabPage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTabPage, CDialog)
	//{{AFX_MSG_MAP(CTabPage)
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabPage message handlers

void CTabPage::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);	
}

void CTabPage::OnOK()
{
}
void CTabPage::OnCancel()
{
}

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
