#include "stdafx.h"
#include "TextBoxCtrl.h"
#include "DclControlObject.h"
#include "ControlPane.h"


/////////////////////////////////////////////////////////////////////////////
// CTextBoxCtrl

CTextBoxCtrl::CTextBoxCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID,
														CInputFilter* pFilter /*= NULL*/, bool bCreate /*= true*/ )
: CDialogControl( pTemplate, pPane, this )
, CFilteredEditCtrl( pFilter )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CTextBoxCtrl::~CTextBoxCtrl()
{
}

bool CTextBoxCtrl::Create( CWnd* pParentWnd, UINT nID )
{
	bool bSuccess = __super::Create( pParentWnd, GetWndRect(), nID, GetWndStyle(), GetInputFilter() );

	if( bSuccess && !ApplyPropertiesEnum() )
		bSuccess = false;

	return bSuccess;
}

DWORD CTextBoxCtrl::GetWndStyle() const
{
	DWORD dwStyle = __super::GetWndStyle();
	dwStyle |= (ES_WANTRETURN);
	switch( mpTemplate->GetLongProperty( Prop::Justification ) )
	{
	case 0:
		dwStyle |= ES_LEFT;
		break;
	case 1:
		dwStyle |= ES_CENTER;
		break;
	case 2:
		dwStyle |= ES_RIGHT;
		break;
	}
	return dwStyle;
}

bool CTextBoxCtrl::OnApplyProperty( TPropertyPtr pProp )
{
	if( !__super::OnApplyProperty( pProp ) )
		return false;
	bool bFailed = false;
	switch( pProp->GetID() )
	{
	case Prop::FilterStyle: //can't currently change style at runtime -- [ORW] 2007-09-10
		break;
	case Prop::Justification:
		{
			switch( pProp->GetLongValue() )
			{
			case 0:
				ModifyStyle( (ES_CENTER | ES_RIGHT), ES_LEFT, 0 );
				break;
			case 1:
				ModifyStyle( (ES_LEFT | ES_RIGHT), ES_CENTER, 0 );
				break;
			case 2:
				ModifyStyle( (ES_LEFT | ES_CENTER), ES_RIGHT, 0 );
				break;
			}
			OnNeedRepaint();
			break;
		}
	case Prop::Text:
		{
			SetWindowText( pProp->GetStringValue() );
			OnNeedRepaint();
			break;
		}
	case Prop::AutoVScroll:
		{
			if( pProp->GetBooleanValue() )
				ModifyStyle( 0, ES_AUTOVSCROLL, 0 );
			else
				ModifyStyle( ES_AUTOVSCROLL, 0, 0 );
			break;
		}
	case Prop::AutoHScroll:
		{
			if( pProp->GetBooleanValue() )
				ModifyStyle( 0, ES_AUTOHSCROLL, 0 );
			else
				ModifyStyle( ES_AUTOHSCROLL, 0, 0 );
			break;
		}
	case Prop::LimitText:
		{
			SetLimitText( pProp->GetLongValue() );
			break;
		}
	case Prop::MarginLeft:
		{
			SetMargins( pProp->GetLongValue(), mpTemplate->GetLongProperty( Prop::MarginRight ) );
			break;
		}
	case Prop::MarginRight:
		{
			if( !IsEnumeratingProperties() )
				SetMargins( mpTemplate->GetLongProperty( Prop::MarginLeft ), pProp->GetLongValue() );
			break;
	case Prop::ReadOnly:
		{
			SetReadOnly( pProp->GetBooleanValue() );
			break;
		}
		}
	}
	return !bFailed;
}

BEGIN_MESSAGE_MAP(CTextBoxCtrl, CFilteredEditCtrl)
	ON_CONTROL_REFLECT(EN_CHANGE, &CTextBoxCtrl::OnChange)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTextBoxCtrl message handlers

BOOL CTextBoxCtrl::PreTranslateMessage(MSG* pMsg) 
{
	GetToolTipCtrl().RelayEvent(pMsg);
	if( pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN )
	{
		if( mpTemplate->GetBooleanProperty( Prop::ReturnAsTab ) )
			pMsg->wParam = VK_TAB;		
	}
	return __super::PreTranslateMessage(pMsg);
}

void CTextBoxCtrl::PostNcDestroy() 
{
	__super::PostNcDestroy();
	delete this;
}

void CTextBoxCtrl::OnChange()
{
	CString sText;
	GetWindowText( sText );
	if( mpTemplate->GetStringProperty( Prop::Text ) != sText )
		mpTemplate->SetStringProperty( Prop::Text, sText );
}
