#include "stdafx.h"
#include "ImageComboBoxCtrl.h"
#include "DclControlTemplate.h"
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
		rectCombo.bottom += FromDIP( nListHeight );;
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

bool CImageComboBoxCtrl::ApplyProperty( TPropertyPtr pProp )
{
	if( !__super::ApplyProperty( pProp ) )
		return false;
	bool bFailed = false;
	switch( pProp->GetID() )
	{
	case Prop::ComboBoxStyle:
		{
			DWORD dwOldComboStyle = (GetStyle() & CBS_DROPDOWNLIST);
			if( dwOldComboStyle != GetComboStyle() )
			{
				CWnd* pParent = GetParent();
				UINT nID = GetControlId();
				CRect rcOld;
				GetClientRect( &rcOld );
				CWnd* pPrevWnd = GetNextWindow(GW_HWNDPREV);
				HWND hwndOld = UnsubclassWindow();
				Create( pParent, nID );
				SetWindowPos( pPrevWnd, rcOld.left, rcOld.top, 0, 0, SWP_NOSIZE | SWP_NOSENDCHANGING );
				if( (dwOldComboStyle & CBS_DROPDOWN) != 0 )
				{
					mpTemplate->SetLongProperty( Prop::Height, ToDIP( rcOld.Height() ) + mpTemplate->GetLongProperty( Prop::DropDownHeight ) );
					ApplyPosition();
				}
				::DestroyWindow( hwndOld );
				OnFrameChanged();
			}
		}
		break;
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
			const PropVal::TIntArray* prInt = mpTemplate->GetPropertyObject( Prop::ItemData )->GetConstIntArrayPtr();
			if( prString )
			{
				for( size_t idx = 0; idx < prString->size(); ++idx )
				{
					int nImage = -1;
					if( prImage && idx < prImage->size() )
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
					if( prInt && idx < prInt->size() )
						SetItemData( idxNewItem, (DWORD_PTR)prInt->at( idx ) );
				}
			}
			mbIgnoreChange = false;
		}
		break;
	case Prop::ItemData:
		if( !IsEnumeratingProperties() && !GetComboHandler() )
		{
			mbIgnoreChange = true;
			const PropVal::TIntArray* prInt = pProp->GetConstIntArrayPtr();
			if( prInt )
			{
				CComboBox* pComboCtrl = GetComboBoxCtrl();
				for( int idx = 0; (size_t)idx < prInt->size(); ++idx )
					pComboCtrl->SetItemData( idx, (DWORD_PTR)prInt->at( idx ) );
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

bool CImageComboBoxCtrl::OnApplyUseVisualStyle( TPropertyPtr pProp )
{
	if( !__super::OnApplyUseVisualStyle( pProp ) )
		return false;
	CComboBox* pComboCtrl = GetComboBoxCtrl();
	if( !pComboCtrl )
		return false;
	if( pProp->GetBooleanValue() )
		GetTheme().SetWindowTheme( NULL, NULL );
	else
		GetTheme().SetWindowTheme( L"", L"" );
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
	ON_CONTROL_REFLECT(CBN_DROPDOWN, &CImageComboBoxCtrl::OnCbnDropdown)
	ON_CONTROL_REFLECT(CBN_CLOSEUP, &CImageComboBoxCtrl::OnCbnCloseup)
	ON_MESSAGE(CB_ADDSTRING, &CImageComboBoxCtrl::OnModifyContent)
	ON_MESSAGE(CB_DELETESTRING, &CImageComboBoxCtrl::OnModifyContent)
	ON_MESSAGE(CB_DIR, &CImageComboBoxCtrl::OnModifyContent)
	ON_MESSAGE(CB_INSERTSTRING, &CImageComboBoxCtrl::OnModifyContent)
	ON_MESSAGE(CB_SETITEMDATA, &CImageComboBoxCtrl::OnModifyContent)
	ON_MESSAGE(CB_RESETCONTENT, &CImageComboBoxCtrl::OnResetContent)
	ON_WM_ERASEBKGND()
	ON_MESSAGE(WM_DPICHANGED_AFTERPARENT, &CImageComboBoxCtrl::OnDpiChanged)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CImageComboBoxCtrl message handlers

LRESULT CImageComboBoxCtrl::OnDpiChanged(WPARAM wParam, LPARAM lParam)
{
	HandleDpiChanged();
	return 0;
}

LRESULT CImageComboBoxCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
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

BOOL CImageComboBoxCtrl::OnEraseBkgnd(CDC* pDC)
{
	if( HandleEraseBkgnd( pDC ) )
		return TRUE;
	return __super::OnEraseBkgnd(pDC);
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
		lpMeasureItemStruct->itemHeight = FromDIP(nItemHeight);
}

void CImageComboBoxCtrl::OnCbnDropdown()
{
}

void CImageComboBoxCtrl::OnCbnCloseup()
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
		pHandler->PopulateList( this );
	if( !sSelection.IsEmpty() )
	{
		int idx = FindStringExact( -1, sSelection );
		if( idx >= 0 )
			SetCurSel( idx );
	}
	OnListChanged();
	return (LRESULT)TRUE;
}
