// (C) Copyright 2002-2005 by Autodesk, Inc. 
//
// Permission to use, copy, modify, and distribute this software in
// object code form for any purpose and without fee is hereby granted, 
// provided that the above copyright notice appears in all copies and 
// that both that copyright notice and the limited warranty and
// restricted rights notice below appear in all supporting 
// documentation.
//
// AUTODESK PROVIDES THIS PROGRAM "AS IS" AND WITH ALL FAULTS. 
// AUTODESK SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF
// MERCHANTABILITY OR FITNESS FOR A PARTICULAR USE.  AUTODESK, INC. 
// DOES NOT WARRANT THAT THE OPERATION OF THE PROGRAM WILL BE
// UNINTERRUPTED OR ERROR FREE.
//
// Use, duplication, or disclosure by the U.S. Government is subject to 
// restrictions set forth in FAR 52.227-19 (Commercial Computer
// Software - Restricted Rights) and DFAR 252.227-7013(c)(1)(ii)
// (Rights in Technical Data and Computer Software), as applicable.
//

//-----------------------------------------------------------------------------
//----- acrxEntryPoint.h
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "Resource.h"
#include "ArxProject.h"
#include "PropertyNames.h"
#include "DclControlObject.h"
#include "PropertyObject.h"
#include "ArgumentsRetrieval.h"
#include "Methods_ActiveX.h"
#include "Methods_Animate.h"
#include "Methods_ListBox.h"
#include "Methods_Grid.h"
#include "Methods_Hatch.h"
#include "Methods_ListCtrl.h"
#include "Methods_PictureBox.h"
#include "Methods_Properties.h"
#include "Methods_SlideView.h"
#include "Methods_tree.h"
#include "Methods_Edit.h"
#include "Methods_Tab.h"
#include "Methods_ComboBox.h"
#include "Methods_ComboBoxEx.h"
#include "Methods_Html.h"
#include "Methods_Month.h"
#include "Methods_BlockView.h"
#include "Methods_DwgPreview.h"
#include "Methods_Files.h"
#include "Methods_ActiveX.h"
#include "Methods_FileDlg.h"
#include "Methods_BinFiles.h"
#include "MethodLexicon.h"
#include "DialogObject.h"
#include "ModalVDcl.h"
#include "CustomFileDialog.h"
#include "ErrorLexicon.h"
#include "LayeredWindowHelperST.h"
#include "ControlTypes.h"
#include "ArchiveEx.h"
#include "CfgTabPane.h"
#include "DockingDialog.h"
#include "ResizableDockingDialog.h"
#include "SlideHolder.h"
#include "InvokeMethod.h"
#include "ModelessDlg.h"
#include "DialogControl.h"
#include "StringCompare.h"
#include "DirDialog.h"
#include "PictureObject.h"
#include "AcadColorTable.h"
#include "LineWeightDlg.h"
#include "LinetypeDlg.h"
#include "CustomFileDialog.h"
#include "Base64.h"


//-----------------------------------------------------------------------------
#define szRDS _RXST("odcl")
#define ADSFUNCBASE 1000 //the externally defined ADS functions will be defined starting at this function index
#define ADSPROPFUNCBASE 2000 //the property functions will be defined starting at this function index

#define theRxApp entryPointObject

static LPCTSTR gpszDclEventsLspFileName = _T("DclEvents.lsp");
static LPCTSTR gpszOnActionEventLispFunction = _T("(defun c:OnActionEvent_%s()%s)(princ)\n");


#ifdef _DIAGNOSTIC
static int DumpProject(void);
static int DumpForm(void);
static int DumpControl(void);
#endif //_DIAGNOSTIC


static const struct AdsFunctionTableEntry { LPCTSTR pszFunctionName; int (*pfHandler) (); } grAdsFunctionTable[] =
{
#ifdef _DIAGNOSTIC
	{_T("Project_Dump"),	DumpProject},
	{_T("Form_Dump"),			DumpForm},
	{_T("Control_Dump"),	DumpControl},
#endif //_DIAGNOSTIC

	// binary file read write calls
	{_T("OpenBin"),				OpenBinFile},
	{_T("WriteBin"),			WriteBinFile},
	{_T("ReadBin"),				ReadBinFile},
	{_T("CloseBin"),			CloseBinFile},						
	{_T("CheckBin"),			CheckBinFile},						

	// general control methods
	{_T("Control_SetProperty"),			SetProperty}, // this method is used to set any property of any control
	{_T("Control_GetProperty"),			GetProperty}, // this method is used to get any property of any control
	{_T("Control_ShowToolTip"),			ShowToolTip},
	{_T("Control_SetFocus"),				SetControlFocus},
	{_T("Control_ZOrder"),				ZOrder},
	{_T("Control_GetCurPos"),			Control_GetCurPos},
	{_T("Control_SetPos"),				Control_SetPos},
	{_T("Control_ForceUpdateNow"),		ForceUpdateNow},

	{_T("ProgressBar_SetPos"),	ProgressBar_SetPos},
	{_T("Files_Dir"),						FilesDir},

	// animation control methods
	{_T("Animate_Load"),			AnimateCtrl_Load},
	{_T("Animate_Seek"),			AnimateCtrl_Seek},
	{_T("Animate_Stop"),			AnimateCtrl_Stop},
	{_T("Animate_Close"),			AnimateCtrl_Close},

	{_T("Hatch_Clear"),			Hatch_Clear},
	{_T("Hatch_SetPattern"), Hatch_SetPattern},

	// file dialog box methods
	{sFileDlgGetFileName,		FileDlgGetFileName},
	{sFileDlgGetFileTitle,		FileDlgGetFileTitle},
	{sFileDlgGetPathName,		FileDlgGetPathName},
	{sFileDlgGetSelectionCount,	FileDlgGetSelectionCount},
	{sFileDlgGetFolderPath,		FileDlgGetFolderPath},
	{sFileDlgGetFileNameList,	FileDlgGetFileNameList},
	{sFileDlgSetOkButtonText,	FileDlgSetOkButtonText},
	{sFileDlgGetFileExt,		FileDlgGetFileExt},
	{sFileDlgGetFolderName,		FileDlgGetFolderName},

	// ActiveX control methods
	{sAxSetProperty,			SetAxProperty},
	{sAxGetProperty,			GetAxProperty},
	{sDoAxMethod,				DoAxMethod},
	{sAxObjectSetProperty,		SetAxObjectProperty},
	{sAxObjectGetProperty,		GetAxObjectProperty},
	{sObjectDoAxMethod,			DoAxObjectMethod},
	{sSetAxColorProperty,		SetAxColorProperty},
	{sSetAxObjColorProperty,	SetAxObjColorProperty},
	{sSetAxPictureProperty,		SetAxPictureProperty},
	{sSetAxObjPictureProperty,	SetAxObjectPictureProperty},
	{sAxGetObject,				AxGetObject},
	{sCloseAxObject,			CloseAxObject},
	{sReleaseAxObject,			CloseAxObject},

	// block view control methods
	{sBlockView_Clear,				BlockView_Clear},
	{sBlockView_SetHighLight,		BlockView_SetHighLight},
	{sBlockView_RemoveHighLight,	BlockView_RemoveHighLight},
	{sBlockView_Zoom,				BlockView_Zoom},
	{sBlockView_ViewBlock,			BlockView_ViewBlock},
	{sBlockView_ViewPaperSpace,		BlockView_ViewPaperSpace},											
	{sBlockView_ViewBlockToScale,	BlockView_ViewBlockToScale},
	{sBlockView_LoadDwg,			BlockView_LoadDwg},	
	{sBlockView_LoadDwgToScale,		BlockView_LoadDwgToScale},
	{sBlockView_PreLoadDwg,			BlockView_PreLoadDwg},
	{sBlockView_GetBlockList,		BlockView_GetBlockList},
	{sBlockView_GetBlockSize,		BlockView_GetBlockSize},
	{sBlockView_GetDwgSize,			BlockView_GetDwgSize},
	{sBlockView_RefreshBlock,		BlockView_RefreshBlock},
	{sBlockView_GetViewInfo,		BlockView_GetViewInfo},
	{sBlockView_SetView,			BlockView_SetView},

	// dwg preview control methods 
	{sDwgPreview_LoadDwg,			DwgPreview_LoadDwg},
	{sDwgPreview_GetDwgName,		DwgPreview_GetDwgName},
	{sDwgPreview_Clear,				DwgPreview_Clear},
	{sDwgPreview_SetHighLight,		DwgPreview_SetHighLight},
	{sDwgPreview_RemoveHighLight,	DwgPreview_RemoveHighLight},

	// list box control methods 
	{sDwgList_Dir,				DwgList_Dir},
	{sDwgList_GetDir,			DwgList_GetDir},
	{sDwgList_GetFileName,		DwgList_GetFileName},
	{sDwgList_SetRowHeight,		DwgList_SetRowHeight},											
	{sDwgList_GetType,			DwgList_GetType},

	{sOptionList_SetEnabled,		OptionList_SetEnabled},
	{sOptionList_SetTttTitle,		OptionList_SetTttTitle},

	{sOptionList_AddList,		ListBox_AddList},
	{sOptionList_AddString,		ListBox_AddString},
	{sOptionList_Clear,			ListBox_Clear},
	{sOptionList_GetText,		ListBox_GetText},
	{sOptionList_GetCurSel,		ListBox_GetCurSel},
	{sOptionList_GetCount,		ListBox_GetCount},
	{sOptionList_SetCurSel,		ListBox_SetCurSel},
	{sOptionList_GetTopIndex,   ListBox_GetTopIndex},
	{sOptionList_SetTopIndex,   ListBox_SetTopIndex},
	{sOptionList_DeleteString,  ListBox_DeleteString},
	{sOptionList_InsertString,	ListBox_InsertString},

	{sListBox_Dir,				ListBox_Dir},
	{sListBox_AddString,		ListBox_AddString},
	{sListBox_AddList,			ListBox_AddList},		
	{sListBox_Clear,			ListBox_Clear},											
	{sListBox_GetText,			ListBox_GetText},
	{sListBox_GetSelectedItems,	ListBox_GetSelectedItems},	
	{sListBox_GetSelectedNths,	ListBox_GetSelectedNths},	
	{sListBox_GetCurSel,		ListBox_GetCurSel},
	{sListBox_GetCount,			ListBox_GetCount},
	{sListBox_SetCurSel,		ListBox_SetCurSel},
	{sListBox_GetTopIndex,		ListBox_GetTopIndex},
	{sListBox_SetTopIndex,		ListBox_SetTopIndex},
	{sListBox_SetItemData,		ListBox_SetItemData},
	{sListBox_GetItemData,		ListBox_GetItemData},
	{sListBox_DeleteString,		ListBox_DeleteString},
	{sListBox_InsertString,		ListBox_InsertString},
	{sListBox_FindString,		ListBox_FindString},
	{sListBox_SelectString,		ListBox_SelectString},
	{sListBox_FindStringExact,	ListBox_FindStringExact},
	{sListBox_SetSel,			ListBox_SetSel},
	{sListBox_GetSel,			ListBox_GetSel},
	{sListBox_SetFocusIndex,	ListBox_SetFocusIndex},
	{sListBox_GetFocusIndex,	ListBox_GetFocusIndex},
	{sListBox_GetSelCount,		ListBox_GetSelCount},
	{sListBox_SelItemRange,		ListBox_SelItemRange},
	{sListBox_SetAnchorIndex,	ListBox_SetAnchorIndex},
	{sListBox_GetAnchorIndex,	ListBox_GetAnchorIndex},

	// grid control methods 
	{sGrid_Cell_TextBox,		Grid_Cell_TextBox},
	{sGrid_AddString,			Grid_AddString},
	{sGrid_Cell_ComboBox,		Grid_Cell_ComboBox},
	{sGrid_Cell_ImageComboBox,	Grid_Cell_ImageComboBox},
	{sGrid_Cell_EllipsesButton,	Grid_Cell_EllipsesButton},
	{sGrid_Cell_PickButton,		Grid_Cell_PickButton},
	{sGrid_Cell_SetStyle,		Grid_Cell_SetStyle},											
	{sGrid_SelectCell,			Grid_SelCurCell},

	{sGrid_SelCurRow,			Grid_SelCurRow},
	{sGrid_HitPointTest,		Grid_HitPointTest},
	{sGrid_Cell_SetDropList,	Grid_Cell_SetDropList},
	{sGrid_Cell_DoCellDlg,		Grid_Cell_DoCellDlg},
	{sGrid_AddColumn,			Grid_AddColumn},
	{sGrid_AddRow,				Grid_AddRow},
	{sGrid_CalcColumnWidth,		Grid_CalcColumnWidth},
	{sGrid_Clear,				Grid_Clear},
	{sGrid_DeleteColumn,		Grid_DeleteColumns},
	{sGrid_DeleteColumns,		Grid_DeleteColumns},
	{sGrid_DeleteItem,			Grid_DeleteItems},
	{sGrid_FillGrid,			Grid_FillGrid},
	{sGrid_GetColWidth,			Grid_GetColWidth},
	{sGrid_GetItemData,			Grid_GetItemData},
	{sGrid_GetItemImage,		Grid_GetItemImage},
	{sGrid_GetItemText,			Grid_GetItemText},
	{sGrid_GetSelectedCell,		Grid_GetSelectedCell},	
	{sGrid_SetColWidth,			Grid_SetColWidth},
	{sGrid_SetItemData,			Grid_SetItemData},
	{sGrid_SetItemImage,		Grid_SetItemImage},
	{sGrid_SetItemText,			Grid_SetItemText},
	{sGrid_CancelLabelEdit,		Grid_CancelLabelEdit},
	{sGrid_SortTextItems,		Grid_SortColTextItems},
	{sGrid_SortNumericItems,	Grid_SortColNumericItems},											
	{sGrid_GetRow,				Grid_GetRow},
	{sGrid_GetColumn,			Grid_GetColumn},
	{sGrid_GetColumnImage,		Grid_GetColumnImage},
	{sGrid_SetColumnImage,		Grid_SetColumnImage},
	{sGrid_GetCount,			Grid_GetCount},
	{sGrid_GetColumnCount,		Grid_GetColumnCount},
	{sGrid_Cell_ToggleImages,	Grid_Cell_ToggleImages},
	{sGrid_GetCheck,			Grid_GetCheck},
	{sGrid_SetCheck,			Grid_SetCheck},
	{sGrid_InsertRow,			Grid_InsertRow},
	{sGrid_InsertString,		Grid_InsertString},
	{sGrid_SetCurRow,			Grid_SelCurRow},

	// flex grid control methods
	{_T("FlexGrid_GetColor"), GetFlexGridColorProperty},
	{_T("FlexGrid_SetColor"), SetFlexGridColorProperty},

	// list view control methods 
	{sListCtrl_AddString,		ListCtrl_AddString},
	{sListCtrl_AddColumn,		ListCtrl_AddColumn},
	{sListCtrl_AddRow,			ListCtrl_AddRow},
	{sListCtrl_Arrange,			ListCtrl_Arrange},
	{sListCtrl_CalcColumnWidth, ListCtrl_CalcColumnWidth},
	{sListCtrl_Clear,			ListCtrl_Clear},
	{sListCtrl_CountItems,		ListCtrl_CountItems},
	{sListCtrl_DeleteColumn,	ListCtrl_DeleteColumns},
	{sListCtrl_DeleteColumns,	ListCtrl_DeleteColumns},
	{sListCtrl_DeleteItem,		ListCtrl_DeleteItems},
	{sListCtrl_DeleteItems,		ListCtrl_DeleteItems},
	{sListCtrl_FillGrid,		ListCtrl_FillGrid},
	{sListCtrl_GetColWidth,		ListCtrl_GetColWidth},
	{sListCtrl_GetFileName,		ListCtrl_GetFileName},											
	{sListCtrl_GetItemData,		ListCtrl_GetItemData},
	{sListCtrl_GetItemImage,	ListCtrl_GetItemImage},
	{sListCtrl_GetItemText,		ListCtrl_GetItemText},
	{sListCtrl_GetSelectedCount,ListCtrl_GetSelectedCount},
	{sListCtrl_GetSelectedItems,ListCtrl_GetSelectedItems},	
	{sListCtrl_GetSelectedNths,	ListCtrl_GetSelectedNths},												
	{sListCtrl_HitPointTest,    ListCtrl_HitPointTest},
	{sListCtrl_InsertItem,		ListCtrl_InsertItem},
	{sListCtrl_LoadDwg,			ListCtrl_LoadDwg},
	{sListCtrl_Reset,			ListCtrl_Reset},											
	{sListCtrl_SetColWidth,		ListCtrl_SetColWidth},
	{sListCtrl_SetCurSel,		ListCtrl_SetCurSel},
	{sListCtrl_SetItemData,		ListCtrl_SetItemData},
	{sListCtrl_SetItemImage,	ListCtrl_SetItemImage},
	{sListCtrl_SetItemText,		ListCtrl_SetItemText},
	{sListCtrl_StartLabelEdit,	ListCtrl_StartLabelEdit},
	{sListCtrl_CancelLabelEdit,	ListCtrl_CancelLabelEdit},
	{sListCtrl_SortTextItems,	ListCtrl_SortColTextItems},
	{sListCtrl_SortNumericItems,ListCtrl_SortColNumericItems},											
	{sListCtrl_GetRow,			ListCtrl_GetRow},
	{sListCtrl_GetColumn,		ListCtrl_GetColumn},
	{sListCtrl_GetColumnImage,	ListCtrl_GetColumnImage},
	{sListCtrl_SetColumnImage,	ListCtrl_SetColumnImage},
	{sListCtrl_GetCount,		ListCtrl_GetCount},
	{sListCtrl_GetColumnCount,	ListCtrl_GetColumnCount},

	// combo box control methods 
	{sComboBox_Dir,					ComboBox_Dir},
	{sComboBox_GetDir,				ComboBox_GetDir},
	{sComboBox_AddPath,				ComboBox_AddPath},											
	{sComboBox_AddColor,			ComboBox_AddColor},
	{sComboBox_FindColor,			ComboBox_FindColor},
	{sComboBox_FindLineWeight,		ComboBox_FindLineWeight},
	{sComboBox_AddString,			ComboBox_AddString},
	{sComboBox_AddList,				ComboBox_AddList},
	{sComboBox_GetCurSel,			ComboBox_GetCurSel},
	{sComboBox_GetCount,			ComboBox_GetCount},
	{sComboBox_Clear,				ComboBox_Clear},
	{sComboBox_DeleteString,		ComboBox_DeleteString},
	{sComboBox_InsertString,		ComboBox_InsertString},
	{sComboBox_FindString,			ComboBox_FindString},											
	{sComboBox_FindStringExact,		ComboBox_FindStringExact},
	{sComboBox_SelectString,		ComboBox_SelectString},
	{sComboBox_SetCurSel,			ComboBox_SetCurSel},
	{sComboBox_GetEditSel,			ComboBox_GetEditSel},
	{sComboBox_SetEditSel,			ComboBox_SetEditSel},
	{sComboBox_SetItemData,			ComboBox_SetItemData},
	{sComboBox_GetItemData,			ComboBox_GetItemData},
	{sComboBox_GetTopIndex,			ComboBox_GetTopIndex},
	{sComboBox_SetTopIndex,			ComboBox_SetTopIndex},
	{sComboBox_GetDroppedWidth,		ComboBox_GetDroppedWidth},
	{sComboBox_SetDroppedWidth,		ComboBox_SetDroppedWidth},
	{sComboBox_ClearEdit,			ComboBox_ClearEdit},
	{sComboBox_GetLBText,			ComboBox_GetLBText},
	{sComboBox_GetEBText,			ComboBox_GetEBText},

	{sComboBoxEx_AddString,			ComboBoxEx_AddString},
	{sComboBoxEx_SetItem,			ComboBoxEx_SetItem},
	{sComboBoxEx_GetItem,			ComboBoxEx_GetItem},
	{sComboBoxEx_GetCurSel,			ComboBoxEx_GetCurSel},
	{sComboBoxEx_GetCount,			ComboBoxEx_GetCount},
	{sComboBoxEx_Clear,				ComboBoxEx_Clear},
	{sComboBoxEx_DeleteString,		ComboBoxEx_DeleteString},
	{sComboBoxEx_FindString,		ComboBoxEx_FindString},											
	{sComboBoxEx_FindStringExact,	ComboBoxEx_FindStringExact},
	{sComboBoxEx_SelectString,		ComboBoxEx_SelectString},
	{sComboBoxEx_SetCurSel,			ComboBoxEx_SetCurSel},
	{sComboBoxEx_GetEditSel,		ComboBoxEx_GetEditSel},
	{sComboBoxEx_SetEditSel,		ComboBoxEx_SetEditSel},
	{sComboBoxEx_SetItemData,		ComboBoxEx_SetItemData},
	{sComboBoxEx_GetItemData,		ComboBoxEx_GetItemData},
	{sComboBoxEx_GetTopIndex,		ComboBoxEx_GetTopIndex},
	{sComboBoxEx_SetTopIndex,		ComboBoxEx_SetTopIndex},
	{sComboBoxEx_GetDroppedWidth,	ComboBoxEx_GetDroppedWidth},
	{sComboBoxEx_SetDroppedWidth,	ComboBoxEx_SetDroppedWidth},
	{sComboBoxEx_ClearEdit,			ComboBoxEx_ClearEdit},
	{sComboBoxEx_GetLBText,			ComboBoxEx_GetLBText},
	{sComboBoxEx_GetEBText,			ComboBoxEx_GetEBText},

	// month control methods 
	{sMonth_SetCurSel,				Month_SetCurSel},
	{sMonth_GetCurSel,				Month_GetCurSel},
	{sMonth_SetRange,				Month_SetRange},
	{sMonth_GetRangeStart,			Month_GetRangeStart},
	{sMonth_GetRangeEnd,			Month_GetRangeEnd},
	{sMonth_GetMonthRangeStart,		Month_GetMonthRangeStart},
	{sMonth_GetMonthRangeEnd,		Month_GetMonthRangeEnd},
	{sMonth_SetSelRange,			Month_SetSelRange},
	{sMonth_GetSelRangeStart,		Month_GetSelRangeStart},
	{sMonth_GetSelRangeEnd,			Month_GetSelRangeEnd},
	{sMonth_GetToday,				Month_GetToday},

	// slide view control methods 
	{sSlideView_SetFileName,		SlideView_SetFileName},
	{sSlideView_Clear,				SlideView_Clear},
	{sSlideView_SetHighLight,		SlideView_SetHighLight},
	{sSlideView_RemoveHighLight,	SlideView_RemoveHighLight},
	{sSlideView_VectorImage,		SlideView_VectorImage},
	{sSlideView_FillImage,			SlideView_FillImage},
	{sSlideView_SlideImage,			SlideView_SlideImage},											
	{sSlideView_EndImage,			SlideView_EndImage},

	// picture box control methods 
	{sPictureBox_StoreImage,		PictureBox_StoreImage},
	{sPictureBox_Clear,				PictureBox_Clear},
	{sPictureBox_Refresh,			PictureBox_Refresh},
	{sPictureBox_DrawLine,			PictureBox_DrawLine},
	{sPictureBox_DrawArc,			PictureBox_DrawArc},
	{sPictureBox_DrawCircle,		PictureBox_DrawCircle},
	{sPictureBox_DrawFillRect,		PictureBox_DrawFillRect},
	{sPictureBox_DrawEdge,			PictureBox_DrawEdge},
	{sPictureBox_DrawHatchRect,		PictureBox_DrawHatchRect},
	{sPictureBox_DrawWrappedText,	PictureBox_DrawWrappedText},
	{sPictureBox_DrawText,			PictureBox_DrawText},
	{sPictureBox_GetTextExtent,		PictureBox_GetTextExtent},
	{sPictureBox_PaintPicture,		PictureBox_PaintPicture},
	{sPictureBox_DrawPoint,			PictureBox_DrawPoint},
	{sPictureBox_LoadPictureFile,	PictureBox_LoadPictureFile},
	{sPictureBox_DrawFocusRect,		PictureBox_DrawFocusRect},
	{sPictureBox_DrawRect,			PictureBox_DrawRect},

	// html control methods
	{sHtml_Navigate,		Html_Navigate},
	{sHtml_Stop,			Html_Stop},
	{sHtml_Refresh,			Html_Refresh},
	{sHtml_GoBack,			Html_GoBack},
	{sHtml_GoForward,		Html_GoForward},
	{sHtml_GoHome,			Html_GoHome},
	{sHtml_GoSearch,		Html_GoSearch},
	{sHtml_GetLocationName,	Html_GetLocationName},
	{sHtml_GetLocationURL,	Html_GetLocationURL},
	{sHtml_GetOffline,		Html_GetOffline},
	{sHtml_SetOffline,		Html_SetOffline},
	{sHtml_GetBusy,			Html_GetBusy},
	{sHtml_GetFullName,		Html_GetFullName},
	{sHtml_UpdateHtmlCode,	Html_UpdateHtmlCode},
	{sHtml_ReplaceText,		Html_ReplaceText},
	{sHtml_GetHtmlDocument,	Html_GetHtmlDocument},

	// tree control methods 
	{sTree_IsItemExpanded,			Tree_IsItemExpanded},
	{sTree_AddParent,				Tree_AddParent},
	{sTree_AddChild,				Tree_AddChild},
	{sTree_InsertAfter,				Tree_InsertAfter},
	{sTree_GetParent,				Tree_GetParent},
	{sTree_Clear,					Tree_Clear},
	{sTree_CountItems,				Tree_CountItems},
	{sTree_SelectItem,				Tree_SelectItem},
	{sTree_ItemHasChildren,			Tree_ItemHasChildren},
	{sTree_GetNextSiblingItem,		Tree_GetNextSiblingItem},
	{sTree_GetPrevSiblingItem,		Tree_GetPrevSiblingItem},
	{sTree_GetFirstVisibleItem,		Tree_GetFirstVisibleItem},
	{sTree_GetNextVisibleItem,		Tree_GetNextVisibleItem},
	{sTree_GetPrevVisibleItem,		Tree_GetPrevVisibleItem},
	{sTree_GetSelectedItem,			Tree_GetSelectedItem},
	{sTree_GetRootItem,				Tree_GetRootItem},
	{sTree_GetItem,					Tree_GetItem},
	{sTree_SetItemImages,			Tree_SetItemImages},
	{sTree_GetItemImages,			Tree_GetItemImages},
	{sTree_GetItemText,				Tree_GetItemText},
	{sTree_SetItemText,				Tree_SetItemText},
	{sTree_GetItemData,				Tree_GetItemData},
	{sTree_SetItemData,				Tree_SetItemData},
	{sTree_GetVisibleCount,			Tree_GetVisibleCount},
	{sTree_DeleteItem,				Tree_DeleteItem},
	{sTree_ExpandItem,				Tree_ExpandItem},
	{sTree_SelectSetFirstVisible,	Tree_SelectSetFirstVisible},
	{sTree_EditLabel,				Tree_EditLabel},
	{sTree_SortChildren,			Tree_SortChildren},
	{sTree_EnsureVisible,			Tree_EnsureVisible},
	{sTree_GetFirstChildItem,		Tree_GetFirstChildItem},
	{sTree_SetExpanedImage,			Tree_SetExpanedImage},
	{sTree_GetExpanedImage,			Tree_GetExpanedImage},
	{sTree_CancelEditLabel,			Tree_CancelEditLabel},

	// tab control methods 
	{sTabControl_SetCurSel,			TabControl_SetCurSel},
	{sTabControl_GetCurSel,			TabControl_GetCurSel},
	{sTabControl_GetRowCount,		TabControl_GetRowCount},
	{sTabControl_SetTabText,		TabControl_SetTabText},
	{sTabControl_HideTab,			TabControl_HideTab},
	{sTabControl_ShowTab,			TabControl_ShowTab},

	// text box control methods 
	{sTextBox_GetFilter,			TextBox_GetFilter},
	{sTextBox_SetFilter,			TextBox_SetFilter},
	{sTextBox_GetLineCount,			TextBox_GetLineCount},
	{sTextBox_GetModify,			TextBox_GetModify},
	{sTextBox_GetSel,				TextBox_GetSel},
	{sTextBox_GetLine,				TextBox_GetLine},
	{sTextBox_GetFirstVisibleLine,	TextBox_GetFirstVisibleLine},
	{sTextBox_GetLineLength,		TextBox_GetLineLength},
	{sTextBox_LineScroll,			TextBox_LineScroll},
	{sTextBox_ReplaceSel,			TextBox_ReplaceSel},
	{sTextBox_SetSel,				TextBox_SetSel},
	{sTextBox_SetTabStops,			TextBox_SetTabStops},
	{sTextBox_Undo,					TextBox_Undo},
};


// Helper function to process lisp arguments (returns false to signal an argument exception)
bool RetrieveProjectFromArgs( /*in-out*/ resbuf*& pArgs, /*out*/ CProject*& pProject )
{
	if (!pArgs)
		return false; //arguments expected

	pProject = NULL;
	switch (pArgs->restype)	
	{
	case RTSHORT:
		pProject = (CProject*)pArgs->resval.rint;
		break;
	case RTLONG:
		pProject = (CProject*)pArgs->resval.rlong;
		break;
	case RTSTR:
		pProject = theArxWorkspace.FindProject( pArgs->resval.rstring );
		break;
	default:
		return false; //wrong argument type
	}
	pArgs = pArgs->rbnext; //move to the next argument
	return true;
}


bool RetrieveFormFromArgs( /*in-out*/ resbuf*& pArgs, /*out*/ CDclFormObject*& pForm )
{
	if (!pArgs)
		return false; //arguments expected

	pForm = NULL;
	switch (pArgs->restype)	
	{
	// Uncomment this code to accept nil arguments without generating an exception
	//case RTNIL:
	//	pForm = NULL;
	//	break;
	case RTSHORT:
		pForm = (CDclFormObject*)pArgs->resval.rint;
		break;
	case RTLONG:
		pForm = (CDclFormObject*)pArgs->resval.rlong;
		break;
	case RTSTR:
		{
			LPCTSTR pszProjectName = pArgs->resval.rstring;
			if (!pszProjectName)
				return false; //invalid argument

			pArgs = pArgs->rbnext; //move to the next argument
			if (!pArgs)
				return false; //not enough arguments

			if (pArgs->restype != RTSTR)
				return false; //wrong argument type
			pForm = theArxWorkspace.FindForm(pszProjectName, pArgs->resval.rstring);
			break;
		}
	default:
		return false; //wrong argument type
	}
	pArgs = pArgs->rbnext; //move to the next argument
	return true;
}


bool RetrieveControlFromArgs( /*in-out*/ resbuf*& pArgs, /*out*/ CDclControlObject*& pControl )
{
	if (!pArgs)
		return false; //arguments expected

	pControl = NULL;
	switch (pArgs->restype)	
	{
	// Uncomment this code to accept nil arguments without generating an exception
	//case RTNIL:
	//	pForm = NULL;
	//	break;
	case RTSHORT:
		pControl = (CDclControlObject*)pArgs->resval.rint;
		break;
	case RTLONG:
		pControl = (CDclControlObject*)pArgs->resval.rlong;
		break;
	case RTSTR:
		{
			LPCTSTR pszProjectName = pArgs->resval.rstring;
			if (!pszProjectName)
				return false; //invalid argument

			pArgs = pArgs->rbnext; //move to the next argument
			if (!pArgs)
				return false; //not enough arguments

			if (pArgs->restype != RTSTR)
				return false; //wrong argument type
			LPCTSTR pszFormName = pArgs->resval.rstring;
			if (!pszFormName)
				return false; //invalid argument

			pArgs = pArgs->rbnext; //move to the next argument
			if (!pArgs)
				return false; //not enough arguments

			if (pArgs->restype != RTSTR)
				return false; //wrong argument type
			pControl = theArxWorkspace.FindControl(pszProjectName, pszFormName, pArgs->resval.rstring);
			break;
		}
	default:
		return false; //wrong argument type
	}
	pArgs = pArgs->rbnext; //move to the next argument
	return true;
}


bool RetrieveDialogFromArgs( /*in-out*/ resbuf*& pArgs, /*out*/ CDialogObject*& pDialog )
{
	CDclFormObject* pForm = NULL;
	if (!RetrieveFormFromArgs (pArgs, pForm))
		return false;
	pDialog = pForm? pForm->GetFormInstance() : NULL;
	return true;
}


BOOL GetDiscSpace(LPCTSTR lpszPath, DWORDLONG *pnFree)
{
	BOOL bRet = FALSE;

	// We need to determine whether GetDiskFreeSpaceEx is available by calling LoadLibrary
	// or LoadLibraryEx, to load Kernel32.DLL, and then calling the GetProcAddress to
	// obtain an address for GetDiskFreeSpaceEx.  If GetProcAddress fails, or if
	// GetDiskFreeSpaceEx fails with the ERROR_CALL_NOT_IMPLEMENTED code, we use the
	// GetDiskFreeSpace function instead.

	typedef BOOL (__stdcall *_FPREGISTERSERVICEPROCESS)(LPCTSTR, PULARGE_INTEGER, PULARGE_INTEGER, PULARGE_INTEGER);

	_FPREGISTERSERVICEPROCESS m_pRegisterServiceProcess;
	HINSTANCE m_hDllInst;

	m_hDllInst = LoadLibrary(_T("Kernel32.DLL"));
	if (m_hDllInst) {
		ULARGE_INTEGER nTotalBytes, nTotalFreeBytes, nTotalAvailable;
		m_pRegisterServiceProcess = reinterpret_cast<_FPREGISTERSERVICEPROCESS>(::GetProcAddress(m_hDllInst, "GetDiskFreeSpaceExA"));

		if (m_pRegisterServiceProcess)
			m_pRegisterServiceProcess(lpszPath, &nTotalAvailable, &nTotalBytes, &nTotalFreeBytes);

		*pnFree = nTotalBytes.QuadPart;
		bRet = TRUE;

		FreeLibrary(m_hDllInst);
	}

	if(!bRet) {	// We have to try and use GetDiskFreeSpace()	
		ULONG secpercluster, bytespersec, nooffreeclusters, totalnoofclusters;	

		if(GetDiskFreeSpace(lpszPath, &secpercluster, &bytespersec, &nooffreeclusters, &totalnoofclusters)) {
			*pnFree = (totalnoofclusters * secpercluster * bytespersec);
			bRet = TRUE;
		}				
	}

	return bRet;
}


//-----------------------------------------------------------------------------
//----- ObjectARX EntryPoint
class CARXApp : public AcRxArxApp {

protected:
	T_PropertyIdSet mPropIds; //store the property ids used to define get/set lisp functions

	//dcl dialog state, probably should be stored in document state
	static CString msDialogToBeShown;
	static CPoint mptToBeShown;
	static CProject* mpProjectToBeShown;
	static CString msActionToBeShown;
	static CDclFormObject* mpDclToBeShown;
	static int mnDoneDialogValue;
	static int mnListOperation; //1 = change selection, 2 = append item, 3 = replace all (default)

public:
	size_t GetPropertyIdCount() const { return mPropIds.size(); }

protected:
	static int ads_dcl_GetCtrlProperty(void);
	static int ads_dcl_SetCtrlProperty(void);

public:
	CARXApp () : AcRxArxApp () {}

	virtual AcRx::AppRetCode On_kInitAppMsg (void *pkt) {
		// Load dependencies here

		// You *must* call On_kInitAppMsg here
		AcRx::AppRetCode retCode =AcRxArxApp::On_kInitAppMsg (pkt) ;
		
		CString sModulePath;
		DWORD cchPath = GetModuleFileName( _hdllInstance, sModulePath.GetBuffer( MAX_PATH ), MAX_PATH );
		sModulePath.ReleaseBuffer( cchPath );
		BOOL ret=acedRegisterExtendedTab( sModulePath, _T("OptionsDialog") );

		GetPropertyIdSet( mPropIds );

		DWORD dwMajor;
		DWORD dwMinor;
		DWORD dwThird;
		DWORD dwFourth;
		if( theWorkspace.GetModuleVersionInfo( dwMajor, dwMinor, dwThird, dwFourth, _hdllInstance ) )
			acutPrintf( theWorkspace.LoadResourceString( IDS_BANNER ), dwMajor, dwMinor, dwThird, dwFourth );

		return (retCode) ;
	}

	virtual AcRx::AppRetCode On_kUnloadAppMsg (void *pkt) {
		try
		{
			theArxWorkspace.CloseAllDialogs();
			theArxWorkspace.GetProjectHolder().ClearProjects();
		}
		catch(...)
		{
		}

		// You *must* call On_kUnloadAppMsg here
		AcRx::AppRetCode retCode =AcRxArxApp::On_kUnloadAppMsg (pkt) ;

		// Unload dependencies here

		return (retCode) ;
	}

	virtual AcRx::AppRetCode On_kInvkSubrMsg (void *pkt) {
		AcRx::AppRetCode retCode =AcRxArxApp::On_kInvkSubrMsg (pkt) ;

		assert( retCode == AcRx::kRetOK );
		if( retCode == AcRx::kRetOK )
		{
			int nFunctionCode = acedGetFunCode();
			if( nFunctionCode >= 0 && nFunctionCode < _elements(grAdsFunctionTable) )
			{
				int nRet = grAdsFunctionTable[nFunctionCode].pfHandler();
				assert( nRet == AcRx::kRetOK );
				return AcRx::kRetOK;
			}
			if( nFunctionCode >= ADSPROPFUNCBASE && nFunctionCode < ADSPROPFUNCBASE + nMaxPropertyId )
			{
				bool bSuccess = GetCtrlProperty( static_cast<PropertyId>(nFunctionCode - ADSPROPFUNCBASE) );
				assert( bSuccess == true );
				return AcRx::kRetOK;
			}
			int nSetPropertyBase = ADSPROPFUNCBASE + nMaxPropertyId;
			if( nFunctionCode >= nSetPropertyBase && nFunctionCode < nSetPropertyBase + nMaxPropertyId )
			{
				bool bSuccess = SetCtrlProperty( static_cast<PropertyId>(nFunctionCode - nSetPropertyBase) );
				assert( bSuccess == true );
				return AcRx::kRetOK;
			}
		}
		return (retCode) ;
	}

	virtual AcRx::AppRetCode On_kLoadDwgMsg (void *pkt) {
		AcRx::AppRetCode retCode =AcRxArxApp::On_kLoadDwgMsg (pkt) ;

		if( retCode == AcRx::kRetOK )
		{
			//register control specific ADS functions
			static const CString sPrefix = _T("dcl_");
			for( int idxFunction = 0; idxFunction < _elements(grAdsFunctionTable); ++idxFunction )
			{
				const AdsFunctionTableEntry& Entry = grAdsFunctionTable[idxFunction];
				acedDefun( sPrefix + Entry.pszFunctionName, ADSFUNCBASE + idxFunction );
				acedRegFunc( Entry.pfHandler, ADSFUNCBASE + idxFunction );
			}

			//register general property get and set functions
			static const CString sGetPrefix = sPrefix + _T("Control_Get");
			static const CString sSetPrefix = sPrefix + _T("Control_Set");
			for( T_PropertyIdSet::const_iterator iter = mPropIds.begin(); iter != mPropIds.end(); ++iter )
			{
				PropertyId id = *iter;
				LPCTSTR pszPropName = GetPropertyName( id );
				if( pszPropName && *pszPropName != _T('(') )
				{
					acedDefun( sGetPrefix + pszPropName, ADSPROPFUNCBASE + id );
					acedDefun( sSetPrefix + pszPropName, ADSPROPFUNCBASE + id + nMaxPropertyId );
					acedRegFunc( ads_dcl_GetCtrlProperty, ADSPROPFUNCBASE + id );
					acedRegFunc( ads_dcl_SetCtrlProperty, ADSPROPFUNCBASE + id + nMaxPropertyId );
				}
			}

			//set global lisp symbols
			theArxWorkspace.UpdateGlobalLispSymbols();
		}
		return (retCode) ;
	}

	virtual AcRx::AppRetCode On_kInitDialogMsg (void *pkt) {
		AcRx::AppRetCode retCode =AcRxArxApp::On_kInitDialogMsg (pkt) ;

		//find config panes in the loaded projects and add their tabs
		theArxWorkspace.OnExtendTabbedDialog( (CAdUiTabExtensionManager*)pkt );

		return (retCode) ;
	}

	virtual void RegisterServerComponents () {
	}


	// ----- OpenDCL.OpenDCL command
	static void OpenDCLOpenDCL(void)
	{  //do nothing, just need it here so that AutoCAD doesn't complain about an unknown command
	}

	// ----- ads_dcl_getversion symbol (do not rename)
	static int ads_dcl_getversion(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		if( pArgs )
			return RSERR; //no arguments allowed

		DWORD dwMajor;
		DWORD dwMinor;
		DWORD dwThird;
		DWORD dwFourth;
		if( !theWorkspace.GetModuleVersionInfo( dwMajor, dwMinor, dwThird, dwFourth, _hdllInstance ) )
			return RSERR;
		assert( dwMinor < 10 ); //otherwise the algorithm fails
		if( dwMinor >= 10 )
			return RSERR;
		acedRetReal( ads_real( dwMajor ) + dwMinor / 10.0 );

		return (RSRSLT) ;
	}

	// ----- ads_dcl_getversion symbol (do not rename)
	static int ads_dcl_getversionex(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		if( pArgs )
			return RSERR; //no arguments allowed

		DWORD dwMajor;
		DWORD dwMinor;
		DWORD dwThird;
		DWORD dwFourth;
		if( !theWorkspace.GetModuleVersionInfo( dwMajor, dwMinor, dwThird, dwFourth, _hdllInstance ) )
			return RSERR;
		CString sVersion;
		sVersion.Format( _T("%d.%d.%d.%d"), dwMajor, dwMinor, dwThird, dwFourth );
		acedRetStr( sVersion );

		return (RSRSLT) ;
	}

	// ----- ads_loadproject symbol (do not rename)
	static int ads_dcl_loadproject(void)
	{
		return ads_dcl_project_load();
	}

	// ----- ads_dcl_load_dialog symbol (do not rename)
	static int ads_dcl_load_dialog(void)
	{
		return ads_dcl_project_load () ;
	}

	// ----- ads_dcl_sendstring symbol (do not rename)
	static int ads_dcl_sendstring(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;
		if (pArgs == NULL)
			return RSERR; //argument expected

		if (pArgs->restype != RTSTR)
			return RSERR; //wrong argument type
		LPCTSTR pszStringToSend = pArgs->resval.rstring;

		if (pArgs->rbnext)
			return RSERR; //too many arguments

		AcApDocument* pDoc = acDocManager->curDocument();
		if (pDoc)
		{
			// give the command line focus (otherwise the command doesn't get processed immediately)
			CWnd* wndCommandLine = acedGetAcadDockCmdLine();
			if( wndCommandLine )
				wndCommandLine->SetFocus();		
			acDocManager->sendStringToExecute(pDoc, pszStringToSend, false, true, false);
		}

		return (RSRSLT) ;
	}

	// ----- ads_dcl_invokefunc symbol (do not rename)
	static int ads_dcl_invokefunc(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;
		if (pArgs == NULL)
			return RSERR; //argument expected

		if (pArgs->restype != RTSTR)
			return RSERR; //wrong argument type
		LPCTSTR pszTargetFunction = pArgs->resval.rstring;

		if (pArgs->rbnext)
			return RSERR; //too many arguments

		resbuf rbCallee = { NULL, RTSTR };
		rbCallee.resval.rstring = (ACHAR*)pszTargetFunction;
		resbuf* prbResult = NULL;
		if( acedInvoke (&rbCallee, &prbResult) == RTNORM )
			acedRetList (prbResult);
		acutRelRb (prbResult);

		return (RSRSLT) ;
	}

	// ----- ads_dcl_new_dialog symbol (do not rename)
	static int ads_dcl_new_dialog(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;
		if (pArgs == NULL)
			return RSERR; //argument expected

		if (pArgs->restype != RTSTR)
			return RSERR; //wrong argument type
		msDialogToBeShown = pArgs->resval.rstring;
		pArgs = pArgs->rbnext; //move to the next argument

		if (pArgs->restype != RTLONG)
			return RSERR; //wrong argument type
		mpProjectToBeShown = (CProject*)pArgs->resval.rlong;
		assert (mpProjectToBeShown != NULL);
		pArgs = pArgs->rbnext; //move to the next argument

		//optional arguments
		msActionToBeShown.Empty();
		mptToBeShown.SetPoint(-1, -1); //should be encapsulated in an instance class [ORW]
		if (pArgs)
		{
			if (pArgs->restype != RTSTR)
				return RSERR; //wrong argument type
			msActionToBeShown = pArgs->resval.rstring;
			pArgs = pArgs->rbnext; //move to the next argument

			if (pArgs)
			{
				if (pArgs->restype != RTPOINT)
					return RSERR; //wrong argument type

				mptToBeShown.x = pArgs->resval.rpoint[X];
				mptToBeShown.y = pArgs->resval.rpoint[Y];

				if (pArgs->rbnext)
					return RSERR; //too many arguments
			}
		}
		
		// get the dcl form object that will be displayed
		mpDclToBeShown = mpProjectToBeShown->FindDclForm(msDialogToBeShown);
		CDclControlObject *pProps = mpDclToBeShown->GetControlProperties();
		pProps->SetStringProperty(nFormEventInitialize, msActionToBeShown);
		
		TCHAR lpPathBuffer[MAX_PATH];
		::GetTempPath(MAX_PATH, lpPathBuffer);
		CString sTempFile = lpPathBuffer;
		sTempFile += gpszDclEventsLspFileName; /*"DclEvents.lsp"*/
		::DeleteFile(sTempFile);

		// create and open the action events lsp file to be loaded.
		CStdioFile tempFile(sTempFile, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeWrite);
		tempFile.WriteString(_T("; This temp file is used for managing the events of dialog boxes using\n"));
		tempFile.WriteString(_T("; DCL equivilent functions in ObjectDCL.\n"));
		tempFile.Close();

		acedRetT();

		return (RSRSLT) ;
	}

	// ----- ads_dcl_unload_dialog symbol (do not rename)
	static int ads_dcl_unload_dialog(void)
	{
		return ads_dcl_project_unload();
	}

	// ----- ads_dcl_done_dialog symbol (do not rename)
	static int ads_dcl_done_dialog(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		//optional arguments
		mnDoneDialogValue = 0;

		if (pArgs->restype != RTSHORT)
			return RSERR; //wrong argument type
		mnDoneDialogValue = pArgs->resval.rint;

		if (pArgs->rbnext)
			return RSERR; //too many arguments

		const CDialogObject *pDialog = mpDclToBeShown->GetFormInstance();
		if( !pDialog )
			return RSERR; //called on invalid dialog

		if( !pDialog->IsModeless() )
			pDialog->CloseDialog(mnDoneDialogValue); //end the modal dialog

		return (RSRSLT) ;
	}

	// ----- ads_dcl_start_dialog symbol (do not rename)
	static int ads_dcl_start_dialog(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;
		if (pArgs)
			return RSERR; //too many arguments

		TCHAR lpPathBuffer[MAX_PATH];
		::GetTempPath(MAX_PATH, lpPathBuffer);
		CString sTempFile = lpPathBuffer;
		sTempFile += gpszDclEventsLspFileName; /*"DclEvents.lsp"*/
		resbuf rbArg = {NULL, RTSTR};
		rbArg.resval.rstring = sTempFile.LockBuffer();
		resbuf rbCallee = {&rbArg, RTSTR};
		rbCallee.resval.rstring = _T("c:loadlisp");
		resbuf* prbResult = NULL;
		if (acedInvoke(&rbCallee, &prbResult) != RTNORM)
			return RSERR;
		acutRelRb(prbResult);

		// call method to display the requested form
		DialogParams params( mptToBeShown, CRect(0,0,0,0) );
		int nDialogId = theArxWorkspace.ActivateDclForm(mpDclToBeShown, &params);
	
		acedRetInt(mnDoneDialogValue);

		return (RSRSLT) ;
	}

	// ----- ads_dcl_start_list symbol (do not rename)
	static int ads_dcl_start_list(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;
		if (pArgs == NULL)
			return RSERR; //argument expected

		if (pArgs->restype != RTSTR)
			return RSERR; //wrong argument type
		LPCTSTR pszListKey = pArgs->resval.rstring;
		pArgs = pArgs->rbnext; //move to the next argument

		//optional arguments
		mnListOperation = 3;
		int nIndex = 0;
		if (pArgs)
		{
			if (pArgs->restype != RTSHORT)
				return RSERR; //wrong argument type
			mnListOperation = pArgs->resval.rint;
			pArgs = pArgs->rbnext; //move to the next argument

			if (pArgs)
			{
				if (pArgs->restype != RTSHORT)
					return RSERR; //wrong argument type

				nIndex = pArgs->resval.rint;

				if (pArgs->rbnext)
					return RSERR; //too many arguments
			}
		}

		CDclControlObject* pCtrl = mpDclToBeShown->FindControl(pszListKey);
		assert (pCtrl != NULL);
		if( !pCtrl)
			return RSERR;

		if (pCtrl->GetWindow() == NULL)
		{ //the control has not been created yet
			RefCountedPtr< CPropertyObject > pProp = pCtrl->GetPropertyObject(nList);
			assert(pProp != NULL);
			if (!pProp)
				return RSERR;
			pProp->GetStringArrayPtr()->clear();
		}
		else
		{ //we are changing an already created control
			switch (pCtrl->GetType())
			{
			case CtlListBox:
				{
					if (mnListOperation == 3)
						((CListBox*)pCtrl->GetWindow())->ResetContent();
					break;
				}
			case CtlComboBox:
				{
					if (mnListOperation == 3)
						((CComboBox*)pCtrl->GetWindow())->ResetContent();
					break;
				}
			case CtlListView:
				{
					if (mnListOperation == 3)
						((CListCtrl*)pCtrl->GetWindow())->DeleteAllItems();
					break;
				}
			}
		}

		return (RSRSLT) ;
	}

	// ----- ads_dcl_add_list symbol (do not rename)
	static int ads_dcl_add_list(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;
		if (pArgs == NULL)
			return RSERR; //argument expected

		if (pArgs->restype != RTSTR)
			return RSERR; //wrong argument type
		LPCTSTR pszListKey = pArgs->resval.rstring;
		pArgs = pArgs->rbnext; //move to the next argument

		if (!pArgs)
			return RSRSLT; //no error, just do nothing

		if (pArgs->restype != RTSTR)
			return RSERR; //wrong argument type
		LPCTSTR pszValue = pArgs->resval.rstring;
		if (!pszValue)
			pszValue = _T("");

		if (pArgs->rbnext)
			return RSERR; //too many arguments

		CDclControlObject* pCtrl = mpDclToBeShown->FindControl(pszListKey);
		assert (pCtrl != NULL);
		if( !pCtrl)
			return RSERR;

		if (pCtrl->GetWindow() == NULL)
		{ //the control has not been created yet
			RefCountedPtr< CPropertyObject > pProp = pCtrl->GetPropertyObject(nList);
			assert(pProp != NULL);
			if (!pProp)
				return RSERR;
			pProp->GetStringArrayPtr()->push_back(pszValue);
		}
		else
		{ //we are changing an already created control
			switch (pCtrl->GetType())
			{
			case CtlListBox:
				{
					CListBox *pListBox = (CListBox*)pCtrl->GetWindow();
					if (mnListOperation == 1)
					{
						int nIndex = pListBox->GetCurSel();
						pListBox->DeleteString(nIndex);
						pListBox->InsertString(nIndex, pszValue);
					}
					else
						pListBox->AddString(pszValue);
					break;
				}
			case CtlComboBox:
				{
					CComboBox *pComboBox = (CComboBox*)pCtrl->GetWindow();
					if (mnListOperation == 1)
					{
						int nIndex = pComboBox->GetCurSel();
						pComboBox->DeleteString(nIndex);
						pComboBox->InsertString(nIndex, pszValue);
					}
					else
						pComboBox->AddString(pszValue);
					break;
				}
			case CtlListView:
				{
					CListCtrl *pListCtrl = (CListCtrl*)pCtrl->GetWindow();
					if (mnListOperation == 1)
					{
						int nIndex = -1;
						POSITION pos = pListCtrl->GetFirstSelectedItemPosition();
						nIndex = pListCtrl->GetNextSelectedItem(pos);
						if (nIndex > -1)
							pListCtrl->SetItemText(nIndex, 0, pszValue);
						else
						{
							LV_ITEM lvItem;
							lvItem.mask = LVIF_TEXT|LVIF_INDENT|LVIF_IMAGE;
							lvItem.iItem = pListCtrl->GetItemCount() + 1;
							lvItem.iSubItem = 0;
							lvItem.pszText = (LPTSTR)pszValue;
							lvItem.iImage = -1;
							lvItem.iIndent = 0;			
							pListCtrl->InsertItem(&lvItem);
						}
					}
					else
					{
						LV_ITEM lvItem;
						lvItem.mask = LVIF_TEXT|LVIF_INDENT|LVIF_IMAGE;
						lvItem.iItem = pListCtrl->GetItemCount() + 1;
						lvItem.iSubItem = 0;
						lvItem.pszText = (LPTSTR)pszValue;
						lvItem.iImage = -1;
						lvItem.iIndent = 0;			
						pListCtrl->InsertItem(&lvItem);
					}
					break;
				}
			}
		}

		return (RSRSLT) ;
	}

	// ----- ads_dcl_end_list symbol (do not rename)
	static int ads_dcl_end_list(void)
	{
		//----- Remove the following line if you do not expect any argument for this ADS function
		struct resbuf *pArgs =acedGetArgs () ;

		if (pArgs)
			return RSERR; //too many arguments

		return (RSRSLT) ;
	}

	// ----- ads_dcl_action_tile symbol (do not rename)
	static int ads_dcl_action_tile(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;
		if (pArgs == NULL)
			return RSERR; //argument expected

		if (pArgs->restype != RTSTR)
			return RSERR; //wrong argument type
		LPCTSTR pszKey = pArgs->resval.rstring;
		pArgs = pArgs->rbnext; //move to the next argument

		if (!pArgs)
			return RSRSLT; //no error, just do nothing

		if (pArgs->restype != RTSTR)
			return RSERR; //wrong argument type
		LPCTSTR pszAction = pArgs->resval.rstring;
		if (!pszAction)
			pszAction = _T("");

		if (pArgs->rbnext)
			return RSERR; //too many arguments

		CDclControlObject* pCtrl = mpDclToBeShown->FindControl(pszKey);
		assert (pCtrl != NULL);
		if( !pCtrl)
			return RSERR;

		CString sAction;
		sAction.Format(gpszOnActionEventLispFunction, pszKey, pszAction);
		pCtrl->m_bEventsAsAction = true;

		switch(pCtrl->GetType())
		{
		case CtlStdButton:
		case CtlCheckBox:
		case CtlGraphicButton:
		case CtlOptionButton:
		case CtlSlideView:
			{
				pCtrl->SetStringProperty(nEventClicked, sAction);
				break;
			}
		case CtlTextBox:
			{
				pCtrl->SetStringProperty(nEventEditChanged, sAction);
				break;
			}		
		case CtlGrid:
		case CtlListView:
			{
				pCtrl->SetStringProperty(nEventClicked, sAction);
				break;
			}
		case CtlComboBox:
		case CtlListBox:
			{
				pCtrl->SetStringProperty(nEventSelChanged, sAction);
				break;
			}	
		case CtlScrollBar:
			{
				pCtrl->SetStringProperty(nEventScroll, sAction);
				break;
			}
		}

		TCHAR lpPathBuffer[4096];
		::GetTempPath(4096, lpPathBuffer);
		CString sTempFile = lpPathBuffer;
		sTempFile += gpszDclEventsLspFileName; /*"DclEvents.lsp"*/
			
		// create and open the action events lsp file to be loaded.
		CStdioFile tempFile(sTempFile, CFile::modeWrite);
		tempFile.SeekToEnd();
		tempFile.WriteString(sAction);
		tempFile.Close();

		acedRetT();

		return (RSRSLT) ;
	}

	// ----- ads_dcl_set_tile symbol (do not rename)
	static int ads_dcl_set_tile(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;
		if (pArgs == NULL)
			return RSERR; //argument expected

		if (pArgs->restype != RTSTR)
			return RSERR; //wrong argument type
		LPCTSTR pszKey = pArgs->resval.rstring;
		pArgs = pArgs->rbnext; //move to the next argument

		if (!pArgs)
			return RSRSLT; //no error, just do nothing

		if (pArgs->restype != RTSTR)
			return RSERR; //wrong argument type
		LPCTSTR pszValue = pArgs->resval.rstring;
		if (!pszValue)
			pszValue = _T("");

		if (pArgs->rbnext)
			return RSERR; //too many arguments

		CDclControlObject* pCtrl = mpDclToBeShown->FindControl(pszKey);
		assert (pCtrl != NULL);
		if( !pCtrl)
			return RSERR;

		pCtrl->SetStringProperty(nText, pszValue);
		if (pCtrl->GetWindow())
			pCtrl->GetWindow()->SetWindowText(pszValue);

		return (RSRSLT) ;
	}

	// ----- ads_dcl_mode_tile symbol (do not rename)
	static int ads_dcl_mode_tile(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;
		if (pArgs == NULL)
			return RSERR; //argument expected

		if (pArgs->restype != RTSTR)
			return RSERR; //wrong argument type
		LPCTSTR pszKey = pArgs->resval.rstring;
		pArgs = pArgs->rbnext; //move to the next argument

		if (!pArgs)
			return RSERR; //not enough arguments

		if (pArgs->restype != RTSHORT)
			return RSERR; //wrong argument type
		int nValue = pArgs->resval.rint;

		if (pArgs->rbnext)
			return RSERR; //too many arguments

		CDclControlObject* pCtrl = mpDclToBeShown->FindControl(pszKey);
		assert (pCtrl != NULL);
		if( !pCtrl)
			return RSERR;

		if (pCtrl->GetWindow() == NULL)
		{ //the control has not been created yet
			switch (nValue)
			{
			case 0:
				{
					pCtrl->SetBooleanProperty(nEnabled, true);
					break;
				}		
			case 1:
				{
					pCtrl->SetBooleanProperty(nEnabled, false);
					break;
				}		
			}
		}
		else
		{ //we are changing an already created control
			switch (nValue)
			{
			case 0:
				{
					pCtrl->SetBooleanProperty(nEnabled, true);
					pCtrl->GetWindow()->EnableWindow(TRUE);
					break;
				}		
			case 1:
				{
					pCtrl->SetBooleanProperty(nEnabled, false);
					pCtrl->GetWindow()->EnableWindow(FALSE);
					break;
				}
			case 2:
				{
					pCtrl->GetWindow()->SetFocus();
					break;
				}		
			case 3:
				{
					if (pCtrl->GetType() == CtlTextBox)
					{
						CEdit *pEdit = (CEdit*)pCtrl->GetWindow();
						pEdit->SetSel(0, -1);
					}
					break;
				}		
			case 4:
				{
					if (pCtrl->GetType() == CtlSlideView)
					{
						CSlideHolder *pSlide = (CSlideHolder*)pCtrl->GetWindow();
						pSlide->m_bSelectedRect = !pSlide->m_bSelectedRect;
						pSlide->Invalidate();
					}
					break;
				}
			}
		}

		return (RSRSLT) ;
	}

	// ----- ads_dcl_get_tile symbol (do not rename)
	static int ads_dcl_get_tile(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;
		if (pArgs == NULL)
			return RSERR; //argument expected

		if (pArgs->restype != RTSTR)
			return RSERR; //wrong argument type
		LPCTSTR pszKey = pArgs->resval.rstring;

		if (pArgs->rbnext)
			return RSERR; //too many arguments

		CDclControlObject* pCtrl = mpDclToBeShown->FindControl(pszKey);
		assert (pCtrl != NULL);
		if( !pCtrl)
			return RSERR;

		if (pCtrl->GetWindow() == NULL)
		{ //the control has not been created yet
			acedRetStr(pCtrl->GetStrProperty(nText));
		}
		else
		{ //we are changing an already created control
			CString sValue;
			pCtrl->GetWindow()->GetWindowText(sValue);
			acedRetStr(sValue);
		}

		return (RSRSLT) ;
	}

	// ----- ads_dcl_get_attr symbol (do not rename)
	static int ads_dcl_get_attr(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		acedAlert(_T("(dcl_Get_Attr) is not implemented yet!"));

		return (RSRSLT) ;
	}

	// ----- ads_dcl_start_image symbol (do not rename)
	static int ads_dcl_start_image(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;
		if (pArgs == NULL)
			return RSERR; //argument expected

		if (pArgs->restype != RTSTR)
			return RSERR; //wrong argument type
		LPCTSTR pszKey = pArgs->resval.rstring;

		if (pArgs->rbnext)
			return RSERR; //too many arguments

		CDclControlObject* pCtrl = mpDclToBeShown->FindControl(pszKey);
		assert (pCtrl != NULL);
		if( !pCtrl)
			return RSERR;

		((CSlideHolder*)pCtrl->GetWindow())->Clear();

		return (RSRSLT) ;
	}

	// ----- ads_dcl_vector_image symbol (do not rename)
	static int ads_dcl_vector_image(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;
		if (pArgs == NULL)
			return RSERR; //argument expected

		if (pArgs->restype != RTSTR)
			return RSERR; //wrong argument type
		LPCTSTR pszKey = pArgs->resval.rstring;
		pArgs = pArgs->rbnext; //move to the next argument

		if (!pArgs)
			return RSERR; //not enough arguments

		if (pArgs->restype != RTSHORT)
			return RSERR; //wrong argument type
		int nStartX = pArgs->resval.rint;
		pArgs = pArgs->rbnext; //move to the next argument

		if (!pArgs)
			return RSERR; //not enough arguments

		if (pArgs->restype != RTSHORT)
			return RSERR; //wrong argument type
		int nStartY = pArgs->resval.rint;
		pArgs = pArgs->rbnext; //move to the next argument

		if (!pArgs)
			return RSERR; //not enough arguments

		if (pArgs->restype != RTSHORT)
			return RSERR; //wrong argument type
		int nEndX = pArgs->resval.rint;
		pArgs = pArgs->rbnext; //move to the next argument

		if (!pArgs)
			return RSERR; //not enough arguments

		if (pArgs->restype != RTSHORT)
			return RSERR; //wrong argument type
		int nEndY = pArgs->resval.rint;
		pArgs = pArgs->rbnext; //move to the next argument

		if (!pArgs)
			return RSERR; //not enough arguments

		if (pArgs->restype != RTSHORT)
			return RSERR; //wrong argument type
		int nLineColor = pArgs->resval.rint;
		pArgs = pArgs->rbnext; //move to the next argument

		if (pArgs->rbnext)
			return RSERR; //too many arguments

		CDclControlObject* pCtrl = mpDclToBeShown->FindControl(pszKey);
		assert (pCtrl != NULL);
		if( !pCtrl)
			return RSERR;

		switch (nLineColor)
		{
		case -2:
			nLineColor = -22;
			break;
		case -15:
			nLineColor = -16;
			break;
		case -16:
			nLineColor = -9;
			break;
		case -18:
			nLineColor = -17;
			break;
		}

		((CSlideHolder*)pCtrl->GetWindow())->DrawLine(nStartX, nStartY, nEndX, nEndY, nLineColor);

		return (RSRSLT) ;
	}

	// ----- ads_dcl_fill_image symbol (do not rename)
	static int ads_dcl_fill_image(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;
		if (pArgs == NULL)
			return RSERR; //argument expected

		if (pArgs->restype != RTSTR)
			return RSERR; //wrong argument type
		LPCTSTR pszKey = pArgs->resval.rstring;
		pArgs = pArgs->rbnext; //move to the next argument

		if (!pArgs)
			return RSERR; //not enough arguments

		if (pArgs->restype != RTSHORT)
			return RSERR; //wrong argument type
		int nStartX = pArgs->resval.rint;
		pArgs = pArgs->rbnext; //move to the next argument

		if (!pArgs)
			return RSERR; //not enough arguments

		if (pArgs->restype != RTSHORT)
			return RSERR; //wrong argument type
		int nStartY = pArgs->resval.rint;
		pArgs = pArgs->rbnext; //move to the next argument

		if (!pArgs)
			return RSERR; //not enough arguments

		if (pArgs->restype != RTSHORT)
			return RSERR; //wrong argument type
		int nEndX = pArgs->resval.rint;
		pArgs = pArgs->rbnext; //move to the next argument

		if (!pArgs)
			return RSERR; //not enough arguments

		if (pArgs->restype != RTSHORT)
			return RSERR; //wrong argument type
		int nEndY = pArgs->resval.rint;
		pArgs = pArgs->rbnext; //move to the next argument

		if (!pArgs)
			return RSERR; //not enough arguments

		if (pArgs->restype != RTSHORT)
			return RSERR; //wrong argument type
		int nLineColor = pArgs->resval.rint;
		pArgs = pArgs->rbnext; //move to the next argument

		if (pArgs->rbnext)
			return RSERR; //too many arguments

		CDclControlObject* pCtrl = mpDclToBeShown->FindControl(pszKey);
		assert (pCtrl != NULL);
		if( !pCtrl)
			return RSERR;

		switch (nLineColor)
		{
		case -2:
			nLineColor = -22;
			break;
		case -15:
			nLineColor = -16;
			break;
		case -16:
			nLineColor = -9;
			break;
		case -18:
			nLineColor = -17;
			break;
		}

		((CSlideHolder*)pCtrl->GetWindow())->DrawFillRect(nStartX, nStartY, nStartX + nEndX, nStartY + nEndY, nLineColor);

		return (RSRSLT) ;
	}

	// ----- ads_dcl_dimx_tile symbol (do not rename)
	static int ads_dcl_dimx_tile(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;
		if (pArgs == NULL)
			return RSERR; //argument expected

		if (pArgs->restype != RTSTR)
			return RSERR; //wrong argument type
		LPCTSTR pszKey = pArgs->resval.rstring;

		if (pArgs->rbnext)
			return RSERR; //too many arguments

		CDclControlObject* pCtrl = mpDclToBeShown->FindControl(pszKey);
		assert (pCtrl != NULL);
		if( !pCtrl)
			return RSERR;

		CRect rc;
		((CSlideHolder*)pCtrl->GetWindow())->GetClientRect(&rc);
		acedRetInt(rc.Width());

		return (RSRSLT) ;
	}

	// ----- ads_dcl_dimy_tile symbol (do not rename)
	static int ads_dcl_dimy_tile(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;
		if (pArgs == NULL)
			return RSERR; //argument expected

		if (pArgs->restype != RTSTR)
			return RSERR; //wrong argument type
		LPCTSTR pszKey = pArgs->resval.rstring;

		if (pArgs->rbnext)
			return RSERR; //too many arguments

		CDclControlObject* pCtrl = mpDclToBeShown->FindControl(pszKey);
		assert (pCtrl != NULL);
		if( !pCtrl)
			return RSERR;

		CRect rc;
		((CSlideHolder*)pCtrl->GetWindow())->GetClientRect(&rc);
		acedRetInt(rc.Height());

		return (RSRSLT) ;
	}

	// ----- ads_dcl_end_image symbol (do not rename)
	static int ads_dcl_end_image(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;
		if (pArgs == NULL)
			return RSERR; //argument expected

		if (pArgs->restype != RTSTR)
			return RSERR; //wrong argument type
		LPCTSTR pszKey = pArgs->resval.rstring;

		if (pArgs->rbnext)
			return RSERR; //too many arguments

		CDclControlObject* pCtrl = mpDclToBeShown->FindControl(pszKey);
		assert (pCtrl != NULL);
		if( !pCtrl)
			return RSERR;

		((CSlideHolder*)pCtrl->GetWindow())->CopyDC();

		return (RSRSLT) ;
	}

	// ----- ads_dcl_slide_image symbol (do not rename)
	static int ads_dcl_slide_image(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;
		if (pArgs == NULL)
			return RSERR; //argument expected

		if (pArgs->restype != RTSTR)
			return RSERR; //wrong argument type
		LPCTSTR pszKey = pArgs->resval.rstring;
		pArgs = pArgs->rbnext; //move to the next argument

		if (!pArgs)
			return RSERR; //not enough arguments

		if (pArgs->restype != RTSHORT)
			return RSERR; //wrong argument type
		int nX = pArgs->resval.rint;
		pArgs = pArgs->rbnext; //move to the next argument

		if (!pArgs)
			return RSERR; //not enough arguments

		if (pArgs->restype != RTSHORT)
			return RSERR; //wrong argument type
		int nY = pArgs->resval.rint;
		pArgs = pArgs->rbnext; //move to the next argument

		if (!pArgs)
			return RSERR; //not enough arguments

		if (pArgs->restype != RTSHORT)
			return RSERR; //wrong argument type
		int nWidth = pArgs->resval.rint;
		pArgs = pArgs->rbnext; //move to the next argument

		if (!pArgs)
			return RSERR; //not enough arguments

		if (pArgs->restype != RTSHORT)
			return RSERR; //wrong argument type
		int nHeight = pArgs->resval.rint;
		pArgs = pArgs->rbnext; //move to the next argument

		if (!pArgs)
			return RSERR; //not enough arguments

		if (pArgs->restype != RTSTR)
			return RSERR; //wrong argument type
		CString sFilename = pArgs->resval.rstring;
		pArgs = pArgs->rbnext; //move to the next argument

		if (pArgs->rbnext)
			return RSERR; //too many arguments

		CDclControlObject* pCtrl = mpDclToBeShown->FindControl(pszKey);
		assert (pCtrl != NULL);
		if( !pCtrl)
			return RSERR;

		int nBracket = sFilename.Find(_T('('));
		if (nBracket == -1)
		{ //no slide library specified
			static const LPCTSTR pszSlideFileExt = _T(".sld");
			if (sFilename.Right(4).CompareNoCase(pszSlideFileExt) != 0)
				sFilename += pszSlideFileExt;
			CString sPath = theWorkspace.FindFile(sFilename);
			if (sPath.IsEmpty())
			{
				theWorkspace.DisplayAlert(CString(ErrorFileNotFound) + "\n" + sFilename);
				acedRetInt(-1);
				return RSRSLT;
			}
			((CSlideHolder*)pCtrl->GetWindow())->DrawASlide(nX, nY, nWidth, nHeight, sPath);
		}
		else
		{ //displaying a slide library
			CString sLibName = sFilename.Mid(nBracket + 1, sFilename.GetLength() - nBracket - 2);
			sFilename = sFilename.Left(nBracket - 1);
			static const LPCTSTR pszSlideLibFileExt = _T(".slb");
			if (sFilename.Right(4).CompareNoCase(pszSlideLibFileExt) != 0)
				sFilename += pszSlideLibFileExt;
			CString sPath = theWorkspace.FindFile(sFilename);
			if (sPath.IsEmpty())
			{
				theWorkspace.DisplayAlert(CString(ErrorFileNotFound) + "\n" + sFilename);
				acedRetInt(-1);
				return RSRSLT;
			}
			((CSlideHolder*)pCtrl->GetWindow())->DrawASlide(nX, nY, nWidth, nHeight, sPath, sLibName);
		}

		return (RSRSLT) ;
	}

	// ----- ads_dcl_getlinetype symbol (do not rename)
	static int ads_dcl_getlinetype(void)
	{
		//----- Remove the following line if you do not expect any argument for this ADS function
		struct resbuf *pArgs =acedGetArgs () ;

		if (pArgs)
			return RSERR; //too many arguments

		AcDbObjectId id;
		CString sLinetype;
		if (SelectLinetypeUI(id, sLinetype))
			acedRetStr(sLinetype);

		return (RSRSLT) ;
	}

	// ----- ads_dcl_getlineweight symbol (do not rename)
	static int ads_dcl_getlineweight(void)
	{
		//----- Remove the following line if you do not expect any argument for this ADS function
		struct resbuf *pArgs =acedGetArgs () ;

		AcDb::LineWeight newLW = AcDb::kLnWtByLwDefault;
		if (pArgs)
		{
			switch (pArgs->restype)	
			{
			case RTSHORT:
				newLW = (AcDb::LineWeight)pArgs->resval.rint;
				break;
			case RTLONG:
				newLW = (AcDb::LineWeight)pArgs->resval.rlong;
				break;
			case RTREAL:
				newLW = (AcDb::LineWeight)(long)pArgs->resval.rreal;
				break;
			default:
				return RSERR; //wrong argument type
			}

			if (pArgs->rbnext)
				return RSERR; //too many arguments
		}

		if (SelectLineWeightUI(newLW))
			acedRetLong((LONG)newLW);

		return (RSRSLT) ;
	}

	// ----- ads_dcl_form_show symbol (do not rename)
	static int ads_dcl_form_show(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		CDclFormObject* pDclObject = NULL;
		if (!RetrieveFormFromArgs (pArgs, pDclObject))
			return RSERR; //invalid input

		if (!pDclObject)
			return RSRSLT; //form not found

		//optional arguments
		int nX = -1;
		int nY = -1;
		LPCTSTR pszDefaultDirectory = NULL;
		LPCTSTR pszDefaultFileName = NULL;
		LPCTSTR pszDefaultExtension = NULL;
		if (pArgs)
		{
			if (pArgs->restype == RTSTR)
			{
				pszDefaultDirectory = pArgs->resval.rstring;

				pArgs = pArgs->rbnext; //move to the next argument
				if (pArgs)
				{
					if (pArgs->restype != RTSTR)
						return RSERR; //wrong argument type
					pszDefaultFileName = pArgs->resval.rstring;

					pArgs = pArgs->rbnext;
					if (pArgs)
					{
						if (pArgs->restype != RTSTR)
							return RSERR; //wrong argument type
						pszDefaultExtension = pArgs->resval.rstring;
					}
				}
			}
			else if (pArgs->restype == RTSHORT)
			{
				nX = pArgs->resval.rint;

				pArgs = pArgs->rbnext; //move to the next argument
				if (!pArgs)
					return RSERR; //not enough arguments

				if (pArgs->restype != RTSHORT)
					return RSERR; //wrong argument type
				nY = pArgs->resval.rint;
			}
			else
				return RSERR; //wrong argument type

			if (pArgs->rbnext)
				return RSERR; //too many arguments
		}

		// call method to display the requested form
		FileDialogParams fdp( TRUE, NULL, NULL, 0, NULL );
		bool bHasFileParams = (pDclObject->GetType() == VdclFileDialog);
		if( bHasFileParams )
		{
			CString sFilename = pszDefaultDirectory;
			if( !sFilename.IsEmpty() && !sFilename.Right(1).SpanExcluding( _T("\\/") ).IsEmpty() )
				sFilename += _T('\\');
			sFilename += pszDefaultFileName;
			fdp.sFilename = sFilename;
			fdp.sDefaultExtension = pszDefaultExtension;
		}
		DialogParams params( CPoint( nX, nY ), CRect(0,0,0,0), bHasFileParams? (LPARAM)&fdp : NULL );
		int nResult = theArxWorkspace.ActivateDclForm(pDclObject, &params);

		if (nResult >= 0)
		{
			if (pDclObject->GetType() == VdclFileDialog)
			{
				if( (fdp.dwFlags & OFN_ALLOWMULTISELECT) )
				{
					// Convert the array to a list that can be returned
					struct resbuf* prbRetList = NULL;
					struct resbuf* prbTail = NULL;

					INT_PTR nCount = fdp.rsFilenames.GetSize();
					for (INT_PTR i = 0; i < nCount; i++)
					{
						// get the text name of the selected line number
						CString sFileName = fdp.rsFilenames.GetAt(i);
						if( prbTail )
						{
							prbTail->rbnext = acutNewRb(RTSTR);
							prbTail = prbTail->rbnext;
						}
						else
						{
							prbRetList = acutNewRb(RTSTR);
							prbTail = prbRetList;
						}
						acutNewString(sFileName, prbTail->resval.rstring);
					}

					acedRetList(prbRetList);
					acutRelRb(prbRetList);
				}
				else
					acedRetStr( fdp.sFilename );
			}
			else
				acedRetInt(nResult);
		}

		return (RSRSLT) ;
	}

	// ----- ads_dcl_form_center symbol (do not rename)
	static int ads_dcl_form_center(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		CDialogObject* pDialog = NULL;
		if (!RetrieveDialogFromArgs (pArgs, pDialog))
			return RSERR; //arguments expected
		if (!pDialog)
			return RSRSLT; //dialog not found

		//optional arguments
		int nX = -1;
		int nY = -1;
		if (pArgs)
		{
			if (pArgs->restype == RTSHORT)
			{
				nX = pArgs->resval.rint;
				pArgs = pArgs->rbnext; //move to the next argument

				if (!pArgs)
					return RSERR; //not enough arguments

				if (pArgs->restype != RTSHORT)
					return RSERR; //wrong argument type
				nY = pArgs->resval.rint;
			}
			else
				return RSERR; //wrong argument type

			if (pArgs->rbnext)
				return RSERR; //too many arguments
		}

		if (nX <= 0 || nY <= 0)
		{
			if( pDialog->CenterDialog() )
				acedRetT();
		}
		else
		{
			if( pDialog->CenterAndResizeDialog( nX, nY ) )
				acedRetT();
		}

		return (RSRSLT) ;
	}

	// ----- ads_dcl_form_resize symbol (do not rename)
	static int ads_dcl_form_resize(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		CDialogObject* pDialog = NULL;
		if (!RetrieveDialogFromArgs (pArgs, pDialog))
			return RSERR; //arguments expected
		if (!pDialog)
			return RSRSLT; //dialog not found

		//optional arguments
		int nNewWidth = -1;
		int nNewHeight = -1;
		if (pArgs)
		{
			if (pArgs->restype == RTSHORT)
			{
				nNewWidth = pArgs->resval.rint;
				pArgs = pArgs->rbnext; //move to the next argument

				if (!pArgs)
					return RSERR; //not enough arguments

				if (pArgs->restype != RTSHORT)
					return RSERR; //wrong argument type
				nNewHeight = pArgs->resval.rint;
			}
			else
				return RSERR; //wrong argument type

			if (pArgs->rbnext)
				return RSERR; //too many arguments
		}

		if( pDialog->ResizeDialog( nNewWidth, nNewHeight ) )
			acedRetT();

		return (RSRSLT) ;
	}

	// ----- ads_dcl_form_setfocus symbol (do not rename)
	static int ads_dcl_form_setfocus(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		CDialogObject* pDialog = NULL;
		if (!RetrieveDialogFromArgs (pArgs, pDialog))
			return RSERR; //arguments expected
		if (!pDialog)
			return RSRSLT; //dialog not found

		if (pArgs)
			return RSERR; //too many arguments

		if (pDialog->SetFocus())
			acedRetT();

		return (RSRSLT) ;
	}

	// ----- ads_dcl_form_hide symbol (do not rename)
	static int ads_dcl_form_hide(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		CDialogObject* pDialog = NULL;
		if (!RetrieveDialogFromArgs (pArgs, pDialog))
			return RSERR; //arguments expected
		if (!pDialog)
			return RSRSLT; //dialog not found

		//optional arguments
		bool bHide = true;
		if (pArgs)
		{
			bHide = (pArgs->restype != RTNIL);
			if( bHide && pArgs->restype == RTSHORT && pArgs->resval.rint == 0 )
				bHide = false; //special case (for legacy behavior): a zero integer is interpreted as unhide

			if (pArgs->rbnext)
				return RSERR; //too many arguments
		}

		if (pDialog->Show( !bHide ))
			acedRetT();

		return (RSRSLT) ;
	}

	// ----- ads_dcl_form_close symbol (do not rename)
	static int ads_dcl_form_close(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		CDialogObject* pDialog = NULL;
		if (!RetrieveDialogFromArgs (pArgs, pDialog))
			return RSERR; //arguments expected
		if (!pDialog)
			return RSRSLT; //dialog not found

		int nStatus = 0;
		if (pArgs)
		{
			if (pArgs->restype != RTSHORT)
				return RSERR; //wrong argument type
			nStatus = pArgs->resval.rint;

			if (pArgs->rbnext)
				return RSERR; //too many arguments
		}

		CRect rcDlg;
		pDialog->GetWindowRect( rcDlg );
		ads_point ptUL = { rcDlg.left, rcDlg.top, 0 };
		acedRetPoint( ptUL ); //return the upper left corner as a 2D point

		pDialog->CloseDialog( nStatus );

		return (RSRSLT) ;
	}

	// ----- ads_dcl_form_getrectangle symbol (do not rename)
	static int ads_dcl_form_getrectangle(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		CDialogObject* pDialog = NULL;
		if (!RetrieveDialogFromArgs (pArgs, pDialog))
			return RSERR; //arguments expected
		if (!pDialog)
			return RSRSLT; //dialog not found

		if (pArgs)
			return RSERR; //too many arguments

		CRect rcDlg(0, 0, 0, 0);
		if( pDialog->GetWindowRect( rcDlg ) )
		{
			resbuf rbHeight = {NULL, RTSHORT};
			rbHeight.resval.rint = rcDlg.Height();
			resbuf rbWidth = {&rbHeight, RTSHORT};
			rbWidth.resval.rint = rcDlg.Width();
			resbuf rbTop = {&rbWidth, RTSHORT};
			rbTop.resval.rint = rcDlg.top;
			resbuf rbLeft = {&rbTop, RTSHORT};
			rbLeft.resval.rint = rcDlg.left;
			acedRetList(&rbLeft);		
		}

		return (RSRSLT) ;
	}

	// ----- ads_dcl_form_getcontrolarea symbol (do not rename)
	static int ads_dcl_form_getcontrolarea(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		CDialogObject* pDialog = NULL;
		if (!RetrieveDialogFromArgs (pArgs, pDialog))
			return RSERR; //arguments expected
		if (!pDialog)
			return RSRSLT; //dialog not found

		if (pArgs)
			return RSERR; //too many arguments

		CRect rcDlg(0, 0, 0, 0);
		if (pDialog->GetClientRect(rcDlg))
		{
			resbuf rbHeight = {NULL, RTSHORT};
			rbHeight.resval.rint = rcDlg.Height();
			resbuf rbWidth = {&rbHeight, RTSHORT};
			rbWidth.resval.rint = rcDlg.Width();
			acedRetList(&rbWidth);		
		}

		return (RSRSLT) ;
	}

	// ----- ads_dcl_form_closeall symbol (do not rename)
	static int ads_dcl_form_closeall(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		//optional arguments
		DWORD fMask = -1;
		if (pArgs)
		{
			if (pArgs->restype != RTSHORT)
				return RSERR; //wrong argument type

			fMask = pArgs->resval.rint;

			if (pArgs->rbnext)
				return RSERR; //too many arguments
		}

		theArxWorkspace.CloseAllDialogs( fMask );

		return (RSRSLT) ;
	}

	// ----- ads_dcl_form_isfloating symbol (do not rename)
	static int ads_dcl_form_isfloating(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		CDialogObject* pDialog = NULL;
		if (!RetrieveDialogFromArgs (pArgs, pDialog))
			return RSERR; //arguments expected
		if (!pDialog)
			return RSRSLT; //dialog not found

		if (pArgs)
			return RSERR; //too many arguments

		if (pDialog->IsFloating())
			acedRetT();
		else
			acedRetNil();

		return (RSRSLT) ;
	}

	// ----- ads_dcl_form_isactive symbol (do not rename)
	static int ads_dcl_form_isactive(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		CDialogObject* pDialog = NULL;
		if (!RetrieveDialogFromArgs (pArgs, pDialog))
			return RSERR; //arguments expected
		if (!pDialog)
			return RSRSLT; //dialog not found

		if (pArgs)
			return RSERR; //too many arguments

		if (pDialog->GetHWnd()/* && CWnd::FromHandle( pDialog->GetHWnd() )->IsWindowVisible()*/)
			acedRetT();

		return (RSRSLT) ;
	}

	// ----- ads_dcl_configtab_isapplyenabled symbol (do not rename)
	static int ads_dcl_configtab_isapplyenabled(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		CDialogObject* pDialog = NULL;
		if (!RetrieveDialogFromArgs (pArgs, pDialog))
			return RSERR; //arguments expected
		if (!pDialog)
			return RSRSLT; //dialog not found

		if (pArgs)
			return RSERR; //too many arguments

		if (pDialog->IsDirty())
			acedRetT();

		return (RSRSLT) ;
	}

	// ----- ads_dcl_configtab_setapplyenabled symbol (do not rename)
	static int ads_dcl_configtab_setapplyenabled(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		CDialogObject* pDialog = NULL;
		if (!RetrieveDialogFromArgs (pArgs, pDialog))
			return RSERR; //arguments expected
		if (!pDialog)
			return RSRSLT; //dialog not found

		if (pArgs)
			return RSERR; //too many arguments

		if (pDialog->SetDirty())
			acedRetT();

		return (RSRSLT) ;
	}

	// ----- ads_dcl_getscreensize symbol (do not rename)
	static int ads_dcl_getscreensize(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;
		if (pArgs)
			return RSERR; //no arguments expected

		resbuf rbScreenHeight = {NULL, RTSHORT};
		rbScreenHeight.resval.rint = ::GetSystemMetrics(SM_CYSCREEN);
		resbuf rbScreenWidth = {&rbScreenHeight, RTSHORT};
		rbScreenWidth.resval.rint = ::GetSystemMetrics(SM_CXSCREEN);
		acedRetList(&rbScreenWidth);

		return (RSRSLT) ;
	}

	// ----- ads_dcl_form_gettitlebartext symbol (do not rename)
	static int ads_dcl_form_gettitlebartext(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		CDialogObject* pDialog = NULL;
		if (!RetrieveDialogFromArgs (pArgs, pDialog))
			return RSERR; //arguments expected
		if (!pDialog)
			return RSRSLT; //dialog not found

		if (pArgs)
			return RSERR; //too many arguments

		CDclControlObject* pPropObj = pDialog->GetSourceForm()->GetControlProperties();
		assert(pPropObj != NULL);
		if(pPropObj)
			acedRetStr(pPropObj->GetStrProperty(nTitleBarText));

		return (RSRSLT) ;
	}

	// ----- ads_dcl_form_settitlebartext symbol (do not rename)
	static int ads_dcl_form_settitlebartext(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		CDialogObject* pDialog = NULL;
		if (!RetrieveDialogFromArgs (pArgs, pDialog))
			return RSERR; //arguments expected
		if (!pDialog)
			return RSRSLT; //dialog not found

		//optional arguments
		LPCTSTR pszTitle = NULL;
		if (pArgs)
		{
			if (pArgs->restype != RTSTR)
				return RSERR; //wrong argument type
			pszTitle = pArgs->resval.rstring;

			if (pArgs->rbnext)
				return RSERR; //too many arguments
		}

		CDclControlObject *pPropObj = pDialog->GetSourceForm()->GetControlProperties();
		pPropObj->SetStringProperty(nTitleBarText, pszTitle);
		::SetWindowText( pDialog->GetHWnd(), pszTitle );

		return (RSRSLT) ;
	}

	// ----- ads_dcl_messagebox symbol (do not rename)
	static int ads_dcl_messagebox(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;
		if (pArgs == NULL)
			return RSERR; //argument expected

		LPCTSTR pszMessage = NULL;
		if (pArgs->restype != RTSTR)
			return RSERR; //wrong argument type
		pszMessage = pArgs->resval.rstring;
		pArgs = pArgs->rbnext; //move to the next argument

		//optional arguments
		LPCTSTR pszTitle = NULL;
		int fButtonStyle = 0;
		int fIconStyle = 0;
		bool bShowHelpButton = false;
		if (pArgs)
		{
			if (pArgs->restype != RTSTR)
				return RSERR; //wrong argument type
			pszTitle = pArgs->resval.rstring;
			pArgs = pArgs->rbnext; //move to the next argument

			if (pArgs)
			{
				if (pArgs->restype != RTSHORT)
					return RSERR; //wrong argument type
				fButtonStyle = pArgs->resval.rint;
				pArgs = pArgs->rbnext; //move to the next argument

				if (pArgs)
				{
					if (pArgs->restype != RTSHORT)
						return RSERR; //wrong argument type
					fIconStyle = pArgs->resval.rint;
					pArgs = pArgs->rbnext; //move to the next argument

					if (pArgs)
					{
						if (pArgs->restype == RTSHORT)
							bShowHelpButton = (pArgs->resval.rint != 0);
						else
							bShowHelpButton = (pArgs->restype != RTNIL);

						if (pArgs->rbnext)
							return RSERR; //too many arguments
					}
				}
			}
		}

		//convert the arguments into windows messagebox type flags
		DWORD dwMsgBoxType = 0;
		switch (fButtonStyle)
		{	
		case 1: dwMsgBoxType = MB_ABORTRETRYIGNORE|MB_DEFBUTTON1; break;
		case -1: case 0: case 2: dwMsgBoxType = MB_OK|MB_DEFBUTTON1; break;
		case 3: dwMsgBoxType = MB_OKCANCEL|MB_DEFBUTTON1; break;
		case 4: dwMsgBoxType = MB_RETRYCANCEL|MB_DEFBUTTON1; break;
		case 5: dwMsgBoxType = MB_YESNO|MB_DEFBUTTON1; break;
		case 6: dwMsgBoxType = MB_YESNOCANCEL|MB_DEFBUTTON1; break;
		case 11: dwMsgBoxType = MB_ABORTRETRYIGNORE|MB_DEFBUTTON2; break;
		case 13: dwMsgBoxType = MB_OKCANCEL|MB_DEFBUTTON2; break;
		case 14: dwMsgBoxType = MB_RETRYCANCEL|MB_DEFBUTTON2; break;
		case 15: dwMsgBoxType = MB_YESNO|MB_DEFBUTTON2; break;
		case 16: dwMsgBoxType = MB_YESNOCANCEL|MB_DEFBUTTON2; break;
		case 21: dwMsgBoxType = MB_ABORTRETRYIGNORE|MB_DEFBUTTON3; break;
		case 26: dwMsgBoxType = MB_YESNOCANCEL|MB_DEFBUTTON3; break;
		default: return RSERR; //invalid argument value
		}

		switch (fIconStyle)
		{
		case 0: break;
		case 1: dwMsgBoxType |= MB_ICONEXCLAMATION; break;
		case 2: dwMsgBoxType |= MB_ICONINFORMATION; break;
		case 3: dwMsgBoxType |= MB_ICONQUESTION; break;
		case 4: dwMsgBoxType |= MB_ICONSTOP; break;
		default: return RSERR; //invalid argument value
		}

		if (bShowHelpButton)
			dwMsgBoxType |= MB_HELP;

		acedRetInt(MessageBox(theArxWorkspace.GetTopmostModalForm(), pszMessage, pszTitle, dwMsgBoxType));
		
		return (RSRSLT) ;
	}

	// ----- ads_dcl_form_setdialogminmaxsizes symbol (do not rename)
	static int ads_dcl_form_setdialogminmaxsizes(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		CDialogObject* pDialog = NULL;
		if (!RetrieveDialogFromArgs (pArgs, pDialog))
			return RSERR; //arguments expected
		if (!pDialog)
			return RSRSLT; //dialog not found

		if (!pArgs)
			return RSERR; //not enough arguments

		if (pArgs->restype != RTSHORT)
			return RSERR; //wrong argument type
		int nMinWidth = pArgs->resval.rint;
		pArgs = pArgs->rbnext; //move to the next argument

		if (!pArgs)
			return RSERR; //not enough arguments

		if (pArgs->restype != RTSHORT)
			return RSERR; //wrong argument type
		int nMinHeight = pArgs->resval.rint;
		pArgs = pArgs->rbnext; //move to the next argument

		if (!pArgs)
			return RSERR; //not enough arguments

		if (pArgs->restype != RTSHORT)
			return RSERR; //wrong argument type
		int nMaxWidth = pArgs->resval.rint;
		pArgs = pArgs->rbnext; //move to the next argument

		if (!pArgs)
			return RSERR; //not enough arguments

		if (pArgs->restype != RTSHORT)
			return RSERR; //wrong argument type
		int nMaxHeight = pArgs->resval.rint;

		if (pArgs->rbnext)
			return RSERR; //too many arguments

		if (pDialog->SetMinMaxSize(CSize(nMinWidth, nMinHeight), CSize(nMaxWidth, nMaxHeight)))
			acedRetT();

		return (RSRSLT) ;
	}

	// ----- ads_dcl_form_gethwnd symbol (do not rename)
	static int ads_dcl_form_gethwnd(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		CDialogObject* pDialog = NULL;
		if (!RetrieveDialogFromArgs (pArgs, pDialog))
			return RSERR; //arguments expected
		if (!pDialog)
			return RSRSLT; //dialog not found

		if (pArgs)
			return RSERR; //too many arguments

		acedRetLong((LONG)pDialog->GetHWnd());

		return (RSRSLT) ;
	}

	// ----- ads_dcl_form_isenabled symbol (do not rename)
	static int ads_dcl_form_isenabled(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		CDialogObject* pDialog = NULL;
		if (!RetrieveDialogFromArgs (pArgs, pDialog))
			return RSERR; //arguments expected
		if (!pDialog)
			return RSRSLT; //dialog not found

		if (pArgs)
			return RSERR; //too many arguments

		if (IsWindowEnabled(pDialog->GetHWnd()))
			acedRetT();

		return (RSRSLT) ;
	}

	// ----- ads_dcl_form_isvisible symbol (do not rename)
	static int ads_dcl_form_isvisible(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		CDialogObject* pDialog = NULL;
		if (!RetrieveDialogFromArgs (pArgs, pDialog))
			return RSERR; //arguments expected
		if (!pDialog)
			return RSRSLT; //dialog not found

		if (pArgs)
			return RSERR; //too many arguments

		if (IsWindowVisible(pDialog->GetHWnd()))
			acedRetT();

		return (RSRSLT) ;
	}

	// ----- ads_dcl_form_enable symbol (do not rename)
	static int ads_dcl_form_enable(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		CDialogObject* pDialog = NULL;
		if (!RetrieveDialogFromArgs (pArgs, pDialog))
			return RSERR; //arguments expected
		if (!pDialog)
			return RSRSLT; //dialog not found

		//optional arguments
		BOOL bEnable = TRUE;
		if (pArgs)
		{
			if (pArgs->restype == RTSHORT)
				bEnable = (pArgs->resval.rint != 0);
			else
				bEnable = (pArgs->restype != RTNIL);

			if (pArgs->rbnext)
				return RSERR; //too many arguments
		}

		EnableWindow(pDialog->GetHWnd(), bEnable);

		return (RSRSLT) ;
	}

	// ----- ads_dcl_getharddrivesize symbol (do not rename)
	static int ads_dcl_getharddrivesize(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;
		if (pArgs == NULL)
			return RSERR; //argument expected

		if (pArgs->restype != RTSTR)
			return RSERR; //wrong argument type
		LPCTSTR pszDrive = pArgs->resval.rstring;

		if (pArgs->rbnext)
			return RSERR; //too many arguments

		if (!pszDrive)
			return RSERR; //invalid argument

		DWORDLONG nFree;
		if (GetDiscSpace(pszDrive, &nFree))
			acedRetReal((double)nFree);

		return (RSRSLT) ;
	}

	// ----- ads_dcl_getfocus symbol (do not rename)
	static int ads_dcl_getfocus(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;
		if (pArgs)
			return RSERR; //no arguments expected

		HWND hwndFocus = GetFocus();
		if (hwndFocus)
		{
			CString sControlName;
			const CDclFormObject* pDclObject = theArxWorkspace.FindDclFormControl (hwndFocus, sControlName);
			if (pDclObject)
			{
				resbuf rbControlName = {NULL, RTSTR};
				rbControlName.resval.rstring = sControlName.LockBuffer();
				resbuf rbDialogName = {sControlName.IsEmpty()? NULL : &rbControlName, RTSTR};
				rbDialogName.resval.rstring = CString(pDclObject->GetKeyName()).LockBuffer();
				resbuf rbProjectName = {&rbDialogName, RTSTR};
				rbProjectName.resval.rstring = CString(pDclObject->GetProject()->GetKeyName()).LockBuffer();
				acedRetList(&rbProjectName);
			}
		}

		return (RSRSLT) ;
	}

	// ----- ads_dcl_hideerrormsgbox symbol (do not rename)
	static int ads_dcl_hideerrormsgbox(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;
		if (pArgs)
			return RSERR; //no arguments expected

		theWorkspace.SetMessagesSuppressed();

		return (RSRSLT) ;
	}

	// ----- ads_dcl_showerrormsgbox symbol (do not rename)
	static int ads_dcl_showerrormsgbox(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;
		if (pArgs)
			return RSERR; //no arguments expected

		theWorkspace.SetMessagesSuppressed(false);

		return (RSRSLT) ;
	}

	// ----- ads_dcl_getblocksize symbol (do not rename)
	static int ads_dcl_getblocksize(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;
		if (pArgs == NULL)
			return RSERR; //argument expected

		if (pArgs->restype != RTSTR)
			return RSERR; //wrong argument type
		LPCTSTR pszBlockName = pArgs->resval.rstring;

		if (!pszBlockName)
			return RSERR; //invalid argument

		if (pArgs->rbnext)
			return RSERR; //too many arguments

    AcDbDatabase* pDb = acdbCurDwg();
		assert(pDb != NULL);
		if (!pDb)
			return RSERR;
		AcDbBlockTable* pBlockTable;
		Acad::ErrorStatus es = pDb->getBlockTable (pBlockTable, AcDb::kForRead);
		if (es != Acad::eOk)
			return RSRSLT;
		AcDbBlockTableRecord* pBTR;
		es = pBlockTable->getAt (pszBlockName, pBTR, AcDb::kForRead);
		pBlockTable->close();
		if (es != Acad::eOk)
			return RSRSLT;
		AcDbExtents ext;
		es = ext.addBlockExt (pBTR);
		pBTR->close();
		if (es != Acad::eOk)
			return RSRSLT;

		AcGeVector3d size = ext.maxPoint() - ext.minPoint();
		resbuf rbHeight = {NULL, RTREAL};
		rbHeight.resval.rreal = size.x;
		resbuf rbWidth = {&rbHeight, RTREAL};
		rbWidth.resval.rreal = size.y;
		acedRetList (&rbWidth);

		return (RSRSLT) ;
	}

	// ----- ads_dcl_control_gethwnd symbol (do not rename)
	static int ads_dcl_control_gethwnd(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;
		if (!pArgs)
			return RSERR; //arguments expected

		CDclControlObject* pControl = NULL;
		LPCTSTR pszProject = NULL;
		switch (pArgs->restype)	
		{
		case RTSHORT:
			pControl = (CDclControlObject*)pArgs->resval.rint;
			break;
		case RTLONG:
			pControl = (CDclControlObject*)pArgs->resval.rlong;
			break;
		case RTSTR:
			{
				pszProject = pArgs->resval.rstring;

				pArgs = pArgs->rbnext; //move to the next argument
				if (!pArgs)
					return RSERR; //not enough arguments

				if (pArgs->restype != RTSTR)
					return RSERR; //wrong argument type
				LPCTSTR pszDialog = pArgs->resval.rstring;

				pArgs = pArgs->rbnext; //move to the next argument
				if (!pArgs)
					return RSERR; //not enough arguments

				if (pArgs->restype != RTSTR)
					return RSERR; //wrong argument type
				LPCTSTR pszControl = pArgs->resval.rstring;
				pControl = theArxWorkspace.FindControl (pszProject, pszDialog, pszControl);
				break;
			}
		default:
			return RSERR; //wrong argument type
		}

		if (!pControl)
			return RSRSLT; //dialog not found

		if (pArgs->rbnext)
			return RSERR; //too many arguments

		acedRetLong((LONG)pControl->GetWindow()->m_hWnd);

		return (RSRSLT) ;
	}

	// ----- ads_dcl_navigatetourl symbol (do not rename)
	static int ads_dcl_navigatetourl(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;
		if (pArgs == NULL)
			return RSERR; //argument expected

		if (pArgs->restype != RTSTR)
			return RSERR; //wrong argument type
		LPCTSTR pszURL = pArgs->resval.rstring;

		if (!pszURL)
			return RSERR; //invalid argument

		if (pArgs->rbnext)
			return RSERR; //too many arguments

		ShellExecute(0, _T("open"), pszURL, 0, 0, SW_SHOWNORMAL);

		return (RSRSLT) ;
	}

	// ----- ads_dcl_activateemail symbol (do not rename)
	static int ads_dcl_activateemail(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;
		if (pArgs == NULL)
			return RSERR; //argument expected

		if (pArgs->restype != RTSTR)
			return RSERR; //wrong argument type
		LPCTSTR pszEmailAddress = pArgs->resval.rstring;

		if (!pszEmailAddress)
			return RSERR; //invalid argument

		LPCTSTR pszParams = NULL;
		pArgs = pArgs->rbnext;
		if (pArgs)
		{
			if (pArgs->restype != RTSTR)
				return RSERR; //wrong argument type
			pszParams = pArgs->resval.rstring;

			if (pArgs->rbnext)
				return RSERR; //too many arguments
		}

		CString sTarget = _T("mailto:");
		sTarget += pszEmailAddress;
		if (pszParams)
		{
			CString sParams = pszParams;
			if( sParams.Left( 1 ) == _T("?") )
				sParams = sParams.Mid( 1 );
			sParams.Replace( _T(" "), _T("%20") );
			sParams.Replace( _T("\r"), _T("%0D") );
			sParams.Replace( _T("\n"), _T("%0A") );
			sTarget += _T("?");
			if( sParams.Find( _T('=') ) < 0 )
				sTarget += _T("subject=");
			sTarget += sParams;
		}
		ShellExecute(0, _T("open"), sTarget, 0, 0, SW_SHOWNORMAL);

		return (RSRSLT) ;
	}

	// ----- ads_dcl_browsefolder symbol (do not rename)
	static int ads_dcl_browsefolder(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;
		if (pArgs == NULL)
			return RSERR; //argument expected

		if (pArgs->restype != RTSTR)
			return RSERR; //wrong argument type
		LPCTSTR pszCaption = pArgs->resval.rstring;

		if (!pszCaption)
			return RSERR; //invalid argument

		//optional arguments
		LPCTSTR pszInitialFolder = NULL;
		LPCTSTR pszRootFolder = NULL;
		DWORD dwFlags = BIF_RETURNONLYFSDIRS;
		pArgs = pArgs->rbnext;
		if (pArgs)
		{
			if (pArgs->restype == RTSTR)
				pszInitialFolder = pArgs->resval.rstring;
			else if (pArgs->restype == RTNIL)
				pszInitialFolder = NULL;
			else
				return RSERR; //wrong argument type

			pArgs = pArgs->rbnext;
			if (pArgs)
			{
				if (pArgs->restype == RTSTR)
					pszRootFolder = pArgs->resval.rstring;
				else if (pArgs->restype == RTNIL)
					pszRootFolder = NULL;
				else
					return RSERR; //wrong argument type

				pArgs = pArgs->rbnext;
				if (pArgs)
				{
					if (pArgs->restype == RTSHORT)
						dwFlags = (DWORD)pArgs->resval.rint;
					else if (pArgs->restype == RTREAL)
						dwFlags = (DWORD)pArgs->resval.rreal;
					else
						return RSERR; //wrong argument type

					if (pArgs->rbnext)
						return RSERR; //too many arguments
				}
			}
		}

		CDirDialog dlg( pszCaption, pszInitialFolder, pszRootFolder, dwFlags );
		if (dlg.DoBrowse(CWnd::FromHandle(theArxWorkspace.GetTopmostModalForm())))
			acedRetStr(dlg.GetSelectedFolder());

		return (RSRSLT) ;
	}

	// ----- ads_dcl_insert symbol (do not rename)
	static int ads_dcl_insert(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;
		if (pArgs == NULL)
			return RSERR; //argument expected

		if (pArgs->restype != RTSTR)
			return RSERR; //wrong argument type
		CString sPath = pArgs->resval.rstring;

		CString sBlockName;
		AcGePoint3d ptBase;
		double dblRotation = 0.0;
		pArgs = pArgs->rbnext;
		if (pArgs)
		{
			switch (pArgs->restype)
			{
			case RT3DPOINT:
				ptBase.set( pArgs->resval.rpoint[X], pArgs->resval.rpoint[Y], pArgs->resval.rpoint[Z]);
				break;
			case RTPOINT:
				ptBase.set( pArgs->resval.rpoint[X], pArgs->resval.rpoint[Y], 0);
				break;
			default:
				return RSERR; //wrong argument type
			}

			pArgs = pArgs->rbnext;
			if (pArgs)
			{
				switch (pArgs->restype)
				{
				case RTREAL:
				case RTANG:
					dblRotation = pArgs->resval.rreal;
					break;
				default:
					return RSERR; //wrong argument type
				}

				pArgs = pArgs->rbnext;
				if (pArgs)
				{
					switch (pArgs->restype)
					{
					case RTSTR:
						sBlockName = pArgs->resval.rstring;
						break;
					case RTNIL:
						//use the filename without extension
						sBlockName = sPath;
						sBlockName.MakeReverse();
						sBlockName = sBlockName.SpanExcluding(_T("\\/:"));
						sBlockName.MakeReverse();
						sBlockName = sBlockName.SpanExcluding(_T("."));
						break;
					default:
						return RSERR; //wrong argument type
					}

					if (pArgs->rbnext)
						return RSERR; //too many arguments
				}
			}
		}

		if (sPath.IsEmpty())
			sPath = sBlockName;

		//----- Read the external DWG file
		AcDbObjectId blockId;
		AcDbDatabase *pDwg = new AcDbDatabase(Adesk::kFalse);
		Acad::ErrorStatus es = pDwg->readDwgFile (sPath, _SH_DENYNO);
		if (es == Acad::eOk)
		{
			//----- Put it into a block table record of the current database
			es = acdbCurDwg()->insert (blockId, sBlockName, pDwg, Adesk::kFalse) ;
			delete pDwg;
		}
		if (es != Acad::eOk)
			return RSRSLT; //failed to insert block

		//----- Step 1: Allocate a block reference object
		AcDbBlockReference *pBlkRef = new AcDbBlockReference ;
		if (!pBlkRef)
			return RSRSLT;
		//----- Step 2: Set up the block reference to the newly
		//----- created block definition
		es = pBlkRef->setBlockTableRecord (blockId) ;
		if (es != Acad::eOk)
		{
			delete pBlkRef;
			return RSRSLT;
		}
		//---- Give it the current UCS normal.
		es = pBlkRef->setPosition (ptBase) ;
		if (es != Acad::eOk)
		{
			delete pBlkRef;
			return RSRSLT;
		}
		es = pBlkRef->setRotation (dblRotation) ;
		if (es != Acad::eOk)
		{
			delete pBlkRef;
			return RSRSLT;
		}
		es = pBlkRef->setNormal (AcGeVector3d (0.0, 0.0, 1.0)) ;
		if (es != Acad::eOk)
		{
			delete pBlkRef;
			return RSRSLT;
		}
		//----- Step 3: Open current database's Model Space
		//----- blockTableRecord
		AcDbBlockTable *pBlockTable ;
		es = acdbCurDwg()->getBlockTable (pBlockTable, AcDb::kForRead) ;
		if (es != Acad::eOk)
		{
			delete pBlkRef;
			return RSRSLT;
		}
		AcDbBlockTableRecord *pBlockTableRecord ;
		es = pBlockTable->getAt (ACDB_MODEL_SPACE, pBlockTableRecord, AcDb::kForWrite) ;
		pBlockTable->close () ;
		if (es != Acad::eOk)
		{
			delete pBlkRef;
			return RSRSLT;
		}
		//----- Append the block reference to the model space
		//----- block table record
		AcDbObjectId newEntId ;
		es = pBlockTableRecord->appendAcDbEntity (newEntId, pBlkRef) ;
		pBlockTableRecord->close () ;
		if (es != Acad::eOk)
		{
			delete pBlkRef;
			return RSRSLT;
		}
		//----- Step 4: Open the block definition for read
		AcDbBlockTableRecord *pBlockDef ;
		es = acdbOpenObject (pBlockDef, blockId, AcDb::kForRead) ;
		if (es != Acad::eOk)
			return RSRSLT;
		AcDbBlockTableRecordIterator *pIterator ;
		es = pBlockDef->newIterator (pIterator) ;
		if (es != Acad::eOk)
			return RSRSLT;
		AcDbEntity *pEnt ;
		AcDbAttributeDefinition *pAttdef ;
		for ( pIterator->start () ; !pIterator->done() ; pIterator->step () ) {
			//----- Get the next entity
			es = pIterator->getEntity (pEnt, AcDb::kForRead) ;
			if (es != Acad::eOk)
				break;
			//----- Make sure the entity is an attribute definition
			//----- and not a constant
			pAttdef =AcDbAttributeDefinition::cast (pEnt) ;
			if ( pAttdef != NULL && !pAttdef->isConstant () ) {
				//----- We have a non-constant attribute definition
				//----- so build an attribute entity
				AcDbAttribute *pAtt =new AcDbAttribute ;
				if (pAtt == NULL)
					break;
				es = pAtt->setPropertiesFrom (pAttdef) ;
				if (es != Acad::eOk )
					break;
				es = pAtt->setInvisible (pAttdef->isInvisible ()) ;
				if (es != Acad::eOk )
					break;
				//----- Translate attribute by block reference.
				//----- To be really correct, entire block
				//----- reference transform should be applied here.
				AcGePoint3d basePoint =pAttdef->position () ;
				basePoint +=pBlkRef->position ().asVector () ;
				es = pAtt->setPosition (basePoint) ;
				if (es != Acad::eOk )
					break;
				es = pAtt->setHeight (pAttdef->height ()) ;
				if (es != Acad::eOk )
					break;
				es = pAtt->setRotation (pAttdef->rotation ()) ;
				if (es != Acad::eOk )
					break;
				es = pAtt->setTag (_T("Tag")) ;
				if (es != Acad::eOk )
					break;
				es = pAtt->setFieldLength (25) ;
				if (es != Acad::eOk )
					break;
				ACHAR *pStr =pAttdef->tag () ;
				es = pAtt->setTag (pStr) ;
				if (es != Acad::eOk )
					break;
				acutDelString(pStr) ;
				es = pAtt->setFieldLength (pAttdef->fieldLength ()) ;
				if (es != Acad::eOk )
					break;
				//----- Database Column value should be displayed
				//----- INSERT would prompt for this...
				es = pAtt->setTextString (CString()) ;
				if (es != Acad::eOk )
					break;
				//----- Set Alignments
				es = pAtt->setHorizontalMode( pAttdef->horizontalMode ()) ;
				if (es != Acad::eOk )
					break;
				es = pAtt->setVerticalMode (pAttdef->verticalMode ()) ;
				if (es != Acad::eOk )
					break;
				es = pAtt->setAlignmentPoint (pAttdef->alignmentPoint () + pBlkRef->position ().asVector ()) ;
				if (es != Acad::eOk )
					break;
				//----- Insert the attribute in the DWG
				AcDbObjectId attId ;
				es = pBlkRef->appendAttribute (attId, pAtt) ;
				if (es != Acad::eOk )
					break;
				pAtt->close () ;
			}	 
			pEnt->close () ; //----- Use pEnt... pAttdef might be NULL
		}
		delete pIterator ;
		pBlockDef->close () ;
		pBlkRef->close () ;

		if (es == Acad::eOk)
			acedRetT();

		return (RSRSLT) ;
	}

	// ----- ads_dcl_loadxref symbol (do not rename)
	static int ads_dcl_loadxref(void)
	{
		/* Revision 2007-01-28 [ORW]  **This change breaks existing code.**
				The original implementation of this function expected a path without filename as the first argument and 
				a drawing file name as the second argument. It simply concatenated the second argument to the first 
				argument and passed that to acedXrefAttach. The problem with this strategy is that it precludes the 
				caller from renaming the block definition when the xref is attached. I changed it so that the first 
				argument expects the filename as part of the path (or just the filename by itself). If the second 
				argument is supplied and is not nil, it will become the block name of the attached xref. If the second 
				argument is not supplied, or is nil, then the block name defaults to the filename without extension.
		*/
		struct resbuf *pArgs =acedGetArgs () ;
		if (pArgs == NULL)
			return RSERR; //argument expected

		if (pArgs->restype != RTSTR)
			return RSERR; //wrong argument type
		CString sPath = pArgs->resval.rstring;

		CString sBlockName;
		AcGePoint3d ptBase;
		double dblRotation = 0.0;
		pArgs = pArgs->rbnext;
		if (pArgs)
		{
			switch (pArgs->restype)
			{
			case RTSTR:
				sBlockName = pArgs->resval.rstring;
				break;
			case RTNIL:
				//use the filename without extension
				sBlockName = sPath;
				sBlockName.MakeReverse();
				sBlockName = sBlockName.SpanExcluding(_T("\\/:"));
				sBlockName.MakeReverse();
				sBlockName = sBlockName.SpanExcluding(_T("."));
				break;
			default:
				return RSERR; //wrong argument type
			}

			pArgs = pArgs->rbnext;
			if (pArgs)
			{
				switch (pArgs->restype)
				{
				case RT3DPOINT:
					ptBase.set( pArgs->resval.rpoint[X], pArgs->resval.rpoint[Y], pArgs->resval.rpoint[Z]);
					break;
				case RTPOINT:
					ptBase.set( pArgs->resval.rpoint[X], pArgs->resval.rpoint[Y], 0);
					break;
				default:
					return RSERR; //wrong argument type
				}

				pArgs = pArgs->rbnext;
				if (pArgs)
				{
					switch (pArgs->restype)
					{
					case RTREAL:
					case RTANG:
						dblRotation = pArgs->resval.rreal;
						break;
					default:
						return RSERR; //wrong argument type
					}

					if (pArgs->rbnext)
						return RSERR; //too many arguments
				}
			}
		}

		if (sPath.IsEmpty())
			sPath = sBlockName;
    acedXrefAttach(sPath, sBlockName, NULL, NULL, &ptBase, &AcGeScale3d(1, 1, 1), &dblRotation, Adesk::kFalse);

		return (RSRSLT) ;
	}

	// ----- ads_dcl_updatevarnames symbol (do not rename)
	static int ads_dcl_updatevarnames(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;
		if (pArgs)
			return RSERR; //no arguments expected

		if (theArxWorkspace.UpdateGlobalLispSymbols())
			acedRetT();

		return (RSRSLT) ;
	}

	// ----- ads_dcl_forcedwgredraw symbol (do not rename)
	static int ads_dcl_forcedwgredraw(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;
		if (pArgs)
			return RSERR; //no arguments expected

		CView* pDwgView = acedGetAcadDwgView();
		assert(pDwgView != NULL);
		pDwgView->RedrawWindow();

		return (RSRSLT) ;
	}

	// ----- ads_dcl_getmousecoords symbol (do not rename)
	static int ads_dcl_getmousecoords(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;
		if (pArgs)
			return RSERR; //no arguments expected

		CPoint pt;
		GetCursorPos(&pt);
		resbuf rbPoint = {NULL, RTPOINT};
		rbPoint.resval.rpoint[X] = pt.x;
		rbPoint.resval.rpoint[Y] = pt.y;
		acedRetVal(&rbPoint);

		return (RSRSLT) ;
	}

	// ----- ads_dcl_getdwgmousecoords symbol (do not rename)
	static int ads_dcl_getdwgmousecoords(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;
		if (pArgs)
			return RSERR; //no arguments expected

		CPoint ptMouse;
		GetCursorPos(&ptMouse);

		AcGePoint3d ptAcad;
		acedDwgPoint cpt;
		acedCoordFromPixelToWorld(ptMouse, cpt);

		// 2: Transform the Mouse coords from DCS to WCS
		struct resbuf rbview;
		ads_getvar(_T("viewdir"), &rbview);
		ads_point norm;
		ads_point_set(rbview.resval.rpoint, norm);
		// If the view direction is 0, 0, 1, then no translation is necessary
		double tol = 1e-10;
		if ((fabs(norm[X]) >= tol) || (fabs(norm[Y]) >= tol) || ((fabs(norm[Z]) - 1.0) >= tol))
		{
			// Transfoming VIEWDIR to WCS
			acdbUcs2Wcs(norm, norm, Adesk::kTrue);
			// Transforming the mouse coords from DCS to WCS
			ads_point ptDCS;
			ads_point_set(cpt, ptDCS);
			ads_point ptWCS;
			acdbEcs2Wcs(ptDCS, ptWCS, norm, Adesk::kTrue);

			// 3: Get the Mouse coordinates in UCS
			AcGeMatrix3d matUcs2Wcs;
			acdbUcsMatrix(matUcs2Wcs);
			AcGePoint3d origin;
			AcGeVector3d e0,
									 e1,
									 e2;
			matUcs2Wcs.getCoordSystem(origin, e0, e1, e2);
			// Creating a plane which lies on the current UCS
			AcGePlane thePlane(origin, e0, e1);
			AcGePoint3d pntMouseInWCS(ptWCS[X], ptWCS[Y], ptWCS[Z]);
			AcGeVector3d vecViewdir(norm [X], norm [Y], norm [Z]);
			// Making the projection
			ptAcad = pntMouseInWCS.project(thePlane, vecViewdir);
			// Transforming the point coordinates to UCS.
			ptAcad.transformBy(matUcs2Wcs.inverse());
		}
		else
		{
			ptAcad.x = cpt[X];
			ptAcad.y = cpt[Y];
			ptAcad.z = 0;
		}

		resbuf rbPoint = {NULL, RTPOINT};
		rbPoint.resval.rpoint[X] = ptAcad.x;
		rbPoint.resval.rpoint[Y] = ptAcad.y;
		acedRetVal(&rbPoint);

		return (RSRSLT) ;
	}

	// ----- ads_dcl_multifiledialog symbol (do not rename)
	static int ads_dcl_multifiledialog(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		//optional arguments
		CString sFilterList = _T("AutoCAD Drawing (*.dwg)|*.dwg|");
		LPCTSTR pszCaption = NULL;
		LPCTSTR pszPath = NULL;

		if (pArgs)
		{
			if (pArgs->restype == RTSTR)
			{
				sFilterList = pArgs->resval.rstring;
				if (!sFilterList.IsEmpty() && sFilterList.Right(1) != _T("|"))
					sFilterList += _T('|');
			}
			else if (pArgs->restype == RTLB)
			{
				sFilterList.Empty();
				for (pArgs = pArgs->rbnext; pArgs && pArgs->restype != RTLE; pArgs = pArgs->rbnext)
				{
					if (pArgs->restype != RTSTR)
						return RSERR; //wrong argument type

					sFilterList += pArgs->resval.rstring;
					if (!sFilterList.IsEmpty() && sFilterList.Right(1) != _T("|"))
						sFilterList += _T('|');
				}

				if (!pArgs)
					return RSERR; //unterminated list argument
			}
			else
				return RSERR; //wrong argument type

			pArgs = pArgs->rbnext;
			if (pArgs)
			{
				if (pArgs->restype != RTSTR)
					return RSERR; //wrong argument type
				pszCaption = pArgs->resval.rstring;

				pArgs = pArgs->rbnext;
				if (pArgs)
				{
					if (pArgs->restype != RTSTR)
						return RSERR; //wrong argument type
					pszPath = pArgs->resval.rstring;

					if (pArgs->rbnext)
						return RSERR; //too many arguments
				}
			}
		}

		CWnd *pParent = CWnd::FromHandle(theArxWorkspace.GetTopmostModalForm());
		DWORD dwFlags = OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING |
										OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
		// create the open dialog box
		CFileDialog BrowseWnd(TRUE, NULL, NULL, dwFlags, sFilterList, pParent);
		BrowseWnd.m_ofn.lpstrInitialDir = pszPath;
		BrowseWnd.m_ofn.lpstrTitle = pszCaption;
		CString sResults;
		// proceed to setup the file buffer size
		BrowseWnd.m_ofn.nMaxFile = 8192;
		BrowseWnd.m_ofn.lpstrFile = sResults.GetBuffer(8192);
		BrowseWnd.m_ofn.lpstrFile[0] = _T('\0');
		int nStat = BrowseWnd.DoModal();
		if (nStat == IDOK)  	
		{		
			// Convert the array to a list that can be returned
			struct resbuf* prbFileList = NULL;
			struct resbuf* prbTail = NULL;
			POSITION posFile = BrowseWnd.GetStartPosition();
			while (posFile)
			{
				resbuf* prbFile = acutNewRb(RTSTR);
				CString sFilename = BrowseWnd.GetNextPathName(posFile);
				sFilename.Replace( _T("\\\\"), _T("\\") ); //remove double backslashes added by MFC when files are in root folder
				acutNewString(sFilename, prbFile->resval.rstring);
				if (!prbTail)
				{
					prbFileList = prbFile;
					prbTail = prbFileList;
				}
				else
				{
					prbTail->rbnext = prbFile;
					prbTail = prbTail->rbnext;
				}
			}
			acedRetList(prbFileList);
			acutRelRb(prbFileList);
		}

		return (RSRSLT) ;
	}

	// ----- ads_dcl_getpicturesize symbol (do not rename)
	static int ads_dcl_getpicturesize(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;
		if (pArgs == NULL)
			return RSERR; //argument expected

		if (pArgs->restype != RTSTR)
			return RSERR; //wrong argument type
		LPCTSTR pszProject = pArgs->resval.rstring;

		if (!pszProject)
			return RSERR; //invalid argument

		pArgs = pArgs->rbnext;
		if (pArgs == NULL)
			return RSERR; //argument expected

		if (pArgs->restype != RTSHORT)
			return RSERR; //wrong argument type
		long id = pArgs->resval.rint;

		if (pArgs->rbnext)
			return RSERR; //too many arguments

		CArxProject* pProject = theArxWorkspace.FindProject(pszProject);
		if (!pProject)
			return RSRSLT; //project not found

		CSize sizePicture;
		if (!pProject->GetPictureSize (id, sizePicture))
			return RSRSLT; //no picture with that id

		resbuf rbHeight = {NULL, RTSHORT};
		rbHeight.resval.rint = sizePicture.cy;
		resbuf rbWidth = {&rbHeight, RTSHORT};
		rbWidth.resval.rint = sizePicture.cx;
		acedRetList(&rbWidth);

		return (RSRSLT) ;
	}

	// ----- ads_dcl_setcmdbarfocus symbol (do not rename)
	static int ads_dcl_setcmdbarfocus(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;
		if (pArgs)
			return RSERR; //no argument expected

		CWnd* CmdBarWnd = acedGetAcadDockCmdLine();
		if (CmdBarWnd)
		{
			CmdBarWnd->SetFocus();
			acedRetT();
		}

		return (RSRSLT) ;
	}

	// ----- ads_dcl_flushgraphicbuttons symbol (do not rename)
	static int ads_dcl_flushgraphicbuttons(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;
		if (pArgs == NULL)
			return RSERR; //argument expected

		if (pArgs->restype != RTSTR)
			return RSERR; //wrong argument type
		LPCTSTR pszProject = pArgs->resval.rstring;

		if (!pszProject)
			return RSERR; //invalid argument

		pArgs = pArgs->rbnext;
		if (pArgs == NULL)
			return RSERR; //argument expected

		if (pArgs->restype != RTSTR)
			return RSERR; //wrong argument type
		LPCTSTR pszDialog = pArgs->resval.rstring;

		if (pArgs->rbnext)
			return RSERR; //too many arguments

		CDialogObject* pDialog = theArxWorkspace.FindDialog(pszProject, pszDialog);
		if (!pDialog)
			return RSRSLT; //dialog not found

		// call the size changed method to force the graphic buttons to refresh
		pDialog->GetControlPane().RecalcLayout();

		return (RSRSLT) ;
	}

	// ----- ads_dcl_xtwipstopixels symbol (do not rename)
	static int ads_dcl_xtwipstopixels(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;
		if (!pArgs)
			return RSERR; //arguments expected

		long twips = 0;
		switch (pArgs->restype)
		{
		case RTSHORT:
		case RTLONG:
			twips = pArgs->resval.rint;
			break;
		case RTREAL:
			twips = (long)pArgs->resval.rreal;
			break;
		default:
			return RSERR; //invalid argument type
		}

		if (pArgs->rbnext)
			return RSERR; //too many arguments

		acedRetLong (MulDiv (GetDeviceCaps (CDC(), LOGPIXELSX), twips, 1440));

		return (RSRSLT) ;
	}

	// ----- ads_dcl_ytwipstopixels symbol (do not rename)
	static int ads_dcl_ytwipstopixels(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;
		if (!pArgs)
			return RSERR; //arguments expected

		long pixels = 0;
		switch (pArgs->restype)
		{
		case RTSHORT:
		case RTLONG:
			pixels = pArgs->resval.rint;
			break;
		case RTREAL:
			pixels = (long)pArgs->resval.rreal;
			break;
		default:
			return RSERR; //invalid argument type
		}

		if (pArgs->rbnext)
			return RSERR; //too many arguments

		acedRetLong (MulDiv (pixels, 1440, GetDeviceCaps (CDC(), LOGPIXELSX)));

		return (RSRSLT) ;
	}

	// ----- ads_dcl_xpixelstotwips symbol (do not rename)
	static int ads_dcl_xpixelstotwips(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;
		if (!pArgs)
			return RSERR; //arguments expected

		long pixels = 0;
		switch (pArgs->restype)
		{
		case RTSHORT:
		case RTLONG:
			pixels = pArgs->resval.rint;
			break;
		case RTREAL:
			pixels = (long)pArgs->resval.rreal;
			break;
		default:
			return RSERR; //invalid argument type
		}

		if (pArgs->rbnext)
			return RSERR; //too many arguments

		acedRetLong (MulDiv (pixels, 1440, GetDeviceCaps (CDC(), LOGPIXELSY)));

		return (RSRSLT) ;
	}

	// ----- ads_dcl_ypixelstotwips symbol (do not rename)
	static int ads_dcl_ypixelstotwips(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;
		if (!pArgs)
			return RSERR; //arguments expected

		long twips = 0;
		switch (pArgs->restype)
		{
		case RTSHORT:
		case RTLONG:
			twips = pArgs->resval.rint;
			break;
		case RTREAL:
			twips = (long)pArgs->resval.rreal;
			break;
		default:
			return RSERR; //invalid argument type
		}

		if (pArgs->rbnext)
			return RSERR; //too many arguments

		acedRetLong (MulDiv (GetDeviceCaps (CDC(), LOGPIXELSX), twips, 1440));

		return (RSRSLT) ;
	}

	// ----- ads_dcl_getcolorvalue symbol (do not rename)
	static int ads_dcl_getcolorvalue(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;
		if (!pArgs)
			return RSERR; //arguments expected

		short lRed = 0;
		switch (pArgs->restype)
		{
		case RTSHORT:
		case RTLONG:
			lRed = pArgs->resval.rint;
			break;
		default:
			return RSERR; //invalid argument type
		}

		pArgs = pArgs->rbnext;
		if (pArgs)
		{
			short lGreen = 0;
			switch (pArgs->restype)
			{
			case RTSHORT:
			case RTLONG:
				lGreen = pArgs->resval.rint;
				break;
			default:
				return RSERR; //invalid argument type
			}

			pArgs = pArgs->rbnext;
			if (!pArgs)
				return RSERR; //expected more arguments

			short lBlue = 0;
			switch (pArgs->restype)
			{
			case RTSHORT:
			case RTLONG:
				lBlue = pArgs->resval.rint;
				break;
			default:
				return RSERR; //invalid argument type
			}

			if (pArgs->rbnext)
				return RSERR; //too many arguments

			acedRetLong (RGB(lRed,lGreen,lBlue));
		}
		else
			acedRetLong (GetRGBColor(lRed));

		return (RSRSLT) ;
	}

	// ----- ads_dcl_registeractivexctrl symbol (do not rename)
	static int ads_dcl_registeractivexctrl(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;
		if (!pArgs)
			return RSERR; //expected arguments

		if (pArgs->restype != RTSTR)
			return RSERR; //invalid argument type

		LPCTSTR pszPath = pArgs->resval.rstring;

		if (pArgs->rbnext)
			return RSERR; //too many arguments

		HMODULE hAxServer = LoadLibrary (pszPath);
		if (!hAxServer)
			hAxServer = LoadLibrary (theWorkspace.FindFile(pszPath));
		if (!hAxServer)
			return RSRSLT;

		typedef HRESULT (STDAPICALLTYPE* PDLLREGISTERSERVER)( void );
		PDLLREGISTERSERVER pfRegister = (PDLLREGISTERSERVER)GetProcAddress (hAxServer, "DllRegisterServer");
		if (pfRegister && SUCCEEDED(pfRegister()))
			acedRetT();

		FreeLibrary (hAxServer);

		return (RSRSLT) ;
	}

	// ----- ads_dcl_project_load symbol (do not rename)
	static int ads_dcl_project_load(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		if (pArgs == NULL)
			return RSERR; //argument expected

		if (pArgs->restype != RTSTR)
			return RSERR; //wrong argument type
		LPCTSTR pszFilename = pArgs->resval.rstring;
		pArgs = pArgs->rbnext; //move to the next argument

		//optional arguments
		bool bReload = false;
		LPCTSTR pszKeyName = NULL;
		if (pArgs)
		{
			bReload = (pArgs->restype != RTNIL);
			pArgs = pArgs->rbnext; //move to the next argument

			if (pArgs)
			{
				if (pArgs->restype != RTSTR)
					return RSERR; //wrong argument type
				pszKeyName = pArgs->resval.rstring;

				if (pArgs->rbnext != NULL)
					return RSERR; //too many arguments
			}
		}

		CProject *pProject = theArxWorkspace.LoadProjectFile(pszFilename, pszKeyName, bReload);
		if (pProject == NULL)
		{
			CString sAlertMsg;
			sAlertMsg.Format( _T("Project failed to load!\r\nThe file could not be found ")
												_T("or an error occurred while reading the file.\r\n\r\n[%s]"), pszFilename );
			theWorkspace.DisplayAlert( sAlertMsg );
			return RSRSLT; 
		}
		acedRetStr( pProject->GetProjectFilePath() );

		return (RSRSLT) ;
	}

	// ----- ads_dcl_project_unload symbol (do not rename)
	static int ads_dcl_project_unload(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;
		if (pArgs == NULL)
			return RSERR; //argument expected

		CProject* pProject = NULL;
		if (pArgs->restype == RTLONG)
			pProject = (CProject*)pArgs->resval.rlong;
		else if( pArgs->restype == RTSTR )
			pProject = theArxWorkspace.FindProject( pArgs->resval.rstring );
		else
			return RSERR; //wrong argument type

		if( !pProject )
			return RSERR; //too many arguments

		//optional arguments
		bool bForce = false;
		if (pArgs)
		{
			bForce = (pArgs->restype != RTNIL);

			if (pArgs->rbnext != NULL)
				return RSERR; //too many arguments
		}

		if (theArxWorkspace.UnloadProject(pProject, bForce))
			acedRetT();

		return (RSRSLT) ;
	}

	// ----- ads_dcl_project_saveas symbol (do not rename)
	static int ads_dcl_project_saveas(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;
		if (pArgs == NULL)
			return RSERR; //argument expected

		CProject* pProject = NULL;
		if (pArgs->restype == RTLONG)
			pProject = (CProject*)pArgs->resval.rlong;
		else if( pArgs->restype == RTSTR )
			pProject = theArxWorkspace.FindProject( pArgs->resval.rstring );
		else
			return RSERR; //wrong argument type

		if( !pProject )
			return RSERR; //project not found

		pArgs = pArgs->rbnext;
		if (pArgs == NULL)
			return RSERR; //argument expected

		LPCTSTR pszFilename = NULL;
		if( pArgs->restype == RTSTR )
			pszFilename = pArgs->resval.rstring;
		else
			return RSERR; //wrong argument type

		pArgs = pArgs->rbnext;
		LPCTSTR pszPassword = NULL;
		if (pArgs)
		{
			switch (pArgs->restype)
			{
			case RTSTR:
				pszPassword = pArgs->resval.rstring;
				break;
			case RTNIL:
				break;
			default:
				return RSERR; //wrong argument type
			}

			if (pArgs->rbnext)
				return RSERR; //too many arguments
		}

		if( pProject->GetPassword() != CString( pszPassword ) )
			return RSERR; //wrong password

		if (pProject->WriteToFile(pszFilename) == statOK)
			acedRetT();

		return (RSRSLT) ;
	}

	// ----- ads_dcl_project_import symbol (do not rename)
	static int ads_dcl_project_import(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;
		if (pArgs == NULL)
			return RSERR; //argument expected

		LPCTSTR pszRawData = NULL;
		CString sRawDataIn;
		switch( pArgs->restype )
		{
		case RTSTR:
			pszRawData = pArgs->resval.rstring;
			break;
		case RTLB:
			{
				while( (pArgs = pArgs->rbnext) && pArgs->restype != RTLE )
				{
					switch (pArgs->restype)
					{
					case RTSTR:
						sRawDataIn += pArgs->resval.rstring;
						break;
					case RTNIL:
						break;
					default:
						return RSERR; //wrong argument type
					}
				}
				if( pArgs )
					pArgs = pArgs->rbnext;
				else
					return RSERR; //no matching RTLE found
				pszRawData = sRawDataIn;
			}
			break;
		default:
			return RSERR; //wrong argument type
		}

		pArgs = pArgs->rbnext;
		LPCTSTR pszPassword = NULL;
		LPCTSTR pszProjectKey = NULL;
		if (pArgs)
		{
			switch (pArgs->restype)
			{
			case RTSTR:
				pszPassword = pArgs->resval.rstring;
				break;
			case RTNIL:
				break;
			default:
				return RSERR; //wrong argument type
			}

			pArgs = pArgs->rbnext;

			if (pArgs)
			{
				switch (pArgs->restype)
				{
				case RTSTR:
					pszProjectKey = pArgs->resval.rstring;
					break;
				case RTNIL:
					break;
				default:
					return RSERR; //wrong argument type
				}

				if (pArgs->rbnext)
					return RSERR; //too many arguments
			}
		}

		CArxProject* pProject = NULL;
		std::string sRawData;
	#ifdef _UNICODE
		CStringA sRawDataA( pszRawData );
		sRawData = (LPCSTR)sRawDataA;
	#else
		sRawData = pszRawData;
	#endif
		try
		{
			std::string sDecodedData = base64_decode( sRawData );
			CMemFile Data( (BYTE*)sDecodedData.c_str(), sDecodedData.length() );
			pProject = theArxWorkspace.ImportProject( Data, pszProjectKey );
			if (pProject)
			{
				if( pszPassword )
					pProject->SetPassword( pszPassword );
				acedRetStr( pProject->GetKeyName() );
			}
		}
		catch( ... )
		{
			delete pProject;
			return NULL; 
		}

		return (RSRSLT) ;
	}

	// ----- ads_dcl_project_export symbol (do not rename)
	static int ads_dcl_project_export(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;
		if (pArgs == NULL)
			return RSERR; //argument expected

		CProject* pProject = NULL;
		if (pArgs->restype == RTLONG)
			pProject = (CProject*)pArgs->resval.rlong;
		else if( pArgs->restype == RTSTR )
			pProject = theArxWorkspace.FindProject( pArgs->resval.rstring );
		else
			return RSERR; //wrong argument type

		if( !pProject )
			return RSERR; //project not found

		pArgs = pArgs->rbnext;
		LPCTSTR pszPassword = NULL;
		if (pArgs)
		{
			switch (pArgs->restype)
			{
			case RTSTR:
				pszPassword = pArgs->resval.rstring;
				break;
			case RTNIL:
				break;
			default:
				return RSERR; //wrong argument type
			}

			if (pArgs->rbnext)
				return RSERR; //too many arguments
		}

		if( pProject->GetPassword() != CString( pszPassword ) )
			return RSERR; //wrong password

		CMemFile bufData( 0x10000 );
		if (theArxWorkspace.ExportProject(pProject, bufData))
		{
			UINT cbData = (UINT)bufData.GetLength();
			BYTE* pbData = bufData.Detach();
			CString sRawData = base64_encode( pbData, cbData ).c_str();
			bufData.Attach( pbData, cbData );
			acedRetStr(sRawData);
		}

		return (RSRSLT) ;
	}
} ;

CString CARXApp::msDialogToBeShown;
CPoint CARXApp::mptToBeShown;
CProject* CARXApp::mpProjectToBeShown = NULL;
CString CARXApp::msActionToBeShown;
CDclFormObject* CARXApp::mpDclToBeShown = NULL;
int CARXApp::mnDoneDialogValue = -1;
int CARXApp::mnListOperation = 3; //1 = change selection, 2 = append item, 3 = replace all (default)

//-----------------------------------------------------------------------------
IMPLEMENT_ARX_ENTRYPOINT(CARXApp)

//static
int CARXApp::ads_dcl_GetCtrlProperty(void)
{
	acedRetNil();
	int nFunctionCode = acedGetFunCode();
	if( nFunctionCode < ADSPROPFUNCBASE || nFunctionCode > ADSPROPFUNCBASE + nMaxPropertyId )
		return RSERR;
	GetCtrlProperty( static_cast<PropertyId>(nFunctionCode - ADSPROPFUNCBASE) );
	return RSRSLT;
}

//static
int CARXApp::ads_dcl_SetCtrlProperty(void)
{
	acedRetNil();
	int nFunctionCode = acedGetFunCode();
	int nSetPropertyBase = ADSPROPFUNCBASE + nMaxPropertyId;
	if( nFunctionCode < nSetPropertyBase || nFunctionCode > nSetPropertyBase + nMaxPropertyId )
		return RSERR;
	SetCtrlProperty( static_cast<PropertyId>(nFunctionCode - nSetPropertyBase) );
	return RSRSLT;
}


#ifdef _DIAGNOSTIC

static int DumpProject(void)
{
	struct resbuf *pArgs =acedGetArgs () ;

	CProject* pProject = NULL;
	if (!RetrieveProjectFromArgs (pArgs, pProject))
		return RSERR; //invalid input

	if (!pProject)
		return RSRSLT; //project not found

	//optional arguments
	bool bDeep = false;
	if (pArgs)
	{
		bDeep = (pArgs->restype != RTNIL);

		if (pArgs->rbnext != NULL)
			return RSERR; //too many arguments
	}

	pProject->dump( bDeep );

	return RSRSLT;
}

static int DumpForm(void)
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDclFormObject* pForm = NULL;
	if (!RetrieveFormFromArgs (pArgs, pForm))
		return RSERR; //invalid input

	if (!pForm)
		return RSRSLT; //form not found

	//optional arguments
	bool bDeep = false;
	if (pArgs)
	{
		bDeep = (pArgs->restype != RTNIL);

		if (pArgs->rbnext != NULL)
			return RSERR; //too many arguments
	}

	pForm->dump( bDeep );

	return RSRSLT;
}

static int DumpControl(void)
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDclControlObject* pControl = NULL;
	if (!RetrieveControlFromArgs (pArgs, pControl))
		return RSERR; //invalid input

	if (!pControl)
		return RSRSLT; //control not found

	//optional arguments
	bool bDeep = false;
	if (pArgs)
	{
		bDeep = (pArgs->restype != RTNIL);

		if (pArgs->rbnext != NULL)
			return RSERR; //too many arguments
	}

	pControl->dump( bDeep );

	return RSRSLT;
}

#endif //_DIAGNOSTIC

ACED_ARXCOMMAND_ENTRY_AUTO(CARXApp, OpenDCL, OpenDCL, OpenDCL, ACRX_CMD_TRANSPARENT, NULL)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_getversion, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_getversionex, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_loadproject, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_load_dialog, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_sendstring, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_invokefunc, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_new_dialog, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_unload_dialog, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_done_dialog, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_start_dialog, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_start_list, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_add_list, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_end_list, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_action_tile, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_set_tile, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_mode_tile, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_get_tile, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_get_attr, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_start_image, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_vector_image, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_fill_image, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_dimx_tile, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_dimy_tile, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_end_image, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_slide_image, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_getlinetype, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_getlineweight, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_form_show, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_form_center, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_form_resize, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_form_setfocus, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_form_hide, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_form_close, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_form_getrectangle, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_form_getcontrolarea, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_form_closeall, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_form_isfloating, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_form_isactive, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_configtab_isapplyenabled, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_configtab_setapplyenabled, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_getscreensize, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_form_gettitlebartext, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_form_settitlebartext, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_messagebox, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_form_setdialogminmaxsizes, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_form_gethwnd, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_form_isenabled, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_form_isvisible, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_form_enable, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_getharddrivesize, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_getfocus, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_hideerrormsgbox, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_showerrormsgbox, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_getblocksize, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_control_gethwnd, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_navigatetourl, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_activateemail, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_browsefolder, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_insert, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_loadxref, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_updatevarnames, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_forcedwgredraw, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_getmousecoords, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_getdwgmousecoords, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_multifiledialog, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_getpicturesize, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_setcmdbarfocus, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_flushgraphicbuttons, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_xtwipstopixels, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_ytwipstopixels, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_xpixelstotwips, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_ypixelstotwips, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_getcolorvalue, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_registeractivexctrl, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_project_load, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_project_unload, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_project_saveas, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_project_import, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_project_export, true)
