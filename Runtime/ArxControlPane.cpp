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
	TraceFmt( _T("> CArxControlPane::CArxControlPane() [this = %p]\r\n"), this );
}

CArxControlPane::CArxControlPane( TDclFormPtr pSourceForm, CWnd* pHostDlg )
: CControlPane( pSourceForm, pHostDlg )
{
	TraceFmt( _T("> CArxControlPane::CArxControlPane(%s [%p], %s [HWND: %p]) [this: %p]\r\n"),
						(LPCTSTR)pSourceForm->GetKeyPath(), (CDclFormObject*)pSourceForm,
						(LPCTSTR)CString(pHostDlg->GetRuntimeClass()->m_lpszClassName),
						pHostDlg->m_hWnd, this );
}

CArxControlPane::~CArxControlPane()
{
	TraceFmt( _T("< CArxControlPane::~CArxControlPane() [this: %p]\r\n"), this );
}


void CArxControlPane::SetGlobalLispSymbols( bool bResetToNil /*= false*/ ) const
{
	for( TDialogControls::const_iterator iter = mControls.begin(); iter != mControls.end(); ++iter )
	{
		const CArxControlServices* pArxServices = (*iter)->GetArxServices();
		assert( pArxServices != NULL );
		pArxServices->SetLispSymbol( bResetToNil );
		std::list< const CControlPane* > listChildren;
		if( (*iter)->GetChildPanes( listChildren ) )
		{
			std::list< const CControlPane* >::const_iterator iterChild = listChildren.begin();
			for( ; iterChild != listChildren.end(); ++iterChild )
				(*iterChild)->SetGlobalLispSymbols( bResetToNil );
		}
	}
}

TDialogControlPtr CArxControlPane::CreateNewDialogControl( TDclControlPtr pTemplate,
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
			((CAxContainerCtrl*)&*(pControl->GetControlWnd()))->CloseWindow();
	}
	CControlPane::CleanUpControls();
}

bool CArxControlPane::FindControl( HWND hwndControl, /*out*/ CString& sControlName ) const
{
	if (!hwndControl)
		return false;
	for( TDialogControls::const_iterator iter = mControls.begin(); iter != mControls.end(); ++iter )
	{
		if( (*iter)->GetControlWnd()->m_hWnd == hwndControl )
		{
			sControlName = (*iter)->GetKeyName();
			return true;
		}
		std::list< const CControlPane* > listChildren;
		if( (*iter)->GetChildPanes( listChildren ) )
		{
			std::list< const CControlPane* >::const_iterator iterChild = listChildren.begin();
			for( ; iterChild != listChildren.end(); ++iterChild )
			{
				if( (*iterChild)->FindControl( hwndControl, sControlName ) )
					return true;
			}
		}
	}
	return false;
}

TDialogControlPtr CArxControlPane::FindControl( HWND hwndControl ) const
{
	if (!hwndControl)
		return false;
	for( TDialogControls::const_iterator iter = mControls.begin(); iter != mControls.end(); ++iter )
	{
		if( (*iter)->GetControlWnd()->m_hWnd == hwndControl )
			return (*iter);
		std::list< const CControlPane* > listChildren;
		if( (*iter)->GetChildPanes( listChildren ) )
		{
			std::list< const CControlPane* >::const_iterator iterChild = listChildren.begin();
			for( ; iterChild != listChildren.end(); ++iterChild )
			{
				TDialogControlPtr pControl = (*iterChild)->FindControl( hwndControl );
				if( pControl )
					return pControl;
			}
		}
	}
	return NULL;
}

TDialogControlPtr CArxControlPane::FindControl( LPCTSTR pszControlName, ControlType type /*= CtlInvalid*/ ) const
{
	for( TDialogControls::const_iterator iter = mControls.begin(); iter != mControls.end(); ++iter )
	{
		if( (type == CtlInvalid || (*iter)->GetControlType() == type) && (*iter)->GetKeyName() == pszControlName )
			return (*iter);
		std::list< const CControlPane* > listChildren;
		if( (*iter)->GetChildPanes( listChildren ) )
		{
			std::list< const CControlPane* >::const_iterator iterChild = listChildren.begin();
			for( ; iterChild != listChildren.end(); ++iterChild )
			{
				TDialogControlPtr pControl = (*iterChild)->FindControl( pszControlName, type );
				if( pControl )
					return pControl;
			}
		}
	}
	return NULL;
}
