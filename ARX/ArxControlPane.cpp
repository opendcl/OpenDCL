// ArxControlPane.cpp : implementation file
//

#include "stdafx.h"
#include "ArxControlPane.h"
#include "ArxDialogControl.h"
#include "DclControlObject.h"
#include "PropertyIds.h"
#include "ControlTypes.h"
#include "XPStyleButtonST.h"
#include "Resource.h"
#include "AxContainer.h"
#include "VdclAngleEdit.h"
#include "OdclEdit.h"
#include "VdclNumericEdit.h"
#include "VdclSymbolEdit.h"
#include "VdclComboBoxEx.h"
#include "VdclComboBox.h"
#include "VdclArrowHeadComboBox.h"
#include "VdclColorComboBox.h"
#include "VdclLineWeightComboBox.h"
#include "VdclPlotStyleNamesComboBox.h"
#include "VdclPlotStyleTablesComboBox.h"
#include "FontCombo.h"
#include "PrinterComboBox.h"
#include "ComboBoxFolder.h"
#include "OdclLayerCombo.h"
#include "PropertyObject.h"
#include "Splitter.h"
#include "DwgDirList.h"
#include "OptionListBox.h"
#include "VdclStatic.h"
#include "GsPreviewCtrl.h"
#include "SpreadSheet.h"
#include "OdclListCtrl.h"
#include "VdclCheckBox.h"
#include "VdclGroupBox.h"
#include "VdclListBox.h"
#include "DwgPreviewCtrl.h"
#include "VdclRadioButton.h"
#include "OdclMonth.h"
#include "PictureBox.h"
#include "ProgressTimeToComplete.h"
#include "RoundSliderCtrl.h"
#include "VdclSliderCtrl.h"
#include "VdclScrollBar.h"
#include "VdclSpinButton.h"
#include "SlideHolder.h"
#include "StaticLink.h"
#include "HtmlCtrl.h"
#include "VdclTree.h"
#include "VdclTextButton.h"
#include "VdclTab.h"
#include "TabPage.h"
#include "DclFormObject.h"
#include "ArxProject.h"
#include "ToolTips.h"

static const TCHAR *sFilterName = _T("IDB_FILTER";)


/////////////////////////////////////////////////////////////////////////////
// CArxControlPane

CArxControlPane::CArxControlPane()
: CControlPane()
{
	TraceFmt( _T("> CArxControlPane::CArxControlPane() [this = %08X]\r\n"), (long)this );
}

CArxControlPane::CArxControlPane( CDclFormObject* pSourceForm, CWnd* pHostDlg )
: CControlPane( pSourceForm, pHostDlg )
{
	TraceFmt( _T("> CArxControlPane::CArxControlPane(%s [%08X], %s [HWND: %08X]) [this: %08X]\r\n"),
						pSourceForm->GetKeyPath(), pSourceForm, CString(pHostDlg->GetRuntimeClass()->m_lpszClassName),
						pHostDlg->m_hWnd, (long)this );
}

CArxControlPane::~CArxControlPane()
{
	TraceFmt( _T("< CArxControlPane::~CArxControlPane() [this: %08X]\r\n"), (long)this );
}


void CArxControlPane::SetGlobalLispSymbols( bool bResetToNil /*= false*/ )
{
	for( TDialogControls::iterator iter = mControls.begin(); iter != mControls.end(); ++iter )
	{
		CString sLispSymbolName = ((CArxDialogControl*)&*(*iter))->GetLispSymbolName();
		if( bResetToNil )
			theArxWorkspace.ResetLispSymbol( sLispSymbolName );
		else	
			theArxWorkspace.SetLispSymbol( sLispSymbolName, (long)&**iter );
	}
}

TDialogControlPtr CArxControlPane::CreateNewDialogControl( CDclControlObject* pTemplate,
																													 UINT nID )
{
	return CArxDialogControl::Create( pTemplate, this, nID );
}

void CArxControlPane::CleanUpControls() 
{	
	for (int idx = mControls.size() - 1; idx >= 0; --idx)
	{
		TDialogControlPtr pControl = mControls[idx];
		if( pControl->GetControlType() == CtlActiveX )
			((CAxContainer*)&*(pControl->GetControl()))->CloseWindow();
	}
	CControlPane::CleanUpControls();
}

bool CArxControlPane::FindControl( HWND hwndControl, /*out*/ CString& sControlName ) const
{
	if (!hwndControl)
		return false;
	size_t ctMax = mControls.size();
	for( size_t idx = 0; idx < ctMax; ++idx )
	{
		TDialogControlPtr pControl = mControls.at( idx );
		assert( pControl != NULL );
		if( pControl->GetControl()->m_hWnd == hwndControl )
		{
			sControlName = pControl->GetKeyName();
			return true;
		}
		TDialogControlPtr pNestedControl = pControl->FindControl( hwndControl );
		if( pNestedControl )
		{
			sControlName = pNestedControl->GetKeyName();
			return true;
		}
	}
	return false;
}

TDialogControlPtr CArxControlPane::FindControl( HWND hwndControl ) const
{
	if (!hwndControl)
		return false;
	size_t ctMax = mControls.size();
	for( size_t idx = 0; idx < ctMax; ++idx )
	{
		TDialogControlPtr pControl = mControls.at( idx );
		assert( pControl != NULL );
		if( pControl->GetControl()->m_hWnd == hwndControl )
			return pControl;
		TDialogControlPtr pNestedControl = pControl->FindControl( hwndControl );
		if( pNestedControl )
			return pNestedControl;
	}
	return NULL;
}

TDialogControlPtr CArxControlPane::FindControl( LPCTSTR pszControlName, ControlType type /*= CtlInvalid*/ ) const
{
	for( size_t idx = 0; idx < mControls.size(); ++idx )
	{
		TDialogControlPtr pControl = mControls.at( idx );
		assert( pControl != NULL );
		if( (type == CtlInvalid || pControl->GetControlType() == type) && pControl->GetKeyName() == pszControlName )
			return pControl;
		TDialogControlPtr pNestedControl = pControl->FindControl( pszControlName, type );
		if( pNestedControl )
			return pNestedControl;
	}
	return NULL;
}
