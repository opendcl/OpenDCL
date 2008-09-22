// ControlType.h : header file
//

#pragma once


enum ControlType
{
	_CtlInvalid = 0,
	_CtlForm = 1,
	CtlLabel = 2,
	CtlTextButton = 3,
	CtlGraphicButton = 4,
	CtlFrame = 5,
	CtlTextBox = 6,
	CtlCheckBox = 7,
	CtlOptionButton = 8,
	CtlComboBox = 9,
	CtlListBox = 10,
	CtlScrollBar = 11,
	CtlStraightSlider = 12,
	CtlPictureBox = 13,
	CtlTabStrip = 14,
	CtlCalendar = 15,
	CtlImageTree = 16,
	CtlRectangle = 17,
	CtlProgressBar = 18,
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
	CtlAnimation = 31,
	CtlImageComboBox = 32,
	CtlGrid = 33,
	CtlSplitter = 34,
	CtlHatch = 35,
	CtlFileExplorer = 36,
	_CtlFirst = 2,
	_CtlMax = 36,
	_CtlOldFileExplorer = 100, //old value is still accepted when reading older files
};
