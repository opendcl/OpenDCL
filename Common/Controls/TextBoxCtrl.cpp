#include "stdafx.h"
#include "TextBoxCtrl.h"
#include "DclControlObject.h"
#include "ControlPane.h"


/////////////////////////////////////////////////////////////////////////////
// CTextBoxCtrl

CTextBoxCtrl::CTextBoxCtrl( CDclControlObject* pTemplate, CControlPane* pPane, UINT nID,
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
	DWORD dwStyle = CDialogControl::GetWndStyle();
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
			Invalidate();
			break;
		}
	case Prop::Text:
		{
			SetWindowText( pProp->GetStringValue() );
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
		}
	}
	return !bFailed;
}

BEGIN_MESSAGE_MAP(CTextBoxCtrl, CFilteredEditCtrl)
	ON_CONTROL_REFLECT(EN_CHANGE, &CTextBoxCtrl::OnEnChange)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTextBoxCtrl message handlers

void CTextBoxCtrl::PostNcDestroy() 
{
	__super::PostNcDestroy();
	delete this;
}

void CTextBoxCtrl::OnEnChange()
{
	CString sText;
	GetWindowText( sText );
	mpTemplate->SetStringProperty( Prop::Text, sText );
}
