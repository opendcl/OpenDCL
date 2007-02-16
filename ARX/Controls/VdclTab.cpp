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
	//switch( pProp->GetType() )
	//{
	//}
	return true;
}

bool CTabControlX::OnApplyEnabled( RefCountedPtr< CPropertyObject > pProp )
{
	mpControl->EnableWindow( pProp->GetBooleanValue() );
	return true;
}

bool CTabControlX::OnApplyCaptionFont( RefCountedPtr< CPropertyObject > pProp )
{
	GetControl()->GetTabCtrl().SetFont( theWorkspace.GetFontCollection().GetFont( mpTemplate, mpControl ) );
	return true;
}

bool CTabControlX::OnApplyImageList( RefCountedPtr< CPropertyObject > pProp )
{
	GetControl()->GetTabCtrl().SetImageList( mpTemplate->GetOwnerForm()->GetImageList( pProp->GetShortValue() ) );
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// VdclTab

VdclTab::VdclTab( CControlPane& Pane, CDclControlObject* pTemplate, UINT nID )
: mControlX( pTemplate, &Pane, this )
, mpSourceControl( pTemplate )
, mpControlPane( &Pane )
, mTabCtrl()
{
	m_nCurrentSelectedTab = -1;
	m_rcPos.left = 0;
	m_rcPos.top = 0;
	m_rcPos.right = 200;
	m_rcPos.bottom = 200;
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
		CStatic::Create( NULL,
										 (WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN),
										 rectPane,
										 pParentWnd,
										 nID );
	VERIFY(CWnd::SubclassDlgItem(nID, pParentWnd));

	CRect rectTabCtrl( 0, 0, rectPane.Width(), rectPane.Height() );
	if( !mTabCtrl.Create( mControlX.GetWndStyle(), rectTabCtrl, this, IDC_TAB ) )
		bSuccess = false;
	VERIFY(mTabCtrl.SubclassDlgItem(IDC_TAB, this));

	if( bSuccess && !mControlX.ApplyPropertiesEnum() )
		bSuccess = false;

	if( mpSourceControl->GetLngProperty(nEventInvoke) == 1 )
		m_bInvokeWithSendString = true;
	else
		m_bInvokeWithSendString = false;

	CreateTabPages( nID, rectPane );
	mTabCtrl.SetCurSel( 0 );// set the first tab as the selected tab
	ActivateTabPage( 0, TRUE );
	SetupTabs();

	return bSuccess;
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
	CString sText;
	TC_ITEM TabCtrlItem;
	CString sTTT;
	TabCtrlItem.mask = TCIF_TEXT;
	
	// delete all previos tabs
	mTabCtrl.DeleteAllItems();

	// get the tab's lists
	RefCountedPtr< CPropertyObject > pTabsCaptionProperty = mpSourceControl->GetPropertyObject(nTabsCaption);
	RefCountedPtr< CPropertyObject > pTabsTTTProperty = mpSourceControl->GetPropertyObject(nTabsTTT);

	size_t nTabQty = pTabsCaptionProperty->size();
	int nImageListIndex = mpSourceControl->GetImageListIndex();

  size_t i;
	for (i = 0; i < nTabQty; i++)
	{
		CString Tab = mpSourceControl->GetPropertyListItem(nTabsCaption, i);
					
		// get the tag caption
		TabCtrlItem.pszText = Tab.GetBuffer(256);

		// set the image list item number is required
		if (nImageListIndex > -1)
			TabCtrlItem.iImage = _ttol(mpSourceControl->GetPropertyListItem(nTabsImageList, i));
					
		// add the new tab
		mTabCtrl.InsertItem(i, &TabCtrlItem );

		//  Get the current tab item text.
		TC_ITEM tcItem;
		tcItem.mask = TCIF_PARAM;
		mTabCtrl.GetItem(i, &tcItem);
		
		tcItem.lParam = (short)i;
		
		mTabCtrl.SetItemExtra(sizeof(short));
		//  Set the item in the tab control.
		mTabCtrl.SetItem(i, &tcItem);

	}
	
	InitToolTip();
	mTabCtrl.SetToolTips(&m_ToolTip);
	
	for (i = 0; i < nTabQty; i++)
	{
		CRect r;
		sText = mpSourceControl->GetPropertyListItem(nTabsTTT, i);
		mTabCtrl.GetItemRect(i, &r);
		m_ToolTip.AddTool(&mTabCtrl, sText, &r, i);
	}

	
   // Activate the tooltip control.
   m_ToolTip.Activate(TRUE);
	

}


void VdclTab::HideTab(int nIndex)
{
	// ensure the tab is not already removed..
	for (int i=0; i<mTabCtrl.GetItemCount(); i++)
	{
		//  Get the current tab item text.
		TC_ITEM tcItem;
		tcItem.mask = TCIF_PARAM;
		mTabCtrl.GetItem(i, &tcItem);
		
		if (nIndex == tcItem.lParam)
		{
			mTabCtrl.DeleteItem(nIndex);
			return;
		}
	}	
}

void VdclTab::ShowTab(int nIndex)
{
	// ensure the tab is not already showing.
	for (int i=0; i<mTabCtrl.GetItemCount(); i++)
	{
		//  Get the current tab item text.
		TC_ITEM tcItem;
		tcItem.mask = TCIF_PARAM;
		mTabCtrl.GetItem(i, &tcItem);
		
		if (nIndex == tcItem.lParam)
			return;
	}

	CString sTab = mpSourceControl->GetPropertyListItem(nTabsCaption, nIndex);
				
	// set the image list item number is required
	int nImage = _ttol(mpSourceControl->GetPropertyListItem(nTabsImageList, nIndex));
				
	// add the new tab
	mTabCtrl.InsertItem(nIndex, sTab, nImage);

	//  Get the current tab item text.
	TC_ITEM tcItem;
	tcItem.mask = TCIF_PARAM;
	mTabCtrl.GetItem(nIndex, &tcItem);
	
	tcItem.lParam = (short)nIndex;
	
	mTabCtrl.SetItemExtra(sizeof(short));
	//  Set the item in the tab control.
	mTabCtrl.SetItem(nIndex, &tcItem);

}

//bool VdclTab::AddTab( const CDclFormObject* pDclTab, int nTabIndex /*= -1*/ )
//{
//	// set counter for ArxControls
//	int nCount = pDclTab->GetControlList().GetCount();
//	
//	
//	CString sText;
//	TC_ITEM TabCtrlItem;
//	CString sTTT;
//	TabCtrlItem.mask = TCIF_TEXT;
//	
//	// delete all previos tabs
//	//pControl->mTabCtrl.DeleteAllItems();
//
//	// get the tab's lists
//	RefCountedPtr< CPropertyObject > pTabsCaptionProperty = mpSourceControl->GetPropertyObject(nTabsCaption);
//	RefCountedPtr< CPropertyObject > pTabsTTTProperty = mpSourceControl->GetPropertyObject(nTabsTTT);
//	RefCountedPtr< CPropertyObject > pTabsImageProperty = mpSourceControl->GetPropertyObject(nTabsImageList);
//
//	size_t nTabQty = pTabsCaptionProperty->size();
//	//for (int i = 0; i < nTabQty; i++)
//	//{
//	//	// get the tag caption
//	//	TabCtrlItem.pszText = pTabsCaptionProperty->GetStringItem(i).GetBuffer(256);
//	//	// add the new tab
//	//	//pControl->mTabCtrl.InsertItem( i, &TabCtrlItem );
//	//}
//	
//	
//	InitToolTip();
//
//	// set start position for navigating ArxControls
//	POSITION pos = pDclTab->GetControlList().GetHeadPosition();
//	
//	// do loop to navigate Arx Controls
//	while (pos != NULL)
//	{
//		// get current ArxControlObject
//		CDclControlObject* pTemplate = pDclTab->GetControlList().GetNext(pos);
//
//		if (pTemplate->GetType() == CtlTabStrip)
//		{
//			// get the tag caption
//			TabCtrlItem.pszText = pTabsCaptionProperty->GetStringItem(nTabIndex).GetBuffer(256);
//			
//			//int i = pControl->mTabCtrl.GetItemCount();
//			//// set the image list item number is required
//			//if (pTabsImageProperty)
//			//	TabCtrlItem.iImage = (int)atof(pTabsImageProperty->GetStringItem(nTabIndex));
//
//			//// add the new tab
//			//i = pControl->mTabCtrl.InsertItem(i, &TabCtrlItem );
//			//
//			//CRect r;
//			//POSITION pos = pTabsTTTProperty->m_stringList.FindIndex(nTabIndex);		
//			//
//			//if (pos != NULL)
//			//{
//			//	sText = pTabsTTTProperty->m_stringList.GetAt(pos);
//			//	pControl->mTabCtrl.GetItemRect(i, &r);
//			//	pControl->m_ToolTip.AddTool(&pControl->mTabCtrl, sText, &r, i);
//			//}
//		}
//
//		// increment counter
//		nCount--;
//	}
//	SetupTabs();
//	//for (i = 0; i < nTabQty; i++)
//	//{
//	//	CRect r;
//	//	POSITION pos = pTabsTTTProperty->m_stringList.FindIndex(i);		
//	//	if (pos != NULL)
//	//		sText = pTabsTTTProperty->m_stringList.GetAt(pos);
//
//	//	mTabCtrl.GetItemRect(i, &r);
//	//	VERIFY(m_ToolTip.AddTool(&mTabCtrl, sText, &r, i));
//	//}
//
//	// Activate the tooltip control.
//	m_ToolTip.Activate(TRUE);
//	
//}

bool VdclTab::CreateTabPages( UINT& nId, CRect rcTab )
{
	bool bFailed = false;
	
	POSITION pos = mpSourceControl->GetOwnerProject()->GetDclFormList().GetHeadPosition();
	while (pos)
	{
		CDclFormObject* pDclForm = mpSourceControl->GetOwnerProject()->GetDclFormList().GetNext(pos);
		pDclForm->EnsureIsLoaded();
		if( pDclForm->GetParentForm() == mpSourceControl->GetOwnerForm() )
		{
			// Create the tab pane itself that the controls actually sit on.
			TTabPagePtr pNewTabPage = new CTabPage( pDclForm, &mTabCtrl );
			mTabPages.push_back( pNewTabPage );
			pNewTabPage->Create( IDD_TABPAGE2, &mTabCtrl);
			pNewTabPage->MoveWindow(rcTab, TRUE);

			pNewTabPage->GetControlPane().GetPaneWindowRect().right = rcTab.right;
			pNewTabPage->GetControlPane().GetPaneWindowRect().bottom = rcTab.bottom;
			
			// call method to create the controls
			if (!pNewTabPage->GetControlPane().CreateControls( pDclForm, ++nId ) )
				bFailed = true;
			
			pNewTabPage->ShowWindow( FALSE );
		}
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

void VdclTab::OnSelchange(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CString sTTT;
	
	int nCurrentSelectedTab = mTabCtrl.GetCurSel();
	
	//  Get the current tab item text.
	TC_ITEM tcItem;
	tcItem.mask = TCIF_PARAM;
	mTabCtrl.GetItem(nCurrentSelectedTab, &tcItem);
	
	nCurrentSelectedTab = tcItem.lParam;	

	// get the tool tip text property
	RefCountedPtr< CPropertyObject > pTabsTTTProperty = mpSourceControl->GetPropertyObject(nTabsTTT);
	// get the tool tip object
	CToolTipCtrl* ThisToolTip = mTabCtrl.GetToolTips();

	
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

	// set the focus to the new tab to be selected
	mTabCtrl.SetCurSel( nTabPageToActivate );
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
		
		// get the tab control position
		CRect rcItem;
		
		int nTTop = 0;
		int nTBottom = 0;
		
		for (int i=0; i < mTabCtrl.GetItemCount(); i++)
		{
			// get the tab's position
			mTabCtrl.GetItemRect(i, &rcItem);
			
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
			m_nCurrentSelectedTab = nTabPageToActivate;

		if (bShow)
		{
			pActualTabPage->GetControlPane().ShowPictureBoxes(bShow);
			
			// force the tab pane to get focus
			SetFirstControlFocus(pActualTabPage);
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
	mTabCtrl.SetImageList(NULL);
	if (m_ToolTip.GetToolCount() > 0)
	{
		for (int i=m_ToolTip.GetToolCount()-1; i>= 0; i--)
		{
			m_ToolTip.DelTool(&mTabCtrl, i);
		}
	}
	CStatic::OnDestroy();
	
	
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
	m_rcPos.right = m_rcPos.left + cx;
	m_rcPos.bottom = m_rcPos.top + cy;
	CRect rcChild(0,0,cx, cy);
	mTabCtrl.MoveWindow(rcChild, TRUE);
	ActivateTabPage(m_nCurrentSelectedTab, TRUE, TRUE);
	CStatic::OnSize(nType, cx, cy);
	
	
}

void VdclTab::OnSelchanging(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// set the selection index
	int nCurrentSelectedTab = mTabCtrl.GetCurSel();


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
	mTabCtrl.SetToolTips(&m_ToolTip);
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
				m_ToolTip.UpdateTipText((LPCTSTR)sText, &mTabCtrl, i);
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
	InvokeMethod(mpSourceControl->GetStrProperty(nEventKillFocus), m_bInvokeWithSendString);

	
}

void VdclTab::OnSetFocus(CWnd* pOldWnd) 
{
	CStatic::OnSetFocus(pOldWnd);
	
	// call methods to invoke the event
	InvokeMethod(mpSourceControl->GetStrProperty(nEventSetFocus), m_bInvokeWithSendString);
	
}

void VdclTab::PostNcDestroy() 
{
	CStatic::PostNcDestroy();
	delete this;
}
