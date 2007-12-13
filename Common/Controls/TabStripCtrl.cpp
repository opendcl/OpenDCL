// TabStripCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "TabStripCtrl.h"
#include "ControlPane.h"
#include "ImageListObject.h"
#include "DclControlObject.h"


/////////////////////////////////////////////////////////////////////////////
// CTabStripCtrl

CTabStripCtrl::CTabStripCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
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

	if( bSuccess && !OnApplyProperty( mpTemplate->GetPropertyObject( Prop::ImageList ) ) )
		bSuccess = false;
	if( bSuccess && !ApplyPropertiesEnum() )
		bSuccess = false;

	return bSuccess;
}

DWORD CTabStripCtrl::GetWndStyle() const
{
	DWORD dwStyle = CDialogControl::GetWndStyle();
	dwStyle |= (/*WS_CLIPCHILDREN | */TCS_FOCUSNEVER | TCS_TOOLTIPS);

	if( mpTemplate->GetLongProperty( Prop::TabStyle ) == 0 )
		dwStyle |= TCS_TABS;
	else
		dwStyle |= TCS_BUTTONS;

	if( mpTemplate->GetBooleanProperty( Prop::MultiRow ) )
		dwStyle |= TCS_MULTILINE;
	else
		dwStyle |= TCS_SINGLELINE;
	
	if( mpTemplate->GetBooleanProperty( Prop::TabFixedWidth ) )
		dwStyle |= TCS_FIXEDWIDTH;
	
	if( mpTemplate->GetLongProperty( Prop::TabLabelAlign ) == 0 )
		dwStyle |= (TCS_FORCEICONLEFT | TCS_FORCELABELLEFT);

	return dwStyle;
}

bool CTabStripCtrl::OnApplyProperty( TPropertyPtr pProp )
{
	if( !__super::OnApplyProperty( pProp ) )
		return false;
	switch( pProp->GetID() )
	{
	case Prop::TabsCaption:
		SetupTabs();
		Invalidate();
		break;
	case Prop::TabsImageList:
		if( !IsEnumeratingProperties() )
			SetupTabs();
		Invalidate();
		break;
	case Prop::ImageList:
		{
			RefCountedPtr< CImageListObject > pImageList = mpTemplate->GetImageList();
			if (pImageList)
			{
				CImageList& ImageList = pImageList->GetImageList();
				ImageList.SetBkColor( CLR_NONE );
				SetImageList( &ImageList );
			}
			else
				SetImageList( NULL );
		}
		break;
	case Prop::MinTabWidth:
		SetMinTabWidth( pProp->GetLongValue() );
		if( !IsEnumeratingProperties() )
		{
			if( mpTemplate->GetBooleanProperty( Prop::TabFixedWidth ) )
			{
				CRect rectTab;
				GetItemRect( 0, &rectTab );
				SetItemSize( CSize( pProp->GetLongValue(), rectTab.Height() ) );
			}
			SetupTabs();
		}
		ModifyStyle( 0, 0, SWP_FRAMECHANGED );
		break;
	case Prop::TabSelected:
		SetCurSel( GetTabItemIndex( pProp->GetLongValue() ) );
		break;
	case Prop::TabStyle:
		if( pProp->GetLongValue() == 0 )
			ModifyStyle( TCS_BUTTONS, TCS_TABS, SWP_FRAMECHANGED );
		else
			ModifyStyle( TCS_TABS, TCS_BUTTONS, SWP_FRAMECHANGED );
		ResetTooltips();
		break;
	case Prop::TabJustified: //not used
		break;
	case Prop::MultiRow:
		if( pProp->GetLongValue() == 0 )
			ModifyStyle( TCS_MULTILINE, TCS_SINGLELINE, SWP_FRAMECHANGED );
		else
			ModifyStyle( TCS_SINGLELINE, TCS_MULTILINE, SWP_FRAMECHANGED );
		ResetTooltips();
		break;
	case Prop::TabLabelAlign:
		if( pProp->GetLongValue() == 0 )
			ModifyStyle( 0, TCS_FORCEICONLEFT | TCS_FORCELABELLEFT, SWP_FRAMECHANGED );
		else
			ModifyStyle( TCS_FORCEICONLEFT | TCS_FORCELABELLEFT, 0, SWP_FRAMECHANGED );
		break;
	case Prop::TabFixedWidth:
		if( pProp->GetBooleanValue() )
		{
			ModifyStyle( TCS_RIGHTJUSTIFY, TCS_FIXEDWIDTH, SWP_FRAMECHANGED );
			CRect rectTab;
			GetItemRect( 0, &rectTab );
			CSize sizeTabs;
			sizeTabs.cx = mpTemplate->GetLongProperty( Prop::MinTabWidth );
			if( sizeTabs.cx < 0 )
				sizeTabs.cx = rectTab.Width();
			sizeTabs.cy = rectTab.Height();
			SetItemSize( sizeTabs );
		}
		else
			ModifyStyle( TCS_FIXEDWIDTH, TCS_RIGHTJUSTIFY, SWP_FRAMECHANGED );
		if( !IsEnumeratingProperties() )
			SetupTabs();
		break;
	case Prop::ToolTipBalloon:
		GetToolTipCtrl().SetDefaultSizes( pProp->GetBooleanValue() );
		break;
	case Prop::TabsTTT:
		ResetTooltips();
		break;
	}
	return true;
}

bool CTabStripCtrl::OnApplyToolTip( TPropertyPtr pProp )
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
	TPropertyPtr pTabsCaptionProperty = mpTemplate->GetPropertyObject(Prop::TabsCaption);
	TPropertyPtr pTabsTTTProperty = mpTemplate->GetPropertyObject(Prop::TabsTTT);
	TPropertyPtr pToolTipBalloon = mpTemplate->GetPropertyObject( Prop::ToolTipBalloon );
	GetToolTipCtrl().SetDefaultSizes( !pToolTipBalloon || pToolTipBalloon->GetBooleanValue() );

	size_t nTabQty = pTabsCaptionProperty->size();
	for (size_t i = 0; i < nTabQty; i++)
	{
		TC_ITEM TabCtrlItem;
		TabCtrlItem.mask = TCIF_TEXT | TCIF_PARAM;
		TabCtrlItem.lParam = (LPARAM)i;

		// get the tag caption
		CString sTabCaption = mpTemplate->GetPropertyListItem(Prop::TabsCaption, i);
		TabCtrlItem.pszText = sTabCaption.LockBuffer();

		// set the image list item number is required
		TPropertyPtr pImageListProp = mpTemplate->GetPropertyObject(Prop::TabsImageList);
		if (pImageListProp && i < pImageListProp->GetIntArrayPtr()->size())
		{
			TabCtrlItem.iImage = pImageListProp->GetIntArrayPtr()->at(i);
			TabCtrlItem.mask |= TCIF_IMAGE;
		}
					
		// add the new tab
		InsertItem(i, &TabCtrlItem );

		CString sToolTipTitle = mpTemplate->GetPropertyListItem(Prop::TabsTTT, i);
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
	TPropertyPtr pTabsTTTProperty = mpTemplate->GetPropertyObject( Prop::TabsTTT );
	GetToolTipCtrl().RemoveAllTools();
	TPropertyPtr pToolTipBalloon = mpTemplate->GetPropertyObject( Prop::ToolTipBalloon );
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
		CString sToolTipTitle = mpTemplate->GetPropertyListItem( Prop::TabsTTT, idx );
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
	ON_NOTIFY_REFLECT(TCN_SELCHANGE, OnSelchange)
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

HBRUSH CTabStripCtrl::CtlColor(CDC* pDC, UINT /*nCtlColor*/)
{
	return mbrushBackground;
}

void CTabStripCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	__super::OnHScroll(nSBCode, nPos, pScrollBar);
	ResetTooltips();
}

void CTabStripCtrl::OnSelchange( NMHDR* pNMHDR, LRESULT* pResult ) 
{
	ResetTooltips();
	*pResult = 0;
}

void CTabStripCtrl::PostNcDestroy() 
{
	CTabCtrl::PostNcDestroy();
	delete this;
}
