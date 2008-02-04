// FolderComboCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "FolderComboCtrl.h"
#include "DclControlObject.h"
#include "ControlPane.h"
#include "FolderTreeCtrl.h"
#include "PropertyIds.h"
#include "PropertyObject.h"


/////////////////////////////////////////////////////////////////////////////
// CFolderComboCtrl

CFolderComboCtrl::CFolderComboCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CFolderComboBox()
, CDialogControl( pTemplate, pPane, this )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CFolderComboCtrl::~CFolderComboCtrl()
{
}

bool CFolderComboCtrl::Create( CWnd* pParentWnd, UINT nID )
{
	bool bSuccess = __super::Create( pParentWnd, GetWndRect(), GetWndStyle(), nID );

	if( bSuccess )
		SetExtendedUI();

	if( bSuccess && !ApplyPropertiesEnum() )
		bSuccess = false;

	return bSuccess;
}

CRect CFolderComboCtrl::GetWndRect() const
{
	CRect rectCombo = CDialogControl::GetWndRect();
	long nListHeight = mpTemplate->GetLongProperty( Prop::DropDownHeight );
	if( nListHeight < 40 )
		nListHeight = 40;
	rectCombo.bottom += nListHeight;
	return rectCombo;
}

DWORD CFolderComboCtrl::GetWndStyle() const
{
	DWORD dwStyle = CDialogControl::GetWndStyle();
	dwStyle |= (CBS_HASSTRINGS | CBS_DROPDOWNLIST);
	if( mpTemplate->GetBooleanProperty( Prop::Sorted ) )
		dwStyle |= CBS_SORT;
	else
		dwStyle &= ~(DWORD)CBS_SORT;
	return dwStyle;
}

bool CFolderComboCtrl::OnApplyProperty( TPropertyPtr pProp )
{
	if( !__super::OnApplyProperty( pProp ) )
		return false;
	bool bFailed = false;
	switch( pProp->GetID() )
	{
	case Prop::List:
		break;
	case Prop::ItemData:
		{
			const PropVal::TIntArray* prInt = pProp->GetConstIntArrayPtr();
			if( prInt )
			{
				for( int idx = 0; (size_t)idx < prInt->size(); ++idx )
					SetItemData( idx, (DWORD_PTR)prInt->at( idx ) );
			}
		}
		break;
	case Prop::Text:
		SetWindowText( pProp->GetStringValue() );
		break;
	case Prop::LimitText:
		LimitText( pProp->GetLongValue() );
		break;
	}
	return !bFailed;
}


/////////////////////////////////////////////////////////////////////////////
// CFolderComboCtrl message handlers

BOOL CFolderComboCtrl::PreTranslateMessage(MSG* pMsg) 
{
	GetToolTipCtrl().RelayEvent(pMsg);
	return __super::PreTranslateMessage(pMsg);
}

void CFolderComboCtrl::PostNcDestroy() 
{
	__super::PostNcDestroy();
	delete this;
}
