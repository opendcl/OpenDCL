#include "stdafx.h"
#include "MethodLexicon.h"

LPCTSTR sFilesDir = _T("Files_Dir");
LPCTSTR sAnimate_Load =	_T("Animate_Load");
LPCTSTR sAnimate_Seek =	_T("Animate_Seek");
LPCTSTR sAnimate_Stop =	_T("Animate_Stop");
LPCTSTR sAnimate_Close = _T("Animate_Close");

LPCTSTR sHatch_Clear = _T("Hatch_Clear");
LPCTSTR sHatch_SetPattern = _T("Hatch_SetPattern");
LPCTSTR sShowToolTip = _T("Control_ShowToolTip");


LPCTSTR sGrid_Cell_TextBox = _T("Grid_ShowTextBox");
LPCTSTR sGrid_Cell_ComboBox = _T("Grid_ShowComboBox");
LPCTSTR sGrid_Cell_ImageComboBox = _T("Grid_ShowImageComboBox");
LPCTSTR sGrid_Cell_EllipsesButton =	_T("Grid_ShowEllipsesButton");
LPCTSTR sGrid_Cell_PickButton =	_T("Grid_ShowPickButton");
LPCTSTR sGrid_Cell_SetStyle = _T("Grid_SetItemStyle");
LPCTSTR sGrid_SelectCell = _T("Grid_SelCurCell");
LPCTSTR sGrid_Cell_DoCellDlg = _T("Grid_ShowItemDlg");
LPCTSTR sGrid_Cell_SetDropList = _T("Grid_SetItemDropList");
LPCTSTR sGrid_HitPointTest = _T("Grid_HitPointTest");
LPCTSTR sGrid_SelCurRow = _T("Grid_SelCurRow");

// these are the Grid methods 
LPCTSTR sGrid_GetColumnImage = _T("Grid_GetColumnImage");
LPCTSTR sGrid_SetColumnImage = _T("Grid_SetColumnImage");
LPCTSTR sGrid_GetCount = _T("Grid_GetRowCount");
LPCTSTR sGrid_GetColumnCount = _T("Grid_GetColumnCount");
LPCTSTR sGrid_Cell_ToggleImages = _T("Grid_ToggleItemImages");
LPCTSTR sGrid_GetCheck = _T("Grid_GetItemCheck");
LPCTSTR sGrid_SetCheck = _T("Grid_SetItemCheck");

LPCTSTR sGrid_AddColumn = _T("Grid_AddColumns");
LPCTSTR sGrid_AddRow = _T("Grid_AddRow");
LPCTSTR sGrid_AddString = _T("Grid_AddString");
LPCTSTR sGrid_CalcColumnWidth = _T("Grid_CalcColWidth");
LPCTSTR sGrid_Clear = _T("Grid_Clear");
LPCTSTR sGrid_DeleteItem = _T("Grid_DeleteRow");
LPCTSTR sGrid_DeleteItems = _T("Grid_DeleteRows");
LPCTSTR sGrid_DeleteColumns = _T("Grid_DeleteColumns");
LPCTSTR sGrid_DeleteColumn = _T("Grid_DeleteColumn");
LPCTSTR sGrid_FillGrid = _T("Grid_FillList");
LPCTSTR sGrid_GetColWidth = _T("Grid_GetColWidth");
LPCTSTR sGrid_GetItemData = _T("Grid_GetItemData");
LPCTSTR sGrid_GetItemImage = _T("Grid_GetItemImage");
LPCTSTR sGrid_GetItemText = _T("Grid_GetItemText");
LPCTSTR sGrid_GetSelectedCell= _T("Grid_GetCurSel");
LPCTSTR sGrid_InsertRow = _T("Grid_InsertRow");
LPCTSTR sGrid_InsertString = _T("Grid_InsertString");
LPCTSTR sGrid_SetColWidth = _T("Grid_SetColWidth");
LPCTSTR sGrid_SetCurSel = _T("Grid_SetCurSel");
LPCTSTR sGrid_SetCurRow = _T("Grid_SetCurRow");
LPCTSTR sGrid_SetItemData = _T("Grid_SetItemData");
LPCTSTR sGrid_SetItemText = _T("Grid_SetItemText");
LPCTSTR sGrid_SetItemImage = _T("Grid_SetItemImage");
LPCTSTR sGrid_CancelLabelEdit = _T("Grid_CancelItemEdit");
LPCTSTR sGrid_SortTextItems = _T("Grid_SortTextItems");
LPCTSTR sGrid_SortNumericItems = _T("Grid_SortNumericItems");
LPCTSTR sGrid_GetRow = _T("Grid_GetRowItems");
LPCTSTR sGrid_GetColumn = _T("Grid_GetColumnItems");

LPCTSTR sComboBoxEx_AddString		=	_T("ImageComboBox_AddString");
LPCTSTR sComboBoxEx_GetLBText		=	_T("ImageComboBox_GetLBText");
LPCTSTR sComboBoxEx_SetItem			=	_T("ImageComboBox_SetItem");
LPCTSTR sComboBoxEx_GetItem			=	_T("ImageComboBox_GetItem");
LPCTSTR sComboBoxEx_Clear			=	_T("ImageComboBox_Clear");
LPCTSTR sComboBoxEx_GetCurSel		=	_T("ImageComboBox_GetCurSel");
LPCTSTR sComboBoxEx_GetCount		=	_T("ImageComboBox_GetCount");
LPCTSTR sComboBoxEx_DeleteString	=	_T("ImageComboBox_DeleteString");
LPCTSTR sComboBoxEx_SelectString	=	_T("ImageComboBox_SelectString");
LPCTSTR sComboBoxEx_FindString		=	_T("ImageComboBox_FindString");
LPCTSTR sComboBoxEx_FindStringExact =	_T("ImageComboBox_FindStringExact");
LPCTSTR sComboBoxEx_SetCurSel		=	_T("ImageComboBox_SetCurSel");
LPCTSTR sComboBoxEx_GetEditSel		=	_T("ImageComboBox_GetEditSel");
LPCTSTR sComboBoxEx_SetEditSel		=	_T("ImageComboBox_SetEditSel");
LPCTSTR sComboBoxEx_SetItemData		=	_T("ImageComboBox_SetItemData");
LPCTSTR sComboBoxEx_GetItemData		=	_T("ImageComboBox_GetItemData");
LPCTSTR sComboBoxEx_GetTopIndex		=	_T("ImageComboBox_GetTopIndex");
LPCTSTR sComboBoxEx_SetTopIndex		=	_T("ImageComboBox_SetTopIndex");
LPCTSTR sComboBoxEx_SetDroppedWidth =	_T("ImageComboBox_SetDroppedWidth");
LPCTSTR sComboBoxEx_ClearEdit		=	_T("ImageComboBox_ClearEdit");
LPCTSTR sComboBoxEx_GetEBText		=	_T("ImageComboBox_GetTBText");
LPCTSTR sComboBoxEx_GetDroppedWidth =	_T("ImageComboBox_GetDroppedWidth");

//LPCTSTR sLoadProject = _T("LoadProject");
//LPCTSTR sShowForm = _T("Form_Show");			
//LPCTSTR sCenterForm = _T("Form_Center");
//LPCTSTR sResizeForm = _T("Form_Resize");
//LPCTSTR sHideForm = _T("Form_Hide");
//LPCTSTR sIsFormActive = _T("Form_IsActive");		
//LPCTSTR sIsFormFloating = _T("Form_IsFloating");		
//LPCTSTR sSetTitleBarText = _T("Form_SetTitleBarText");	 
//LPCTSTR sGetTitleBarText = _T("Form_GetTitleBarText");	
//LPCTSTR sCloseForm = 	  _T("Form_Close");				
//LPCTSTR sCloseAllForms =   _T("Form_CloseAll");
//LPCTSTR sGetFormSize = 	  _T("Form_GetRectangle");		
//LPCTSTR sGetFormArea = 	  _T("Form_GetControlArea");	
//LPCTSTR sEnableForm = _T("Form_Enable");
//LPCTSTR sIsFormEnabled = _T("Form_IsEnabled");
//LPCTSTR sIsFormVisible = _T("Form_IsVisible");
//LPCTSTR sGetFocus = _T("GetFocus");
//LPCTSTR sGetFormId = _T("Form_GetFormId");	
//LPCTSTR sShowMessageBox = _T("MessageBox");					
//LPCTSTR sGetScreenSize =  _T("GetScreenSize");				
//LPCTSTR sIsApplyEnabled =  _T("ConfigTab_IsApplyEnabled");	
//LPCTSTR sSetApplyEnabled =  _T("ConfigTab_SetApplyEnabled");	
//LPCTSTR sBrowseForFolder =  _T("BrowseFolder");	
//LPCTSTR sGetPictureSize =  _T("GetPictureSize");	
//LPCTSTR sSetCmdBarFocus =  _T("SetCmdBarFocus");	
//LPCTSTR sNavigateToUrl = _T("NavigateToUrl");
//LPCTSTR sActivateEmail = _T("ActivateEmail");
//LPCTSTR sControl_FlushGraphicButtons = _T("FlushGraphicButtons");
//LPCTSTR sSetDialogMinMaxSizes = _T("Form_SetDialogMinMaxSizes");
//LPCTSTR sGetVolumnSerialNumber = _T("GetHardDriveSerialNumber");
//LPCTSTR sGetDiskSize = _T("GetHardDriveSize");
//LPCTSTR sHideErrorMsgBox = _T("HideErrorMsgBox");
// this method id used to set any property of any control
LPCTSTR sSetProperty = _T("Control_SetProperty");	
// this method id used to get any property of any control
LPCTSTR sGetProperty = _T("Control_GetProperty");	
//LPCTSTR sMultiFileDlg = _T("MultiFileDialog");
LPCTSTR sSetFocus = _T("Control_SetFocus");	
LPCTSTR sZOrder = _T("Control_ZOrder");		
LPCTSTR sControl_GetCurPos = _T("Control_GetCurPos");
LPCTSTR sControl_SetPos = _T("Control_SetPos");
LPCTSTR sForceUpdateNow = _T("Control_ForceUpdateNow");
//LPCTSTR sGetBlockBoundingBox = _T("GetBlockSize");	
//LPCTSTR sGetControlHwnd = _T("Control_GetHwnd");	
//LPCTSTR sGetFormHwnd = _T("Form_GetHwnd");	
//LPCTSTR sGetVersion = _T("GetVersion");		
//LPCTSTR sGetColorValue = _T("GetColorValue");

// FileDlgCtrl methods
LPCTSTR sFileDlgGetFileName = _T("FileDlg_GetFileName");
LPCTSTR sFileDlgGetFileTitle = _T("FileDlg_GetFileTitle");
LPCTSTR sFileDlgGetPathName = _T("FileDlg_GetPathName");
LPCTSTR sFileDlgGetSelectionCount = _T("FileDlg_GetSelectionCount");
LPCTSTR sFileDlgGetFolderPath = _T("FileDlg_GetFolderPath");
LPCTSTR sFileDlgGetFileNameList = _T("FileDlg_GetFileNameList");
LPCTSTR sFileDlgSetOkButtonText = _T("FileDlg_SetOkButtonText");
LPCTSTR sFileDlgGetFileExt = _T("FileDlg_GetFileExt");
LPCTSTR sFileDlgGetFolderName = _T("FileDlg_GetFolderName");
		
// activeX methods
LPCTSTR sAxSetProperty = _T("AxControl_SetProperty");
LPCTSTR sAxGetProperty = _T("AxControl_GetProperty");
LPCTSTR sDoAxMethod = _T("AxControl_DoMethod");
LPCTSTR sAxObjectSetProperty = _T("AxObject_SetProperty");
LPCTSTR sAxObjectGetProperty = _T("AxObject_GetProperty");
LPCTSTR sObjectDoAxMethod = _T("AxObject_DoMethod");
LPCTSTR sAxGetObject = _T("AxControl_GetOleObject");
LPCTSTR sCloseAxObject = _T("AxObject_Close");
LPCTSTR sReleaseAxObject = _T("AxObject_Close");
LPCTSTR sSetAxPictureProperty = _T("AxControl_SetPicture");
LPCTSTR sSetAxObjPictureProperty = _T("AxObject_SetPicture");
LPCTSTR sSetAxColorProperty = _T("AxControl_SetColor");
LPCTSTR sSetAxObjColorProperty = _T("AxObject_SetColor");

//LPCTSTR sXTwipsToPixels = _T("XTwipsToPixels");
//LPCTSTR sYTwipsToPixels = _T("YTwipsToPixels");
//LPCTSTR sXPixelsToTwips = _T("XPixelsToTwips");
//LPCTSTR sYPixelsToTwips = _T("YPixelsToTwips");

// these are the block preview methods 
LPCTSTR sBlockView_Clear = _T("BlockView_Clear");	
LPCTSTR sBlockView_SetHighLight = _T("BlockView_SetHighLight");	
LPCTSTR sBlockView_RemoveHighLight = _T("BlockView_RemoveHighLight");	
LPCTSTR sBlockView_Zoom = _T("BlockView_Zoom");
LPCTSTR sBlockView_ViewBlock = _T("BlockView_DisplayBlock");
LPCTSTR sBlockView_ViewPaperSpace = _T("BlockView_DisplayPaperSpace");
LPCTSTR sBlockView_ViewBlockToScale = _T("BlockView_DisplayBlockToScale");
LPCTSTR sBlockView_LoadDwg = _T("BlockView_LoadDwg");	
LPCTSTR sBlockView_LoadDwgToScale = _T("BlockView_LoadDwgToScale");	
LPCTSTR sBlockView_PreLoadDwg = _T("BlockView_PreLoadDwg");
LPCTSTR sBlockView_GetBlockList = _T("BlockView_GetBlockList");
LPCTSTR sBlockView_GetBlockSize = _T("BlockView_GetBlockSize");	
LPCTSTR sBlockView_GetDwgSize = _T("BlockView_GetDwgSize");	
LPCTSTR sBlockView_RefreshBlock = _T("BlockView_RefreshBlock");
LPCTSTR sBlockView_GetViewInfo = _T("BlockView_GetViewInfo");
LPCTSTR sBlockView_SetView = _T("BlockView_SetView");

// these are the dwg preview methods 
LPCTSTR sDwgPreview_LoadDwg = _T("DwgPreview_LoadDwg");	
LPCTSTR sDwgPreview_GetDwgName = _T("DwgPreview_GetDwgName");
LPCTSTR sDwgPreview_Clear = _T("DwgPreview_Clear");	
LPCTSTR sDwgPreview_SetHighLight = _T("DwgPreview_SetHighLight");	
LPCTSTR sDwgPreview_RemoveHighLight = _T("DwgPreview_RemoveHighLight");	

// these are the list box methods 

LPCTSTR sDwgList_Dir = _T("DwgList_Dir");
LPCTSTR sDwgList_GetDir = _T("DwgList_GetDir");
LPCTSTR sDwgList_GetFileName = _T("DwgList_GetFileName");
LPCTSTR sDwgList_SetRowHeight = _T("DwgList_SetRowHeight");
LPCTSTR sDwgList_GetType = _T("DwgList_GetType");

LPCTSTR sOptionList_SetEnabled = _T("OptionList_SetEnabled");
LPCTSTR sOptionList_SetTttTitle = _T("OptionList_SetTttTitle");
LPCTSTR sOptionList_SetTttTitleLine = _T("OptionList_SetTttTitleLine");
LPCTSTR sOptionList_SetTttTitleColor = _T("OptionList_SetTttTitleColor");
LPCTSTR sOptionList_SetTttAvi = _T("OptionList_SetTttAvi");
LPCTSTR sOptionList_SetTttPicture = _T("OptionList_SetTttPicture");
LPCTSTR sOptionList_SetTttMain = _T("OptionList_SetTttMain");

LPCTSTR sOptionList_AddList = _T("OptionList_AddList");		
LPCTSTR sOptionList_AddString = _T("OptionList_AddString");		
LPCTSTR sOptionList_Clear =_T("OptionList_Clear");			
LPCTSTR sOptionList_GetText = _T("OptionList_GetText");		
LPCTSTR sOptionList_GetCurSel = _T("OptionList_GetCurSel");		
LPCTSTR sOptionList_GetCount = _T("OptionList_GetCount");		
LPCTSTR sOptionList_SetCurSel = _T("OptionList_SetCurSel");		
LPCTSTR sOptionList_GetTopIndex = _T("OptionList_GetTopIndex");
LPCTSTR sOptionList_SetTopIndex = _T("OptionList_SetTopIndex");
LPCTSTR sOptionList_DeleteString = _T("OptionList_DeleteString");	
LPCTSTR sOptionList_InsertString = _T("OptionList_InsertString");	


LPCTSTR sListBox_Dir = _T("ListBox_Dir");
LPCTSTR sListBox_AddString = _T("ListBox_AddString");		
LPCTSTR sListBox_AddList = _T("ListBox_AddList");		
LPCTSTR sListBox_Clear =_T("ListBox_Clear");			
LPCTSTR sListBox_GetText = _T("ListBox_GetText");		
LPCTSTR sListBox_GetSelectedItems = _T("ListBox_GetSelectedItems");
LPCTSTR sListBox_GetSelectedNths = _T("ListBox_GetSelectedNths");
LPCTSTR sListBox_GetCurSel = _T("ListBox_GetCurSel");		
LPCTSTR sListBox_GetCount = _T("ListBox_GetCount");		
LPCTSTR sListBox_SetCurSel = _T("ListBox_SetCurSel");		
LPCTSTR sListBox_GetTopIndex = _T("ListBox_GetTopIndex");
LPCTSTR sListBox_SetTopIndex = _T("ListBox_SetTopIndex");
LPCTSTR sListBox_SetItemData = _T("ListBox_SetItemData");	
LPCTSTR sListBox_GetItemData = _T("ListBox_GetItemData");	
LPCTSTR sListBox_DeleteString = _T("ListBox_DeleteString");	
LPCTSTR sListBox_InsertString = _T("ListBox_InsertString");	
LPCTSTR sListBox_FindString = _T("ListBox_FindString");		
LPCTSTR sListBox_SelectString = _T("ListBox_SelectString");	
LPCTSTR sListBox_FindStringExact = _T("ListBox_FindStringExact");
LPCTSTR sListBox_SetSel = _T("ListBox_SetSel");			
LPCTSTR sListBox_GetSel = _T("ListBox_GetSel");			
LPCTSTR sListBox_SetFocusIndex = _T("ListBox_SetFocusIndex");	
LPCTSTR sListBox_GetFocusIndex = _T("ListBox_GetFocusIndex");	
LPCTSTR sListBox_GetSelCount = _T("ListBox_GetSelCount");	
LPCTSTR sListBox_SelItemRange = _T("ListBox_SelItemRange");	
LPCTSTR sListBox_SetAnchorIndex = _T("ListBox_SetAnchorIndex");	
LPCTSTR sListBox_GetAnchorIndex = _T("ListBox_GetAnchorIndex");	

// these are the ListView methods 
LPCTSTR sListCtrl_GetColumnImage = _T("ListView_GetColumnImage");
LPCTSTR sListCtrl_SetColumnImage = _T("ListView_SetColumnImage");
LPCTSTR sListCtrl_GetCount = _T("ListView_GetCount");
LPCTSTR sListCtrl_GetColumnCount = _T("ListView_GetColumnCount");

LPCTSTR sListCtrl_AddString = _T("ListView_AddString");
LPCTSTR sListCtrl_AddColumn = _T("ListView_AddColumns");
LPCTSTR sListCtrl_AddRow = _T("ListView_AddItem");
LPCTSTR sListCtrl_Arrange = _T("ListView_Arrange");
LPCTSTR sListCtrl_CalcColumnWidth = _T("ListView_CalcColWidth");
LPCTSTR sListCtrl_Clear = _T("ListView_Clear");
LPCTSTR sListCtrl_CountItems = _T("ListView_CountItems");
LPCTSTR sListCtrl_DeleteItem = _T("ListView_DeleteItem");
LPCTSTR sListCtrl_DeleteItems = _T("ListView_DeleteItems");
LPCTSTR sListCtrl_DeleteColumns = _T("ListView_DeleteColumns");
LPCTSTR sListCtrl_DeleteColumn = _T("ListView_DeleteColumn");
LPCTSTR sListCtrl_FillGrid = _T("ListView_FillList");
LPCTSTR sListCtrl_GetColWidth = _T("ListView_GetColWidth");
LPCTSTR sListCtrl_GetFileName = _T("BlockList_GetFileName");
LPCTSTR sListCtrl_GetItemData = _T("ListView_GetItemData");
LPCTSTR sListCtrl_GetItemImage = _T("ListView_GetItemImage");
LPCTSTR sListCtrl_GetItemText = _T("ListView_GetItemText");
LPCTSTR sListCtrl_GetSelectedCount = _T("ListView_GetSelCount");
LPCTSTR sListCtrl_GetSelectedItems = _T("ListView_GetSelectedItems");
LPCTSTR sListCtrl_GetSelectedNths = _T("ListView_GetSelectedNths");
LPCTSTR sListCtrl_HitPointTest = _T("ListView_HitPointTest");
LPCTSTR sListCtrl_InsertItem = _T("ListView_InsertItem");
LPCTSTR sListCtrl_InsertRow = _T("ListView_InsertRow");
LPCTSTR sListCtrl_LoadDwg = _T("BlockList_LoadDwg");
LPCTSTR sListCtrl_Reset = _T("BlockList_Reset");
LPCTSTR sListCtrl_SetColWidth = _T("ListView_SetColWidth");
LPCTSTR sListCtrl_SetCurSel = _T("ListView_SetCurSel");
LPCTSTR sListCtrl_SetItemData = _T("ListView_SetItemData");
LPCTSTR sListCtrl_SetItemText = _T("ListView_SetItemText");
LPCTSTR sListCtrl_SetItemImage = _T("ListView_SetItemImage");
LPCTSTR sListCtrl_StartLabelEdit = _T("ListView_StartLabelEdit");
LPCTSTR sListCtrl_CancelLabelEdit = _T("ListView_CancelLabelEdit");
LPCTSTR sListCtrl_SortTextItems = _T("ListView_SortTextItems");
LPCTSTR sListCtrl_SortNumericItems = _T("ListView_SortNumericItems");
LPCTSTR sListCtrl_GetRow = _T("ListView_GetRowItems");
LPCTSTR sListCtrl_GetColumn = _T("ListView_GetColumnItems");

// these are the ComboBox methods 
LPCTSTR sComboBox_Dir = _T("ComboBox_Dir");	
LPCTSTR sComboBox_GetDir = _T("ComboBox_GetDir");	
LPCTSTR sComboBox_AddPath = _T("ComboBox_AddPath");
LPCTSTR sComboBox_AddColor = _T("ComboBox_AddColor");			
LPCTSTR sComboBox_FindColor = _T("ComboBox_FindColor");
LPCTSTR sComboBox_FindLineWeight = _T("ComboBox_FindLineWeight");
LPCTSTR sComboBox_AddString = _T("ComboBox_AddString");			
LPCTSTR sComboBox_AddList = _T("ComboBox_AddList");		
LPCTSTR sComboBox_GetCurSel = _T("ComboBox_GetCurSel");			
LPCTSTR sComboBox_GetCount = _T("ComboBox_GetCount");			
LPCTSTR sComboBox_Clear = _T("ComboBox_Clear");				
LPCTSTR sComboBox_DeleteString = _T("ComboBox_DeleteString");	
LPCTSTR sComboBox_InsertString = _T("ComboBox_InsertString");		
LPCTSTR sComboBox_FindString = 	_T("ComboBox_FindString");		
LPCTSTR sComboBox_FindStringExact = _T("ComboBox_FindStringExact");
LPCTSTR sComboBox_SelectString = _T("ComboBox_SelectString");		
LPCTSTR sComboBox_SetCurSel = _T("ComboBox_SetCurSel");			
LPCTSTR sComboBox_GetEditSel = _T("ComboBox_GetEditSel");		
LPCTSTR sComboBox_SetEditSel = _T("ComboBox_SetEditSel");		
LPCTSTR sComboBox_SetItemData = _T("ComboBox_SetItemData");		
LPCTSTR sComboBox_GetItemData = _T("ComboBox_GetItemData");		
LPCTSTR sComboBox_GetTopIndex = _T("ComboBox_GetTopIndex");		
LPCTSTR sComboBox_SetTopIndex = _T("ComboBox_SetTopIndex");		
LPCTSTR sComboBox_GetDroppedWidth = _T("ComboBox_GetDroppedWidth");	
LPCTSTR sComboBox_SetDroppedWidth = _T("ComboBox_SetDroppedWidth");	
LPCTSTR sComboBox_ClearEdit = _T("ComboBox_ClearEdit");			
LPCTSTR sComboBox_GetLBText = _T("ComboBox_GetLBText");			
LPCTSTR sComboBox_GetEBText = _T("ComboBox_GetTBText");			
LPCTSTR sComboBox_GetFontFileName = _T("ComboBox_GetFontFileName");			
// these are the Month control methods 
LPCTSTR sMonth_SetCurSel = _T("Month_SetCurSel");		
LPCTSTR sMonth_GetCurSel = _T("Month_GetCurSel");		
LPCTSTR sMonth_SetRange = _T("Month_SetRange");		
LPCTSTR sMonth_GetRangeStart = _T("Month_GetRangeStart");	
LPCTSTR sMonth_GetRangeEnd = _T("Month_GetRangeEnd");		
LPCTSTR sMonth_GetMonthRangeStart = _T("Month_GetMonthRangeStart");	
LPCTSTR sMonth_GetMonthRangeEnd = _T("Month_GetMonthRangeEnd");		
LPCTSTR sMonth_SetSelRange = _T("Month_SetSelRange");		
LPCTSTR sMonth_GetSelRangeStart = _T("Month_GetSelRangeStart");		
LPCTSTR sMonth_GetSelRangeEnd = _T("Month_GetSelRangeEnd");		
LPCTSTR sMonth_GetToday = _T("Month_GetToday");				
// these are the slide view methods 
LPCTSTR sSlideView_SetFileName = _T("SlideView_Load");				
LPCTSTR sSlideView_Clear = _T("SlideView_Clear");			
LPCTSTR sSlideView_SetHighLight = _T("SlideView_SetHighLight");	
LPCTSTR sSlideView_RemoveHighLight = _T("SlideView_RemoveHighLight");	
LPCTSTR sSlideView_VectorImage = _T("SlideView_VectorImage");	
LPCTSTR sSlideView_FillImage = _T("SlideView_FillImage");
LPCTSTR sSlideView_SlideImage = _T("SlideView_SlideImage");
LPCTSTR sSlideView_EndImage = _T("SlideView_EndImage");
// these are the PictureBox methods 
LPCTSTR sPictureBox_StoreImage = _T("PictureBox_StoreImage");				
LPCTSTR sPictureBox_Clear = _T("PictureBox_Clear");	
LPCTSTR sPictureBox_Refresh = _T("PictureBox_Refresh");
LPCTSTR sPictureBox_DrawLine = _T("PictureBox_DrawLine");			
LPCTSTR sPictureBox_DrawArc = _T("PictureBox_DrawArc");			
LPCTSTR sPictureBox_DrawCircle = _T("PictureBox_DrawCircle");		
LPCTSTR sPictureBox_DrawFillRect = _T("PictureBox_DrawFillRect");	
LPCTSTR sPictureBox_DrawEdge = _T("PictureBox_DrawEdge");			
LPCTSTR sPictureBox_DrawHatchRect = _T("PictureBox_DrawHatchRect");
LPCTSTR sPictureBox_DrawWrappedText = _T("PictureBox_DrawWrappedText");	
LPCTSTR sPictureBox_DrawText = _T("PictureBox_DrawText");			
LPCTSTR sPictureBox_GetTextExtent = _T("PictureBox_GetTextExtent");
LPCTSTR sPictureBox_PaintAnIcon = _T("PictureBox_PaintPicture");	
LPCTSTR sPictureBox_DrawPoint = _T("PictureBox_DrawPoint");		
LPCTSTR sPictureBox_LoadPictureFile = _T("PictureBox_LoadPictureFile");		
LPCTSTR sPictureBox_DrawFocusRect = _T("PictureBox_DrawFocusRect");		
LPCTSTR sPictureBox_DrawRect = _T("PictureBox_DrawRect");			

LPCTSTR sHtml_Navigate = _T("Html_Navigate");	
LPCTSTR sHtml_Stop = _T("Html_Stop");			
LPCTSTR sHtml_Refresh = _T("Html_Refresh");	
LPCTSTR sHtml_GoBack = _T("Html_GoBack");		
LPCTSTR sHtml_GoForward = _T("Html_GoForward");
LPCTSTR sHtml_GoHome = _T("Html_GoHome");		
LPCTSTR sHtml_GoSearch = _T("Html_GoSearch");	
LPCTSTR sHtml_GetLocationName = _T("Html_GetLocationName");	
LPCTSTR sHtml_GetLocationURL = _T("Html_GetLocationURL");	
LPCTSTR sHtml_GetOffline = _T("Html_GetOffline");	 
LPCTSTR sHtml_SetOffline = _T("Html_SetOffline");	
LPCTSTR sHtml_GetBusy = _T("Html_GetBusy");		
LPCTSTR sHtml_GetFullName = _T("Html_GetFullName");
LPCTSTR sHtml_UpdateHtmlCode = _T("Html_UpdateHtmlCode");
LPCTSTR sHtml_GetHtmlDocument = _T("Html_GetHtmlDocument");
LPCTSTR sHtml_ReplaceText = _T("sHtml_ReplaceText");

// these are the tree methods 
LPCTSTR sTree_IsItemExpanded = _T("Tree_IsItemExpanded");
LPCTSTR sTree_AddParent = _T("Tree_AddParent");				
LPCTSTR sTree_AddChild = _T("Tree_AddChild");				
LPCTSTR sTree_InsertAfter = _T("Tree_InsertAfter");		
LPCTSTR sTree_GetParent = _T("Tree_GetParent");			
LPCTSTR sTree_Clear = _T("Tree_Clear");				
LPCTSTR sTree_CountItems = _T("Tree_CountItems");		
LPCTSTR sTree_SelectItem = _T("Tree_SelectItem");		
LPCTSTR sTree_ItemHasChildren = _T("Tree_ItemHasChildren");		
LPCTSTR sTree_GetNextSiblingItem = _T("Tree_GetNextSiblingItem");	
LPCTSTR sTree_GetPrevSiblingItem = _T("Tree_GetPrevSiblingItem");	
LPCTSTR sTree_GetFirstVisibleItem = _T("Tree_GetFirstVisibleItem");
LPCTSTR sTree_GetNextVisibleItem = _T("Tree_GetNextVisibleItem");	
LPCTSTR sTree_GetPrevVisibleItem = _T("Tree_GetPrevVisibleItem");	
LPCTSTR sTree_GetSelectedItem = _T("Tree_GetSelectedItem");		
LPCTSTR sTree_GetRootItem = _T("Tree_GetRootItem");			
LPCTSTR sTree_GetItem = _T("Tree_GetItem");
LPCTSTR sTree_SetItemImages = _T("Tree_SetItemImages");			
LPCTSTR sTree_GetItemImages = _T("Tree_GetItemImages");			
LPCTSTR sTree_GetItemText = _T("Tree_GetItemText");			
LPCTSTR sTree_SetItemText = _T("Tree_SetItemText");			
LPCTSTR sTree_GetItemData = _T("Tree_GetItemData");			
LPCTSTR sTree_SetItemData = _T("Tree_SetItemData");			
LPCTSTR sTree_GetVisibleCount = _T("Tree_GetVisibleCount");		
LPCTSTR sTree_DeleteItem = _T("Tree_DeleteItem");			
LPCTSTR sTree_ExpandItem = _T("Tree_ExpandItem");			
LPCTSTR sTree_SelectSetFirstVisible = _T("Tree_SelectSetFirstVisible");	
LPCTSTR sTree_EditLabel = _T("Tree_EditLabel");				
LPCTSTR sTree_SortChildren = _T("Tree_SortChildren");			
LPCTSTR sTree_EnsureVisible = _T("Tree_EnsureVisible");		
LPCTSTR sTree_GetFirstChildItem = _T("Tree_GetFirstChildItem");		
LPCTSTR sTree_SetExpanedImage = _T("Tree_SetExpandedImage");
LPCTSTR sTree_GetExpanedImage = _T("Tree_GetExpandedImage");
LPCTSTR sTree_CancelEditLabel = _T("Tree_CancelEditLabel");
// these are the tab methods 
LPCTSTR sTabControl_SetCurSel = _T("Tab_SetCurSel");	
LPCTSTR sTabControl_GetCurSel = _T("Tab_GetCurSel");	
LPCTSTR sTabControl_GetRowCount = _T("Tab_GetRowCount");	
LPCTSTR sTabControl_SetTabText = _T("Tab_SetTabText");	
LPCTSTR sTabControl_HideTab = _T("Tab_HideTab");	
LPCTSTR sTabControl_ShowTab = _T("Tab_ShowTab");

// these are the text box methods 
LPCTSTR sTextBox_SetFilter = _T("TextBox_SetFilter");
LPCTSTR sTextBox_GetFilter = _T("TextBox_GetFilter");
LPCTSTR sTextBox_GetLineCount = _T("TextBox_GetLineCount");		
LPCTSTR sTextBox_GetModify = _T("TextBox_GetModify");			
LPCTSTR sTextBox_GetSel = _T("TextBox_GetSel");				
LPCTSTR sTextBox_GetLine = _T("TextBox_GetLine");			
LPCTSTR sTextBox_GetFirstVisibleLine = _T("TextBox_GetFirstVisibleLine");
LPCTSTR sTextBox_GetLineLength = _T("TextBox_GetLineLength");		
LPCTSTR sTextBox_LineScroll = _T("TextBox_LineScroll");			
LPCTSTR sTextBox_ReplaceSel = _T("TextBox_ReplaceSel");			
LPCTSTR sTextBox_SetSel = _T("TextBox_SetSel");				
LPCTSTR sTextBox_SetTabStops = _T("TextBox_SetTabStops");		
LPCTSTR sTextBox_Undo = _T("TextBox_Undo");			
