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


/////////////////////////////////////////////////////////////////////////////
// VdclTab

VdclTab::VdclTab( CControlPane& Pane, CDclControlObject* pTemplate, UINT nID )
: CArxDialogControl( pTemplate, &Pane, this )
{
	m_nCurrentSelectedTab = -1;
	Create( Pane.GetHostDialog(), nID );
}

VdclTab::~VdclTab()
{
}

bool VdclTab::Create( CWnd* pParentWnd, UINT nID )
{
	CRect rectPane = GetWndRect();

	bool bSuccess = (CTabCtrl::Create( GetWndStyle(), rectPane, pParentWnd, nID ) != FALSE);

	RefCountedPtr< CImageListObject > pImageList = mpTemplate->GetImageList();
	if( pImageList )
		SetImageList( &pImageList->m_ImageList );

	SetupTabs();
	if( bSuccess && !ApplyPropertiesEnum() )
		bSuccess = false;

	ModifyStyleEx( 0, WS_EX_CONTROLPARENT ); //this prevents the TAB key from locking up the dialog!

	if( mpTemplate->GetLngProperty(nEventInvoke) == 1 )
		m_bInvokeWithSendString = true;
	else
		m_bInvokeWithSendString = false;

	CreateTabPages( nID );
	ActivateTabPage( GetCurSel(), true, true );

	return bSuccess;
}

DWORD VdclTab::GetWndStyle() const
{
	DWORD dwStyle = CArxDialogControl::GetWndStyle();
	dwStyle |= (/*WS_CLIPCHILDREN | */TCS_FOCUSNEVER | TCS_TOOLTIPS);

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

bool VdclTab::OnApplyProperty( RefCountedPtr< CPropertyObject > pProp )
{
	if( !__super::OnApplyProperty( pProp ) )
		return false;
	switch( pProp->GetType() )
	{
	case nMinTabWidth:
		SetMinTabWidth(pProp->GetLongValue());
		break;
	case nTabSelected:
		ActivateTabPage(pProp->GetLongValue(), true, true);
		break;
	case nTabStyle:
		if( pProp->GetLongValue() == 0 )
			ModifyStyle( TCS_BUTTONS, TCS_TABS, SWP_FRAMECHANGED );
		else
			ModifyStyle( TCS_TABS, TCS_BUTTONS, SWP_FRAMECHANGED );
		ResetTooltips();
		break;
	case nTabLabelAlign:
		if( pProp->GetLongValue() == 0 )
			ModifyStyle( 0, TCS_FORCELABELLEFT, SWP_FRAMECHANGED );
		else
			ModifyStyle( TCS_FORCELABELLEFT, 0, SWP_FRAMECHANGED );
		break;
	case nTabFixedWidth:
		if( pProp->GetBooleanValue() )
		{
			ModifyStyle( 0, TCS_FIXEDWIDTH, SWP_FRAMECHANGED );
			CRect rectTab;
			GetItemRect( 0, &rectTab );
			CSize sizeTabs;
			sizeTabs.cx = mpTemplate->GetLngProperty( nMinTabWidth );
			sizeTabs.cy = rectTab.Height();
			SetItemSize( sizeTabs );
		}
		else
			ModifyStyle( TCS_FIXEDWIDTH, 0, SWP_FRAMECHANGED );
		ResetTooltips();
	case nTabsTTT:
		ResetTooltips();
		break;
	}
	return true;
}

bool VdclTab::OnApplyToolTip( RefCountedPtr< CPropertyObject > pProp )
{
	ResetTooltips();
	return true;
}

bool VdclTab::GetChildPanes( std::list< const CControlPane* >& listChildren ) const
{
	size_t idx = mTabPages.size();
	while( idx-- > 0 )
		listChildren.push_back( &mTabPages.at( idx )->GetControlPane() );
	return true;
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
	rectTab.DeflateRect( 1, 1, 3, 3 ); //kludge to leave room for the tab control border
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
	// delete all previous tabs
	DeleteAllItems();

	// get the tab's lists
	RefCountedPtr< CPropertyObject > pTabsCaptionProperty = mpTemplate->GetPropertyObject(nTabsCaption);
	RefCountedPtr< CPropertyObject > pTabsTTTProperty = mpTemplate->GetPropertyObject(nTabsTTT);
	RefCountedPtr< CPropertyObject > pToolTipBalloon = mpTemplate->GetPropertyObject( nToolTipBalloon );
	GetToolTipCtrl().SetDefaultSizes( !pToolTipBalloon || pToolTipBalloon->GetBooleanValue() );

	size_t nTabQty = pTabsCaptionProperty->size();
	for (size_t i = 0; i < nTabQty; i++)
	{
		TC_ITEM TabCtrlItem;
		TabCtrlItem.mask = TCIF_TEXT | TCIF_PARAM;
		TabCtrlItem.lParam = (LPARAM)i;

		// get the tag caption
		CString sTabCaption = mpTemplate->GetPropertyListItem(nTabsCaption, i);
		TabCtrlItem.pszText = sTabCaption.LockBuffer();

		// set the image list item number is required
		RefCountedPtr< CPropertyObject > pImageListProp = mpTemplate->GetPropertyObject(nTabsImageList);
		if (pImageListProp && i < pImageListProp->GetIntArrayPtr()->size())
		{
			TabCtrlItem.iImage = pImageListProp->GetIntArrayPtr()->at(i);
			TabCtrlItem.mask |= TCIF_IMAGE;
		}
					
		// add the new tab
		InsertItem(i, &TabCtrlItem );

		CString sToolTipTitle = mpTemplate->GetPropertyListItem(nTabsTTT, i);
		if( !sToolTipTitle.IsEmpty() )
		{
			CRect rectTab;
			GetItemRect(i, &rectTab);
			GetToolTipCtrl().AddTool(this, sToolTipTitle, &rectTab, i);
		}
	}
}

void VdclTab::ResetTooltips()
{
	RefCountedPtr< CPropertyObject > pTabsTTTProperty = mpTemplate->GetPropertyObject( nTabsTTT );
	GetToolTipCtrl().RemoveAllTools();
	RefCountedPtr< CPropertyObject > pToolTipBalloon = mpTemplate->GetPropertyObject( nToolTipBalloon );
	GetToolTipCtrl().SetDefaultSizes( !pToolTipBalloon || pToolTipBalloon->GetBooleanValue() );
	size_t nTabQty = pTabsTTTProperty->size();
	for (size_t i = 0; i < GetItemCount(); i++)
	{
		TC_ITEM tcItem;
		tcItem.mask = TCIF_PARAM;
		GetItem( i, &tcItem );
		size_t idx = tcItem.lParam;
		assert( idx < nTabQty );
		if( idx >= nTabQty )
			continue;
		CString sToolTipTitle = mpTemplate->GetPropertyListItem( nTabsTTT, idx );
		if( !sToolTipTitle.IsEmpty() )
		{
			CRect rectTab;
			GetItemRect(i, &rectTab);
			GetToolTipCtrl().AddTool(this, sToolTipTitle, &rectTab, i);
		}
	}
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
			DeleteItem(i);
			ResetTooltips();
			return;
		}
	}	
}

void VdclTab::ShowTab(int nIndex)
{
	int idxToInsertAt = 0;
	// ensure the tab is not already showing.
	for (int i = GetItemCount() - 1; i >= 0 ; i--)
	{
		//  Get the current tab item text.
		TC_ITEM tcItem;
		tcItem.mask = TCIF_PARAM;
		GetItem(i, &tcItem);
		
		if (nIndex == tcItem.lParam)
			return;
		if (nIndex > tcItem.lParam)
		{
			idxToInsertAt = i + 1;
			break;
		}
	}

	CString sTabCaption = mpTemplate->GetPropertyListItem(nTabsCaption, nIndex);
				
	// set the image list item number is required
	int nImage = -1;
	RefCountedPtr< CPropertyObject > pImageListProp = mpTemplate->GetPropertyObject(nTabsImageList);
	if (pImageListProp && nIndex < pImageListProp->GetIntArrayPtr()->size())
		nImage = pImageListProp->GetIntArrayPtr()->at(nIndex);
				
	// add the new tab
	InsertItem(idxToInsertAt, sTabCaption, nImage);

	//  Set the item in the tab control.
	TC_ITEM tcItem;
	tcItem.mask = TCIF_PARAM;
	tcItem.lParam = (LPARAM)nIndex;
	SetItem(idxToInsertAt, &tcItem);
	ResetTooltips();
}

bool VdclTab::CreateTabPages( UINT& nId )
{
	bool bFailed = false;

	CRect rectPage = GetUsedArea();
	POSITION pos = mpTemplate->GetOwnerProject()->GetDclFormList().GetHeadPosition();
	while (pos)
	{
		CDclFormObject* pDclForm = mpTemplate->GetOwnerProject()->GetDclFormList().GetNext(pos);
		pDclForm->EnsureIsLoaded();
		if( pDclForm->GetParentForm() == mpTemplate->GetOwnerForm() )
		{
			CTabPage* pNewPage = new CTabPage( pDclForm, this, rectPage, nId );
			short nTabIndex = pDclForm->GetTabIndex();
			if( nTabIndex >= mTabPages.size() )
				mTabPages.push_back( pNewPage );
			else
				mTabPages.insert( mTabPages.begin() + nTabIndex, pNewPage );
		}
	}
	return !bFailed;
}


BEGIN_MESSAGE_MAP(VdclTab, CTabCtrl)
	ON_NOTIFY_REFLECT(TCN_SELCHANGE, OnSelchange)
	ON_NOTIFY_REFLECT(TCN_SELCHANGING, OnSelchanging)	
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_HSCROLL()
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
	RefCountedPtr< CPropertyObject > pTabsTTTProperty = mpTemplate->GetPropertyObject(nTabsTTT);
	// get the tool tip object
	CToolTipCtrl* ThisToolTip = GetToolTips();

	
	// call method to display the appropriate tab pane
	ActivateTabPage(nCurrentSelectedTab, true, true);
	
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
		GetTabPageAt( m_nCurrentSelectedTab )->ShowWindow(SW_HIDE);

	CDclFormObject* pSourceForm = pActualTabPage->GetControlPane().GetSourceForm();
	if (pSourceForm != NULL)
	{
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
		pActualTabPage->ShowWindow(bShow? SW_SHOW : SW_HIDE);
		bool bNewPage = (m_nCurrentSelectedTab != nTabPageToActivate);
		m_nCurrentSelectedTab = nTabPageToActivate;

		// set the selection index if this tab is to be shown
		if (bShow)
		{
			pActualTabPage->GetControlPane().ShowPictureBoxes(TRUE);
			
			// force the tab pane to get focus
			if( bNewPage )
				SetFirstControlFocus(pActualTabPage);
			RedrawWindow();
		}
	}

	if (bFireEvent)
		// call method to invoke autolisp function
		InvokeMethodInt(mpTemplate->GetStrProperty(nEventChanged), m_nCurrentSelectedTab, m_bInvokeWithSendString);
}

void VdclTab::ZOrderFrontAllTabs()
{
	for( size_t idx = 0; idx < mTabPages.size(); ++idx )
		mTabPages.at( idx )->GetSourceForm()->ZOrderFrontAddTabControls();
}

void VdclTab::OnDestroy() 
{	
	SetImageList(NULL);
	GetToolTipCtrl().RemoveAllTools();
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
	CTabCtrl::OnSize(nType, cx, cy);
	ActivateTabPage(m_nCurrentSelectedTab, true, false);
}

void VdclTab::OnSelchanging(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// set the selection index
	int nCurrentSelectedTab = GetCurSel();

	// call method to invoke autolisp function
	InvokeMethodInt(mpTemplate->GetStrProperty(nEventSelChanging), nCurrentSelectedTab, m_bInvokeWithSendString);
	
	*pResult = 0;
}

BOOL VdclTab::PreTranslateMessage(MSG* pMsg) 
{
	GetToolTipCtrl().RelayEvent(pMsg);	
	return CTabCtrl::PreTranslateMessage(pMsg);
}

void VdclTab::OnKillFocus(CWnd* pNewWnd) 
{
	CTabCtrl::OnKillFocus(pNewWnd);
	
	// call methods to invoke the event
	InvokeMethod(mpTemplate->GetStrProperty(nEventKillFocus), m_bInvokeWithSendString);
}

void VdclTab::OnSetFocus(CWnd* pOldWnd) 
{
	CTabCtrl::OnSetFocus(pOldWnd);
	
	// call methods to invoke the event
	InvokeMethod(mpTemplate->GetStrProperty(nEventSetFocus), m_bInvokeWithSendString);
}

void VdclTab::PostNcDestroy() 
{
	CTabCtrl::PostNcDestroy();
	delete this;
}

HBRUSH VdclTab::CtlColor(CDC* pDC, UINT /*nCtlColor*/)
{
	pDC->SelectObject(CBrush(COLORREF(0)));
	return NULL;
}

void VdclTab::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	__super::OnHScroll(nSBCode, nPos, pScrollBar);
	ResetTooltips();
}
