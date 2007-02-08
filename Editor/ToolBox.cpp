// ToolBox.cpp : implementation file
//

#include "stdafx.h"
#include "ToolBox.h"
#include "Resource.h"
#include "PurchaseMode.h"
#include "ControlTypes.h"
#include "ObjectDCLView.h"
#include "DclFormObject.h"
#include "ZOrderListCtrl.h"
#include "InsertControlDlg.h"
#include "OleLicenseKey.h"
#include "EditorWorkspace.h"


/////////////////////////////////////////////////////////////////////////////
// CToolBox

CToolBox::CToolBox()
{
	m_pTBButtons = NULL;
	m_pActiveView = NULL;
	m_nSelectedCtrl = 1;
}

CToolBox::~CToolBox()
{
	if (m_pTBButtons)
		delete []m_pTBButtons;

}


BEGIN_MESSAGE_MAP(CToolBox, CDialog)
	//{{AFX_MSG_MAP(CToolBox)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_TOOLBOX_POINTER, OnToolboxPointer)
	ON_COMMAND(ID_TOOLBOX_ANGLESLIDER, OnToolboxAngleslider)
	ON_COMMAND(ID_TOOLBOX_BLOCKLIST, OnToolboxBlocklist)
	ON_COMMAND(ID_TOOLBOX_BLOCKVIEW, OnToolboxBlockview)
	ON_COMMAND(ID_TOOLBOX_CHECKBOX, OnToolboxCheckbox)
	ON_COMMAND(ID_TOOLBOX_COMBOBOX, OnToolboxCombobox)
	ON_COMMAND(ID_TOOLBOX_DWGPREVIEW, OnToolboxDwgpreview)
	ON_COMMAND(ID_TOOLBOX_FRAME, OnToolboxFrame)
	ON_COMMAND(ID_TOOLBOX_GRAPHICBUTTON, OnToolboxGraphicbutton)
	ON_COMMAND(ID_TOOLBOX_HTML, OnToolboxHtml)
	ON_COMMAND(ID_TOOLBOX_LABEL, OnToolboxLabel)
	ON_COMMAND(ID_TOOLBOX_LISTBOX, OnToolboxListbox)
	ON_COMMAND(ID_TOOLBOX_LISTVIEW, OnToolboxListview)
	ON_COMMAND(ID_TOOLBOX_MONTHPICKER, OnToolboxMonthpicker)
	ON_COMMAND(ID_TOOLBOX_OPTION, OnToolboxOption)
	ON_COMMAND(ID_TOOLBOX_PICTUREBOX, OnToolboxPicturebox)
	ON_COMMAND(ID_TOOLBOX_PROGRESSBAR, OnToolboxProgressbar)
	ON_COMMAND(ID_TOOLBOX_RECTANGLE, OnToolboxRectangle)
	ON_COMMAND(ID_TOOLBOX_SCROLLBAR, OnToolboxScrollbar)
	ON_COMMAND(ID_TOOLBOX_SLIDER, OnToolboxSlider)
	ON_COMMAND(ID_TOOLBOX_SLIDEVIEW, OnToolboxSlideview)
	ON_COMMAND(ID_TOOLBOX_SPINBUTTON, OnToolboxSpinbutton)
	ON_COMMAND(ID_TOOLBOX_TABS, OnToolboxTabs)
	ON_COMMAND(ID_TOOLBOX_TEXTBOX, OnToolboxTextbox)
	ON_COMMAND(ID_TOOLBOX_TEXTBUTTON, OnToolboxTextbutton)
	ON_COMMAND(ID_TOOLBOX_TREE, OnToolboxTree)
	ON_COMMAND(ID_TOOLBOX_URLLINK, OnToolboxUrllink)
	ON_COMMAND(ID_TOOLBOX_OPTIONLIST, OnToolboxOptionList) // was going to be a toolbar but now the option list
	ON_COMMAND(ID_TOOLBOX_ACTIVEX, OnToolboxActivex)
	ON_COMMAND(ID_TOOLBOX_DWGLIST, OnToolboxDwglist)
	ON_COMMAND(ID_TOOLBOX_ANIMATE, OnToolboxAnimate)
	ON_COMMAND(ID_TOOLBOX_IMAGECOMBOBOX, OnToolboxImagecombobox)
	ON_COMMAND(ID_TOOLBOX_GRID, OnToolboxGrid)
	ON_COMMAND(ID_TOOLBOX_SPLITTER, OnToolboxSlitter)
	ON_COMMAND(ID_TOOLBOX_HATCH, OnToolboxHatch)
	
	//}}AFX_MSG_MAP
	ON_NOTIFY_RANGE( TTN_NEEDTEXTA, ID_TOOLBOX_POINTER, ID_TOOLBOX_POINTER + nDeButtonCount, OnNeedTextA)
	ON_NOTIFY_RANGE( TTN_NEEDTEXTW, ID_TOOLBOX_POINTER, ID_TOOLBOX_POINTER + nDeButtonCount, OnNeedTextW)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CToolBox message handlers
void CToolBox::OnNeedTextW( UINT nID, NMHDR * pNotifyStruct, LRESULT * lResult )
{
	CString toolTipText = NeedText(nID, pNotifyStruct, lResult);

	LPTOOLTIPTEXTW lpTTT = (LPTOOLTIPTEXTW)pNotifyStruct;

#ifdef _UNICODE
	_tcsncpy(lpTTT->szText,(LPCTSTR)toolTipText, toolTipText.GetLength() + 1);
#else
	mbstowcs(lpTTT->szText,(LPCTSTR)toolTipText, toolTipText.GetLength() + 1);
#endif
}

BOOL CToolBox::OnNeedText( UINT id, NMHDR * pTTTStruct, LRESULT * pResult )
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


void CToolBox::OnNeedTextA( UINT nID, NMHDR * pNotifyStruct, LRESULT * lResult )
{
	CString toolTipText = NeedText(nID, pNotifyStruct, lResult);

	LPTOOLTIPTEXT lpTTT = (LPTOOLTIPTEXT)pNotifyStruct;

	_tcscpy(lpTTT->szText,(LPCTSTR)toolTipText);
}

CString CToolBox::NeedText( UINT nID, NMHDR * pNotifyStruct, LRESULT * lResult )
{
	LPTOOLTIPTEXT lpTTT = (LPTOOLTIPTEXT)pNotifyStruct ;
	ASSERT(nID == lpTTT->hdr.idFrom);

	CString toolTipText;
	
	toolTipText = theWorkspace.LoadResourceString(nID - ID_TOOLBOX_POINTER + IDS_POINTER);

	// szText length is 80
	int nLength = (toolTipText.GetLength() > nDeToolTipLength) ? nDeToolTipLength : toolTipText.GetLength();

	toolTipText = toolTipText.Left(nLength);

	return toolTipText;
}


int CToolBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == nNotSet)
		return nNotSet;

	DWORD dwStyle = WS_VISIBLE | WS_CHILD | CCS_NODIVIDER | CCS_NOPARENTALIGN      
			| CCS_ADJUSTABLE | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT | TBSTYLE_WRAPABLE;

	if (!m_ToolBoxButtons.Create( dwStyle, CRect (0,0,2,2), this, ID_TOOLBAR) )
	{
		TRACE0("Failed to create instant bar child\n");
		return nNotSet;		// fail to create
	}
	// enable the toolbox
	m_ToolBoxButtons.EnableWindow(TRUE);
	
	m_ToolBoxButtons.AutoSize();
	
	// call the method to add the buttons to the toolbar
	AddTheButtons();
	


	return 0;
}

void CToolBox::AddButton(UINT nID)
{	
//	m_pTBButtons = new TBBUTTON[nDeButtonCount];
	int nIndex = nID - ID_TOOLBOX_POINTER;

	if (nID == ID_TOOLBOX_POINTER)
		m_pTBButtons[nButtonIndex].fsState = BYTE(TBSTYLE_CHECKGROUP|TBSTYLE_TOOLTIPS);//TBSTATE_ENABLED|TBSTATE_CHECKED
	else
		m_pTBButtons[nButtonIndex].fsState = BYTE(TBSTYLE_CHECKGROUP|TBSTYLE_TOOLTIPS);//TBSTATE_ENABLED

	m_pTBButtons[nButtonIndex].dwData = 0;
		
	m_pTBButtons[nButtonIndex].iBitmap = nIndex;
	m_pTBButtons[nButtonIndex].idCommand = nID;
	
		
	m_pTBButtons[nButtonIndex].iString = nIndex + ID_TOOLBOX_POINTER;
	
	nButtonIndex++;

	BOOL b = m_ToolBoxButtons.AddButtons(nButtonIndex, &m_pTBButtons[nButtonIndex]);
}
void CToolBox::AddTheButtons()
{	
	
	m_ToolBoxButtons.AddBitmap(nDeButtonCount,IDR_TOOLBOX);
	m_ToolBoxButtons.SetBitmapSize(CSize(nDeBitmapSize,nDeBitmapSize));
	m_pTBButtons = new TBBUTTON[nDeButtonCount];

	nButtonIndex = 0;

  int nIndex;
	for (nIndex = 0; nIndex < nDeButtonCount; nIndex++)
	{
		if (nIndex == 0)
			m_pTBButtons[nIndex].fsState = TBSTATE_ENABLED|TBSTATE_CHECKED;
		else
			m_pTBButtons[nIndex].fsState = TBSTATE_ENABLED;

		m_pTBButtons[nIndex].fsStyle = BYTE(TBSTYLE_CHECKGROUP|TBSTYLE_TOOLTIPS);
		m_pTBButtons[nIndex].dwData = 0;
		
		// here we want to place the ActiveX button last
		if (nIndex == nDeActiveXButton)
		{
			//m_pTBButtons[nIndex].iBitmap = nDeButtonCount - 1;
			//m_pTBButtons[nIndex].idCommand = nDeButtonCount - 1 + ID_TOOLBOX_POINTER;
		}
		else if (nIndex > nDeActiveXButton)
		{
			m_pTBButtons[nIndex-1].iBitmap = nIndex;
			m_pTBButtons[nIndex-1].idCommand = nIndex + ID_TOOLBOX_POINTER;
			m_pTBButtons[nIndex-1].iString = nIndex + ID_TOOLBOX_POINTER;		
		}
		else		
		{
			m_pTBButtons[nIndex].iBitmap = nIndex;
			m_pTBButtons[nIndex].idCommand = nIndex + ID_TOOLBOX_POINTER;
			m_pTBButtons[nIndex].iString = nIndex + ID_TOOLBOX_POINTER;		
		}		
		
	}
	m_pTBButtons[nDeButtonCount-1].iBitmap = nDeActiveXButton;
	m_pTBButtons[nDeButtonCount-1].idCommand = nDeActiveXButton + ID_TOOLBOX_POINTER;
	m_pTBButtons[nDeButtonCount-1].iString = nDeActiveXButton + ID_TOOLBOX_POINTER;		
	
	for (nIndex = 0; nIndex < nDeButtonCount; nIndex++)
	{
		if (nCurrentPurchaseMode == nPurchasedLT && nIndex >= 12 && nIndex <= 15)
			continue;
		if (nCurrentPurchaseMode == nPurchasedLT && nIndex >= 18 && nIndex <= 21)
			continue;
		if (nCurrentPurchaseMode == nPurchasedLT && nIndex >= 23 && nIndex <= 26)
			continue;
		if (nCurrentPurchaseMode == nPurchasedLT && nIndex >= 28)
			continue;

		if (nCurrentPurchaseMode != nPurchasedEnt &&
			nCurrentPurchaseMode != nDemoPro &&
			nIndex == CtlGrid -2)
			continue;

		if (nCurrentPurchaseMode == nPurchasedStd && nIndex == CtlImageComboBox -2)
			continue;
		if (nCurrentPurchaseMode == nPurchasedLT && nIndex == CtlImageComboBox -2)
			continue;

		if (nCurrentPurchaseMode == nPurchasedStd && nIndex == 21)
			continue;
		if (nCurrentPurchaseMode == nPurchasedStd && nIndex == 28)
			continue;
		if (nCurrentPurchaseMode == nPurchasedStd && nIndex == 29)
			continue;

		if (nCurrentPurchaseMode == nPurchasedR14Pro && nIndex == CtlBlockView - 1)
			continue;
		if (nCurrentPurchaseMode == nPurchasedR14Pro && nIndex == CtlBlockList - 1)
			continue;
		
		VERIFY(m_ToolBoxButtons.AddButtons(1, &m_pTBButtons[nIndex]));		
	}
	
}

void CToolBox::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	m_ToolBoxButtons.MoveWindow(0,0,cx,cy,TRUE);
}

BOOL CToolBox::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
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

BOOL CToolBox::OnCmdMsg(UINT nID, int nCode, void* pExtra,
	AFX_CMDHANDLERINFO* pHandlerInfo)
{
	if (CWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	BOOL b =  IS_COMMAND_ID(nID);
	if ((nCode != CN_COMMAND && nCode != CN_UPDATE_COMMAND_UI) ||
			!IS_COMMAND_ID(nID) || nID >= nCommandIDLimit)
	{
		// control notification or non-command button or system command
		return FALSE;       // not routed any further
	}

	// if we have an owner window, give it second crack
	CWnd* pOwner = GetParent();
	if (pOwner != NULL)
	{
#ifdef _DEBUG
		if (afxTraceFlags & traceCmdRouting)
			TRACE1("Routing command id 0x%04X to owner window.\n", nID);
#endif
		ASSERT(pOwner != this);
		if (pOwner->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
			return TRUE;
	}

	// last crack goes to the current CWinThread object
	CWinThread* pThread = AfxGetThread();
	if (pThread != NULL)
	{
#ifdef _DEBUG
		if (afxTraceFlags & traceCmdRouting)
			TRACE1("Routing command id 0x%04X to app.\n", nID);
#endif
		if (pThread->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
			return TRUE;
	}

#ifdef _DEBUG
	if (afxTraceFlags & traceCmdRouting)
	{
		TRACE2("IGNORING command id 0x%04X sent to %hs dialog.\n", nID,
				GetRuntimeClass()->m_lpszClassName);
	}
#endif
	return FALSE;
}


void CToolBox::EnableToolboxTabs(bool bEnabled) 
{
	m_ToolBoxButtons.EnableButton(ID_TOOLBOX_TABS, bEnabled);
}


void CToolBox::OnToolboxPointer()
{
	
	((CObjectDCLView*)m_pActiveView)->m_StandardCursorID = false;
	m_nSelectedCtrl = 1;
}

void CToolBox::OnToolboxAngleslider() 
{
	if (((CObjectDCLView*)m_pActiveView)->m_pThisDclForm == NULL)
	{
		ResetToPointer();
		return;
	}
	
	((CObjectDCLView*)m_pActiveView)->m_StandardCursorID = true;
	m_nSelectedCtrl = CtlRoundSlider;
}

void CToolBox::OnToolboxBlocklist() 
{
	if (((CObjectDCLView*)m_pActiveView)->m_pThisDclForm == NULL)
	{
		ResetToPointer();
		return;
	}
	
	((CObjectDCLView*)m_pActiveView)->m_StandardCursorID = true;
	m_nSelectedCtrl = CtlBlockList;
}

void CToolBox::OnToolboxBlockview() 
{
	if (((CObjectDCLView*)m_pActiveView)->m_pThisDclForm == NULL)
	{
		ResetToPointer();
		return;
	}
	
	((CObjectDCLView*)m_pActiveView)->m_StandardCursorID = true;
	m_nSelectedCtrl = CtlBlockView;
	
}

void CToolBox::OnToolboxCheckbox() 
{
	if (((CObjectDCLView*)m_pActiveView)->m_pThisDclForm == NULL)
	{
		ResetToPointer();
		return;
	}
	
	((CObjectDCLView*)m_pActiveView)->m_StandardCursorID = true;
	m_nSelectedCtrl = CtlCheckBox;
}

void CToolBox::OnToolboxCombobox() 
{
	if (((CObjectDCLView*)m_pActiveView)->m_pThisDclForm == NULL)
	{
		ResetToPointer();
		return;
	}
	
	((CObjectDCLView*)m_pActiveView)->m_StandardCursorID = true;
	m_nSelectedCtrl = CtlComboBox;
	
}

void CToolBox::OnToolboxDwgpreview() 
{
	if (((CObjectDCLView*)m_pActiveView)->m_pThisDclForm == NULL)
	{
		ResetToPointer();
		return;
	}
	
	((CObjectDCLView*)m_pActiveView)->m_StandardCursorID = true;
	m_nSelectedCtrl = CtlDwgPreview;
	
}

void CToolBox::OnToolboxFrame() 
{
	if (((CObjectDCLView*)m_pActiveView)->m_pThisDclForm == NULL)
	{
		ResetToPointer();
		return;
	}
	
	((CObjectDCLView*)m_pActiveView)->m_StandardCursorID = true;
	m_nSelectedCtrl = CtlFrame;
	
}

void CToolBox::OnToolboxGraphicbutton() 
{
	if (((CObjectDCLView*)m_pActiveView)->m_pThisDclForm == NULL)
	{
		ResetToPointer();
		return;
	}
	
	((CObjectDCLView*)m_pActiveView)->m_StandardCursorID = true;
	m_nSelectedCtrl = CtlGraphicButton;
	
}

void CToolBox::OnToolboxHtml() 
{
	if (((CObjectDCLView*)m_pActiveView)->m_pThisDclForm == NULL)
	{
		ResetToPointer();
		return;
	}
	
	((CObjectDCLView*)m_pActiveView)->m_StandardCursorID = true;
	m_nSelectedCtrl = CtlHtmlCtrl;
	
}

void CToolBox::OnToolboxLabel() 
{
	if (((CObjectDCLView*)m_pActiveView)->m_pThisDclForm == NULL)
	{
		ResetToPointer();
		return;
	}
	
	((CObjectDCLView*)m_pActiveView)->m_StandardCursorID = true;
	m_nSelectedCtrl = CtlLabel;
	
}

void CToolBox::OnToolboxListbox() 
{
	if (((CObjectDCLView*)m_pActiveView)->m_pThisDclForm == NULL)
	{
		ResetToPointer();
		return;
	}
	
	((CObjectDCLView*)m_pActiveView)->m_StandardCursorID = true;
	m_nSelectedCtrl = CtlListBox;
	
}

void CToolBox::OnToolboxListview() 
{
	if (((CObjectDCLView*)m_pActiveView)->m_pThisDclForm == NULL)
	{
		ResetToPointer();
		return;
	}
	
	((CObjectDCLView*)m_pActiveView)->m_StandardCursorID = true;
	m_nSelectedCtrl = CtlListView;
	
}

void CToolBox::OnToolboxMonthpicker() 
{
	if (((CObjectDCLView*)m_pActiveView)->m_pThisDclForm == NULL)
	{
		ResetToPointer();
		return;
	}
	
	((CObjectDCLView*)m_pActiveView)->m_StandardCursorID = true;
	m_nSelectedCtrl = CtlMonth;
	
}

void CToolBox::OnToolboxOption() 
{
	if (((CObjectDCLView*)m_pActiveView)->m_pThisDclForm == NULL)
	{
		ResetToPointer();
		return;
	}
	
	((CObjectDCLView*)m_pActiveView)->m_StandardCursorID = true;
	m_nSelectedCtrl = CtlOptionButton;
	
}

void CToolBox::OnToolboxPicturebox() 
{
	if (((CObjectDCLView*)m_pActiveView)->m_pThisDclForm == NULL)
	{
		ResetToPointer();
		return;
	}
	
	((CObjectDCLView*)m_pActiveView)->m_StandardCursorID = true;
	m_nSelectedCtrl = CtlPictureBox;
}

void CToolBox::OnToolboxProgressbar() 
{
	if (((CObjectDCLView*)m_pActiveView)->m_pThisDclForm == NULL)
	{
		ResetToPointer();
		return;
	}
	
	((CObjectDCLView*)m_pActiveView)->m_StandardCursorID = true;
	m_nSelectedCtrl = CtlProgress;
}

void CToolBox::OnToolboxRectangle() 
{
	if (((CObjectDCLView*)m_pActiveView)->m_pThisDclForm == NULL)
	{
		ResetToPointer();
		return;
	}
	
	((CObjectDCLView*)m_pActiveView)->m_StandardCursorID = true;
	m_nSelectedCtrl = Ctl3DRect;	
}

void CToolBox::OnToolboxScrollbar() 
{
	if (((CObjectDCLView*)m_pActiveView)->m_pThisDclForm == NULL)
	{
		ResetToPointer();
		return;
	}
	
	((CObjectDCLView*)m_pActiveView)->m_StandardCursorID = true;
	m_nSelectedCtrl = CtlScrollBar;
}

void CToolBox::OnToolboxSlider() 
{
	if (((CObjectDCLView*)m_pActiveView)->m_pThisDclForm == NULL)
	{
		ResetToPointer();
		return;
	}
	
	((CObjectDCLView*)m_pActiveView)->m_StandardCursorID = true;
	m_nSelectedCtrl = CtlSlider;	
}

void CToolBox::OnToolboxSlideview() 
{
	if (((CObjectDCLView*)m_pActiveView)->m_pThisDclForm == NULL)
	{
		ResetToPointer();
		return;
	}
	
	((CObjectDCLView*)m_pActiveView)->m_StandardCursorID = true;
	m_nSelectedCtrl = CtlSlideView;
}

void CToolBox::OnToolboxSpinbutton() 
{
	if (((CObjectDCLView*)m_pActiveView)->m_pThisDclForm == NULL)
	{
		ResetToPointer();
		return;
	}
	
	((CObjectDCLView*)m_pActiveView)->m_StandardCursorID = true;
	m_nSelectedCtrl = CtlSpinButton;
}

void CToolBox::OnToolboxTabs() 
{
	if (m_pActiveView == NULL)
	{
		return;
	}
	
	// check to see if we need to disallow the user from inserting or selecting a control
	if (((CObjectDCLView*)m_pActiveView)->m_pThisDclForm != NULL)
	{
		
		if (((CObjectDCLView*)m_pActiveView)->IsTabsEnabled() == false)
		{
			CString sMsg;
			CString sTitle;
			sMsg = theWorkspace.LoadResourceString(IDS_NOMORETABS);
			sTitle = theWorkspace.LoadResourceString(IDR_MAINFRAME);
			
			ResetToPointer();
			MessageBox(sMsg, sTitle, MB_ICONEXCLAMATION);
			return;
		}
		if (((CObjectDCLView*)m_pActiveView)->m_pThisDclForm->GetType() == VdclTabForm)
		{
			CString sMsg;
			CString sTitle;
			sMsg = theWorkspace.LoadResourceString(IDS_NOTABWITHINTAB);
			sTitle = theWorkspace.LoadResourceString(IDR_MAINFRAME);
			
			ResetToPointer();
			MessageBox(sMsg, sTitle, MB_ICONEXCLAMATION);
			return;
		}
	}

	if (((CObjectDCLView*)m_pActiveView)->m_pThisDclForm == NULL)
	{
		ResetToPointer();
		return;
	}
	
	((CObjectDCLView*)m_pActiveView)->m_StandardCursorID = true;
	m_nSelectedCtrl = CtlTabStrip;
}

void CToolBox::OnToolboxTextbox() 
{
	if (((CObjectDCLView*)m_pActiveView)->m_pThisDclForm == NULL)
	{
		ResetToPointer();
		return;
	}
	
	((CObjectDCLView*)m_pActiveView)->m_StandardCursorID = true;
	m_nSelectedCtrl = CtlTextBox;
}

void CToolBox::OnToolboxTextbutton() 
{
	if (((CObjectDCLView*)m_pActiveView)->m_pThisDclForm == NULL)
	{
		ResetToPointer();
		return;
	}
	
	((CObjectDCLView*)m_pActiveView)->m_StandardCursorID = true;
	m_nSelectedCtrl = CtlStdButton;
}

void CToolBox::OnToolboxTree() 
{
	if (((CObjectDCLView*)m_pActiveView)->m_pThisDclForm == NULL)
	{
		ResetToPointer();
		return;
	}
	
	((CObjectDCLView*)m_pActiveView)->m_StandardCursorID = true;
	m_nSelectedCtrl = CtlTree;
}

void CToolBox::OnToolboxUrllink() 
{
	((CObjectDCLView*)m_pActiveView)->m_StandardCursorID = true;
	m_nSelectedCtrl = CtlStaticURL;
}

void CToolBox::SetActiveView(CView *pView) 
{
	m_pActiveView = pView;

	if (m_pActiveView == NULL)
	{
		CZOrderListCtrl* pZOrderList = theEditorWorkspace.GetZOrderListCtrl();

		// clear the list
		pZOrderList->ClearList((CObjectDCLView*)pView);

	}
}

void CToolBox::OnToolboxOptionList() 
{
	if (((CObjectDCLView*)m_pActiveView)->m_pThisDclForm == NULL)
	{
		ResetToPointer();
		return;
	}
	((CObjectDCLView*)m_pActiveView)->m_StandardCursorID = true;
	m_nSelectedCtrl = CtlOptionList	;
	
}

void CToolBox::ResetToPointer()
{
	m_ToolBoxButtons.CheckButton(ID_TOOLBOX_POINTER, TRUE);
	m_nSelectedCtrl = 1;
}

BOOL CToolBox::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_ToolBoxButtons.EnableToolTips(TRUE);	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX PropertyObject Pages should return FALSE
}

void CToolBox::OnToolboxActivex() 
{
	if (((CObjectDCLView*)m_pActiveView)->m_pThisDclForm == NULL)
	{
		ResetToPointer();
		return;
	}
	
	CInsertControlDlg dlg;
	int nResult;

	nResult = dlg.DoModal();
	if( nResult != IDOK )
	{
		return;
	}

	((CObjectDCLView*)m_pActiveView)->m_clsid = dlg.m_clsid;
	((CObjectDCLView*)m_pActiveView)->m_ActiveXFileName = dlg.m_FileName;
	((CObjectDCLView*)m_pActiveView)->m_StandardCursorID = true;
	CString strLicenseKey;
	if (RequestLicenseKey(strLicenseKey, dlg.m_clsid) == TRUE)
		((CObjectDCLView*)m_pActiveView)->m_sLicenseKey = strLicenseKey;
	else
		((CObjectDCLView*)m_pActiveView)->m_sLicenseKey = CString();
	
	m_nSelectedCtrl = CtlActiveX;	
	
}

void CToolBox::OnToolboxDwglist() 
{
	if (((CObjectDCLView*)m_pActiveView)->m_pThisDclForm == NULL)
	{
		ResetToPointer();
		return;
	}
	
	((CObjectDCLView*)m_pActiveView)->m_StandardCursorID = true;
	m_nSelectedCtrl = CtlDwgList;
	
}


void CToolBox::OnToolboxAnimate() 
{
	if (((CObjectDCLView*)m_pActiveView)->m_pThisDclForm == NULL)
	{
		ResetToPointer();
		return;
	}
	
	((CObjectDCLView*)m_pActiveView)->m_StandardCursorID = true;
	m_nSelectedCtrl = CtlAnimate;
	
	
}

void CToolBox::OnToolboxImagecombobox() 
{
	if (((CObjectDCLView*)m_pActiveView)->m_pThisDclForm == NULL)
	{
		ResetToPointer();
		return;
	}
	
	((CObjectDCLView*)m_pActiveView)->m_StandardCursorID = true;
	m_nSelectedCtrl = CtlImageComboBox;
}

void CToolBox::OnToolboxGrid() 
{
	if (((CObjectDCLView*)m_pActiveView)->m_pThisDclForm == NULL)
	{
		ResetToPointer();
		return;
	}
	
	((CObjectDCLView*)m_pActiveView)->m_StandardCursorID = true;
	m_nSelectedCtrl = CtlGrid;
}

void CToolBox::OnToolboxSlitter() 
{
	if (((CObjectDCLView*)m_pActiveView)->m_pThisDclForm == NULL)
	{
		ResetToPointer();
		return;
	}
	
	((CObjectDCLView*)m_pActiveView)->m_StandardCursorID = true;
	m_nSelectedCtrl = CtlSplitter;
}

void CToolBox::OnToolboxHatch() 
{
	if (((CObjectDCLView*)m_pActiveView)->m_pThisDclForm == NULL)
	{
		ResetToPointer();
		return;
	}
	
	((CObjectDCLView*)m_pActiveView)->m_StandardCursorID = true;
	m_nSelectedCtrl = CtlHatch;
}