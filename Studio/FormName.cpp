// FormName.cpp : implementation file
//

#include "stdafx.h"
#include "FormName.h"
#include "Resource.h"
#include "FormTypes.h"
#include "Workspace.h"
#include "DclFormTemplate.h"


CString GetFormDisplayName( FormType type )
{
	switch (type)
	{
	case FrmModalDlg: return theWorkspace.LoadResourceString( IDS_MODALFORM );
	case FrmModelessDlg: return theWorkspace.LoadResourceString( IDS_MODELESSFORM );
	case FrmControlBar: return theWorkspace.LoadResourceString( IDS_CONTROLBAR );
	case FrmOptionsTab: return theWorkspace.LoadResourceString( IDS_OPTIONSTAB );
	case FrmTabPage: return _T("Tab Page");
	case FrmFileDlg: return theWorkspace.LoadResourceString( IDS_FILEDLG );
	case FrmPaletteDlg: return theWorkspace.LoadResourceString( IDS_PALETTEFORM );
	}
	return _T("Unknown");
}


CString GetFormDisplayName( TDclFormPtr pForm )
{
	if( !pForm )
		return NULL;
	return GetFormDisplayName( pForm->GetType() );
}


CString GetFormApiName( FormType type )
{
	switch (type)
	{
	case FrmModalDlg: return _T("ModalDialog");
	case FrmModelessDlg: return _T("ModelessDialog");
	case FrmControlBar: return _T("ControlBar");
	case FrmOptionsTab: return _T("OptionsTab");
	case FrmTabPage: return _T("TabPage");
	case FrmFileDlg: return _T("FileDialog");
	case FrmPaletteDlg: return _T("Palette");
	}
	return _T("Unknown");
}


CString GetFormApiName( TDclFormPtr pForm )
{
	if( !pForm )
		return NULL;
	return GetFormApiName( pForm->GetType() );
}
