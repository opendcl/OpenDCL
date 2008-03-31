// ToolboxPane.cpp : implementation file
//

#include "stdafx.h"
#include "ToolboxPane.h"
#include "Resource.h"
#include "ControlTypes.h"
#include "StudioDialogObject.h"
#include "DclFormObject.h"
#include "ZOrderPane.h"
#include "InsertControlDlg.h"
#include "OleLicenseKey.h"
#include "StudioWorkspace.h"


/////////////////////////////////////////////////////////////////////////////
// CToolboxPane

CToolboxPane::CToolboxPane()
: mbToolSelected( false )
, mpActiveDlgObject( NULL )
{
	m_pTBButtons = NULL;
	m_nSelectedCtrl = 1;
}

CToolboxPane::~CToolboxPane()
{
	delete[] m_pTBButtons;
}

ControlType CToolboxPane::GetSelectedTool() const
{
	if( !mbToolSelected )
		return _CtlInvalid;
	return (ControlType)(m_nSelectedCtrl);
}

bool CToolboxPane::GetActiveXControlInfo( CLSID& clsid, CString& sLicenseKey, CString& sFilename ) const
{
	if( clsid == GUID_NULL )
		return false;
	clsid = m_clsid;
	sLicenseKey = m_sLicenseKey;
	sFilename = m_ActiveXFileName;
	return true;
}

void CToolboxPane::ResetToPointer()
{
	m_ToolBoxButtons.CheckButton(ID_TOOLBOX_POINTER, TRUE);
	m_nSelectedCtrl = 1;
	mbToolSelected = false;
}

void CToolboxPane::UpdateTabStripToolUI() 
{
	BOOL bEnable = FALSE;
	if( mpActiveDlgObject )
	{
		TDclFormPtr pForm = mpActiveDlgObject->GetSourceForm();
		if( !pForm->FindFirstControlOfType( CtlTabStrip ) )
			bEnable = TRUE;
	}
	if( !bEnable && GetSelectedTool() == CtlTabStrip )
		ResetToPointer();
	m_ToolBoxButtons.EnableButton( ID_TOOLBOX_TABS, bEnable );
}

void CToolboxPane::ActivateDlgObject( CStudioDialogObject* pDlgObject )
{
	mpActiveDlgObject = pDlgObject;
	if( !pDlgObject )
	{
		m_ToolBoxButtons.EnableWindow( FALSE );
		return;
	}
	m_ToolBoxButtons.EnableWindow( TRUE );
	UpdateTabStripToolUI();
}

void CToolboxPane::ActivateDclControl( TDclControlPtr pDclControl )
{
	UpdateTabStripToolUI();
}

void CToolboxPane::AddButton(UINT nID)
{	
	int nIndex = nID - ID_TOOLBOX_POINTER;
	if (nID == ID_TOOLBOX_POINTER)
		m_pTBButtons[nButtonIndex].fsState = BYTE(TBSTYLE_CHECKGROUP|TBSTYLE_TOOLTIPS);//TBSTATE_ENABLED|TBSTATE_CHECKED
	else
		m_pTBButtons[nButtonIndex].fsState = BYTE(TBSTYLE_CHECKGROUP|TBSTYLE_TOOLTIPS);//TBSTATE_ENABLED

	m_pTBButtons[nButtonIndex].dwData = 0;
	m_pTBButtons[nButtonIndex].iBitmap = nIndex;
	m_pTBButtons[nButtonIndex].idCommand = nID;
	m_pTBButtons[nButtonIndex].iString = -1;
	nButtonIndex++;
	m_ToolBoxButtons.AddButtons(nButtonIndex, &m_pTBButtons[nButtonIndex]);
}

void CToolboxPane::AddTheButtons()
{	
	m_ToolBoxButtons.AddBitmap(nDeButtonCount,IDR_TOOLBOX);
	m_ToolBoxButtons.SetBitmapSize(CSize(nDeBitmapSize,nDeBitmapSize));
	nButtonIndex = 0;
	m_pTBButtons = new TBBUTTON[nDeButtonCount];
	for (int nIndex = 0; nIndex < nDeButtonCount; nIndex++)
	{
		if (nIndex == nButtonIndex)
			m_pTBButtons[nIndex].fsState = TBSTATE_ENABLED | TBSTATE_CHECKED;
		else
			m_pTBButtons[nIndex].fsState = TBSTATE_ENABLED;

		m_pTBButtons[nIndex].fsStyle = (BYTE)(TBSTYLE_CHECKGROUP | TBSTYLE_TOOLTIPS);
		m_pTBButtons[nIndex].dwData = 0;
		m_pTBButtons[nIndex].iBitmap = nIndex;
		m_pTBButtons[nIndex].idCommand = nIndex + ID_TOOLBOX_POINTER;
		m_pTBButtons[nIndex].iString = -1;		
	}
	
	for (int nIndex = 0; nIndex < nDeButtonCount; nIndex++)
		VERIFY(m_ToolBoxButtons.AddButtons(1, &m_pTBButtons[nIndex]));
}


BEGIN_MESSAGE_MAP(CToolboxPane, CDialog)
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
	ON_COMMAND(ID_TOOLBOX_ANIMATION, OnToolboxAnimate)
	ON_COMMAND(ID_TOOLBOX_IMAGECOMBOBOX, OnToolboxImagecombobox)
	ON_COMMAND(ID_TOOLBOX_GRID, OnToolboxGrid)
	ON_COMMAND(ID_TOOLBOX_SPLITTER, OnToolboxSlitter)
	ON_COMMAND(ID_TOOLBOX_HATCH, OnToolboxHatch)
	ON_NOTIFY_RANGE( TTN_NEEDTEXTA, ID_TOOLBOX_POINTER, ID_TOOLBOX_POINTER + nDeButtonCount, OnNeedTextA)
	ON_NOTIFY_RANGE( TTN_NEEDTEXTW, ID_TOOLBOX_POINTER, ID_TOOLBOX_POINTER + nDeButtonCount, OnNeedTextW)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CToolboxPane message handlers

BOOL CToolboxPane::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_ToolBoxButtons.EnableToolTips(TRUE);	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX PropertyObject Pages should return FALSE
}

void CToolboxPane::OnNeedTextW( UINT nID, NMHDR * pNotifyStruct, LRESULT * lResult )
{
	CString toolTipText = GetTooltipText(nID);
	LPTOOLTIPTEXTW lpTTT = (LPTOOLTIPTEXTW)pNotifyStruct;
#ifdef _UNICODE
	_tcsncpy(lpTTT->szText,(LPCTSTR)toolTipText, sizeof(lpTTT->szText) / sizeof(WCHAR));
#else
	mbstowcs(lpTTT->szText,(LPCTSTR)toolTipText, sizeof(lpTTT->szText) / sizeof(WCHAR));
#endif
}

BOOL CToolboxPane::OnNeedText( UINT id, NMHDR * pTTTStruct, LRESULT * pResult )
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


void CToolboxPane::OnNeedTextA( UINT nID, NMHDR * pNotifyStruct, LRESULT * lResult )
{
	CString toolTipText = GetTooltipText(nID);
	LPTOOLTIPTEXT lpTTT = (LPTOOLTIPTEXT)pNotifyStruct;
	lstrcpyn(lpTTT->szText,toolTipText, _elements(lpTTT->szText));
}

CString CToolboxPane::GetTooltipText( UINT nID )
{
	return theWorkspace.LoadResourceString(nID - ID_TOOLBOX_POINTER + IDS_POINTER);
}


int CToolboxPane::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	DWORD dwStyle = WS_VISIBLE | WS_DISABLED | WS_CHILD | CCS_NODIVIDER | CCS_NOPARENTALIGN      
			| CCS_ADJUSTABLE | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT | TBSTYLE_WRAPABLE;

	if (!m_ToolBoxButtons.Create( dwStyle, CRect (0,0,2,2), this, IDR_TOOLBOX) )
	{
		TRACE0("Failed to create instant bar child\n");
		return -1;		// fail to create
	}
	m_ToolBoxButtons.AutoSize();
	AddTheButtons();
	return 0;
}

void CToolboxPane::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	m_ToolBoxButtons.MoveWindow(0,0,cx,cy,TRUE);
}

BOOL CToolboxPane::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
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

BOOL CToolboxPane::OnCmdMsg(UINT nID, int nCode, void* pExtra,
	AFX_CMDHANDLERINFO* pHandlerInfo)
{
	if (__super::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
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

void CToolboxPane::OnToolboxPointer()
{
	
	mbToolSelected = false;
	m_nSelectedCtrl = 1;
}

void CToolboxPane::OnToolboxAngleslider() 
{
	mbToolSelected = true;
	m_nSelectedCtrl = CtlAngleSlider;
}

void CToolboxPane::OnToolboxBlocklist() 
{
	mbToolSelected = true;
	m_nSelectedCtrl = CtlBlockList;
}

void CToolboxPane::OnToolboxBlockview() 
{
	mbToolSelected = true;
	m_nSelectedCtrl = CtlBlockView;
}

void CToolboxPane::OnToolboxCheckbox() 
{
	mbToolSelected = true;
	m_nSelectedCtrl = CtlCheckBox;
}

void CToolboxPane::OnToolboxCombobox() 
{
	mbToolSelected = true;
	m_nSelectedCtrl = CtlComboBox;
}

void CToolboxPane::OnToolboxDwgpreview() 
{
	mbToolSelected = true;
	m_nSelectedCtrl = CtlDwgPreview;
}

void CToolboxPane::OnToolboxFrame() 
{
	mbToolSelected = true;
	m_nSelectedCtrl = CtlFrame;
}

void CToolboxPane::OnToolboxGraphicbutton() 
{
	mbToolSelected = true;
	m_nSelectedCtrl = CtlGraphicButton;
}

void CToolboxPane::OnToolboxHtml() 
{
	mbToolSelected = true;
	m_nSelectedCtrl = CtlHtmlCtrl;
}

void CToolboxPane::OnToolboxLabel() 
{
	mbToolSelected = true;
	m_nSelectedCtrl = CtlLabel;
}

void CToolboxPane::OnToolboxListbox() 
{
	mbToolSelected = true;
	m_nSelectedCtrl = CtlListBox;
}

void CToolboxPane::OnToolboxListview() 
{
	mbToolSelected = true;
	m_nSelectedCtrl = CtlListView;
}

void CToolboxPane::OnToolboxMonthpicker() 
{
	mbToolSelected = true;
	m_nSelectedCtrl = CtlMonth;
}

void CToolboxPane::OnToolboxOption() 
{
	mbToolSelected = true;
	m_nSelectedCtrl = CtlOptionButton;
}

void CToolboxPane::OnToolboxPicturebox() 
{
	mbToolSelected = true;
	m_nSelectedCtrl = CtlPictureBox;
}

void CToolboxPane::OnToolboxProgressbar() 
{
	mbToolSelected = true;
	m_nSelectedCtrl = CtlProgress;
}

void CToolboxPane::OnToolboxRectangle() 
{
	mbToolSelected = true;
	m_nSelectedCtrl = CtlRectangle;	
}

void CToolboxPane::OnToolboxScrollbar() 
{
	mbToolSelected = true;
	m_nSelectedCtrl = CtlScrollBar;
}

void CToolboxPane::OnToolboxSlider() 
{
	mbToolSelected = true;
	m_nSelectedCtrl = CtlSlider;	
}

void CToolboxPane::OnToolboxSlideview() 
{
	mbToolSelected = true;
	m_nSelectedCtrl = CtlSlideView;
}

void CToolboxPane::OnToolboxSpinbutton() 
{
	mbToolSelected = true;
	m_nSelectedCtrl = CtlSpinButton;
}

void CToolboxPane::OnToolboxTabs() 
{
	mbToolSelected = true;
	m_nSelectedCtrl = CtlTabStrip;
}

void CToolboxPane::OnToolboxTextbox() 
{
	mbToolSelected = true;
	m_nSelectedCtrl = CtlTextBox;
}

void CToolboxPane::OnToolboxTextbutton() 
{
	mbToolSelected = true;
	m_nSelectedCtrl = CtlStdButton;
}

void CToolboxPane::OnToolboxTree() 
{
	mbToolSelected = true;
	m_nSelectedCtrl = CtlTree;
}

void CToolboxPane::OnToolboxUrllink() 
{
	mbToolSelected = true;
	m_nSelectedCtrl = CtlUrlLink;
}

void CToolboxPane::OnToolboxOptionList() 
{
	mbToolSelected = true;
	m_nSelectedCtrl = CtlOptionList;
}

void CToolboxPane::OnToolboxActivex() 
{
	m_nSelectedCtrl = CtlActiveX;	
	CInsertControlDlg dlg;
	if( dlg.DoModal() != IDOK )
	{
		ResetToPointer();
		return;
	}

	m_clsid = dlg.m_clsid;
	m_ActiveXFileName = dlg.m_FileName;
	mbToolSelected = true;
	CString strLicenseKey;
	CWaitCursor wait;
	if (RequestLicenseKey(strLicenseKey, dlg.m_clsid) == TRUE)
		m_sLicenseKey = strLicenseKey;
	else
		m_sLicenseKey.Empty();
	
}

void CToolboxPane::OnToolboxDwglist() 
{
	mbToolSelected = true;
	m_nSelectedCtrl = CtlDwgList;
}


void CToolboxPane::OnToolboxAnimate() 
{
	mbToolSelected = true;
	m_nSelectedCtrl = CtlAnimate;
}

void CToolboxPane::OnToolboxImagecombobox() 
{
	mbToolSelected = true;
	m_nSelectedCtrl = CtlImageComboBox;
}

void CToolboxPane::OnToolboxGrid() 
{
	mbToolSelected = true;
	m_nSelectedCtrl = CtlGrid;
}

void CToolboxPane::OnToolboxSlitter() 
{
	mbToolSelected = true;
	m_nSelectedCtrl = CtlSplitter;
}

void CToolboxPane::OnToolboxHatch() 
{
	mbToolSelected = true;
	m_nSelectedCtrl = CtlHatch;
}
