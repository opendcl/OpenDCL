// ArxPrinterComboBoxCtrl.cpp : implementation file
//

#include "Stdafx.h"
#include "ArxPrinterComboBoxCtrl.h"
#include "PrinterComboHandler.h"
#include "ArxPaperComboBoxCtrl.h"
#include "DclControlObject.h"
#include "PropertyObject.h"
#include "PropertyIds.h"
#include "InvokeMethod.h"
#include "Workspace.h"
#include "ControlPane.h"


/////////////////////////////////////////////////////////////////////////////
// CArxPrinterComboBoxCtrl

CArxPrinterComboBoxCtrl::CArxPrinterComboBoxCtrl( CDclControlObject* pTemplate, CControlPane* pPane, UINT nID,
																									CDclControlObject* pPaperCombo /*= NULL*/, bool bCreate /*= true*/ )
: CArxComboBoxCtrl( pTemplate, pPane, nID, new CPrinterComboHandler, false )
, mpPaperCombo( NULL )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );

	SetPaperSizeCombo( pPaperCombo );
}

CArxPrinterComboBoxCtrl::~CArxPrinterComboBoxCtrl()
{
}

bool CArxPrinterComboBoxCtrl::Create( CWnd* pParentWnd, UINT nID )
{
	bool bSuccess =
		__super::Create( pParentWnd, nID );

	return bSuccess;
}

DWORD CArxPrinterComboBoxCtrl::GetWndStyle() const
{
	DWORD dwStyle = __super::GetWndStyle();
	dwStyle |= (CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED);
	return dwStyle;
}

bool CArxPrinterComboBoxCtrl::OnApplyProperty( TPropertyPtr pProp )
{
	if( !__super::OnApplyProperty( pProp ) )
		return false;
	bool bFailed = false;
	//switch( pProp->GetID() )
	//{
	//case Prop::DragnDropAllowDrop:
	//	{
	//		SetDragnDrop( pProp->GetBooleanValue() );
	//		break;
	//	}
	//}
	return !bFailed;
}

void CArxPrinterComboBoxCtrl::SetPaperSizeCombo( CDclControlObject* pPaperCombo )
{
	mpPaperCombo = pPaperCombo;
	if( pPaperCombo )
	{
		CDialogControl* pDclControl = pPaperCombo->GetControlInstance();
		if( pDclControl )
		{
			CArxPaperComboBoxCtrl* pComboCtrl = (CArxPaperComboBoxCtrl*)pDclControl->GetControl();
			CComboHandler* pComboHandler = pComboCtrl->GetComboHandler();
			if( pComboHandler )
				pComboHandler->PopulateList( pComboCtrl );
		}
	}
}


BEGIN_MESSAGE_MAP(CArxPrinterComboBoxCtrl, CArxComboBoxCtrl)
	ON_CONTROL_REFLECT(CBN_SELCHANGE, &CArxPrinterComboBoxCtrl::OnSelchange)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CArxPrinterComboBoxCtrl message handlers

void CArxPrinterComboBoxCtrl::OnSelchange()
{
	__super::OnSelchange();
	if( mpPaperCombo )
	{
		CDialogControl* pDclControl = mpPaperCombo->GetControlInstance();
		if( pDclControl )
		{
			CArxPaperComboBoxCtrl* pComboCtrl = (CArxPaperComboBoxCtrl*)pDclControl->GetControl();
			CComboHandler* pComboHandler = pComboCtrl->GetComboHandler();
			if( pComboHandler )
				pComboHandler->PopulateList( pComboCtrl );
		}
	}
}
