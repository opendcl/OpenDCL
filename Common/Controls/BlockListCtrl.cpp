// BlockListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "BlockListCtrl.h"
#include "ControlPane.h"
#include "Workspace.h"
#include "Resource.h"


/////////////////////////////////////////////////////////////////////////////
// CBlockListCtrl

CBlockListCtrl::CBlockListCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CListViewCtrl( pTemplate, pPane, nID, false )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CBlockListCtrl::~CBlockListCtrl()
{
}

bool CBlockListCtrl::Create( CWnd* pParentWnd, UINT nID )
{
	bool bSuccess = __super::Create( pParentWnd, nID );
	if( bSuccess )
	{
		mBlockViewImageList.Create( 32, 32, ILC_COLOR4, 1, 1 );
		mBlockViewImageList.SetBkColor( GetBkColor() );
		HICON hIcon = (HICON)::LoadImage( theWorkspace.GetLocalResourceModule(), MAKEINTRESOURCE(IDI_LARGEBLOCK), IMAGE_ICON, 0, 0, 0 );
		mBlockViewImageList.Add( hIcon );
		DestroyIcon( hIcon );
		SetImageList( &mBlockViewImageList, LVSIL_NORMAL );
		SetImageList( &mBlockViewImageList, LVSIL_SMALL );
	}

	return bSuccess;
}

DWORD CBlockListCtrl::GetWndStyle() const
{
	DWORD dwStyle = __super::GetWndStyle();
	dwStyle &= ~(LVS_SHAREIMAGELISTS | LVS_REPORT);

	switch( mpTemplate->GetLongProperty( Prop::BlockListStyle ) )
	{
	case 0:
		dwStyle |= (LVS_ICON | LVS_NOCOLUMNHEADER);
		break;
	case 1:
		dwStyle |= (LVS_LIST| LVS_NOCOLUMNHEADER);
		break;
	}

	return dwStyle;
}
