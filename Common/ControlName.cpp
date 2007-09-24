// ControlName.cpp : implementation file
//

#include "stdafx.h"
#include "ControlName.h"
#include "SharedRes.h"
#include "ControlTypes.h"
#include "Workspace.h"
#include "DclControlObject.h"


CString GetControlName( ControlType type )
{
	switch (type)
	{
	case CtlForm: return theWorkspace.LoadResourceString(IDS_DCLFORM);
	case CtlLabel: return theWorkspace.LoadResourceString(IDS_LABEL);
	case CtlStdButton: return theWorkspace.LoadResourceString(IDS_TEXTBUTTON);
	case CtlGraphicButton: return theWorkspace.LoadResourceString(IDS_GRAPHICBUTTON);
	case CtlFrame: return theWorkspace.LoadResourceString(IDS_FRAME);
	case CtlTextBox: return theWorkspace.LoadResourceString(IDS_TEXTBOX);
	case CtlCheckBox: return theWorkspace.LoadResourceString(IDS_CHECKBOX);
	case CtlOptionButton: return theWorkspace.LoadResourceString(IDS_OPTION);
	case CtlComboBox: return theWorkspace.LoadResourceString(IDS_COMBOBOX);
	case CtlListBox: return theWorkspace.LoadResourceString(IDS_LISTBOX);
	case CtlScrollBar: return theWorkspace.LoadResourceString(IDS_SCROLLBAR);
	case CtlSlider: return theWorkspace.LoadResourceString(IDS_SLIDERBAR);
	case CtlPictureBox: return theWorkspace.LoadResourceString(IDS_PICTUREBOX);
	case CtlTabStrip: return theWorkspace.LoadResourceString(IDS_TABS);
	case CtlMonth: return theWorkspace.LoadResourceString(IDS_MONTHCAL);
	case CtlTree: return theWorkspace.LoadResourceString(IDS_TREE);
	case Ctl3DRect: return theWorkspace.LoadResourceString(IDS_RECT);
	case CtlProgress: return theWorkspace.LoadResourceString(IDS_PROGRESSBAR);
	case CtlSpinButton: return theWorkspace.LoadResourceString(IDS_SPINBUTTON);
	case CtlStaticURL: return theWorkspace.LoadResourceString(IDS_URL);
	case CtlRoundSlider: return theWorkspace.LoadResourceString(IDS_ROUNDSLIDER);
	case CtlBlockView: return theWorkspace.LoadResourceString(IDS_BLOCKVIEW);
	case CtlSlideView: return theWorkspace.LoadResourceString(IDS_SLIDEVIEW);
	case CtlHtmlCtrl: return theWorkspace.LoadResourceString(IDS_HTML);
	case CtlDwgPreview: return theWorkspace.LoadResourceString(IDS_DWGPREVIEW);
	case CtlListView: return theWorkspace.LoadResourceString(IDS_LISTVIEW);
	case CtlBlockList: return theWorkspace.LoadResourceString(IDS_BLOCKLIST);
	case CtlOptionList: return theWorkspace.LoadResourceString(IDS_OPTIONLIST);
	case CtlDwgList: return theWorkspace.LoadResourceString(IDS_DWGLIST);
	case CtlAnimate: return theWorkspace.LoadResourceString(IDS_ANIMATION);
	case CtlImageComboBox: return theWorkspace.LoadResourceString(IDS_IMAGECOMBOBOX);
	case CtlGrid: return theWorkspace.LoadResourceString(IDS_GRID);
	case CtlSplitter: return theWorkspace.LoadResourceString(IDS_SPLITTER);
	case CtlHatch: return theWorkspace.LoadResourceString(IDS_HATCH);
	case CtlFileDlgCtrl: return theWorkspace.LoadResourceString(IDS_FILEDLGCTRL);
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
