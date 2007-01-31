// ControlName.cpp : implementation file
//

#include "stdafx.h"
#include "ControlName.h"
#include "Resource.h"
#include "ControlTypes.h"
#include "Workspace.h"


CString GetControlName(int nControl)
{
	CString sCtrlDesc;
	switch (nControl)
	{
	case CtlLabel:
		sCtrlDesc = theWorkspace.LoadResourceString(IDS_LABEL);
		break;
	case CtlStdButton:
		sCtrlDesc = theWorkspace.LoadResourceString(IDS_TEXTBUTTON);
		break;
	case CtlGraphicButton:
		sCtrlDesc = theWorkspace.LoadResourceString(IDS_GRAPHICBUTTON);
		break;
	case CtlFrame:
		sCtrlDesc = theWorkspace.LoadResourceString(IDS_FRAME);
		break;
	case CtlTextBox:
		sCtrlDesc = theWorkspace.LoadResourceString(IDS_TEXTBOX);
		break;
	case CtlCheckBox:
		sCtrlDesc = theWorkspace.LoadResourceString(IDS_CHECKBOX);
		break;		
	case CtlOptionButton:
		sCtrlDesc = theWorkspace.LoadResourceString(IDS_OPTION);
		break;
	case CtlComboBox:
		sCtrlDesc = theWorkspace.LoadResourceString(IDS_COMBOBOX);
		break;
	case CtlListBox:
		sCtrlDesc = theWorkspace.LoadResourceString(IDS_LISTBOX);
		break;
	case CtlScrollBar:
		sCtrlDesc = theWorkspace.LoadResourceString(IDS_SCROLLBAR);
		break;
	case CtlSlider:
		sCtrlDesc = theWorkspace.LoadResourceString(IDS_SLIDERBAR);
		break;
	case CtlPictureBox:
		sCtrlDesc = theWorkspace.LoadResourceString(IDS_PICTUREBOX);
		break;
	case CtlTabStrip:
		sCtrlDesc = theWorkspace.LoadResourceString(IDS_TABS);
		break;
	case CtlMonth:
		sCtrlDesc = theWorkspace.LoadResourceString(IDS_MONTHCAL);
		break;
	case CtlTree:
		sCtrlDesc = theWorkspace.LoadResourceString(IDS_TREE);
		break;
	case Ctl3DRect:
		sCtrlDesc = theWorkspace.LoadResourceString(IDS_RECT);
		break;
	case CtlProgress:
		sCtrlDesc = theWorkspace.LoadResourceString(IDS_PROGRESSBAR);
		break;		
	case CtlSpinButton:
		sCtrlDesc = theWorkspace.LoadResourceString(IDS_SPINBUTTON);
		break;
	case CtlStaticURL:
		sCtrlDesc = theWorkspace.LoadResourceString(IDS_URL);
		break;
	case CtlRoundSlider:
		sCtrlDesc = theWorkspace.LoadResourceString(IDS_ROUNDSLIDER);
		break;
	case CtlBlockView:
		sCtrlDesc = theWorkspace.LoadResourceString(IDS_BLOCKVIEW);
		break;
	case CtlSlideView:
		sCtrlDesc = theWorkspace.LoadResourceString(IDS_SLIDEVIEW);
		break;
	case CtlHtmlCtrl:
		sCtrlDesc = theWorkspace.LoadResourceString(IDS_HTML);
		break;
	case CtlDwgPreview:
		sCtrlDesc = theWorkspace.LoadResourceString(IDS_DWGPREVIEW);
		break;
	case CtlListView:
		sCtrlDesc = theWorkspace.LoadResourceString(IDS_LISTVIEW);
		break;
	case CtlBlockList:
		sCtrlDesc = theWorkspace.LoadResourceString(IDS_BLOCKLIST);
		break;
	case CtlOptionList:
		sCtrlDesc = theWorkspace.LoadResourceString(IDS_OPTIONLIST);
		break;
	case CtlDwgList:
		sCtrlDesc = theWorkspace.LoadResourceString(IDS_DWGLIST);
	    break;
	case CtlAnimate:
		sCtrlDesc = theWorkspace.LoadResourceString(IDS_ANIMATION);
	    break;
	case CtlImageComboBox:
		sCtrlDesc = theWorkspace.LoadResourceString(IDS_IMAGECOMBOBOX);
	    break;
	case CtlGrid:
		sCtrlDesc = theWorkspace.LoadResourceString(IDS_GRID);
	    break;
	case CtlSplitter:
		sCtrlDesc = theWorkspace.LoadResourceString(IDS_SPLITTER);
	    break;
	case CtlHatch:
		sCtrlDesc = theWorkspace.LoadResourceString(IDS_HATCH);
	    break;
	case CtlFileDlgCtrl:
		sCtrlDesc = theWorkspace.LoadResourceString(IDS_FILEDLG2);
	    break;
	}
	return sCtrlDesc;
}
