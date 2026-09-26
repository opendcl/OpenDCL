#include "stdafx.h"
#include "ArxPlotStyleTableComboBoxCtrl.h"
#include "DclControlTemplate.h"
#include "ControlPane.h"
#include "InvokeMethod.h"
#include "ComboHostThemeHelper.h"


/////////////////////////////////////////////////////////////////////////////
// CArxPlotStyleTableComboBoxCtrl

CArxPlotStyleTableComboBoxCtrl::CArxPlotStyleTableComboBoxCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CDialogControl( pTemplate, pPane, this )
, CAcUiPlotStyleTablesComboBox()
, mArxServices( this )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CArxPlotStyleTableComboBoxCtrl::~CArxPlotStyleTableComboBoxCtrl()
{
}

bool CArxPlotStyleTableComboBoxCtrl::Create( CWnd* pParentWnd, UINT nID )
{
	bool bSuccess = (__super::Create( GetWndStyle(), GetWndRect(), pParentWnd, nID ) != FALSE);

	if( bSuccess && !ApplyPropertiesEnum() )
		bSuccess = false;

	if( bSuccess )
		SyncAcUiComboHostTheme( this, this );

	return bSuccess;
}

CRect CArxPlotStyleTableComboBoxCtrl::GetWndRect() const
{
	CRect rectCombo = CDialogControl::GetWndRect();
	long nListHeight = mpTemplate->GetLongProperty( Prop::DropDownHeight );
	if( nListHeight < 40 )
		nListHeight = 40;
	rectCombo.bottom += FromDIP( nListHeight );;
	return rectCombo;
}

DWORD CArxPlotStyleTableComboBoxCtrl::GetWndStyle() const
{
	DWORD dwStyle = CDialogControl::GetWndStyle();
	dwStyle |= (WS_VSCROLL | CBS_HASSTRINGS | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | CBS_NOINTEGRALHEIGHT);
	//if( mpTemplate->GetBooleanProperty( Prop::Sorted ) )
	//	dwStyle |= CBS_SORT;
	//else
	//	dwStyle &= ~(DWORD)CBS_SORT;
	return dwStyle;
}

bool CArxPlotStyleTableComboBoxCtrl::ApplyProperty( TPropertyPtr pProp )
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

bool CArxPlotStyleTableComboBoxCtrl::OnApplyUseVisualStyle( TPropertyPtr pProp )
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
	const bool bOk = __super::OnApplyUseVisualStyle( pProp );
	SyncAcUiComboHostTheme( this, this );
	return bOk;
}

void CArxPlotStyleTableComboBoxCtrl::OnListChanged()
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


void CArxPlotStyleTableComboBoxCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if( !lpDrawItemStruct || !CHostThemeHelper::HostMaps() )
	{
		__super::DrawItem( lpDrawItemStruct );
		return;
	}
#if defined(ODCL_ACUI_HOST_DRAWITEM)
	OdclAcUiComboHostDrawItem( this, lpDrawItemStruct );
#else
	__super::DrawItem( lpDrawItemStruct );
#endif
}

BOOL CArxPlotStyleTableComboBoxCtrl::GetItemColors(DRAWITEMSTRUCT& dis, COLORREF& fgColor, COLORREF& bgColor, COLORREF& fillColor)
{
	const BOOL bOk = __super::GetItemColors( dis, fgColor, bgColor, fillColor );
	RemapAcUiComboItemColors( fgColor, bgColor, fillColor );
	return bOk;
}

void CArxPlotStyleTableComboBoxCtrl::HandleHostThemeChanged()
{
	SyncAcUiComboHostTheme( this, this );
}

HBRUSH CArxPlotStyleTableComboBoxCtrl::CtlColor(CDC* pDC, UINT nCtlColor)
{
	return AcUiComboHostCtlColor( this, pDC, nCtlColor );
}

HBRUSH CArxPlotStyleTableComboBoxCtrl::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = AcUiComboHostCtlColor( this, pDC, nCtlColor );
	if( hbr )
		return hbr;
	return __super::OnCtlColor( pDC, pWnd, nCtlColor );
}

BEGIN_MESSAGE_MAP(CArxPlotStyleTableComboBoxCtrl, CAcUiPlotStyleTablesComboBox)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_CTLCOLOR()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_MOUSEMOVE()
	ON_CONTROL_REFLECT(CBN_SELCHANGE, &CArxPlotStyleTableComboBoxCtrl::OnCbnSelchange)
	ON_CONTROL_REFLECT(CBN_DROPDOWN, &CArxPlotStyleTableComboBoxCtrl::OnCbnDropdown)
	ON_MESSAGE(CB_ADDSTRING, &CArxPlotStyleTableComboBoxCtrl::OnModifyContent)
	ON_MESSAGE(CB_DELETESTRING, &CArxPlotStyleTableComboBoxCtrl::OnModifyContent)
	ON_MESSAGE(CB_DIR, &CArxPlotStyleTableComboBoxCtrl::OnModifyContent)
	ON_MESSAGE(CB_INSERTSTRING, &CArxPlotStyleTableComboBoxCtrl::OnModifyContent)
	ON_MESSAGE(CB_SETITEMDATA, &CArxPlotStyleTableComboBoxCtrl::OnModifyContent)
	ON_MESSAGE(CB_RESETCONTENT, &CArxPlotStyleTableComboBoxCtrl::OnResetContent)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CArxPlotStyleTableComboBoxCtrl message handlers

LRESULT CArxPlotStyleTableComboBoxCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = __super::WindowProc(message, wParam, lParam);
	if( message == WM_PAINT )
		PaintAcUiComboChrome( this );
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

BOOL CArxPlotStyleTableComboBoxCtrl::PreTranslateMessage(MSG* pMsg)
{
	GetToolTipCtrl().RelayEvent(pMsg);
	return __super::PreTranslateMessage(pMsg);
}

void CArxPlotStyleTableComboBoxCtrl::PostNcDestroy()
{
	__super::PostNcDestroy();
	delete this;
}

LRESULT CArxPlotStyleTableComboBoxCtrl::OnModifyContent( WPARAM wParam, LPARAM lParam )
{
	LRESULT lResult = Default();
	OnListChanged();
	return lResult;
}

LRESULT CArxPlotStyleTableComboBoxCtrl::OnResetContent( WPARAM wParam, LPARAM lParam )
{
	Default();
	OnListChanged();
	return (LRESULT)TRUE;
}

void CArxPlotStyleTableComboBoxCtrl::OnSetFocus(CWnd* pOldWnd)
{
	__super::OnSetFocus( pOldWnd );
#if (_ARXTARGET == 17)
	if( GetTheme().IsThemeActive() )
		Invalidate();
#endif
	GetArxServices()->HandleEvent( Prop::EventSetFocus );
}

void CArxPlotStyleTableComboBoxCtrl::OnKillFocus(CWnd* pNewWnd)
{
	__super::OnKillFocus( pNewWnd );
	GetArxServices()->HandleEvent( Prop::EventKillFocus );
}

void CArxPlotStyleTableComboBoxCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	GetArxServices()->HandleEvent( Prop::EventMouseMove, args_NNN( nFlags, ToDIP( point.x ), ToDIP( point.y ) ) );
	__super::OnMouseMove(nFlags, point);
}

void CArxPlotStyleTableComboBoxCtrl::OnCbnSelchange()
{
	int nCurSel = GetCurSel();
	CString sText;
	GetLBText( nCurSel, sText );
	GetArxServices()->HandleEvent( Prop::EventSelChanged, args_NS( nCurSel, sText ) );
	mpTemplate->SetStringProperty( Prop::Text, sText );
}

void CArxPlotStyleTableComboBoxCtrl::OnCbnDropdown()
{
	__super::OnDropDown();
	GetArxServices()->HandleEvent( Prop::EventDropDown );
}
