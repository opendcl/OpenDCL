#include "stdafx.h"
#include "ArxPlotStyleTableComboBoxCtrl.h"
#include "DclControlObject.h"
#include "ControlPane.h"
#include "InvokeMethod.h"


/////////////////////////////////////////////////////////////////////////////
// CArxPlotStyleTableComboBoxCtrl

CArxPlotStyleTableComboBoxCtrl::CArxPlotStyleTableComboBoxCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CDialogControl( pTemplate, pPane, this )
, CAcUiPlotStyleTablesComboBox()
, mArxServices( pTemplate )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CArxPlotStyleTableComboBoxCtrl::~CArxPlotStyleTableComboBoxCtrl()
{
}

bool CArxPlotStyleTableComboBoxCtrl::Create( CWnd* pParentWnd, UINT nID )
{
	bool bSuccess = __super::Create( GetWndStyle(), GetWndRect(), pParentWnd, nID );

	if( bSuccess && !ApplyPropertiesEnum() )
		bSuccess = false;

	return bSuccess;
}

CRect CArxPlotStyleTableComboBoxCtrl::GetWndRect() const
{
	CRect rectCombo = CDialogControl::GetWndRect();
	long nListHeight = mpTemplate->GetLongProperty( Prop::DropDownHeight );
	if( nListHeight < 40 )
		nListHeight = 40;
	rectCombo.bottom += nListHeight;
	return rectCombo;
}

DWORD CArxPlotStyleTableComboBoxCtrl::GetWndStyle() const
{
	DWORD dwStyle = CDialogControl::GetWndStyle();
	dwStyle |= (WS_VSCROLL | CBS_HASSTRINGS | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | CBS_NOINTEGRALHEIGHT);
	if( mpTemplate->GetBooleanProperty( Prop::Sorted ) )
		dwStyle |= CBS_SORT;
	else
		dwStyle &= ~(DWORD)CBS_SORT;
	return dwStyle;
}

bool CArxPlotStyleTableComboBoxCtrl::OnApplyProperty( TPropertyPtr pProp )
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
			const PropVal::TIntArray& rInt = *pProp->GetIntArrayPtr();
			for( int idx = 0; (size_t)idx < rInt.size(); ++idx )
				SetItemData( idx, (DWORD_PTR)rInt.at( idx ) );
		}
		break;
	case Prop::LimitText:
		LimitText( pProp->GetLongValue() );
		break;
	case Prop::Sorted:
		if( pProp->GetBooleanValue() )
			ModifyStyle( 0, CBS_SORT, SWP_FRAMECHANGED );
		else
			ModifyStyle( CBS_SORT, 0, SWP_FRAMECHANGED );
		break;
	}
	return !bFailed;
}

BEGIN_MESSAGE_MAP(CArxPlotStyleTableComboBoxCtrl, CAcUiPlotStyleTablesComboBox)
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_MOUSEMOVE()
	ON_CONTROL_REFLECT(CBN_SELCHANGE, OnSelchange)
	ON_CONTROL_REFLECT(CBN_DROPDOWN, OnDropdown)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CComboBoxCtrl message handlers

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

void CArxPlotStyleTableComboBoxCtrl::OnSetFocus(CWnd* pOldWnd)
{
	__super::OnSetFocus( pOldWnd );
	SetEditSel( 0 , -1 );	//combobox is gaining focus, highlight text in edit control
	InvokeMethod( mpTemplate->GetStringProperty( Prop::EventSetFocus ), IsAsyncEvents() );
}

void CArxPlotStyleTableComboBoxCtrl::OnKillFocus(CWnd* pNewWnd)
{
	__super::OnKillFocus( pNewWnd );
	InvokeMethod( mpTemplate->GetStringProperty( Prop::EventKillFocus ), IsAsyncEvents() );
}

void CArxPlotStyleTableComboBoxCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	InvokeMethodIntIntInt(
			mpTemplate->GetStringProperty( Prop::EventMouseMove ),
			nFlags,
			point.x,
			point.y,
			IsAsyncEvents());
	__super::OnMouseMove(nFlags, point);
}

void CArxPlotStyleTableComboBoxCtrl::OnSelchange()
{
	CString sText;
	GetWindowText( sText );
	InvokeMethodIntString( mpTemplate->GetStringProperty( Prop::EventSelChanged ),
												 GetCurSel(),
												 sText,
												 IsAsyncEvents() );
	mpTemplate->SetStringProperty( Prop::Text, sText );
}

void CArxPlotStyleTableComboBoxCtrl::OnDropdown()
{
	__super::OnDropDown();
	InvokeMethod( mpTemplate->GetStringProperty( Prop::EventDropDown ), IsAsyncEvents() );
}
