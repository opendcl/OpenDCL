#include "stdafx.h"
#include "ArxPlotStyleNameComboBoxCtrl.h"
#include "DclControlTemplate.h"
#include "ControlPane.h"
#include "InvokeMethod.h"


/////////////////////////////////////////////////////////////////////////////
// CArxPlotStyleNameComboBoxCtrl

CArxPlotStyleNameComboBoxCtrl::CArxPlotStyleNameComboBoxCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CDialogControl( pTemplate, pPane, this )
, CAcUiPlotStyleNamesComboBox()
, mArxServices( this )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CArxPlotStyleNameComboBoxCtrl::~CArxPlotStyleNameComboBoxCtrl()
{
}

bool CArxPlotStyleNameComboBoxCtrl::Create( CWnd* pParentWnd, UINT nID )
{
	bool bSuccess = (__super::Create( GetWndStyle(), GetWndRect(), pParentWnd, nID ) != FALSE);

	if( bSuccess && !ApplyPropertiesEnum() )
		bSuccess = false;

	return bSuccess;
}

CRect CArxPlotStyleNameComboBoxCtrl::GetWndRect() const
{
	CRect rectCombo = CDialogControl::GetWndRect();
	long nListHeight = mpTemplate->GetLongProperty( Prop::DropDownHeight );
	if( nListHeight < 40 )
		nListHeight = 40;
	rectCombo.bottom += nListHeight;
	return rectCombo;
}

DWORD CArxPlotStyleNameComboBoxCtrl::GetWndStyle() const
{
	DWORD dwStyle = CDialogControl::GetWndStyle();
	dwStyle |= (WS_VSCROLL | CBS_HASSTRINGS | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | CBS_NOINTEGRALHEIGHT);
	//if( mpTemplate->GetBooleanProperty( Prop::Sorted ) )
	//	dwStyle |= CBS_SORT;
	//else
	//	dwStyle &= ~(DWORD)CBS_SORT;
	return dwStyle;
}

bool CArxPlotStyleNameComboBoxCtrl::ApplyProperty( TPropertyPtr pProp )
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

bool CArxPlotStyleNameComboBoxCtrl::OnApplyUseVisualStyle( TPropertyPtr pProp )
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

void CArxPlotStyleNameComboBoxCtrl::OnListChanged()
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

BEGIN_MESSAGE_MAP(CArxPlotStyleNameComboBoxCtrl, CAcUiPlotStyleNamesComboBox)
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_MOUSEMOVE()
	ON_CONTROL_REFLECT(CBN_SELCHANGE, &CArxPlotStyleNameComboBoxCtrl::OnCbnSelchange)
	ON_CONTROL_REFLECT(CBN_DROPDOWN, &CArxPlotStyleNameComboBoxCtrl::OnCbnDropdown)
	ON_MESSAGE(CB_ADDSTRING, &CArxPlotStyleNameComboBoxCtrl::OnModifyContent)
	ON_MESSAGE(CB_DELETESTRING, &CArxPlotStyleNameComboBoxCtrl::OnModifyContent)
	ON_MESSAGE(CB_DIR, &CArxPlotStyleNameComboBoxCtrl::OnModifyContent)
	ON_MESSAGE(CB_INSERTSTRING, &CArxPlotStyleNameComboBoxCtrl::OnModifyContent)
	ON_MESSAGE(CB_SETITEMDATA, &CArxPlotStyleNameComboBoxCtrl::OnModifyContent)
	ON_MESSAGE(CB_RESETCONTENT, &CArxPlotStyleNameComboBoxCtrl::OnResetContent)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CArxPlotStyleNameComboBoxCtrl message handlers

LRESULT CArxPlotStyleNameComboBoxCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
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

BOOL CArxPlotStyleNameComboBoxCtrl::PreTranslateMessage(MSG* pMsg)
{
	GetToolTipCtrl().RelayEvent(pMsg);
	return __super::PreTranslateMessage(pMsg);
}

void CArxPlotStyleNameComboBoxCtrl::PostNcDestroy()
{
	__super::PostNcDestroy();
	delete this;
}

LRESULT CArxPlotStyleNameComboBoxCtrl::OnModifyContent( WPARAM wParam, LPARAM lParam )
{
	LRESULT lResult = Default();
	OnListChanged();
	return lResult;
}

LRESULT CArxPlotStyleNameComboBoxCtrl::OnResetContent( WPARAM wParam, LPARAM lParam )
{
	CString sSelection;
	GetWindowText( sSelection );
	Default();
	OnListChanged();
	return (LRESULT)TRUE;
}

void CArxPlotStyleNameComboBoxCtrl::OnSetFocus(CWnd* pOldWnd)
{
	__super::OnSetFocus( pOldWnd );
#if (_ARXTARGET == 17)
	if( GetTheme().IsThemeActive() )
		Invalidate();
#endif
	GetArxServices()->HandleEvent( Prop::EventSetFocus );
}

void CArxPlotStyleNameComboBoxCtrl::OnKillFocus(CWnd* pNewWnd)
{
	__super::OnKillFocus( pNewWnd );
	GetArxServices()->HandleEvent( Prop::EventKillFocus );
}

void CArxPlotStyleNameComboBoxCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	GetArxServices()->HandleEvent( Prop::EventMouseMove, args_NNN( nFlags, ToDIP( point.x ), ToDIP( point.y ) ) );
	__super::OnMouseMove(nFlags, point);
}

void CArxPlotStyleNameComboBoxCtrl::OnCbnSelchange()
{
	int nCurSel = GetCurSel();
	CString sText;
	GetLBText( nCurSel, sText );
	GetArxServices()->HandleEvent( Prop::EventSelChanged, args_NS( nCurSel, sText ) );
	mpTemplate->SetStringProperty( Prop::Text, sText );
}

void CArxPlotStyleNameComboBoxCtrl::OnCbnDropdown()
{
	__super::OnDropDown();
	GetArxServices()->HandleEvent( Prop::EventDropDown );
}
