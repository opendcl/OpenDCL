// ZOrderPane.cpp : implementation file
//

#include "stdafx.h"
#include "ZOrderPane.h"
#include "Resource.h"
#include "StudioWorkspace.h"
#include "ControlManager.h"
#include "DclFormView.h"
#include "DialogControl.h"


/////////////////////////////////////////////////////////////////////////////
// CZOrderPane

CZOrderPane::CZOrderPane()
{
}

CZOrderPane::~CZOrderPane()
{
}

void CZOrderPane::OnActivateDlgObject( CStudioDialogObject* pDlgObject )
{
	mZOrderList.OnActivateDlgObject( pDlgObject );
	UpdateToolUI();
}

void CZOrderPane::OnActivateDclControl( TDclControlPtr pDclControl )
{
	mZOrderList.OnActivateDclControl( pDclControl );
	UpdateToolUI();
}

void CZOrderPane::UpdateToolUI()
{
	BOOL bEnable = mZOrderList.GetSelCount() > 0;
	mToolbar.EnableButton( ID_SENDTOFRONT, bEnable );
	mToolbar.EnableButton( ID_SENDTOBACK, bEnable );
	mToolbar.EnableButton( ID_MOVEZFRONTBY1, bEnable );
	mToolbar.EnableButton( ID_MOVEZBACKBY1, bEnable );
}


BEGIN_MESSAGE_MAP(CZOrderPane, CDialog)
	ON_WM_CREATE()
	ON_WM_SIZE()	
	ON_WM_DESTROY()
	ON_NOTIFY_RANGE( TTN_NEEDTEXTA, ID_SENDTOFRONT, ID_MOVEZBACKBY1, OnNeedTextA)
	ON_NOTIFY_RANGE( TTN_NEEDTEXTW, ID_SENDTOFRONT, ID_MOVEZBACKBY1, OnNeedTextW)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CZOrderPane message handlers

int CZOrderPane::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if( CDialog::OnCreate( lpCreateStruct ) == -1 )
		return -1;
	
	CRect rc( 0, 0, 2, 2 );
	if( !mZOrderList.Create( WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER, rc, this, 107 ) )
		return -1;

	DWORD dwStyle = WS_VISIBLE | WS_CHILD | CCS_NODIVIDER | CCS_NOPARENTALIGN      
			| CCS_ADJUSTABLE | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT | TBSTYLE_WRAPABLE;

	if( !mToolbar.Create( dwStyle, CRect( 0, 0, 2, 2 ), this, IDR_TOOLBOX ) )
		return -1;		// fail to create

	static TBBUTTON rTbButtons[] =
	{
		{0, ID_SENDTOFRONT, 0, BYTE(TBSTYLE_BUTTON)},
		{1, ID_SENDTOBACK, 0, BYTE(TBSTYLE_BUTTON)},
		{2, ID_MOVEZFRONTBY1, 0, BYTE(TBSTYLE_BUTTON)},
		{3, ID_MOVEZBACKBY1, 0, BYTE(TBSTYLE_BUTTON)},
	};
	mToolbar.AddBitmap( 4, IDR_ZTABORDER );
	mToolbar.SetBitmapSize( CSize( 16, 16 ) );
	VERIFY(mToolbar.AddButtons( _elements(rTbButtons), rTbButtons ));
	mToolbar.AutoSize();
	mToolbar.EnableWindow(TRUE);
	mToolbar.ShowWindow(TRUE);
	
	return 0;
}


void CZOrderPane::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

	mToolbar.MoveWindow(0,0,cx,cy,TRUE);

	CRect rc;
	if (IsWindow(mToolbar.m_hWnd))
		mToolbar.GetWindowRect(&rc);
	ScreenToClient(rc);
	// resize the ZOrder list 
	CRect rcZOrder(0,rc.bottom + 1,cx-2, cy);
	mZOrderList.MoveWindow(rcZOrder, TRUE);
}


/////////////////////////////////////////////////////////////////////////////
// CToolboxPane message handlers
void CZOrderPane::OnNeedTextW( UINT nID, NMHDR * pNotifyStruct, LRESULT * lResult )
{
	LPTOOLTIPTEXTW lpTTT = (LPTOOLTIPTEXTW)pNotifyStruct;
  lpTTT->lpszText = MAKEINTRESOURCEW(nID);
  lpTTT->hinst = AfxGetResourceHandle();
}

void CZOrderPane::OnNeedTextA( UINT nID, NMHDR * pNotifyStruct, LRESULT * lResult )
{
	LPTOOLTIPTEXTA lpTTT = (LPTOOLTIPTEXTA)pNotifyStruct;
  lpTTT->lpszText = MAKEINTRESOURCEA(nID);
  lpTTT->hinst = AfxGetResourceHandle();
}

BOOL CZOrderPane::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	ASSERT(pResult != NULL);
	NMHDR* pNMHDR = (NMHDR*)lParam;
	HWND hWndCtrl = pNMHDR->hwndFrom;

	int nCode = pNMHDR->code;
	if (nCode == TTN_NEEDTEXTA || nCode == TTN_NEEDTEXTW)
	{
		UINT nID = (UINT)wParam;
		ASSERT((UINT)pNMHDR->idFrom == (UINT)wParam);
		ASSERT(hWndCtrl != NULL);
		ASSERT(::IsWindow(hWndCtrl));

		if (AfxGetThreadState()->m_hLockoutNotifyWindow == m_hWnd)
			return TRUE;        // locked out - ignore control notification

		// reflect notification to child window control
		if (ReflectLastMsg(hWndCtrl, pResult))
			return TRUE;        // eaten by child

		AFX_NOTIFY notify;
		notify.pResult = pResult;
		notify.pNMHDR = pNMHDR;
		return OnCmdMsg(nID, MAKELONG(nCode, WM_NOTIFY), &notify, NULL); 
	}		
	return __super::OnNotify(wParam, lParam, pResult);
}

BOOL CZOrderPane::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message== WM_KEYDOWN )
	{
		if (pMsg->wParam==VK_RETURN)
		{
			pMsg->wParam = NULL;
			pMsg->message = NULL;
		}
		CDclFormView* pFormView = theStudioWorkspace.GetActiveFormView();
		if( pFormView )
		{
			if( pFormView->PreTranslateMessage(pMsg) )
				return TRUE;
		}
	}		
	
	return CDialog::PreTranslateMessage(pMsg);
}

void CZOrderPane::OnDestroy() 
{
	CDialog::OnDestroy();
}

BOOL CZOrderPane::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if( (BOOL)mZOrderList.SendMessage( WM_COMMAND, wParam, lParam ) )
		return TRUE;

	return __super::OnCommand(wParam, lParam);
}
