// DialogControl.cpp : implementation file
//

#include "stdafx.h"
#include "DialogControl.h"
#include "DclControlObject.h"


/////////////////////////////////////////////////////////////////////////////
// CDialogControl

CDialogControl::CDialogControl( CDclControlObject* pControl, CWnd* pWnd )
: mpControl( pControl )
, mpWnd( pWnd )
{
}

CDialogControl::~CDialogControl()
{
}

long CDialogControl::GetDclControlType() const
{
	return mpControl? mpControl->GetType() : 0;
}

CString CDialogControl::GetKeyName() const
{
	if( mpControl )
		return mpControl->GetKeyName();
	return CString();
}

CString CDialogControl::GetKeyPath() const
{
	if( mpControl )
		return mpControl->GetKeyPath();
	return CString();
}
