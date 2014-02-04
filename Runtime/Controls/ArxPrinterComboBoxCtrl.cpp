// ArxPrinterComboBoxCtrl.cpp : implementation file
//

#include "Stdafx.h"
#include "ArxPrinterComboBoxCtrl.h"
#include "PrinterComboHandler.h"
#include "ArxPaperComboBoxCtrl.h"
#include "DclControlTemplate.h"
#include "PropertyObject.h"
#include "PropertyIds.h"
#include "InvokeMethod.h"
#include "Workspace.h"
#include "ControlPane.h"


/////////////////////////////////////////////////////////////////////////////
// CArxPrinterComboBoxCtrl

CArxPrinterComboBoxCtrl::CArxPrinterComboBoxCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID,
																									TDclControlPtr pPaperCombo /*= NULL*/, bool bCreate /*= true*/ )
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

bool CArxPrinterComboBoxCtrl::ApplyProperty( TPropertyPtr pProp )
{
	if( !__super::ApplyProperty( pProp ) )
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

void CArxPrinterComboBoxCtrl::SetPaperSizeCombo( TDclControlPtr pPaperCombo )
{
	mpPaperCombo = pPaperCombo;
	if( pPaperCombo )
	{
		CDialogControl* pDclControl = pPaperCombo->GetControlInstance();
		if( pDclControl )
		{
			CArxPaperComboBoxCtrl* pComboCtrl = (CArxPaperComboBoxCtrl*)pDclControl->GetControlWnd();
			pComboCtrl->ResetContent();
		}
	}
}


BEGIN_MESSAGE_MAP(CArxPrinterComboBoxCtrl, CArxComboBoxCtrl)
	ON_CONTROL_REFLECT(CBN_SELCHANGE, &CArxPrinterComboBoxCtrl::OnCbnSelchange)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CArxPrinterComboBoxCtrl message handlers

LRESULT CArxPrinterComboBoxCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
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

void CArxPrinterComboBoxCtrl::OnCbnSelchange()
{
	__super::OnCbnSelchange();
	if( mpPaperCombo )
	{
		CDialogControl* pDclControl = mpPaperCombo->GetControlInstance();
		if( pDclControl )
		{
			CArxPaperComboBoxCtrl* pComboCtrl = (CArxPaperComboBoxCtrl*)pDclControl->GetControlWnd();
			pComboCtrl->ResetContent();
		}
	}
}
