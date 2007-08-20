// TabStripCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "TabStripCtrl.h"
#include "ControlPane.h"
#include "ImageListObject.h"
#include "DclControlObject.h"


/////////////////////////////////////////////////////////////////////////////
// CTabStripCtrl

CTabStripCtrl::CTabStripCtrl( CDclControlObject* pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CDialogControl( pTemplate, pPane, this )
, mbrushBackground( RGB(0,0,0) )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CTabStripCtrl::~CTabStripCtrl()
{
	mbrushBackground.DeleteObject();
}

bool CTabStripCtrl::Create( CWnd* pParentWnd, UINT nID )
{
	CRect rectPane = GetWndRect();

	bool bSuccess = (CTabCtrl::Create( GetWndStyle(), rectPane, pParentWnd, nID ) != FALSE);

	RefCountedPtr< CImageListObject > pImageList = mpTemplate->GetImageList();
	if( pImageList )
		SetImageList( &pImageList->m_ImageList );

	SetupTabs();
	if( bSuccess && !ApplyPropertiesEnum() )
		bSuccess = false;

	return bSuccess;
}

DWORD CTabStripCtrl::GetWndStyle() const
{
	DWORD dwStyle = CDialogControl::GetWndStyle();
	dwStyle |= (/*WS_CLIPCHILDREN | */TCS_FOCUSNEVER | TCS_TOOLTIPS);

	if( mpTemplate->GetLongProperty( nTabStyle ) == 0 )
		dwStyle |= TCS_TABS;
	else
		dwStyle |= TCS_BUTTONS;

	if( mpTemplate->GetBoolProperty( nMultiRow ) )
		dwStyle |= TCS_MULTILINE;
	else
		dwStyle |= TCS_SINGLELINE;
	
	if( mpTemplate->GetBoolProperty( nTabFixedWidth ) )
		dwStyle |= TCS_FIXEDWIDTH;
	
	if( mpTemplate->GetLongProperty( nTabLabelAlign ) == 0 )
		dwStyle |= (TCS_FORCEICONLEFT | TCS_FORCELABELLEFT);

	return dwStyle;
}

bool CTabStripCtrl::OnApplyProperty( RefCountedPtr< CPropertyObject > pProp )
{
	if( !__super::OnApplyProperty( pProp ) )
		return false;
	switch( pProp->GetID() )
	{
	case nTabsCaption:
	case nTabsImageList:
		SetupTabs();
		Invalidate();
		break;
	case nImageList:
		{
			RefCountedPtr< CImageListObject > pImageList = mpTemplate->GetImageList();
			if (pImageList)
			{
				pImageList->m_ImageList.SetBkColor( ::GetSysColor( COLOR_BTNFACE ) );
				SetImageList( &pImageList->m_ImageList );
			}
			else
				SetImageList( NULL );
		}
		break;
	case nMinTabWidth:
		SetMinTabWidth(pProp->GetLongValue());
		SetupTabs();
		Invalidate();
		break;
	case nTabSelected:
		SetCurSel( GetTabItemIndex( pProp->GetLongValue() ) );
		break;
	case nTabStyle:
		if( pProp->GetLongValue() == 0 )
			ModifyStyle( TCS_BUTTONS, TCS_TABS, SWP_FRAMECHANGED );
		else
			ModifyStyle( TCS_TABS, TCS_BUTTONS, SWP_FRAMECHANGED );
		ResetTooltips();
		break;
	case nTabJustified: //not used
		break;
	case nMultiRow:
		if( pProp->GetLongValue() == 0 )
			ModifyStyle( TCS_MULTILINE, TCS_SINGLELINE, SWP_FRAMECHANGED );
		else
			ModifyStyle( TCS_SINGLELINE, TCS_MULTILINE, SWP_FRAMECHANGED );
		ResetTooltips();
		break;
	case nTabLabelAlign:
		if( pProp->GetLongValue() == 0 )
			ModifyStyle( 0, TCS_FORCEICONLEFT | TCS_FORCELABELLEFT, SWP_FRAMECHANGED );
		else
			ModifyStyle( TCS_FORCEICONLEFT | TCS_FORCELABELLEFT, 0, SWP_FRAMECHANGED );
		break;
	case nTabFixedWidth:
		if( pProp->GetBooleanValue() )
		{
			ModifyStyle( TCS_RIGHTJUSTIFY, TCS_FIXEDWIDTH, SWP_FRAMECHANGED );
			CRect rectTab;
			GetItemRect( 0, &rectTab );
			CSize sizeTabs;
			sizeTabs.cx = mpTemplate->GetLongProperty( nMinTabWidth );
			if( sizeTabs.cx < 0 )
				sizeTabs.cx = rectTab.Width();
			sizeTabs.cy = rectTab.Height();
			SetItemSize( sizeTabs );
		}
		else
			ModifyStyle( TCS_FIXEDWIDTH, TCS_RIGHTJUSTIFY, SWP_FRAMECHANGED );
		SetupTabs();
		break;
	case nToolTipBalloon:
		GetToolTipCtrl().SetDefaultSizes( pProp->GetBooleanValue() );
		break;
	case nTabsTTT:
		ResetTooltips();
		break;
	}
	return true;
}

bool CTabStripCtrl::OnApplyToolTip( RefCountedPtr< CPropertyObject > pProp )
{
	ResetTooltips();
	return true;
}

CRect CTabStripCtrl::GetUsedArea() const
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

void CTabStripCtrl::SetupTabs()
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

void CTabStripCtrl::ResetTooltips()
{
	RefCountedPtr< CPropertyObject > pTabsTTTProperty = mpTemplate->GetPropertyObject( nTabsTTT );
	GetToolTipCtrl().RemoveAllTools();
	RefCountedPtr< CPropertyObject > pToolTipBalloon = mpTemplate->GetPropertyObject( nToolTipBalloon );
	GetToolTipCtrl().SetDefaultSizes( !pToolTipBalloon || pToolTipBalloon->GetBooleanValue() );
	size_t nTabQty = pTabsTTTProperty->size();
	for (size_t i = 0; i < (size_t)GetItemCount(); i++)
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


BEGIN_MESSAGE_MAP(CTabStripCtrl, CTabCtrl)
	ON_WM_DESTROY()
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_HSCROLL()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTabStripCtrl message handlers

void CTabStripCtrl::OnDestroy() 
{	
	SetImageList(NULL);
	GetToolTipCtrl().RemoveAllTools();
	CTabCtrl::OnDestroy();
}

BOOL CTabStripCtrl::PreTranslateMessage(MSG* pMsg) 
{
	GetToolTipCtrl().RelayEvent(pMsg);	
	return CTabCtrl::PreTranslateMessage(pMsg);
}

void CTabStripCtrl::PostNcDestroy() 
{
	CTabCtrl::PostNcDestroy();
	delete this;
}

HBRUSH CTabStripCtrl::CtlColor(CDC* pDC, UINT /*nCtlColor*/)
{
	return mbrushBackground;
}

void CTabStripCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	__super::OnHScroll(nSBCode, nPos, pScrollBar);
	ResetTooltips();
}
