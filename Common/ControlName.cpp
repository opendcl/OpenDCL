// ControlName.cpp : implementation file
//

#include "stdafx.h"
#include "ControlName.h"
#include "Resource.h"
#include "ControlTypes.h"
#include "Workspace.h"
#include "DclControlObject.h"


CString GetControlName( ControlType type )
{
	switch (type)
	{
	case _CtlForm: return theWorkspace.LoadResourceString(IDS_DCLFORM);
	case CtlAngleSlider: return theWorkspace.LoadResourceString(IDS_ANGLESLIDER);
	case CtlAnimation: return theWorkspace.LoadResourceString(IDS_ANIMATION);
	case CtlBlockList: return theWorkspace.LoadResourceString(IDS_BLOCKLIST);
	case CtlBlockView: return theWorkspace.LoadResourceString(IDS_BLOCKVIEW);
	case CtlCalendar: return theWorkspace.LoadResourceString(IDS_CALENDAR);
	case CtlCheckBox: return theWorkspace.LoadResourceString(IDS_CHECKBOX);
	case CtlComboBox: return theWorkspace.LoadResourceString(IDS_COMBOBOX);
	case CtlDwgList: return theWorkspace.LoadResourceString(IDS_DWGLIST);
	case CtlDwgPreview: return theWorkspace.LoadResourceString(IDS_DWGPREVIEW);
	case CtlFileExplorer: return theWorkspace.LoadResourceString(IDS_FILEDLGCTRL);
	case CtlFrame: return theWorkspace.LoadResourceString(IDS_FRAME);
	case CtlGraphicButton: return theWorkspace.LoadResourceString(IDS_GRAPHICBUTTON);
	case CtlGrid: return theWorkspace.LoadResourceString(IDS_GRID);
	case CtlHatch: return theWorkspace.LoadResourceString(IDS_HATCH);
	case CtlHtmlCtrl: return theWorkspace.LoadResourceString(IDS_HTML);
	case CtlImageComboBox: return theWorkspace.LoadResourceString(IDS_IMAGECOMBOBOX);
	case CtlLabel: return theWorkspace.LoadResourceString(IDS_LABEL);
	case CtlListBox: return theWorkspace.LoadResourceString(IDS_LISTBOX);
	case CtlListView: return theWorkspace.LoadResourceString(IDS_LISTVIEW);
	case CtlOptionButton: return theWorkspace.LoadResourceString(IDS_OPTION);
	case CtlOptionList: return theWorkspace.LoadResourceString(IDS_OPTIONLIST);
	case CtlPictureBox: return theWorkspace.LoadResourceString(IDS_PICTUREBOX);
	case CtlProgressBar: return theWorkspace.LoadResourceString(IDS_PROGRESSBAR);
	case CtlRectangle: return theWorkspace.LoadResourceString(IDS_RECTANGLE);
	case CtlScrollBar: return theWorkspace.LoadResourceString(IDS_SCROLLBAR);
	case CtlSlideView: return theWorkspace.LoadResourceString(IDS_SLIDEVIEW);
	case CtlSpinButton: return theWorkspace.LoadResourceString(IDS_SPINBUTTON);
	case CtlSplitter: return theWorkspace.LoadResourceString(IDS_SPLITTER);
	case CtlStraightSlider: return theWorkspace.LoadResourceString(IDS_SLIDERBAR);
	case CtlTabStrip: return theWorkspace.LoadResourceString(IDS_TABS);
	case CtlTextBox: return theWorkspace.LoadResourceString(IDS_TEXTBOX);
	case CtlTextButton: return theWorkspace.LoadResourceString(IDS_TEXTBUTTON);
	case CtlImageTree: return theWorkspace.LoadResourceString(IDS_TREE);
	case CtlUrlLink: return theWorkspace.LoadResourceString(IDS_URLLINK);
	}
	return _T("Custom");
}


CString GetControlName( TDclControlPtr pTemplate )
{
	switch (pTemplate->GetType())
	{
	case CtlActiveX: return pTemplate->GetActiveXTypeName();
	}
	return GetControlName( pTemplate->GetType() );
}


CString GetControlName( CLSID clsid )
{
	// get the ProgId
	WCHAR* pwszProgID = NULL;        
	HRESULT hResult = ProgIDFromCLSID(clsid, &pwszProgID);
	if (FAILED(hResult))
		return _T("OLEControl");
	CString sName( pwszProgID );
	CoTaskMemFree(pwszProgID);
	sName.MakeReverse();
	sName = sName.Right(sName.GetLength() - sName.SpanIncluding(_T("0123456789.")).GetLength()).SpanExcluding(_T("."));
	sName.MakeReverse();
	return sName;
}
