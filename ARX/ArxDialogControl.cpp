// ArxDialogControl.cpp : implementation file
//

#include "stdafx.h"
#include "ArxDialogControl.h"
#include "DclControlObject.h"
#include "PropertyIds.h"
#include "InvokeMethod.h"


/////////////////////////////////////////////////////////////////////////////
// CArxDialogControl

CArxDialogControl::CArxDialogControl( CDclControlObject *pControl, CWnd *pWnd )
: CDialogControl( pControl, pWnd )
{
	msLispSymbolName = mpControl->GetStrProperty( nGlobalVarName );
	//if( msLispSymbolName.IsEmpty() )
	//	msLispSymbolName = GetKeyPath();
	CreateGlobalVariables();
}

CArxDialogControl::~CArxDialogControl()
{
	if( !msLispSymbolName.IsEmpty() )
	{
		resbuf rbNil = {NULL, RTNIL};
		acedPutSym( msLispSymbolName, &rbNil );
	}
}

void CArxDialogControl::CreateGlobalVariables() const
{
	if( !msLispSymbolName.IsEmpty() )
		SetVariable( msLispSymbolName, (long)mpControl );
}
