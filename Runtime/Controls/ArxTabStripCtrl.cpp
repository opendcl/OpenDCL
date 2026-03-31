// ArxTabStripCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ArxTabStripCtrl.h"
#include "Resource.h"
#include "InvokeMethod.h"
#include "ControlPane.h"
#include "PropertyIds.h"
#include "DclControlTemplate.h"
#include "PropertyObject.h"
#include "TabPageDlg.h"
#include "DclFormTemplate.h"
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
	ResetTooltips();
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

	std::vector< CTabPageDlg* > rTabPages;

	CRect rectPage = GetUsedArea();
	const TDclFormList& Forms = mpTemplate->GetOwnerProject()->GetDclFormList();
	for( TDclFormList::const_iterator iter = Forms.begin(); iter != Forms.end(); ++iter )
	{
		if( (*iter)->GetParentForm() == mpTemplate->GetOwnerForm() )
		{
			CTabPageDlg* pNewPage = new CTabPageDlg( (*iter), this, rectPage, nId );
			size_t nTabIndex = static_cast<size_t>( (*iter)->GetTabIndex() );
			if( rTabPages.size() <= nTabIndex )
				rTabPages.resize( nTabIndex + 1 );
			CTabPageDlg*& pPage = rTabPages[nTabIndex];
			assert( pPage == NULL ); //duplicate tab index!
			pPage = pNewPage;
		}
	}
	for( std::vector< CTabPageDlg* >::const_iterator iter = rTabPages.begin();
			 iter != rTabPages.end();
			 ++iter )
	{
		CTabPageDlg* pPage = *iter;
		if( !pPage )
			continue;
		mTabPages.push_back( pPage );
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
	{
		pPreviousTabPage->ShowWindow( SW_HIDE );
		OnNeedRepaint( true, true );
	}
	if( pTabPage )
	{
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
	}
	OnNeedRepaint();

	if( bFireEvent )
		GetArxServices()->HandleEvent( Prop::EventChanged, args_N( GetCurTabPage() ) );
}

void CArxTabStripCtrl::SetFirstControlFocus( CTabPageDlg* pTabPage )
{
	pTabPage->GetControlPane()->SetFirstControlFocus();
}


BEGIN_MESSAGE_MAP(CArxTabStripCtrl, CTabStripCtrl)
	ON_NOTIFY_REFLECT(TCN_SELCHANGE, OnCbnSelchange)
	ON_NOTIFY_REFLECT(TCN_SELCHANGING, OnSelchanging)	
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CArxTabStripCtrl message handlers

void CArxTabStripCtrl::OnCbnSelchange( NMHDR* pNMHDR, LRESULT* pResult ) 
{
	ActivateTabPage( GetTabPageAt( GetCurTabPage() ), true );
	*pResult = 0;
	__super::OnCbnSelchange( pNMHDR, pResult );
}

void CArxTabStripCtrl::OnSelchanging( NMHDR* pNMHDR, LRESULT* pResult ) 
{
	*pResult = FALSE;
	CString sSelChangingEvent = mpTemplate->GetStringProperty( Prop::EventSelChanging );
	if( !sSelChangingEvent.IsEmpty() )
	{
		resbuf rbCurrentPage = { NULL, RTSHORT };
		rbCurrentPage.resval.rint = GetCurTabPage();
		resbuf* prbResult = NULL;
		GetArxServices()->HandleEvent( sSelChangingEvent, prbResult, &rbCurrentPage );
		if( prbResult )
		{
			if( !prbResult->rbnext && prbResult->restype == RTT )
				*pResult = TRUE; //prevent change
			acutRelRb( prbResult );
		}
	}
}

void CArxTabStripCtrl::OnKillFocus( CWnd* pNewWnd ) 
{
	__super::OnKillFocus( pNewWnd );
	GetArxServices()->HandleEvent( Prop::EventKillFocus );
}

void CArxTabStripCtrl::OnSetFocus( CWnd* pOldWnd ) 
{
	__super::OnSetFocus( pOldWnd );
	GetArxServices()->HandleEvent( Prop::EventSetFocus );
}

LRESULT CArxTabStripCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	return __super::WindowProc(message, wParam, lParam);
}
