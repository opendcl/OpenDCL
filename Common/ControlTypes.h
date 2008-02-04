// ControlType.h : header file
//

#pragma once


enum ControlType
{
	_CtlInvalid = 0,
	_CtlForm = 1,
	CtlLabel = 2,
	CtlStdButton = 3,
	CtlGraphicButton = 4,
	CtlFrame = 5,
	CtlTextBox = 6,
	CtlCheckBox = 7,
	CtlOptionButton = 8,
	CtlComboBox = 9,
	CtlListBox = 10,
	CtlScrollBar = 11,
	CtlSlider = 12,
	CtlPictureBox = 13,
	CtlTabStrip = 14,
	CtlMonth = 15,
	CtlTree = 16,
	CtlRectangle = 17,
	CtlProgress = 18,
	CtlSpinButton = 19,
	CtlUrlLink = 20,
	CtlAngleSlider = 21,
	CtlBlockView = 22,
	CtlSlideView = 23,
	CtlHtmlCtrl = 24,
	CtlDwgPreview = 25,
	CtlListView = 26,
	CtlBlockList = 27,
	CtlOptionList = 28,
	CtlActiveX = 29,
	CtlDwgList = 30,
	CtlAnimate = 31,
	CtlImageComboBox = 32,
	CtlGrid = 33,
	CtlSplitter = 34,
	CtlHatch = 35,
	CtlFileDlgCtrl = 36,
	_CtlFirst = 2,
	_CtlMax = 36,
	_CtlOldFileDlgCtrl = 100, //old value is still accepted when reading older files
};


namespace EditFilter
{
	enum Type
	{
		String = 0,
		Angle = 1,
		Integer = 2,
		Numeric = 3,
		Symbol = 4,
		UpperCase = 5,
		LowerCase = 6,
		Password = 7,
		Multiline = 8,
	};
};

namespace ComboStyle
{
	enum Type
	{
		Combo = 0,
		Simple = 1,
		DropDown = 2,
		ArrowHead = 3,
		Color = 4,
		LineWeight = 5,
		PlotNames = 6,
		PlotTables = 7,
		FontDropList = 8,
		FontSimpleList = 9,
		Plotters = 10,
		PlotterPaperSizes = 11,
		DirPicker = 12,
		Layers = 13,
		Linetypes = 14,
	};
};
