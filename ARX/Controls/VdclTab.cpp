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
#include "Project.h"

const int IDC_TAB = 94;


TDialogControlPtr CTabControlX::FindControl( HWND hwndControl ) const
{
	return GetControl()->FindControl( hwndControl );
}

TDialogControlPtr CTabControlX::FindControl( LPCTSTR pszControlName, ControlType type /*= CtlInvalid*/ ) const
{
	return GetControl()->FindControl( pszControlName, type );
}

DWORD CTabControlX::GetWndStyle() const
{
	DWORD dwStyle = CArxDialogControl::GetWndStyle();
	dwStyle |= (WS_CLIPCHILDREN | TCS_FOCUSNEVER | TCS_TOOLTIPS);

	if( mpTemplate->GetLngProperty( nTabStyle ) == 0 )
		dwStyle |= TCS_TABS;
	else
		dwStyle |= TCS_BUTTONS;

	if( mpTemplate->GetBoolProperty( nMultiRow ) )
		dwStyle |= TCS_MULTILINE;
	else
		dwStyle |= TCS_SINGLELINE;
	
	if( mpTemplate->GetBoolProperty( nTabFixedWidth ) )
		dwStyle |= TCS_FIXEDWIDTH;

	return dwStyle;
}

bool CTabControlX::OnApplyProperty( RefCountedPtr< CPropertyObject > pProp )
{
	switch( pProp->GetType() )
	{
	case nMinTabWidth:
		GetControl()->GetTabCtrl().SetMinTabWidth(pProp->GetLongValue());
		break;
	case nTabSelected:
		GetControl()->GetTabCtrl().SetCurSel(pProp->GetLongValue());
		break;
	case nTabStyle:
		if( pProp->GetLongValue() == 0 )
			GetControl()->GetTabCtrl().ModifyStyle( TCS_BUTTONS, TCS_TABS, SWP_FRAMECHANGED );
		else
			GetControl()->GetTabCtrl().ModifyStyle( TCS_TABS, TCS_BUTTONS, SWP_FRAMECHANGED );
		break;
	case nTabLabelAlign:
		if( pProp->GetLongValue() == 0 )
			GetControl()->GetTabCtrl().ModifyStyle( 0, TCS_FORCELABELLEFT, SWP_FRAMECHANGED );
		else
			GetControl()->GetTabCtrl().ModifyStyle( TCS_FORCELABELLEFT, 0, SWP_FRAMECHANGED );
		break;
	case nTabFixedWidth:
		if( pProp->GetBooleanValue() )
		{
			GetControl()->GetTabCtrl().ModifyStyle( 0, TCS_FIXEDWIDTH, SWP_FRAMECHANGED );
			CRect rectTab;
			GetControl()->GetTabCtrl().GetItemRect( 0, &rectTab );
			CSize sizeTabs;
			sizeTabs.cx = mpTemplate->GetLngProperty( nMinTabWidth );
			sizeTabs.cy = rectTab.Height();
			GetControl()->GetTabCtrl().SetItemSize( sizeTabs );
		}
		else
			GetControl()->GetTabCtrl().ModifyStyle( TCS_FIXEDWIDTH, 0, SWP_FRAMECHANGED );
		break;
	}
	return true;
}

bool CTabControlX::OnApplyEnabled( RefCountedPtr< CPropertyObject > pProp )
{
	GetControl()->GetTabCtrl().EnableWindow( pProp->GetBooleanValue() );
	return true;
}

bool CTabControlX::OnApplyCaptionFont( RefCountedPtr< CPropertyObject > pProp )
{
	GetControl()->GetTabCtrl().SetFont( theWorkspace.GetFontCollection().GetFont( mpTemplate, mpControl ) );
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// VdclTab

VdclTab::VdclTab( CControlPane& Pane, CDclControlObject* pTemplate, UINT nID )
: mControlX( pTemplate, &Pane, this )
, mpSourceControl( pTemplate )
, mpControlPane( &Pane )
//, mrectTabCtrl( Pane.GetPaneWindowRect() )
{
	m_nCurrentSelectedTab = -1;
	// No tooltip created
	m_ToolTip.m_hWnd = NULL;
	m_ToolTipsUpdated = false;
	Create( Pane.GetHostDialog(), nID );
}

VdclTab::~VdclTab()
{
}

bool VdclTab::Create( CWnd* pParentWnd, UINT nID )
{
	CRect rectPane = mControlX.GetWndRect();

	bool bSuccess =
		CTabCtrl::Create( (WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN),
											rectPane,
											pParentWnd,
											nID );
	VERIFY(CWnd::SubclassDlgItem(nID, pParentWnd));

	RefCountedPtr< CImageListObject > pImageList = mpSourceControl->GetImageList();
	if( pImageList )
		SetImageList( &pImageList->m_ImageList );

	if( bSuccess && !mControlX.ApplyPropertiesEnum() )
		bSuccess = false;

	if( mpSourceControl->GetLngProperty(nEventInvoke) == 1 )
		m_bInvokeWithSendString = true;
	else
		m_bInvokeWithSendString = false;
	SetupTabs();
	SetCurSel(0);
	CreateTabPages( nID );
	ActivateTabPage( 0, TRUE );

	return bSuccess;
}

CRect VdclTab::GetUsedArea() const
{
	CRect rectTab;
	GetClientRect( &rectTab );
	int nMaxTabHeight = 0;
	for( int idx = GetItemCount() - 1; idx >= 0; --idx )
	{
		CRect rectItem;
		GetItemRect( idx, &rectItem );
		if( rectItem.bottom > nMaxTabHeight )
			nMaxTabHeight = rectItem.bottom;
	}
	rectTab.top = nMaxTabHeight;
	return rectTab;
}

void VdclTab::SetFirstControlFocus(CTabPage *pActualTabPage)
{
	pActualTabPage->GetControlPane().SetFirstControlFocus();
}

TTabPagePtr VdclTab::GetTabPageAt( size_t nIndex ) const
{
	if( nIndex >= mTabPages.size() )
		return NULL;
	return mTabPages.at( nIndex );
}

const CDclFormObject* VdclTab::GetTabSourceFormAt( size_t nIndex ) const
{
	if( nIndex >= mTabPages.size() )
		return NULL;
	return mTabPages.at( nIndex )->GetSourceForm();
}

const CControlPane* VdclTab::GetTabControlPaneAt( size_t nIndex ) const
{
	if( nIndex >= mTabPages.size() )
		return NULL;
	return &mTabPages.at( nIndex )->GetControlPane();
}

void VdclTab::SetupTabs()
{
	InitToolTip();
	
	// delete all previous tabs
	DeleteAllItems();

	// get the tab's lists
	RefCountedPtr< CPropertyObject > pTabsCaptionProperty = mpSourceControl->GetPropertyObject(nTabsCaption);
	RefCountedPtr< CPropertyObject > pTabsTTTProperty = mpSourceControl->GetPropertyObject(nTabsTTT);

	size_t nTabQty = pTabsCaptionProperty->size();
	for (size_t i = 0; i < nTabQty; i++)
	{
		TC_ITEM TabCtrlItem;
		TabCtrlItem.mask = TCIF_TEXT | TCIF_PARAM;
		TabCtrlItem.lParam = (LPARAM)i;

		// get the tag caption
		CString sTabCaption = mpSourceControl->GetPropertyListItem(nTabsCaption, i);
		TabCtrlItem.pszText = sTabCaption.LockBuffer();

		// set the image list item number is required
		RefCountedPtr< CPropertyObject > pImageListProp = mpSourceControl->GetPropertyObject(nTabsImageList);
		if (pImageListProp && i < pImageListProp->GetIntArrayPtr()->size())
		{
			TabCtrlItem.iImage = pImageListProp->GetIntArrayPtr()->at(i);
			TabCtrlItem.mask |= TCIF_IMAGE;
		}
					
		// add the new tab
		InsertItem(i, &TabCtrlItem );

		CString sToolTipText = mpSourceControl->GetPropertyListItem(nTabsTTT, i);
		CRect rectTab;
		GetItemRect(i, &rectTab);
		m_ToolTip.AddTool(this, sToolTipText, &rectTab, i);
	}

	// Activate the tooltip control.
   m_ToolTip.Activate(TRUE);
}


void VdclTab::HideTab(int nIndex)
{
	// ensure the tab is not already removed..
	for (int i = 0; i < GetItemCount(); i++)
	{
		//  Get the current tab item text.
		TC_ITEM tcItem;
		tcItem.mask = TCIF_PARAM;
		GetItem(i, &tcItem);
		
		if (nIndex == tcItem.lParam)
		{
			DeleteItem(nIndex);
			return;
		}
	}	
}

void VdclTab::ShowTab(int nIndex)
{
	// ensure the tab is not already showing.
	for (int i = 0; i < GetItemCount(); i++)
	{
		//  Get the current tab item text.
		TC_ITEM tcItem;
		tcItem.mask = TCIF_PARAM;
		GetItem(i, &tcItem);
		
		if (nIndex == tcItem.lParam)
			return;
	}

	CString sTabCaption = mpSourceControl->GetPropertyListItem(nTabsCaption, nIndex);
				
	// set the image list item number is required
	int nImage = -1;
	RefCountedPtr< CPropertyObject > pImageListProp = mpSourceControl->GetPropertyObject(nTabsImageList);
	if (pImageListProp && nIndex < pImageListProp->GetIntArrayPtr()->size())
		nImage = pImageListProp->GetIntArrayPtr()->at(nIndex);
				
	// add the new tab
	InsertItem(nIndex, sTabCaption, nImage);

	//  Set the item in the tab control.
	TC_ITEM tcItem;
	tcItem.mask = TCIF_PARAM;
	tcItem.lParam = (LPARAM)nIndex;
	SetItem(nIndex, &tcItem);

}

bool VdclTab::CreateTabPages( UINT& nId )
{
	bool bFailed = false;

	CRect rectPage = GetUsedArea();
	POSITION pos = mpSourceControl->GetOwnerProject()->GetDclFormList().GetHeadPosition();
	while (pos)
	{
		CDclFormObject* pDclForm = mpSourceControl->GetOwnerProject()->GetDclFormList().GetNext(pos);
		pDclForm->EnsureIsLoaded();
		if( pDclForm->GetParentForm() == mpSourceControl->GetOwnerForm() )
			mTabPages.push_back( new CTabPage( pDclForm, this, rectPage, nId ) );
	}
	return !bFailed;
}

TDialogControlPtr VdclTab::FindControl( HWND hwndControl ) const
{
	size_t idx = mTabPages.size();
	while( idx-- > 0 )
	{
		TDialogControlPtr pControl = mTabPages.at( idx )->GetControlPane().FindControl( hwndControl );
		if( pControl )
			return pControl;
	}
	return NULL;
}

TDialogControlPtr VdclTab::FindControl( LPCTSTR pszControlName, ControlType type /*= CtlInvalid*/ ) const
{
	size_t idx = mTabPages.size();
	while( idx-- > 0 )
	{
		TDialogControlPtr pControl = mTabPages.at( idx )->GetControlPane().FindControl( pszControlName, type );
		if( pControl )
			return pControl;
	}
	return NULL;
}


BEGIN_MESSAGE_MAP(VdclTab, CTabCtrl)
	//{{AFX_MSG_MAP(VdclTab)
	ON_NOTIFY_REFLECT(TCN_SELCHANGE, OnSelchange)
	ON_NOTIFY_REFLECT(TCN_SELCHANGING, OnSelchanging)	
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
	ON_WM_WINDOWPOSCHANGED()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// VdclTab message handlers

void VdclTab::OnSelchange(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CString sTTT;
	
	int nCurrentSelectedTab = GetCurSel();
	
	//  Get the current tab item text.
	TC_ITEM tcItem;
	tcItem.mask = TCIF_PARAM;
	GetItem(nCurrentSelectedTab, &tcItem);
	
	nCurrentSelectedTab = tcItem.lParam;	

	// get the tool tip text property
	RefCountedPtr< CPropertyObject > pTabsTTTProperty = mpSourceControl->GetPropertyObject(nTabsTTT);
	// get the tool tip object
	CToolTipCtrl* ThisToolTip = GetToolTips();

	
	// call method to display the appropriate tab pane
	ActivateTabPage(nCurrentSelectedTab, TRUE, TRUE);
	
	// get the tooltip text for the selected tab
	if ((int)pTabsTTTProperty->size() < nCurrentSelectedTab)
		sTTT = pTabsTTTProperty->GetStringItem(nCurrentSelectedTab);
	
	// update the tooltip
	ThisToolTip->UpdateTipText(sTTT, this, 15);

	*pResult = 0;
}

void VdclTab::ActivateTabPage( int nTabPageToActivate, bool bShow, bool bFireEvent /*= false*/ ) 
{
	if (nTabPageToActivate == -1)
		return;

	TTabPagePtr pActualTabPage = GetTabPageAt( nTabPageToActivate );
	if( !pActualTabPage )
		return;

	if (nTabPageToActivate != m_nCurrentSelectedTab && m_nCurrentSelectedTab != -1)
		GetTabPageAt( m_nCurrentSelectedTab )->ShowWindow(FALSE);

	CDclFormObject* pSourceForm = pActualTabPage->GetControlPane().GetSourceForm();
	if (pSourceForm != NULL)
	{
		// setup to get the pane size
		CDclControlObject *pFormPropertiesArx = pSourceForm->GetControlProperties();

		CRect rectTab = GetUsedArea();
		
		// set the tab page's position
		pActualTabPage->SetWindowPos(
			&CWnd::wndTop, 
			rectTab.left,
			rectTab.top,
			rectTab.Width(),
			rectTab.Height(),
			SWP_FRAMECHANGED);
		
		// call the sizechanged method of the tab pane so that it's controls may move as well
		pActualTabPage->GetControlPane().SetPanePos(CRect(0, 0, rectTab.Width(), rectTab.Height()));

		if (bShow)
			pActualTabPage->GetControlPane().ShowPictureBoxes(FALSE);

		// show the pane
		pActualTabPage->ShowWindow(bShow);

		// set the selection index if this tab is to be shown
		if (bShow)
		{
			m_nCurrentSelectedTab = nTabPageToActivate;

			pActualTabPage->GetControlPane().ShowPictureBoxes(bShow);
			
			// force the tab pane to get focus
			SetFirstControlFocus(pActualTabPage);
			RedrawWindow();
		}
	}

	if (bFireEvent)
		// call method to invoke autolisp function
		InvokeMethodInt(mpSourceControl->GetStrProperty(nEventChanged), m_nCurrentSelectedTab, m_bInvokeWithSendString);
}

void VdclTab::ZOrderFrontAllTabs()
{
	for( size_t idx = 0; idx < mTabPages.size(); ++idx )
		mTabPages.at( idx )->GetSourceForm()->ZOrderFrontAddTabControls();
}

void VdclTab::OnDestroy() 
{	
	SetImageList(NULL);
	if (m_ToolTip.GetToolCount() > 0)
	{
		for (int i=m_ToolTip.GetToolCount()-1; i>= 0; i--)
		{
			m_ToolTip.DelTool(this, i);
		}
	}
	CTabCtrl::OnDestroy();
	
	
}
void VdclTab::DestroyTabPages()
{
	size_t idx = mTabPages.size();
	while( idx-- > 0 )
	{
		TTabPagePtr pTabPage = mTabPages.at( idx );
		pTabPage->GetControlPane().CleanUpControls();
		pTabPage->DestroyWindow();
	}
	mTabPages.clear();
}

void VdclTab::OnSize(UINT nType, int cx, int cy) 
{
	//mrectTabCtrl.right = mrectTabCtrl.left + cx;
	//mrectTabCtrl.bottom = mrectTabCtrl.top + cy;
	//CRect rcChild(0,0,cx, cy);
	//MoveWindow(rcChild, TRUE);
	CTabCtrl::OnSize(nType, cx, cy);
	ActivateTabPage(m_nCurrentSelectedTab, TRUE, TRUE);
}

void VdclTab::OnSelchanging(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// set the selection index
	int nCurrentSelectedTab = GetCurSel();

	// call method to invoke autolisp function
	InvokeMethodInt(mpSourceControl->GetStrProperty(nEventSelChanging), nCurrentSelectedTab, m_bInvokeWithSendString);
	
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
	SetToolTips(&m_ToolTip);
} // End of InitToolTip



BOOL VdclTab::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == 512)
	{
		if (!m_ToolTipsUpdated)
		{
			RefCountedPtr< CPropertyObject > pTabsTTTProperty = mpSourceControl->GetPropertyObject(nTabsTTT);
			size_t nTabQty = pTabsTTTProperty->size();
			for (size_t i = 0; i < nTabQty; i++)
			{
				CString sText = pTabsTTTProperty->GetStringArrayPtr()->at(i);		
				m_ToolTip.UpdateTipText((LPCTSTR)sText, this, i);
			}
			m_ToolTipsUpdated = true;
		}
	}

	m_ToolTip.RelayEvent(pMsg);	

	return CTabCtrl::PreTranslateMessage(pMsg);
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
	CTabCtrl::OnKillFocus(pNewWnd);
	
	// call methods to invoke the event
	InvokeMethod(mpSourceControl->GetStrProperty(nEventKillFocus), m_bInvokeWithSendString);
	
}

void VdclTab::OnSetFocus(CWnd* pOldWnd) 
{
	CTabCtrl::OnSetFocus(pOldWnd);
	
	// call methods to invoke the event
	InvokeMethod(mpSourceControl->GetStrProperty(nEventSetFocus), m_bInvokeWithSendString);
}

void VdclTab::PostNcDestroy() 
{
	CTabCtrl::PostNcDestroy();
	delete this;
}

void VdclTab::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	CTabCtrl::OnWindowPosChanged(lpwndpos);

	// TODO: Add your message handler code here
}
