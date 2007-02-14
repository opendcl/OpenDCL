// VdclTab.cpp : implementation file
//

#include "stdafx.h"
#include "VdclTab.h"
#include "Resource.h"
#include "InvokeMethod.h"
#include "ControlPane.h"
#include "PropertyIds.h"
#include "DclControlObject.h"
#include "PropertyObject.h"
#include "TabPage.h"
#include "DclFormObject.h"

const int IDC_TAB = 94;


/////////////////////////////////////////////////////////////////////////////
// VdclTab

VdclTab::VdclTab()
{
	m_ArxTabsList.RemoveAll();
	m_nCurrentSelectedTab = -1;
	m_rcPos.left = 0;
	m_rcPos.top = 0;
	m_rcPos.right = 200;
	m_rcPos.bottom = 200;
	// No tooltip created
	m_ToolTip.m_hWnd = NULL;
	m_ToolTipsUpdated = false;

}

VdclTab::~VdclTab()
{
}


BEGIN_MESSAGE_MAP(VdclTab, CStatic)
	//{{AFX_MSG_MAP(VdclTab)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB, OnSelchange)
	ON_NOTIFY(TCN_SELCHANGING, IDC_TAB, OnSelchanging)	
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VdclTab message handlers



BOOL VdclTab::Create(CDclControlObject* pControl, CWnd* pParentWnd, UINT nID ) 
{
	DWORD dwStyle;

	// set the arx control pointer
    m_ArxControl = pControl;
	
	// get the rectangle of the new control
	m_rcPos.top = pControl->m_pTop->GetLongValue();
	m_rcPos.left = pControl->m_pLeft->GetLongValue();
	m_rcPos.bottom = pControl->m_pHeight->GetLongValue() + m_rcPos.top;
	m_rcPos.right = pControl->m_pWidth->GetLongValue() + m_rcPos.left;
	
	dwStyle = WS_CHILD	| WS_VISIBLE		| TCS_FOCUSNEVER 
		| TCS_TOOLTIPS			 
		| WS_GROUP		| WS_CLIPSIBLINGS	| WS_CLIPCHILDREN;

	if (pControl->GetLngProperty(nTabStyle) == 0)		
		dwStyle |= TCS_TABS;
	else
		dwStyle |= TCS_BUTTONS;
	
	
	if (pControl->GetBoolProperty(nMultiRow))
		dwStyle = dwStyle | TCS_MULTILINE;
	else
		dwStyle = dwStyle | TCS_SINGLELINE;
	
	if (pControl->GetBoolProperty(nTabFixedWidth) == TRUE)
		dwStyle = dwStyle |  TCS_FIXEDWIDTH;
		

	BOOL RetVal = CStatic::Create(
		CString(), 
		 WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		m_rcPos,
		pParentWnd,
		nID);
	
	VERIFY(CStatic::SubclassDlgItem(nID, pParentWnd));
	
	CRect rcChild(0,0,m_rcPos.Width(), m_rcPos.Height());
	
	RetVal = m_Child.Create(dwStyle, rcChild, this, IDC_TAB);
	VERIFY(m_Child.SubclassDlgItem(IDC_TAB, this));
	
	switch (m_ArxControl->GetLngProperty(nEventInvoke))
	{
	case 1:
		m_bInvokeWithSendString = true;
		break;
	default:
		m_bInvokeWithSendString = false;
		break;
	}
	//call method to setup the tabs
	//SetupTabs();
	
	
	return RetVal;
}

void VdclTab::SetupTabs()
{
	CString sText;
	TC_ITEM TabCtrlItem;
	CString sTTT;
	TabCtrlItem.mask = TCIF_TEXT;
	
	// delete all previos tabs
	m_Child.DeleteAllItems();

	// get the tab's lists
	RefCountedPtr< CPropertyObject > pTabsCaptionProperty = m_ArxControl->GetPropertyObject(nTabsCaption);
	RefCountedPtr< CPropertyObject > pTabsTTTProperty = m_ArxControl->GetPropertyObject(nTabsTTT);

	size_t nTabQty = pTabsCaptionProperty->size();
	int nImageListIndex = m_ArxControl->GetImageListIndex();

  size_t i;
	for (i = 0; i < nTabQty; i++)
	{
		CString Tab = m_ArxControl->GetPropertyListItem(nTabsCaption, i);
					
		// get the tag caption
		TabCtrlItem.pszText = Tab.GetBuffer(256);

		// set the image list item number is required
		if (nImageListIndex > -1)
			TabCtrlItem.iImage = _ttol(m_ArxControl->GetPropertyListItem(nTabsImageList, i));
					
		// add the new tab
		m_Child.InsertItem(i, &TabCtrlItem );

		//  Get the current tab item text.
		TC_ITEM tcItem;
		tcItem.mask = TCIF_PARAM;
		m_Child.GetItem(i, &tcItem);
		
		tcItem.lParam = (short)i;
		
		m_Child.SetItemExtra(sizeof(short));
		//  Set the item in the tab control.
		m_Child.SetItem(i, &tcItem);

	}
	
	InitToolTip();
	m_Child.SetToolTips(&m_ToolTip);
	
	for (i = 0; i < nTabQty; i++)
	{
		CRect r;
		sText = m_ArxControl->GetPropertyListItem(nTabsTTT, i);
		m_Child.GetItemRect(i, &r);
		m_ToolTip.AddTool(&m_Child, sText, &r, i);
	}

	
   // Activate the tooltip control.
   m_ToolTip.Activate(TRUE);
	

}


void VdclTab::HideTab(int nIndex)
{
	// ensure the tab is not already removed..
	for (int i=0; i<m_Child.GetItemCount(); i++)
	{
		//  Get the current tab item text.
		TC_ITEM tcItem;
		tcItem.mask = TCIF_PARAM;
		m_Child.GetItem(i, &tcItem);
		
		if (nIndex == tcItem.lParam)
		{
			m_Child.DeleteItem(nIndex);
			return;
		}
	}	
}

void VdclTab::ShowTab(int nIndex)
{
	// ensure the tab is not already showing.
	for (int i=0; i<m_Child.GetItemCount(); i++)
	{
		//  Get the current tab item text.
		TC_ITEM tcItem;
		tcItem.mask = TCIF_PARAM;
		m_Child.GetItem(i, &tcItem);
		
		if (nIndex == tcItem.lParam)
			return;
	}

	CString sTab = m_ArxControl->GetPropertyListItem(nTabsCaption, nIndex);
				
	// set the image list item number is required
	int nImage = _ttol(m_ArxControl->GetPropertyListItem(nTabsImageList, nIndex));
				
	// add the new tab
	m_Child.InsertItem(nIndex, sTab, nImage);

	//  Get the current tab item text.
	TC_ITEM tcItem;
	tcItem.mask = TCIF_PARAM;
	m_Child.GetItem(nIndex, &tcItem);
	
	tcItem.lParam = (short)nIndex;
	
	m_Child.SetItemExtra(sizeof(short));
	//  Set the item in the tab control.
	m_Child.SetItem(nIndex, &tcItem);

}

void VdclTab::OnSelchange(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CString sTTT;
	
	int nCurrentSelectedTab = m_Child.GetCurSel();
	
	//  Get the current tab item text.
	TC_ITEM tcItem;
	tcItem.mask = TCIF_PARAM;
	m_Child.GetItem(nCurrentSelectedTab, &tcItem);
	
	nCurrentSelectedTab = tcItem.lParam;	

	// get the tool tip text property
	RefCountedPtr< CPropertyObject > pTabsTTTProperty = m_ArxControl->GetPropertyObject(nTabsTTT);
	// get the tool tip object
	CToolTipCtrl* ThisToolTip = m_Child.GetToolTips();

	
	// call method to display the appropriate tab pane
	SetPaneVisibility(nCurrentSelectedTab, TRUE, TRUE);
	
	// get the tooltip text for the selected tab
	if ((int)pTabsTTTProperty->size() < nCurrentSelectedTab)
		sTTT = pTabsTTTProperty->GetStringItem(nCurrentSelectedTab);
	
	// update the tooltip
	ThisToolTip->UpdateTipText(sTTT, this, 15);

	*pResult = 0;
}

void VdclTab::SetPaneVisibility(int nCurrentSelectedTab, BOOL bShow, BOOL bFireEvent) 
{
	if (nCurrentSelectedTab == -1)
		return;

	// set the focus to the new tab to be selected
	m_Child.SetCurSel(nCurrentSelectedTab);

	int nQty = m_ArxTabsList.GetCount();
	if (nQty == 0)
		return;


	
	CTabPage  *pActualTabPage;
	pActualTabPage = GetActualTabPage(nCurrentSelectedTab);
	

	if (nCurrentSelectedTab != m_nCurrentSelectedTab && m_nCurrentSelectedTab != -1)
	{
		CTabPage  *pLastTabPage = GetActualTabPage(m_nCurrentSelectedTab);
		pLastTabPage->ShowWindow(FALSE);
	}


	CDclFormObject* pSourceForm = pActualTabPage->GetControlPane().GetSourceForm();
	if (pSourceForm != NULL)
	{
		// setup to get the pane size
		CDclControlObject *pFormPropertiesArx = pSourceForm->GetControlProperties();
		
		// get the tab control position
		CRect rcItem;
		
		int nTTop = 0;
		int nTBottom = 0;
		
		for (int i=0; i < m_Child.GetItemCount(); i++)
		{
			// get the tab's position
			m_Child.GetItemRect(i, &rcItem);
			
			if (rcItem.bottom > nTBottom)
				nTBottom = rcItem.bottom;
			if (rcItem.top > nTTop)
				nTTop = rcItem.top;
		}
		
		// set the pane's position
		CRect rcNewRect(
			3,
			nTBottom + 3,
			m_rcPos.Width() - 6,
			m_rcPos.Height() - 3);

		// move the tab pane
		CRect& PanePos = pActualTabPage->GetControlPane().GetPaneWindowRect();
		PanePos.left = 0;
		PanePos.top = 0;
		PanePos.right = rcNewRect.Width();
		PanePos.bottom = rcNewRect.Height();

		// set the position member for use by other methods
		// like the SetProperty for sizes.
		pActualTabPage->GetControlPane().GetSourceForm()->m_rcPos = PanePos;
		
		// call the sizechanged method of the tab pane so that it's controls may move as well
		pActualTabPage->GetControlPane().SizeChanged(rcNewRect.Width(), rcNewRect.Height());
		
		// set the tab page's position
		pActualTabPage->SetWindowPos(
			&CWnd::wndTop, 
			rcNewRect.left,
			rcNewRect.top,
			rcNewRect.Width(),
			rcNewRect.Height(),
			SWP_FRAMECHANGED);

		if (bShow)
			pActualTabPage->GetControlPane().ShowPictureBoxes(FALSE);

		// show the pane
		pActualTabPage->ShowWindow(bShow);
		//pActualTabPage->GetControlPane().InvalidateControls();

		// set the selection index if this tab is to be shown
		if (bShow)
			m_nCurrentSelectedTab = nCurrentSelectedTab;

		if (bShow)
		{
			pActualTabPage->GetControlPane().ShowPictureBoxes(bShow);
			
			// force the tab pane to get focus
			SetFirstControlFocus(pActualTabPage);
			
			
		}
		
	}

	if (bFireEvent)
		// call method to invoke autolisp function
		InvokeMethodInt(m_ArxControl->GetStrProperty(nEventChanged), m_nCurrentSelectedTab, m_bInvokeWithSendString);
  	
}
void VdclTab::SetFirstControlFocus(CTabPage *pActualTabPage)
{
	pActualTabPage->GetControlPane().SetFirstControlFocus();
}

CDclFormObject * VdclTab::GetTabPane(int nIndex)
{
	POSITION pos = m_ArxTabsList.FindIndex(nIndex);
	if (pos != NULL)
	{
		CTabTracker *pTab = m_ArxTabsList.GetAt(pos);

		if (pTab != NULL)
		{
			return pTab->m_pDclForm;
		}
	}
	return NULL;
}

CTabPage * VdclTab::GetActualTabPage(int nIndex)
{
	POSITION pos = m_ArxTabsList.GetHeadPosition();
	while (pos != NULL)
	{
		CTabTracker *pTab = m_ArxTabsList.GetNext(pos);

		if (pTab->m_nIndex == nIndex)
		{
			return pTab->m_pTabPage;
		}
	}
	return NULL;
}

void VdclTab::ZOrderFrontAddTabControls(CDclFormObject *pDclObject)
{
	pDclObject->ZOrderFrontAddTabControls();
}

void VdclTab::ZOrderFrontAllTabs()
{
	for (int i=0; i< m_ArxTabsList.GetCount(); i++)
	{
		POSITION pos = m_ArxTabsList.FindIndex(i);
		if (pos != NULL)
		{
			CTabTracker *pTab = m_ArxTabsList.GetAt(pos);

			if (pTab != NULL)
			{
				ZOrderFrontAddTabControls(pTab->m_pDclForm);
			}
		}
	}
	
}

void VdclTab::OnDestroy() 
{	
	m_Child.SetImageList(NULL);
	if (m_ToolTip.GetToolCount() > 0)
	{
		for (int i=m_ToolTip.GetToolCount()-1; i>= 0; i--)
		{
			m_ToolTip.DelTool(&m_Child, i);
		}
	}
	CStatic::OnDestroy();
	
	
}
void VdclTab::DestroyChildren()
{
	for (int i=m_ArxTabsList.GetCount(); i>=0; i--)
	{
		POSITION pos = m_ArxTabsList.FindIndex(i);
		if (pos != NULL)
		{
			CTabTracker *pTab = m_ArxTabsList.GetAt(pos);

			if (pTab != NULL)
				pTab->m_pTabPage->GetControlPane().CleanUpControls();				
			if (pTab->m_pTabPage)
			{
				pTab->m_pTabPage->DestroyWindow();
				delete pTab->m_pTabPage;
				pTab->m_pTabPage = NULL;
			}
			m_ArxTabsList.RemoveAt(pos);
			delete pTab;
		}
	}		
}
void VdclTab::OnSize(UINT nType, int cx, int cy) 
{
	m_rcPos.right = m_rcPos.left + cx;
	m_rcPos.bottom = m_rcPos.top + cy;
	CRect rcChild(0,0,cx, cy);
	m_Child.MoveWindow(rcChild, TRUE);
	SetPaneVisibility(m_nCurrentSelectedTab, TRUE, TRUE);
	CStatic::OnSize(nType, cx, cy);
	
	
}

void VdclTab::OnSelchanging(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// set the selection index
	int nCurrentSelectedTab = m_Child.GetCurSel();


	// call method to invoke autolisp function
	InvokeMethodInt(m_ArxControl->GetStrProperty(nEventSelChanging), nCurrentSelectedTab, m_bInvokeWithSendString);
	
	*pResult = 0;
}

void VdclTab::InitToolTip()
{
	if (m_ToolTip.m_hWnd == NULL)
	{
		// Create ToolTip control
		m_ToolTip.Create(this);
		// Create inactive
		m_ToolTip.Activate(FALSE);
	}
} // End of InitToolTip



BOOL VdclTab::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == 512)
	{
		if (!m_ToolTipsUpdated)
		{
			RefCountedPtr< CPropertyObject > pTabsTTTProperty = m_ArxControl->GetPropertyObject(nTabsTTT);
			size_t nTabQty = pTabsTTTProperty->size();
			for (size_t i = 0; i < nTabQty; i++)
			{
				CString sText = pTabsTTTProperty->GetStringArrayPtr()->at(i);		
				m_ToolTip.UpdateTipText((LPCTSTR)sText, &m_Child, i);
			}
			m_ToolTipsUpdated = true;
		}
	}

	m_ToolTip.RelayEvent(pMsg);	

	return CStatic::PreTranslateMessage(pMsg);
}


void VdclTab::SetTooltipText(CString* spText, BOOL bActivate)
{
	// We cannot accept NULL pointer
	if (spText == NULL) return;

	// Initialize ToolTip
	InitToolTip();

	// If there is no tooltip defined then add it
	if (m_ToolTip.GetToolCount() == 0)
	{
		CRect rectBtn; 
		GetClientRect(rectBtn);
		m_ToolTip.AddTool(this, (LPCTSTR)*spText, rectBtn, 1);
	}

	// Set text for tooltip
	m_ToolTip.UpdateTipText((LPCTSTR)*spText, this, 1);
	m_ToolTip.Activate(bActivate);
} // End of SetTooltipText
void VdclTab::OnKillFocus(CWnd* pNewWnd) 
{
	CStatic::OnKillFocus(pNewWnd);
	
	// call methods to invoke the event
	InvokeMethod(m_ArxControl->GetStrProperty(nEventKillFocus), m_bInvokeWithSendString);

	
}

void VdclTab::OnSetFocus(CWnd* pOldWnd) 
{
	CStatic::OnSetFocus(pOldWnd);
	
	// call methods to invoke the event
	InvokeMethod(m_ArxControl->GetStrProperty(nEventSetFocus), m_bInvokeWithSendString);
	
}
