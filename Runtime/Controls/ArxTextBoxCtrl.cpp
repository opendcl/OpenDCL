#include "stdafx.h"
#include "ArxTextBoxCtrl.h"
#include "DclControlObject.h"
#include "ControlPane.h"


/////////////////////////////////////////////////////////////////////////////
// CArxTextBoxCtrl

CArxTextBoxCtrl::CArxTextBoxCtrl( CDclControlObject* pTemplate, CControlPane* pPane, UINT nID,
																	CInputFilter* pFilter /*= NULL*/, DWORD dwAcUiStyle /*= 0*/,
																	bool bCreate /*= true*/ )
: CTextBoxCtrl( pTemplate, pPane, nID, pFilter, false )
, mArxServices( pTemplate )
{
	SetStyleMask( dwAcUiStyle );
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CArxTextBoxCtrl::~CArxTextBoxCtrl()
{
}

bool CArxTextBoxCtrl::Create( CWnd* pParentWnd, UINT nID )
{
	bool bSuccess =
		__super::Create( pParentWnd, nID );

	if( GetTemplate()->GetLongProperty(Prop::EventInvoke) == 1 )
		m_bInvokeWithSendString = true;
	else
		m_bInvokeWithSendString = false;

	return bSuccess;
}

bool CArxTextBoxCtrl::OnApplyProperty( TPropertyPtr pProp )
{
	if( !__super::OnApplyProperty( pProp ) )
		return false;
	bool bFailed = false;
	switch( pProp->GetID() )
	{
	case Prop::DragnDropAllowDrop:
		{
			SetDragnDrop( pProp->GetBooleanValue() );
			break;
		}
	}
	return !bFailed;
}

void CArxTextBoxCtrl::SetDragnDrop(BOOL bRegister)
{
	if (bRegister == TRUE)
	{
		BOOL success = m_DropTarget.Register(this);
		m_DropTarget.m_pThisArxControl = mpTemplate;
		m_DropTarget.m_pParent = this;
	}
	else
		m_DropTarget.Revoke();
}

BEGIN_MESSAGE_MAP(CArxTextBoxCtrl, CTextBoxCtrl)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CArxTextBoxCtrl message handlers
