#include "StdAfx.h"
#include "ControlApiName.h"

CString GetControlApiName(ControlType type)
{
	switch (type)
	{
	case _CtlForm: return _T("Form");
	case CtlActiveX: return _T("AxControl");
	case CtlAngleSlider: return _T("AngleSlider");
	case CtlAnimation: return _T("Animation");
	case CtlBlockList: return _T("BlockList");
	case CtlBlockView: return _T("BlockView");
	case CtlCalendar: return _T("Calendar");
	case CtlCheckBox: return _T("CheckBox");
	case CtlComboBox: return _T("ComboBox");
	case CtlDwgList: return _T("DwgList");
	case CtlDwgPreview: return _T("DwgPreview");
	case CtlFileExplorer: return _T("FileExplorer");
	case CtlFrame: return _T("Frame");
	case CtlGraphicButton: return _T("GraphicButton");
	case CtlGrid: return _T("Grid");
	case CtlHatch: return _T("Hatch");
	case CtlHtml: return _T("Html");
	case CtlHyperlink: return _T("Hyperlink");
	case CtlImageComboBox: return _T("ImageComboBox");
	case CtlLabel: return _T("Label");
	case CtlListBox: return _T("ListBox");
	case CtlListView: return _T("ListView");
	case CtlOptionButton: return _T("OptionButton");
	case CtlOptionList: return _T("OptionList");
	case CtlPictureBox: return _T("PictureBox");
	case CtlProgressBar: return _T("ProgressBar");
	case CtlRectangle: return _T("Rectangle");
	case CtlScrollBar: return _T("ScrollBar");
	case CtlSlideView: return _T("SlideView");
	case CtlSpinButton: return _T("SpinButton");
	case CtlSplitter: return _T("Splitter");
	case CtlSlider: return _T("Slider");
	case CtlTabStrip: return _T("TabStrip");
	case CtlTextBox: return _T("TextBox");
	case CtlTextButton: return _T("TextButton");
	case CtlTree: return _T("Tree");
	}
	return _T("Custom");
}
