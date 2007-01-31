// ZOrderPane.cpp : implementation file
//

#include "stdafx.h"
#include "ZOrderPane.h"
#include "Resource.h"
#include "SharedRes.h"
#include "ObjectDCLView.h"
#include "Workspace.h"


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
	//{{AFX_MSG_MAP(CZOrderPane)
	ON_WM_CREATE()
	ON_WM_SIZE()	
	ON_COMMAND(ID_BRINGTOFRONT, OnBringtofront)
	ON_COMMAND(ID_SENDTOBACK, OnSendtoback)
	ON_COMMAND(ID_MOVEZBACKBY1, OnMovezbackby1)
	ON_COMMAND(ID_MOVEZFRONTBY1, OnMovezfrontby1)
	ON_COMMAND(ID_ZORDERHELP, OnZorderhelp)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_NOTIFY_RANGE( TTN_NEEDTEXTA, ID_BRINGTOFRONT, ID_MOVEZBACKBY1, OnNeedTextA)
	ON_NOTIFY_RANGE( TTN_NEEDTEXTW, ID_BRINGTOFRONT, ID_MOVEZBACKBY1, OnNeedTextW)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CZOrderPane message handlers

int CZOrderPane::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == nNotSet)
		return nNotSet;
	
	CRect rc(0,0,2,2);

	// create the ZOrder List
	if (!m_ZOrderList.Create(
		WS_CHILD | WS_VISIBLE | WS_TABSTOP | LVS_SHOWSELALWAYS | LVS_NOCOLUMNHEADER | LVS_REPORT | LVS_SHOWSELALWAYS ,
		rc,
		this,
		ZOrderListID))
	{
		return nNotSet;
	}

	m_ZOrderList.InsertColumn(0, _T("Controls"), LVCFMT_LEFT, nDeColWidth200);
	m_ZOrderList.ModifyStyleEx(0, WS_EX_CLIENTEDGE, SWP_FRAMECHANGED);

	if (!m_font.CreateStockObject(DEFAULT_GUI_FONT))
		if (!m_font.CreatePointFont(nDeFontPtSize, _T("MS Sans Serif")))
			return nNotSet;
	
	m_ZOrderList.SetFont(&m_font);

	// set the image list
	m_ZOrderList.m_ImageList.Create(nDeIconW,nDeIconHt, ILC_COLOR | ILC_MASK, 1, 1);
	HINSTANCE hInstResource = AfxFindResourceHandle(MAKEINTRESOURCE(IDI_LABEL), RT_GROUP_ICON);

	HICON hicon = LoadIcon(hInstResource, MAKEINTRESOURCE(IDI_LABEL));
  	m_ZOrderList.m_ImageList.Add(hicon);
	DestroyIcon(hicon);

	hicon = LoadIcon(hInstResource, MAKEINTRESOURCE(IDI_STDBUTTON));
	m_ZOrderList.m_ImageList.Add(hicon);
	DestroyIcon(hicon);

	hicon = LoadIcon(hInstResource, MAKEINTRESOURCE(IDI_GRAPHICBUTTON));
	m_ZOrderList.m_ImageList.Add(hicon);
	DestroyIcon(hicon);

	hicon = LoadIcon(hInstResource, MAKEINTRESOURCE(IDI_FRAME));
	m_ZOrderList.m_ImageList.Add(hicon);
	DestroyIcon(hicon);

	hicon = LoadIcon(hInstResource, MAKEINTRESOURCE(IDI_TEXTBOX));
	m_ZOrderList.m_ImageList.Add(hicon);
	DestroyIcon(hicon);

	hicon = LoadIcon(hInstResource, MAKEINTRESOURCE(IDI_CHECKBOX));
	m_ZOrderList.m_ImageList.Add(hicon);
	DestroyIcon(hicon);

	hicon = LoadIcon(hInstResource, MAKEINTRESOURCE(IDI_OPTION));
	m_ZOrderList.m_ImageList.Add(hicon);
	DestroyIcon(hicon);

	hicon = LoadIcon(hInstResource, MAKEINTRESOURCE(IDI_COMBOBOX));
	m_ZOrderList.m_ImageList.Add(hicon);
	DestroyIcon(hicon);

	hicon = LoadIcon(hInstResource, MAKEINTRESOURCE(IDI_LISTBOX));
	m_ZOrderList.m_ImageList.Add(hicon);
	DestroyIcon(hicon);

	hicon = LoadIcon(hInstResource, MAKEINTRESOURCE(IDI_SCROLLBAR));
	m_ZOrderList.m_ImageList.Add(hicon);
	DestroyIcon(hicon);

	hicon = LoadIcon(hInstResource, MAKEINTRESOURCE(IDI_SLIDERBAR));
	m_ZOrderList.m_ImageList.Add(hicon);
	DestroyIcon(hicon);

	hicon = LoadIcon(hInstResource, MAKEINTRESOURCE(IDI_PICTUREBOX));
	m_ZOrderList.m_ImageList.Add(hicon);
	DestroyIcon(hicon);

	hicon = LoadIcon(hInstResource, MAKEINTRESOURCE(IDI_TABS));
	m_ZOrderList.m_ImageList.Add(hicon);
	DestroyIcon(hicon);

	hicon = LoadIcon(hInstResource, MAKEINTRESOURCE(IDI_MONTH));
	m_ZOrderList.m_ImageList.Add(hicon);
	DestroyIcon(hicon);

	hicon = LoadIcon(hInstResource, MAKEINTRESOURCE(IDI_TREE));
	m_ZOrderList.m_ImageList.Add(hicon);
	DestroyIcon(hicon);

	hicon = LoadIcon(hInstResource, MAKEINTRESOURCE(IDI_3DRECT));
	m_ZOrderList.m_ImageList.Add(hicon);
	DestroyIcon(hicon);

	hicon = LoadIcon(hInstResource, MAKEINTRESOURCE(IDI_PROGRESSBAR));
	m_ZOrderList.m_ImageList.Add(hicon);
	DestroyIcon(hicon);

	hicon = LoadIcon(hInstResource, MAKEINTRESOURCE(IDI_SPINBUTTON));
	m_ZOrderList.m_ImageList.Add(hicon);
	DestroyIcon(hicon);

	hicon = LoadIcon(hInstResource, MAKEINTRESOURCE(IDI_URL));
	m_ZOrderList.m_ImageList.Add(hicon);
	DestroyIcon(hicon);

	hicon = LoadIcon(hInstResource, MAKEINTRESOURCE(IDI_ANGLESLIDER));
	m_ZOrderList.m_ImageList.Add(hicon);
	DestroyIcon(hicon);

	hicon = LoadIcon(hInstResource, MAKEINTRESOURCE(IDI_BLOCKVIEW));
	m_ZOrderList.m_ImageList.Add(hicon);
	DestroyIcon(hicon);

	hicon = LoadIcon(hInstResource, MAKEINTRESOURCE(IDI_SLIDEVW));
	m_ZOrderList.m_ImageList.Add(hicon);
	DestroyIcon(hicon);

	hicon = LoadIcon(hInstResource, MAKEINTRESOURCE(IDI_HTML));
	m_ZOrderList.m_ImageList.Add(hicon);
	DestroyIcon(hicon);

	hicon = LoadIcon(hInstResource, MAKEINTRESOURCE(IDI_DWGPREVIEW));
	m_ZOrderList.m_ImageList.Add(hicon);
	DestroyIcon(hicon);

	hicon = LoadIcon(hInstResource, MAKEINTRESOURCE(IDI_LISTVIEW));
	m_ZOrderList.m_ImageList.Add(hicon);
	DestroyIcon(hicon);

	hicon = LoadIcon(hInstResource, MAKEINTRESOURCE(IDI_BLOCKLIST));
	m_ZOrderList.m_ImageList.Add(hicon);
	DestroyIcon(hicon);

	hicon = LoadIcon(hInstResource, MAKEINTRESOURCE(IDI_OPTIONLIST));
	m_ZOrderList.m_ImageList.Add(hicon);
	DestroyIcon(hicon);

	hicon = LoadIcon(hInstResource, MAKEINTRESOURCE(IDI_ACTIVEX));
	m_ZOrderList.m_ImageList.Add(hicon);
	DestroyIcon(hicon);

	hicon = LoadIcon(hInstResource, MAKEINTRESOURCE(IDI_DWGLIST));
	m_ZOrderList.m_ImageList.Add(hicon);
	DestroyIcon(hicon);

	hicon = LoadIcon(hInstResource, MAKEINTRESOURCE(IDI_ANIMATE));
	m_ZOrderList.m_ImageList.Add(hicon);
	DestroyIcon(hicon);

	hicon = LoadIcon(hInstResource, MAKEINTRESOURCE(IDI_IMAGECOMBOBOX));
	m_ZOrderList.m_ImageList.Add(hicon);
	DestroyIcon(hicon);


	hicon = LoadIcon(hInstResource, MAKEINTRESOURCE(IDI_GRID));
	m_ZOrderList.m_ImageList.Add(hicon);
	DestroyIcon(hicon);

	
	hicon = LoadIcon(hInstResource, MAKEINTRESOURCE(IDI_SPLITTER));
	m_ZOrderList.m_ImageList.Add(hicon);
	DestroyIcon(hicon);

	hicon = LoadIcon(hInstResource, MAKEINTRESOURCE(IDI_HATCH));
	m_ZOrderList.m_ImageList.Add(hicon);
	DestroyIcon(hicon);

	hicon = LoadIcon(hInstResource, MAKEINTRESOURCE(IDI_OPENFOLDER));
	m_ZOrderList.m_ImageList.Add(hicon);
	DestroyIcon(hicon);


	int cx, cy;
	::ImageList_GetIconSize(m_ZOrderList.m_ImageList.m_hImageList, &cx, &cy);
	//
	//// set the project point 
	//SetZOrderList(&m_ZOrderList);

	m_ZOrderList.SetImageList(&m_ZOrderList.m_ImageList, LVSIL_SMALL);

	DWORD dwStyle = WS_VISIBLE | WS_CHILD | CCS_NODIVIDER | CCS_NOPARENTALIGN      
			| CCS_ADJUSTABLE | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT | TBSTYLE_WRAPABLE;

	if (!m_Buttons.Create( dwStyle, CRect (0,0,2,2), this, ID_TOOLBAR) )
	{
		return nNotSet;		// fail to create
	}

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
	m_pTBButtons[0].idCommand = ID_SENDTOBACK;
	m_pTBButtons[0].iString = 0;		
	VERIFY(m_Buttons.AddButtons(1,&m_pTBButtons[0]));


	m_pTBButtons[1].fsState = TBSTATE_ENABLED;
	m_pTBButtons[1].fsStyle = BYTE(TBSTYLE_BUTTON|TBSTYLE_TOOLTIPS);
	m_pTBButtons[1].dwData = 0;
	m_pTBButtons[1].iBitmap = 1;
	m_pTBButtons[1].idCommand = ID_MOVEZBACKBY1;
	m_pTBButtons[1].iString = IDS_TTTSENDTOBACK;		
	VERIFY(m_Buttons.AddButtons(1,&m_pTBButtons[1]));		

	m_pTBButtons[2].fsState = TBSTATE_ENABLED;
	m_pTBButtons[2].fsStyle = BYTE(TBSTYLE_BUTTON|TBSTYLE_TOOLTIPS);
	m_pTBButtons[2].dwData = 0;
	m_pTBButtons[2].iBitmap = 2;
	m_pTBButtons[2].idCommand = ID_MOVEZFRONTBY1;
	m_pTBButtons[2].iString = 0;		
	VERIFY(m_Buttons.AddButtons(1,&m_pTBButtons[2]));		

	m_pTBButtons[3].fsState = TBSTATE_ENABLED;
	m_pTBButtons[3].fsStyle = BYTE(TBSTYLE_BUTTON|TBSTYLE_TOOLTIPS);
	m_pTBButtons[3].dwData = 0;
	m_pTBButtons[3].iBitmap = 3;
	m_pTBButtons[3].idCommand = ID_BRINGTOFRONT;
	m_pTBButtons[3].iString = IDS_TTTSENDTOBACK;		
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
	m_ZOrderList.MoveWindow(rcZOrder, TRUE);

	
}

void CZOrderPane::OnBringtofront() 
{
	m_ZOrderList.Bringtofront();
	
}

void CZOrderPane::OnSendtoback() 
{
	m_ZOrderList.SendtoBack();
	
}

void CZOrderPane::OnMovezbackby1() 
{
	m_ZOrderList.SentToBackBy1();
	
}

void CZOrderPane::OnMovezfrontby1() 
{
	m_ZOrderList.BringtofrontBy1();
	
}

void CZOrderPane::OnZorderhelp() 
{
	// TODO: Add your command handler code here
	
}

CString CZOrderPane::NeedText( UINT nID, NMHDR * pNotifyStruct, LRESULT * lResult )
{
	LPTOOLTIPTEXT lpTTT = (LPTOOLTIPTEXT)pNotifyStruct ;
	ASSERT(nID == lpTTT->hdr.idFrom);

	CString toolTipText;

	switch (nID)
	{
	case ID_BRINGTOFRONT:
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
	if (toolTipText.GetLength() > nDeToolTipTextLen)
		nLength = nDeToolTipTextLen;
	else
		nLength = toolTipText.GetLength();
	
	toolTipText = toolTipText.Left(nLength);

	return toolTipText;
}


/////////////////////////////////////////////////////////////////////////////
// CToolBox message handlers
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

	_tcscpy(lpTTT->szText,(LPCTSTR)toolTipText);
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
		if (m_ZOrderList.m_pView != NULL)
			m_ZOrderList.m_pView->PreTranslateMessage(pMsg);
	}		
	
	return CDialog::PreTranslateMessage(pMsg);
}

void CZOrderPane::OnDestroy() 
{
	CDialog::OnDestroy();
	
	m_font.DeleteObject();		
}
