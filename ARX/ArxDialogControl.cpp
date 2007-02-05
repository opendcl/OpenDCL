// ArxDialogControl.cpp : implementation file
//

#include "stdafx.h"
#include "ArxDialogControl.h"
#include "DclControlObject.h"
#include "PropertyIds.h"
#include "InvokeMethod.h"


/////////////////////////////////////////////////////////////////////////////
// CArxDialogControl

CArxDialogControl::CArxDialogControl( CDclControlObject* pTemplate, RefCountedPtr< CWnd > pControl )
: CDialogControl( pTemplate, pControl )
{
	pTemplate->SetControlInstance( this );
	msLispSymbolName = mpTemplate->GetStrProperty( nGlobalVarName );
	CreateGlobalVariables();
}

CArxDialogControl::~CArxDialogControl()
{
	if( !msLispSymbolName.IsEmpty() )
	{
		resbuf rbNil = {NULL, RTNIL};
		acedPutSym( msLispSymbolName, &rbNil );
	}
	mpTemplate->SetControlInstance( NULL );
}

void CArxDialogControl::CreateGlobalVariables() const
{
	if( !msLispSymbolName.IsEmpty() )
		SetVariable( msLispSymbolName, (long)mpTemplate );
}
