// ArxTabStripCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ArxTabStripCtrl.h"
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
// CArxTabStripCtrl

CArxTabStripCtrl::CArxTabStripCtrl( TDclControlPtr pTemplate,
																		CControlPane* pPane,
																		UINT nID,
																		bool bCreate /*= true*/ )
: CTabStripCtrl( pTemplate, pPane, nID, false )
, mArxServices( this )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CArxTabStripCtrl::~CArxTabStripCtrl()
{
}

bool CArxTabStripCtrl::Create( CWnd* pParentWnd, UINT nID )
{
	bool bSuccess =
		__super::Create( pParentWnd, nID );
	if( !bSuccess )
		return false;

	CreateTabPages( nID );
	ActivateTabPage( GetTabPageAt( GetCurTabPage() ), true );

	return bSuccess;
}

bool CArxTabStripCtrl::GetChildPanes( std::list< const CControlPane* >& listChildren ) const
{
	size_t idx = mTabPages.size();
	while( idx-- > 0 )
		listChildren.push_back( mTabPages.at( idx )->GetControlPane() );
	return true;
}

int CArxTabStripCtrl::GetTabItemIndex( size_t nPageIndex ) const
{
	if( nPageIndex >= mTabPages.size() )
		return -1;
	for (int i = 0; i < GetItemCount(); i++)
	{
		TC_ITEM tcItem = { TCIF_PARAM };
		GetItem(i, &tcItem);
		if (nPageIndex == tcItem.lParam)
			return i;
	}
	return -1;
}

size_t CArxTabStripCtrl::GetTabPageIndex( int nItemIndex ) const
{
	TC_ITEM tcItem = { TCIF_PARAM };
	if( !GetItem(nItemIndex, &tcItem) )
		return (size_t)-1;
	return (size_t)tcItem.lParam;
}

TTabPagePtr CArxTabStripCtrl::GetTabPageAt( size_t nPageIndex ) const
{
	if( nPageIndex >= mTabPages.size() )
		return NULL;
	return mTabPages.at( nPageIndex );
}

const TDclFormPtr CArxTabStripCtrl::GetTabSourceFormAt( size_t nPageIndex ) const
{
	if( nPageIndex >= mTabPages.size() )
		return NULL;
	return mTabPages.at( nPageIndex )->GetSourceForm();
}
//
//const CControlPane* CArxTabStripCtrl::GetTabControlPaneAt( size_t nPageIndex ) const
//{
//	if( nPageIndex >= mTabPages.size() )
//		return NULL;
//	return mTabPages.at( nPageIndex )->GetControlPane();
//}

bool CArxTabStripCtrl::SetCurrentTab( size_t nPageIndex )
{
	TTabPagePtr pTabPage = GetTabPageAt( nPageIndex );
	if( !pTabPage )
		return false;
	if( !pTabPage->GetSourceForm()->GetControlProperties()->GetBooleanProperty( Prop::Visible ) )
		ShowTab( nPageIndex );
	SetCurSel( GetTabItemIndex( nPageIndex ) );
	ActivateTabPage( pTabPage, true );
	return true;
}

void CArxTabStripCtrl::HideTab( size_t nPageIndex )
{
	TTabPagePtr pTabPage = GetTabPageAt( nPageIndex );
	if( !pTabPage )
		return;
	pTabPage->GetSourceForm()->GetControlProperties()->SetBooleanProperty( Prop::Visible, false );
	int nItemIndex = GetTabItemIndex( nPageIndex );
	if( nItemIndex < 0 )
		return;

	bool bHideCurrent = (nItemIndex == GetCurSel());
	DeleteItem( nItemIndex );
	ResetTooltips();
	if( bHideCurrent )
	{
		if( nItemIndex >= GetItemCount() )
			--nItemIndex;
		SetCurSel( nItemIndex );
		ActivateTabPage( GetTabPageAt( GetTabPageIndex( nItemIndex ) ), true );
	}
	OnNeedRepaint();
}

void CArxTabStripCtrl::ShowTab( size_t nPageIndex )
{
	TTabPagePtr pTabPage = GetTabPageAt( nPageIndex );
	if( !pTabPage )
		return;
	pTabPage->GetSourceForm()->GetControlProperties()->SetBooleanProperty( Prop::Visible, true );

	int idxToInsertAt = 0;
	for( int i = GetItemCount() - 1; i >= 0; --i )
	{
		TC_ITEM tcItem;
		tcItem.mask = TCIF_PARAM;
		GetItem( i, &tcItem );
		
		if( nPageIndex == tcItem.lParam )
			return; //the tab is already showing
		if( nPageIndex > tcItem.lParam )
		{
			idxToInsertAt = i + 1;
			break;
		}
	}

	CString sTabCaption = mpTemplate->GetPropertyListItem( Prop::TabsCaption, nPageIndex );
				
	int nImage = -1;
	TPropertyPtr pImageListProp = mpTemplate->GetPropertyObject(Prop::TabsImageList);
	if (pImageListProp && nPageIndex < pImageListProp->GetIntArrayPtr()->size())
		nImage = pImageListProp->GetIntArrayPtr()->at( nPageIndex );
				
	InsertItem( idxToInsertAt, sTabCaption, nImage );
	TC_ITEM tcItem;
	tcItem.mask = TCIF_PARAM;
	tcItem.lParam = (LPARAM)nPageIndex;
	SetItem( idxToInsertAt, &tcItem );

	ResetTooltips();
	if( GetCurSel() == idxToInsertAt )
		ActivateTabPage( pTabPage, true );
	OnNeedRepaint();
}

bool CArxTabStripCtrl::CreateTabPages( UINT& nId )
{
	bool bFailed = false;

	CRect rectPage = GetUsedArea();
	const TDclFormList& Forms = mpTemplate->GetOwnerProject()->GetDclFormList();
	for( TDclFormList::const_iterator iter = Forms.begin(); iter != Forms.end(); ++iter )
	{
		if( (*iter)->GetParentForm() == mpTemplate->GetOwnerForm() )
		{
			CTabPage* pNewPage = new CTabPage( (*iter), this, rectPage, nId );
			short nTabIndex = (*iter)->GetTabIndex();
			if( nTabIndex >= mTabPages.size() )
				mTabPages.push_back( pNewPage );
			else
				mTabPages.insert( mTabPages.begin() + nTabIndex, pNewPage );
		}
	}
	return !bFailed;
}

void CArxTabStripCtrl::DestroyTabPages()
{
	size_t idx = mTabPages.size();
	while( idx-- > 0 )
	{
		TTabPagePtr pTabPage = mTabPages.at( idx );
		pTabPage->GetControlPane()->CleanUpControls();
		pTabPage->DestroyWindow();
	}
	mTabPages.clear();
}

void CArxTabStripCtrl::ActivateTabPage( TTabPagePtr pTabPage, bool bFireEvent /*= false*/ ) 
{
	TTabPagePtr pPreviousTabPage = mpActiveTabPage;
	mpActiveTabPage = pTabPage;
	bool bNewPage = (pPreviousTabPage != pTabPage);
	if( bNewPage && pPreviousTabPage )
		pPreviousTabPage->ShowWindow( SW_HIDE );
	if( !pTabPage )
		return;

	TDclFormPtr pSourceForm = pTabPage->GetSourceForm();
	if( !pSourceForm )
		return;

	//CRect rectTab = GetUsedArea();
	//pTabPage->SetWindowPos( NULL, 
	//												rectTab.left,
	//												rectTab.top,
	//												rectTab.Width(),
	//												rectTab.Height(),
	//												SWP_FRAMECHANGED | SWP_NOCOPYBITS | SWP_NOZORDER | SWP_NOOWNERZORDER );
	//pTabPage->GetControlPane()->RecalcLayout();
	pTabPage->ShowWindow( SW_SHOW );
	if( bNewPage )
		SetFirstControlFocus( pTabPage );
	OnNeedRepaint();

	if (bFireEvent)
		InvokeMethodInt( mpTemplate->GetStringProperty(Prop::EventChanged), GetCurTabPage(), IsAsyncEvents() );
}

void CArxTabStripCtrl::SetFirstControlFocus( CTabPage* pTabPage )
{
	pTabPage->GetControlPane()->SetFirstControlFocus();
}


BEGIN_MESSAGE_MAP(CArxTabStripCtrl, CTabCtrl)
	ON_NOTIFY_REFLECT(TCN_SELCHANGE, OnSelchange)
	ON_NOTIFY_REFLECT(TCN_SELCHANGING, OnSelchanging)	
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CArxTabStripCtrl message handlers

void CArxTabStripCtrl::OnSelchange( NMHDR* pNMHDR, LRESULT* pResult ) 
{
	ActivateTabPage( GetTabPageAt( GetCurTabPage() ), true );
	*pResult = 0;
	__super::OnSelchange( pNMHDR, pResult );
}

void CArxTabStripCtrl::OnSelchanging( NMHDR* pNMHDR, LRESULT* pResult ) 
{
	InvokeMethodInt( mpTemplate->GetStringProperty(Prop::EventSelChanging), GetCurTabPage(), IsAsyncEvents() );
	*pResult = 0;
}

void CArxTabStripCtrl::OnKillFocus( CWnd* pNewWnd ) 
{
	__super::OnKillFocus( pNewWnd );
	InvokeMethod( mpTemplate->GetStringProperty(Prop::EventKillFocus), IsAsyncEvents() );
}

void CArxTabStripCtrl::OnSetFocus( CWnd* pOldWnd ) 
{
	__super::OnSetFocus( pOldWnd );
	InvokeMethod( mpTemplate->GetStringProperty(Prop::EventSetFocus), IsAsyncEvents() );
}
