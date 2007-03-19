// MainFrameToolBar.cpp : implementation file
//

#include "stdafx.h"
#include "MainFrameToolBar.h"
#include "ObjectDCLView.h"
#include "PropertyIds.h"
#include "DclControlObject.h"
#include "PropertyObject.h"
#include "ControlTypes.h"
#include "ControlHolder.h"
#include "OleFont.h"
#include "AxContainerCtrl.h"
#include "AxPropertyDescriptor.h"
#include "AxInterfaceDescriptor.h"
#include "ToolBox.h"
#include "EditorWorkspace.h"
#include "SharedRes.h"


static CString LTOA(int nVal)
{
  CString sLong;
	sLong.Format(_T("%d"), nVal);
  return sLong;
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrameToolBar dialog
#define IDC_FONT2 109
#define IDC_FONTSIZE2 110
#define ID_TOOLBARBTNS 111

CMainFrameToolBar::CMainFrameToolBar()
	: CDialogBar()
{
	//{{AFX_DATA_INIT(CMainFrameToolBar)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pTBButtons = NULL;
	m_pActiveView = NULL;
	m_bScaled = false;
}

	
	
void CMainFrameToolBar::DoDataExchange(CDataExchange* pDX)
{
	CDialogBar::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMainFrameToolBar)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMainFrameToolBar, CDialogBar)
	//{{AFX_MSG_MAP(CMainFrameToolBar)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()	
	ON_COMMAND(ID_FONTBOLDBTN, OnFontBold)
	ON_COMMAND(ID_FONTITALICBTN, OnFontItalic)
	ON_COMMAND(ID_FONTUNDERLINEBTN, OnFontUnderline)
	ON_COMMAND(ID_FONTSCALED, OnFontScaled)
	
	//}}AFX_MSG_MAP
	ON_NOTIFY_RANGE( TTN_NEEDTEXTA, ID_FONTBOLDBTN, ID_FONTSCALED, OnNeedTextA)
	ON_NOTIFY_RANGE( TTN_NEEDTEXTW, ID_FONTBOLDBTN, ID_FONTSCALED, OnNeedTextW)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMainFrameToolBar message handlers

int CMainFrameToolBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialogBar::OnCreate(lpCreateStruct) == nNotSet)
		return nNotSet;
	
	Setup();

	return 0;
}

void CMainFrameToolBar::Setup()
{
	CString sDefaultFont;
	sDefaultFont = theWorkspace.LoadResourceString(IDS_DEFAULTFONT);

	if (!m_font.CreateStockObject(DEFAULT_GUI_FONT))
		if (!m_font.CreatePointFont(80, sDefaultFont))
			return;

	DWORD dwStyle = WS_VISIBLE | WS_CHILD | CCS_NODIVIDER | CCS_NOPARENTALIGN      
		| CCS_ADJUSTABLE | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT | TBSTYLE_WRAPABLE;

	if (!m_Buttons.Create( dwStyle, CRect (0,2,317,26), this, ID_TOOLBARBTNS) )
	{
		return;
	}

	// enable the toolbox
	m_Buttons.EnableWindow(TRUE);
	m_Buttons.ShowWindow(TRUE);
	m_Buttons.AutoSize();
	
	// call the method to add the buttons to the toolbar
	AddTheButtons();

	if(m_FontNames.Create(WS_VISIBLE|WS_CHILD|CBS_DROPDOWNLIST | CBS_AUTOHSCROLL |
                    CBS_OWNERDRAWFIXED | CBS_SORT | CBS_HASSTRINGS | 
                    WS_VSCROLL | WS_TABSTOP,
					CRect(2,1,183,175),&m_Buttons, IDC_FONT2) == TRUE)
	{
		m_FontNames.SetFont(&m_font);
	}

	if(m_FontSizes.Create(WS_VISIBLE|WS_CHILD|CBS_DROPDOWN | CBS_AUTOHSCROLL | 
                    WS_TABSTOP | WS_VSCROLL,
					CRect(184,1,225,155),&m_Buttons, IDC_FONTSIZE2) == TRUE)
	{
		m_FontSizes.SetFont(&m_font);

		// Fill Size combobox with "common" sizes
		TCHAR* Defaults[] = { _T("8"), _T("9"), _T("10"), _T("11"), _T("12"), _T("14"),
					_T("16"), _T("18"), _T("20"), _T("22"), _T("24"), _T("26"), 
					_T("28"), _T("36"), _T("48") };
		for (int i = 0; i < (sizeof(Defaults)/sizeof(Defaults[0])); i++)
			m_FontSizes.AddString(Defaults[i]);

	}

	return;
}


void CMainFrameToolBar::AddTheButtons()
{
	int m_nButtonCount = nDeMainButtonCount;

	m_Buttons.AddBitmap(m_nButtonCount,IDR_FONTBTNS);
	m_Buttons.SetBitmapSize(CSize(nDeBtnSizeH,nDeBtnSizeW));
	m_pTBButtons = new TBBUTTON[m_nButtonCount];

	for (int i= 0; i<10; i++)
	{
		m_pTBButtons[i].fsState = NULL;
		m_pTBButtons[i].fsStyle = BYTE(TBSTYLE_BUTTON|TBSTYLE_TOOLTIPS);
		m_pTBButtons[i].dwData = 0;
		m_pTBButtons[i].iBitmap = 4;
		m_pTBButtons[i].idCommand = ID_BUTTON12909;
		m_pTBButtons[i].iString = 0;		
		VERIFY(m_Buttons.AddButtons(1,&m_pTBButtons[i]));		
	}
	
	m_pTBButtons[10].fsState = TBSTATE_ENABLED;
	m_pTBButtons[10].fsStyle = BYTE(TBSTYLE_CHECK|TBSTYLE_TOOLTIPS);
	m_pTBButtons[10].dwData = 0;
	m_pTBButtons[10].iBitmap = 0;
	m_pTBButtons[10].idCommand = ID_FONTBOLDBTN;
	m_pTBButtons[10].iString = ID_FONTBOLDBTN;		
	VERIFY(m_Buttons.AddButtons(1,&m_pTBButtons[10]));		


	m_pTBButtons[11].fsState = TBSTATE_ENABLED;
	m_pTBButtons[11].fsStyle = BYTE(TBSTYLE_CHECK|TBSTYLE_TOOLTIPS);
	m_pTBButtons[11].dwData = 0;
	m_pTBButtons[11].iBitmap = 1;
	m_pTBButtons[11].idCommand = ID_FONTITALICBTN;
	m_pTBButtons[11].iString = ID_FONTITALICBTN;		
	VERIFY(m_Buttons.AddButtons(1,&m_pTBButtons[11]));		

	m_pTBButtons[12].fsState = TBSTATE_ENABLED;
	m_pTBButtons[12].fsStyle = BYTE(TBSTYLE_CHECK|TBSTYLE_TOOLTIPS);
	m_pTBButtons[12].dwData = 0;
	m_pTBButtons[12].iBitmap = 2;
	m_pTBButtons[12].idCommand = ID_FONTUNDERLINEBTN;
	m_pTBButtons[12].iString = ID_FONTUNDERLINEBTN;			
	VERIFY(m_Buttons.AddButtons(1,&m_pTBButtons[12]));		
	
	m_pTBButtons[13].fsState = TBSTATE_ENABLED;
	m_pTBButtons[13].fsStyle = BYTE(TBSTYLE_CHECK|TBSTYLE_TOOLTIPS);
	m_pTBButtons[13].dwData = 0;
	m_pTBButtons[13].iBitmap = 3;
	m_pTBButtons[13].idCommand = ID_FONTSCALED;
	m_pTBButtons[13].iString = ID_FONTSCALED;		
	VERIFY(m_Buttons.AddButtons(1,&m_pTBButtons[13]));

	
}



void CMainFrameToolBar::SetFontToolBar(CDclControlObject *pCtrl)
{
	if (pCtrl == NULL)
	{
		m_Buttons.SetState(ID_FONTBOLDBTN, TBSTATE_ENABLED);
		m_Buttons.SetState(ID_FONTITALICBTN, TBSTATE_ENABLED);
		m_Buttons.SetState(ID_FONTUNDERLINEBTN, TBSTATE_ENABLED);
		m_Buttons.SetState(ID_FONTSCALED, TBSTATE_ENABLED);
		m_FontNames.SetCurSel(nNotSet);
		m_FontSizes.SetCurSel(nNotSet);
		m_FontSizes.SetWindowText(CString());
		return;
	}

	if (pCtrl->GetType() == CtlActiveX)
	{
		CString	sFontName;
		CY cyFontSize;
		try
		{
			RefCountedPtr< CPropertyObject > pProp;
			RefCountedPtr< CPropertyObject > pFontProp = NULL;
			POSITION pos = pCtrl->GetPropertyList().GetHeadPosition();
			while (pos != NULL)
			{
				pProp = pCtrl->GetPropertyList().GetNext(pos);
				if (pProp->GetType() == PropActiveXProp)
				{
					if (pProp->GetAxInterfaceDescriptorPtr()->GetGuid() == IID_IFontDisp ||
							pProp->GetAxInterfaceDescriptorPtr()->GetGuid() == IID_IFont)
					{
						pFontProp = pProp;
						break;
					}
				}
			}
			if (pFontProp == NULL) 
				return;

			CAxContainerCtrl *axContainer = ((CControlHolder*)pCtrl->m_pCtrlHolder)->GetActiveXCtrl();
			COleFont font = axContainer->GetFont( pFontProp->GetAxInterfaceDescriptorPtr()->GetGetDispId());
			sFontName = font.GetName();
			cyFontSize = font.GetSize();
			m_bBold = (font.GetBold() == TRUE);
			m_bUnderline = (font.GetUnderline() == TRUE);
			m_bScaled = false;
			m_bItalic = (font.GetItalic() == TRUE);
			m_nFontWeight = font.GetWeight();
			m_nFontCharset = font.GetCharset();
		}
		catch (...)
		{
			return;
		}
		// set the bold button state
		if (m_bBold)
			m_Buttons.SetState(ID_FONTBOLDBTN, TBSTATE_CHECKED|TBSTATE_ENABLED);
		else
			m_Buttons.SetState(ID_FONTBOLDBTN, TBSTATE_ENABLED);
		
		
		// set the Italic button state
		if (m_bItalic)
			m_Buttons.SetState(ID_FONTITALICBTN, TBSTATE_CHECKED|TBSTATE_ENABLED);
		else
			m_Buttons.SetState(ID_FONTITALICBTN, TBSTATE_ENABLED);

		// set the Underline button state
		if (m_bUnderline)
			m_Buttons.SetState(ID_FONTUNDERLINEBTN, TBSTATE_CHECKED|TBSTATE_ENABLED);
		else
			m_Buttons.SetState(ID_FONTUNDERLINEBTN, TBSTATE_ENABLED);
		
		// set the Scaled button state
		if (m_bScaled)
			m_Buttons.SetState(ID_FONTSCALED, TBSTATE_CHECKED|TBSTATE_ENABLED);
		else
			m_Buttons.SetState(ID_FONTSCALED, TBSTATE_ENABLED);
		
		// select the font
		m_FontNames.SelectString(0, sFontName);
		
		// select the size
		COleCurrency size = COleCurrency( 10/cyFontSize.Lo, 
		  (10%cyFontSize.Lo) / 1000);

		int nSize = cyFontSize.Lo / 10000;		
		if (nSize < 0)
			nSize = nSize * nNotSet;
		
		m_FontSizes.SetWindowText(LTOA(nSize));

		return;
	}

	// else if pCtrl->GetType() != CtlActiveX
	m_bBold = (pCtrl->GetBoolProperty(nLabelBold) == TRUE);
	m_bItalic = (pCtrl->GetBoolProperty(nLabelItalic) == TRUE);
	m_bUnderline = (pCtrl->GetBoolProperty(nLabelUnderline) == TRUE);
	m_bScaled = (pCtrl->GetBoolProperty(nFontSizeStyle) == TRUE);
	
	if (pCtrl->GetPropertyObject(nLabelBold) == NULL)
	{
		m_bBold = FALSE;
		m_bItalic = FALSE;
		m_bUnderline = FALSE;
		m_bScaled = FALSE;
	}

	// set the bold button state
	if (m_bBold)
		m_Buttons.SetState(ID_FONTBOLDBTN, TBSTATE_CHECKED|TBSTATE_ENABLED);
	else
		m_Buttons.SetState(ID_FONTBOLDBTN, TBSTATE_ENABLED);
	
	
	// set the Italic button state
	if (m_bItalic)
		m_Buttons.SetState(ID_FONTITALICBTN, TBSTATE_CHECKED|TBSTATE_ENABLED);
	else
		m_Buttons.SetState(ID_FONTITALICBTN, TBSTATE_ENABLED);

	// set the Underline button state
	if (m_bUnderline)
		m_Buttons.SetState(ID_FONTUNDERLINEBTN, TBSTATE_CHECKED|TBSTATE_ENABLED);
	else
		m_Buttons.SetState(ID_FONTUNDERLINEBTN, TBSTATE_ENABLED);
	
	// set the scaled button state
	if (m_bScaled)
		m_Buttons.SetState(ID_FONTSCALED, TBSTATE_CHECKED|TBSTATE_ENABLED);
	else
		m_Buttons.SetState(ID_FONTSCALED, TBSTATE_ENABLED);
	
	if (pCtrl->GetPropertyObject(nLabelBold) == NULL)
	{
		m_FontNames.SetWindowText(CString());
	}
	else
	{
		// select the font
		m_FontNames.SelectString(0, pCtrl->GetStrProperty(nLabelName));

		// select the size
		int nSize = pCtrl->GetLngProperty(nLabelSize);
		if (nSize < 0)
			nSize = nSize * nNotSet;
		m_FontSizes.SetWindowText(LTOA(nSize));
	}
}

void CMainFrameToolBar::AddFontToToolBar(CDclControlObject *pCtrl)
{
	if (pCtrl == NULL || pCtrl->GetPropertyObject(nLabelBold) == NULL)
		return;

	if (pCtrl->GetType() == CtlActiveX)
	{
		CString	sFontName;
		CY cyFontSize;
		try
		{
			RefCountedPtr< CPropertyObject > pProp;
			RefCountedPtr< CPropertyObject > pFontProp = NULL;
			POSITION pos = pCtrl->GetPropertyList().GetHeadPosition();
			while (pos != NULL)
			{
				pProp = pCtrl->GetPropertyList().GetNext(pos);
				if (pProp->GetType() == PropActiveXProp)
				{
					if (pProp->GetAxInterfaceDescriptorPtr()->GetGuid() == IID_IFontDisp ||
							pProp->GetAxInterfaceDescriptorPtr()->GetGuid() == IID_IFont)
					{
						pFontProp = pProp;
						break;
					}
				}
			}
			if (pFontProp == NULL) 
				return;

			CAxContainerCtrl *axContainer = ((CControlHolder*)pCtrl->m_pCtrlHolder)->GetActiveXCtrl();
			COleFont font = axContainer->GetFont(pFontProp->GetAxInterfaceDescriptorPtr()->GetGetDispId());
			sFontName = font.GetName();
			cyFontSize = font.GetSize();
			m_bBold = (font.GetBold() == TRUE);
			m_bUnderline = (font.GetUnderline() == TRUE);
			m_bItalic = (font.GetItalic() == TRUE);
			m_bScaled = false;
			m_nFontWeight = font.GetWeight();
			m_nFontCharset = font.GetCharset();
		}
		catch (...)
		{
			return;
		}
		// set the bold button state
		if (m_bBold)
			m_Buttons.SetState(ID_FONTBOLDBTN, TBSTATE_CHECKED|TBSTATE_ENABLED);
		else
			m_Buttons.SetState(ID_FONTBOLDBTN, TBSTATE_ENABLED);
		
		
		// set the Italic button state
		if (m_bItalic)
			m_Buttons.SetState(ID_FONTITALICBTN, TBSTATE_CHECKED|TBSTATE_ENABLED);
		else
			m_Buttons.SetState(ID_FONTITALICBTN, TBSTATE_ENABLED);

		// set the Underline button state
		if (m_bUnderline)
			m_Buttons.SetState(ID_FONTUNDERLINEBTN, TBSTATE_CHECKED|TBSTATE_ENABLED);
		else
			m_Buttons.SetState(ID_FONTUNDERLINEBTN, TBSTATE_ENABLED);
		
		// set the scaled button state
		if (m_bScaled)
			m_Buttons.SetState(ID_FONTSCALED, TBSTATE_CHECKED|TBSTATE_ENABLED);
		else
			m_Buttons.SetState(ID_FONTSCALED, TBSTATE_ENABLED);
		
		// get the selected text 
		int nSel = m_FontNames.GetCurSel();
		if (nSel > nNotSet)
		{
			CString sText;
			m_FontNames.GetLBText(nSel, sText);

			// here we need to make sure that if the property does not equal the existing property it must be blanked.
			if (sText != sFontName)
				// blank out the font
				m_FontNames.SetCurSel(nNotSet);
			else	
				// select the font
				m_FontNames.SelectString(0, sFontName);
		}
		
		
		// select the size
		COleCurrency size = COleCurrency( 10/cyFontSize.Lo, 
		  (10%cyFontSize.Lo) / 1000);

		int nSize = cyFontSize.Lo / 10000;		
		if (nSize < 0)
			nSize = nSize * nNotSet;
		
		m_FontSizes.SetWindowText(LTOA(nSize));
		// get the selected text 
		nSel = m_FontSizes.GetCurSel();
		if (nSel > nNotSet)
		{
			CString sText;
			m_FontSizes.GetWindowText(sText);

			// here we need to make sure that if the property does not equal the existing property it must be blanked.
			if (_tstol(sText) != nSize)
				// blank out the font
				m_FontSizes.SetCurSel(nNotSet);
			else	
			{
				// select the size
				m_FontSizes.SetWindowText(LTOA(nSize));
			}
		}

		return;
	}

	// here we need to make sure that if the property does not equal the existing property it must be blanked.
	if (m_bBold != (pCtrl->GetBoolProperty(nLabelBold) == TRUE))
		m_bBold = false;
	else
		m_bBold = (pCtrl->GetBoolProperty(nLabelBold) == TRUE);
	if (m_bItalic != (pCtrl->GetBoolProperty(nLabelItalic) == TRUE))
		m_bItalic = false;
	else
		m_bItalic = (pCtrl->GetBoolProperty(nLabelItalic) == TRUE);
	
	if (m_bUnderline != (pCtrl->GetBoolProperty(nLabelUnderline) == TRUE))
		m_bUnderline = false;
	else
		m_bUnderline = (pCtrl->GetBoolProperty(nLabelUnderline) == TRUE);
	
	
	if (m_bScaled != (pCtrl->GetBoolProperty(nFontSizeStyle) == TRUE))
	{
		m_bScaled = false;
	}
	else
		m_bScaled = (pCtrl->GetBoolProperty(nFontSizeStyle) == TRUE);
	
	

	// set the bold button state
	if (m_bBold)
		m_Buttons.SetState(ID_FONTBOLDBTN, TBSTATE_CHECKED|TBSTATE_ENABLED);
	else
		m_Buttons.SetState(ID_FONTBOLDBTN, TBSTATE_ENABLED);
	
	// set the Italic button state
	if (m_bItalic)
		m_Buttons.SetState(ID_FONTITALICBTN, TBSTATE_CHECKED|TBSTATE_ENABLED);
	else
		m_Buttons.SetState(ID_FONTITALICBTN, TBSTATE_ENABLED);

	// set the Underline button state
	if (m_bUnderline)
		m_Buttons.SetState(ID_FONTUNDERLINEBTN, TBSTATE_CHECKED|TBSTATE_ENABLED);
	else
		m_Buttons.SetState(ID_FONTUNDERLINEBTN, TBSTATE_ENABLED);
	
	// set the Scaled button state
	if (m_bScaled)
		m_Buttons.SetState(ID_FONTSCALED, TBSTATE_CHECKED|TBSTATE_ENABLED);
	else
		m_Buttons.SetState(ID_FONTSCALED, TBSTATE_ENABLED);
	
	// get the selected text 
	int nSel = m_FontNames.GetCurSel();
	if (nSel > nNotSet)
	{
		CString sText;
		m_FontNames.GetLBText(nSel, sText);

		// here we need to make sure that if the property does not equal the existing property it must be blanked.
		if (sText != pCtrl->GetStrProperty(nLabelName))
			// blank out the font
			m_FontNames.SetCurSel(nNotSet);
		else	
			// select the font
			m_FontNames.SelectString(0, pCtrl->GetStrProperty(nLabelName));
	}
	
	// get the selected text 
	nSel = m_FontSizes.GetCurSel();
	if (nSel > nNotSet)
	{
		CString sText;
		m_FontSizes.GetWindowText(sText);

		// here we need to make sure that if the property does not equal the existing property it must be blanked.
		if (_tstol(sText) != pCtrl->GetLngProperty(nLabelSize))
			// blank out the font
			m_FontSizes.SetCurSel(nNotSet);
		else	
		{
			// select the size
			int nSize = pCtrl->GetLngProperty(nLabelSize);
			if (nSize < 0)
				nSize = nSize * nNotSet;
			m_FontSizes.SetWindowText(LTOA(nSize));
		}
	}
}

void CMainFrameToolBar::OnDestroy() 
{
	CDialogBar::OnDestroy();
	
	// delete the button info holders
	m_font.DeleteObject();			
	if (m_pTBButtons)
		delete []m_pTBButtons;
}



void CMainFrameToolBar::OnSize(UINT nType, int cx, int cy) 
{
	CDialogBar::OnSize(nType, cx, cy);
	
	m_Buttons.MoveWindow(0,2,cx,cy,TRUE);
	
}


CString CMainFrameToolBar::NeedText( UINT nID, NMHDR * pNotifyStruct, LRESULT * lResult )
{
	LPTOOLTIPTEXT lpTTT = (LPTOOLTIPTEXT)pNotifyStruct ;
	ASSERT(nID == lpTTT->hdr.idFrom);

	CString toolTipText;

	switch (nID)
	{
	case ID_FONTBOLDBTN:
		toolTipText = theWorkspace.LoadResourceString(IDS_STRINGBOLD);
		break;
	case ID_FONTITALICBTN:
		toolTipText = theWorkspace.LoadResourceString(IDS_STRINGITALIC);
		break;
	case ID_FONTUNDERLINEBTN:
		toolTipText = theWorkspace.LoadResourceString(IDS_STRINGUL);
		break;
	case ID_FONTSCALED:
		toolTipText = theWorkspace.LoadResourceString(IDS_STRINGSCALED);
		break;		
	}

	int nLength = (toolTipText.GetLength() > nDeTTTLen) ? nDeTTTLen : toolTipText.GetLength();

	toolTipText = toolTipText.Left(nLength);

	return toolTipText;
}



/////////////////////////////////////////////////////////////////////////////
// CToolBar message handlers
void CMainFrameToolBar::OnNeedTextW( UINT nID, NMHDR * pNotifyStruct, LRESULT * lResult )
{
	CString toolTipText = NeedText(nID, pNotifyStruct, lResult);

	LPTOOLTIPTEXTW lpTTT = (LPTOOLTIPTEXTW)pNotifyStruct;

#ifdef _UNICODE
	_tcsncpy(lpTTT->szText,(LPCTSTR)toolTipText, toolTipText.GetLength() + 1);
#else
	mbstowcs(lpTTT->szText,(LPCTSTR)toolTipText, toolTipText.GetLength() + 1);
#endif
}

BOOL CMainFrameToolBar::OnNeedText( UINT id, NMHDR * pTTTStruct, LRESULT * pResult )
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


void CMainFrameToolBar::OnNeedTextA( UINT nID, NMHDR * pNotifyStruct, LRESULT * lResult )
{
	CString toolTipText = NeedText(nID, pNotifyStruct, lResult);

	LPTOOLTIPTEXT lpTTT = (LPTOOLTIPTEXT)pNotifyStruct;

	lstrcpyn(lpTTT->szText,toolTipText, _elements(lpTTT->szText));
}

BOOL CMainFrameToolBar::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	ASSERT(pResult != NULL);
	NMHDR* pNMHDR = (NMHDR*)lParam;
	HWND hWndCtrl = pNMHDR->hwndFrom;

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
	
	return CDialogBar::OnNotify(wParam, lParam, pResult);
}

void CMainFrameToolBar::SetActiveView(CView *pView) 
{
	m_pActiveView = pView;
	EnableWindow( (pView != NULL) );
}

void CMainFrameToolBar::OnFontBold()
{		
	if (theEditorWorkspace.GetToolBox()->m_pActiveView == NULL)
		return;
	if (((CObjectDCLView*)theEditorWorkspace.GetToolBox()->m_pActiveView)->m_pThisDclForm == NULL)
		return;
	
	BOOL bChecked = m_Buttons.IsButtonChecked(ID_FONTBOLDBTN);
	((CObjectDCLView*)theEditorWorkspace.GetToolBox()->m_pActiveView)->UpdateFontBool((bChecked == TRUE), nLabelBold);
	
}
void CMainFrameToolBar::OnFontItalic()
{
	if (theEditorWorkspace.GetToolBox()->m_pActiveView == NULL)
		return;
	if (((CObjectDCLView*)theEditorWorkspace.GetToolBox()->m_pActiveView)->m_pThisDclForm == NULL)
		return;
	
	BOOL bChecked = m_Buttons.IsButtonChecked(ID_FONTITALICBTN);
	((CObjectDCLView*)theEditorWorkspace.GetToolBox()->m_pActiveView)->UpdateFontBool((bChecked == TRUE), nLabelItalic);
	
}
void CMainFrameToolBar::OnFontUnderline()
{
	if (theEditorWorkspace.GetToolBox()->m_pActiveView == NULL)
		return;
	if (((CObjectDCLView*)theEditorWorkspace.GetToolBox()->m_pActiveView)->m_pThisDclForm == NULL)
		return;
	
	
	BOOL bChecked = m_Buttons.IsButtonChecked(ID_FONTUNDERLINEBTN);
	((CObjectDCLView*)theEditorWorkspace.GetToolBox()->m_pActiveView)->UpdateFontBool((bChecked == TRUE), nLabelUnderline);
	
}


void CMainFrameToolBar::OnFontScaled()
{
	if (theEditorWorkspace.GetToolBox()->m_pActiveView == NULL)
		return;
	if (((CObjectDCLView*)theEditorWorkspace.GetToolBox()->m_pActiveView)->m_pThisDclForm == NULL)
		return;
	
	
	BOOL bChecked = m_Buttons.IsButtonChecked(ID_FONTSCALED);
	((CObjectDCLView*)theEditorWorkspace.GetToolBox()->m_pActiveView)->UpdateFontBool((bChecked == TRUE), nFontSizeStyle);
	
}
