#include "stdafx.h"
#include "ImageComboBoxCtrl.h"
#include "DclControlObject.h"
#include "ControlPane.h"
#include "ComboStyles.h"
#include "ComboHandler.h"


/////////////////////////////////////////////////////////////////////////////
// CImageComboBoxCtrl

CImageComboBoxCtrl::CImageComboBoxCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID,
																				CComboHandler* pHandler /*= NULL*/, bool bCreate /*= true*/ )
: CDialogControl( pTemplate, pPane, this )
, CFilteredComboExCtrl( pHandler? pHandler->GetInputFilter() : NULL )
, mpHandler( pHandler )
, mbIgnoreChange( false )
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
	if( bSuccess )
	{
	#ifdef _UNICODE
		BOOL bUnicode = TRUE;
	#else
		BOOL bUnicode = FALSE;
	#endif
		SendMessage( CCM_SETUNICODEFORMAT, (WPARAM)bUnicode, 0 );
	}

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
	const CComboHandler* pHandler = GetComboHandler();
	if( pHandler )
	{
		if( pHandler->IsOwnerDrawn() )
			dwStyle |= (pHandler->GetItemHeight() > 0? CBS_OWNERDRAWFIXED : CBS_OWNERDRAWVARIABLE);
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
			TImageListPtr pImageList = mpTemplate->GetImageList();
			if (pImageList && pImageList->GetImageList().GetSafeHandle())
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
			mbIgnoreChange = true;
			ResetContent();
			const PropVal::TCStringArray* prString = pProp->GetConstStringArrayPtr();
			TPropertyPtr pImagesProp = mpTemplate->GetPropertyObject( Prop::ListImages );
			const PropVal::TIntArray* prImage = pImagesProp? pImagesProp->GetConstIntArrayPtr() : NULL;
			if( prString )
			{
				for( int idx = 0; (size_t)idx < prString->size(); ++idx )
				{
					int nImage = -1;
					if( prImage && (size_t)idx < prImage->size() )
						nImage = prImage->at( idx );
					CString sItemLabel = prString->at( idx );
					COMBOBOXEXITEM cbi =
					{
						(CBEIF_TEXT | CBEIF_IMAGE | CBEIF_SELECTEDIMAGE | CBEIF_INDENT),
						idx,
						sItemLabel.LockBuffer(),
						-1,
						nImage,
						nImage,
						-1,
						0
					};
					int idxNewItem = InsertItem( &cbi );
					if( idxNewItem < 0 )
						continue;
					if( IsEnumeratingProperties() )
					{
						const PropVal::TIntArray* prInt = mpTemplate->GetPropertyObject( Prop::ItemData )->GetConstIntArrayPtr();
						if( prInt )
							SetItemData( idxNewItem, (DWORD_PTR)prInt->at( idx ) );
					}
				}
			}
			mbIgnoreChange = false;
		}
		break;
	case Prop::ItemData:
		if( !IsEnumeratingProperties() && !GetComboHandler() )
		{
			const PropVal::TIntArray* prInt = pProp->GetConstIntArrayPtr();
			if( prInt )
			{
				CComboBox* pComboCtrl = GetComboBoxCtrl();
				for( int idx = 0; (size_t)idx < prInt->size(); ++idx )
					pComboCtrl->SetItemData( idx, (DWORD_PTR)prInt->at( idx ) );
			}
		}
		break;
	case Prop::Text:
		SetWindowText( pProp->GetStringValue() );
		break;
	case Prop::TextLimit:
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
	case ComboStyle::Combo: return CBS_DROPDOWN;
	case ComboStyle::Simple: return CBS_SIMPLE;
	case ComboStyle::DropDown: return CBS_DROPDOWNLIST;
	}
	return CBS_DROPDOWNLIST;
}

int CImageComboBoxCtrl::FindString(int nIndexStart, LPCTSTR lpszFind) const
{
	if( !lpszFind || !*lpszFind )
		return -1;
	int ctItems = GetCount();
	if( nIndexStart < -1 )
		nIndexStart = -1;
	++nIndexStart;
	int cchSearch = lstrlen( lpszFind );
	for( int idx = nIndexStart; idx < ctItems; ++idx )
	{
		CString sItem;
		GetLBText( idx, sItem );
		if( sItem.Left( cchSearch ).CompareNoCase( lpszFind ) == 0 )
			return idx;
	}
	for( int idx = 0; idx < nIndexStart; ++idx )
	{
		CString sItem;
		GetLBText( idx, sItem );
		if( sItem.Left( cchSearch ).CompareNoCase( lpszFind ) == 0 )
			return idx;
	}
	return -1;
}

int CImageComboBoxCtrl::FindStringExact(int nIndexStart, LPCTSTR lpszFind) const
{
	if( !lpszFind || !*lpszFind )
		return -1;
	int ctItems = GetCount();
	if( nIndexStart < -1 )
		nIndexStart = -1;
	++nIndexStart;
	for( int idx = nIndexStart; idx < ctItems; ++idx )
	{
		CString sItem;
		GetLBText( idx, sItem );
		if( sItem.CompareNoCase( lpszFind ) == 0 )
			return idx;
	}
	for( int idx = 0; idx < nIndexStart; ++idx )
	{
		CString sItem;
		GetLBText( idx, sItem );
		if( sItem.CompareNoCase( lpszFind ) == 0 )
			return idx;
	}
	return -1;
}

void CImageComboBoxCtrl::OnListChanged()
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


BEGIN_MESSAGE_MAP(CImageComboBoxCtrl, CFilteredComboExCtrl)
	ON_WM_MEASUREITEM_REFLECT()
	ON_CONTROL_REFLECT(CBN_DROPDOWN, &CImageComboBoxCtrl::OnDropdown)
	ON_CONTROL_REFLECT(CBN_CLOSEUP, &CImageComboBoxCtrl::OnCloseUp)
	ON_MESSAGE(CB_ADDSTRING, &CImageComboBoxCtrl::OnModifyContent)
	ON_MESSAGE(CB_DELETESTRING, &CImageComboBoxCtrl::OnModifyContent)
	ON_MESSAGE(CB_DIR, &CImageComboBoxCtrl::OnModifyContent)
	ON_MESSAGE(CB_INSERTSTRING, &CImageComboBoxCtrl::OnModifyContent)
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

LRESULT CImageComboBoxCtrl::OnModifyContent( WPARAM wParam, LPARAM lParam )
{
	LRESULT lResult = Default();
	OnListChanged();
	return lResult;
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
	OnListChanged();
	return (LRESULT)TRUE;
}
