#include "stdafx.h"
#include "ArxLineweightComboBoxCtrl.h"
#include "DclControlTemplate.h"
#include "ControlPane.h"
#include "InvokeMethod.h"


/////////////////////////////////////////////////////////////////////////////
// CArxLineweightComboBoxCtrl

CArxLineweightComboBoxCtrl::CArxLineweightComboBoxCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CDialogControl( pTemplate, pPane, this )
, CAcUiLineWeightComboBox()
, mArxServices( this )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CArxLineweightComboBoxCtrl::~CArxLineweightComboBoxCtrl()
{
}

bool CArxLineweightComboBoxCtrl::Create( CWnd* pParentWnd, UINT nID )
{
	SetUseDefault( TRUE );

	bool bSuccess = (__super::Create( GetWndStyle(), GetWndRect(), pParentWnd, nID ) != FALSE);

	if( bSuccess && !ApplyPropertiesEnum() )
		bSuccess = false;

	return bSuccess;
}

CRect CArxLineweightComboBoxCtrl::GetWndRect() const
{
	CRect rectCombo = CDialogControl::GetWndRect();
	long nListHeight = mpTemplate->GetLongProperty( Prop::DropDownHeight );
	if( nListHeight < 40 )
		nListHeight = 40;
	rectCombo.bottom += FromDIP( nListHeight );;
	return rectCombo;
}

DWORD CArxLineweightComboBoxCtrl::GetWndStyle() const
{
	DWORD dwStyle = CDialogControl::GetWndStyle();
	dwStyle |= (WS_VSCROLL | CBS_HASSTRINGS | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | CBS_NOINTEGRALHEIGHT);
	//if( mpTemplate->GetBooleanProperty( Prop::Sorted ) )
	//	dwStyle |= CBS_SORT;
	//else
	//	dwStyle &= ~(DWORD)CBS_SORT;
	return dwStyle;
}

bool CArxLineweightComboBoxCtrl::ApplyProperty( TPropertyPtr pProp )
{
	if( !__super::ApplyProperty( pProp ) )
		return false;
	bool bFailed = false;
	switch( pProp->GetID() )
	{
	case Prop::List:
		break;
	case Prop::ItemData:
		break;
	case Prop::TextLimit:
		LimitText( pProp->GetLongValue() );
		break;
	case Prop::Sorted:
		//if( pProp->GetBooleanValue() )
		//	ModifyStyle( 0, CBS_SORT, SWP_FRAMECHANGED );
		//else
		//	ModifyStyle( CBS_SORT, 0, SWP_FRAMECHANGED );
		break;
	}
	return !bFailed;
}

bool CArxLineweightComboBoxCtrl::OnApplyUseVisualStyle( TPropertyPtr pProp )
{
#if (_ARXTARGET >= 17)
	static bool bVistaOrLater = (LOBYTE(LOWORD(GetVersion())) >= 6);
	if( bVistaOrLater )
	{
	#if (_ARXTARGET == 17)
		bool bThemeEnabled = GetTheme().IsThemeActive();
	#else
		bool bThemeEnabled = IsVistaTheme();
	#endif
		if( bThemeEnabled )
		{
			if( !pProp->GetBooleanValue() )
				return false; //AcUi combos don't display correctly when theme is active but disabled for the control
		}
	}
#endif
	return __super::OnApplyUseVisualStyle( pProp );
}

void CArxLineweightComboBoxCtrl::OnListChanged()
{
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

BEGIN_MESSAGE_MAP(CArxLineweightComboBoxCtrl, CAcUiLineWeightComboBox)
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_MOUSEMOVE()
	ON_CONTROL_REFLECT(CBN_SELCHANGE, &CArxLineweightComboBoxCtrl::OnCbnSelchange)
	ON_CONTROL_REFLECT(CBN_DROPDOWN, &CArxLineweightComboBoxCtrl::OnCbnDropdown)
	ON_MESSAGE(CB_ADDSTRING, &CArxLineweightComboBoxCtrl::OnModifyContent)
	ON_MESSAGE(CB_DELETESTRING, &CArxLineweightComboBoxCtrl::OnModifyContent)
	ON_MESSAGE(CB_DIR, &CArxLineweightComboBoxCtrl::OnModifyContent)
	ON_MESSAGE(CB_INSERTSTRING, &CArxLineweightComboBoxCtrl::OnModifyContent)
	ON_MESSAGE(CB_SETITEMDATA, &CArxLineweightComboBoxCtrl::OnModifyContent)
	ON_MESSAGE(CB_RESETCONTENT, &CArxLineweightComboBoxCtrl::OnResetContent)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CArxLineweightComboBoxCtrl message handlers

LRESULT CArxLineweightComboBoxCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
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

BOOL CArxLineweightComboBoxCtrl::PreTranslateMessage(MSG* pMsg)
{
	GetToolTipCtrl().RelayEvent(pMsg);
	return __super::PreTranslateMessage(pMsg);
}

void CArxLineweightComboBoxCtrl::PostNcDestroy()
{
	__super::PostNcDestroy();
	delete this;
}

LRESULT CArxLineweightComboBoxCtrl::OnModifyContent( WPARAM wParam, LPARAM lParam )
{
	LRESULT lResult = Default();
	OnListChanged();
	return lResult;
}

LRESULT CArxLineweightComboBoxCtrl::OnResetContent( WPARAM wParam, LPARAM lParam )
{
	CString sSelection;
	GetWindowText( sSelection );
	Default();
	OnListChanged();
	return (LRESULT)TRUE;
}

void CArxLineweightComboBoxCtrl::OnSetFocus(CWnd* pOldWnd)
{
	__super::OnSetFocus( pOldWnd );
#if (_ARXTARGET == 17)
	if( GetTheme().IsThemeActive() )
		Invalidate();
#endif
	GetArxServices()->HandleEvent( Prop::EventSetFocus );
}

void CArxLineweightComboBoxCtrl::OnKillFocus(CWnd* pNewWnd)
{
	__super::OnKillFocus( pNewWnd );
	GetArxServices()->HandleEvent( Prop::EventKillFocus );
}

void CArxLineweightComboBoxCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	GetArxServices()->HandleEvent( Prop::EventMouseMove, args_NNN( nFlags, ToDIP( point.x ), ToDIP( point.y ) ) );
	__super::OnMouseMove(nFlags, point);
}

void CArxLineweightComboBoxCtrl::OnCbnSelchange()
{
	int nCurSel = GetCurSel();
	CString sText;
	GetLBText( nCurSel, sText );
	GetArxServices()->HandleEvent( Prop::EventSelChanged, args_NS( nCurSel, sText ) );
	mpTemplate->SetStringProperty( Prop::Text, sText );
}

void CArxLineweightComboBoxCtrl::OnCbnDropdown()
{
	__super::OnDropDown();
	GetArxServices()->HandleEvent( Prop::EventDropDown );
}
