// ArxControlServices.cpp : implementation file
//

#include "stdafx.h"
#include "ArxControlServices.h"
#include "ArxWorkspace.h"


/////////////////////////////////////////////////////////////////////////////
// CArxControlServices

CArxControlServices::CArxControlServices( CDclControlObject* pTemplate )
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
		theArxWorkspace.SetLispSymbol( msLispSymbolName, (long)mpTemplate );
	else
		theArxWorkspace.ResetLispSymbol( msLispSymbolName );
}
