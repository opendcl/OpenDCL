#include "stdafx.h"
#include "ArxArrowComboBoxCtrl.h"
#include "DclControlObject.h"
#include "ControlPane.h"
#include "InvokeMethod.h"


/////////////////////////////////////////////////////////////////////////////
// CArxArrowComboBoxCtrl

CArxArrowComboBoxCtrl::CArxArrowComboBoxCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CDialogControl( pTemplate, pPane, this )
, CAcUiArrowHeadComboBox()
, mArxServices( this )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CArxArrowComboBoxCtrl::~CArxArrowComboBoxCtrl()
{
}

bool CArxArrowComboBoxCtrl::Create( CWnd* pParentWnd, UINT nID )
{
	bool bSuccess = __super::Create( GetWndStyle(), GetWndRect(), pParentWnd, nID );

	if( bSuccess && !ApplyPropertiesEnum() )
		bSuccess = false;

	return bSuccess;
}

CRect CArxArrowComboBoxCtrl::GetWndRect() const
{
	CRect rectCombo = CDialogControl::GetWndRect();
	long nListHeight = mpTemplate->GetLongProperty( Prop::DropDownHeight );
	if( nListHeight < 40 )
		nListHeight = 40;
	rectCombo.bottom += nListHeight;
	return rectCombo;
}

DWORD CArxArrowComboBoxCtrl::GetWndStyle() const
{
	DWORD dwStyle = CDialogControl::GetWndStyle();
	dwStyle |= (WS_VSCROLL | CBS_HASSTRINGS | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | CBS_NOINTEGRALHEIGHT);
	if( mpTemplate->GetBooleanProperty( Prop::Sorted ) )
		dwStyle |= CBS_SORT;
	else
		dwStyle &= ~(DWORD)CBS_SORT;
	return dwStyle;
}

bool CArxArrowComboBoxCtrl::ApplyProperty( TPropertyPtr pProp )
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
		if( pProp->GetBooleanValue() )
			ModifyStyle( 0, CBS_SORT, SWP_FRAMECHANGED );
		else
			ModifyStyle( CBS_SORT, 0, SWP_FRAMECHANGED );
		break;
	}
	return !bFailed;
}

BEGIN_MESSAGE_MAP(CArxArrowComboBoxCtrl, CAcUiArrowHeadComboBox)
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_MOUSEMOVE()
	ON_CONTROL_REFLECT(CBN_SELCHANGE, OnSelchange)
	ON_CONTROL_REFLECT(CBN_DROPDOWN, OnDropdown)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CComboBoxCtrl message handlers

BOOL CArxArrowComboBoxCtrl::PreTranslateMessage(MSG* pMsg)
{
	GetToolTipCtrl().RelayEvent(pMsg);
	return __super::PreTranslateMessage(pMsg);
}

void CArxArrowComboBoxCtrl::PostNcDestroy()
{
	__super::PostNcDestroy();
	delete this;
}

void CArxArrowComboBoxCtrl::OnSetFocus(CWnd* pOldWnd)
{
	__super::OnSetFocus( pOldWnd );
	SetEditSel( 0 , -1 );	//combobox is gaining focus, highlight text in edit control
	GetArxServices()->HandleEvent( Prop::EventSetFocus );
}

void CArxArrowComboBoxCtrl::OnKillFocus(CWnd* pNewWnd)
{
	__super::OnKillFocus( pNewWnd );
	GetArxServices()->HandleEvent( Prop::EventKillFocus );
}

void CArxArrowComboBoxCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	GetArxServices()->HandleEvent( Prop::EventMouseMove, args_NNN( nFlags, point.x, point.y ) );
	__super::OnMouseMove(nFlags, point);
}

void CArxArrowComboBoxCtrl::OnSelchange()
{
	int nCurSel = GetCurSel();
	CString sText;
	GetLBText( nCurSel, sText );
	GetArxServices()->HandleEvent( Prop::EventSelChanged, args_NS( nCurSel, sText ) );
	mpTemplate->SetStringProperty( Prop::Text, sText );
}

void CArxArrowComboBoxCtrl::OnDropdown()
{
	__super::OnDropDown();
	GetArxServices()->HandleEvent( Prop::EventDropDown );
}
