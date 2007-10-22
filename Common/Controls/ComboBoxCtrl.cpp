#include "stdafx.h"
#include "ComboBoxCtrl.h"
#include "DclControlObject.h"
#include "ControlPane.h"
#include "ComboHandler.h"


/////////////////////////////////////////////////////////////////////////////
// CComboBoxCtrl

CComboBoxCtrl::CComboBoxCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID,
															CComboHandler* pHandler /*= NULL*/, bool bCreate /*= true*/ )
: CDialogControl( pTemplate, pPane, this )
, CFilteredComboCtrl( pHandler? pHandler->GetInputFilter() : NULL )
, mpHandler( pHandler )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CComboBoxCtrl::~CComboBoxCtrl()
{
}

bool CComboBoxCtrl::Create( CWnd* pParentWnd, UINT nID )
{
	bool bSuccess = __super::Create( pParentWnd, GetWndRect(), GetWndStyle(), nID );

	if( bSuccess )
		SetExtendedUI();

	if( bSuccess && !ApplyPropertiesEnum() )
		bSuccess = false;

	if( bSuccess )
	{
		CComboHandler* pHandler = GetComboHandler();
		if( pHandler )
			bSuccess = pHandler->PopulateList( this );
	}

	return bSuccess;
}

CRect CComboBoxCtrl::GetWndRect() const
{
	CRect rectCombo = CDialogControl::GetWndRect();
	if( (GetComboStyle() & CBS_DROPDOWN) != 0 )
	{
		long nListHeight = mpTemplate->GetLongProperty( Prop::DropDownHeight );
		//if( nListHeight < 40 )
		//	nListHeight = 40;
		//long nItemHeight = 16;
		//CComboHandler* pHandler = const_cast< CComboBoxCtrl* >(this)->GetComboHandler();
		//if( pHandler )
		//	nItemHeight = pHandler->GetItemHeight();
		//nListHeight = nItemHeight * ((nListHeight + nItemHeight - 3) / nItemHeight) + 2; //make it an integral height, + 2 pixels for the border
		//mpTemplate->SetLongProperty( Prop::DropDownHeight, nListHeight );
		rectCombo.bottom += nListHeight;
	}
	return rectCombo;
}

DWORD CComboBoxCtrl::GetWndStyle() const
{
	DWORD dwStyle = CDialogControl::GetWndStyle();
	dwStyle |= (CBS_NOINTEGRALHEIGHT);
	dwStyle |= GetComboStyle();
	const CComboHandler* pHandler = const_cast< CComboBoxCtrl* >(this)->GetComboHandler();
	if( pHandler )
	{
		if( pHandler->IsOwnerDrawn() )
			dwStyle |= (pHandler->GetItemHeight() > 0? CBS_OWNERDRAWFIXED : CBS_OWNERDRAWVARIABLE);
		if( pHandler->IsAutoSorted() )
			dwStyle |= CBS_SORT;
		else
			dwStyle &= ~(DWORD)CBS_SORT;
	}
	else
	{
		if( mpTemplate->GetBooleanProperty( Prop::Sorted ) )
			dwStyle |= CBS_SORT;
		else
			dwStyle &= ~(DWORD)CBS_SORT;
	}
	dwStyle &= ~(DWORD)CBS_DROPDOWNLIST;
	dwStyle |= GetComboStyle();
	return dwStyle;
}

bool CComboBoxCtrl::OnApplyProperty( TPropertyPtr pProp )
{
	if( !__super::OnApplyProperty( pProp ) )
		return false;
	bool bFailed = false;
	switch( pProp->GetID() )
	{
	case Prop::List:
		if( !GetComboHandler() )
		{
			const PropVal::TCStringArray& rString = *pProp->GetStringArrayPtr();
			ResetContent();
			for( int idx = 0; (size_t)idx < rString.size(); ++idx )
				AddString( rString.at( idx ) );
			if( IsEnumeratingProperties() )
			{
				const PropVal::TIntArray& rInt = *mpTemplate->GetPropertyObject( Prop::ItemData )->GetIntArrayPtr();
				for( int idx = 0; (size_t)idx < rInt.size(); ++idx )
					SetItemData( idx, (DWORD_PTR)rInt.at( idx ) );
			}
		}
		break;
	case Prop::ItemData:
		if( !IsEnumeratingProperties() || GetComboHandler() )
		{
			const PropVal::TIntArray& rInt = *pProp->GetIntArrayPtr();
			for( int idx = 0; (size_t)idx < rInt.size(); ++idx )
				SetItemData( idx, (DWORD_PTR)rInt.at( idx ) );
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

DWORD CComboBoxCtrl::GetComboStyle() const
{
	switch( mpTemplate->GetLongProperty( Prop::ComboBoxStyle ) )
	{
	case CmboStyle_Combo: return CBS_DROPDOWN;
	case CmboStyle_Simple: return CBS_SIMPLE;
	case CmboStyle_DropDown: return CBS_DROPDOWNLIST;
	case CmboStyle_FontSimpleList: return CBS_SIMPLE;
	}
	return CBS_DROPDOWNLIST;
}

BEGIN_MESSAGE_MAP(CComboBoxCtrl, CFilteredComboCtrl)
	ON_WM_MEASUREITEM_REFLECT()
	ON_CONTROL_REFLECT(CBN_DROPDOWN, &CComboBoxCtrl::OnDropdown)
	ON_CONTROL_REFLECT(CBN_CLOSEUP, &CComboBoxCtrl::OnCloseUp)
	ON_MESSAGE(CB_RESETCONTENT, &CComboBoxCtrl::OnResetContent)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CComboBoxCtrl message handlers

BOOL CComboBoxCtrl::PreTranslateMessage(MSG* pMsg) 
{
	GetToolTipCtrl().RelayEvent(pMsg);
	if( pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN )
	{
		if( mpTemplate->GetBooleanProperty( Prop::ReturnAsTab ) )
			pMsg->wParam = VK_TAB;		
	}
	return __super::PreTranslateMessage(pMsg);
}

void CComboBoxCtrl::PostNcDestroy() 
{
	__super::PostNcDestroy();
	delete this;
}

void CComboBoxCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CComboHandler* pHandler = GetComboHandler();
	assert( pHandler != NULL ); //should never call this function if there is no handler
	if( pHandler )
		pHandler->DrawItem( this, lpDrawItemStruct );
}

void CComboBoxCtrl::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	CComboHandler* pHandler = GetComboHandler();
	assert( pHandler != NULL ); //should never call this function if there is no handler
	if( !pHandler )
		return;
	UINT nItemHeight = pHandler->GetItemHeight();
	if( nItemHeight > 0 )
		lpMeasureItemStruct->itemHeight = nItemHeight;
}

void CComboBoxCtrl::OnDropdown()
{
}

void CComboBoxCtrl::OnCloseUp()
{
	CComboHandler* pHandler = GetComboHandler();
	if( pHandler )
		pHandler->OnDropdownClose( this );
}

LRESULT CComboBoxCtrl::OnResetContent( WPARAM wParam, LPARAM lParam )
{
	Default();
	CComboHandler* pHandler = GetComboHandler();
	if( pHandler )
		pHandler->PopulateList( this );
	return (LRESULT)TRUE;
}
