// TabStripCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "TabStripCtrl.h"
#include "ControlPane.h"
#include "DclImageList.h"
#include "DclControlTemplate.h"
#include "DialogObject.h"
#include "HostThemeHelper.h"
#include "PropertyIds.h"
#include "ColorService.h"


/////////////////////////////////////////////////////////////////////////////
// CTabStripCtrl

CTabStripCtrl::CTabStripCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CDialogControl( pTemplate, pPane, this )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CTabStripCtrl::~CTabStripCtrl()
{
}

bool CTabStripCtrl::Create( CWnd* pParentWnd, UINT nID )
{
	bool bSuccess = (CTabCtrl::Create( GetWndStyle(), GetWndRect(), pParentWnd, nID ) != FALSE);

	if( bSuccess )
		ModifyStyleEx( 0, WS_EX_CONTROLPARENT ); //this prevents the TAB key from locking up the dialog!

	if( bSuccess && !ApplyPropertiesEnum() )
		bSuccess = false;

	if( bSuccess )
		SyncHostTabTheme();

	return bSuccess;
}

DWORD CTabStripCtrl::GetWndStyle() const
{
	DWORD dwStyle = CDialogControl::GetWndStyle();
	dwStyle |= (WS_TABSTOP | WS_CLIPCHILDREN | TCS_FOCUSNEVER | TCS_TOOLTIPS);

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
	
	if( mpTemplate->GetLongProperty( Prop::LabelAlignment ) == 0 )
		dwStyle |= (TCS_FORCEICONLEFT | TCS_FORCELABELLEFT);

	return dwStyle;
}

void CTabStripCtrl::HandleDpiChanged()
{
	__super::HandleDpiChanged();
	ApplyPropertiesEnum();
	TPropertyPtr pTabsProp = mpTemplate->GetPropertyObject( Prop::TabsCaption );
	if( !pTabsProp )
		return;
	const TProjectPtr pProject = mpTemplate->GetOwnerProject();
	TDclFormPtr pOwnerForm = mpTemplate->GetOwnerForm();
	for( size_t idxTab = pTabsProp->size(); idxTab > 0; --idxTab )
	{
		TDclFormPtr pChildForm = pProject->FindDclTabChildForm( pOwnerForm->GetUniqueName(), idxTab - 1 );
		if( !pChildForm )
			continue;
		TDclControlPtr pFormProps = pChildForm->GetControlProperties();
		CDialogObject* pDlgObject = pChildForm->GetFormInstance();
		pDlgObject->HandleDpiChanged();
	}
}

bool CTabStripCtrl::ApplyProperty( TPropertyPtr pProp )
{
	if( !__super::ApplyProperty( pProp ) )
		return false;
	switch( pProp->GetID() )
	{
	case Prop::TabsCaption:
		SetupTabs();
		OnNeedRepaint();
		break;
	case Prop::TabsImageList:
		if( !IsEnumeratingProperties() )
			SetupTabs();
		OnNeedRepaint();
		break;
	case Prop::ImageList:
		{
			TImageListPtr pImageList = mpTemplate->GetImageList();
			if (pImageList && pImageList->GetImageList().GetSafeHandle())
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
		SetMinTabWidth( FromDIP( pProp->GetLongValue() ) );
		if( !IsEnumeratingProperties() )
		{
			if( mpTemplate->GetBooleanProperty( Prop::TabFixedWidth ) )
			{
				CRect rectTab;
				GetItemRect( 0, &rectTab );
				SetItemSize( CSize( FromDIP( pProp->GetLongValue() ), rectTab.Height() ) );
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
		if( !IsEnumeratingProperties() )
			ResetTooltips();
		break;
	case Prop::TabJustification: //not used
		break;
	case Prop::MultiRow:
		if( pProp->GetLongValue() == 0 )
			ModifyStyle( TCS_MULTILINE, TCS_SINGLELINE, SWP_FRAMECHANGED );
		else
			ModifyStyle( TCS_SINGLELINE, TCS_MULTILINE, SWP_FRAMECHANGED );
		if( !IsEnumeratingProperties() )
			ResetTooltips();
		break;
	case Prop::LabelAlignment:
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
			sizeTabs.cx = FromDIP( mpTemplate->GetLongProperty( Prop::MinTabWidth ) );
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

void CTabStripCtrl::ApplyPropertiesOrder( std::vector< Prop::Id >& ridFirst, std::vector< Prop::Id >& ridLast )
{
	__super::ApplyPropertiesOrder( ridFirst, ridLast );
	ridFirst.push_back( Prop::ImageList );
	ridFirst.push_back( Prop::TabsCaption );
	ridLast.push_back( Prop::TabsTTT );
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
	if( rectTab.bottom < rectTab.top )
		rectTab.bottom = rectTab.top;
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
		if (pImageListProp && i < pImageListProp->size())
		{
			TabCtrlItem.iImage = pImageListProp->GetConstIntArrayPtr()->at(i);
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
	OnUsedAreaChanged();
	TPropertyPtr pTabsTTTProperty = mpTemplate->GetPropertyObject( Prop::TabsTTT );
	GetToolTipCtrl().RemoveAllTools();
	TPropertyPtr pToolTipBalloon = mpTemplate->GetPropertyObject( Prop::ToolTipBalloon );
	GetToolTipCtrl().SetDefaultSizes( !pToolTipBalloon || pToolTipBalloon->GetBooleanValue() );
	size_t nTTQty = pTabsTTTProperty->size();
	size_t nTabQty = (size_t)GetItemCount();
	if( nTTQty < nTabQty )
		nTabQty = nTTQty;
	for (size_t i = 0; i < nTabQty; i++)
	{
		TC_ITEM tcItem;
		tcItem.mask = TCIF_PARAM;
		GetItem( i, &tcItem );
		size_t idx = tcItem.lParam;
		CString sToolTipTitle = mpTemplate->GetPropertyListItem( Prop::TabsTTT, idx );
		if( !sToolTipTitle.IsEmpty() )
		{
			CRect rectTab;
			GetItemRect(i, &rectTab);
			GetToolTipCtrl().AddTool(this, sToolTipTitle, &rectTab, i);
		}
	}
}

void CTabStripCtrl::OnUsedAreaChanged()
{
	TPropertyPtr pTabsProp = mpTemplate->GetPropertyObject( Prop::TabsCaption );
	if( !pTabsProp )
		return;
	const TProjectPtr pProject = mpTemplate->GetOwnerProject();
	TDclFormPtr pOwnerForm = mpTemplate->GetOwnerForm();
	CRect rcControlArea = GetUsedArea();
	ToDIP( rcControlArea );
	long lNewLeft = rcControlArea.left;
	long lNewTop = rcControlArea.top;
	CSize sizeNew = rcControlArea.Size();
	for( size_t idxTab = pTabsProp->size(); idxTab > 0; --idxTab )
	{
		TDclFormPtr pChildForm = pProject->FindDclTabChildForm( pOwnerForm->GetUniqueName(), idxTab - 1 );
		if( !pChildForm )
			continue;
		bool bChanged = false;
		TDclControlPtr pFormProps = pChildForm->GetControlProperties();
		CDialogObject* pDlgObject = pChildForm->GetFormInstance();
		if( pDlgObject )
		{
			CRect rcPage = pDlgObject->GetEffectiveWindowRect();
			ToDIP( rcPage );
			if( lNewLeft != rcPage.left || lNewTop != rcPage.top )
				pDlgObject->MoveDialog( lNewLeft, lNewTop );
		}
		if( sizeNew.cx != pFormProps->GetLongProperty( Prop::Width ) )
		{
			bChanged = true;
			if( pDlgObject )
				pDlgObject->SetPosWidth( sizeNew.cx );
			else
				pFormProps->SetLongProperty( Prop::Width, sizeNew.cx );
		}
		if( sizeNew.cy != pFormProps->GetLongProperty( Prop::Height ) )
		{
			bChanged = true;
			if( pDlgObject )
				pDlgObject->SetPosHeight( sizeNew.cy );
			else
				pFormProps->SetLongProperty( Prop::Height, sizeNew.cy );
		}
		if( bChanged && pDlgObject )
			pDlgObject->ApplyPosition();
	}
}


BEGIN_MESSAGE_MAP(CTabStripCtrl, CTabCtrl)
	ON_WM_DESTROY()
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_HSCROLL()
	ON_NOTIFY_REFLECT(TCN_SELCHANGE, OnCbnSelchange)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnNMCustomDraw)
	ON_WM_NCHITTEST()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_MESSAGE(WM_DPICHANGED_AFTERPARENT, &CTabStripCtrl::OnDpiChanged)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTabStripCtrl message handlers

LRESULT CTabStripCtrl::OnDpiChanged(WPARAM wParam, LPARAM lParam)
{
	HandleDpiChanged();
	return 0;
}

void CTabStripCtrl::OnDestroy() 
{	
	SetImageList(NULL);
	GetToolTipCtrl().RemoveAllTools();
	__super::OnDestroy();
}

BOOL CTabStripCtrl::PreTranslateMessage(MSG* pMsg) 
{
	GetToolTipCtrl().RelayEvent(pMsg);	
	return __super::PreTranslateMessage(pMsg);
}

HBRUSH CTabStripCtrl::CtlColor(CDC* pDC, UINT nCtlColor)
{
	return HandleCtlColor( pDC, nCtlColor );
}

BOOL CTabStripCtrl::OnEraseBkgnd(CDC* pDC)
{
	if( CHostThemeHelper::HostMaps() )
	{
		CRect rc;
		GetClientRect( &rc );
		pDC->FillSolidRect( &rc, OdclSysColor( COLOR_BTNFACE ) );
		return TRUE;
	}
	//if( HandleEraseBkgnd( pDC ) )
	//	return TRUE;
	return __super::OnEraseBkgnd(pDC);
}

void CTabStripCtrl::OnPaint() 
{
	PAINTSTRUCT ps;
	/*CDC* pDC = */BeginPaint( &ps );
	EndPaint( &ps );
	InvalidateRect( &ps.rcPaint );
	//CRect rcTarget = GetUsedArea();
	//if( rcTarget.IntersectRect( &rcTarget, &ps.rcPaint ) )
	//{
	//	for( CWnd* pTabPage = GetWindow( GW_CHILD ); pTabPage; pTabPage = pTabPage->GetWindow( GW_HWNDNEXT ) )
	//	{
	//		if( !pTabPage->IsWindowVisible() )
	//			continue;
	//		CRect rcPage = rcTarget;
	//		ClientToScreen( &rcPage );
	//		pTabPage->ScreenToClient( &rcPage );
	//		pTabPage->InvalidateRect( &rcPage );
	//		for( CWnd* pChild = pTabPage->GetWindow( GW_CHILD ); pChild; pChild = pChild->GetWindow( GW_HWNDNEXT ) )
	//		{
	//			if( !pChild->IsWindowVisible() )
	//				continue;
	//			if( pChild->GetExStyle() & WS_EX_TRANSPARENT )
	//				continue;
	//			CRect rcChild;
	//			pChild->GetWindowRect( &rcChild );
	//			ScreenToClient( &rcChild );
	//			rcChild.IntersectRect( &rcChild, &rcTarget );
	//			ValidateRect( &rcChild );
	//		}
	//	}
	//}
	CRect rcPaint = ps.rcPaint;
	__super::OnPaint();
	if( CHostThemeHelper::HostMaps() )
	{
		CClientDC dc( this );
		PaintHostThemedTabs( &dc );
	}
	if( !rcPaint.IsRectEmpty() )
	{
		CRect rcTarget = GetUsedArea();
		for( CWnd* pTabPage = GetWindow( GW_CHILD ); pTabPage; pTabPage = pTabPage->GetWindow( GW_HWNDNEXT ) )
		{
			if( !pTabPage->IsWindowVisible() )
				continue;
			CRect rcPage;
			rcPage.IntersectRect( &rcTarget, &rcPaint );
			ClientToScreen( &rcPage );
			pTabPage->ScreenToClient( &rcPage );
			pTabPage->RedrawWindow( &rcPage, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_ALLCHILDREN );
		}
	}
}

void CTabStripCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	__super::OnHScroll(nSBCode, nPos, pScrollBar);
	ResetTooltips();
}

void CTabStripCtrl::OnCbnSelchange( NMHDR* pNMHDR, LRESULT* pResult ) 
{
	ResetTooltips();
	*pResult = 0;
}

__UINT_LRESULT CTabStripCtrl::OnNcHitTest(CPoint point)
{
	__UINT_LRESULT nHitTest = __super::OnNcHitTest(point);
	if( nHitTest == HTTRANSPARENT )
		nHitTest = HTCLIENT;
	return nHitTest;
}

LRESULT CTabStripCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = __super::WindowProc(message, wParam, lParam);
	switch( message )
	{
	case WM_WINDOWPOSCHANGING:
		break;
	case WM_WINDOWPOSCHANGED:
		break;
	case WM_SIZE: //subclass' OnSize() never gets called, so intercept it here
		ResetTooltips();
		break;
	}
	return lResult;
}

void CTabStripCtrl::PostNcDestroy() 
{
	__super::PostNcDestroy();
	delete this;
}

HBRUSH CTabStripCtrl::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = __super::OnCtlColor(pDC, pWnd, nCtlColor);
	if( CHostThemeHelper::HostMaps() )
	{
		// Match tab-page erase (COLOR_BTNFACE), not COLOR_WINDOW.
		mColorService.SetBackgroundColor( -16L );
		pDC->SetTextColor( OdclSysColor( COLOR_BTNTEXT ) );
		pDC->SetBkColor( mColorService.GetBackgroundColor() );
		return mColorService.GetBackgroundBrush();
	}
	if( GetTheme().GetWindowTheme() )
	{
		mColorService.SetBackgroundColor( -6L );
		return mColorService.GetBackgroundBrush();
	}
	return hbr;
}

bool CTabStripCtrl::OnApplyUseVisualStyle( TPropertyPtr pProp )
{
	if( !CDialogControl::OnApplyUseVisualStyle( pProp ) )
		return false;
	SyncHostTabTheme();
	return true;
}

void CTabStripCtrl::HandleHostThemeChanged()
{
	SyncHostTabTheme();
}

void CTabStripCtrl::SyncHostTabTheme()
{
	if( !m_hWnd )
		return;
	const bool bVisual = mpTemplate && mpTemplate->GetBooleanProperty( Prop::UseVisualStyle );
	LPCWSTR pszTheme = CHostThemeHelper::ThemeClass( bVisual );
	GetTheme().SetWindowTheme( pszTheme, pszTheme );
	CHostThemeHelper::Apply( m_hWnd, pszTheme );
	OnNeedRepaint( true );
}

void CTabStripCtrl::DrawHostThemedTabItem( CDC* pDC, const CRect& rcItem, bool bSelected, bool bHot, const CString& sText, int nImage )
{
	if( !pDC || rcItem.IsRectEmpty() )
		return;

	const COLORREF crFace = OdclSysColor( COLOR_BTNFACE );
	const COLORREF crWindow = OdclSysColor( COLOR_WINDOW );
	const COLORREF crText = OdclSysColor( COLOR_BTNTEXT );
	const COLORREF crEdge = OdclSysColor( COLOR_3DSHADOW );
	const bool bButtons = (GetStyle() & TCS_BUTTONS) != 0;

	// Selected matches tab page face; inactive uses window (darker under HostMaps).
	COLORREF crFill = bSelected ? crFace : crWindow;
	if( bHot && !bSelected )
		crFill = crFace;

	pDC->FillSolidRect( &rcItem, crFill );

	CPen pen( PS_SOLID, 1, crEdge );
	CPen* pOldPen = pDC->SelectObject( &pen );
	const int xL = rcItem.left;
	const int xR = rcItem.right - 1;
	const int yT = rcItem.top;
	const int yB = rcItem.bottom - 1;
	pDC->MoveTo( xL, yB );
	pDC->LineTo( xL, yT );
	pDC->LineTo( xR, yT );
	pDC->LineTo( xR, yB + 1 );
	if( bButtons || !bSelected )
	{
		pDC->MoveTo( xL, yB );
		pDC->LineTo( xR + 1, yB );
	}
	pDC->SelectObject( pOldPen );

	CRect rcText = rcItem;
	rcText.DeflateRect( 6, 2, 6, 2 );
	if( nImage >= 0 )
	{
		CImageList* pList = GetImageList();
		if( pList && pList->GetSafeHandle() )
		{
			IMAGEINFO ii = {};
			if( pList->GetImageInfo( nImage, &ii ) )
			{
				const int cx = ii.rcImage.right - ii.rcImage.left;
				const int cy = ii.rcImage.bottom - ii.rcImage.top;
				const int x = rcText.left;
				const int y = rcText.top + max( 0, (rcText.Height() - cy) / 2 );
				pList->Draw( pDC, nImage, CPoint( x, y ), ILD_TRANSPARENT );
				rcText.left += cx + 4;
			}
		}
	}

	const COLORREF crOldText = pDC->SetTextColor( crText );
	const int nOldBk = pDC->SetBkMode( TRANSPARENT );
	CFont* pFont = GetFont();
	CFont* pOldFont = pFont ? pDC->SelectObject( pFont ) : NULL;
	UINT nFormat = DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS;
	if( mpTemplate && mpTemplate->GetLongProperty( Prop::LabelAlignment ) == 0 )
		nFormat |= DT_LEFT;
	else
		nFormat |= DT_CENTER;
	pDC->DrawText( sText, &rcText, nFormat );
	if( pOldFont )
		pDC->SelectObject( pOldFont );
	pDC->SetBkMode( nOldBk );
	pDC->SetTextColor( crOldText );
}

void CTabStripCtrl::PaintHostThemedTabs( CDC* pDC )
{
	if( !pDC || !CHostThemeHelper::HostMaps() )
		return;

	CRect rcClient;
	GetClientRect( &rcClient );
	int nMaxBottom = 0;
	const int nCount = GetItemCount();
	for( int i = 0; i < nCount; ++i )
	{
		CRect rcItem;
		if( GetItemRect( i, &rcItem ) && rcItem.bottom > nMaxBottom )
			nMaxBottom = rcItem.bottom;
	}
	if( nMaxBottom > 0 )
	{
		CRect rcRow = rcClient;
		rcRow.bottom = nMaxBottom + 2;
		pDC->FillSolidRect( &rcRow, OdclSysColor( COLOR_BTNFACE ) );
	}

	for( int i = 0; i < nCount; ++i )
	{
		CRect rcItem;
		if( !GetItemRect( i, &rcItem ) )
			continue;
		TCITEM item = {};
		item.mask = TCIF_TEXT | TCIF_IMAGE;
		TCHAR sz[256] = {};
		item.pszText = sz;
		item.cchTextMax = 255;
		if( !GetItem( i, &item ) )
			continue;
		const bool bSelected = (GetCurSel() == i);
		DrawHostThemedTabItem( pDC, rcItem, bSelected, false, CString( sz ), item.iImage );
	}
}

void CTabStripCtrl::OnNMCustomDraw( NMHDR* pNMHDR, LRESULT* pResult )
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast< LPNMCUSTOMDRAW >( pNMHDR );
	*pResult = CDRF_DODEFAULT;
	if( !pNMCD || !CHostThemeHelper::HostMaps() )
		return;

	switch( pNMCD->dwDrawStage )
	{
	case CDDS_PREPAINT:
		{
			CDC* pDC = CDC::FromHandle( pNMCD->hdc );
			if( pDC )
			{
				CRect rc;
				GetClientRect( &rc );
				CRect rcRow = rc;
				int nMaxBottom = 0;
				for( int i = GetItemCount() - 1; i >= 0; --i )
				{
					CRect rcItem;
					if( GetItemRect( i, &rcItem ) && rcItem.bottom > nMaxBottom )
						nMaxBottom = rcItem.bottom;
				}
				if( nMaxBottom > 0 )
					rcRow.bottom = nMaxBottom + 2;
				pDC->FillSolidRect( &rcRow, OdclSysColor( COLOR_BTNFACE ) );
			}
			*pResult = CDRF_NOTIFYITEMDRAW;
		}
		break;
	case CDDS_ITEMPREPAINT:
		{
			const int nItem = (int)pNMCD->dwItemSpec;
			TCITEM item = {};
			item.mask = TCIF_TEXT | TCIF_IMAGE;
			TCHAR sz[256] = {};
			item.pszText = sz;
			item.cchTextMax = 255;
			if( !GetItem( nItem, &item ) )
				break;
			const bool bSelected = (GetCurSel() == nItem) || ((pNMCD->uItemState & CDIS_SELECTED) != 0);
			const bool bHot = (pNMCD->uItemState & CDIS_HOT) != 0;
			CDC* pDC = CDC::FromHandle( pNMCD->hdc );
			DrawHostThemedTabItem( pDC, CRect( pNMCD->rc ), bSelected, bHot, CString( sz ), item.iImage );
			*pResult = CDRF_SKIPDEFAULT;
		}
		break;
	}
}
