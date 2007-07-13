// PropertyTabPane.cpp : implementation file
//

#include "stdafx.h"
#include "PropertyTabPane.h"
#include "Resource.h"
#include "DclControlObject.h"
#include "ControlName.h"
#include "ControlTypes.h"
#include "OpenDCLView.h"
#include "PropertyIds.h"

#define IDC_TABCTRL 109


/////////////////////////////////////////////////////////////////////////////
// CPropertyTabPane

CPropertyTabPane::CPropertyTabPane()
{
}

CPropertyTabPane::~CPropertyTabPane()
{
}


BEGIN_MESSAGE_MAP(CPropertyTabPane, CDialog)
	//{{AFX_MSG_MAP(CPropertyTabPane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TABCTRL, OnSelchange)	
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertyTabPane message handlers

void CPropertyTabPane::SetupTabs()
{
	TC_ITEM TabCtrlItem;
	CString sTTT;
	CString sCaptionText;

	TabCtrlItem.mask = TCIF_TEXT;
	
	// delete all previos tabs
	m_TabCtrl.DeleteAllItems();
	
	
	sCaptionText = theWorkspace.LoadResourceString(IDS_PROPERTIES);
	// set the tab caption
	TabCtrlItem.pszText = sCaptionText.GetBuffer(256);
	// add the new tab
	m_TabCtrl.InsertItem(0, &TabCtrlItem );
	
	sCaptionText = theWorkspace.LoadResourceString(IDS_EVENTS);
	// set the tab caption
	TabCtrlItem.pszText = sCaptionText.GetBuffer(256);
	// add the new tab
	m_TabCtrl.InsertItem(1, &TabCtrlItem );
	
}

int CPropertyTabPane::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | TCS_FOCUSNEVER | TCS_TOOLTIPS	| TCS_TABS | WS_GROUP |
									WS_CLIPSIBLINGS	| WS_CLIPCHILDREN | TCS_MULTILINE;

	if (m_TabCtrl.Create(dwStyle, CRect(0,0,10,10), this, IDC_TABCTRL))
		SetupTabs();

	if (!m_font.CreateStockObject(DEFAULT_GUI_FONT) && !m_font.CreatePointFont(80, _T("MS Sans Serif")))
		return -1;

	// Create the properties tab 
	m_PropertiesTabPane.Create(IDD_TABPAGE_PROPERTIES, &m_TabCtrl);

	// create the events tab
	m_EventsTabPane.Create(IDD_EVENTS, &m_TabCtrl);

	m_TabCtrl.SetFont(&m_font);
	m_EventsTabPane.SetFont(&m_font);
	m_PropertiesTabPane.SetFont(&m_font);
	m_PropertiesTabPane.GetPropertiesCtrl().SetFont(&m_font);
	m_PropertiesTabPane.ShowWindow(TRUE);
	m_EventsTabPane.ShowWindow(FALSE);
	return 0;
}

void CPropertyTabPane::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	CRect rc;
	GetClientRect(&rc);
	m_TabCtrl.MoveWindow(rc, TRUE);
	
	CRect rcTab;
	m_TabCtrl.GetClientRect(&rcTab);

	// get the tab control position
	CRect rcItem;
	
	int nTBottom = 0;
	
	for (int i=0; i < m_TabCtrl.GetItemCount(); i++)
	{
		// get the tab's position
		m_TabCtrl.GetItemRect(i, &rcItem);
		
		if (rcItem.bottom > nTBottom)
			nTBottom = rcItem.bottom;
	}
	
	// set the pane's position
	CRect rcNewRect(
		3,
		nTBottom + 3,
		rcTab.Width() - 6,
		rcTab.Height() - 3);

	m_PropertiesTabPane.MoveWindow(rcNewRect, TRUE);
	m_EventsTabPane.MoveWindow(rcNewRect, TRUE);	
}

void CPropertyTabPane::OnSelchange(NMHDR* pNMHDR, LRESULT* pResult) 
{
	int nCurrentSelectedTab = m_TabCtrl.GetCurSel();
	
	switch (nCurrentSelectedTab)
	{
	case 0:
		m_PropertiesTabPane.ShowWindow(TRUE);
		m_EventsTabPane.ShowWindow(FALSE);
		break;
	case 1:
		m_PropertiesTabPane.ShowWindow(FALSE);
		m_EventsTabPane.ShowWindow(TRUE);
		break;
	}
	*pResult = 0;
}

void CPropertyTabPane::DisplaySelectedControlProperties(CDclControlObject *pControl, COpenDCLView *pView) 
{
	if (m_PropertiesTabPane.GetPropertiesCtrl().m_pControl == pControl)
		return;

	CString sControlName;
	if( pControl )
	{
		CString sControlType = GetControlName(pControl);
		CString sName = pControl->GetStrProperty( nGlobalVarName );
		if( sName.IsEmpty() )
			sName = pControl->GetKeyPath();
		if (!sControlType.IsEmpty())
			sControlName.Format( _T("%s [%s]"), (LPCTSTR)sName, (LPCTSTR)sControlType );
		else
			sControlName = sName;

		// get the old name
		CString sOldName;
		m_PropertiesTabPane.m_ControlDesc.GetWindowText(sOldName);

		// check if we are displaying the properties for the same control
		if (sOldName == sControlName) 
		{
			CRect rc;
			if (pControl->m_pCtrlHolder != NULL)
			{
				pControl->m_pCtrlHolder->GetWindowRect(&rc);
				pControl->m_pCtrlHolder->GetParent()->ScreenToClient(rc);
				if (pControl->m_rcOldPosition == rc)
					return;
			}
			pControl->m_rcOldPosition = rc;
		}
	}

	// set the control description text
	m_PropertiesTabPane.m_ControlDesc.SetWindowText(sControlName);
	// set the CView pointer for updating controls
	m_PropertiesTabPane.GetPropertiesCtrl().m_pView = pView;	
	// display the properties of the control
	m_PropertiesTabPane.GetPropertiesCtrl().DisplayProperties(pControl);

	// set the CView pointer for updating controls
	m_EventsTabPane.ClearEvents();
	// update the events tab pane
	m_EventsTabPane.UpdateEvents(pControl);
	m_EventsTabPane.m_pView = pView;
		
}

void CPropertyTabPane::ClearControlProperties() 
{
	// call the method to clear the propery grid
	if (IsWindow(m_PropertiesTabPane.m_ControlDesc.m_hWnd))
	{
		m_PropertiesTabPane.GetPropertiesCtrl().ClearGrid();
		m_PropertiesTabPane.m_ControlDesc.SetWindowText(_T(""));
		m_PropertiesTabPane.m_PropertyDesc.SetWindowText(_T(""));
		m_PropertiesTabPane.m_PropertyTitle.SetWindowText(_T(""));
		// call the method to clear the event tab page
		m_EventsTabPane.ClearEvents();
	}
}

BOOL CPropertyTabPane::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message== WM_KEYDOWN )
	{
		if (pMsg->wParam==VK_RETURN && !m_PropertiesTabPane.GetPropertiesCtrl().m_Edit.IsWindowVisible())
		{
			pMsg->wParam = NULL;
			pMsg->message = NULL;
		}
	}		
	return CDialog::PreTranslateMessage(pMsg);
}

void CPropertyTabPane::OnDestroy() 
{
	CDialog::OnDestroy();
	
	m_font.DeleteObject();		
}
