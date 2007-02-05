// DialogControl.cpp : implementation file
//

#include "stdafx.h"
#include "DialogControl.h"
#include "DclControlObject.h"


/////////////////////////////////////////////////////////////////////////////
// CDialogControl

CDialogControl::CDialogControl( CDclControlObject* pTemplate, RefCountedPtr< CWnd > pControl )
: mpTemplate( pTemplate )
, mpControl( pControl )
{
}

CDialogControl::~CDialogControl()
{
}

long CDialogControl::GetDclControlType() const
{
	return mpTemplate? mpTemplate->GetType() : 0;
}

CString CDialogControl::GetKeyName() const
{
	if( mpTemplate )
		return mpTemplate->GetKeyName();
	return CString();
}

CString CDialogControl::GetKeyPath() const
{
	if( mpTemplate )
		return mpTemplate->GetKeyPath();
	return CString();
}
