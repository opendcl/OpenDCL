#include "stdafx.h"
#include "ComboBoxCtrl.h"
#include "DclControlTemplate.h"
#include "ControlPane.h"
#include "ComboHandler.h"
#include "ComboStyles.h"


/////////////////////////////////////////////////////////////////////////////
// CComboBoxCtrl

CComboBoxCtrl::CComboBoxCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID,
															CComboHandler* pHandler /*= NULL*/, bool bCreate /*= true*/ )
: CDialogControl( pTemplate, pPane, this )
, CFilteredComboCtrl( pHandler? pHandler->GetInputFilter() : NULL )
, mpHandler( pHandler )
, mbIgnoreChange( false )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CComboBoxCtrl::~CComboBoxCtrl()
{
	delete mpHandler;
}

bool CComboBoxCtrl::Create( CWnd* pParentWnd, UINT nID )
{
	bool bSuccess = __super::Create( pParentWnd, GetWndRect(), GetWndStyle(), nID );

	if( bSuccess )
		SetExtendedUI();

	if( bSuccess && !ApplyPropertiesEnum() )
		bSuccess = false;

	SetEditSel( -1, -1 );

	return bSuccess;
}

CRect CComboBoxCtrl::GetWndRect() const
{
	CRect rectCombo = CDialogControl::GetWndRect();
	if( (GetComboStyle() & CBS_DROPDOWN) != 0 )
	{
		long nListHeight = mpTemplate->GetLongProperty( Prop::DropDownHeight );
		rectCombo.bottom += FromDIP( nListHeight );
	}
	return rectCombo;
}

DWORD CComboBoxCtrl::GetWndStyle() const
{
	DWORD dwStyle = CDialogControl::GetWndStyle();
	dwStyle |= (CBS_NOINTEGRALHEIGHT | CBS_HASSTRINGS);
	const CComboHandler* pHandler = GetComboHandler();
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

void CComboBoxCtrl::ApplyPropertiesOrder( std::vector< Prop::Id >& ridFirst, std::vector< Prop::Id >& ridLast )
{
	__super::ApplyPropertiesOrder( ridFirst, ridLast );
	ridFirst.push_back( Prop::List );
}

bool CComboBoxCtrl::ApplyProperty( TPropertyPtr pProp )
{
	if( !__super::ApplyProperty( pProp ) )
		return false;
	bool bFailed = false;
	switch( pProp->GetID() )
	{
	case Prop::Sorted:
		{
			DWORD dwOldStyle = GetStyle();
			if( pProp->GetBooleanValue() )
				ModifyStyle( 0, CBS_SORT, 0 );
			else
				ModifyStyle( CBS_SORT, 0, 0 );
			if( dwOldStyle != GetStyle() )
				OnListChanged();
			OnNeedRepaint();
		}
		break;
	//case Prop::ComboBoxStyle:
	//	{
	//		DWORD dwOldComboStyle = (GetStyle() & CBS_DROPDOWNLIST);
	//		if( dwOldComboStyle != GetComboStyle() )
	//		{
	//			CWnd* pParent = GetParent();
	//			UINT nID = GetControlId();
	//			CRect rcOld;
	//			GetClientRect( &rcOld );
	//			CWnd* pPrevWnd = GetNextWindow(GW_HWNDPREV);
	//			HWND hwndOld = UnsubclassWindow();
	//			Create( pParent, nID );
	//			SetWindowPos( pPrevWnd, rcOld.left, rcOld.top, 0, 0, SWP_NOSIZE | SWP_NOSENDCHANGING );
	//			if( (dwOldComboStyle & CBS_DROPDOWN) != 0 )
	//			{
	//				mpTemplate->SetLongProperty( Prop::Height, ToDIP( rcOld.Height() ) + mpTemplate->GetLongProperty( Prop::DropDownHeight ) );
	//				ApplyPosition();
	//			}
	//			::DestroyWindow( hwndOld );
	//			OnFrameChanged();
	//		}
	//	}
	//	break;
	case Prop::List:
		{
			ResetContent();
		}
		break;
	case Prop::ItemData:
		if( !IsEnumeratingProperties() && !GetComboHandler() )
		{
			mbIgnoreChange = true;
			const PropVal::TIntArray* prInt = pProp->GetConstIntArrayPtr();
			if( prInt )
			{
				for( int idx = 0; (size_t)idx < prInt->size(); ++idx )
					SetItemData( idx, (DWORD_PTR)prInt->at( idx ) );
			}
			mbIgnoreChange = false;
		}
		break;
	case Prop::Text:
		{
			CString sText = pProp->GetStringValue();
			SetWindowText( sText );
			if( !sText.IsEmpty() )
			{
				int idx = FindStringExact( -1, sText );
				if( idx >= 0 )
					SetCurSel( idx );
			}
		}
		break;
	case Prop::TextLimit:
		LimitText( pProp->GetLongValue() );
		break;
	}
	return !bFailed;
}

DWORD CComboBoxCtrl::GetComboStyle() const
{
	switch( mpTemplate->GetLongProperty( Prop::ComboBoxStyle ) )
	{
	case ComboStyle::Combo: return CBS_DROPDOWN;
	case ComboStyle::Simple: return CBS_SIMPLE;
	case ComboStyle::DropDown: return CBS_DROPDOWNLIST;
	case ComboStyle::FontSimpleList: return CBS_SIMPLE;
	}
	return CBS_DROPDOWNLIST;
}

void CComboBoxCtrl::OnListChanged()
{
	if( mbIgnoreChange )
		return;
	TPropertyPtr pItemList = mpTemplate->GetPropertyObject( Prop::List );
	TPropertyPtr pItemDataList = mpTemplate->GetPropertyObject( Prop::ItemData );
	if( !pItemList && !pItemDataList )
		return;
	if( pItemList )
		pItemList->clear();
	if( pItemDataList )
		pItemDataList->clear();
	int ctItems = GetCount();
	for( int idx = 0; idx < ctItems; ++idx )
	{
		if( pItemList )
		{
			CString sItem;
			GetLBText( idx, sItem );
			pItemList->AddStringItem( sItem );
		}
		if( pItemDataList )
			pItemDataList->GetIntArrayPtr()->push_back( GetItemData( idx ) );
	}
}


BEGIN_MESSAGE_MAP(CComboBoxCtrl, CFilteredComboCtrl)
	ON_WM_MEASUREITEM_REFLECT()
	ON_CONTROL_REFLECT(CBN_DROPDOWN, &CComboBoxCtrl::OnCbnDropdown)
	ON_CONTROL_REFLECT(CBN_CLOSEUP, &CComboBoxCtrl::OnCbnCloseup)
	ON_MESSAGE(CB_ADDSTRING, &CComboBoxCtrl::OnModifyContent)
	ON_MESSAGE(CB_DELETESTRING, &CComboBoxCtrl::OnModifyContent)
	ON_MESSAGE(CB_DIR, &CComboBoxCtrl::OnModifyContent)
	ON_MESSAGE(CB_INSERTSTRING, &CComboBoxCtrl::OnModifyContent)
	ON_MESSAGE(CB_SETITEMDATA, &CComboBoxCtrl::OnModifyContent)
	ON_MESSAGE(CB_RESETCONTENT, &CComboBoxCtrl::OnResetContent)
	ON_WM_ERASEBKGND()
	ON_MESSAGE(WM_DPICHANGED_AFTERPARENT, &CComboBoxCtrl::OnDpiChanged)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CComboBoxCtrl message handlers

LRESULT CComboBoxCtrl::OnDpiChanged(WPARAM wParam, LPARAM lParam)
{
	HandleDpiChanged();
	return 0;
}

LRESULT CComboBoxCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = __super::WindowProc(message, wParam, lParam);
	switch( message )
	{
		case CB_SELECTSTRING:
		case CB_SETCURSEL:
			{
				CString sText;
				int nCurSel = GetCurSel();
				if( nCurSel >= 0 )
					GetLBText( nCurSel, sText );
				if( sText != mpTemplate->GetStringProperty( Prop::Text ) )
					mpTemplate->SetStringProperty( Prop::Text, sText );
			}
			break;
	}
	return lResult;
}

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
		lpMeasureItemStruct->itemHeight = FromDIP(nItemHeight);
}

void CComboBoxCtrl::OnCbnDropdown()
{
}

void CComboBoxCtrl::OnCbnCloseup()
{
	CComboHandler* pHandler = GetComboHandler();
	if( pHandler )
		pHandler->OnDropdownClose( this );
}

LRESULT CComboBoxCtrl::OnModifyContent( WPARAM wParam, LPARAM lParam )
{
	LRESULT lResult = Default();
	OnListChanged();
	return lResult;
}

LRESULT CComboBoxCtrl::OnResetContent( WPARAM wParam, LPARAM lParam )
{
	CString sSelection;
	GetWindowText( sSelection );
	Default();

	CComboHandler* pHandler = GetComboHandler();
	if( pHandler )
	{
		mbIgnoreChange = true;
		pHandler->PopulateList(this);
		mbIgnoreChange = false;
		OnListChanged();
	}
	else
	{
		TPropertyPtr pProp = mpTemplate->GetPropertyObject( Prop::List );
		if( pProp )
		{
			const PropVal::TCStringArray* prString =  pProp->GetConstStringArrayPtr();
			const PropVal::TIntArray* prInt = mpTemplate->GetPropertyObject( Prop::ItemData )->GetConstIntArrayPtr();
			if( prString )
			{
				mbIgnoreChange = true;
				for( size_t idx = 0; idx < prString->size(); ++idx )
				{
					int idxNewItem = AddString( prString->at( idx ) );
					if( idxNewItem < 0 )
						continue;
					if( prInt && idx < prInt->size() )
						SetItemData( idxNewItem, (DWORD_PTR)prInt->at( idx ) );
				}
				mbIgnoreChange = false;
			}
			if( (GetStyle() & CBS_SORT) )
				OnListChanged(); //in case the list is sorted, to update the List property
		}
	}
	if( !sSelection.IsEmpty() )
	{
		int idx = FindStringExact( -1, sSelection );
		if( idx >= 0 )
			SetCurSel( idx );
	}
	return (LRESULT)TRUE;
}

BOOL CComboBoxCtrl::OnEraseBkgnd(CDC* pDC)
{
	if( HandleEraseBkgnd( pDC ) )
		return TRUE;
	return __super::OnEraseBkgnd(pDC);
}
