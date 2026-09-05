// ControlName.cpp : implementation file
//

#include "stdafx.h"
#include "ControlName.h"
#include "Resource.h"
#include "ControlTypes.h"
#include "Workspace.h"
#include "DclControlTemplate.h"
#include "AxTypeUtils.h"

CString GetControlDisplayName( ControlType type )
{
	switch (type)
	{
	case _CtlForm: return theWorkspace.LoadResourceString(IDS_DCLFORM);
	case CtlActiveX: return theWorkspace.LoadResourceString(IDS_ACTIVEX);
	case CtlAngleSlider: return theWorkspace.LoadResourceString(IDS_ANGLESLIDER);
	case CtlAnimation: return theWorkspace.LoadResourceString(IDS_ANIMATION);
	case CtlBlockList: return theWorkspace.LoadResourceString(IDS_BLOCKLIST);
	case CtlBlockView: return theWorkspace.LoadResourceString(IDS_BLOCKVIEW);
	case CtlCalendar: return theWorkspace.LoadResourceString(IDS_CALENDAR);
	case CtlCheckBox: return theWorkspace.LoadResourceString(IDS_CHECKBOX);
	case CtlComboBox: return theWorkspace.LoadResourceString(IDS_COMBOBOX);
	case CtlDwgList: return theWorkspace.LoadResourceString(IDS_DWGLIST);
	case CtlDwgPreview: return theWorkspace.LoadResourceString(IDS_DWGPREVIEW);
	case CtlFileExplorer: return theWorkspace.LoadResourceString(IDS_FILEEXPLORER);
	case CtlFrame: return theWorkspace.LoadResourceString(IDS_FRAME);
	case CtlGraphicButton: return theWorkspace.LoadResourceString(IDS_GRAPHICBUTTON);
	case CtlGrid: return theWorkspace.LoadResourceString(IDS_GRID);
	case CtlHatch: return theWorkspace.LoadResourceString(IDS_HATCH);
	case CtlHtml: return theWorkspace.LoadResourceString(IDS_HTML);
	case CtlImageComboBox: return theWorkspace.LoadResourceString(IDS_IMAGECOMBOBOX);
	case CtlLabel: return theWorkspace.LoadResourceString(IDS_LABEL);
	case CtlListBox: return theWorkspace.LoadResourceString(IDS_LISTBOX);
	case CtlListView: return theWorkspace.LoadResourceString(IDS_LISTVIEW);
	case CtlOptionButton: return theWorkspace.LoadResourceString(IDS_OPTIONBUTTON);
	case CtlOptionList: return theWorkspace.LoadResourceString(IDS_OPTIONLIST);
	case CtlPictureBox: return theWorkspace.LoadResourceString(IDS_PICTUREBOX);
	case CtlProgressBar: return theWorkspace.LoadResourceString(IDS_PROGRESSBAR);
	case CtlRectangle: return theWorkspace.LoadResourceString(IDS_RECTANGLE);
	case CtlScrollBar: return theWorkspace.LoadResourceString(IDS_SCROLLBAR);
	case CtlSlideView: return theWorkspace.LoadResourceString(IDS_SLIDEVIEW);
	case CtlSpinButton: return theWorkspace.LoadResourceString(IDS_SPINBUTTON);
	case CtlSplitter: return theWorkspace.LoadResourceString(IDS_SPLITTER);
	case CtlSlider: return theWorkspace.LoadResourceString(IDS_SLIDER);
	case CtlTabStrip: return theWorkspace.LoadResourceString(IDS_TABSTRIP);
	case CtlTextBox: return theWorkspace.LoadResourceString(IDS_TEXTBOX);
	case CtlTextButton: return theWorkspace.LoadResourceString(IDS_TEXTBUTTON);
	case CtlTree: return theWorkspace.LoadResourceString(IDS_TREE);
	case CtlHyperlink: return theWorkspace.LoadResourceString(IDS_HYPERLINK);
	}
	return _T("Custom");
}

CString GetControlDisplayName( TDclControlPtr pDclControl )
{
	if( !pDclControl )
		return CString();
	switch( pDclControl->GetType() )
	{
	case CtlActiveX:
		{
			TAxCtrlInitInfoPtr pAxCtrlInitInfo = pDclControl->GetAxCtrlInitInfo();
			if( pAxCtrlInitInfo )
				return pAxCtrlInitInfo->GetDisplayName();
		}
		break;
	}
	return GetControlDisplayName( pDclControl->GetType() );
}

CString GetControlDisplayName( CLSID clsid )
{
	CString sName = GetAxShortTypeName( clsid );
	if( !sName.IsEmpty() )
		return sName;
	return _T("AxObject");
}

CString GetControlSimpleName( ControlType type )
{
	CStringW sDisplayNameW = CStringW( GetControlDisplayName( type ) );
	int cchRaw = sDisplayNameW.GetLength();
	CStringA sDisplayNameA;
	int cchTrans = WideCharToMultiByte( CP_ACP, 0, sDisplayNameW, cchRaw, sDisplayNameA.GetBuffer( cchRaw + 1 ), cchRaw + 1, NULL, NULL );
	sDisplayNameA.ReleaseBuffer( cchTrans );
	CString sDisplayName = CString( sDisplayNameA );
	CString sSimpleName;
	int cchDisplayName = sDisplayName.GetLength();
	for( int idx = 0; idx < cchDisplayName; ++idx )
	{
		TCHAR chThis = sDisplayName.GetAt( idx );
		if( IsCharAlpha( chThis ) )
			sSimpleName += chThis;
	}
	if( !sSimpleName.IsEmpty() )
		return sSimpleName;
	return GetControlApiName( type );
}

CString GetControlSimpleName( CLSID clsid )
{
	static const TCHAR szAllowedChars[] = _T("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_-");
	CString sApiName;
	CString sDisplayName = GetControlDisplayName( clsid );
	while( !sDisplayName.IsEmpty() )
	{
		CString sApiNamePart = sDisplayName.SpanIncluding( szAllowedChars );
		sDisplayName = sDisplayName.Mid( sApiNamePart.GetLength() + sDisplayName.SpanExcluding( szAllowedChars ).GetLength() );
		sApiName += sApiNamePart;
	}
	return sApiName;
}

CString GetControlApiName( TDclControlPtr pDclControl )
{
	if( !pDclControl )
		return CString();
	return GetControlApiName( pDclControl->GetType() );
}
