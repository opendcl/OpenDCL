// ArxControlServices.cpp : implementation file
//

#include "stdafx.h"
#include "ArxControlServices.h"
#include "ArxWorkspace.h"


/////////////////////////////////////////////////////////////////////////////
// CArxControlServices

CArxControlServices::CArxControlServices( TDclControlPtr pTemplate )
: mpTemplate( pTemplate )
, msLispSymbolName( pTemplate->GetVarName() )
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
		theArxWorkspace.SetLispSymbol( msLispSymbolName, (UINT_PTR)(const CDclControlObject*)mpTemplate );
	else
		theArxWorkspace.ResetLispSymbol( msLispSymbolName );
}
