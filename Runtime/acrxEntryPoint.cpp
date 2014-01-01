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
#include "ArxWorkspace.h"
#include "PropertyNames.h"
#include "DclControlObject.h"
#include "PropertyObject.h"
#include "ArgumentsRetrieval.h"
#include "Methods_ActiveX.h"
#include "Methods_Animation.h"
#include "Methods_BinFile.h"
#include "Methods_BlockList.h"
#include "Methods_BlockView.h"
#include "Methods_Calendar.h"
#include "Methods_ComboBox.h"
#include "Methods_Control.h"
#include "Methods_DwgList.h"
#include "Methods_DwgPreview.h"
#include "Methods_FileExplorer.h"
#include "Methods_Form.h"
#include "Methods_Grid.h"
#include "Methods_Hatch.h"
#include "Methods_Html.h"
#include "Methods_ImageComboBox.h"
#include "Methods_ListBox.h"
#include "Methods_ListView.h"
#include "Methods_OptionList.h"
#include "Methods_PictureBox.h"
#include "Methods_ProgressBar.h"
#include "Methods_SlideView.h"
#include "Methods_TabStrip.h"
#include "Methods_TextBox.h"
#include "Methods_Tree.h"
#include "DialogObject.h"
#include "LayeredWindowHelperST.h"
#include "ControlTypes.h"
#include "ArchiveEx.h"
#include "InvokeMethod.h"
#include "DelayedInvoke.h"
#include "ArxDialogControl.h"
#include "StringCompare.h"
#include "FolderBrowseDlg.h"
#include "PictureObject.h"
#include "ArxAcadSlideCtrl.h"
#include "AcadColorTable.h"
#include "LineWeightDlg.h"
#include "LinetypeDlg.h"
#include "Base64.h"
#include "UpdateCheck.h"


//-----------------------------------------------------------------------------
#define szRDS _RXST("dcl")
#define ADSFUNCBASE 1000 //the externally defined ADS functions will be defined starting at this function index
#define ADSPROPFUNCBASE 2000 //the property functions will be defined starting at this function index

#ifdef _BRXTARGET
#define theRxApp singleInstanceEpObject
#else
#define theRxApp entryPointObject
#endif

static LPCTSTR gpszDclEventsLspFileName = _T("DclEvents.lsp");
static LPCTSTR gpszOnActionEventLispFunction = _T("(defun c:OnActionEvent_%s()%s)(princ)\n");

#ifdef _PRERELEASE
static LPCTSTR gpszUCProductName = _T("OpenDCL Runtime Dev");
#else
static LPCTSTR gpszUCProductName = _T("OpenDCL Runtime");
#endif


#ifdef _DIAGNOSTIC
static int DumpProject(void);
static int DumpForm(void);
static int DumpControl(void);
#endif //_DIAGNOSTIC


static const struct AdsFunctionTableEntry { LPCTSTR pszFunctionName; int (*pfHandler) (); } grAdsFunctionTable[] =
{
#ifdef _DIAGNOSTIC
	{_T("Project_Dump"),                 DumpProject},
	{_T("Form_Dump"),                    DumpForm},
	{_T("Control_Dump"),                 DumpControl},
#endif //_DIAGNOSTIC

	// General control methods
	{_T("Control_GetHWND"),              Control::GetHWND},
	{_T("Control_GetName"),              Control::GetName},
	{_T("Control_GetProperties"),        Control::GetProperties},
	{_T("Control_GetProperty"),          Control::GetProperty}, //used to get any property of any control
	{_T("Control_GetPos"),               Control::GetPos},
	{_T("Control_Redraw"),               Control::Redraw},
	{_T("Control_SetFocus"),             Control::SetFocus},
	{_T("Control_SetPos"),               Control::SetPos},
	{_T("Control_SetProperty"),          Control::SetProperty}, //used to set any property of any control
	{_T("Control_ShowToolTip"),          Control::ShowToolTip},
	{_T("Control_ZOrder"),               Control::ZOrder},

	// General control deprecated methods
	{_T("Control_GetCurPos"),            Control::GetPos},
	{_T("Control_ForceUpdateNow"),       Control::Redraw},

	// Property get/set deprecated methods
	{_T("Control_GetAlternateColor"),    T_GetCtrlProperty< Prop::AlternatingColor >},
	{_T("Control_SetAlternateColor"),    T_SetCtrlProperty< Prop::AlternatingColor >},
	{_T("Control_GetLimitText"),         T_GetCtrlProperty< Prop::TextLimit >},
	{_T("Control_SetLimitText"),         T_SetCtrlProperty< Prop::TextLimit >},
	{_T("Control_GetEditLabel"),         T_GetCtrlProperty< Prop::EditLabels >},
	{_T("Control_SetEditLabel"),         T_SetCtrlProperty< Prop::EditLabels >},
	{_T("Control_GetURLAddress"),        T_GetCtrlProperty< Prop::Hyperlink >},
	{_T("Control_SetURLAddress"),        T_SetCtrlProperty< Prop::Hyperlink >},
	{_T("Control_GetSingleExpanded"),    T_GetCtrlProperty< Prop::SingleClickExpand >},
	{_T("Control_SetSingleExpanded"),    T_SetCtrlProperty< Prop::SingleClickExpand >},

	// General form methods
	{_T("Form_Center"),                  Form::Center},
	{_T("Form_Close"),                   Form::Close},
	{_T("Form_CloseAll"),                Form::CloseAll},
	{_T("Form_Enable"),                  Form::Enable},
	{_T("Form_GetControlArea"),          Form::GetControlArea},
	{_T("Form_GetControls"),             Form::GetControls},
	{_T("Form_GetUUID"),                 Form::GetUUID},
	{_T("Form_Hide"),                    Form::Hide},
	{_T("Form_IsActive"),                Form::IsActive},
	{_T("Form_IsApplyEnabled"),          Form::IsApplyEnabled},
	{_T("Form_IsEnabled"),               Form::IsEnabled},
	{_T("Form_IsFloating"),              Form::IsFloating},
	{_T("Form_IsVisible"),               Form::IsVisible},
	{_T("Form_RecalcLayout"),            Form::RecalcLayout},
	{_T("Form_Resize"),                  Form::Resize},
	{_T("Form_SetApplyEnabled"),         Form::SetApplyEnabled},
	{_T("Form_SetMinMaxSizes"),          Form::SetMinMaxSizes},
	{_T("Form_Show"),                    Form::Show},
	{_T("Form_StartTimer"),              Form::StartTimer},

	// General form deprecated methods
	{_T("ConfigTab_IsApplyEnabled"),     Form::IsApplyEnabled},
	{_T("ConfigTab_SetApplyEnabled"),    Form::SetApplyEnabled},
	{_T("FlushGraphicButtons"),          Form::RecalcLayout},
	{_T("Form_GetHWND"),                 Control::GetHWND},
	{_T("Form_GetRectangle"),            Control::GetPos},
	{_T("Form_GetTitleBarText"),         T_GetCtrlProperty< Prop::TitleBarText >},
	{_T("Form_SetDialogMinMaxSizes"),    Form::SetMinMaxSizes},
	{_T("Form_SetFocus"),                Control::SetFocus},
	{_T("Form_SetPos"),                  Control::SetPos},
	{_T("Form_SetTitleBarText"),         T_SetCtrlProperty< Prop::TitleBarText >},

	// Animation control methods
	{_T("Animation_Close"),              Animation::Close},
	{_T("Animation_Load"),               Animation::Load},
	{_T("Animation_Play"),               Animation::Play},
	{_T("Animation_Seek"),               Animation::Seek},
	{_T("Animation_Stop"),               Animation::Stop},

	// Animation control deprecated methods
	{_T("Animate_Close"),                Animation::Close},
	{_T("Animate_Load"),                 Animation::Load},
	{_T("Animate_Play"),                 Animation::Play},
	{_T("Animate_Seek"),                 Animation::Seek},
	{_T("Animate_Stop"),                 Animation::Stop},

	// ActiveX Control methods
	{_T("AxControl_Get"),                AxControl::Get},
	{_T("AxControl_GetAxObject"),        AxControl::GetAxObject},
	{_T("AxControl_Invoke"),             AxControl::Invoke},
	{_T("AxControl_Put"),                AxControl::Put},

	// ActiveX Control deprecated methods
	{_T("AxControl_GetProperty"),        AxControl::Get},
	{_T("AxControl_SetProperty"),        AxControl::Put},
	{_T("AxControl_DoMethod"),           AxControl::Invoke},
	{_T("AxControl_GetOleObject"),       AxControl::GetAxObject},

	// ActiveX general functions
	{_T("GetOlePictureFromId"),          AxGeneral::GetOlePictureFromId},
	{_T("GetOlePictureFromFile"),        AxGeneral::GetOlePictureFromFile},

	// ActiveX Object methods
	{_T("AxObject_Get"),                 AxObject::Get},
	{_T("AxObject_Invoke"),              AxObject::Invoke},
	{_T("AxObject_Put"),                 AxObject::Put},
	{_T("AxObject_Release"),             AxObject::Release},

	// ActiveX Object deprecated methods
	{_T("AxObject_Close"),               AxObject::Release},
	{_T("AxObject_DoMethod"),            AxObject::Invoke},
	{_T("AxObject_GetProperty"),         AxObject::Get},
	{_T("AxObject_SetProperty"),         AxObject::Put},

	// Binary File methods
	{_T("BinFile_Check"),                BinFile::Check},						
	{_T("BinFile_Close"),                BinFile::Close},						
	{_T("BinFile_Open"),                 BinFile::Open},
	{_T("BinFile_Read"),                 BinFile::Read},
	{_T("BinFile_Write"),                BinFile::Write},

	// Binary File deprecated methods
	{_T("CheckBin"),                     BinFile::Check},						
	{_T("CloseBin"),                     BinFile::Close},						
	{_T("OpenBin"),                      BinFile::Open},
	{_T("ReadBin"),                      BinFile::Read},
	{_T("WriteBin"),                     BinFile::Write},

	// Block List control methods
	{_T("BlockList_Arrange"),            BlockList::Arrange},
	{_T("BlockList_GetCount"),           BlockList::GetCount},
	{_T("BlockList_GetCurSel"),          BlockList::GetCurSel},
	{_T("BlockList_GetFileName"),        BlockList::GetFileName},
	{_T("BlockList_GetItemText"),        BlockList::GetItemText},
	{_T("BlockList_GetSelCount"),        BlockList::GetSelCount},
	{_T("BlockList_GetSelectedItems"),   BlockList::GetSelectedItems},
	{_T("BlockList_GetSelectedNths"),    BlockList::GetSelectedNths},
	{_T("BlockList_LoadDwg"),            BlockList::LoadDwg},
	{_T("BlockList_Reset"),              BlockList::Reset},
	{_T("BlockList_SetCurSel"),          BlockList::SetCurSel},
	{_T("BlockList_SortTextItems"),      BlockList::SortTextItems},

	// Block View control methods
	{_T("BlockView_Clear"),              BlockView::Clear},
	{_T("BlockView_DisplayBlock"),       BlockView::DisplayBlock},
	{_T("BlockView_DisplayBlockToScale"),BlockView::DisplayBlockToScale},
	{_T("BlockView_DisplayDwg"),         BlockView::DisplayDwg},
	{_T("BlockView_DisplayDwgToScale"),  BlockView::DisplayDwgToScale},
	{_T("BlockView_DisplayPaperSpace"),  BlockView::DisplayPaperSpace},
	{_T("BlockView_GetBlockList"),       BlockView::GetBlockList},
	//{_T("BlockView_GetBlockSize"),       BlockView::GetBlockSize},
	{_T("BlockView_GetDwgSize"),         BlockView::GetDwgSize},
	{_T("BlockView_PreLoadDwg"),         BlockView::PreLoadDwg},
	{_T("BlockView_RefreshBlock"),       BlockView::RefreshBlock},
	{_T("BlockView_RemoveHighlight"),    BlockView::RemoveHighlight},
	{_T("BlockView_SetHighlight"),       BlockView::SetHighlight},
	{_T("BlockView_Zoom"),               BlockView::Zoom},

	// Block View control deprecated methods
	{_T("BlockView_LoadDwg"),            BlockView::DisplayDwg},
	{_T("BlockView_LoadDwgToScale"),     BlockView::DisplayDwgToScale},

	// Calendar control methods
	{_T("Calendar_GetCurSel"),           Calendar::GetCurSel},
	{_T("Calendar_GetMonthRangeEnd"),    Calendar::GetMonthRangeEnd},
	{_T("Calendar_GetMonthRangeStart"),  Calendar::GetMonthRangeStart},
	{_T("Calendar_GetRangeEnd"),         Calendar::GetRangeEnd},
	{_T("Calendar_GetRangeStart"),       Calendar::GetRangeStart},
	{_T("Calendar_GetSelRangeEnd"),      Calendar::GetSelRangeEnd},
	{_T("Calendar_GetSelRangeStart"),    Calendar::GetSelRangeStart},
	{_T("Calendar_GetToday"),            Calendar::GetToday},
	{_T("Calendar_SetCurSel"),           Calendar::SetCurSel},
	{_T("Calendar_SetRange"),            Calendar::SetRange},
	{_T("Calendar_SetSelRange"),         Calendar::SetSelRange},

	// Calendar control deprecated methods
	{_T("Month_GetCurSel"),              Calendar::GetCurSel},
	{_T("Month_GetMonthRangeEnd"),       Calendar::GetMonthRangeEnd},
	{_T("Month_GetMonthRangeStart"),     Calendar::GetMonthRangeStart},
	{_T("Month_GetRangeEnd"),            Calendar::GetRangeEnd},
	{_T("Month_GetRangeStart"),          Calendar::GetRangeStart},
	{_T("Month_GetSelRangeEnd"),         Calendar::GetSelRangeEnd},
	{_T("Month_GetSelRangeStart"),       Calendar::GetSelRangeStart},
	{_T("Month_GetToday"),               Calendar::GetToday},
	{_T("Month_SetCurSel"),              Calendar::SetCurSel},
	{_T("Month_SetRange"),               Calendar::SetRange},
	{_T("Month_SetSelRange"),            Calendar::SetSelRange},

	// Combo Box control methods
	{_T("ComboBox_AddColor"),            ComboBox::AddColor},
	{_T("ComboBox_AddList"),             ComboBox::AddList},
	{_T("ComboBox_AddPath"),             ComboBox::AddPath},											
	{_T("ComboBox_AddString"),           ComboBox::AddString},
	{_T("ComboBox_Clear"),               ComboBox::Clear},
	{_T("ComboBox_ClearEdit"),           ComboBox::ClearEdit},
	{_T("ComboBox_DeleteItem"),          ComboBox::DeleteItem},
	{_T("ComboBox_Dir"),                 ComboBox::Dir},
	{_T("ComboBox_FindColor"),           ComboBox::FindColor},
	{_T("ComboBox_FindLineWeight"),      ComboBox::FindLineWeight},
	{_T("ComboBox_FindString"),          ComboBox::FindString},											
	{_T("ComboBox_FindStringExact"),     ComboBox::FindStringExact},
	{_T("ComboBox_GetCount"),            ComboBox::GetCount},
	{_T("ComboBox_GetCurSel"),           ComboBox::GetCurSel},
	{_T("ComboBox_GetDir"),              ComboBox::GetDir},
	{_T("ComboBox_GetDroppedWidth"),     ComboBox::GetDroppedWidth},
	{_T("ComboBox_GetEBText"),           ComboBox::GetEBText},
	{_T("ComboBox_GetEditSel"),          ComboBox::GetEditSel},
	{_T("ComboBox_GetItemData"),         ComboBox::GetItemData},
	{_T("ComboBox_GetLBText"),           ComboBox::GetLBText},
	{_T("ComboBox_GetTopIndex"),         ComboBox::GetTopIndex},
	{_T("ComboBox_InsertString"),        ComboBox::InsertString},
	{_T("ComboBox_SelectString"),        ComboBox::SelectString},
	{_T("ComboBox_SetCurSel"),           ComboBox::SetCurSel},
	{_T("ComboBox_SetDroppedWidth"),     ComboBox::SetDroppedWidth},
	{_T("ComboBox_SetEditSel"),          ComboBox::SetEditSel},
	{_T("ComboBox_SetItemData"),         ComboBox::SetItemData},
	{_T("ComboBox_SetTopIndex"),         ComboBox::SetTopIndex},

	// Combo Box control deprecated methods
	{_T("ComboBox_DeleteString"),        ComboBox::DeleteItem},
	{_T("ComboBox_GetTBText"),           ComboBox::GetEBText},

	// DWG List control methods
	{_T("DwgList_DeleteItem"),           DwgList::DeleteItem},
	{_T("DwgList_Dir"),                  DwgList::Dir},
	{_T("DwgList_GetAnchorIndex"),       DwgList::GetAnchorIndex},
	{_T("DwgList_GetCount"),             DwgList::GetCount},
	{_T("DwgList_GetCurSel"),            DwgList::GetCurSel},
	{_T("DwgList_GetDir"),               DwgList::GetDir},
	{_T("DwgList_GetFileName"),          DwgList::GetFileName},
	{_T("DwgList_GetFocusIndex"),        DwgList::GetFocusIndex},
	{_T("DwgList_GetSelCount"),          DwgList::GetSelCount},
	{_T("DwgList_GetSelectedItems"),     DwgList::GetSelectedItems},	
	{_T("DwgList_GetSelectedNths"),      DwgList::GetSelectedNths},	
	{_T("DwgList_GetText"),              DwgList::GetText},
	{_T("DwgList_GetTopIndex"),          DwgList::GetTopIndex},
	{_T("DwgList_GetType"),              DwgList::GetType},
	{_T("DwgList_IsItemSelected"),       DwgList::IsItemSelected},
	{_T("DwgList_SelectItem"),           DwgList::SelectItem},
	{_T("DwgList_SelItemRange"),         DwgList::SelItemRange},
	{_T("DwgList_SetAnchorIndex"),       DwgList::SetAnchorIndex},
	{_T("DwgList_SetCurSel"),            DwgList::SetCurSel},
	{_T("DwgList_SetFocusIndex"),        DwgList::SetFocusIndex},
	{_T("DwgList_SetTopIndex"),          DwgList::SetTopIndex},

	// DWG List control deprecated methods
	{_T("DwgList_DeleteString"),         DwgList::DeleteItem},
	{_T("DwgList_GetSel"),               DwgList::IsItemSelected},
	{_T("DwgList_SetSel"),               DwgList::SelectItem},

	// DWG Preview control methods
	{_T("DwgPreview_Clear"),             DwgPreview::Clear},
	{_T("DwgPreview_GetDwgName"),        DwgPreview::GetDwgName},
	{_T("DwgPreview_LoadDwg"),           DwgPreview::LoadDwg},
	{_T("DwgPreview_RemoveHighlight"),   DwgPreview::RemoveHighlight},
	{_T("DwgPreview_SetHighlight"),      DwgPreview::SetHighlight},

	// File Explorer control methods
	{_T("FileExplorer_GetFileExt"),      FileExplorer::GetFileExt},
	{_T("FileExplorer_GetFileName"),     FileExplorer::GetFileName},
	{_T("FileExplorer_GetFileNameList"), FileExplorer::GetFileNameList},
	{_T("FileExplorer_GetFileTitle"),    FileExplorer::GetFileTitle},
	{_T("FileExplorer_GetFolderName"),   FileExplorer::GetFolderName},
	{_T("FileExplorer_GetFolderPath"),   FileExplorer::GetFolderPath},
	{_T("FileExplorer_GetPathName"),     FileExplorer::GetPathName},
	{_T("FileExplorer_GetSelCount"),     FileExplorer::GetSelCount},
	{_T("FileExplorer_SetOkButtonText"), FileExplorer::SetOkButtonText},

	// File Explorer control deprecated methods
	{_T("FileDlg_GetFileExt"),           FileExplorer::GetFileExt},
	{_T("FileDlg_GetFileName"),          FileExplorer::GetFileName},
	{_T("FileDlg_GetFileNameList"),      FileExplorer::GetFileNameList},
	{_T("FileDlg_GetFileTitle"),         FileExplorer::GetFileTitle},
	{_T("FileDlg_GetFolderName"),        FileExplorer::GetFolderName},
	{_T("FileDlg_GetFolderPath"),        FileExplorer::GetFolderPath},
	{_T("FileDlg_GetPathName"),          FileExplorer::GetPathName},
	{_T("FileDlg_GetSelectionCount"),    FileExplorer::GetSelCount},
	{_T("FileDlg_SetOkButtonText"),      FileExplorer::SetOkButtonText},

	// Grid control methods
	{_T("Grid_AddColumns"),              Grid::AddColumns},
	{_T("Grid_AddRow"),                  Grid::AddRow},
	{_T("Grid_AddString"),               Grid::AddString},
	{_T("Grid_CalcColumnWidth"),         Grid::CalcColumnWidth},
	{_T("Grid_CancelCellEdit"),          Grid::CancelCellEdit},
	{_T("Grid_Clear"),                   Grid::Clear},
	{_T("Grid_DeleteColumn"),            Grid::DeleteColumn},
	{_T("Grid_DeleteRow"),               Grid::DeleteRow},
	{_T("Grid_FillList"),                Grid::FillList},
	{_T("Grid_GetCellCheckState"),       Grid::GetCellCheckState},
	{_T("Grid_GetCellDropList"),         Grid::GetCellDropList},
	{_T("Grid_GetCellImages"),           Grid::GetCellImages},
	{_T("Grid_GetCellStyle"),            Grid::GetCellStyle},
	{_T("Grid_GetCellText"),             Grid::GetCellText},
	{_T("Grid_GetColumnCaption"),        Grid::GetColumnCaption},
	{_T("Grid_GetColumnCells"),          Grid::GetColumnCells},
	{_T("Grid_GetColumnCount"),          Grid::GetColumnCount},
	{_T("Grid_GetColumnImage"),          Grid::GetColumnImage},
	{_T("Grid_GetColumnWidth"),          Grid::GetColumnWidth},
	{_T("Grid_GetCurCell"),              Grid::GetCurCell},	
	{_T("Grid_GetRowCells"),             Grid::GetRowCells},
	{_T("Grid_GetRowCount"),             Grid::GetRowCount},
	{_T("Grid_GetRowData"),              Grid::GetRowData},
	{_T("Grid_HitPointTest"),            Grid::HitPointTest},
	{_T("Grid_InsertColumn"),            Grid::InsertColumn},
	{_T("Grid_InsertRow"),               Grid::InsertRow},
	{_T("Grid_InsertString"),            Grid::InsertString},
	{_T("Grid_SetCellCheckState"),       Grid::SetCellCheckState},
	{_T("Grid_SetCellDropList"),         Grid::SetCellDropList},
	{_T("Grid_SetCellImages"),           Grid::SetCellImages},
	{_T("Grid_SetCellStyle"),            Grid::SetCellStyle},											
	{_T("Grid_SetCellText"),             Grid::SetCellText},
	{_T("Grid_SetColumnCaption"),        Grid::SetColumnCaption},
	{_T("Grid_SetColumnImage"),          Grid::SetColumnImage},
	{_T("Grid_SetColumnWidth"),          Grid::SetColumnWidth},
	{_T("Grid_SetCurCell"),              Grid::SetCurCell},
	{_T("Grid_SetRowData"),              Grid::SetRowData},
	{_T("Grid_SortNumericCells"),        Grid::SortNumericCells},											
	{_T("Grid_SortTextCells"),           Grid::SortTextCells},
	{_T("Grid_StartCellEdit"),           Grid::StartCellEdit},

	// Grid control deprecated methods
	{_T("Grid_CalcColWidth"),            Grid::CalcColumnWidth},
	{_T("Grid_CancelItemEdit"),          Grid::CancelCellEdit},
	{_T("Grid_GetColCount"),             Grid::GetColumnCount},
	{_T("Grid_GetColImage"),             Grid::GetColumnImage},
	{_T("Grid_GetColItems"),             Grid::GetColumnCells},
	{_T("Grid_GetColWidth"),             Grid::GetColumnWidth},
	{_T("Grid_GetCurSel"),               Grid::GetCurCell},	
	{_T("Grid_GetItemCheck"),            Grid::GetCellCheckState},
	{_T("Grid_GetItemData"),             Grid::GetRowData},
	{_T("Grid_GetItemDropList"),         Grid::GetCellDropList},
	{_T("Grid_GetItemImage"),            Grid::GetCellImages},
	{_T("Grid_GetItemText"),             Grid::GetCellText},
	{_T("Grid_GetRowItems"),             Grid::GetRowCells},
	{_T("Grid_SetColImage"),             Grid::SetColumnImage},
	{_T("Grid_SetColWidth"),             Grid::SetColumnWidth},
	{_T("Grid_SelCurCell"),              Grid::SetCurCell},
	{_T("Grid_SelCurRow"),               Grid::SetCurCell},
	{_T("Grid_SetItemCheck"),            Grid::SetCellCheckState},
	{_T("Grid_SetItemData"),             Grid::SetRowData},
	{_T("Grid_SetItemDropList"),         Grid::SetCellDropList},
	{_T("Grid_SetItemImage"),            Grid::SetCellImages},
	{_T("Grid_SetItemStyle"),            Grid::SetCellStyle},											
	{_T("Grid_SetItemText"),             Grid::SetCellText},
	{_T("Grid_SortNumericItems"),        Grid::SortNumericCells},											
	{_T("Grid_SortTextItems"),           Grid::SortTextCells},
	{_T("Grid_StartItemEdit"),           Grid::StartCellEdit},

	// Hatch control methods
	{_T("Hatch_Clear"),                  Hatch::Clear},
	{_T("Hatch_SetPattern"),             Hatch::SetPattern},

	// Html control methods
	{_T("Html_GetBusy"),                 Html::GetBusy},
	{_T("Html_GetFullName"),             Html::GetFullName},
	{_T("Html_GetHtmlDocument"),         Html::GetHtmlDocument},
	{_T("Html_GetLocationName"),         Html::GetLocationName},
	{_T("Html_GetLocationURL"),          Html::GetLocationURL},
	{_T("Html_GetOffline"),              Html::GetOffline},
	{_T("Html_GoBack"),                  Html::GoBack},
	{_T("Html_GoForward"),               Html::GoForward},
	{_T("Html_GoHome"),                  Html::GoHome},
	{_T("Html_GoSearch"),                Html::GoSearch},
	{_T("Html_Navigate"),                Html::Navigate},
	{_T("Html_Refresh"),                 Html::Refresh},
	{_T("Html_ReplaceText"),             Html::ReplaceText},
	{_T("Html_SetOffline"),              Html::SetOffline},
	{_T("Html_Stop"),                    Html::Stop},
	{_T("Html_UpdateHtmlCode"),          Html::UpdateHtmlCode},

	// Image Combo Box control methods
	{_T("ImageComboBox_AddString"),      ImageComboBox::AddString},
	{_T("ImageComboBox_Clear"),          ImageComboBox::Clear},
	{_T("ImageComboBox_ClearEdit"),      ImageComboBox::ClearEdit},
	{_T("ImageComboBox_DeleteItem"),     ImageComboBox::DeleteItem},
	{_T("ImageComboBox_FindString"),     ImageComboBox::FindString},											
	{_T("ImageComboBox_FindStringExact"), ImageComboBox::FindStringExact},
	{_T("ImageComboBox_GetCount"),       ImageComboBox::GetCount},
	{_T("ImageComboBox_GetCurSel"),      ImageComboBox::GetCurSel},
	{_T("ImageComboBox_GetDroppedWidth"), ImageComboBox::GetDroppedWidth},
	{_T("ImageComboBox_GetEBText"),      ImageComboBox::GetEBText},
	{_T("ImageComboBox_GetEditSel"),     ImageComboBox::GetEditSel},
	{_T("ImageComboBox_GetItem"),        ImageComboBox::GetItem},
	{_T("ImageComboBox_GetItemData"),    ImageComboBox::GetItemData},
	{_T("ImageComboBox_GetLBText"),      ImageComboBox::GetLBText},
	{_T("ImageComboBox_GetTopIndex"),    ImageComboBox::GetTopIndex},
	{_T("ImageComboBox_SelectString"),   ImageComboBox::SelectString},
	{_T("ImageComboBox_SetCurSel"),      ImageComboBox::SetCurSel},
	{_T("ImageComboBox_SetDroppedWidth"), ImageComboBox::SetDroppedWidth},
	{_T("ImageComboBox_SetEditSel"),     ImageComboBox::SetEditSel},
	{_T("ImageComboBox_SetItem"),        ImageComboBox::SetItem},
	{_T("ImageComboBox_SetItemData"),    ImageComboBox::SetItemData},
	{_T("ImageComboBox_SetTopIndex"),    ImageComboBox::SetTopIndex},

	// Image Combo Box control deprecated methods
	{_T("ImageComboBox_DeleteString"),   ImageComboBox::DeleteItem},
	{_T("ImageComboBox_GetTBText"),      ImageComboBox::GetEBText},

	// List Box control methods
	{_T("ListBox_AddList"),              ListBox::AddList},		
	{_T("ListBox_AddString"),            ListBox::AddString},
	{_T("ListBox_Clear"),                ListBox::Clear},											
	{_T("ListBox_DeleteItem"),           ListBox::DeleteItem},
	{_T("ListBox_Dir"),                  ListBox::Dir},
	{_T("ListBox_FindString"),           ListBox::FindString},
	{_T("ListBox_FindStringExact"),      ListBox::FindStringExact},
	{_T("ListBox_GetAnchorIndex"),       ListBox::GetAnchorIndex},
	{_T("ListBox_GetCount"),             ListBox::GetCount},
	{_T("ListBox_GetCurSel"),            ListBox::GetCurSel},
	{_T("ListBox_GetFocusIndex"),        ListBox::GetFocusIndex},
	{_T("ListBox_GetItemData"),          ListBox::GetItemData},
	{_T("ListBox_GetItemText"),          ListBox::GetItemText},
	{_T("ListBox_GetSelCount"),          ListBox::GetSelCount},
	{_T("ListBox_GetSelectedItems"),     ListBox::GetSelectedItems},	
	{_T("ListBox_GetSelectedNths"),      ListBox::GetSelectedNths},	
	{_T("ListBox_GetTopIndex"),          ListBox::GetTopIndex},
	{_T("ListBox_HitPointTest"),         ListBox::HitPointTest},
	{_T("ListBox_InsertString"),         ListBox::InsertString},
	{_T("ListBox_IsItemSelected"),       ListBox::IsItemSelected},
	{_T("ListBox_SelectItem"),           ListBox::SelectItem},
	{_T("ListBox_SelectString"),         ListBox::SelectString},
	{_T("ListBox_SelItemRange"),         ListBox::SelItemRange},
	{_T("ListBox_SetAnchorIndex"),       ListBox::SetAnchorIndex},
	{_T("ListBox_SetCurSel"),            ListBox::SetCurSel},
	{_T("ListBox_SetFocusIndex"),        ListBox::SetFocusIndex},
	{_T("ListBox_SetItemData"),          ListBox::SetItemData},
	{_T("ListBox_SetTopIndex"),          ListBox::SetTopIndex},

	// List Box control deprecated methods
	{_T("ListBox_DeleteString"),         ListBox::DeleteItem},
	{_T("ListBox_GetSel"),               ListBox::IsItemSelected},
	{_T("ListBox_GetText"),              ListBox::GetItemText},
	{_T("ListBox_SetSel"),               ListBox::SelectItem},

	// List View control methods
	{_T("ListView_AddColumns"),          ListView::AddColumns},
	{_T("ListView_AddItem"),             ListView::AddItem},
	{_T("ListView_AddString"),           ListView::AddString},
	{_T("ListView_Arrange"),             ListView::Arrange},
	{_T("ListView_CalcColumnWidth"),     ListView::CalcColumnWidth},
	{_T("ListView_CancelLabelEdit"),     ListView::CancelLabelEdit},
	{_T("ListView_Clear"),               ListView::Clear},
	{_T("ListView_DeleteColumn"),        ListView::DeleteColumn},
	{_T("ListView_DeleteColumns"),       ListView::DeleteColumns},
	{_T("ListView_DeleteItem"),          ListView::DeleteItem},
	{_T("ListView_DeleteItems"),         ListView::DeleteItems},
	{_T("ListView_FillList"),            ListView::FillList},
	{_T("ListView_GetColumnCount"),      ListView::GetColumnCount},
	{_T("ListView_GetColumnImage"),      ListView::GetColumnImage},
	{_T("ListView_GetColumnItems"),      ListView::GetColumnItems},
	{_T("ListView_GetColumnWidth"),      ListView::GetColumnWidth},
	{_T("ListView_GetCount"),            ListView::GetCount},
	{_T("ListView_GetCurSel"),           ListView::GetCurSel},
	{_T("ListView_GetItemData"),         ListView::GetItemData},
	{_T("ListView_GetItemImage"),        ListView::GetItemImage},
	{_T("ListView_GetItemText"),         ListView::GetItemText},
	{_T("ListView_GetRowItems"),         ListView::GetRowItems},
	{_T("ListView_GetSelCount"),         ListView::GetSelCount},
	{_T("ListView_GetSelectedItems"),    ListView::GetSelectedItems},	
	{_T("ListView_GetSelectedNths"),     ListView::GetSelectedNths},
	{_T("ListView_HitPointTest"),        ListView::HitPointTest},
	{_T("ListView_InsertItem"),          ListView::InsertItem},
	{_T("ListView_SetColumnImage"),      ListView::SetColumnImage},
	{_T("ListView_SetColumnWidth"),      ListView::SetColumnWidth},
	{_T("ListView_SetCurSel"),           ListView::SetCurSel},
	{_T("ListView_SetItemData"),         ListView::SetItemData},
	{_T("ListView_SetItemImage"),        ListView::SetItemImage},
	{_T("ListView_SetItemText"),         ListView::SetItemText},
	{_T("ListView_SortNumericItems"),    ListView::SortNumericItems},
	{_T("ListView_SortTextItems"),       ListView::SortTextItems},
	{_T("ListView_StartLabelEdit"),      ListView::StartLabelEdit},

	// List View control deprecated methods
	{_T("ListView_CalcColWidth"),        ListView::CalcColumnWidth},
	{_T("ListView_CountItems"),          ListView::GetCount},
	{_T("ListView_GetColWidth"),         ListView::GetColumnWidth},
	{_T("ListView_SetColWidth"),         ListView::SetColumnWidth},

	// Option List control methods
	{_T("OptionList_AddButton"),         OptionList::AddButton},
	{_T("OptionList_AddList"),           OptionList::AddList},
	{_T("OptionList_Clear"),             OptionList::Clear},
	{_T("OptionList_DeleteButton"),      OptionList::DeleteButton},
	{_T("OptionList_GetButtonCaption"),  OptionList::GetButtonCaption},
	{_T("OptionList_GetCount"),          OptionList::GetCount},
	{_T("OptionList_GetCurSel"),         OptionList::GetCurSel},
	{_T("OptionList_GetTopIndex"),       OptionList::GetTopIndex},
	{_T("OptionList_InsertButton"),      OptionList::InsertButton},
	{_T("OptionList_SetButtonEnabled"),  OptionList::SetButtonEnabled},
	{_T("OptionList_SetButtonTooltip"),  OptionList::SetButtonTooltip},
	{_T("OptionList_SetCurSel"),         OptionList::SetCurSel},
	{_T("OptionList_SetTopIndex"),       OptionList::SetTopIndex},

	// Option List control deprecated methods
	{_T("OptionList_AddString"),         OptionList::AddButton},
	{_T("OptionList_DeleteString"),      OptionList::DeleteButton},
	{_T("OptionList_GetText"),           OptionList::GetButtonCaption},
	{_T("OptionList_InsertString"),      OptionList::InsertButton},
	{_T("OptionList_SetEnabled"),        OptionList::SetButtonEnabled},
	{_T("OptionList_SetTttTitle"),       OptionList::SetButtonTooltip},

	// Picture Box control methods
	{_T("PictureBox_Clear"),             PictureBox::Clear},
	{_T("PictureBox_DrawArc"),           PictureBox::DrawArc},
	{_T("PictureBox_DrawCircle"),        PictureBox::DrawCircle},
	{_T("PictureBox_DrawEdge"),          PictureBox::DrawEdge},
	{_T("PictureBox_DrawFocusRect"),     PictureBox::DrawFocusRect},
	{_T("PictureBox_DrawHatchRect"),     PictureBox::DrawHatchRect},
	{_T("PictureBox_DrawLine"),          PictureBox::DrawLine},
	{_T("PictureBox_DrawPoint"),         PictureBox::DrawPoint},
	{_T("PictureBox_DrawRect"),          PictureBox::DrawRect},
	{_T("PictureBox_DrawSolidRect"),     PictureBox::DrawSolidRect},
	{_T("PictureBox_DrawText"),          PictureBox::DrawText},
	{_T("PictureBox_DrawWrappedText"),   PictureBox::DrawWrappedText},
	{_T("PictureBox_GetTextExtent"),     PictureBox::GetTextExtent},
	{_T("PictureBox_LoadPictureFile"),   PictureBox::LoadPictureFile},
	{_T("PictureBox_PaintPicture"),      PictureBox::PaintPicture},
	{_T("PictureBox_Refresh"),           PictureBox::Refresh},
	{_T("PictureBox_StoreImage"),        PictureBox::StoreImage},

	// Picture Box control deprecated methods
	{_T("PictureBox_DrawFillRect"),      PictureBox::DrawSolidRect},

	// Progress Bar control methods
	{_T("ProgressBar_Reset"),            ProgressBar::Reset},

	// Progress Bar control deprecated methods
	{_T("ProgressBar_SetPos"),           ProgressBar::SetPos},

	// Slide View control methods
	{_T("SlideView_Clear"),              SlideView::Clear},
	{_T("SlideView_EndImage"),           SlideView::EndImage},
	{_T("SlideView_FillImage"),          SlideView::FillImage},
	{_T("SlideView_Load"),               SlideView::Load},
	{_T("SlideView_RemoveHighlight"),    SlideView::RemoveHighlight},
	{_T("SlideView_SetHighlight"),       SlideView::SetHighlight},
	{_T("SlideView_SlideImage"),         SlideView::SlideImage},											
	{_T("SlideView_VectorImage"),        SlideView::VectorImage},

	// Tab Strip control methods
	{_T("TabStrip_GetControlArea"),      TabStrip::GetControlArea},
	{_T("TabStrip_GetCurSel"),           TabStrip::GetCurSel},
	{_T("TabStrip_GetRowCount"),         TabStrip::GetRowCount},
	{_T("TabStrip_HideTab"),             TabStrip::HideTab},
	{_T("TabStrip_SetCurSel"),           TabStrip::SetCurSel},
	{_T("TabStrip_SetTabCaption"),       TabStrip::SetTabCaption},
	{_T("TabStrip_ShowTab"),             TabStrip::ShowTab},

	// Tab Strip deprecated control methods
	{_T("Tab_GetControlArea"),           TabStrip::GetControlArea},
	{_T("Tab_GetCurSel"),                TabStrip::GetCurSel},
	{_T("Tab_GetRowCount"),              TabStrip::GetRowCount},
	{_T("Tab_HideTab"),                  TabStrip::HideTab},
	{_T("Tab_SetCurSel"),                TabStrip::SetCurSel},
	{_T("Tab_SetTabText"),               TabStrip::SetTabCaption},
	{_T("Tab_ShowTab"),                  TabStrip::ShowTab},

	// Text Box control methods
	{_T("TextBox_GetFilter"),            TextBox::GetFilter},
	{_T("TextBox_GetFirstVisibleLine"),  TextBox::GetFirstVisibleLine},
	{_T("TextBox_GetLine"),              TextBox::GetLine},
	{_T("TextBox_GetLineCount"),         TextBox::GetLineCount},
	{_T("TextBox_GetLineLength"),        TextBox::GetLineLength},
	{_T("TextBox_IsModified"),           TextBox::IsModified},
	{_T("TextBox_GetSel"),               TextBox::GetSel},
	{_T("TextBox_LineScroll"),           TextBox::LineScroll},
	{_T("TextBox_ReplaceSel"),           TextBox::ReplaceSel},
	{_T("TextBox_SetFilter"),            TextBox::SetFilter},
	{_T("TextBox_SetSel"),               TextBox::SetSel},
	{_T("TextBox_SetTabStops"),          TextBox::SetTabStops},
	{_T("TextBox_Undo"),                 TextBox::Undo},

	// Text Box control deprecated methods
	{_T("TextBox_GetModify"),            TextBox::IsModified},

	// Tree control methods
	{_T("Tree_AddChild"),                Tree::AddChild},
	{_T("Tree_AddParent"),               Tree::AddParent},
	{_T("Tree_AddSibling"),              Tree::AddSibling},
	{_T("Tree_CancelLabelEdit"),         Tree::CancelLabelEdit},
	{_T("Tree_Clear"),                   Tree::Clear},
	{_T("Tree_DeleteItem"),              Tree::DeleteItem},
	{_T("Tree_EnsureVisible"),           Tree::EnsureVisible},
	{_T("Tree_ExpandItem"),              Tree::ExpandItem},
	{_T("Tree_GetCount"),                Tree::GetCount},
	{_T("Tree_GetFirstChildItem"),       Tree::GetFirstChildItem},
	{_T("Tree_GetFirstVisibleItem"),     Tree::GetFirstVisibleItem},
	{_T("Tree_GetItemData"),             Tree::GetItemData},
	{_T("Tree_GetItemHandle"),           Tree::GetItemHandle},
	{_T("Tree_GetItemImages"),           Tree::GetItemImages},
	{_T("Tree_GetItemKey"),              Tree::GetItemKey},
	{_T("Tree_GetItemLabel"),            Tree::GetItemLabel},
	{_T("Tree_GetNextSiblingItem"),      Tree::GetNextSiblingItem},
	{_T("Tree_GetNextVisibleItem"),      Tree::GetNextVisibleItem},
	{_T("Tree_GetParentItem"),           Tree::GetParentItem},
	{_T("Tree_GetPrevSiblingItem"),      Tree::GetPrevSiblingItem},
	{_T("Tree_GetPrevVisibleItem"),      Tree::GetPrevVisibleItem},
	{_T("Tree_GetRootItem"),             Tree::GetRootItem},
	{_T("Tree_GetSelectedItem"),         Tree::GetSelectedItem},
	{_T("Tree_GetVisibleCount"),         Tree::GetVisibleCount},
	{_T("Tree_IsItemExpanded"),          Tree::IsItemExpanded},
	{_T("Tree_ItemHasChildren"),         Tree::ItemHasChildren},
	{_T("Tree_SelectItem"),              Tree::SelectItem},
	{_T("Tree_SelectSetFirstVisible"),   Tree::SelectSetFirstVisible},
	{_T("Tree_SetItemData"),             Tree::SetItemData},
	{_T("Tree_SetItemImages"),           Tree::SetItemImages},
	{_T("Tree_SetItemLabel"),            Tree::SetItemLabel},
	{_T("Tree_SortChildren"),            Tree::SortChildren},
	{_T("Tree_StartLabelEdit"),          Tree::StartLabelEdit},

	// Tree control deprecated methods
	{_T("Tree_CancelEditLabel"),         Tree::CancelLabelEdit},
	{_T("Tree_CountItems"),              Tree::GetCount},
	{_T("Tree_EditLabel"),               Tree::StartLabelEdit},
	{_T("Tree_GetExpandedImage"),        Tree::GetExpandedImage},
	{_T("Tree_GetItem"),                 Tree::GetItem},
	{_T("Tree_GetItemText"),             Tree::GetItemLabel},
	{_T("Tree_GetParent"),               Tree::GetParentItem},
	{_T("Tree_InsertAfter"),             Tree::AddSibling},
	{_T("Tree_SetExpandedImage"),        Tree::SetExpandedImage},
	{_T("Tree_SetItemText"),             Tree::SetItemLabel},
};


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


#ifdef _USE_ODCL_DROPTARGET
class COdclDropTarget : public COleDropTarget
{
public:
	COdclDropTarget() {}
	virtual ~COdclDropTarget() {}

// COleDropTarget Overrides
protected:
	virtual DROPEFFECT OnDragEnter( CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point )
		{
			if( GetOdclFilename( pDataObject ) )
				return DROPEFFECT_COPY;
			return DROPEFFECT_NONE;
		}
	virtual DROPEFFECT OnDragOver( CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point )
		{
			if( GetOdclFilename( pDataObject ) )
				return DROPEFFECT_COPY;
			return DROPEFFECT_NONE;
		}
	virtual void OnDragLeave( CWnd* pWnd ) {}
	virtual BOOL OnDrop( CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point )
		{
			LPCTSTR pszFilename = GetOdclFilename( pDataObject );
			if( !pszFilename )
				return FALSE;
			TArxProjectPtr pProject = theArxWorkspace.LoadProjectFile( pszFilename, NULL, true );
			return (pProject != NULL);
		}
	virtual DROPEFFECT OnDropEx( CWnd* pWnd, COleDataObject* pDataObject,
															 DROPEFFECT dropDefault, DROPEFFECT dropList, CPoint point)
		{ return DROPEFFECT_NONE; } //returning DROPEFFECT_NONE will cause OnDrop to be called instead

	static LPCTSTR GetOdclFilename( COleDataObject* pDataObject )
		{
			HGLOBAL hData = pDataObject->GetGlobalData( CF_HDROP );
			if( !hData )
				return NULL;
			static CString sFilename;
			sFilename.Empty();
			DROPFILES* pdf = (DROPFILES*)GlobalLock( hData );
			if( pdf )
			{
				if( pdf->fWide )
					sFilename = (LPCWSTR)((BYTE*)pdf + pdf->pFiles);
				else
					sFilename = (LPCSTR)((BYTE*)pdf + pdf->pFiles);
			}
			GlobalUnlock( hData );
			GlobalFree( hData );
			if( sFilename.IsEmpty() )
				return NULL;
			if( sFilename.Right( 5 ).CompareNoCase( _T(".odcl") ) == 0 )
				return sFilename;
			if( sFilename.Right( 9 ).CompareNoCase( _T(".odcl.lsp") ) == 0 )
				return sFilename;
			return NULL;
		}
};
#endif //_USE_ODCL_DROPTARGET


//-----------------------------------------------------------------------------
//----- ObjectARX EntryPoint
class CARXApp : public AcRxArxApp {

protected:
	T_PropertyIdSet mPropIds; //store the property ids used to define get/set lisp functions
#ifdef _USE_ODCL_DROPTARGET
	COdclDropTarget mOdclDropTarget; //for supporting drag/drop of .odcl file
#endif

	//dcl dialog state, probably should be stored in document state
	static CString msDialogToBeShown;
	static CPoint mptToBeShown;
	static TArxProjectPtr mpProjectToBeShown;
	static CString msActionToBeShown;
	static TDclFormPtr mpDclToBeShown;
	static int mnDoneDialogValue;
	static int mnListOperation; //1 = change selection, 2 = append item, 3 = replace all (default)

public:
	size_t GetPropertyIdCount() const { return mPropIds.size(); }
	CString GetCurrentFunctionName() const
		{
			int nFunctionCode = acedGetFunCode();
			if( nFunctionCode >= ADSFUNCBASE && nFunctionCode < (ADSFUNCBASE + _elements(grAdsFunctionTable)) )
				return (dclPrefix() + grAdsFunctionTable[nFunctionCode - ADSFUNCBASE].pszFunctionName);
			if( nFunctionCode >= ADSPROPFUNCBASE && nFunctionCode < ADSPROPFUNCBASE + Prop::_MaxId )
			{
				LPCTSTR pszPropName = GetPropertyApiName( (Prop::Id)(nFunctionCode - ADSPROPFUNCBASE) );
				if( pszPropName && *pszPropName != _T('(') )
					return (controlGetPrefix() + pszPropName);
			}
			else if( nFunctionCode >= (ADSPROPFUNCBASE + Prop::_MaxId) &&
							 nFunctionCode < (ADSPROPFUNCBASE + Prop::_MaxId + Prop::_MaxId) )
			{
				LPCTSTR pszPropName = GetPropertyApiName( (Prop::Id)(nFunctionCode - (ADSPROPFUNCBASE + Prop::_MaxId)) );
				if( pszPropName && *pszPropName != _T('(') )
					return (controlSetPrefix() + pszPropName);
			}
			else
			{
			#if (_BRXTARGET >= 11)
				if( nFunctionCode >= 0 && nFunctionCode < __adsRegisteredSymbols.length() )
					return __adsRegisteredSymbols[nFunctionCode]->getName();
			#elif (_BRXTARGET)
				if( nFunctionCode >= 0 && nFunctionCode < __adsRegisteredSymbols.length() )
					return __adsRegisteredSymbols[nFunctionCode].getName();
			#else
				_ADSSYMBOL_ENTRY** ppAdsSymbolMapEntryFirst = &__pAdsSymbolMapEntryFirst + 1;
				_ADSSYMBOL_ENTRY** ppAdsSymbolMapEntryLast = &__pAdsSymbolMapEntryLast;
				int paramIter = 0;
				for( _ADSSYMBOL_ENTRY** ppEntry = ppAdsSymbolMapEntryFirst;
						 ppEntry < ppAdsSymbolMapEntryLast;
						 ppEntry++ )
				{
					if( !*ppEntry )
						continue;
					if( paramIter++ == nFunctionCode )
						return (*ppEntry)->pszName;
				}
			#endif
			}
			return _T("");
		}

protected:
	static int ads_dcl_GetCtrlProperty(void);
	static int ads_dcl_SetCtrlProperty(void);
	static const CString& dclPrefix()
		{
			static const CString sPrefix = _T("dcl_");
			return sPrefix;
		}
	static const CString& controlGetPrefix()
		{
			static const CString sPrefix = dclPrefix() + _T("Control_Get");
			return sPrefix;
		}
	static const CString& controlSetPrefix()
		{
			static const CString sPrefix = dclPrefix() + _T("Control_Set");
			return sPrefix;
		}

public:
	CARXApp () : AcRxArxApp () {}

	virtual AcRx::AppRetCode On_kInitAppMsg (void *pkt) {
		
		if( !theWorkspace.GetLocalResourceModule() )
		{
			AfxMessageBox( _T("The OpenDCL local language resource module was not found or could not be loaded. OpenDCL cannot continue.\r\nPlease reinstall OpenDCL Runtime to correct the problem."), MB_OK | MB_ICONHAND );
			return AcRx::kRetError;
		}

		// You *must* call On_kInitAppMsg here
		AcRx::AppRetCode retCode =AcRxArxApp::On_kInitAppMsg (pkt) ;
		
		CString sModulePath;
		DWORD cchPath = GetModuleFileName( _hdllInstance, sModulePath.GetBuffer( MAX_PATH ), MAX_PATH );
		sModulePath.ReleaseBuffer( cchPath );
		acedRegisterExtendedTab( sModulePath, _T("OptionsDialog") );
	#ifdef _USE_ODCL_DROPTARGET
		acedAddDropTarget( &mOdclDropTarget );
	#endif

		GetApiPropertyIdSet( mPropIds );

		DWORD dwMajor;
		DWORD dwMinor;
		DWORD dwThird;
		DWORD dwFourth;
		if( theWorkspace.GetModuleVersionInfo( dwMajor, dwMinor, dwThird, dwFourth, _hdllInstance ) )
		{
			CString sVersion;
			sVersion.Format( _T("%d.%d.%d.%d"), dwMajor, dwMinor, dwThird, dwFourth );
			if( theWorkspace.IsAutoUpdateCheckEnabled() )
				UpdateCheck( gpszUCProductName, sVersion );
			acutPrintf( theWorkspace.LoadResourceString( IDS_BANNER ), (LPCTSTR)sVersion );
		}

		return (retCode) ;
	}

	virtual AcRx::AppRetCode On_kUnloadAppMsg (void *pkt) {
	#ifdef _USE_ODCL_DROPTARGET
		acedRemoveDropTarget( &mOdclDropTarget );
	#endif
		try
		{
			theArxWorkspace.UnloadAllProjects();
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
		acedRetNil();
		AcRx::AppRetCode retCode =AcRxArxApp::On_kInvkSubrMsg (pkt) ;

		assert( retCode == AcRx::kRetOK );
		if( retCode == AcRx::kRetOK )
		{
			int nFunctionCode = acedGetFunCode();
			if( nFunctionCode >= ADSFUNCBASE && nFunctionCode < ADSFUNCBASE + _elements(grAdsFunctionTable) )
			{
				if( RTNORM != grAdsFunctionTable[nFunctionCode - ADSFUNCBASE].pfHandler() )
					return AcRx::kRetError;
				return AcRx::kRetOK;
			}
			if( nFunctionCode >= ADSPROPFUNCBASE && nFunctionCode < ADSPROPFUNCBASE + Prop::_MaxId )
			{
				if( !GetCtrlProperty( static_cast<Prop::Id>(nFunctionCode - ADSPROPFUNCBASE) ) )
					return AcRx::kRetError;
				return AcRx::kRetOK;
			}
			int nSetPropertyBase = ADSPROPFUNCBASE + Prop::_MaxId;
			if( nFunctionCode >= nSetPropertyBase && nFunctionCode < nSetPropertyBase + Prop::_MaxId )
			{
				if( !SetCtrlProperty( static_cast<Prop::Id>(nFunctionCode - nSetPropertyBase) ) )
					return AcRx::kRetError;
				return AcRx::kRetOK;
			}
		}
		return (retCode) ;
	}

	virtual AcRx::AppRetCode On_kLoadDwgMsg (void *pkt) {
		AcRx::AppRetCode retCode =AcRxArxApp::On_kLoadDwgMsg (pkt) ;
		
	#if (_ACADTARGET >= 19)
		static bool bInitialized = false;
		if( !bInitialized )
		{
			bInitialized = true;
			ads_queueexpr( _ACRX_T("(opendcl_init_ui)") );
		}
	#endif

		if( retCode == AcRx::kRetOK )
		{
			//register control specific ADS functions
			for( int idxFunction = 0; idxFunction < _elements(grAdsFunctionTable); ++idxFunction )
			{
				const AdsFunctionTableEntry& Entry = grAdsFunctionTable[idxFunction];
				acedDefun( dclPrefix() + Entry.pszFunctionName, ADSFUNCBASE + idxFunction );
				acedRegFunc( Entry.pfHandler, ADSFUNCBASE + idxFunction );
			}

			//register general property get and set functions
			for( T_PropertyIdSet::const_iterator iter = mPropIds.begin(); iter != mPropIds.end(); ++iter )
			{
				Prop::Id id = *iter;
				LPCTSTR pszPropName = GetPropertyApiName( id );
				if( pszPropName && *pszPropName != _T('(') )
				{
					acedDefun( controlGetPrefix() + pszPropName, ADSPROPFUNCBASE + id );
					acedDefun( controlSetPrefix() + pszPropName, ADSPROPFUNCBASE + id + Prop::_MaxId );
					acedRegFunc( ads_dcl_GetCtrlProperty, ADSPROPFUNCBASE + id );
					acedRegFunc( ads_dcl_SetCtrlProperty, ADSPROPFUNCBASE + id + Prop::_MaxId );
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

		if( !AssertOutOfArgs( pArgs ) )
			return RSERR;

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

		if( !AssertOutOfArgs( pArgs ) )
			return RSERR;

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

		CString sToSend;
		if( !GetStringArgument( pArgs, sToSend ) )
			return RSERR; //invalid input

		if( !AssertOutOfArgs( pArgs ) )
			return RSERR;

		AcApDocument* pDoc = acDocManager->curDocument();
		if (pDoc)
		{
			// give the command line focus (otherwise the command doesn't get processed immediately)
			CWnd* wndCommandLine = acedGetAcadDockCmdLine();
			if( wndCommandLine )
				wndCommandLine->SetFocus();		
			Acad::ErrorStatus es = acDocManager->sendStringToExecute(pDoc, sToSend, false, true, false);
			if( es == Acad::eOk )
				acedRetT();
		}

		return (RSRSLT) ;
	}

	// ----- ads_dcl_invokefunc symbol (do not rename)
	static int ads_dcl_invokefunc(void)
	{
		return ads_dcl_invoke();
	}

	// ----- ads_dcl_invoke symbol (do not rename)
	static int ads_dcl_invoke(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		CString sTargetFunction;
		if( !GetStringArgument( pArgs, sTargetFunction ) )
			return RSERR; //invalid input

		if( !AssertOutOfArgs( pArgs ) )
			return RSERR;

		resbuf rbCallee = { NULL, RTSTR };
		rbCallee.resval.rstring = sTargetFunction.LockBuffer();
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

		switch (pArgs->restype)
		{
		case RTLONG:
			mpProjectToBeShown = TArxProjectLockedPtr( (CArxProject*)pArgs->resval.rlong );
			break;
		case RTENAME:
			mpProjectToBeShown = TArxProjectLockedPtr( (CArxProject*)pArgs->resval.rlname[0] );
			break;
		default:
			return RSERR; //wrong argument type
		};
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
		TDclControlPtr pProps = mpDclToBeShown->GetControlProperties();
		pProps->SetStringProperty(Prop::FormEventInitialize, msActionToBeShown);
		
		TCHAR lpPathBuffer[MAX_PATH];
		::GetTempPath(MAX_PATH, lpPathBuffer);
		CString sTempFile = lpPathBuffer;
		sTempFile += gpszDclEventsLspFileName; /*"DclEvents.lsp"*/
		::DeleteFile(sTempFile);

		// create and open the action events lsp file to be loaded.
		CStdioFile tempFile(sTempFile, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeWrite);
		tempFile.WriteString(_T("; This temp file is used for managing the events of dialog boxes using\n"));
		tempFile.WriteString(_T("; DCL equivalent functions in ObjectDCL.\n"));
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

		CDialogObject *pDialog = mpDclToBeShown->GetFormInstance();
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

		TDclControlPtr pCtrl = mpDclToBeShown->FindControl(pszListKey);
		assert (pCtrl != NULL);
		if( !pCtrl)
			return RSERR;

		if (pCtrl->GetWindow() == NULL)
		{ //the control has not been created yet
			TPropertyPtr pProp = pCtrl->GetPropertyObject(Prop::List);
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

		TDclControlPtr pCtrl = mpDclToBeShown->FindControl(pszListKey);
		assert (pCtrl != NULL);
		if( !pCtrl)
			return RSERR;

		if (pCtrl->GetWindow() == NULL)
		{ //the control has not been created yet
			TPropertyPtr pProp = pCtrl->GetPropertyObject(Prop::List);
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

		TDclControlPtr pCtrl = mpDclToBeShown->FindControl(pszKey);
		assert (pCtrl != NULL);
		if( !pCtrl)
			return RSERR;

		CString sAction;
		sAction.Format(gpszOnActionEventLispFunction, pszKey, pszAction);

		switch(pCtrl->GetType())
		{
		case CtlTextButton:
		case CtlCheckBox:
		case CtlGraphicButton:
		case CtlOptionButton:
		case CtlSlideView:
			{
				pCtrl->SetStringProperty(Prop::EventClicked, sAction);
				break;
			}
		case CtlTextBox:
			{
				pCtrl->SetStringProperty(Prop::EventEditChanged, sAction);
				break;
			}		
		case CtlGrid:
		case CtlListView:
			{
				pCtrl->SetStringProperty(Prop::EventClicked, sAction);
				break;
			}
		case CtlComboBox:
		case CtlListBox:
			{
				pCtrl->SetStringProperty(Prop::EventSelChanged, sAction);
				break;
			}	
		case CtlScrollBar:
			{
				pCtrl->SetStringProperty(Prop::EventScroll, sAction);
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

		TDclControlPtr pCtrl = mpDclToBeShown->FindControl(pszKey);
		assert (pCtrl != NULL);
		if( !pCtrl)
			return RSERR;

		pCtrl->SetStringProperty(Prop::Text, pszValue);
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

		TDclControlPtr pCtrl = mpDclToBeShown->FindControl(pszKey);
		assert (pCtrl != NULL);
		if( !pCtrl)
			return RSERR;

		if (pCtrl->GetWindow() == NULL)
		{ //the control has not been created yet
			switch (nValue)
			{
			case 0:
				{
					pCtrl->SetBooleanProperty(Prop::Enabled, true);
					break;
				}		
			case 1:
				{
					pCtrl->SetBooleanProperty(Prop::Enabled, false);
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
					pCtrl->SetBooleanProperty(Prop::Enabled, true);
					pCtrl->GetWindow()->EnableWindow(TRUE);
					break;
				}		
			case 1:
				{
					pCtrl->SetBooleanProperty(Prop::Enabled, false);
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
						CArxAcadSlideCtrl *pSlide = (CArxAcadSlideCtrl*)pCtrl->GetWindow();
						if( CAcadColorService::IsTransparentColor( pSlide->GetHighlight() ) )
							pSlide->SetHighlight( RGB(255,0,0) );
						else
							pSlide->RemoveHighlight();
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

		TDclControlPtr pCtrl = mpDclToBeShown->FindControl(pszKey);
		assert (pCtrl != NULL);
		if( !pCtrl)
			return RSERR;

		if (pCtrl->GetWindow() == NULL)
		{ //the control has not been created yet
			acedRetStr(pCtrl->GetStringProperty(Prop::Text));
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

		theWorkspace.DisplayAlert(_T("(dcl_Get_Attr) is not implemented yet!"));

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

		TDclControlPtr pCtrl = mpDclToBeShown->FindControl(pszKey);
		assert (pCtrl != NULL);
		if( !pCtrl)
			return RSERR;

		((CArxAcadSlideCtrl*)pCtrl->GetWindow())->Clear();

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

		TDclControlPtr pCtrl = mpDclToBeShown->FindControl(pszKey);
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

		((CArxAcadSlideCtrl*)pCtrl->GetWindow())->DrawLine(nStartX, nStartY, nEndX, nEndY, nLineColor);

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

		TDclControlPtr pCtrl = mpDclToBeShown->FindControl(pszKey);
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

		((CArxAcadSlideCtrl*)pCtrl->GetWindow())->DrawFillRect(nStartX, nStartY, nStartX + nEndX, nStartY + nEndY, nLineColor);

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

		TDclControlPtr pCtrl = mpDclToBeShown->FindControl(pszKey);
		assert (pCtrl != NULL);
		if( !pCtrl)
			return RSERR;

		CRect rc;
		((CArxAcadSlideCtrl*)pCtrl->GetWindow())->GetClientRect(&rc);
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

		TDclControlPtr pCtrl = mpDclToBeShown->FindControl(pszKey);
		assert (pCtrl != NULL);
		if( !pCtrl)
			return RSERR;

		CRect rc;
		((CArxAcadSlideCtrl*)pCtrl->GetWindow())->GetClientRect(&rc);
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

		TDclControlPtr pCtrl = mpDclToBeShown->FindControl(pszKey);
		assert (pCtrl != NULL);
		if( !pCtrl)
			return RSERR;

		((CArxAcadSlideCtrl*)pCtrl->GetWindow())->Snapshot();

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

		TDclControlPtr pCtrl = mpDclToBeShown->FindControl(pszKey);
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
				acedRetInt(-1);
				return RSRSLT;
			}
			((CArxAcadSlideCtrl*)pCtrl->GetWindow())->DrawASlide(nX, nY, nWidth, nHeight, sPath, NULL);
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
				acedRetInt(-1);
				return RSRSLT;
			}
			((CArxAcadSlideCtrl*)pCtrl->GetWindow())->DrawASlide(nX, nY, nWidth, nHeight, sPath, sLibName);
		}
		acedRetT();

		return (RSRSLT) ;
	}

	// ----- ads_dcl_getlinetype symbol (do not rename)
	static int ads_dcl_getlinetype(void)
	{
		return ads_dcl_selectlinetype();
	}

	// ----- ads_dcl_selectlinetype symbol (do not rename)
	static int ads_dcl_selectlinetype(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		bool bIncludeMetaTypes = true;
		GetBoolArgument( pArgs, bIncludeMetaTypes, true );

		if( !AssertOutOfArgs( pArgs ) )
			return RSERR;

		AcDbObjectId id;
		CString sLinetype;
		if( SelectLinetypeUI( id, sLinetype, bIncludeMetaTypes ) )
			acedRetStr( sLinetype );

		return (RSRSLT) ;
	}

	// ----- ads_dcl_getlineweight symbol (do not rename)
	static int ads_dcl_getlineweight(void)
	{
		return ads_dcl_selectlineweight();
	}

	// ----- ads_dcl_selectlineweight symbol (do not rename)
	static int ads_dcl_selectlineweight(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		long newLW = AcDb::kLnWtByLwDefault;
		GetLongArgument( pArgs, newLW, true );

		bool bIncludeMetaTypes = true;
		GetBoolArgument( pArgs, bIncludeMetaTypes, true );

		if( !AssertOutOfArgs( pArgs ) )
			return RSERR;

		if( SelectLineWeightUI( (AcDb::LineWeight&)newLW, bIncludeMetaTypes ) )
			theArxWorkspace.RetLong( newLW );

		return (RSRSLT) ;
	}

	// ----- ads_dcl_getscreensize symbol (do not rename)
	static int ads_dcl_getscreensize(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		if( !AssertOutOfArgs( pArgs ) )
			return RSERR;

		CRect rcWorkArea;
		if( SystemParametersInfo( SPI_GETWORKAREA, 0, &rcWorkArea, 0 ) > 0 )
		{
			resbuf rbHeight = {NULL, RTSHORT};
			rbHeight.resval.rint = rcWorkArea.Height();
			resbuf rbWidth = {&rbHeight, RTSHORT};
			rbWidth.resval.rint = rcWorkArea.Width();
			acedRetList(&rbWidth);
		}

		return (RSRSLT) ;
	}

	// ----- ads_dcl_messagebox symbol (do not rename)
	static int ads_dcl_messagebox(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		CString sMessage;
		if( !GetStringArgument( pArgs, sMessage ) )
			return RSERR; //arguments expected

		CString sTitle = theWorkspace.GetAppKey();
		GetStringArgument( pArgs, sTitle, true );

		DWORD dwMsgBoxType = 0;
		//optional arguments
		int fButtonStyle = 0;
		int fIconStyle = 0;
		bool bShowHelpButton = false;
		if( GetIntArgument( pArgs, fButtonStyle, true ) )
		{
			//convert the arguments into windows messagebox type flags
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
			default: HandleArgValueError( pArgs ); return RSERR; //invalid argument value
			}
			if( GetIntArgument( pArgs, fIconStyle, true ) )
			{
				switch (fIconStyle)
				{
				case 0: break;
				case 1: dwMsgBoxType |= MB_ICONEXCLAMATION; break;
				case 2: dwMsgBoxType |= MB_ICONINFORMATION; break;
				case 3: dwMsgBoxType |= MB_ICONQUESTION; break;
				case 4: dwMsgBoxType |= MB_ICONSTOP; break;
				default: HandleArgValueError( pArgs ); return RSERR; //invalid argument value
				}
				GetBoolArgument( pArgs, bShowHelpButton, true );
			}
		}

		if( !AssertOutOfArgs( pArgs ) )
			return RSERR;

		if( bShowHelpButton )
			dwMsgBoxType |= MB_HELP;

		acedRetInt( MessageBox( theArxWorkspace.GetTopmostModalForm(), sMessage, sTitle, dwMsgBoxType ) );
		
		return (RSRSLT) ;
	}

	// ----- ads_dcl_getharddrivesize symbol (do not rename)
	static int ads_dcl_getharddrivesize(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		CString sDrive;
		if( !GetStringArgument( pArgs, sDrive ) )
			return RSERR; //invalid argument

		if( !AssertOutOfArgs( pArgs ) )
			return RSERR;

		if( sDrive.IsEmpty() )
			return RSERR; //invalid argument

		DWORDLONG nFree;
		if( GetDiscSpace( sDrive, &nFree ) )
			acedRetReal( (double)nFree );

		return (RSRSLT) ;
	}

	// ----- ads_dcl_getfocus symbol (do not rename)
	static int ads_dcl_getfocus(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		if( !AssertOutOfArgs( pArgs ) )
			return RSERR;

		HWND hwndFocus = GetFocus();
		if (hwndFocus)
		{
			CString sControlName;
			const TDclFormPtr pDclObject = theArxWorkspace.FindDclFormControl (hwndFocus, sControlName);
			if (pDclObject)
			{
				resbuf rbControlName = {NULL, RTSTR};
				rbControlName.resval.rstring = sControlName.LockBuffer();
				resbuf rbDialogName = {sControlName.IsEmpty()? NULL : &rbControlName, RTSTR};
				CString sFormName = pDclObject->GetKeyName();
				rbDialogName.resval.rstring = sFormName.LockBuffer();
				resbuf rbProjectName = {&rbDialogName, RTSTR};
				CString sProjectName = pDclObject->GetProject()->GetKeyName();
				rbProjectName.resval.rstring = sProjectName.LockBuffer();
				acedRetList(&rbProjectName);
			}
		}

		return (RSRSLT) ;
	}

	// ----- ads_dcl_hideerrormsgbox symbol (do not rename)
	static int ads_dcl_hideerrormsgbox(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		if( !AssertOutOfArgs( pArgs ) )
			return RSERR;

		theWorkspace.SetMessagesSuppressed();
		acedRetT();

		return (RSRSLT) ;
	}

	// ----- ads_dcl_showerrormsgbox symbol (do not rename)
	static int ads_dcl_showerrormsgbox(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		if( !AssertOutOfArgs( pArgs ) )
			return RSERR;

		theWorkspace.SetMessagesSuppressed(false);
		acedRetT();

		return (RSRSLT) ;
	}

	// ----- ads_dcl_suppressmessages symbol (do not rename)
	static int ads_dcl_suppressmessages(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		bool bSuppress = true;
		GetBoolArgument( pArgs, bSuppress, true );

		if( !AssertOutOfArgs( pArgs ) )
			return RSERR;

		theWorkspace.SetMessagesSuppressed( bSuppress );
		acedRetT();

		return (RSRSLT) ;
	}

	// ----- ads_dcl_getblocksize symbol (do not rename)
	static int ads_dcl_getblocksize(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		CString sBlockName;
		if( !GetStringArgument( pArgs, sBlockName ) )
			return RSERR; //invalid argument

		if( !AssertOutOfArgs( pArgs ) )
			return RSERR;

		if( sBlockName.IsEmpty() )
			return RSERR; //invalid argument

		AcDbDatabase* pDb = acdbCurDwg();
		assert(pDb != NULL);
		if (!pDb)
			return RSERR;
		AcDbBlockTable* pBlockTable;
		Acad::ErrorStatus es = pDb->getBlockTable (pBlockTable, AcDb::kForRead);
		if (es != Acad::eOk)
			return RSRSLT;
		AcDbBlockTableRecord* pBTR;
		es = pBlockTable->getAt (sBlockName, pBTR, AcDb::kForRead);
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
		rbHeight.resval.rreal = size.y;
		resbuf rbWidth = {&rbHeight, RTREAL};
		rbWidth.resval.rreal = size.x;
		acedRetList (&rbWidth);

		return (RSRSLT) ;
	}

	// ----- ads_dcl_navigatetourl symbol (do not rename)
	static int ads_dcl_navigatetourl(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		CString sURL;
		if( !GetStringArgument( pArgs, sURL ) )
			return RSERR; //invalid argument

		if( !AssertOutOfArgs( pArgs ) )
			return RSERR;

		if( sURL.IsEmpty() )
			return RSERR; //invalid argument

		ShellExecute(0, _T("open"), sURL, 0, 0, SW_SHOWNORMAL);
		acedRetT();

		return (RSRSLT) ;
	}

	// ----- ads_dcl_activateemail symbol (do not rename)
	static int ads_dcl_activateemail(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		CString sEmailAddress;
		if( !GetStringArgument( pArgs, sEmailAddress ) )
			return RSERR; //invalid argument

		//optional arguments
		CString sParams;
		GetStringArgument( pArgs, sParams, true );

		if( !AssertOutOfArgs( pArgs ) )
			return RSERR;

		if( sEmailAddress.IsEmpty() )
			return RSERR; //invalid argument

		CString sTarget = _T("mailto:");
		sTarget += sEmailAddress;
		if (!sParams.IsEmpty())
		{
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
		acedRetT();

		return (RSRSLT) ;
	}

	// ----- ads_dcl_browsefolder symbol (do not rename)
	static int ads_dcl_browsefolder(void)
	{
		return ads_dcl_selectfolder();
	}

	// ----- ads_dcl_selectfolder symbol (do not rename)
	static int ads_dcl_selectfolder(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		CString sCaption;
		if( !GetStringArgument( pArgs, sCaption ) )
			return RSERR; //invalid argument

		//optional arguments
		CString sInitialFolder;
		CString sRootFolder;
		UINT nFlags = BIF_RETURNONLYFSDIRS;
		GetStringArgument( pArgs, sInitialFolder, true );
		GetStringArgument( pArgs, sRootFolder, true );
		GetUIntArgument( pArgs, nFlags, true );

		if( !AssertOutOfArgs( pArgs ) )
			return RSERR;

		if( sCaption.IsEmpty() )
			return RSERR; //invalid argument

		CFolderBrowseDlg dlg( sCaption, sInitialFolder, sRootFolder, nFlags );
		if (dlg.DoBrowse(CWnd::FromHandle(theArxWorkspace.GetTopmostModalForm())))
			acedRetStr(dlg.GetSelectedFolder());

		return (RSRSLT) ;
	}

	// ----- ads_dcl_insert symbol (do not rename)
	static int ads_dcl_insert(void)
	{
		return ads_dcl_insertblock();
	}

	// ----- ads_dcl_insertblock symbol (do not rename)
	static int ads_dcl_insertblock(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		CString sPath;
		if( !GetStringArgument( pArgs, sPath ) )
			return RSERR; //invalid argument

		//optional arguments
		AcGePoint3d ptBase;
		double dblRotation = 0.0;
		CString sBlockName;
		if( Get3dPointArgument( pArgs, ptBase, true ) )
		{
			if( GetDoubleArgument( pArgs, dblRotation, true ) )
			{
				if( !GetStringArgument( pArgs, sBlockName, true ) )
					GetNilArgument( pArgs, true );
			}
		}

		if( !AssertOutOfArgs( pArgs ) )
			return RSERR;

		if( sPath.IsEmpty() )
			return RSERR; //invalid argument

		if( sBlockName.IsEmpty() )
		{
			//use the filename without extension
			sBlockName = sPath;
			sBlockName.MakeReverse();
			sBlockName = sBlockName.SpanExcluding(_T("\\/:"));
			sBlockName.MakeReverse();
			sBlockName = sBlockName.SpanExcluding(_T("."));
		}

		//----- Read the external DWG file
		AcDbObjectId blockId;
		AcDbDatabase *pDwg = new AcDbDatabase(false);
		Acad::ErrorStatus es = pDwg->readDwgFile (sPath, _SH_DENYNO);
		if (es == Acad::eOk)
		{
			//----- Put it into a block table record of the current database
			es = acdbCurDwg()->insert (blockId, sBlockName, pDwg, Adesk::kFalse) ;
		}
		delete pDwg;
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
		//----- Step 3: Open current database's active space blockTableRecord
		AcDbBlockTableRecord *pBlockTableRecord ;
		es = acdbOpenObject( pBlockTableRecord, acdbCurDwg()->currentSpaceId(), AcDb::kForWrite) ;
		if (es != Acad::eOk)
		{
			delete pBlkRef;
			return RSRSLT;
		}
		//----- Append the block reference to the block table record
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
		{
			pBlkRef->close () ;
			return RSRSLT;
		}
		AcDbBlockTableRecordIterator *pIterator ;
		es = pBlockDef->newIterator (pIterator) ;
		pBlockDef->close () ;
		if (es != Acad::eOk)
		{
			pBlkRef->close () ;
			return RSRSLT;
		}
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
		pBlkRef->close () ;

		if (es == Acad::eOk)
			acedRetT();

		return (RSRSLT) ;
	}

	// ----- ads_dcl_loadxref symbol (do not rename)
	static int ads_dcl_loadxref(void)
	{
		return ads_dcl_attachxref();
	}

	// ----- ads_dcl_attachxref symbol (do not rename)
	static int ads_dcl_attachxref(void)
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

		CString sPath;
		if( !GetStringArgument( pArgs, sPath ) )
			return RSERR; //invalid argument

		//optional arguments
		CString sBlockName;
		AcGePoint3d ptBase;
		double dblRotation = 0.0;
		if( GetStringArgument( pArgs, sBlockName, true ) )
		{
			if( Get3dPointArgument( pArgs, ptBase, true ) )
				GetDoubleArgument( pArgs, dblRotation, true );
		}
		else
			GetNilArgument( pArgs, true );

		if( !AssertOutOfArgs( pArgs ) )
			return RSERR;

		if( sPath.IsEmpty() )
			return RSERR; //invalid argument

		if( sBlockName.IsEmpty() )
		{
			//use the filename without extension
			sBlockName = sPath;
			sBlockName.MakeReverse();
			sBlockName = sBlockName.SpanExcluding(_T("\\/:"));
			sBlockName.MakeReverse();
			sBlockName = sBlockName.SpanExcluding(_T("."));
		}

		acedXrefAttach(sPath, sBlockName, NULL, NULL, &ptBase, &AcGeScale3d(1, 1, 1), &dblRotation, Adesk::kFalse);

		return (RSRSLT) ;
	}

	// ----- ads_dcl_updatevarnames symbol (do not rename)
	static int ads_dcl_updatevarnames(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		if( !AssertOutOfArgs( pArgs ) )
			return RSERR;

		if (theArxWorkspace.UpdateGlobalLispSymbols())
			acedRetT();

		return (RSRSLT) ;
	}

	// ----- ads_dcl_forcedwgredraw symbol (do not rename)
	static int ads_dcl_forcedwgredraw(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		if( !AssertOutOfArgs( pArgs ) )
			return RSERR;

		CView* pDwgView = acedGetAcadDwgView();
		assert(pDwgView != NULL);
		pDwgView->RedrawWindow();
		acedRetT();

		return (RSRSLT) ;
	}

	// ----- ads_dcl_getmousecoords symbol (do not rename)
	static int ads_dcl_getmousecoords(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		if( !AssertOutOfArgs( pArgs ) )
			return RSERR;

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

		if( !AssertOutOfArgs( pArgs ) )
			return RSERR;

		CPoint ptMouse;
		GetCursorPos(&ptMouse);
		ScreenToClient( adsw_acadDocWnd(), &ptMouse );

		acedDwgPoint cpt;
		acedCoordFromPixelToWorld(ptMouse, cpt);

/*
		AcGePoint3d ptAcad;
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
*/

		resbuf rbPoint = {NULL, RT3DPOINT};
		rbPoint.resval.rpoint[X] = cpt[X];
		rbPoint.resval.rpoint[Y] = cpt[Y];
		rbPoint.resval.rpoint[Z] = cpt[Z];
		acedRetVal(&rbPoint);

		return (RSRSLT) ;
	}

	// ----- ads_dcl_multifiledialog symbol (do not rename)
	static int ads_dcl_multifiledialog(void)
	{
		return ads_dcl_selectfiles();
	}

	// ----- ads_dcl_selectfiles symbol (do not rename)
	static int ads_dcl_selectfiles(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		//optional arguments
		CString sFilterList;
		CString sCaption;
		CString sPath;
		CString sFilter;
		if( GetStringArgument( pArgs, sFilter, true ) )
		{
			if( !sFilter.IsEmpty() )
			{
				if( sFilter.Right( 1 ) != _T("|") )
					sFilter += _T('|');
				int nFirstSeparator = sFilter.Find( _T('|') );
				if( nFirstSeparator == sFilter.GetLength() - 1 )
					sFilter += sFilter; //only one element was provided, so just use the same text for both
				else if( sFilter.Find( _T('|'), nFirstSeparator + 1 ) != sFilter.GetLength() - 1 )
					sFilter = sFilter.Left( sFilter.Find( _T('|'), nFirstSeparator + 1 ) + 1 ); //too many elements, so chop off the extras
			}
			sFilterList += sFilter;
			if( GetStringArgument( pArgs, sCaption, true ) )
				GetStringArgument( pArgs, sPath, true );
		}
		else
		{
			PropVal::TCStringArray rsFilters;
			if( GetStringArrayArgument( pArgs, rsFilters, true ) )
			{
				for( size_t idx = 0; idx < rsFilters.size(); ++idx )
				{
					CString sFilter = rsFilters[idx];
					if( !sFilter.IsEmpty() )
					{
						if( sFilter.Right( 1 ) != _T("|") )
							sFilter += _T('|');
						int nFirstSeparator = sFilter.Find( _T('|') );
						if( nFirstSeparator == sFilter.GetLength() - 1 )
							sFilter += sFilter; //only one element was provided, so just use the same text for both
						else if( sFilter.Find( _T('|'), nFirstSeparator + 1 ) != sFilter.GetLength() - 1 )
							sFilter = sFilter.Left( sFilter.Find( _T('|'), nFirstSeparator + 1 ) + 1 ); //too many elements, so chop off the extras
					}
					sFilterList += sFilter;
				}
				if( GetStringArgument( pArgs, sCaption, true ) )
					GetStringArgument( pArgs, sPath, true );
			}
			else
				sFilterList = _T("All Files (*.*)|*.*|");
		}

		if( !AssertOutOfArgs( pArgs ) )
			return RSERR;

		CWnd *pParent = CWnd::FromHandle(theArxWorkspace.GetTopmostModalForm());
		DWORD dwFlags = OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING |
										OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
		sPath.Replace( _T('/'), _T('\\') );
		sPath.TrimRight( _T('\\') );
		// create the open dialog box
		CFileDialog BrowseWnd( TRUE, NULL, sPath, dwFlags, sFilterList, pParent );
		BrowseWnd.m_ofn.lpstrTitle = sCaption.LockBuffer();
		CString sResults = BrowseWnd.m_ofn.lpstrFile;
		if( BrowseWnd.m_ofn.lStructSize > OPENFILENAME_SIZE_VERSION_400/*0x98*/ )
		{
			DWORD dwAttrib = GetFileAttributes( sPath );
			if( dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY) != 0 )
			{
				sPath += _T('\\');
				BrowseWnd.m_ofn.lpstrInitialDir = sPath;
				sResults.Empty();
			}
		}
		BrowseWnd.m_ofn.nMaxFile = 8192;
		BrowseWnd.m_ofn.lpstrFile = sResults.GetBuffer(8192);
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
		return ads_dcl_project_getpicturesize();
	}

	// ----- ads_dcl_project_getpicturesize symbol (do not rename)
	static int ads_dcl_project_getpicturesize(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		TArxProjectPtr pProject;
		if( !GetProjectArgument( pArgs, pProject ) )
			return RSERR; //invalid argument

		long id = -1;
		if( !GetLongArgument( pArgs, id ) )
			return RSERR; //invalid argument

		if( !AssertOutOfArgs( pArgs ) )
			return RSERR;

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

		if( !AssertOutOfArgs( pArgs ) )
			return RSERR;

		CWnd* CmdBarWnd = acedGetAcadDockCmdLine();
		if (CmdBarWnd)
		{
			CmdBarWnd->SetFocus();
			acedRetT();
		}

		return (RSRSLT) ;
	}

	// ----- ads_dcl_delayedinvoke symbol (do not rename)
	static int ads_dcl_delayedinvoke(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		UINT cMsDelay = 0;
		if( !GetUIntArgument( pArgs, cMsDelay ) )
			return RSERR; //invalid argument

		CString sCallbackName;
		if( !GetStringArgument( pArgs, sCallbackName ) )
			return RSERR; //invalid argument

		if( sCallbackName.IsEmpty() )
			return HandleArgValueError( pArgs ); //invalid value

		if( !AssertOutOfArgs( pArgs ) )
			return RSERR;

		new CDelayedInvoke( sCallbackName, cMsDelay );

		return (RSRSLT) ;
	}

	// ----- ads_dcl_xtwipstopixels symbol (do not rename)
	static int ads_dcl_xtwipstopixels(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		long twips = 0;
		if( !GetLongArgument( pArgs, twips ) )
			return RSERR; //invalid argument

		if( !AssertOutOfArgs( pArgs ) )
			return RSERR;

		HDC hdc = ::GetDC( NULL );
		int nX = GetDeviceCaps( hdc, LOGPIXELSX );
		::ReleaseDC( NULL, hdc );
		theArxWorkspace.RetLong( MulDiv( nX, twips, 1440 ) );

		return (RSRSLT) ;
	}

	// ----- ads_dcl_ytwipstopixels symbol (do not rename)
	static int ads_dcl_ytwipstopixels(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		long twips = 0;
		if( !GetLongArgument( pArgs, twips ) )
			return RSERR; //invalid argument

		if( !AssertOutOfArgs( pArgs ) )
			return RSERR;

		HDC hdc = ::GetDC( NULL );
		int nY = GetDeviceCaps( hdc, LOGPIXELSY );
		::ReleaseDC( NULL, hdc );
		theArxWorkspace.RetLong( MulDiv( nY, twips, 1440 ) );

		return (RSRSLT) ;
	}

	// ----- ads_dcl_xpixelstotwips symbol (do not rename)
	static int ads_dcl_xpixelstotwips(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		long pixels = 0;
		if( !GetLongArgument( pArgs, pixels ) )
			return RSERR; //invalid argument

		if( !AssertOutOfArgs( pArgs ) )
			return RSERR;

		HDC hdc = ::GetDC( NULL );
		int nX = GetDeviceCaps( hdc, LOGPIXELSX );
		::ReleaseDC( NULL, hdc );
		theArxWorkspace.RetLong( MulDiv( pixels, 1440, nX ) );

		return (RSRSLT) ;
	}

	// ----- ads_dcl_ypixelstotwips symbol (do not rename)
	static int ads_dcl_ypixelstotwips(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		long pixels = 0;
		if( !GetLongArgument( pArgs, pixels ) )
			return RSERR; //invalid argument

		if( !AssertOutOfArgs( pArgs ) )
			return RSERR;

		HDC hdc = ::GetDC( NULL );
		int nY = GetDeviceCaps( hdc, LOGPIXELSY );
		::ReleaseDC( NULL, hdc );
		theArxWorkspace.RetLong( MulDiv( pixels, 1440, nY ) );

		return (RSRSLT) ;
	}

	// ----- ads_dcl_getolecolorvalue symbol (do not rename)
	static int ads_dcl_getolecolorvalue(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		COLORREF crArg;
		if( !GetColorArgument( pArgs, crArg ) )
			return RSERR; //invalid input

		if( !AssertOutOfArgs( pArgs ) )
			return RSERR;

		theArxWorkspace.RetLong( crArg );

		return (RSRSLT) ;
	}

	// ----- ads_dcl_registeractivexctrl symbol (do not rename)
	static int ads_dcl_registeractivexctrl(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		CString sPath;
		if( !GetStringArgument( pArgs, sPath ) )
			return RSERR; //invalid input

		if( !AssertOutOfArgs( pArgs ) )
			return RSERR;

		HMODULE hAxServer = LoadLibrary (sPath);
		if (!hAxServer)
			hAxServer = LoadLibrary (theWorkspace.FindFile(sPath));
		if (!hAxServer)
			return RSRSLT;

		typedef HRESULT (STDAPICALLTYPE* PDLLREGISTERSERVER)( void );
		PDLLREGISTERSERVER pfRegister = (PDLLREGISTERSERVER)GetProcAddress (hAxServer, "DllRegisterServer");
		if (pfRegister && SUCCEEDED(pfRegister()))
			acedRetT();

		FreeLibrary (hAxServer);

		return (RSRSLT) ;
	}

	// ----- ads_dcl_dirfiles symbol (do not rename)
	static int ads_dcl_dirfiles(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		CString sPath;
		if( !GetStringArgument( pArgs, sPath ) )
			return RSERR; //invalid input

		CString sFilter = _T("*.*");
		GetStringArgument( pArgs, sFilter, true );

		if( !AssertOutOfArgs( pArgs ) )
			return RSERR;

		if( !sPath.IsEmpty() && sPath.Right( 1 ) != _T("\\") )
			sPath += _T('\\');

		struct resbuf* prbFiles = NULL;
		struct resbuf* prbTail = NULL;
		CFileFind finder;
		BOOL bResult = finder.FindFile( sPath + sFilter );
		while( bResult )
		{	
			bResult = finder.FindNextFile();
			if( finder.IsDots() )
				continue;
			
			if( !prbTail )
			{
				prbFiles = acutNewRb( RTSTR );
				prbTail = prbFiles;
			}
			else
			{
				prbTail->rbnext = acutNewRb( RTSTR );
				prbTail = prbTail->rbnext;
			}
			acutNewString( finder.GetFileName(), prbTail->resval.rstring );
		}
		finder.Close();
		acedRetList( prbFiles );
		acutRelRb( prbFiles );

		return (RSRSLT) ;
	}

	// ----- ads_dcl_project_load symbol (do not rename)
	static int ads_dcl_project_load(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		CString sPath;
		if( !GetStringArgument( pArgs, sPath ) )
			return RSERR; //invalid argument

		//optional arguments
		bool bReload = false;
		CString sKeyName;
		if( GetBoolArgument( pArgs, bReload, true ) )
		{
			if( !GetStringArgument( pArgs, sKeyName, true ) )
				GetNilArgument( pArgs, true );
		}

		if( !AssertOutOfArgs( pArgs ) )
			return RSERR;

		if( sPath.IsEmpty() )
			return RSERR; //invalid argument

		TArxProjectPtr pProject = theArxWorkspace.LoadProjectFile( sPath, sKeyName, bReload );
		if( !pProject )
		{
			CString sFmt = theWorkspace.LoadResourceString( IDS_PROJECTLOADFAILURE );
			if( !sFmt.IsEmpty() )
			{
				CString sAlertMsg;
				sAlertMsg.Format( sFmt, (LPCTSTR)sPath );
				theWorkspace.DisplayAlert( sAlertMsg );
			}
			return RSRSLT; 
		}
		acedRetStr( pProject->GetKeyName() );

		return (RSRSLT) ;
	}

	// ----- ads_dcl_project_unload symbol (do not rename)
	static int ads_dcl_project_unload(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		TArxProjectPtr pProject;
		if( !GetProjectArgument( pArgs, pProject ) )
			return RSERR; //wrong argument type

		//optional arguments
		bool bForce = false;
		GetBoolArgument( pArgs, bForce, true );

		if( !AssertOutOfArgs( pArgs ) )
			return RSERR;

		if( !pProject )
			return RSERR; //too many arguments

		if( theArxWorkspace.UnloadProject( pProject, bForce ) )
			acedRetT();

		return (RSRSLT) ;
	}

	// ----- ads_dcl_project_saveas symbol (do not rename)
	static int ads_dcl_project_saveas(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		TArxProjectPtr pProject;
		if( !GetProjectArgument( pArgs, pProject ) )
			return RSERR; //wrong argument type

		CString sFilename;
		if( !GetStringArgument( pArgs, sFilename ) )
			return RSERR; //wrong argument type

		//optional arguments
		CString sPassword;
		GetStringArgument( pArgs, sPassword, true );

		if( !AssertOutOfArgs( pArgs ) )
			return RSERR;

		if( !pProject )
			return RSERR; //too many arguments

		if( pProject->GetPassword() != sPassword )
			return RSRSLT; //wrong password

		if (pProject->WriteToFile( sFilename ) == statOK)
			acedRetT();

		return (RSRSLT) ;
	}

	// ----- ads_dcl_project_import symbol (do not rename)
	static int ads_dcl_project_import(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		CString sRawDataIn;
		PropVal::TCStringArray rsRawData;
		if( pArgs->restype == RTLB )
		{ //we expect a list of strings
			if( !GetStringArrayArgument( pArgs, rsRawData ) )
				return RSERR; //wrong argument type
			for( size_t idx = 0; idx < rsRawData.size(); ++idx )
				sRawDataIn += rsRawData[idx];
		}
		else
		{ //otherwise it must be a single string
			if( !GetStringArgument( pArgs, sRawDataIn ) )
				return RSERR; //wrong argument type
		}

		//optional arguments
		CString sPassword;
		CString sProjectKey;
		if( GetStringArgument( pArgs, sPassword, true ) )
			GetStringArgument( pArgs, sProjectKey, true );

		if( !AssertOutOfArgs( pArgs ) )
			return RSERR;

		TArxProjectPtr pProject;
		std::string sRawData;
	#ifdef _UNICODE
		CStringA sRawDataA( sRawDataIn );
		sRawData = (LPCSTR)sRawDataA;
	#else
		sRawData = sRawDataIn;
	#endif
		try
		{
			std::string sDecodedData = base64_decode( sRawData );
			CMemFile Data( (BYTE*)sDecodedData.c_str(), sDecodedData.length() );
			pProject = theArxWorkspace.ImportProject( Data, sProjectKey );
			if (pProject)
			{
				if( !sPassword.IsEmpty() )
					pProject->SetPassword( sPassword );
				acedRetStr( pProject->GetKeyName() );
			}
		}
		catch( ... )
		{
			return RSERR; 
		}

		return (RSRSLT) ;
	}

	// ----- ads_dcl_project_export symbol (do not rename)
	static int ads_dcl_project_export(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		TArxProjectPtr pProject;
		if( !GetProjectArgument( pArgs, pProject ) )
			return RSERR; //wrong argument type

		//optional arguments
		CString sPassword;
		GetStringArgument( pArgs, sPassword, true );

		if( !AssertOutOfArgs( pArgs ) )
			return RSERR;

		if( !pProject )
			return RSERR; //too many arguments

		if( pProject->GetPassword() != sPassword )
			return RSRSLT; //wrong password

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

	// ----- ads_dcl_updatecheck symbol (do not rename)
	static int ads_dcl_updatecheck(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		if( !AssertOutOfArgs( pArgs ) )
			return RSERR;

		DWORD dwMajor;
		DWORD dwMinor;
		DWORD dwThird;
		DWORD dwFourth;
		if( theWorkspace.GetModuleVersionInfo( dwMajor, dwMinor, dwThird, dwFourth, _hdllInstance ) )
		{
			CString sVersion;
			sVersion.Format( _T("%d.%d.%d.%d"), dwMajor, dwMinor, dwThird, dwFourth );
			UpdateCheck( gpszUCProductName, sVersion );
			acedRetT();
		}

		return (RSRSLT) ;
	}

	// ----- ads_dcl_setautoupdatecheck symbol (do not rename)
	static int ads_dcl_setautoupdatecheck(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		bool bEnable = true;
		GetBoolArgument( pArgs, bEnable, true );

		if( !AssertOutOfArgs( pArgs ) )
			return RSERR;

		if( theWorkspace.SetAutoUpdateCheckEnabled( bEnable ) )
			acedRetT();

		return (RSRSLT) ;
	}

	// ----- ads_dcl_getprojects symbol (do not rename)
	static int ads_dcl_getprojects(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		if( !AssertOutOfArgs( pArgs ) )
			return RSERR;

		resbuf* prbProjects = NULL;
		resbuf* prbTail = NULL;
		const TProjectList& Projects = theArxWorkspace.GetProjects();
		for( TProjectList::const_iterator iter = Projects.begin(); iter != Projects.end(); ++iter )
		{
			resbuf* prbProject = acutNewRb( RTSTR );
			acutNewString( (*iter)->GetKeyName(), prbProject->resval.rstring );
			if( prbTail )
				prbTail->rbnext = prbProject;
			else
				prbProjects = prbProject;
			prbTail = prbProject;
		}
		acedRetList( prbProjects );
		acutRelRb( prbProjects );

		return (RSRSLT) ;
	}

	// ----- ads_dcl_project_getforms symbol (do not rename)
	static int ads_dcl_project_getforms(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		TArxProjectPtr pProject;
		if( !GetProjectArgument( pArgs, pProject ) )
			return RSERR; //wrong argument type

		//optional arguments
		CString sPassword;
		GetStringArgument( pArgs, sPassword, true );

		if( !AssertOutOfArgs( pArgs ) )
			return RSERR;

		if( !pProject )
			return RSERR; //too many arguments

		if( pProject->GetPassword() != sPassword )
			return RSRSLT; //wrong password

		resbuf* prbForms = NULL;
		resbuf* prbTail = NULL;
		const TDclFormList& Forms = pProject->GetDclFormList();
		for( TDclFormList::const_iterator iter = Forms.begin(); iter != Forms.end(); ++iter )
		{
			resbuf* prbForm = acutNewRb( RTENAME );
			prbForm->resval.rlname[0] = (LONG_PTR)(const CDclControlObject*)(*iter)->GetControlProperties();
			prbForm->resval.rlname[1] = odcl::ptrDclControl;
			if( prbTail )
				prbTail->rbnext = prbForm;
			else
				prbForms = prbForm;
			prbTail = prbForm;
		}
		acedRetList( prbForms );
		acutRelRb( prbForms );

		return (RSRSLT) ;
	}

	// ----- ads_opendcl_init_ui symbol (do not rename)
	static int ads_opendcl_init_ui(void)
	{
		//----- Remove the following line if you do not expect any argument for this ADS function
		struct resbuf *pArgs =acedGetArgs () ;

	#if (_ACADTARGET >= 19)
		CString sModuleFilename;
		DWORD cchFilename = GetModuleFileName( _hdllInstance, sModuleFilename.GetBuffer( MAX_PATH ), MAX_PATH );
		sModuleFilename.ReleaseBuffer( cchFilename );
		CString sModulePath;
		DWORD cchPath = GetLongPathName( sModuleFilename, sModulePath.GetBuffer( MAX_PATH ), MAX_PATH );
		sModulePath.ReleaseBuffer( cchPath );
		sModulePath.MakeReverse();
		sModulePath = sModulePath.Mid( sModulePath.SpanExcluding( _T("\\/:") ).GetLength() );
		sModulePath.MakeReverse();
		resbuf rbTrustedPaths = { NULL };
		if( RTNORM == acedGetVar( _ACRX_T("TRUSTEDPATHS"), &rbTrustedPaths ) )
		{
			CString sTrustedPaths = rbTrustedPaths.resval.rstring;
			acutDelString( rbTrustedPaths.resval.rstring );
			int idxToken = 0;
			do
			{
				CString sPath = sTrustedPaths.Tokenize( _T(";"), idxToken );
				if( sPath.CompareNoCase( sModulePath ) == 0 )
					break;
			} while( idxToken != -1 );
			if( idxToken == -1 )
			{ //module path wasn't found, so add it
				if( !sTrustedPaths.IsEmpty() && sTrustedPaths.GetAt( sTrustedPaths.GetLength() - 1 ) != _T(';') )
					sTrustedPaths += _T(';');
				sTrustedPaths += sModulePath;
				rbTrustedPaths.resval.rstring = sTrustedPaths.LockBuffer();
				acedSetVar( _ACRX_T("TRUSTEDPATHS"), &rbTrustedPaths );
			}
		}
	#endif

		acedRetVoid () ;

		return (RSRSLT) ;
	}
} ;

CString CARXApp::msDialogToBeShown;
CPoint CARXApp::mptToBeShown;
TArxProjectPtr CARXApp::mpProjectToBeShown = NULL;
CString CARXApp::msActionToBeShown;
TDclFormPtr CARXApp::mpDclToBeShown = NULL;
int CARXApp::mnDoneDialogValue = -1;
int CARXApp::mnListOperation = 3; //1 = change selection, 2 = append item, 3 = replace all (default)

//-----------------------------------------------------------------------------
IMPLEMENT_ARX_ENTRYPOINT(CARXApp)

CString GetCurrentFunctionName() { return theRxApp.GetCurrentFunctionName(); }

//static
int CARXApp::ads_dcl_GetCtrlProperty(void)
{
	acedRetNil();
	int nFunctionCode = acedGetFunCode();
	if( nFunctionCode < ADSPROPFUNCBASE || nFunctionCode > ADSPROPFUNCBASE + Prop::_MaxId )
		return RSERR;
	GetCtrlProperty( static_cast<Prop::Id>(nFunctionCode - ADSPROPFUNCBASE) );
	return RSRSLT;
}

//static
int CARXApp::ads_dcl_SetCtrlProperty(void)
{
	acedRetNil();
	int nFunctionCode = acedGetFunCode();
	int nSetPropertyBase = ADSPROPFUNCBASE + Prop::_MaxId;
	if( nFunctionCode < nSetPropertyBase || nFunctionCode > nSetPropertyBase + Prop::_MaxId )
		return RSERR;
	SetCtrlProperty( static_cast<Prop::Id>(nFunctionCode - nSetPropertyBase) );
	return RSRSLT;
}


#ifdef _DIAGNOSTIC

static int DumpProject(void)
{
	struct resbuf *pArgs =acedGetArgs () ;

	TArxProjectPtr pProject = NULL;
	if( !GetProjectArgument( pArgs, pProject ) )
		return RSERR; //invalid input

	//optional arguments
	bool bDeep = false;
	GetBoolArgument( pArgs, bDeep, true );

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	if( !pProject )
		return RSRSLT; //project not found

	pProject->dump( bDeep );
	acedRetT();

	return RSRSLT;
}

static int DumpForm(void)
{
	struct resbuf *pArgs =acedGetArgs () ;

	TDclFormPtr pForm;
	if( !GetFormArgument( pArgs, pForm ) )
		return RSERR; //invalid input

	//optional arguments
	bool bDeep = false;
	GetBoolArgument( pArgs, bDeep, true );

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	if( !pForm )
		return RSRSLT; //form not found

	pForm->dump( bDeep );
	acedRetT();

	return RSRSLT;
}

static int DumpControl(void)
{
	struct resbuf *pArgs =acedGetArgs () ;

	TDclControlPtr pDclControl = NULL;
	if( !GetControlArgument( pArgs, pDclControl ) )
		return RSERR; //invalid input

	//optional arguments
	bool bDeep = false;
	GetBoolArgument( pArgs, bDeep, true );

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	if( !pDclControl )
		return RSRSLT; //control not found

	pDclControl->dump( bDeep );
	acedRetT();

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
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_invoke, true)
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
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_selectlinetype, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_getlineweight, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_selectlineweight, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_getscreensize, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_messagebox, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_getharddrivesize, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_getfocus, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_hideerrormsgbox, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_showerrormsgbox, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_suppressmessages, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_getblocksize, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_navigatetourl, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_activateemail, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_browsefolder, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_selectfolder, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_insert, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_insertblock, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_loadxref, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_attachxref, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_updatevarnames, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_forcedwgredraw, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_getmousecoords, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_getdwgmousecoords, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_multifiledialog, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_selectfiles, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_getpicturesize, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_project_getpicturesize, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_setcmdbarfocus, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_delayedinvoke, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_xtwipstopixels, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_ytwipstopixels, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_xpixelstotwips, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_ypixelstotwips, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_getolecolorvalue, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_registeractivexctrl, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_dirfiles, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_project_load, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_project_unload, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_project_saveas, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_project_import, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_project_export, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_updatecheck, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_setautoupdatecheck, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_getprojects, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_project_getforms, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, opendcl_init_ui, true)
