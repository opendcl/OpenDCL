#include "stdafx.h"
#include "ArxComboBoxCtrl.h"
#include "DclControlObject.h"
#include "ControlPane.h"


/////////////////////////////////////////////////////////////////////////////
// CArxComboBoxCtrl

CArxComboBoxCtrl::CArxComboBoxCtrl( CDclControlObject* pTemplate, CControlPane* pPane, UINT nID,
																		CComboHandler* pHandler, bool bCreate /*= true*/ )
: CComboBoxCtrl( pTemplate, pPane, nID, pHandler )
, mArxServices( pTemplate )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CArxComboBoxCtrl::~CArxComboBoxCtrl()
{
}

bool CArxComboBoxCtrl::Create( CWnd* pParentWnd, UINT nID )
{
	bool bSuccess =
		__super::Create( pParentWnd, nID );

	if( GetTemplate()->GetLongProperty(Prop::EventInvoke) == 1 )
		m_bInvokeWithSendString = true;
	else
		m_bInvokeWithSendString = false;

	return bSuccess;
}

bool CArxComboBoxCtrl::ApplyProperty( TPropertyPtr pProp )
{
	if( !__super::ApplyProperty( pProp ) )
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

void CArxComboBoxCtrl::SetDragnDrop(BOOL bRegister)
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

BEGIN_MESSAGE_MAP(CArxComboBoxCtrl, CComboBoxCtrl)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CArxComboBoxCtrl message handlers
