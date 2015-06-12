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
#include "DclControlTemplate.h"
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
#include "Methods_ImageList.h"
#include "Methods_ListBox.h"
#include "Methods_ListView.h"
#include "Methods_OptionList.h"
#include "Methods_PictureBox.h"
#include "Methods_ProgressBar.h"
#include "Methods_Project.h"
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
#include "DclPicture.h"
#include "ArxAcadSlideCtrl.h"
#include "AcadColorTable.h"
#include "LineWeightDlg.h"
#include "LinetypeDlg.h"
#include "UpdateCheck.h"


//-----------------------------------------------------------------------------
#define szRDS _RXST("dcl")
#define ADSFUNCBASE 1000 //the externally defined ADS functions will be defined starting at this function index
#define ADSPROPFUNCBASE 4000 //the property functions will be defined starting at this function index

// Custom ADS function macros that register function names with a dcl- prefix, and also register the old-style name with dcl_ prefix.
#ifdef _ZRXTARGET
	#define ODCL_ACED_ADSSYMBOL_ENTRY_AUTO(classname, name, regFunc) \
		__declspec(selectany) _ZDSSYMBOL_ENTRY __ZdsSymbolMap_##name = { _RXST("dcl-") _RXST(#name), classname::ads_dcl_ ##name, regFunc, -1 } ; \
		extern "C" __declspec(allocate("ZDSSYMBOL$__m")) __declspec(selectany) _ZDSSYMBOL_ENTRY* const __pZdsSymbolMap_##name = &__ZdsSymbolMap_##name ; \
		ZCED_ZDSSYMBOL_ENTRY_PRAGMA(name) \
		__declspec(selectany) _ZDSSYMBOL_ENTRY __ZdsSymbolMap_old##name = { _RXST("dcl_") _RXST(#name), classname::ads_dcl_ ##name, regFunc, -1 } ; \
		extern "C" __declspec(allocate("ZDSSYMBOL$__m")) __declspec(selectany) _ZDSSYMBOL_ENTRY* const __pZdsSymbolMap_old##name = &__ZdsSymbolMap_old##name ; \
		ZCED_ZDSSYMBOL_ENTRY_PRAGMA(old##name)
#elif _GRXTARGET
	#define ODCL_ACED_ADSSYMBOL_ENTRY_AUTO(classname, name, regFunc) \
		_GDSSYMBOL_ENTRY __GdsSymbolMap_dcl__##name = { L"dcl-" L#name, classname::ads_dcl_ ##name, regFunc, (UINT)-1 , NULL } ; \
		_CGcRxAddSymbolEntryAuto __GrxAddSymbolAuto_dcl__##name(& __GdsSymbolMap_dcl__##name) ; \
		_GDSSYMBOL_ENTRY __GdsSymbolMap_dcl_##name = { L"dcl_" L#name, classname::ads_dcl_ ##name, regFunc, (UINT)-1 , NULL } ; \
		_CGcRxAddSymbolEntryAuto __GrxAddSymbolAuto_dcl_##name(& __GdsSymbolMap_dcl_##name) ;
#elif _BRXTARGET
	#define ODCL_ACED_ADSSYMBOL_ENTRY_AUTO(T_CLASS,T_NAME,T_REGISTERFUNCTION) \
		__declspec(selectany) _AdsRegisteredSymbol __adsRegisteredSymbol_##T_NAME(T_CLASS::ads_dcl_ ##T_NAME,_ACRX_T("dcl-") _ACRX_T(#T_NAME),T_REGISTERFUNCTION); \
		const bool __adsRegisteredFunction_##T_CLASS##T_NAME = __adsRegisteredSymbol_##T_NAME.registerFunction(); \
		__declspec(selectany) _AdsRegisteredSymbol __adsRegisteredSymbol_old##T_NAME(T_CLASS::ads_dcl_ ##T_NAME,_ACRX_T("dcl_") _ACRX_T(#T_NAME),T_REGISTERFUNCTION); \
		const bool __adsRegisteredFunction_##T_CLASSold##T_NAME = __adsRegisteredSymbol_old##T_NAME.registerFunction();
#else
	#define ODCL_ACED_ADSSYMBOL_ENTRY_AUTO(classname, name, regFunc) \
		__declspec(selectany) _ADSSYMBOL_ENTRY __AdsSymbolMap_##name = { _RXST("dcl-") _RXST(#name), classname::ads_dcl_ ##name, regFunc, -1 } ; \
		extern "C" __declspec(allocate("ADSSYMBOL$__m")) __declspec(selectany) _ADSSYMBOL_ENTRY* const __pAdsSymbolMap_##name = &__AdsSymbolMap_##name ; \
		ACED_ADSSYMBOL_ENTRY_PRAGMA(name) \
		__declspec(selectany) _ADSSYMBOL_ENTRY __AdsSymbolMap_old##name = { _RXST("dcl_") _RXST(#name), classname::ads_dcl_ ##name, regFunc, -1 } ; \
		extern "C" __declspec(allocate("ADSSYMBOL$__m")) __declspec(selectany) _ADSSYMBOL_ENTRY* const __pAdsSymbolMap_old##name = &__AdsSymbolMap_old##name ; \
		ACED_ADSSYMBOL_ENTRY_PRAGMA(old##name)
#endif

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
	{_T("Project-Dump"),                 DumpProject},
	{_T("Form-Dump"),                    DumpForm},
	{_T("Control-Dump"),                 DumpControl},
#endif //_DIAGNOSTIC

	// General control methods
	{_T("Control-GetHWND"),              Control::GetHWND},
	{_T("Control-GetName"),              Control::GetName},
	{_T("Control-GetProperties"),        Control::GetProperties},
	{_T("Control-GetProperty"),          Control::GetProperty}, //used to get any property of any control
	{_T("Control-GetPos"),               Control::GetPos},
	{_T("Control-Redraw"),               Control::Redraw},
	{_T("Control-SetFocus"),             Control::SetFocus},
	{_T("Control-SetPos"),               Control::SetPos},
	{_T("Control-SetProperty"),          Control::SetProperty}, //used to set any property of any control
	{_T("Control-ShowToolTip"),          Control::ShowToolTip},
	{_T("Control-TabOrder"),             Control::TabOrder},

	// General control deprecated methods
	{_T("Control-GetCurPos"),            Control::GetPos},
	{_T("Control-ForceUpdateNow"),       Control::Redraw},
	{_T("Control-ZOrder"),               Control::TabOrder},

	// Property get/set deprecated methods
	{_T("Control-GetAlternateColor"),    T_GetCtrlProperty< Prop::AlternatingColor >},
	{_T("Control-SetAlternateColor"),    T_SetCtrlProperty< Prop::AlternatingColor >},
	{_T("Control-GetLimitText"),         T_GetCtrlProperty< Prop::TextLimit >},
	{_T("Control-SetLimitText"),         T_SetCtrlProperty< Prop::TextLimit >},
	{_T("Control-GetEditLabel"),         T_GetCtrlProperty< Prop::EditLabels >},
	{_T("Control-SetEditLabel"),         T_SetCtrlProperty< Prop::EditLabels >},
	{_T("Control-GetURLAddress"),        T_GetCtrlProperty< Prop::Hyperlink >},
	{_T("Control-SetURLAddress"),        T_SetCtrlProperty< Prop::Hyperlink >},
	{_T("Control-GetSingleExpanded"),    T_GetCtrlProperty< Prop::SingleClickExpand >},
	{_T("Control-SetSingleExpanded"),    T_SetCtrlProperty< Prop::SingleClickExpand >},

	// General form methods
	{_T("Form-Center"),                  Form::Center},
	{_T("Form-Close"),                   Form::Close},
	{_T("Form-CloseAll"),                Form::CloseAll},
	{_T("Form-Enable"),                  Form::Enable},
	{_T("Form-GetControlArea"),          Form::GetControlArea},
	{_T("Form-GetControls"),             Form::GetControls},
	{_T("Form-GetUUID"),                 Form::GetUUID},
	{_T("Form-Hide"),                    Form::Hide},
	{_T("Form-IsActive"),                Form::IsActive},
	{_T("Form-IsApplyEnabled"),          Form::IsApplyEnabled},
	{_T("Form-IsEnabled"),               Form::IsEnabled},
	{_T("Form-IsFloating"),              Form::IsFloating},
	{_T("Form-IsVisible"),               Form::IsVisible},
	{_T("Form-RecalcLayout"),            Form::RecalcLayout},
	{_T("Form-Resize"),                  Form::Resize},
	{_T("Form-SetApplyEnabled"),         Form::SetApplyEnabled},
	{_T("Form-SetMinMaxSizes"),          Form::SetMinMaxSizes},
	{_T("Form-Show"),                    Form::Show},
	{_T("Form-StartTimer"),              Form::StartTimer},

	// General form deprecated methods
	{_T("ConfigTab-IsApplyEnabled"),     Form::IsApplyEnabled},
	{_T("ConfigTab-SetApplyEnabled"),    Form::SetApplyEnabled},
	{_T("FlushGraphicButtons"),          Form::RecalcLayout},
	{_T("Form-GetHWND"),                 Control::GetHWND},
	{_T("Form-GetRectangle"),            Control::GetPos},
	{_T("Form-GetTitleBarText"),         T_GetCtrlProperty< Prop::TitleBarText >},
	{_T("Form-SetDialogMinMaxSizes"),    Form::SetMinMaxSizes},
	{_T("Form-SetFocus"),                Control::SetFocus},
	{_T("Form-SetPos"),                  Control::SetPos},
	{_T("Form-SetTitleBarText"),         T_SetCtrlProperty< Prop::TitleBarText >},

	// General project methods
	{_T("Project-Export"),               Project::Export},
	{_T("Project-ExportPicture"),        Project::ExportPicture},
	{_T("Project-GetForms"),             Project::GetForms},
	{_T("Project-GetPictures"),          Project::GetPictures},
	{_T("Project-GetPictureSize"),       Project::GetPictureSize},
	{_T("Project-Import"),               Project::Import},
	{_T("Project-Load"),                 Project::Load},
	{_T("Project-SaveAs"),               Project::SaveAs},
	{_T("Project-SetPicture"),           Project::SetPicture},
	{_T("Project-Unload"),               Project::Unload},

	// Animation control methods
	{_T("Animation-Close"),              Animation::Close},
	{_T("Animation-Load"),               Animation::Load},
	{_T("Animation-Play"),               Animation::Play},
	{_T("Animation-Seek"),               Animation::Seek},
	{_T("Animation-Stop"),               Animation::Stop},

	// Animation control deprecated methods
	{_T("Animate-Close"),                Animation::Close},
	{_T("Animate-Load"),                 Animation::Load},
	{_T("Animate-Play"),                 Animation::Play},
	{_T("Animate-Seek"),                 Animation::Seek},
	{_T("Animate-Stop"),                 Animation::Stop},

	// ActiveX Control methods
	{_T("AxControl-Get"),                AxControl::Get},
	{_T("AxControl-GetAxObject"),        AxControl::GetAxObject},
	{_T("AxControl-Invoke"),             AxControl::Invoke},
	{_T("AxControl-Put"),                AxControl::Put},

	// ActiveX Control deprecated methods
	{_T("AxControl-GetProperty"),        AxControl::Get},
	{_T("AxControl-SetProperty"),        AxControl::Put},
	{_T("AxControl-DoMethod"),           AxControl::Invoke},
	{_T("AxControl-GetOleObject"),       AxControl::GetAxObject},

	// ActiveX general functions
	{_T("GetOlePictureFromId"),          AxGeneral::GetOlePictureFromId},
	{_T("GetOlePictureFromFile"),        AxGeneral::GetOlePictureFromFile},

	// ActiveX Object methods
	{_T("AxObject-Get"),                 AxObject::Get},
	{_T("AxObject-Invoke"),              AxObject::Invoke},
	{_T("AxObject-Put"),                 AxObject::Put},
	{_T("AxObject-Release"),             AxObject::Release},

	// ActiveX Object deprecated methods
	{_T("AxObject-Close"),               AxObject::Release},
	{_T("AxObject-DoMethod"),            AxObject::Invoke},
	{_T("AxObject-GetProperty"),         AxObject::Get},
	{_T("AxObject-SetProperty"),         AxObject::Put},

	// Binary File methods
	{_T("BinFile-Check"),                BinFile::Check},
	{_T("BinFile-Close"),                BinFile::Close},
	{_T("BinFile-Open"),                 BinFile::Open},
	{_T("BinFile-Read"),                 BinFile::Read},
	{_T("BinFile-Write"),                BinFile::Write},

	// Binary File deprecated methods
	{_T("CheckBin"),                     BinFile::Check},
	{_T("CloseBin"),                     BinFile::Close},
	{_T("OpenBin"),                      BinFile::Open},
	{_T("ReadBin"),                      BinFile::Read},
	{_T("WriteBin"),                     BinFile::Write},

	// Block List control methods
	{_T("BlockList-Arrange"),            BlockList::Arrange},
	{_T("BlockList-GetCount"),           BlockList::GetCount},
	{_T("BlockList-GetCurSel"),          BlockList::GetCurSel},
	{_T("BlockList-GetFileName"),        BlockList::GetFileName},
	{_T("BlockList-GetItemText"),        BlockList::GetItemText},
	{_T("BlockList-GetSelCount"),        BlockList::GetSelCount},
	{_T("BlockList-GetSelectedItems"),   BlockList::GetSelectedItems},
	{_T("BlockList-GetSelectedNths"),    BlockList::GetSelectedNths},
	{_T("BlockList-LoadDwg"),            BlockList::LoadDwg},
	{_T("BlockList-Reset"),              BlockList::Reset},
	{_T("BlockList-SetCurSel"),          BlockList::SetCurSel},
	{_T("BlockList-SortTextItems"),      BlockList::SortTextItems},

	// Block View control methods
	{_T("BlockView-Clear"),              BlockView::Clear},
	{_T("BlockView-DisplayBlock"),       BlockView::DisplayBlock},
	{_T("BlockView-DisplayBlockToScale"),BlockView::DisplayBlockToScale},
	{_T("BlockView-DisplayDwg"),         BlockView::DisplayDwg},
	{_T("BlockView-DisplayDwgToScale"),  BlockView::DisplayDwgToScale},
	{_T("BlockView-DisplayPaperSpace"),  BlockView::DisplayPaperSpace},
	{_T("BlockView-GetBlockList"),       BlockView::GetBlockList},
	//{_T("BlockView-GetBlockSize"),       BlockView::GetBlockSize},
	{_T("BlockView-GetDwgSize"),         BlockView::GetDwgSize},
	{_T("BlockView-PreLoadDwg"),         BlockView::PreLoadDwg},
	{_T("BlockView-RefreshBlock"),       BlockView::RefreshBlock},
	{_T("BlockView-RemoveHighlight"),    BlockView::RemoveHighlight},
	{_T("BlockView-SetHighlight"),       BlockView::SetHighlight},
	{_T("BlockView-Zoom"),               BlockView::Zoom},

	// Block View control deprecated methods
	{_T("BlockView-LoadDwg"),            BlockView::DisplayDwg},
	{_T("BlockView-LoadDwgToScale"),     BlockView::DisplayDwgToScale},

	// Calendar control methods
	{_T("Calendar-GetCurSel"),           Calendar::GetCurSel},
	{_T("Calendar-GetMonthRangeEnd"),    Calendar::GetMonthRangeEnd},
	{_T("Calendar-GetMonthRangeStart"),  Calendar::GetMonthRangeStart},
	{_T("Calendar-GetRangeEnd"),         Calendar::GetRangeEnd},
	{_T("Calendar-GetRangeStart"),       Calendar::GetRangeStart},
	{_T("Calendar-GetSelRangeEnd"),      Calendar::GetSelRangeEnd},
	{_T("Calendar-GetSelRangeStart"),    Calendar::GetSelRangeStart},
	{_T("Calendar-GetToday"),            Calendar::GetToday},
	{_T("Calendar-SetCurSel"),           Calendar::SetCurSel},
	{_T("Calendar-SetRange"),            Calendar::SetRange},
	{_T("Calendar-SetSelRange"),         Calendar::SetSelRange},

	// Calendar control deprecated methods
	{_T("Month-GetCurSel"),              Calendar::GetCurSel},
	{_T("Month-GetMonthRangeEnd"),       Calendar::GetMonthRangeEnd},
	{_T("Month-GetMonthRangeStart"),     Calendar::GetMonthRangeStart},
	{_T("Month-GetRangeEnd"),            Calendar::GetRangeEnd},
	{_T("Month-GetRangeStart"),          Calendar::GetRangeStart},
	{_T("Month-GetSelRangeEnd"),         Calendar::GetSelRangeEnd},
	{_T("Month-GetSelRangeStart"),       Calendar::GetSelRangeStart},
	{_T("Month-GetToday"),               Calendar::GetToday},
	{_T("Month-SetCurSel"),              Calendar::SetCurSel},
	{_T("Month-SetRange"),               Calendar::SetRange},
	{_T("Month-SetSelRange"),            Calendar::SetSelRange},

	// Combo Box control methods
	{_T("ComboBox-AddColor"),            ComboBox::AddColor},
	{_T("ComboBox-AddList"),             ComboBox::AddList},
	{_T("ComboBox-AddPath"),             ComboBox::AddPath},
	{_T("ComboBox-AddString"),           ComboBox::AddString},
	{_T("ComboBox-Clear"),               ComboBox::Clear},
	{_T("ComboBox-ClearEdit"),           ComboBox::ClearEdit},
	{_T("ComboBox-DeleteItem"),          ComboBox::DeleteItem},
	{_T("ComboBox-Dir"),                 ComboBox::Dir},
	{_T("ComboBox-FindColor"),           ComboBox::FindColor},
	{_T("ComboBox-FindLineWeight"),      ComboBox::FindLineWeight},
	{_T("ComboBox-FindString"),          ComboBox::FindString},
	{_T("ComboBox-FindStringExact"),     ComboBox::FindStringExact},
	{_T("ComboBox-GetCount"),            ComboBox::GetCount},
	{_T("ComboBox-GetCurSel"),           ComboBox::GetCurSel},
	{_T("ComboBox-GetDir"),              ComboBox::GetDir},
	{_T("ComboBox-GetDroppedWidth"),     ComboBox::GetDroppedWidth},
	{_T("ComboBox-GetEBText"),           ComboBox::GetEBText},
	{_T("ComboBox-GetEditSel"),          ComboBox::GetEditSel},
	{_T("ComboBox-GetItemData"),         ComboBox::GetItemData},
	{_T("ComboBox-GetLBText"),           ComboBox::GetLBText},
	{_T("ComboBox-GetTopIndex"),         ComboBox::GetTopIndex},
	{_T("ComboBox-InsertString"),        ComboBox::InsertString},
	{_T("ComboBox-SelectString"),        ComboBox::SelectString},
	{_T("ComboBox-SetCurSel"),           ComboBox::SetCurSel},
	{_T("ComboBox-SetDroppedWidth"),     ComboBox::SetDroppedWidth},
	{_T("ComboBox-SetEditSel"),          ComboBox::SetEditSel},
	{_T("ComboBox-SetItemData"),         ComboBox::SetItemData},
	{_T("ComboBox-SetTopIndex"),         ComboBox::SetTopIndex},

	// Combo Box control deprecated methods
	{_T("ComboBox-DeleteString"),        ComboBox::DeleteItem},
	{_T("ComboBox-GetTBText"),           ComboBox::GetEBText},

	// DWG List control methods
	{_T("DwgList-DeleteItem"),           DwgList::DeleteItem},
	{_T("DwgList-Dir"),                  DwgList::Dir},
	{_T("DwgList-GetAnchorIndex"),       DwgList::GetAnchorIndex},
	{_T("DwgList-GetCount"),             DwgList::GetCount},
	{_T("DwgList-GetCurSel"),            DwgList::GetCurSel},
	{_T("DwgList-GetDir"),               DwgList::GetDir},
	{_T("DwgList-GetFileName"),          DwgList::GetFileName},
	{_T("DwgList-GetFocusIndex"),        DwgList::GetFocusIndex},
	{_T("DwgList-GetSelCount"),          DwgList::GetSelCount},
	{_T("DwgList-GetSelectedItems"),     DwgList::GetSelectedItems},
	{_T("DwgList-GetSelectedNths"),      DwgList::GetSelectedNths},
	{_T("DwgList-GetText"),              DwgList::GetText},
	{_T("DwgList-GetTopIndex"),          DwgList::GetTopIndex},
	{_T("DwgList-GetType"),              DwgList::GetType},
	{_T("DwgList-IsItemSelected"),       DwgList::IsItemSelected},
	{_T("DwgList-SelectItem"),           DwgList::SelectItem},
	{_T("DwgList-SelItemRange"),         DwgList::SelItemRange},
	{_T("DwgList-SetAnchorIndex"),       DwgList::SetAnchorIndex},
	{_T("DwgList-SetCurSel"),            DwgList::SetCurSel},
	{_T("DwgList-SetFocusIndex"),        DwgList::SetFocusIndex},
	{_T("DwgList-SetTopIndex"),          DwgList::SetTopIndex},

	// DWG List control deprecated methods
	{_T("DwgList-DeleteString"),         DwgList::DeleteItem},
	{_T("DwgList-GetSel"),               DwgList::IsItemSelected},
	{_T("DwgList-SetSel"),               DwgList::SelectItem},

	// DWG Preview control methods
	{_T("DwgPreview-Clear"),             DwgPreview::Clear},
	{_T("DwgPreview-GetDwgName"),        DwgPreview::GetDwgName},
	{_T("DwgPreview-LoadDwg"),           DwgPreview::LoadDwg},
	{_T("DwgPreview-RemoveHighlight"),   DwgPreview::RemoveHighlight},
	{_T("DwgPreview-SetHighlight"),      DwgPreview::SetHighlight},

	// File Explorer control methods
	{_T("FileExplorer-GetFileExt"),      FileExplorer::GetFileExt},
	{_T("FileExplorer-GetFileName"),     FileExplorer::GetFileName},
	{_T("FileExplorer-GetFileNameList"), FileExplorer::GetFileNameList},
	{_T("FileExplorer-GetFileTitle"),    FileExplorer::GetFileTitle},
	{_T("FileExplorer-GetFolderName"),   FileExplorer::GetFolderName},
	{_T("FileExplorer-GetFolderPath"),   FileExplorer::GetFolderPath},
	{_T("FileExplorer-GetPathName"),     FileExplorer::GetPathName},
	{_T("FileExplorer-GetSelCount"),     FileExplorer::GetSelCount},
	{_T("FileExplorer-SetOkButtonText"), FileExplorer::SetOkButtonText},

	// File Explorer control deprecated methods
	{_T("FileDlg-GetFileExt"),           FileExplorer::GetFileExt},
	{_T("FileDlg-GetFileName"),          FileExplorer::GetFileName},
	{_T("FileDlg-GetFileNameList"),      FileExplorer::GetFileNameList},
	{_T("FileDlg-GetFileTitle"),         FileExplorer::GetFileTitle},
	{_T("FileDlg-GetFolderName"),        FileExplorer::GetFolderName},
	{_T("FileDlg-GetFolderPath"),        FileExplorer::GetFolderPath},
	{_T("FileDlg-GetPathName"),          FileExplorer::GetPathName},
	{_T("FileDlg-GetSelectionCount"),    FileExplorer::GetSelCount},
	{_T("FileDlg-SetOkButtonText"),      FileExplorer::SetOkButtonText},

	// Grid control methods
	{_T("Grid-AddColumns"),              Grid::AddColumns},
	{_T("Grid-AddRow"),                  Grid::AddRow},
	{_T("Grid-AddString"),               Grid::AddString},
	{_T("Grid-CalcColumnWidth"),         Grid::CalcColumnWidth},
	{_T("Grid-CancelCellEdit"),          Grid::CancelCellEdit},
	{_T("Grid-Clear"),                   Grid::Clear},
	{_T("Grid-DeleteColumn"),            Grid::DeleteColumn},
	{_T("Grid-DeleteRow"),               Grid::DeleteRow},
	{_T("Grid-FillList"),                Grid::FillList},
	{_T("Grid-GetCellCheckState"),       Grid::GetCellCheckState},
	{_T("Grid-GetCellDropList"),         Grid::GetCellDropList},
	{_T("Grid-GetCellImages"),           Grid::GetCellImages},
	{_T("Grid-GetCellStyle"),            Grid::GetCellStyle},
	{_T("Grid-GetCellText"),             Grid::GetCellText},
	{_T("Grid-GetColumnCaption"),        Grid::GetColumnCaption},
	{_T("Grid-GetColumnCells"),          Grid::GetColumnCells},
	{_T("Grid-GetColumnCount"),          Grid::GetColumnCount},
	{_T("Grid-GetColumnImage"),          Grid::GetColumnImage},
	{_T("Grid-GetColumnWidth"),          Grid::GetColumnWidth},
	{_T("Grid-GetCurCell"),              Grid::GetCurCell},	
	{_T("Grid-GetRowCells"),             Grid::GetRowCells},
	{_T("Grid-GetRowCount"),             Grid::GetRowCount},
	{_T("Grid-GetRowData"),              Grid::GetRowData},
	{_T("Grid-HitPointTest"),            Grid::HitPointTest},
	{_T("Grid-InsertColumn"),            Grid::InsertColumn},
	{_T("Grid-InsertRow"),               Grid::InsertRow},
	{_T("Grid-InsertString"),            Grid::InsertString},
	{_T("Grid-SetCellCheckState"),       Grid::SetCellCheckState},
	{_T("Grid-SetCellDropList"),         Grid::SetCellDropList},
	{_T("Grid-SetCellImages"),           Grid::SetCellImages},
	{_T("Grid-SetCellStyle"),            Grid::SetCellStyle},
	{_T("Grid-SetCellText"),             Grid::SetCellText},
	{_T("Grid-SetColumnCaption"),        Grid::SetColumnCaption},
	{_T("Grid-SetColumnImage"),          Grid::SetColumnImage},
	{_T("Grid-SetColumnWidth"),          Grid::SetColumnWidth},
	{_T("Grid-SetCurCell"),              Grid::SetCurCell},
	{_T("Grid-SetRowData"),              Grid::SetRowData},
	{_T("Grid-SortNumericCells"),        Grid::SortNumericCells},
	{_T("Grid-SortTextCells"),           Grid::SortTextCells},
	{_T("Grid-StartCellEdit"),           Grid::StartCellEdit},

	// Grid control deprecated methods
	{_T("Grid-CalcColWidth"),            Grid::CalcColumnWidth},
	{_T("Grid-CancelItemEdit"),          Grid::CancelCellEdit},
	{_T("Grid-GetColCount"),             Grid::GetColumnCount},
	{_T("Grid-GetColImage"),             Grid::GetColumnImage},
	{_T("Grid-GetColItems"),             Grid::GetColumnCells},
	{_T("Grid-GetColWidth"),             Grid::GetColumnWidth},
	{_T("Grid-GetCurSel"),               Grid::GetCurCell},	
	{_T("Grid-GetItemCheck"),            Grid::GetCellCheckState},
	{_T("Grid-GetItemData"),             Grid::GetRowData},
	{_T("Grid-GetItemDropList"),         Grid::GetCellDropList},
	{_T("Grid-GetItemImage"),            Grid::GetCellImages},
	{_T("Grid-GetItemText"),             Grid::GetCellText},
	{_T("Grid-GetRowItems"),             Grid::GetRowCells},
	{_T("Grid-SetColImage"),             Grid::SetColumnImage},
	{_T("Grid-SetColWidth"),             Grid::SetColumnWidth},
	{_T("Grid-SelCurCell"),              Grid::SetCurCell},
	{_T("Grid-SelCurRow"),               Grid::SetCurCell},
	{_T("Grid-SetItemCheck"),            Grid::SetCellCheckState},
	{_T("Grid-SetItemData"),             Grid::SetRowData},
	{_T("Grid-SetItemDropList"),         Grid::SetCellDropList},
	{_T("Grid-SetItemImage"),            Grid::SetCellImages},
	{_T("Grid-SetItemStyle"),            Grid::SetCellStyle},
	{_T("Grid-SetItemText"),             Grid::SetCellText},
	{_T("Grid-SortNumericItems"),        Grid::SortNumericCells},
	{_T("Grid-SortTextItems"),           Grid::SortTextCells},
	{_T("Grid-StartItemEdit"),           Grid::StartCellEdit},

	// Hatch control methods
	{_T("Hatch-Clear"),                  Hatch::Clear},
	{_T("Hatch-SetPattern"),             Hatch::SetPattern},

	// Html control methods
	{_T("Html-GetBusy"),                 Html::GetBusy},
	{_T("Html-GetFullName"),             Html::GetFullName},
	{_T("Html-GetHtmlDocument"),         Html::GetHtmlDocument},
	{_T("Html-GetLocationName"),         Html::GetLocationName},
	{_T("Html-GetLocationURL"),          Html::GetLocationURL},
	{_T("Html-GetOffline"),              Html::GetOffline},
	{_T("Html-GoBack"),                  Html::GoBack},
	{_T("Html-GoForward"),               Html::GoForward},
	{_T("Html-GoHome"),                  Html::GoHome},
	{_T("Html-GoSearch"),                Html::GoSearch},
	{_T("Html-Navigate"),                Html::Navigate},
	{_T("Html-Refresh"),                 Html::Refresh},
	{_T("Html-ReplaceText"),             Html::ReplaceText},
	{_T("Html-SetOffline"),              Html::SetOffline},
	{_T("Html-Stop"),                    Html::Stop},
	{_T("Html-UpdateHtmlCode"),          Html::UpdateHtmlCode},

	// Image Combo Box control methods
	{_T("ImageComboBox-AddString"),      ImageComboBox::AddString},
	{_T("ImageComboBox-Clear"),          ImageComboBox::Clear},
	{_T("ImageComboBox-ClearEdit"),      ImageComboBox::ClearEdit},
	{_T("ImageComboBox-DeleteItem"),     ImageComboBox::DeleteItem},
	{_T("ImageComboBox-FindString"),     ImageComboBox::FindString},
	{_T("ImageComboBox-FindStringExact"), ImageComboBox::FindStringExact},
	{_T("ImageComboBox-GetCount"),       ImageComboBox::GetCount},
	{_T("ImageComboBox-GetCurSel"),      ImageComboBox::GetCurSel},
	{_T("ImageComboBox-GetDroppedWidth"), ImageComboBox::GetDroppedWidth},
	{_T("ImageComboBox-GetEBText"),      ImageComboBox::GetEBText},
	{_T("ImageComboBox-GetEditSel"),     ImageComboBox::GetEditSel},
	{_T("ImageComboBox-GetItem"),        ImageComboBox::GetItem},
	{_T("ImageComboBox-GetItemData"),    ImageComboBox::GetItemData},
	{_T("ImageComboBox-GetLBText"),      ImageComboBox::GetLBText},
	{_T("ImageComboBox-GetTopIndex"),    ImageComboBox::GetTopIndex},
	{_T("ImageComboBox-SelectString"),   ImageComboBox::SelectString},
	{_T("ImageComboBox-SetCurSel"),      ImageComboBox::SetCurSel},
	{_T("ImageComboBox-SetDroppedWidth"), ImageComboBox::SetDroppedWidth},
	{_T("ImageComboBox-SetEditSel"),     ImageComboBox::SetEditSel},
	{_T("ImageComboBox-SetItem"),        ImageComboBox::SetItem},
	{_T("ImageComboBox-SetItemData"),    ImageComboBox::SetItemData},
	{_T("ImageComboBox-SetTopIndex"),    ImageComboBox::SetTopIndex},

	// Image Combo Box control deprecated methods
	{_T("ImageComboBox-DeleteString"),   ImageComboBox::DeleteItem},
	{_T("ImageComboBox-GetTBText"),      ImageComboBox::GetEBText},

	// Image List methods
	{_T("ImageList-GetCount"),           ImageList::GetCount},
	{_T("ImageList-SetAt"),              ImageList::SetAt},
	{_T("ImageList-GetSize"),            ImageList::GetSize},

	// List Box control methods
	{_T("ListBox-AddList"),              ListBox::AddList},
	{_T("ListBox-AddString"),            ListBox::AddString},
	{_T("ListBox-Clear"),                ListBox::Clear},
	{_T("ListBox-DeleteItem"),           ListBox::DeleteItem},
	{_T("ListBox-Dir"),                  ListBox::Dir},
	{_T("ListBox-FindString"),           ListBox::FindString},
	{_T("ListBox-FindStringExact"),      ListBox::FindStringExact},
	{_T("ListBox-GetAnchorIndex"),       ListBox::GetAnchorIndex},
	{_T("ListBox-GetCount"),             ListBox::GetCount},
	{_T("ListBox-GetCurSel"),            ListBox::GetCurSel},
	{_T("ListBox-GetFocusIndex"),        ListBox::GetFocusIndex},
	{_T("ListBox-GetItemData"),          ListBox::GetItemData},
	{_T("ListBox-GetItemText"),          ListBox::GetItemText},
	{_T("ListBox-GetSelCount"),          ListBox::GetSelCount},
	{_T("ListBox-GetSelectedItems"),     ListBox::GetSelectedItems},
	{_T("ListBox-GetSelectedNths"),      ListBox::GetSelectedNths},
	{_T("ListBox-GetTopIndex"),          ListBox::GetTopIndex},
	{_T("ListBox-HitPointTest"),         ListBox::HitPointTest},
	{_T("ListBox-InsertString"),         ListBox::InsertString},
	{_T("ListBox-IsItemSelected"),       ListBox::IsItemSelected},
	{_T("ListBox-SelectItem"),           ListBox::SelectItem},
	{_T("ListBox-SelectString"),         ListBox::SelectString},
	{_T("ListBox-SelItemRange"),         ListBox::SelItemRange},
	{_T("ListBox-SetAnchorIndex"),       ListBox::SetAnchorIndex},
	{_T("ListBox-SetCurSel"),            ListBox::SetCurSel},
	{_T("ListBox-SetFocusIndex"),        ListBox::SetFocusIndex},
	{_T("ListBox-SetItemData"),          ListBox::SetItemData},
	{_T("ListBox-SetTopIndex"),          ListBox::SetTopIndex},

	// List Box control deprecated methods
	{_T("ListBox-DeleteString"),         ListBox::DeleteItem},
	{_T("ListBox-GetSel"),               ListBox::IsItemSelected},
	{_T("ListBox-GetText"),              ListBox::GetItemText},
	{_T("ListBox-SetSel"),               ListBox::SelectItem},

	// List View control methods
	{_T("ListView-AddColumns"),          ListView::AddColumns},
	{_T("ListView-AddItem"),             ListView::AddItem},
	{_T("ListView-AddString"),           ListView::AddString},
	{_T("ListView-Arrange"),             ListView::Arrange},
	{_T("ListView-CalcColumnWidth"),     ListView::CalcColumnWidth},
	{_T("ListView-CancelLabelEdit"),     ListView::CancelLabelEdit},
	{_T("ListView-Clear"),               ListView::Clear},
	{_T("ListView-DeleteColumn"),        ListView::DeleteColumn},
	{_T("ListView-DeleteColumns"),       ListView::DeleteColumns},
	{_T("ListView-DeleteItem"),          ListView::DeleteItem},
	{_T("ListView-DeleteItems"),         ListView::DeleteItems},
	{_T("ListView-FillList"),            ListView::FillList},
	{_T("ListView-GetColumnCount"),      ListView::GetColumnCount},
	{_T("ListView-GetColumnImage"),      ListView::GetColumnImage},
	{_T("ListView-GetColumnItems"),      ListView::GetColumnItems},
	{_T("ListView-GetColumnWidth"),      ListView::GetColumnWidth},
	{_T("ListView-GetCount"),            ListView::GetCount},
	{_T("ListView-GetCurSel"),           ListView::GetCurSel},
	{_T("ListView-GetItemData"),         ListView::GetItemData},
	{_T("ListView-GetItemImage"),        ListView::GetItemImage},
	{_T("ListView-GetItemText"),         ListView::GetItemText},
	{_T("ListView-GetRowItems"),         ListView::GetRowItems},
	{_T("ListView-GetSelCount"),         ListView::GetSelCount},
	{_T("ListView-GetSelectedItems"),    ListView::GetSelectedItems},
	{_T("ListView-GetSelectedNths"),     ListView::GetSelectedNths},
	{_T("ListView-HitPointTest"),        ListView::HitPointTest},
	{_T("ListView-InsertItem"),          ListView::InsertItem},
	{_T("ListView-SetColumnImage"),      ListView::SetColumnImage},
	{_T("ListView-SetColumnWidth"),      ListView::SetColumnWidth},
	{_T("ListView-SetCurSel"),           ListView::SetCurSel},
	{_T("ListView-SetItemData"),         ListView::SetItemData},
	{_T("ListView-SetItemImage"),        ListView::SetItemImage},
	{_T("ListView-SetItemText"),         ListView::SetItemText},
	{_T("ListView-SortNumericItems"),    ListView::SortNumericItems},
	{_T("ListView-SortTextItems"),       ListView::SortTextItems},
	{_T("ListView-StartLabelEdit"),      ListView::StartLabelEdit},

	// List View control deprecated methods
	{_T("ListView-CalcColWidth"),        ListView::CalcColumnWidth},
	{_T("ListView-CountItems"),          ListView::GetCount},
	{_T("ListView-GetColWidth"),         ListView::GetColumnWidth},
	{_T("ListView-SetColWidth"),         ListView::SetColumnWidth},

	// Option List control methods
	{_T("OptionList-AddButton"),         OptionList::AddButton},
	{_T("OptionList-AddList"),           OptionList::AddList},
	{_T("OptionList-Clear"),             OptionList::Clear},
	{_T("OptionList-DeleteButton"),      OptionList::DeleteButton},
	{_T("OptionList-GetButtonCaption"),  OptionList::GetButtonCaption},
	{_T("OptionList-GetCount"),          OptionList::GetCount},
	{_T("OptionList-GetCurSel"),         OptionList::GetCurSel},
	{_T("OptionList-GetTopIndex"),       OptionList::GetTopIndex},
	{_T("OptionList-InsertButton"),      OptionList::InsertButton},
	{_T("OptionList-SetButtonEnabled"),  OptionList::SetButtonEnabled},
	{_T("OptionList-SetButtonTooltip"),  OptionList::SetButtonTooltip},
	{_T("OptionList-SetCurSel"),         OptionList::SetCurSel},
	{_T("OptionList-SetTopIndex"),       OptionList::SetTopIndex},

	// Option List control deprecated methods
	{_T("OptionList-AddString"),         OptionList::AddButton},
	{_T("OptionList-DeleteString"),      OptionList::DeleteButton},
	{_T("OptionList-GetText"),           OptionList::GetButtonCaption},
	{_T("OptionList-InsertString"),      OptionList::InsertButton},
	{_T("OptionList-SetEnabled"),        OptionList::SetButtonEnabled},
	{_T("OptionList-SetTttTitle"),       OptionList::SetButtonTooltip},

	// Picture Box control methods
	{_T("PictureBox-Clear"),             PictureBox::Clear},
	{_T("PictureBox-DrawArc"),           PictureBox::DrawArc},
	{_T("PictureBox-DrawCircle"),        PictureBox::DrawCircle},
	{_T("PictureBox-DrawEdge"),          PictureBox::DrawEdge},
	{_T("PictureBox-DrawFocusRect"),     PictureBox::DrawFocusRect},
	{_T("PictureBox-DrawHatchRect"),     PictureBox::DrawHatchRect},
	{_T("PictureBox-DrawLine"),          PictureBox::DrawLine},
	{_T("PictureBox-DrawPoint"),         PictureBox::DrawPoint},
	{_T("PictureBox-DrawRect"),          PictureBox::DrawRect},
	{_T("PictureBox-DrawSolidRect"),     PictureBox::DrawSolidRect},
	{_T("PictureBox-DrawText"),          PictureBox::DrawText},
	{_T("PictureBox-DrawWrappedText"),   PictureBox::DrawWrappedText},
	{_T("PictureBox-GetTextExtent"),     PictureBox::GetTextExtent},
	{_T("PictureBox-LoadPictureFile"),   PictureBox::LoadPictureFile},
	{_T("PictureBox-PaintPicture"),      PictureBox::PaintPicture},
	{_T("PictureBox-Refresh"),           PictureBox::Refresh},
	{_T("PictureBox-StoreImage"),        PictureBox::StoreImage},

	// Picture Box control deprecated methods
	{_T("PictureBox-DrawFillRect"),      PictureBox::DrawSolidRect},

	// Progress Bar control methods
	{_T("ProgressBar-Reset"),            ProgressBar::Reset},

	// Progress Bar control deprecated methods
	{_T("ProgressBar-SetPos"),           ProgressBar::SetPos},

	// Slide View control methods
	{_T("SlideView-Clear"),              SlideView::Clear},
	{_T("SlideView-EndImage"),           SlideView::EndImage},
	{_T("SlideView-FillImage"),          SlideView::FillImage},
	{_T("SlideView-Load"),               SlideView::Load},
	{_T("SlideView-RemoveHighlight"),    SlideView::RemoveHighlight},
	{_T("SlideView-SetHighlight"),       SlideView::SetHighlight},
	{_T("SlideView-SlideImage"),         SlideView::SlideImage},
	{_T("SlideView-VectorImage"),        SlideView::VectorImage},

	// Tab Strip control methods
	{_T("TabStrip-GetControlArea"),      TabStrip::GetControlArea},
	{_T("TabStrip-GetCurSel"),           TabStrip::GetCurSel},
	{_T("TabStrip-GetRowCount"),         TabStrip::GetRowCount},
	{_T("TabStrip-HideTab"),             TabStrip::HideTab},
	{_T("TabStrip-SetCurSel"),           TabStrip::SetCurSel},
	{_T("TabStrip-SetTabCaption"),       TabStrip::SetTabCaption},
	{_T("TabStrip-ShowTab"),             TabStrip::ShowTab},

	// Tab Strip deprecated control methods
	{_T("Tab-GetControlArea"),           TabStrip::GetControlArea},
	{_T("Tab-GetCurSel"),                TabStrip::GetCurSel},
	{_T("Tab-GetRowCount"),              TabStrip::GetRowCount},
	{_T("Tab-HideTab"),                  TabStrip::HideTab},
	{_T("Tab-SetCurSel"),                TabStrip::SetCurSel},
	{_T("Tab-SetTabText"),               TabStrip::SetTabCaption},
	{_T("Tab-ShowTab"),                  TabStrip::ShowTab},

	// Text Box control methods
	{_T("TextBox-GetFilter"),            TextBox::GetFilter},
	{_T("TextBox-GetFirstVisibleLine"),  TextBox::GetFirstVisibleLine},
	{_T("TextBox-GetLine"),              TextBox::GetLine},
	{_T("TextBox-GetLineCount"),         TextBox::GetLineCount},
	{_T("TextBox-GetLineLength"),        TextBox::GetLineLength},
	{_T("TextBox-IsModified"),           TextBox::IsModified},
	{_T("TextBox-GetSel"),               TextBox::GetSel},
	{_T("TextBox-LineScroll"),           TextBox::LineScroll},
	{_T("TextBox-ReplaceSel"),           TextBox::ReplaceSel},
	{_T("TextBox-SetFilter"),            TextBox::SetFilter},
	{_T("TextBox-SetSel"),               TextBox::SetSel},
	{_T("TextBox-SetTabStops"),          TextBox::SetTabStops},
	{_T("TextBox-Undo"),                 TextBox::Undo},

	// Text Box control deprecated methods
	{_T("TextBox-GetModify"),            TextBox::IsModified},

	// Tree control methods
	{_T("Tree-AddChild"),                Tree::AddChild},
	{_T("Tree-AddParent"),               Tree::AddParent},
	{_T("Tree-AddSibling"),              Tree::AddSibling},
	{_T("Tree-CancelLabelEdit"),         Tree::CancelLabelEdit},
	{_T("Tree-Clear"),                   Tree::Clear},
	{_T("Tree-DeleteItem"),              Tree::DeleteItem},
	{_T("Tree-EnsureVisible"),           Tree::EnsureVisible},
	{_T("Tree-ExpandItem"),              Tree::ExpandItem},
	{_T("Tree-GetCount"),                Tree::GetCount},
	{_T("Tree-GetFirstChildItem"),       Tree::GetFirstChildItem},
	{_T("Tree-GetFirstVisibleItem"),     Tree::GetFirstVisibleItem},
	{_T("Tree-GetItemData"),             Tree::GetItemData},
	{_T("Tree-GetItemHandle"),           Tree::GetItemHandle},
	{_T("Tree-GetItemImages"),           Tree::GetItemImages},
	{_T("Tree-GetItemKey"),              Tree::GetItemKey},
	{_T("Tree-GetItemLabel"),            Tree::GetItemLabel},
	{_T("Tree-GetNextSiblingItem"),      Tree::GetNextSiblingItem},
	{_T("Tree-GetNextVisibleItem"),      Tree::GetNextVisibleItem},
	{_T("Tree-GetParentItem"),           Tree::GetParentItem},
	{_T("Tree-GetPrevSiblingItem"),      Tree::GetPrevSiblingItem},
	{_T("Tree-GetPrevVisibleItem"),      Tree::GetPrevVisibleItem},
	{_T("Tree-GetRootItem"),             Tree::GetRootItem},
	{_T("Tree-GetSelectedItem"),         Tree::GetSelectedItem},
	{_T("Tree-GetVisibleCount"),         Tree::GetVisibleCount},
	{_T("Tree-IsItemExpanded"),          Tree::IsItemExpanded},
	{_T("Tree-ItemHasChildren"),         Tree::ItemHasChildren},
	{_T("Tree-SelectItem"),              Tree::SelectItem},
	{_T("Tree-SelectSetFirstVisible"),   Tree::SelectSetFirstVisible},
	{_T("Tree-SetItemData"),             Tree::SetItemData},
	{_T("Tree-SetItemImages"),           Tree::SetItemImages},
	{_T("Tree-SetItemLabel"),            Tree::SetItemLabel},
	{_T("Tree-SortChildren"),            Tree::SortChildren},
	{_T("Tree-StartLabelEdit"),          Tree::StartLabelEdit},

	// Tree control deprecated methods
	{_T("Tree-CancelEditLabel"),         Tree::CancelLabelEdit},
	{_T("Tree-CountItems"),              Tree::GetCount},
	{_T("Tree-EditLabel"),               Tree::StartLabelEdit},
	{_T("Tree-GetExpandedImage"),        Tree::GetExpandedImage},
	{_T("Tree-GetItem"),                 Tree::GetItem},
	{_T("Tree-GetItemText"),             Tree::GetItemLabel},
	{_T("Tree-GetParent"),               Tree::GetParentItem},
	{_T("Tree-InsertAfter"),             Tree::AddSibling},
	{_T("Tree-SetExpandedImage"),        Tree::SetExpandedImage},
	{_T("Tree-SetItemText"),             Tree::SetItemLabel},
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
			if( nFunctionCode >= ADSFUNCBASE )
			{
				int nFuncEnd = (ADSFUNCBASE + _elements(grAdsFunctionTable));
				if (nFunctionCode < nFuncEnd )
					return (dclPrefix() + grAdsFunctionTable[nFunctionCode - ADSFUNCBASE].pszFunctionName);
				if (nFunctionCode < (nFuncEnd + _elements(grAdsFunctionTable)) )
				{
					CString sFunctionName = (dclPrefix() + grAdsFunctionTable[nFunctionCode - nFuncEnd].pszFunctionName);
					sFunctionName.Replace( _T('-'), _T('_') );
					return sFunctionName;
				}
			}
			if( nFunctionCode >= ADSPROPFUNCBASE )
			{
				int nFuncEnd = (ADSPROPFUNCBASE + Prop::_MaxId);
				if( nFunctionCode < nFuncEnd )
				{
					LPCTSTR pszPropName = GetPropertyApiName( (Prop::Id)(nFunctionCode - ADSPROPFUNCBASE) );
					if( pszPropName && *pszPropName != _T('(') )
						return (controlGetPrefix() + pszPropName);
				}
				else if( nFunctionCode < (nFuncEnd + Prop::_MaxId) )
				{
					LPCTSTR pszPropName = GetPropertyApiName( (Prop::Id)(nFunctionCode - nFuncEnd) );
					if( pszPropName && *pszPropName != _T('(') )
						return (controlSetPrefix() + pszPropName);
				}
				else if( nFunctionCode < (nFuncEnd + Prop::_MaxId + Prop::_MaxId) )
				{
					LPCTSTR pszPropName = GetPropertyApiName( (Prop::Id)(nFunctionCode - nFuncEnd - Prop::_MaxId) );
					if( pszPropName && *pszPropName != _T('(') )
					{
						CString sFunctionName = (controlSetPrefix() + pszPropName);
						sFunctionName.Replace( _T('-'), _T('_') );
						return sFunctionName;
					}
				}
				else if( nFunctionCode < (nFuncEnd + Prop::_MaxId + Prop::_MaxId + Prop::_MaxId) )
				{
					LPCTSTR pszPropName = GetPropertyApiName( (Prop::Id)(nFunctionCode - nFuncEnd - Prop::_MaxId - Prop::_MaxId) );
					if( pszPropName && *pszPropName != _T('(') )
					{
						CString sFunctionName = (controlSetPrefix() + pszPropName);
						sFunctionName.Replace( _T('-'), _T('_') );
						return sFunctionName;
					}
				}
			}
			else
			{
			#if (_BRXTARGET >= 11)
				if( nFunctionCode >= 0 && nFunctionCode < __adsRegisteredSymbols.length() )
					return __adsRegisteredSymbols[nFunctionCode]->getName();
			#elif (_BRXTARGET)
				if( nFunctionCode >= 0 && nFunctionCode < __adsRegisteredSymbols.length() )
					return __adsRegisteredSymbols[nFunctionCode].getName();
			#elif (_GRXTARGET)
				GcRxArxApp *pApp = (GcRxArxApp *)gcrxGetApp();
				_GDSSYMBOL_ENTRY* pAdsSymbolMapEntry = pApp->m_pSymbolEntryFirst;
				while (pAdsSymbolMapEntry)
				{
					if (pAdsSymbolMapEntry->nameID == nFunctionCode)
						return pAdsSymbolMapEntry->pszName;
					pAdsSymbolMapEntry = pAdsSymbolMapEntry->next;
				}
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
			static const CString sPrefix = _T("dcl-");
			return sPrefix;
		}
	static const CString& controlGetPrefix()
		{
			static const CString sPrefix = dclPrefix() + _T("Control-Get");
			return sPrefix;
		}
	static const CString& controlSetPrefix()
		{
			static const CString sPrefix = dclPrefix() + _T("Control-Set");
			return sPrefix;
		}
	static bool disableAutostartForVersion( LPCTSTR pszRegKeyInstance, bool bX64 )
		{
			HKEY hkRegTarget = NULL;
			if( ERROR_SUCCESS == RegOpenKeyEx( HKEY_CURRENT_USER, pszRegKeyInstance, 0, KEY_READ | (bX64? KEY_WOW64_64KEY : KEY_WOW64_32KEY), &hkRegTarget ) )
			{
				DWORD cchMaxSubkey = MAX_PATH;
				RegQueryInfoKey( hkRegTarget, NULL, NULL, NULL, NULL, &cchMaxSubkey, NULL, NULL, NULL, NULL, NULL, NULL );
				TCHAR* pszSubkey = new TCHAR[cchMaxSubkey + 1];
				DWORD idxSubkey = 0;
				DWORD dwBufSize = cchMaxSubkey + 1;
				while( RegEnumKeyEx( hkRegTarget,
															idxSubkey,
															pszSubkey,
															&dwBufSize,
															NULL,
															NULL,
															NULL,
															NULL ) == ERROR_SUCCESS )
				{
					idxSubkey++;
					CString sSubkey = pszSubkey;
					sSubkey += _T("\\Applications\\OpenDCL");
					HKEY hkRegApp = NULL;
					if( ERROR_SUCCESS == RegOpenKeyEx( hkRegTarget, sSubkey, 0, KEY_READ | KEY_WRITE | (bX64? KEY_WOW64_64KEY : KEY_WOW64_32KEY), &hkRegApp ) )
					{
						DWORD dwLoadCtrls = 0;
						DWORD dwSize = sizeof(dwLoadCtrls);
						DWORD dwType;
						if( ERROR_SUCCESS == RegQueryValueEx( hkRegApp, _T("LOADCTRLS"), 0, &dwType, (BYTE*)&dwLoadCtrls, &dwSize ) )
						{
							dwLoadCtrls &= (~DWORD(AcadApp::kOnAutoCADStartup));
							RegSetValueEx( hkRegApp, _T("LOADCTRLS"), 0, REG_DWORD, (BYTE*)&dwLoadCtrls, sizeof(dwLoadCtrls) );
						}
						RegCloseKey( hkRegApp );
					}
					dwBufSize = cchMaxSubkey + 1;
				}
				delete[] pszSubkey;
				RegCloseKey( hkRegTarget );
			}
			return true;
		}
	static bool disableAutostartForApp( LPCTSTR pszAppKey, bool bX64 )
		{
			bool bError = false;
			CString sRegTarget = _T("Software\\");
			sRegTarget += pszAppKey;
			HKEY hkRegTarget = NULL;
			if( ERROR_SUCCESS == RegOpenKeyEx( HKEY_CURRENT_USER, sRegTarget, 0, KEY_READ | (bX64? KEY_WOW64_64KEY : KEY_WOW64_32KEY), &hkRegTarget ) )
			{
				DWORD cchMaxSubkey = MAX_PATH;
				RegQueryInfoKey( hkRegTarget, NULL, NULL, NULL, NULL, &cchMaxSubkey, NULL, NULL, NULL, NULL, NULL, NULL );
				TCHAR* pszSubkey = new TCHAR[cchMaxSubkey + 1];
				DWORD idxSubkey = 0;
				DWORD dwBufSize = cchMaxSubkey + 1;
				while( RegEnumKeyEx( hkRegTarget,
															idxSubkey,
															pszSubkey,
															&dwBufSize,
															NULL,
															NULL,
															NULL,
															NULL ) == ERROR_SUCCESS )
				{
					idxSubkey++;
					CString sInstanceKey = sRegTarget + _T('\\') + pszSubkey;
					if( !disableAutostartForVersion( sInstanceKey, bX64 ) )
						bError = true;
					dwBufSize = cchMaxSubkey + 1;
				}
				delete[] pszSubkey;
				RegCloseKey( hkRegTarget );
			}
			return !bError;
		}

	#if (_ARXTARGET >= 19)
	static bool addTrustedPath( LPCTSTR pszPath )
		{
			CString sNewPath = pszPath;
			if( sNewPath.IsEmpty() )
				return false;
			sNewPath.Replace( _T('/'), _T('\\') );
			if( sNewPath.GetAt( sNewPath.GetLength() - 1 ) != _T('\\') )
				sNewPath += _T('\\');
			resbuf rbTrustedPaths = { NULL };
			if( RTNORM == acedGetVar( _ACRX_T("TRUSTEDPATHS"), &rbTrustedPaths ) )
			{
				CString sTrustedPaths = rbTrustedPaths.resval.rstring;
				acutDelString( rbTrustedPaths.resval.rstring );
				int idxToken = 0;
				do
				{
					CString sPath = sTrustedPaths.Tokenize( _T(";"), idxToken );
					if( sPath.CompareNoCase( sNewPath ) == 0 )
						break;
				} while( idxToken != -1 );
				if( idxToken == -1 )
				{ //path wasn't found, so add it
					if( !sTrustedPaths.IsEmpty() && sTrustedPaths.GetAt( sTrustedPaths.GetLength() - 1 ) != _T(';') )
						sTrustedPaths += _T(';');
					sTrustedPaths += sNewPath;
					rbTrustedPaths.resval.rstring = sTrustedPaths.LockBuffer();
					acedSetVar( _ACRX_T("TRUSTEDPATHS"), &rbTrustedPaths );
				}
			}
			return true;
		}
	#endif

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
			if( nFunctionCode >= ADSFUNCBASE )
			{
				int nFuncEnd = (ADSFUNCBASE + _elements(grAdsFunctionTable));
				if( nFunctionCode < nFuncEnd )
				{
					if( RSRSLT != grAdsFunctionTable[nFunctionCode - ADSFUNCBASE].pfHandler() )
						return AcRx::kRetError;
					return AcRx::kRetOK;
				}
				if( nFunctionCode < nFuncEnd + _elements(grAdsFunctionTable) )
				{
					if( RSRSLT != grAdsFunctionTable[nFunctionCode - nFuncEnd].pfHandler() )
						return AcRx::kRetError;
					return AcRx::kRetOK;
				}
			}
			if( nFunctionCode >= ADSPROPFUNCBASE )
			{
				int nGetPropertyEnd = (ADSPROPFUNCBASE + Prop::_MaxId);
				if( nFunctionCode < nGetPropertyEnd )
				{
					if( !GetCtrlProperty( static_cast<Prop::Id>(nFunctionCode - ADSPROPFUNCBASE) ) )
						return AcRx::kRetError;
					return AcRx::kRetOK;
				}
				int nSetPropertyEnd = nGetPropertyEnd + Prop::_MaxId;
				if( nFunctionCode < nSetPropertyEnd )
				{
					if( !SetCtrlProperty( static_cast<Prop::Id>(nFunctionCode - nGetPropertyEnd) ) )
						return AcRx::kRetError;
					return AcRx::kRetOK;
				}
				int nOldGetPropertyEnd = nSetPropertyEnd + Prop::_MaxId;
				if( nFunctionCode < nOldGetPropertyEnd )
				{
					if( !SetCtrlProperty( static_cast<Prop::Id>(nFunctionCode - nSetPropertyEnd) ) )
						return AcRx::kRetError;
					return AcRx::kRetOK;
				}
				int nOldSetPropertyEnd = nOldGetPropertyEnd + Prop::_MaxId;
				if( nFunctionCode < nOldSetPropertyEnd )
				{
					if( !SetCtrlProperty( static_cast<Prop::Id>(nFunctionCode - nOldGetPropertyEnd) ) )
						return AcRx::kRetError;
					return AcRx::kRetOK;
				}
			}
		}
		return (retCode) ;
	}

	virtual AcRx::AppRetCode On_kLoadDwgMsg (void *pkt) {
		AcRx::AppRetCode retCode =AcRxArxApp::On_kLoadDwgMsg (pkt) ;
		
		static bool bInitialized = false;
		if( !bInitialized )
		{
			bInitialized = true;
			ads_queueexpr( CString(_T("(opendcl_init_ui)")).LockBuffer() );
		}

		if( retCode == AcRx::kRetOK )
		{
			//register control specific ADS functions
			for( int idxFunction = 0; idxFunction < _elements(grAdsFunctionTable); ++idxFunction )
			{
				const AdsFunctionTableEntry& Entry = grAdsFunctionTable[idxFunction];
				CString sFunctionName = dclPrefix() + Entry.pszFunctionName;
				acedDefun( sFunctionName, ADSFUNCBASE + idxFunction );
				acedRegFunc( Entry.pfHandler, ADSFUNCBASE + idxFunction );
				if( sFunctionName.Replace( _T('-'), _T('_') ) > 0 )
				{
					acedDefun( sFunctionName, ADSFUNCBASE + _elements(grAdsFunctionTable) + idxFunction ); //register using the old name convention
					acedRegFunc( Entry.pfHandler, ADSFUNCBASE + _elements(grAdsFunctionTable) + idxFunction );
				}
			}

			//register general property get and set functions
			for( T_PropertyIdSet::const_iterator iter = mPropIds.begin(); iter != mPropIds.end(); ++iter )
			{
				Prop::Id id = *iter;
				LPCTSTR pszPropName = GetPropertyApiName( id );
				if( pszPropName && *pszPropName != _T('(') )
				{
					CString sGetFunctionName = controlGetPrefix() + pszPropName;
					acedDefun( sGetFunctionName, ADSPROPFUNCBASE + id );
					acedRegFunc( ads_dcl_GetCtrlProperty, ADSPROPFUNCBASE + id );
					if( sGetFunctionName.Replace( _T('-'), _T('_') ) > 0 )
					{
						acedDefun( sGetFunctionName, ADSPROPFUNCBASE + Prop::_MaxId + Prop::_MaxId + id ); //register using the old name convention
						acedRegFunc( ads_dcl_GetCtrlProperty, ADSPROPFUNCBASE + Prop::_MaxId + Prop::_MaxId + id );
					}
					CString sSetFunctionName = controlSetPrefix() + pszPropName;
					acedDefun( sSetFunctionName, ADSPROPFUNCBASE + id + Prop::_MaxId );
					acedRegFunc( ads_dcl_SetCtrlProperty, ADSPROPFUNCBASE + id + Prop::_MaxId );
					if( sSetFunctionName.Replace( _T('-'), _T('_') ) > 0 )
					{
						acedDefun( sSetFunctionName, ADSPROPFUNCBASE + id + Prop::_MaxId + Prop::_MaxId + Prop::_MaxId ); //register using the old name convention
						acedRegFunc( ads_dcl_SetCtrlProperty, ADSPROPFUNCBASE + id + Prop::_MaxId + Prop::_MaxId + Prop::_MaxId );
					}
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

	// ----- OpenDCL.OpenDCLDemo command
	static void OpenDCLOpenDCLDemo(void)
	{ //load OpenDCL demo if samples are installed
		static const LPCTSTR pszDemoFilename = _T("@AllSamples.lsp");
		CString sDemoFilepath;
	#ifdef _DEBUG
		TCHAR szDemoFilepath[MAX_PATH];
		if( RTNORM == acedFindFile( pszDemoFilename, szDemoFilepath ) )
			sDemoFilepath = szDemoFilepath;
		if( sDemoFilepath.IsEmpty() )
	#endif
		{
			static const LPCTSTR pszRKRoot = _T("SOFTWARE\\OpenDCL");
			static const LPCTSTR pszRKSamplesFolder = _T("SamplesFolder");
			CString sSamplesFolder;
			HKEY hkReg = NULL;
			if( ERROR_SUCCESS == RegOpenKeyEx( HKEY_CURRENT_USER, pszRKRoot, 0, KEY_QUERY_VALUE, &hkReg ) )
			{
				DWORD dwType = 0;
				DWORD cbValue = MAX_PATH - sizeof(TCHAR);
				if( ERROR_SUCCESS == RegQueryValueEx( hkReg, pszRKSamplesFolder, NULL, &dwType, (BYTE*)sSamplesFolder.GetBuffer( MAX_PATH ), &cbValue ) &&
						dwType == REG_SZ )
					sSamplesFolder.ReleaseBuffer();
				else
					sSamplesFolder.ReleaseBuffer( 0 );
				RegCloseKey( hkReg );
			}
			if( sSamplesFolder.IsEmpty() )
			{
				if( ERROR_SUCCESS == RegOpenKeyEx( HKEY_LOCAL_MACHINE, pszRKRoot, 0, KEY_QUERY_VALUE, &hkReg ) )
				{
					DWORD dwType = 0;
					DWORD cbValue = MAX_PATH - sizeof(TCHAR);
					if( ERROR_SUCCESS == RegQueryValueEx( hkReg, pszRKSamplesFolder, NULL, &dwType, (BYTE*)sSamplesFolder.GetBuffer( MAX_PATH ), &cbValue ) &&
							dwType == REG_SZ )
						sSamplesFolder.ReleaseBuffer();
					else
						sSamplesFolder.ReleaseBuffer( 0 );
					RegCloseKey( hkReg );
				}
			#ifdef _WIN64
				if( sSamplesFolder.IsEmpty() )
				{
					if( ERROR_SUCCESS == RegOpenKeyEx( HKEY_LOCAL_MACHINE, pszRKRoot, 0, KEY_QUERY_VALUE | KEY_WOW64_32KEY, &hkReg ) )
					{
						DWORD dwType = 0;
						DWORD cbValue = MAX_PATH - sizeof(TCHAR);
						if( ERROR_SUCCESS == RegQueryValueEx( hkReg, pszRKSamplesFolder, NULL, &dwType, (BYTE*)sSamplesFolder.GetBuffer( MAX_PATH ), &cbValue ) &&
								dwType == REG_SZ )
							sSamplesFolder.ReleaseBuffer();
						else
							sSamplesFolder.ReleaseBuffer( 0 );
						RegCloseKey( hkReg );
					}
				}
			#endif //_WIN64
			}
			if( sSamplesFolder.IsEmpty() )
				return;
			if( sSamplesFolder.Right(1).FindOneOf( _T("\\/") ) < 0 )
				sSamplesFolder += _T('\\');
		#if (_ARXTARGET >= 19)
			addTrustedPath( sSamplesFolder );
		#endif
			sDemoFilepath = sSamplesFolder + pszDemoFilename;
		}
		sDemoFilepath.Replace( _T('\\'), _T('/') );
		CString sExpr;
		sExpr.Format( _T("(load \"%s\")"), (LPCTSTR)sDemoFilepath );
		ads_queueexpr( sExpr.LockBuffer() );
	}
	
	// ----- ads_opendcl_init_ui symbol (do not rename)
	static int ads_opendcl_init_ui(void)
	{
	#if (_ARXTARGET >= 19)
		{
			CString sModuleFilename;
			DWORD cchFilename = GetModuleFileName( _hdllInstance, sModuleFilename.GetBuffer( MAX_PATH ), MAX_PATH );
			sModuleFilename.ReleaseBuffer( cchFilename );
			CString sModulePath;
			DWORD cchPath = GetLongPathName( sModuleFilename, sModulePath.GetBuffer( MAX_PATH ), MAX_PATH );
			sModulePath.ReleaseBuffer( cchPath );
			sModulePath.MakeReverse();
			sModulePath = sModulePath.Mid( sModulePath.SpanExcluding( _T("\\/:") ).GetLength() );
			sModulePath.MakeReverse();
			addTrustedPath( sModulePath );
		}
	#endif

		{
			HKEY hkReg = NULL;
			if( ERROR_SUCCESS == RegOpenKeyEx( HKEY_CURRENT_USER, _T("Software\\OpenDCL"), 0, KEY_READ | KEY_WRITE, &hkReg ) )
			{
				DWORD dwValue = 0;
				DWORD dwType = 0;
				DWORD cbValue = sizeof(dwValue);
				RegQueryValueEx( hkReg, _T("DisableAutoStartOnNextLoad"), NULL, &dwType, (BYTE*)&dwValue, &cbValue );
				bool bDisableNow = (dwValue == 1);
				if( bDisableNow )
				{
					dwValue = 0;
					RegSetValueEx( hkReg, _T("DisableAutoStartOnNextLoad"), 0, REG_DWORD, (BYTE*)&dwValue, sizeof(dwValue) );
				}
				RegCloseKey( hkReg );
				if( bDisableNow )
				{
					acutPrintf( _T("\n") );
					OpenDCLOpenDCLDemo();
					disableAutostartForApp( _T("Autodesk\\AutoCAD"), false );
					disableAutostartForApp( _T("Autodesk\\AutoCAD"), true );
					disableAutostartForApp( _T("Bricsys\\Bricscad"), false );
					disableAutostartForApp( _T("Bricsys\\Bricscad"), true );
					disableAutostartForApp( _T("ZWSOFT\\ZWCAD"), false );
				}
			}
		}

		acedRetVoid () ;

		return (RSRSLT) ;
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
		return Project::Load();
	}

	// ----- ads_dcl_load_dialog symbol (do not rename)
	static int ads_dcl_load_dialog(void)
	{
		return Project::Load();
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
		mptToBeShown.SetPoint(INT_MIN, INT_MIN); //should be encapsulated in an instance class [ORW]
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
		return Project::Unload();
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
		DialogParams params( mptToBeShown, CSize(0, 0) );
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
		return Project::GetPictureSize();
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
	if( nFunctionCode >= ADSPROPFUNCBASE )
	{
		if( nFunctionCode < ADSPROPFUNCBASE + Prop::_MaxId )
			GetCtrlProperty( static_cast<Prop::Id>(nFunctionCode - ADSPROPFUNCBASE) );
		else if( nFunctionCode < ADSPROPFUNCBASE + Prop::_MaxId + Prop::_MaxId )
			return RSERR;
		else if( nFunctionCode < ADSPROPFUNCBASE + Prop::_MaxId + Prop::_MaxId + Prop::_MaxId )
			GetCtrlProperty( static_cast<Prop::Id>(nFunctionCode - Prop::_MaxId - Prop::_MaxId - ADSPROPFUNCBASE) );
		else
			return RSERR;
		return RSRSLT;
	}
	return RSERR;
}

//static
int CARXApp::ads_dcl_SetCtrlProperty(void)
{
	acedRetNil();
	int nFunctionCode = acedGetFunCode();
	int nSetPropertyBase = ADSPROPFUNCBASE + Prop::_MaxId;
	if( nFunctionCode >= nSetPropertyBase )
	{
		if( nFunctionCode < nSetPropertyBase + Prop::_MaxId )
			SetCtrlProperty( static_cast<Prop::Id>(nFunctionCode - nSetPropertyBase) );
		else if( nFunctionCode < nSetPropertyBase + Prop::_MaxId + Prop::_MaxId )
			return RSERR;
		else if( nFunctionCode < nSetPropertyBase + Prop::_MaxId + Prop::_MaxId + Prop::_MaxId )
			SetCtrlProperty( static_cast<Prop::Id>(nFunctionCode - Prop::_MaxId -  Prop::_MaxId - nSetPropertyBase) );
		else
			return RSERR;
		return RSRSLT;
	}
	return RSERR;
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
ACED_ARXCOMMAND_ENTRY_AUTO(CARXApp, OpenDCL, OpenDCLDemo, OpenDCLDemo, ACRX_CMD_TRANSPARENT, NULL)
ODCL_ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, getversion, true)
ODCL_ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, getversionex, true)
ODCL_ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, loadproject, true)
ODCL_ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, load_dialog, true)
ODCL_ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, sendstring, true)
ODCL_ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, invokefunc, true)
ODCL_ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, invoke, true)
ODCL_ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, new_dialog, true)
ODCL_ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, unload_dialog, true)
ODCL_ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, done_dialog, true)
ODCL_ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, start_dialog, true)
ODCL_ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, start_list, true)
ODCL_ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, add_list, true)
ODCL_ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, end_list, true)
ODCL_ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, action_tile, true)
ODCL_ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, set_tile, true)
ODCL_ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, mode_tile, true)
ODCL_ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, get_tile, true)
ODCL_ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, get_attr, true)
ODCL_ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, start_image, true)
ODCL_ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, vector_image, true)
ODCL_ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, fill_image, true)
ODCL_ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dimx_tile, true)
ODCL_ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dimy_tile, true)
ODCL_ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, end_image, true)
ODCL_ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, slide_image, true)
ODCL_ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, getlinetype, true)
ODCL_ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, selectlinetype, true)
ODCL_ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, getlineweight, true)
ODCL_ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, selectlineweight, true)
ODCL_ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, getscreensize, true)
ODCL_ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, messagebox, true)
ODCL_ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, getharddrivesize, true)
ODCL_ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, getfocus, true)
ODCL_ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, hideerrormsgbox, true)
ODCL_ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, showerrormsgbox, true)
ODCL_ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, suppressmessages, true)
ODCL_ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, getblocksize, true)
ODCL_ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, navigatetourl, true)
ODCL_ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, activateemail, true)
ODCL_ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, browsefolder, true)
ODCL_ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, selectfolder, true)
ODCL_ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, insert, true)
ODCL_ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, insertblock, true)
ODCL_ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, loadxref, true)
ODCL_ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, attachxref, true)
ODCL_ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, updatevarnames, true)
ODCL_ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, forcedwgredraw, true)
ODCL_ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, getmousecoords, true)
ODCL_ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, getdwgmousecoords, true)
ODCL_ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, multifiledialog, true)
ODCL_ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, selectfiles, true)
ODCL_ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, getpicturesize, true)
ODCL_ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, setcmdbarfocus, true)
ODCL_ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, delayedinvoke, true)
ODCL_ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, xtwipstopixels, true)
ODCL_ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, ytwipstopixels, true)
ODCL_ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, xpixelstotwips, true)
ODCL_ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, ypixelstotwips, true)
ODCL_ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, getolecolorvalue, true)
ODCL_ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, registeractivexctrl, true)
ODCL_ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dirfiles, true)
ODCL_ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, updatecheck, true)
ODCL_ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, setautoupdatecheck, true)
ODCL_ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, getprojects, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, opendcl_init_ui, true)
