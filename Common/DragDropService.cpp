// DragDropService.cpp : implementation file
//

#include "StdAfx.h"
#include "DragDropService.h"


/////////////////////////////////////////////////////////////////////////////
// CDragDropService

CDragDropService::CDragDropService()
{
}

CDragDropService::~CDragDropService()
{
}

UINT CDragDropService::GetDclControlClipboardFormat()
{
	static const UINT CF_DclControl = RegisterClipboardFormat( _T("OpenDCL.DclControl") );
	return CF_DclControl;
}
