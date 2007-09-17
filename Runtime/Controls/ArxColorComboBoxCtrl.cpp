#include "stdafx.h"
#include "ArxColorComboBoxCtrl.h"
#include "DclControlObject.h"
#include "ControlPane.h"
#include "InvokeMethod.h"


/////////////////////////////////////////////////////////////////////////////
// CArxColorComboBoxCtrl

CArxColorComboBoxCtrl::CArxColorComboBoxCtrl( CDclControlObject* pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CDialogControl( pTemplate, pPane, this )
, CAcUiColorComboBox()
, mArxServices( pTemplate )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CArxColorComboBoxCtrl::~CArxColorComboBoxCtrl()
{
}

bool CArxColorComboBoxCtrl::Create( CWnd* pParentWnd, UINT nID )
{
	bool bSuccess = __super::Create( GetWndStyle(), GetWndRect(), pParentWnd, nID );

	if( bSuccess )
	{
		SetUseOther1( TRUE );
		SetUseOther2( TRUE );
	}

	if( bSuccess && !ApplyPropertiesEnum() )
		bSuccess = false;

	if( mpTemplate->GetLongProperty(Prop::EventInvoke) == 1 )
		m_bInvokeWithSendString = true;
	else
		m_bInvokeWithSendString = false;

	return bSuccess;
}

CRect CArxColorComboBoxCtrl::GetWndRect() const
{
	CRect rectCombo = CDialogControl::GetWndRect();
	long nListHeight = mpTemplate->GetLongProperty( Prop::DropDownHeight );
	if( nListHeight < 40 )
		nListHeight = 40;
	rectCombo.bottom += nListHeight;
	return rectCombo;
}

DWORD CArxColorComboBoxCtrl::GetWndStyle() const
{
	DWORD dwStyle = CDialogControl::GetWndStyle();
	dwStyle |= (CBS_HASSTRINGS | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED);
	if( mpTemplate->GetBooleanProperty( Prop::Sorted ) )
		dwStyle |= CBS_SORT;
	else
		dwStyle &= ~(DWORD)CBS_SORT;
	return dwStyle;
}

bool CArxColorComboBoxCtrl::OnApplyProperty( TPropertyPtr pProp )
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

BEGIN_MESSAGE_MAP(CArxColorComboBoxCtrl, CAcUiColorComboBox)
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_MOUSEMOVE()
	ON_CONTROL_REFLECT(CBN_SELCHANGE, OnSelchange)
	ON_CONTROL_REFLECT(CBN_DROPDOWN, OnDropdown)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CComboBoxCtrl message handlers

BOOL CArxColorComboBoxCtrl::PreTranslateMessage(MSG* pMsg)
{
	GetToolTipCtrl().RelayEvent(pMsg);
	return __super::PreTranslateMessage(pMsg);
}

void CArxColorComboBoxCtrl::PostNcDestroy()
{
	__super::PostNcDestroy();
	delete this;
}

void CArxColorComboBoxCtrl::OnSetFocus(CWnd* pOldWnd)
{
	__super::OnSetFocus( pOldWnd );
	SetEditSel( 0 , -1 );	//combobox is gaining focus, highlight text in edit control
	InvokeMethod( mpTemplate->GetStringProperty( Prop::EventSetFocus ), m_bInvokeWithSendString );
}

void CArxColorComboBoxCtrl::OnKillFocus(CWnd* pNewWnd)
{
	__super::OnKillFocus( pNewWnd );
	InvokeMethod( mpTemplate->GetStringProperty( Prop::EventKillFocus ), m_bInvokeWithSendString );
}

void CArxColorComboBoxCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	InvokeMethodIntIntInt(
			mpTemplate->GetStringProperty( Prop::EventMouseMove ),
			nFlags,
			point.x,
			point.y,
			m_bInvokeWithSendString);
	__super::OnMouseMove(nFlags, point);
}

void CArxColorComboBoxCtrl::OnSelchange()
{
	CString sText;
	GetWindowText( sText );
	InvokeMethodIntString( mpTemplate->GetStringProperty( Prop::EventSelChanged ),
												 GetCurSel(),
												 sText,
												 m_bInvokeWithSendString );
	mpTemplate->SetStringProperty( Prop::Text, sText );
}

void CArxColorComboBoxCtrl::OnDropdown()
{
	InvokeMethod( mpTemplate->GetStringProperty( Prop::EventDropDown ), m_bInvokeWithSendString );
}
