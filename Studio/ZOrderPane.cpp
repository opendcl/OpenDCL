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
	m_pTBButtons = NULL;
}

CZOrderPane::~CZOrderPane()
{
	if (m_pTBButtons)
		delete []m_pTBButtons;
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
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CRect rc(0,0,2,2);

	// create the ZOrder List
	if (!mZOrderList.Create( WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,
													 rc,
													 this,
													 ZOrderListID))
		return -1;

	DWORD dwStyle = WS_VISIBLE | WS_CHILD | CCS_NODIVIDER | CCS_NOPARENTALIGN      
			| CCS_ADJUSTABLE | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT | TBSTYLE_WRAPABLE;

	if (!m_Buttons.Create( dwStyle, CRect (0,0,2,2), this, ID_TOOLBAR) )
		return -1;		// fail to create

	// enable the toolbox
	m_Buttons.EnableWindow(TRUE);
	m_Buttons.ShowWindow(TRUE);
	m_Buttons.AutoSize();
	
	// call the method to add the buttons to the toolbar
	AddTheButtons();
	
	return 0;
}


void CZOrderPane::AddTheButtons()
{
	int nButtonCount = nDeButtonCountZO;
	
	m_Buttons.AddBitmap(nButtonCount,IDR_ZTABORDER);
	m_Buttons.SetBitmapSize(CSize(nDeIconHt,nDeIconHt));
	m_pTBButtons = new TBBUTTON[nButtonCount];

	m_pTBButtons[0].fsState = TBSTATE_ENABLED;
	m_pTBButtons[0].fsStyle = BYTE(TBSTYLE_BUTTON|TBSTYLE_TOOLTIPS);
	m_pTBButtons[0].dwData = 0;
	m_pTBButtons[0].iBitmap = 0;
	m_pTBButtons[0].idCommand = ID_SENDTOFRONT;
	m_pTBButtons[0].iString = IDS_TTTBRINGTOFRONT	;		
	VERIFY(m_Buttons.AddButtons(1,&m_pTBButtons[0]));		

	m_pTBButtons[1].fsState = TBSTATE_ENABLED;
	m_pTBButtons[1].fsStyle = BYTE(TBSTYLE_BUTTON|TBSTYLE_TOOLTIPS);
	m_pTBButtons[1].dwData = 0;
	m_pTBButtons[1].iBitmap = 1;
	m_pTBButtons[1].idCommand = ID_SENDTOBACK;
	m_pTBButtons[1].iString = IDS_TTTSENDTOBACK;		
	VERIFY(m_Buttons.AddButtons(1,&m_pTBButtons[1]));

	m_pTBButtons[2].fsState = TBSTATE_ENABLED;
	m_pTBButtons[2].fsStyle = BYTE(TBSTYLE_BUTTON|TBSTYLE_TOOLTIPS);
	m_pTBButtons[2].dwData = 0;
	m_pTBButtons[2].iBitmap = 2;
	m_pTBButtons[2].idCommand = ID_MOVEZFRONTBY1;
	m_pTBButtons[2].iString = IDS_ZMOVEUP;		
	VERIFY(m_Buttons.AddButtons(1,&m_pTBButtons[2]));		

	m_pTBButtons[3].fsState = TBSTATE_ENABLED;
	m_pTBButtons[3].fsStyle = BYTE(TBSTYLE_BUTTON|TBSTYLE_TOOLTIPS);
	m_pTBButtons[3].dwData = 0;
	m_pTBButtons[3].iBitmap = 3;
	m_pTBButtons[3].idCommand = ID_MOVEZBACKBY1;
	m_pTBButtons[3].iString = IDS_ZMOVEDOWN;		
	VERIFY(m_Buttons.AddButtons(1,&m_pTBButtons[3]));		

}


void CZOrderPane::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

	m_Buttons.MoveWindow(0,0,cx,cy,TRUE);

	CRect rc;
	if (IsWindow(m_Buttons.m_hWnd))
		m_Buttons.GetWindowRect(&rc);
	ScreenToClient(rc);
	// resize the ZOrder list 
	CRect rcZOrder(0,rc.bottom + 1,cx-2, cy);
	mZOrderList.MoveWindow(rcZOrder, TRUE);
}

CString CZOrderPane::NeedText( UINT nID, NMHDR * pNotifyStruct, LRESULT * lResult )
{
	LPTOOLTIPTEXT lpTTT = (LPTOOLTIPTEXT)pNotifyStruct ;
	ASSERT(nID == lpTTT->hdr.idFrom);

	CString toolTipText;

	switch (nID)
	{
	case ID_SENDTOFRONT:
		toolTipText = theWorkspace.LoadResourceString(IDS_TTTBRINGTOFRONT);
		break;
	case ID_SENDTOBACK:
		toolTipText = theWorkspace.LoadResourceString(IDS_TTTSENDTOBACK);
		break;
	case ID_MOVEZFRONTBY1:
		toolTipText = theWorkspace.LoadResourceString(IDS_ZMOVEUP);
		break;
	case ID_MOVEZBACKBY1:
		toolTipText = theWorkspace.LoadResourceString(IDS_ZMOVEDOWN);
		break;	
	}

	int nLength;

	// szText length is 80
	if (toolTipText.GetLength() > nDeToolTipTitleLen)
		nLength = nDeToolTipTitleLen;
	else
		nLength = toolTipText.GetLength();
	
	toolTipText = toolTipText.Left(nLength);

	return toolTipText;
}


/////////////////////////////////////////////////////////////////////////////
// CToolboxPane message handlers
void CZOrderPane::OnNeedTextW( UINT nID, NMHDR * pNotifyStruct, LRESULT * lResult )
{
	CString toolTipText = NeedText(nID, pNotifyStruct, lResult);

	LPTOOLTIPTEXTW lpTTT = (LPTOOLTIPTEXTW)pNotifyStruct;

#ifdef _UNICODE
	_tcsncpy(lpTTT->szText,(LPCTSTR)toolTipText, toolTipText.GetLength() + 1);
#else
	mbstowcs(lpTTT->szText,(LPCTSTR)toolTipText, toolTipText.GetLength() + 1);
#endif
}

BOOL CZOrderPane::OnNeedText( UINT id, NMHDR * pTTTStruct, LRESULT * pResult )
{
	TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pTTTStruct;
    UINT nID =pTTTStruct->idFrom;
    if (pTTT->uFlags & TTF_IDISHWND)
    {
        // idFrom is actually the HWND of the tool
        nID = ::GetDlgCtrlID((HWND)nID);
        if(nID)
        {
            pTTT->lpszText = MAKEINTRESOURCE(nID);
            pTTT->hinst = AfxGetResourceHandle();
            return(TRUE);
        }
    }
    return(FALSE);

}


void CZOrderPane::OnNeedTextA( UINT nID, NMHDR * pNotifyStruct, LRESULT * lResult )
{
	CString toolTipText = NeedText(nID, pNotifyStruct, lResult);

	LPTOOLTIPTEXT lpTTT = (LPTOOLTIPTEXT)pNotifyStruct;

	lstrcpyn(lpTTT->szText,toolTipText, _elements(lpTTT->szText));
}

BOOL CZOrderPane::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	ASSERT(pResult != NULL);
	NMHDR* pNMHDR = (NMHDR*)lParam;
	HWND hWndCtrl = pNMHDR->hwndFrom;

	// get the child ID from the window itself
	// UINT nID = _AfxGetDlgCtrlID(hWndCtrl);

	//////////////////////////////////////////////////////////////////
	// If TTN_NEEDTEXT we cannot get the ID from the tooltip window //
	//////////////////////////////////////////////////////////////////

	int nCode = pNMHDR->code;

	//
	// if it is the following notification message
	// nID has to obtained from wParam
	//
	
	if (nCode == TTN_NEEDTEXTA || nCode == TTN_NEEDTEXTW)
	{
		UINT nID;   // = _AfxGetDlgCtrlID(hWndCtrl);
		nID = (UINT)wParam;


		ASSERT((UINT)pNMHDR->idFrom == (UINT)wParam);
		UNUSED(wParam);  // not used in release build
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
		BOOL b = OnCmdMsg(nID, MAKELONG(nCode, WM_NOTIFY), &notify, NULL); 
		return b;
	}		
	return CDialog::OnNotify(wParam, lParam, pResult);
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
