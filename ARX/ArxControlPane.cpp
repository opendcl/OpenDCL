// ArxControlPane.cpp : implementation file
//

#include "stdafx.h"
#include "ArxControlPane.h"
#include "ArxDialogControl.h"
#include "DclControlObject.h"
#include "AxContainerCtrl.h"
#include "ArxProject.h"


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
		CArxControlServices* pArxServices = (*iter)->GetArxServices();
		assert( pArxServices != NULL );
		pArxServices->SetLispSymbol( bResetToNil );
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
			((CAxContainerCtrl*)&*(pControl->GetControl()))->CloseWindow();
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
