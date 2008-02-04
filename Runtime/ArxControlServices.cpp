// ArxControlServices.cpp : implementation file
//

#include "stdafx.h"
#include "ArxControlServices.h"
#include "DialogControl.h"
#include "ArxWorkspace.h"


/////////////////////////////////////////////////////////////////////////////
// CArxControlServices

CArxControlServices::CArxControlServices( CDialogControl* pDlgControl )
: mpDlgControl( pDlgControl )
, msLispSymbolName( pDlgControl->GetTemplate()->GetVarName() )
{
	SetLispSymbol();
}

CArxControlServices::~CArxControlServices()
{
	SetLispSymbol( true );
}

void CArxControlServices::SetLispSymbol( bool bResetToNil /*= false*/ ) const
{
	if( msLispSymbolName.IsEmpty() )
		return;
	if( !bResetToNil )
		theArxWorkspace.SetLispSymbol( msLispSymbolName, (UINT_PTR)(const CDclControlObject*)mpDlgControl->GetTemplate() );
	else
		theArxWorkspace.ResetLispSymbol( msLispSymbolName );
}
