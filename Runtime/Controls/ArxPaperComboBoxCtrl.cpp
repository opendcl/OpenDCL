// PaperComboBoxCtrl.cpp : implementation file
//

#include "Stdafx.h"
#include "ArxPaperComboBoxCtrl.h"
#include "PaperComboHandler.h"
#include "DclControlObject.h"
#include "PropertyObject.h"
#include "PropertyIds.h"
#include "InvokeMethod.h"
#include "Workspace.h"
#include "ControlPane.h"
#include "AutoDocLock.h"


static TDclControlPtr FindPrinterCombo( TDclFormPtr pForm )
{
	if( !pForm )
		return NULL;
	TDclControlList listCombos;
	pForm->FindControls( CtlComboBox, listCombos );
	for( TDclControlList::iterator iter = listCombos.begin(); iter != listCombos.end(); ++iter )
	{
		if( (*iter)->GetLongProperty(Prop::ComboBoxStyle) == CmboStyle_Plotters )
			return *iter;
	}
	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CArxPaperComboBoxCtrl

CArxPaperComboBoxCtrl::CArxPaperComboBoxCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID,
																							bool bCreate /*= true*/ )
: CArxComboBoxCtrl( pTemplate, pPane, nID, new CPaperComboHandler( FindPrinterCombo( pTemplate->GetOwnerForm() ) ), false )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CArxPaperComboBoxCtrl::~CArxPaperComboBoxCtrl()
{
}

bool CArxPaperComboBoxCtrl::Create( CWnd* pParentWnd, UINT nID )
{
	bool bSuccess =
		__super::Create( pParentWnd, nID );

	return bSuccess;
}

DWORD CArxPaperComboBoxCtrl::GetWndStyle() const
{
	DWORD dwStyle = __super::GetWndStyle();
	dwStyle |= (CBS_DROPDOWNLIST);
	return dwStyle;
}

bool CArxPaperComboBoxCtrl::OnApplyProperty( TPropertyPtr pProp )
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


BEGIN_MESSAGE_MAP(CArxPaperComboBoxCtrl, CArxComboBoxCtrl)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CArxPaperComboBoxCtrl message handlers
