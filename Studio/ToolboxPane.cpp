// ToolboxPane.cpp : implementation file
//

#include "stdafx.h"
#include "ToolboxPane.h"
#include "Resource.h"
#include "ControlTypes.h"
#include "StudioDialogObject.h"
#include "DclFormTemplate.h"
#include "TabOrderPane.h"
#include "InsertControlDlg.h"
#include "OleLicenseKey.h"
#include "StudioWorkspace.h"


#define nDeButtonCount 35
#define nDeBitmapSize 21
#define nCommandIDLimit 0xf000


/////////////////////////////////////////////////////////////////////////////
// CToolboxPane

CToolboxPane::CToolboxPane()
: mbToolSelected( false )
, mpActiveDlgObject( NULL )
, mnSelectedCtrl( 1 )
{
}

CToolboxPane::~CToolboxPane()
{
}

ControlType CToolboxPane::GetSelectedTool() const
{
	if( !mbToolSelected )
		return _CtlInvalid;
	return (ControlType)(mnSelectedCtrl);
}

bool CToolboxPane::GetActiveXControlInfo( CLSID& clsid, CString& sLicenseKey, CString& sFilename ) const
{
	if( clsid == GUID_NULL )
		return false;
	clsid = mAxClsid;
	sLicenseKey = msAxLicenseKey;
	sFilename = msAxFileName;
	return true;
}

void CToolboxPane::ResetToPointer()
{
	mToolbar.CheckButton(ID_TOOLBOX_POINTER, TRUE);
	mnSelectedCtrl = 1;
	mbToolSelected = false;
}

void CToolboxPane::UpdateToolUI() 
{
	BOOL bEnable = FALSE;
	BOOL bEnableTabCtrl = FALSE;
	if( mpActiveDlgObject )
	{
		bEnable = TRUE;
		TDclFormPtr pForm = mpActiveDlgObject->GetSourceForm();
		if( pForm->GetType() != FrmTabPage && !pForm->FindFirstControlOfType( CtlTabStrip ) )
			bEnableTabCtrl = TRUE;
	}
	if( !bEnableTabCtrl && GetSelectedTool() == CtlTabStrip )
		ResetToPointer();
	for( int idx = 0; idx < nDeButtonCount; ++idx )
	{
		UINT nID = idx + ID_TOOLBOX_POINTER;
		switch( nID )
		{
			case ID_TOOLBOX_TABS:
				mToolbar.EnableButton( ID_TOOLBOX_TABS, bEnableTabCtrl );
				break;
			default:
				mToolbar.EnableButton( nID, bEnable );
				break;
		}
	}
}

void CToolboxPane::ActivateDlgObject( CStudioDialogObject* pDlgObject )
{
	mpActiveDlgObject = pDlgObject;
	UpdateToolUI();
}

void CToolboxPane::ActivateDclControl( TDclControlPtr pDclControl )
{
	UpdateToolUI();
}


BEGIN_MESSAGE_MAP(CToolboxPane, CDialog)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_NOTIFY_RANGE( TTN_NEEDTEXTA, ID_TOOLBOX_POINTER, ID_TOOLBOX_POINTER + nDeButtonCount, OnNeedTextA)
	ON_NOTIFY_RANGE( TTN_NEEDTEXTW, ID_TOOLBOX_POINTER, ID_TOOLBOX_POINTER + nDeButtonCount, OnNeedTextW)
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
	ON_COMMAND(ID_TOOLBOX_OPTIONLIST, OnToolboxOptionList)
	ON_COMMAND(ID_TOOLBOX_ACTIVEX, OnToolboxActivex)
	ON_COMMAND(ID_TOOLBOX_DWGLIST, OnToolboxDwglist)
	ON_COMMAND(ID_TOOLBOX_ANIMATION, OnToolboxAnimate)
	ON_COMMAND(ID_TOOLBOX_IMAGECOMBOBOX, OnToolboxImagecombobox)
	ON_COMMAND(ID_TOOLBOX_GRID, OnToolboxGrid)
	ON_COMMAND(ID_TOOLBOX_SPLITTER, OnToolboxSlitter)
	ON_COMMAND(ID_TOOLBOX_HATCH, OnToolboxHatch)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CToolboxPane message handlers

BOOL CToolboxPane::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	mToolbar.EnableWindow( TRUE );
	mToolbar.EnableToolTips(TRUE);	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX PropertyObject Pages should return FALSE
}


int CToolboxPane::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	DWORD dwStyle = WS_VISIBLE | WS_DISABLED | WS_CHILD | CCS_NODIVIDER | CCS_NOPARENTALIGN      
			| CCS_ADJUSTABLE | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT | TBSTYLE_WRAPABLE;

	if (!mToolbar.Create( dwStyle, CRect (0,0,2,2), this, IDR_TOOLBOX) )
	{
		TRACE0("Failed to create instant bar child\n");
		return -1;		// fail to create
	}
	mToolbar.AddBitmap(nDeButtonCount,IDR_TOOLBOX);
	mToolbar.SetBitmapSize(CSize(nDeBitmapSize,nDeBitmapSize));
	for (int idx = 0; idx < nDeButtonCount; idx++)
	{
		TBBUTTON btn = 
		{
			idx,
			idx + ID_TOOLBOX_POINTER,
			0,
			BYTE(TBSTYLE_CHECKGROUP),
		};
		VERIFY(mToolbar.AddButtons( 1, &btn ));
	}
	mToolbar.AutoSize();
	return 0;
}

void CToolboxPane::OnSize(UINT nType, int cx, int cy) 
{
	__super::OnSize(nType, cx, cy);
	mToolbar.MoveWindow(0,0,cx,cy,TRUE);
}

void CToolboxPane::OnNeedTextW( UINT nID, NMHDR * pNotifyStruct, LRESULT * lResult )
{
	LPTOOLTIPTEXTW lpTTT = (LPTOOLTIPTEXTW)pNotifyStruct;
  lpTTT->lpszText = MAKEINTRESOURCEW(nID - ID_TOOLBOX_POINTER + IDS_POINTER);
  lpTTT->hinst = AfxGetResourceHandle();
}

void CToolboxPane::OnNeedTextA( UINT nID, NMHDR * pNotifyStruct, LRESULT * lResult )
{
	LPTOOLTIPTEXTA lpTTT = (LPTOOLTIPTEXTA)pNotifyStruct;
  lpTTT->lpszText = MAKEINTRESOURCEA(nID - ID_TOOLBOX_POINTER + IDS_POINTER);
  lpTTT->hinst = AfxGetResourceHandle();
}

BOOL CToolboxPane::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
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

BOOL CToolboxPane::OnCmdMsg(UINT nID, int nCode, void* pExtra,
	AFX_CMDHANDLERINFO* pHandlerInfo)
{
	if (__super::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

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
	mnSelectedCtrl = 1;
}

void CToolboxPane::OnToolboxAngleslider() 
{
	mbToolSelected = true;
	mnSelectedCtrl = CtlAngleSlider;
}

void CToolboxPane::OnToolboxBlocklist() 
{
	mbToolSelected = true;
	mnSelectedCtrl = CtlBlockList;
}

void CToolboxPane::OnToolboxBlockview() 
{
	mbToolSelected = true;
	mnSelectedCtrl = CtlBlockView;
}

void CToolboxPane::OnToolboxCheckbox() 
{
	mbToolSelected = true;
	mnSelectedCtrl = CtlCheckBox;
}

void CToolboxPane::OnToolboxCombobox() 
{
	mbToolSelected = true;
	mnSelectedCtrl = CtlComboBox;
}

void CToolboxPane::OnToolboxDwgpreview() 
{
	mbToolSelected = true;
	mnSelectedCtrl = CtlDwgPreview;
}

void CToolboxPane::OnToolboxFrame() 
{
	mbToolSelected = true;
	mnSelectedCtrl = CtlFrame;
}

void CToolboxPane::OnToolboxGraphicbutton() 
{
	mbToolSelected = true;
	mnSelectedCtrl = CtlGraphicButton;
}

void CToolboxPane::OnToolboxHtml() 
{
	mbToolSelected = true;
	mnSelectedCtrl = CtlHtml;
}

void CToolboxPane::OnToolboxLabel() 
{
	mbToolSelected = true;
	mnSelectedCtrl = CtlLabel;
}

void CToolboxPane::OnToolboxListbox() 
{
	mbToolSelected = true;
	mnSelectedCtrl = CtlListBox;
}

void CToolboxPane::OnToolboxListview() 
{
	mbToolSelected = true;
	mnSelectedCtrl = CtlListView;
}

void CToolboxPane::OnToolboxMonthpicker() 
{
	mbToolSelected = true;
	mnSelectedCtrl = CtlCalendar;
}

void CToolboxPane::OnToolboxOption() 
{
	mbToolSelected = true;
	mnSelectedCtrl = CtlOptionButton;
}

void CToolboxPane::OnToolboxPicturebox() 
{
	mbToolSelected = true;
	mnSelectedCtrl = CtlPictureBox;
}

void CToolboxPane::OnToolboxProgressbar() 
{
	mbToolSelected = true;
	mnSelectedCtrl = CtlProgressBar;
}

void CToolboxPane::OnToolboxRectangle() 
{
	mbToolSelected = true;
	mnSelectedCtrl = CtlRectangle;	
}

void CToolboxPane::OnToolboxScrollbar() 
{
	mbToolSelected = true;
	mnSelectedCtrl = CtlScrollBar;
}

void CToolboxPane::OnToolboxSlider() 
{
	mbToolSelected = true;
	mnSelectedCtrl = CtlSlider;	
}

void CToolboxPane::OnToolboxSlideview() 
{
	mbToolSelected = true;
	mnSelectedCtrl = CtlSlideView;
}

void CToolboxPane::OnToolboxSpinbutton() 
{
	mbToolSelected = true;
	mnSelectedCtrl = CtlSpinButton;
}

void CToolboxPane::OnToolboxTabs() 
{
	mbToolSelected = true;
	mnSelectedCtrl = CtlTabStrip;
}

void CToolboxPane::OnToolboxTextbox() 
{
	mbToolSelected = true;
	mnSelectedCtrl = CtlTextBox;
}

void CToolboxPane::OnToolboxTextbutton() 
{
	mbToolSelected = true;
	mnSelectedCtrl = CtlTextButton;
}

void CToolboxPane::OnToolboxTree() 
{
	mbToolSelected = true;
	mnSelectedCtrl = CtlTree;
}

void CToolboxPane::OnToolboxUrllink() 
{
	mbToolSelected = true;
	mnSelectedCtrl = CtlHyperlink;
}

void CToolboxPane::OnToolboxOptionList() 
{
	mbToolSelected = true;
	mnSelectedCtrl = CtlOptionList;
}

void CToolboxPane::OnToolboxActivex() 
{
	mnSelectedCtrl = CtlActiveX;	
	CInsertControlDlg dlg;
	if( dlg.DoModal() != IDOK )
	{
		ResetToPointer();
		return;
	}

	mAxClsid = dlg.m_clsid;
	msAxFileName = dlg.m_FileName;
	mbToolSelected = true;
	CString sLicenseKey;
	CWaitCursor wait;
	if( RequestLicenseKey( sLicenseKey, dlg.m_clsid ) == TRUE )
		msAxLicenseKey = sLicenseKey;
	else
		msAxLicenseKey.Empty();
}

void CToolboxPane::OnToolboxDwglist() 
{
	mbToolSelected = true;
	mnSelectedCtrl = CtlDwgList;
}

void CToolboxPane::OnToolboxAnimate() 
{
	mbToolSelected = true;
	mnSelectedCtrl = CtlAnimation;
}

void CToolboxPane::OnToolboxImagecombobox() 
{
	mbToolSelected = true;
	mnSelectedCtrl = CtlImageComboBox;
}

void CToolboxPane::OnToolboxGrid() 
{
	mbToolSelected = true;
	mnSelectedCtrl = CtlGrid;
}

void CToolboxPane::OnToolboxSlitter() 
{
	mbToolSelected = true;
	mnSelectedCtrl = CtlSplitter;
}

void CToolboxPane::OnToolboxHatch() 
{
	mbToolSelected = true;
	mnSelectedCtrl = CtlHatch;
}
