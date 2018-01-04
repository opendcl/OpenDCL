// ArxControlPane.cpp : implementation file
//

#include "stdafx.h"
#include "ArxControlPane.h"
#include "ArxDialogControl.h"
#include "DclControlTemplate.h"
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
	mnDPI = (int)GetDpiForWindow( adsw_acadMainWnd() );
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
