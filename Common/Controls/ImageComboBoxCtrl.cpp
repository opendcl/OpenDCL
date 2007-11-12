#include "stdafx.h"
#include "ImageComboBoxCtrl.h"
#include "DclControlObject.h"
#include "ControlPane.h"
#include "ComboHandler.h"


/////////////////////////////////////////////////////////////////////////////
// CImageComboBoxCtrl

CImageComboBoxCtrl::CImageComboBoxCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID,
																				CComboHandler* pHandler /*= NULL*/, bool bCreate /*= true*/ )
: CDialogControl( pTemplate, pPane, this )
, CFilteredComboExCtrl( pHandler? pHandler->GetInputFilter() : NULL )
, mpHandler( pHandler )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CImageComboBoxCtrl::~CImageComboBoxCtrl()
{
}

bool CImageComboBoxCtrl::Create( CWnd* pParentWnd, UINT nID )
{
	bool bSuccess = __super::Create( pParentWnd, GetWndRect(), GetWndStyle(), nID );

	if( bSuccess && GetComboHandler() )
		ResetContent();  //populate list

	if( bSuccess && !ApplyPropertiesEnum() )
		bSuccess = false;

	return bSuccess;
}

CRect CImageComboBoxCtrl::GetWndRect() const
{
	CRect rectCombo = CDialogControl::GetWndRect();
	if( (GetComboStyle() & CBS_DROPDOWN) != 0 )
	{
		long nListHeight = mpTemplate->GetLongProperty( Prop::DropDownHeight );
		if( nListHeight < 40 )
			nListHeight = 40;
		rectCombo.bottom += nListHeight;
	}
	return rectCombo;
}

DWORD CImageComboBoxCtrl::GetWndStyle() const
{
	DWORD dwStyle = CDialogControl::GetWndStyle();
	const CComboHandler* pHandler = const_cast< CImageComboBoxCtrl* >(this)->GetComboHandler();
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

bool CImageComboBoxCtrl::OnApplyProperty( TPropertyPtr pProp )
{
	if( !__super::OnApplyProperty( pProp ) )
		return false;
	bool bFailed = false;
	switch( pProp->GetID() )
	{
	case Prop::ImageList:
		{
			RefCountedPtr< CImageListObject > pImageList = mpTemplate->GetImageList();
			if (pImageList)
			{
				CImageList& ImageList = pImageList->GetImageList();
				CAcadColorService* pColorService = GetColorService();
				ImageList.SetBkColor( pColorService? pColorService->GetBackgroundColor() : CLR_NONE );
				SetImageList( &ImageList );
			}
			else
				SetImageList( NULL );
		}
		break;
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

bool CImageComboBoxCtrl::OnApplyUseVisualStyle( TPropertyPtr pProp )
{
	if( !__super::OnApplyUseVisualStyle( pProp ) )
		return false;
	CThemeHelperST* pThemeHelper = mpControlPane->GetThemeHelper();
	if( !pThemeHelper || !pThemeHelper->IsThemeActive() )
		return false; //visual styles not supported
	CComboBox* pComboCtrl = GetComboBoxCtrl();
	if( !pComboCtrl )
		return false;
	HWND hwnd = pComboCtrl->m_hWnd;
	if( pProp->GetBooleanValue() )
		pThemeHelper->SetWindowTheme( hwnd, NULL, NULL );
	else
		pThemeHelper->SetWindowTheme( hwnd, L"", L"" );
	return true;
}

DWORD CImageComboBoxCtrl::GetComboStyle() const
{
	switch( mpTemplate->GetLongProperty( Prop::ComboBoxStyle ) )
	{
	case CmboStyle_Combo: return CBS_DROPDOWN;
	case CmboStyle_Simple: return CBS_SIMPLE;
	case CmboStyle_DropDown: return CBS_DROPDOWNLIST;
	}
	return CBS_DROPDOWNLIST;
}

BEGIN_MESSAGE_MAP(CImageComboBoxCtrl, CFilteredComboExCtrl)
	ON_WM_MEASUREITEM_REFLECT()
	ON_CONTROL_REFLECT(CBN_DROPDOWN, &CImageComboBoxCtrl::OnDropdown)
	ON_CONTROL_REFLECT(CBN_CLOSEUP, &CImageComboBoxCtrl::OnCloseUp)
	ON_MESSAGE(CB_RESETCONTENT, &CImageComboBoxCtrl::OnResetContent)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CImageComboBoxCtrl message handlers

BOOL CImageComboBoxCtrl::PreTranslateMessage(MSG* pMsg) 
{
	GetToolTipCtrl().RelayEvent(pMsg);
	return __super::PreTranslateMessage(pMsg);
}

void CImageComboBoxCtrl::PostNcDestroy() 
{
	__super::PostNcDestroy();
	delete this;
}

void CImageComboBoxCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CComboHandler* pHandler = GetComboHandler();
	if( pHandler )
		pHandler->DrawItem( this, lpDrawItemStruct );
}

void CImageComboBoxCtrl::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	CComboHandler* pHandler = GetComboHandler();
	assert( pHandler != NULL ); //should never call this function if there is no handler
	if( !pHandler )
		return;
	UINT nItemHeight = pHandler->GetItemHeight();
	if( nItemHeight > 0 )
		lpMeasureItemStruct->itemHeight = nItemHeight;
}

void CImageComboBoxCtrl::OnDropdown()
{
}

void CImageComboBoxCtrl::OnCloseUp()
{
	CComboHandler* pHandler = GetComboHandler();
	if( pHandler )
		pHandler->OnDropdownClose( this );
}

LRESULT CImageComboBoxCtrl::OnResetContent( WPARAM wParam, LPARAM lParam )
{
	CString sSelection;
	GetWindowText( sSelection );
	Default();
	CComboHandler* pHandler = GetComboHandler();
	if( pHandler )
	{
		pHandler->PopulateList( this );
		if( !sSelection.IsEmpty() )
		{
			int idx = FindStringExact( -1, sSelection );
			if( idx >= 0 )
				SetCurSel( idx );
		}
	}
	return (LRESULT)TRUE;
}
