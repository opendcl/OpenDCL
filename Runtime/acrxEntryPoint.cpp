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
#include "Methods_Animate.h"
#include "Methods_ListBox.h"
#include "Methods_BlockList.h"
#include "Methods_Grid.h"
#include "Methods_Hatch.h"
#include "Methods_ListView.h"
#include "Methods_PictureBox.h"
#include "Methods_Properties.h"
#include "Methods_SlideView.h"
#include "Methods_Tree.h"
#include "Methods_Edit.h"
#include "Methods_Tab.h"
#include "Methods_ComboBox.h"
#include "Methods_ComboBoxEx.h"
#include "Methods_Html.h"
#include "Methods_Month.h"
#include "Methods_BlockView.h"
#include "Methods_DwgPreview.h"
#include "Methods_ActiveX.h"
#include "Methods_FileDlg.h"
#include "Methods_BinFiles.h"
#include "Methods_OptionList.h"
#include "Methods_DwgList.h"
#include "DialogObject.h"
#include "CustomFileDialog.h"
#include "ErrorLexicon.h"
#include "LayeredWindowHelperST.h"
#include "ControlTypes.h"
#include "ArchiveEx.h"
#include "SlideHolder.h"
#include "InvokeMethod.h"
#include "DialogControl.h"
#include "StringCompare.h"
#include "DirDialog.h"
#include "PictureObject.h"
#include "AcadColorTable.h"
#include "LineWeightDlg.h"
#include "LinetypeDlg.h"
#include "Base64.h"
#include "UpdateCheck.h"


//-----------------------------------------------------------------------------
#define szRDS _RXST("dcl")
#define ADSFUNCBASE 1000 //the externally defined ADS functions will be defined starting at this function index
#define ADSPROPFUNCBASE 2000 //the property functions will be defined starting at this function index

#define theRxApp entryPointObject

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
	{_T("Control_SetProperty"),			Control::SetProperty}, // this method is used to set any property of any control
	{_T("Control_GetProperty"),			Control::GetProperty}, // this method is used to get any property of any control
	{_T("Control_ShowToolTip"),			Control::ShowToolTip},
	{_T("Control_SetFocus"),				Control::SetFocus},
	{_T("Control_ZOrder"),				Control::ZOrder},
	{_T("Control_GetCurPos"),			Control::GetCurPos},
	{_T("Control_SetPos"),				Control::SetPos},
	{_T("Control_ForceUpdateNow"),		Control::ForceUpdateNow},

	{_T("ProgressBar_SetPos"),	ProgressBar::SetPos},

	// animation control methods
	{_T("Animate_Load"),			Animate::Load},
	{_T("Animate_Seek"),			Animate::Seek},
	{_T("Animate_Stop"),			Animate::Stop},
	{_T("Animate_Close"),			Animate::Close},

	{_T("Hatch_Clear"),			Hatch::Clear},
	{_T("Hatch_SetPattern"), Hatch::SetPattern},

	// file dialog box methods
	{_T("FileDlg_GetFileName"),		FileDlg::GetFileName},
	{_T("FileDlg_GetFileTitle"),		FileDlg::GetFileTitle},
	{_T("FileDlg_GetPathName"),		FileDlg::GetPathName},
	{_T("FileDlg_GetSelectionCount"),	FileDlg::GetSelectionCount},
	{_T("FileDlg_GetFolderPath"),		FileDlg::GetFolderPath},
	{_T("FileDlg_GetFileNameList"),	FileDlg::GetFileNameList},
	{_T("FileDlg_SetOkButtonText"),	FileDlg::SetOkButtonText},
	{_T("FileDlg_GetFileExt"),		FileDlg::GetFileExt},
	{_T("FileDlg_GetFolderName"),		FileDlg::GetFolderName},

	// ActiveX control methods
	{_T("AxControl_GetProperty"),		AxControl::GetProperty},
	{_T("AxControl_SetProperty"),		AxControl::SetProperty},
	{_T("AxControl_DoMethod"),		AxControl::DoMethod},
	{_T("AxControl_SetColor"),		AxControl::SetColorProperty},
	{_T("AxControl_SetPicture"),		AxControl::SetPictureProperty},
	{_T("AxControl_GetOleObject"),		AxControl::GetOleObject},
	{_T("AxObject_GetProperty"),		AxObject::GetProperty},
	{_T("AxObject_SetProperty"),		AxObject::SetProperty},
	{_T("AxObject_DoMethod"),		AxObject::DoMethod},
	{_T("AxObject_Close"),		AxObject::Close},

	// block view control methods
	{_T("BlockView_Clear"),				BlockView::Clear},
	{_T("BlockView_SetHighLight"),		BlockView::SetHighLight},
	{_T("BlockView_RemoveHighLight"),	BlockView::RemoveHighLight},
	{_T("BlockView_Zoom"),				BlockView::Zoom},
	{_T("BlockView_DisplayBlock"),			BlockView::DisplayBlock},
	{_T("BlockView_DisplayBlockToScale"),	BlockView::DisplayBlockToScale},
	{_T("BlockView_DisplayPaperSpace"),		BlockView::DisplayPaperSpace},											
	{_T("BlockView_LoadDwg"),			BlockView::LoadDwg},	
	{_T("BlockView_LoadDwgToScale"),		BlockView::LoadDwgToScale},
	{_T("BlockView_PreLoadDwg"),			BlockView::PreLoadDwg},
	{_T("BlockView_GetBlockList"),		BlockView::GetBlockList},
	{_T("BlockView_GetDwgSize"),			BlockView::GetDwgSize},
	{_T("BlockView_RefreshBlock"),		BlockView::RefreshBlock},
	//{_T("BlockView_GetViewInfo"),		BlockView_GetViewInfo},
	//{_T("BlockView_SetView"),			BlockView_SetView},

	// dwg preview control methods 
	{_T("DwgPreview_LoadDwg"),			DwgPreview::LoadDwg},
	{_T("DwgPreview_GetDwgName"),		DwgPreview::GetDwgName},
	{_T("DwgPreview_Clear"),				DwgPreview::Clear},
	{_T("DwgPreview_SetHighLight"),		DwgPreview::SetHighLight},
	{_T("DwgPreview_RemoveHighLight"),	DwgPreview::RemoveHighLight},

	// dwglist control methods 
	{_T("DwgList_Dir"),				DwgList::Dir},
	{_T("DwgList_GetDir"),			DwgList::GetDir},
	{_T("DwgList_GetFileName"),		DwgList::GetFileName},
	{_T("DwgList_GetType"),			DwgList::GetType},
	{_T("DwgList_GetText"),			DwgList::GetText},
	{_T("DwgList_GetSelectedItems"),	DwgList::GetSelectedItems},	
	{_T("DwgList_GetSelectedNths"),	DwgList::GetSelectedNths},	
	{_T("DwgList_GetCurSel"),		DwgList::GetCurSel},
	{_T("DwgList_SetCurSel"),		DwgList::SetCurSel},
	{_T("DwgList_GetCount"),			DwgList::GetCount},
	{_T("DwgList_GetTopIndex"),		DwgList::GetTopIndex},
	{_T("DwgList_SetTopIndex"),		DwgList::SetTopIndex},
	{_T("DwgList_DeleteString"),		DwgList::DeleteString},
	{_T("DwgList_SetSel"),			DwgList::SetSel},
	{_T("DwgList_GetSel"),			DwgList::GetSel},
	{_T("DwgList_SetFocusIndex"),	DwgList::SetFocusIndex},
	{_T("DwgList_GetFocusIndex"),	DwgList::GetFocusIndex},
	{_T("DwgList_GetSelCount"),		DwgList::GetSelCount},
	{_T("DwgList_SelItemRange"),		DwgList::SelItemRange},
	{_T("DwgList_SetAnchorIndex"),	DwgList::SetAnchorIndex},
	{_T("DwgList_GetAnchorIndex"),	DwgList::GetAnchorIndex},

	// optionlist control methods 
	{_T("OptionList_SetEnabled"),		OptionList::SetEnabled},
	{_T("OptionList_SetTttTitle"),		OptionList::SetTttTitle},
	{_T("OptionList_AddList"),		OptionList::AddList},
	{_T("OptionList_AddString"),		OptionList::AddString},
	{_T("OptionList_Clear"),			OptionList::Clear},
	{_T("OptionList_GetText"),		OptionList::GetText},
	{_T("OptionList_GetCurSel"),		OptionList::GetCurSel},
	{_T("OptionList_GetCount"),		OptionList::GetCount},
	{_T("OptionList_SetCurSel"),		OptionList::SetCurSel},
	{_T("OptionList_GetTopIndex"),   OptionList::GetTopIndex},
	{_T("OptionList_SetTopIndex"),   OptionList::SetTopIndex},
	{_T("OptionList_DeleteString"),  OptionList::DeleteString},
	{_T("OptionList_InsertString"),	OptionList::InsertString},

	// listbox control methods 
	{_T("ListBox_Dir"),				ListBox::Dir},
	{_T("ListBox_AddString"),		ListBox::AddString},
	{_T("ListBox_AddList"),			ListBox::AddList},		
	{_T("ListBox_Clear"),			ListBox::Clear},											
	{_T("ListBox_GetText"),			ListBox::GetText},
	{_T("ListBox_GetSelectedItems"),	ListBox::GetSelectedItems},	
	{_T("ListBox_GetSelectedNths"),	ListBox::GetSelectedNths},	
	{_T("ListBox_GetCurSel"),		ListBox::GetCurSel},
	{_T("ListBox_GetCount"),			ListBox::GetCount},
	{_T("ListBox_SetCurSel"),		ListBox::SetCurSel},
	{_T("ListBox_GetTopIndex"),		ListBox::GetTopIndex},
	{_T("ListBox_SetTopIndex"),		ListBox::SetTopIndex},
	{_T("ListBox_SetItemData"),		ListBox::SetItemData},
	{_T("ListBox_GetItemData"),		ListBox::GetItemData},
	{_T("ListBox_DeleteString"),		ListBox::DeleteString},
	{_T("ListBox_InsertString"),		ListBox::InsertString},
	{_T("ListBox_FindString"),		ListBox::FindString},
	{_T("ListBox_SelectString"),		ListBox::SelectString},
	{_T("ListBox_FindStringExact"),	ListBox::FindStringExact},
	{_T("ListBox_SetSel"),			ListBox::SetSel},
	{_T("ListBox_GetSel"),			ListBox::GetSel},
	{_T("ListBox_SetFocusIndex"),	ListBox::SetFocusIndex},
	{_T("ListBox_GetFocusIndex"),	ListBox::GetFocusIndex},
	{_T("ListBox_GetSelCount"),		ListBox::GetSelCount},
	{_T("ListBox_SelItemRange"),		ListBox::SelItemRange},
	{_T("ListBox_SetAnchorIndex"),	ListBox::SetAnchorIndex},
	{_T("ListBox_GetAnchorIndex"),	ListBox::GetAnchorIndex},

	// grid control methods 
	{_T("Grid_AddString"),			Grid::AddString},
	{_T("Grid_StartItemEdit"),	Grid::StartItemEdit},
	{_T("Grid_SetItemStyle"),		Grid::SetItemStyle},											
	{_T("Grid_SetCurSel"),			Grid::SetCurSel},
	{_T("Grid_HitPointTest"),		Grid::HitPointTest},
	{_T("Grid_SetItemDropList"),	Grid::SetItemDropList},
	{_T("Grid_AddColumns"),			Grid::AddColumns},
	{_T("Grid_AddRow"),				Grid::AddRow},
	{_T("Grid_CalcColWidth"),		Grid::CalcColWidth},
	{_T("Grid_Clear"),				Grid::Clear},
	{_T("Grid_DeleteColumn"),		Grid::DeleteColumn},
	{_T("Grid_DeleteRow"),			Grid::DeleteRow},
	{_T("Grid_FillList"),			Grid::FillList},
	{_T("Grid_GetColWidth"),			Grid::GetColWidth},
	{_T("Grid_GetItemData"),			Grid::GetItemData},
	{_T("Grid_GetItemImage"),		Grid::GetItemImage},
	{_T("Grid_GetItemText"),			Grid::GetItemText},
	{_T("Grid_GetCurSel"),		Grid::GetCurSel},	
	{_T("Grid_SetColWidth"),			Grid::SetColWidth},
	{_T("Grid_SetItemData"),			Grid::SetItemData},
	{_T("Grid_SetItemImage"),		Grid::SetItemImage},
	{_T("Grid_SetItemText"),			Grid::SetItemText},
	{_T("Grid_CancelItemEdit"),		Grid::CancelItemEdit},
	{_T("Grid_SortTextItems"),		Grid::SortTextItems},
	{_T("Grid_SortNumericItems"),	Grid::SortNumericItems},											
	{_T("Grid_GetRowItems"),				Grid::GetRowItems},
	{_T("Grid_GetColumnItems"),			Grid::GetColumnItems},
	{_T("Grid_GetColumnImage"),		Grid::GetColumnImage},
	{_T("Grid_SetColumnImage"),		Grid::SetColumnImage},
	{_T("Grid_GetRowCount"),			Grid::GetRowCount},
	{_T("Grid_GetColumnCount"),		Grid::GetColumnCount},
	{_T("Grid_GetItemCheck"),			Grid::GetItemCheck},
	{_T("Grid_SetItemCheck"),			Grid::SetItemCheck},
	{_T("Grid_InsertRow"),			Grid::InsertRow},
	{_T("Grid_InsertColumn"),			Grid::InsertColumn},
	{_T("Grid_InsertString"),		Grid::InsertString},

	// flex grid control methods
	//{_T("FlexGrid_GetColor"), GetFlexGridColorProperty},
	//{_T("FlexGrid_SetColor"), SetFlexGridColorProperty},

	// list view control methods 
	{_T("ListView_AddString"),		ListView::AddString},
	{_T("ListView_AddColumns"),		ListView::AddColumns},
	{_T("ListView_AddItem"),			ListView::AddItem},
	{_T("ListView_Arrange"),			ListView::Arrange},
	{_T("ListView_CalcColWidth"), ListView::CalcColWidth},
	{_T("ListView_Clear"),			ListView::Clear},
	{_T("ListView_CountItems"),		ListView::CountItems},
	{_T("ListView_DeleteItem"),	ListView::DeleteItem},
	{_T("ListView_DeleteItems"),	ListView::DeleteItems},
	{_T("ListView_DeleteColumn"),	ListView::DeleteColumn},
	{_T("ListView_DeleteColumns"),	ListView::DeleteColumns},
	{_T("ListView_FillList"),		ListView::FillList},
	{_T("ListView_GetColWidth"),		ListView::GetColWidth},
	{_T("ListView_GetItemData"),		ListView::GetItemData},
	{_T("ListView_GetItemImage"),	ListView::GetItemImage},
	{_T("ListView_GetItemText"),		ListView::GetItemText},
	{_T("ListView_GetSelCount"),ListView::GetSelCount},
	{_T("ListView_GetSelectedItems"),ListView::GetSelectedItems},	
	{_T("ListView_GetSelectedNths"),	ListView::GetSelectedNths},
	{_T("ListView_HitPointTest"),    ListView::HitPointTest},
	{_T("ListView_InsertItem"),		ListView::InsertItem},
	{_T("ListView_SetColWidth"),		ListView::SetColWidth},
	{_T("ListView_GetCurSel"),		ListView::GetCurSel},
	{_T("ListView_SetCurSel"),		ListView::SetCurSel},
	{_T("ListView_SetItemData"),		ListView::SetItemData},
	{_T("ListView_SetItemImage"),	ListView::SetItemImage},
	{_T("ListView_SetItemText"),		ListView::SetItemText},
	{_T("ListView_StartLabelEdit"),	ListView::StartLabelEdit},
	{_T("ListView_CancelLabelEdit"),	ListView::CancelLabelEdit},
	{_T("ListView_SortTextItems"),	ListView::SortTextItems},
	{_T("ListView_SortNumericItems"),ListView::SortNumericItems},
	{_T("ListView_GetRowItems"),			ListView::GetRowItems},
	{_T("ListView_GetColumnItems"),		ListView::GetColumnItems},
	{_T("ListView_GetColumnImage"),	ListView::GetColumnImage},
	{_T("ListView_SetColumnImage"),	ListView::SetColumnImage},
	{_T("ListView_GetCount"),		ListView::GetCount},
	{_T("ListView_GetColumnCount"),	ListView::GetColumnCount},

	// blocklist control methods 
	{_T("BlockList_LoadDwg"),			BlockList::LoadDwg},
	{_T("BlockList_Reset"),			BlockList::Reset},											
	{_T("BlockList_GetFileName"),		BlockList::GetFileName},

	// combo box control methods 
	{_T("ComboBox_Dir"),					ComboBox::Dir},
	{_T("ComboBox_GetDir"),				ComboBox::GetDir},
	{_T("ComboBox_AddPath"),				ComboBox::AddPath},											
	{_T("ComboBox_AddColor"),			ComboBox::AddColor},
	{_T("ComboBox_FindColor"),			ComboBox::FindColor},
	{_T("ComboBox_FindLineWeight"),		ComboBox::FindLineWeight},
	{_T("ComboBox_AddString"),			ComboBox::AddString},
	{_T("ComboBox_AddList"),				ComboBox::AddList},
	{_T("ComboBox_GetCurSel"),			ComboBox::GetCurSel},
	{_T("ComboBox_GetCount"),			ComboBox::GetCount},
	{_T("ComboBox_Clear"),				ComboBox::Clear},
	{_T("ComboBox_DeleteString"),		ComboBox::DeleteString},
	{_T("ComboBox_InsertString"),		ComboBox::InsertString},
	{_T("ComboBox_FindString"),			ComboBox::FindString},											
	{_T("ComboBox_FindStringExact"),		ComboBox::FindStringExact},
	{_T("ComboBox_SelectString"),		ComboBox::SelectString},
	{_T("ComboBox_SetCurSel"),			ComboBox::SetCurSel},
	{_T("ComboBox_GetEditSel"),			ComboBox::GetEditSel},
	{_T("ComboBox_SetEditSel"),			ComboBox::SetEditSel},
	{_T("ComboBox_SetItemData"),			ComboBox::SetItemData},
	{_T("ComboBox_GetItemData"),			ComboBox::GetItemData},
	{_T("ComboBox_GetTopIndex"),			ComboBox::GetTopIndex},
	{_T("ComboBox_SetTopIndex"),			ComboBox::SetTopIndex},
	{_T("ComboBox_GetDroppedWidth"),		ComboBox::GetDroppedWidth},
	{_T("ComboBox_SetDroppedWidth"),		ComboBox::SetDroppedWidth},
	{_T("ComboBox_ClearEdit"),			ComboBox::ClearEdit},
	{_T("ComboBox_GetLBText"),			ComboBox::GetLBText},
	{_T("ComboBox_GetTBText"),			ComboBox::GetEBText},

	{_T("ImageComboBox_AddString"),			ImageComboBox::AddString},
	{_T("ImageComboBox_SetItem"),			ImageComboBox::SetItem},
	{_T("ImageComboBox_GetItem"),			ImageComboBox::GetItem},
	{_T("ImageComboBox_GetCurSel"),			ImageComboBox::GetCurSel},
	{_T("ImageComboBox_GetCount"),			ImageComboBox::GetCount},
	{_T("ImageComboBox_Clear"),				ImageComboBox::Clear},
	{_T("ImageComboBox_DeleteString"),		ImageComboBox::DeleteString},
	{_T("ImageComboBox_FindString"),		ImageComboBox::FindString},											
	{_T("ImageComboBox_FindStringExact"),	ImageComboBox::FindStringExact},
	{_T("ImageComboBox_SelectString"),		ImageComboBox::SelectString},
	{_T("ImageComboBox_SetCurSel"),			ImageComboBox::SetCurSel},
	{_T("ImageComboBox_GetEditSel"),		ImageComboBox::GetEditSel},
	{_T("ImageComboBox_SetEditSel"),		ImageComboBox::SetEditSel},
	{_T("ImageComboBox_SetItemData"),		ImageComboBox::SetItemData},
	{_T("ImageComboBox_GetItemData"),		ImageComboBox::GetItemData},
	{_T("ImageComboBox_GetTopIndex"),		ImageComboBox::GetTopIndex},
	{_T("ImageComboBox_SetTopIndex"),		ImageComboBox::SetTopIndex},
	{_T("ImageComboBox_GetDroppedWidth"),	ImageComboBox::GetDroppedWidth},
	{_T("ImageComboBox_SetDroppedWidth"),	ImageComboBox::SetDroppedWidth},
	{_T("ImageComboBox_ClearEdit"),			ImageComboBox::ClearEdit},
	{_T("ImageComboBox_GetLBText"),			ImageComboBox::GetLBText},
	{_T("ImageComboBox_GetEBText"),			ImageComboBox::GetEBText},

	// month control methods 
	{_T("Month_SetCurSel"),				Month::SetCurSel},
	{_T("Month_GetCurSel"),				Month::GetCurSel},
	{_T("Month_SetRange"),				Month::SetRange},
	{_T("Month_GetRangeStart"),			Month::GetRangeStart},
	{_T("Month_GetRangeEnd"),			Month::GetRangeEnd},
	{_T("Month_GetMonthRangeStart"),		Month::GetMonthRangeStart},
	{_T("Month_GetMonthRangeEnd"),		Month::GetMonthRangeEnd},
	{_T("Month_SetSelRange"),			Month::SetSelRange},
	{_T("Month_GetSelRangeStart"),		Month::GetSelRangeStart},
	{_T("Month_GetSelRangeEnd"),			Month::GetSelRangeEnd},
	{_T("Month_GetToday"),				Month::GetToday},

	// slide view control methods 
	{_T("SlideView_Load"),		SlideView::Load},
	{_T("SlideView_Clear"),				SlideView::Clear},
	{_T("SlideView_SetHighLight"),		SlideView::SetHighLight},
	{_T("SlideView_RemoveHighLight"),	SlideView::RemoveHighLight},
	{_T("SlideView_VectorImage"),		SlideView::VectorImage},
	{_T("SlideView_FillImage"),			SlideView::FillImage},
	{_T("SlideView_SlideImage"),			SlideView::SlideImage},											
	{_T("SlideView_EndImage"),			SlideView::EndImage},

	// picture box control methods 
	{_T("PictureBox_StoreImage"),		PictureBox::StoreImage},
	{_T("PictureBox_Clear"),				PictureBox::Clear},
	{_T("PictureBox_Refresh"),			PictureBox::Refresh},
	{_T("PictureBox_DrawLine"),			PictureBox::DrawLine},
	{_T("PictureBox_DrawArc"),			PictureBox::DrawArc},
	{_T("PictureBox_DrawCircle"),		PictureBox::DrawCircle},
	{_T("PictureBox_DrawFillRect"),		PictureBox::DrawFillRect},
	{_T("PictureBox_DrawEdge"),			PictureBox::DrawEdge},
	{_T("PictureBox_DrawHatchRect"),		PictureBox::DrawHatchRect},
	{_T("PictureBox_DrawWrappedText"),	PictureBox::DrawWrappedText},
	{_T("PictureBox_DrawText"),			PictureBox::DrawText},
	{_T("PictureBox_GetTextExtent"),		PictureBox::GetTextExtent},
	{_T("PictureBox_PaintPicture"),		PictureBox::PaintPicture},
	{_T("PictureBox_DrawPoint"),			PictureBox::DrawPoint},
	{_T("PictureBox_LoadPictureFile"),	PictureBox::LoadPictureFile},
	{_T("PictureBox_DrawFocusRect"),		PictureBox::DrawFocusRect},
	{_T("PictureBox_DrawRect"),			PictureBox::DrawRect},

	// html control methods
	{_T("Html_Navigate"),		Html::Navigate},
	{_T("Html_Stop"),			Html::Stop},
	{_T("Html_Refresh"),			Html::Refresh},
	{_T("Html_GoBack"),			Html::GoBack},
	{_T("Html_GoForward"),		Html::GoForward},
	{_T("Html_GoHome"),			Html::GoHome},
	{_T("Html_GoSearch"),		Html::GoSearch},
	{_T("Html_GetLocationName"),	Html::GetLocationName},
	{_T("Html_GetLocationURL"),	Html::GetLocationURL},
	{_T("Html_GetOffline"),		Html::GetOffline},
	{_T("Html_SetOffline"),		Html::SetOffline},
	{_T("Html_GetBusy"),			Html::GetBusy},
	{_T("Html_GetFullName"),		Html::GetFullName},
	{_T("Html_UpdateHtmlCode"),	Html::UpdateHtmlCode},
	{_T("Html_ReplaceText"),		Html::ReplaceText},
	{_T("Html_GetHtmlDocument"),	Html::GetHtmlDocument},

	// tree control methods 
	{_T("Tree_IsItemExpanded"),			Tree::IsItemExpanded},
	{_T("Tree_AddParent"),				Tree::AddParent},
	{_T("Tree_AddChild"),				Tree::AddChild},
	{_T("Tree_InsertAfter"),				Tree::InsertAfter},
	{_T("Tree_GetParent"),				Tree::GetParent},
	{_T("Tree_Clear"),					Tree::Clear},
	{_T("Tree_CountItems"),				Tree::CountItems},
	{_T("Tree_SelectItem"),				Tree::SelectItem},
	{_T("Tree_ItemHasChildren"),			Tree::ItemHasChildren},
	{_T("Tree_GetNextSiblingItem"),		Tree::GetNextSiblingItem},
	{_T("Tree_GetPrevSiblingItem"),		Tree::GetPrevSiblingItem},
	{_T("Tree_GetFirstVisibleItem"),		Tree::GetFirstVisibleItem},
	{_T("Tree_GetNextVisibleItem"),		Tree::GetNextVisibleItem},
	{_T("Tree_GetPrevVisibleItem"),		Tree::GetPrevVisibleItem},
	{_T("Tree_GetSelectedItem"),			Tree::GetSelectedItem},
	{_T("Tree_GetRootItem"),				Tree::GetRootItem},
	{_T("Tree_GetItem"),					Tree::GetItem},
	{_T("Tree_SetItemImages"),			Tree::SetItemImages},
	{_T("Tree_GetItemImages"),			Tree::GetItemImages},
	{_T("Tree_GetItemText"),				Tree::GetItemText},
	{_T("Tree_SetItemText"),				Tree::SetItemText},
	{_T("Tree_GetItemData"),				Tree::GetItemData},
	{_T("Tree_SetItemData"),				Tree::SetItemData},
	{_T("Tree_GetVisibleCount"),			Tree::GetVisibleCount},
	{_T("Tree_DeleteItem"),				Tree::DeleteItem},
	{_T("Tree_ExpandItem"),				Tree::ExpandItem},
	{_T("Tree_SelectSetFirstVisible"),	Tree::SelectSetFirstVisible},
	{_T("Tree_EditLabel"),				Tree::EditLabel},
	{_T("Tree_SortChildren"),			Tree::SortChildren},
	{_T("Tree_EnsureVisible"),			Tree::EnsureVisible},
	{_T("Tree_GetFirstChildItem"),		Tree::GetFirstChildItem},
	{_T("Tree_SetExpandedImage"),			Tree::SetExpandedImage},
	{_T("Tree_GetExpandedImage"),			Tree::GetExpandedImage},
	{_T("Tree_CancelEditLabel"),			Tree::CancelEditLabel},

	// tab control methods 
	{_T("Tab_SetCurSel"),			Tab::SetCurSel},
	{_T("Tab_GetCurSel"),			Tab::GetCurSel},
	{_T("Tab_GetRowCount"),		Tab::GetRowCount},
	{_T("Tab_SetTabText"),		Tab::SetTabText},
	{_T("Tab_HideTab"),			Tab::HideTab},
	{_T("Tab_ShowTab"),			Tab::ShowTab},
	{_T("Tab_GetControlArea"),	Tab::GetControlArea},

	// text box control methods 
	{_T("TextBox_GetFilter"),			TextBox::GetFilter},
	{_T("TextBox_SetFilter"),			TextBox::SetFilter},
	{_T("TextBox_GetLineCount"),			TextBox::GetLineCount},
	{_T("TextBox_GetModify"),			TextBox::GetModify},
	{_T("TextBox_GetSel"),				TextBox::GetSel},
	{_T("TextBox_GetLine"),				TextBox::GetLine},
	{_T("TextBox_GetFirstVisibleLine"),	TextBox::GetFirstVisibleLine},
	{_T("TextBox_GetLineLength"),		TextBox::GetLineLength},
	{_T("TextBox_LineScroll"),			TextBox::LineScroll},
	{_T("TextBox_ReplaceSel"),			TextBox::ReplaceSel},
	{_T("TextBox_SetSel"),				TextBox::SetSel},
	{_T("TextBox_SetTabStops"),			TextBox::SetTabStops},
	{_T("TextBox_Undo"),					TextBox::Undo},
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


//-----------------------------------------------------------------------------
//----- ObjectARX EntryPoint
class CARXApp : public AcRxArxApp {

protected:
	T_PropertyIdSet mPropIds; //store the property ids used to define get/set lisp functions

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
				LPCTSTR pszPropName = GetPropertyName( (Prop::Id)(nFunctionCode - ADSPROPFUNCBASE) );
				if( pszPropName && *pszPropName != _T('(') )
					return (controlGetPrefix() + pszPropName);
			}
			else if( nFunctionCode >= (ADSPROPFUNCBASE + Prop::_MaxId) &&
							 nFunctionCode < (ADSPROPFUNCBASE + Prop::_MaxId + Prop::_MaxId) )
			{
				LPCTSTR pszPropName = GetPropertyName( (Prop::Id)(nFunctionCode - (ADSPROPFUNCBASE + Prop::_MaxId)) );
				if( pszPropName && *pszPropName != _T('(') )
					return (controlSetPrefix() + pszPropName);
			}
			else
			{
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
		AcRx::AppRetCode retCode =AcRxArxApp::On_kInvkSubrMsg (pkt) ;

		assert( retCode == AcRx::kRetOK );
		if( retCode == AcRx::kRetOK )
		{
			acedRetNil();
			int nFunctionCode = acedGetFunCode();
			if( nFunctionCode >= 0 && nFunctionCode < _elements(grAdsFunctionTable) )
			{
				if( RTNORM != grAdsFunctionTable[nFunctionCode].pfHandler() )
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
				LPCTSTR pszPropName = GetPropertyName( id );
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
			acDocManager->sendStringToExecute(pDoc, sToSend, false, true, false);
		}

		return (RSRSLT) ;
	}

	// ----- ads_dcl_invokefunc symbol (do not rename)
	static int ads_dcl_invokefunc(void)
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
		case CtlStdButton:
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

		TDclControlPtr pCtrl = mpDclToBeShown->FindControl(pszKey);
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

		TDclControlPtr pCtrl = mpDclToBeShown->FindControl(pszKey);
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

		TDclControlPtr pCtrl = mpDclToBeShown->FindControl(pszKey);
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

		TDclControlPtr pCtrl = mpDclToBeShown->FindControl(pszKey);
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
				theWorkspace.DisplayAlert(CString(ErrorFileNotFound) + "\n" + sFilename);
				acedRetInt(-1);
				return RSRSLT;
			}
			((CSlideHolder*)pCtrl->GetWindow())->DrawASlide(nX, nY, nWidth, nHeight, sPath, NULL);
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
		struct resbuf *pArgs =acedGetArgs () ;

		long newLW = AcDb::kLnWtByLwDefault;
		GetLongArgument( pArgs, newLW, true );

		bool bIncludeMetaTypes = true;
		GetBoolArgument( pArgs, bIncludeMetaTypes, true );

		if( !AssertOutOfArgs( pArgs ) )
			return RSERR;

		if( SelectLineWeightUI( (AcDb::LineWeight&)newLW, bIncludeMetaTypes ) )
			acedRetLong( newLW );

		return (RSRSLT) ;
	}

	// ----- ads_dcl_form_show symbol (do not rename)
	static int ads_dcl_form_show(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		TDclFormPtr pDclForm;
		if( !GetFormArgument( pArgs, pDclForm ) )
			return RSERR; //invalid input

		//optional arguments
		int nX = -1;
		int nY = -1;
		CString sDefaultDirectory;
		CString sDefaultFileName;
		CString sDefaultExtension;
		if( GetStringArgument( pArgs, sDefaultDirectory, true ) )
		{
			if( !GetStringArgument( pArgs, sDefaultFileName ) )
				return RSERR; //invalid input
			if( !GetStringArgument( pArgs, sDefaultExtension ) )
				return RSERR; //invalid input
		}
		else if( GetIntArgument( pArgs, nX, true ) )
		{
			if( !GetIntArgument( pArgs, nY ) )
				return RSERR; //invalid input
		}

		if( !AssertOutOfArgs( pArgs ) )
			return RSERR;

		if( !pDclForm )
			return RSRSLT; //form not found

		// call method to display the requested form
		FileDialogParams fdp( TRUE, NULL, NULL, 0, NULL );
		bool bHasFileParams = (pDclForm->GetType() == VdclFileDialog);
		if( bHasFileParams )
		{
			CString sFilename = sDefaultDirectory;
			if( !sFilename.IsEmpty() && !sFilename.Right(1).SpanExcluding( _T("\\/") ).IsEmpty() )
				sFilename += _T('\\');
			sFilename += sDefaultFileName;
			fdp.sFilename = sFilename;
			fdp.sDefaultExtension = sDefaultExtension;
		}
		DialogParams params( CPoint( nX, nY ), CRect(0,0,0,0), bHasFileParams? (LPARAM)&fdp : NULL );
		int nResult = theArxWorkspace.ActivateDclForm(pDclForm, &params);

		if (nResult >= 0)
		{
			if (nResult == IDOK && pDclForm->GetType() == VdclFileDialog)
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

		CDialogObject* pDlgObject = NULL;
		if( !GetDialogArgument( pArgs, pDlgObject ) )
			return RSERR; //arguments expected

		//optional arguments
		int nX = -1;
		int nY = -1;
		if( GetIntArgument( pArgs, nX, true ) )
		{
			if( !GetIntArgument( pArgs, nY ) )
				return RSERR; //invalid input
		}

		if( !AssertOutOfArgs( pArgs ) )
			return RSERR;

		if( !pDlgObject )
			return RSRSLT; //dialog not found

		if( nX <= 0 || nY <= 0 )
		{
			if( pDlgObject->CenterDialog() )
				acedRetT();
		}
		else
		{
			if( pDlgObject->CenterAndResizeDialog( nX, nY ) )
				acedRetT();
		}

		return (RSRSLT) ;
	}

	// ----- ads_dcl_form_resize symbol (do not rename)
	static int ads_dcl_form_resize(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		CDialogObject* pDlgObject = NULL;
		if( !GetDialogArgument( pArgs, pDlgObject ) )
			return RSERR; //arguments expected

		//optional arguments
		int nNewWidth = -1;
		int nNewHeight = -1;
		if( GetIntArgument( pArgs, nNewWidth, true ) )
		{
			if( !GetIntArgument( pArgs, nNewHeight ) )
				return RSERR; //invalid input
		}

		if( !AssertOutOfArgs( pArgs ) )
			return RSERR;

		if( !pDlgObject )
			return RSRSLT; //dialog not found

		if( pDlgObject->ResizeDialog( nNewWidth, nNewHeight ) )
			acedRetT();

		return (RSRSLT) ;
	}

	// ----- ads_dcl_form_setfocus symbol (do not rename)
	static int ads_dcl_form_setfocus(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		CDialogObject* pDlgObject = NULL;
		if( !GetDialogArgument( pArgs, pDlgObject ) )
			return RSERR; //arguments expected

		if( !AssertOutOfArgs( pArgs ) )
			return RSERR;

		if( !pDlgObject )
			return RSRSLT; //dialog not found

		if( pDlgObject->Focus() )
			acedRetT();

		return (RSRSLT) ;
	}

	// ----- ads_dcl_form_hide symbol (do not rename)
	static int ads_dcl_form_hide(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		CDialogObject* pDlgObject = NULL;
		if( !GetDialogArgument( pArgs, pDlgObject ) )
			return RSERR; //arguments expected

		//optional arguments
		bool bHide = true;
		GetBoolArgument( pArgs, bHide, true );

		if( !AssertOutOfArgs( pArgs ) )
			return RSERR;

		if( !pDlgObject )
			return RSRSLT; //dialog not found

		if( pDlgObject->Show( !bHide ) )
			acedRetT();

		return (RSRSLT) ;
	}

	// ----- ads_dcl_form_close symbol (do not rename)
	static int ads_dcl_form_close(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		CDialogObject* pDlgObject = NULL;
		if( !GetDialogArgument( pArgs, pDlgObject ) )
			return RSERR; //arguments expected

		//optional arguments
		int nStatus = 0;
		GetIntArgument( pArgs, nStatus, true );

		if( !AssertOutOfArgs( pArgs ) )
			return RSERR;

		if( !pDlgObject )
			return RSRSLT; //dialog not found

		CPoint ptXlate( 0, 0 );
		pDlgObject->GetControlWnd()->ClientToScreen( &ptXlate );
		ads_point ptUL = { ptXlate.x, ptXlate.y, 0 };
		acedRetPoint( ptUL ); //return the upper left corner as a 2D point

		pDlgObject->CloseDialog( nStatus );

		return (RSRSLT) ;
	}

	// ----- ads_dcl_form_getrectangle symbol (do not rename)
	static int ads_dcl_form_getrectangle(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		CDialogObject* pDlgObject = NULL;
		if( !GetDialogArgument( pArgs, pDlgObject ) )
			return RSERR; //arguments expected

		if( !AssertOutOfArgs( pArgs ) )
			return RSERR;

		if( !pDlgObject )
			return RSRSLT; //dialog not found

		CRect rcDlg = pDlgObject->GetEffectiveWindowRect();
		if( !rcDlg.IsRectNull() )
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

		TDclFormPtr pDclForm;
		if( !GetFormArgument( pArgs, pDclForm ) )
			return RSERR; //invalid input

		if( !AssertOutOfArgs( pArgs ) )
			return RSERR;

		if( !pDclForm )
			return RSRSLT; //dialog not found

		TDclControlPtr pPropObj = pDclForm->GetControlProperties();
		assert( pPropObj != NULL );

		resbuf rbHeight = {NULL, RTSHORT};
		rbHeight.resval.rint = pPropObj->GetLongProperty( Prop::Height );
		resbuf rbWidth = {&rbHeight, RTSHORT};
		rbWidth.resval.rint = pPropObj->GetLongProperty( Prop::Width );
		acedRetList(&rbWidth);		

		return (RSRSLT) ;
	}

	// ----- ads_dcl_form_setpos symbol (do not rename)
	static int ads_dcl_form_setpos(void)
	{
		return Control::SetPos();
	}

	// ----- ads_dcl_form_closeall symbol (do not rename)
	static int ads_dcl_form_closeall(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		//optional arguments
		UINT fMask = -1;
		GetUIntArgument( pArgs, fMask, true );

		if( !AssertOutOfArgs( pArgs ) )
			return RSERR;

		theArxWorkspace.CloseAllDialogs( fMask );

		return (RSRSLT) ;
	}

	// ----- ads_dcl_form_isfloating symbol (do not rename)
	static int ads_dcl_form_isfloating(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		CDialogObject* pDlgObject = NULL;
		if( !GetDialogArgument( pArgs, pDlgObject ) )
			return RSERR; //arguments expected

		if( !AssertOutOfArgs( pArgs ) )
			return RSERR;

		if( !pDlgObject )
			return RSRSLT; //dialog not found

		if( pDlgObject->IsFloating() )
			acedRetT();

		return (RSRSLT) ;
	}

	// ----- ads_dcl_form_isactive symbol (do not rename)
	static int ads_dcl_form_isactive(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		if( GetNilArgument( pArgs, true ) )
			return RSRSLT; //accept a NIL argument and return quietly

		TDclFormPtr pDclForm;
		if( !GetFormArgument( pArgs, pDclForm ) )
			return RSERR; //arguments expected

		if( !AssertOutOfArgs( pArgs ) )
			return RSERR;

		if( !pDclForm )
			return RSRSLT; //dialog not found

		if( pDclForm->GetFormInstance() )
			acedRetT();

		return (RSRSLT) ;
	}

	// ----- ads_dcl_configtab_isapplyenabled symbol (do not rename)
	static int ads_dcl_configtab_isapplyenabled(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		CDialogObject* pDlgObject = NULL;
		if( !GetDialogArgument( pArgs, pDlgObject ) )
			return RSERR; //arguments expected

		if( !AssertOutOfArgs( pArgs ) )
			return RSERR;

		if( !pDlgObject )
			return RSRSLT; //dialog not found

		if( pDlgObject->IsDirty() )
			acedRetT();

		return (RSRSLT) ;
	}

	// ----- ads_dcl_configtab_setapplyenabled symbol (do not rename)
	static int ads_dcl_configtab_setapplyenabled(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		CDialogObject* pDlgObject = NULL;
		if( !GetDialogArgument( pArgs, pDlgObject ) )
			return RSERR; //arguments expected

		if( !AssertOutOfArgs( pArgs ) )
			return RSERR;

		if( !pDlgObject )
			return RSRSLT; //dialog not found

		if( pDlgObject->SetDirty() )
			acedRetT();

		return (RSRSLT) ;
	}

	// ----- ads_dcl_getscreensize symbol (do not rename)
	static int ads_dcl_getscreensize(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		if( !AssertOutOfArgs( pArgs ) )
			return RSERR;

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
		if( !GetCtrlProperty( Prop::TitleBarText ) )
			return RSERR;

		return (RSRSLT) ;
	}

	// ----- ads_dcl_form_settitlebartext symbol (do not rename)
	static int ads_dcl_form_settitlebartext(void)
	{
		if( !SetCtrlProperty( Prop::TitleBarText ) )
			return RSERR;

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

	// ----- ads_dcl_form_setdialogminmaxsizes symbol (do not rename)
	static int ads_dcl_form_setdialogminmaxsizes(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		TDclControlPtr pControl;
		if( !GetControlArgument( pArgs, pControl ) )
			return RSERR; //invalid input

		int nMinWidth = 0;
		if( !GetIntArgument( pArgs, nMinWidth ) )
			return RSERR; //invalid input

		int nMinHeight = 0;
		if( !GetIntArgument( pArgs, nMinWidth ) )
			return RSERR; //invalid input

		int nMaxWidth = 0;
		if( !GetIntArgument( pArgs, nMinWidth ) )
			return RSERR; //invalid input

		int nMaxHeight = 0;
		if( !GetIntArgument( pArgs, nMinWidth ) )
			return RSERR; //invalid input

		if( !AssertOutOfArgs( pArgs ) )
			return RSERR;

		bool bFailed = false;
		if( !pControl->SetLongProperty( Prop::MinDialogWidth, nMinWidth ) )
			bFailed = true;
		if( !pControl->SetLongProperty( Prop::MinDialogHeight, nMinHeight ) )
			bFailed = true;
		if( !pControl->SetLongProperty( Prop::MaxDialogWidth, nMaxWidth ) )
			bFailed = true;
		if( !pControl->SetLongProperty( Prop::MaxDialogHeight, nMaxHeight ) )
			bFailed = true;
		CArxDialogControl::UpdateProperty( pControl, Prop::MaxDialogHeight );

		if( !bFailed )
			acedRetT();

		return (RSRSLT) ;
	}

	// ----- ads_dcl_form_gethwnd symbol (do not rename)
	static int ads_dcl_form_gethwnd(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		CDialogObject* pDlgObject = NULL;
		if( !GetDialogArgument( pArgs, pDlgObject ) )
			return RSERR; //arguments expected

		if( !AssertOutOfArgs( pArgs ) )
			return RSERR;

		if( !pDlgObject )
			return RSRSLT; //dialog not found

		acedRetHandle( (DWORD_PTR)pDlgObject->GetHWnd() );

		return (RSRSLT) ;
	}

	// ----- ads_dcl_form_isenabled symbol (do not rename)
	static int ads_dcl_form_isenabled(void)
	{
		if( !GetCtrlProperty( Prop::Enabled ) )
			return RSERR;

		return (RSRSLT) ;
	}

	// ----- ads_dcl_form_isvisible symbol (do not rename)
	static int ads_dcl_form_isvisible(void)
	{
		if( !GetCtrlProperty( Prop::Visible ) )
			return RSERR;

		return (RSRSLT) ;
	}

	// ----- ads_dcl_form_enable symbol (do not rename)
	static int ads_dcl_form_enable(void)
	{
		if( !SetCtrlProperty( Prop::Enabled ) )
			return RSERR;

		return (RSRSLT) ;
	}

	// ----- ads_dcl_getharddrivesize symbol (do not rename)
	static int ads_dcl_getharddrivesize(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		CString sDrive;
		if( !GetStringArgument( pArgs, sDrive ) )
			return RSERR; //nvalid argument

		if( !AssertOutOfArgs( pArgs ) )
			return RSERR;

		if( sDrive.IsEmpty() )
			return RSERR; //i

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

		if( !AssertOutOfArgs( pArgs ) )
			return RSERR;

		theWorkspace.SetMessagesSuppressed();

		return (RSRSLT) ;
	}

	// ----- ads_dcl_showerrormsgbox symbol (do not rename)
	static int ads_dcl_showerrormsgbox(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		if( !AssertOutOfArgs( pArgs ) )
			return RSERR;

		theWorkspace.SetMessagesSuppressed(false);

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

		CDialogControl* pDlgControl;
		if( !GetDlgControlArgument( pArgs, pDlgControl, _CtlInvalid ) )
			return RSERR; //invalid argument

		if( !AssertOutOfArgs( pArgs ) )
			return RSERR;

		acedRetHandle( (DWORD_PTR)pDlgControl->GetHWnd() );

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

		return (RSRSLT) ;
	}

	// ----- ads_dcl_browsefolder symbol (do not rename)
	static int ads_dcl_browsefolder(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		CString sCaption;
		if( !GetStringArgument( pArgs, sCaption ) )
			return RSERR; //invalid argument

		//optional arguments
		CString sInitialFolder;
		CString sRootFolder;
		UINT nFlags = BIF_RETURNONLYFSDIRS;
		if( GetStringArgument( pArgs, sInitialFolder, true ) )
		{
			if( GetStringArgument( pArgs, sRootFolder, true ) )
				GetUIntArgument( pArgs, nFlags, true );
		}

		if( !AssertOutOfArgs( pArgs ) )
			return RSERR;

		if( sCaption.IsEmpty() )
			return RSERR; //invalid argument

		CDirDialog dlg( sCaption, sInitialFolder, sRootFolder, nFlags );
		if (dlg.DoBrowse(CWnd::FromHandle(theArxWorkspace.GetTopmostModalForm())))
			acedRetStr(dlg.GetSelectedFolder());

		return (RSRSLT) ;
	}

	// ----- ads_dcl_insert symbol (do not rename)
	static int ads_dcl_insert(void)
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
		CString sFilterList;
		CString sCaption;
		CString sPath;
		CStringArray rsFilters;
		if( GetStringArrayArgument( pArgs, rsFilters, true ) )
		{
			for( INT_PTR idx = 0; idx < rsFilters.GetCount(); ++idx )
			{
				sFilterList += rsFilters.GetAt( idx );
				if( !sFilterList.IsEmpty() && sFilterList.Right( 1 ) != _T("|") )
					sFilterList += _T('|');
			}
			if( GetStringArgument( pArgs, sCaption, true ) )
				GetStringArgument( pArgs, sPath, true );
		}
		else
			sFilterList = _T("AutoCAD Drawing (*.dwg)|*.dwg|");

		if( !AssertOutOfArgs( pArgs ) )
			return RSERR;

		CWnd *pParent = CWnd::FromHandle(theArxWorkspace.GetTopmostModalForm());
		DWORD dwFlags = OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING |
										OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
		// create the open dialog box
		CFileDialog BrowseWnd(TRUE, NULL, NULL, dwFlags, sFilterList, pParent);
		BrowseWnd.m_ofn.lpstrInitialDir = sPath.LockBuffer();
		BrowseWnd.m_ofn.lpstrTitle = sCaption.LockBuffer();
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

	// ----- ads_dcl_flushgraphicbuttons symbol (do not rename)
	static int ads_dcl_flushgraphicbuttons(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		CDialogObject* pDlgObject = NULL;
		if( !GetDialogArgument( pArgs, pDlgObject ) )
			return RSERR; //arguments expected

		if( !AssertOutOfArgs( pArgs ) )
			return RSERR;

		if( !pDlgObject )
			return RSRSLT; //dialog not found

		pDlgObject->GetControlPane()->RecalcLayout();

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

		CDC dc;
		acedRetLong (MulDiv (GetDeviceCaps (dc, LOGPIXELSX), twips, 1440));
		dc.DeleteDC();

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

		CDC dc;
		acedRetLong (MulDiv (GetDeviceCaps (dc, LOGPIXELSY), twips, 1440));
		dc.DeleteDC();

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

		CDC dc;
		acedRetLong (MulDiv (pixels, 1440, GetDeviceCaps (dc, LOGPIXELSX)));
		dc.DeleteDC();

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

		CDC dc;
		acedRetLong (MulDiv (pixels, 1440, GetDeviceCaps (dc, LOGPIXELSY)));
		dc.DeleteDC();

		return (RSRSLT) ;
	}

	// ----- ads_dcl_getcolorvalue symbol (do not rename)
	static int ads_dcl_getcolorvalue(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		COLORREF crArg;
		if( !GetColorArgument( pArgs, crArg ) )
			return RSERR; //invalid input

		if( !AssertOutOfArgs( pArgs ) )
			return RSERR;

		acedRetLong( crArg );

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

	// ----- ads_dcl_files_dir symbol (do not rename)
	static int ads_dcl_files_dir(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		CString sPath;
		if( !GetStringArgument( pArgs, sPath ) )
			return RSERR; //invalid input

		CString sFilter = _T("*.*");
		GetStringArgument( pArgs, sFilter, true );

		if( !AssertOutOfArgs( pArgs ) )
			return RSERR;

		if( sPath.Right( 1 ) != _T("\\") )
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
				prbTail = acutNewRb( RTSTR );
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
		CStringArray rsRawData;
		if( !GetStringArrayArgument( pArgs, rsRawData ) )
			return RSERR; //wrong argument type

		for( INT_PTR idx = 0; idx < rsRawData.GetCount(); ++idx )
			sRawDataIn += rsRawData.GetAt( idx );

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
			prbTail = prbProjects;
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
			prbForm->resval.rlname[0] = (LONG_PTR)(const CDclFormObject*)(*iter);
			prbForm->resval.rlname[1] = 0;
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

	// ----- ads_dcl_form_getcontrols symbol (do not rename)
	static int ads_dcl_form_getcontrols(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		TDclFormPtr pDclForm;
		if( !GetFormArgument( pArgs, pDclForm ) )
			return RSERR; //invalid input

		if( !AssertOutOfArgs( pArgs ) )
			return RSERR;

		if( !pDclForm )
			return RSRSLT; //form not found

		resbuf* prbControls = NULL;
		resbuf* prbTail = NULL;
		const TDclControlList& Controls = pDclForm->GetControlList();
		for( TDclControlList::const_iterator iter = Controls.begin(); iter != Controls.end(); ++iter )
		{
			resbuf* prbControl = acutNewRb( RTENAME );
			prbControl->resval.rlname[0] = (LONG_PTR)(const CDclControlObject*)(*iter);
			prbControl->resval.rlname[1] = 0;
			if( prbTail )
				prbTail->rbnext = prbControl;
			else
				prbControls = prbControl;
			prbTail = prbControl;
		}
		acedRetList( prbControls );
		acutRelRb( prbControls );

		return (RSRSLT) ;
	}

	// ----- ads_dcl_control_getproperties symbol (do not rename)
	static int ads_dcl_control_getproperties(void)
	{
		struct resbuf *pArgs =acedGetArgs () ;

		TDclControlPtr pDclControl = NULL;
		if( !GetControlArgument( pArgs, pDclControl ) )
			return RSERR; //invalid input

		if( !AssertOutOfArgs( pArgs ) )
			return RSERR;

		if( !pDclControl )
			return RSRSLT; //form not found

		resbuf* prbProperties = NULL;
		resbuf* prbTail = NULL;
		const TPropertyList& Props = pDclControl->GetPropertyList();
		for( TPropertyList::const_iterator iter = Props.begin(); iter != Props.end(); ++iter )
		{
			resbuf* prbProp = acutNewRb( RTSTR );
			acutNewString( (*iter)->GetName(), prbProp->resval.rstring );
			if( prbTail )
				prbTail->rbnext = prbProp;
			else
				prbProperties = prbProp;
			prbTail = prbProp;
		}
		acedRetList( prbProperties );
		acutRelRb( prbProperties );

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
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_form_setpos, true)
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
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_files_dir, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_project_load, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_project_unload, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_project_saveas, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_project_import, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_project_export, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_updatecheck, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_setautoupdatecheck, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_getprojects, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_project_getforms, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_form_getcontrols, true)
ACED_ADSSYMBOL_ENTRY_AUTO(CARXApp, dcl_control_getproperties, true)
