// ListBoxMethods.h : header file
//

#pragma once


int DwgList_Dir();
int DwgList_GetDir();
int DwgList_GetFileName();
int DwgList_GetType();

int OptionList_SetEnabled();
int OptionList_SetTttTitle();

int ListBox_Dir();
int ListBox_AddString();
int ListBox_AddList();
int ListBox_GetText();
int ListBox_GetSelectedItems();
int ListBox_GetSelectedNths();
int ListBox_GetCurSel();
int ListBox_SetCurSel();
int ListBox_GetCount();
int ListBox_Clear();

int ListBox_GetTopIndex();
int ListBox_SetTopIndex();

int ListBox_SetItemData();
int ListBox_GetItemData();

int ListBox_DeleteString();
int ListBox_InsertString();
int ListBox_FindString();
int ListBox_SelectString();
int ListBox_FindStringExact();

int ListBox_SetSel();
int ListBox_GetSel();
int ListBox_SetFocusIndex(); // SetCaretIndex
int ListBox_GetFocusIndex(); // GetCaretIndex
int ListBox_GetSelCount();
int ListBox_SelItemRange();
int ListBox_SetAnchorIndex();
int ListBox_GetAnchorIndex();
//int ListBox_SetTabStops();
