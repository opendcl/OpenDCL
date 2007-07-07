// Methods_Tree.h : header file
//

#pragma once


void ReturnTreeItem(CString sText, int nImage, int nSelectedImage, CString sKey);

int Tree_IsItemExpanded();

int Tree_SetByList();
int Tree_AddParent();
int Tree_AddChild();
int Tree_InsertAfter();
int Tree_GetParent();
int Tree_Clear();
int Tree_CountItems();
int Tree_SelectItem();
int Tree_ItemHasChildren();
int Tree_GetNextSiblingItem();
int Tree_GetPrevSiblingItem();
int Tree_GetFirstVisibleItem();
int Tree_GetNextVisibleItem();
int Tree_GetPrevVisibleItem();
int Tree_GetSelectedItem();
int Tree_GetRootItem();
int Tree_GetItem();
int Tree_SetItemImages();
int Tree_GetItemImages();
int Tree_GetItemText();
int Tree_SetItemText();
int Tree_GetItemData();
int Tree_SetItemData();
int Tree_GetVisibleCount();

int Tree_SetExpanedImage();
int Tree_GetExpanedImage();
/*
int Tree_GetCheck();
int Tree_SetCheck();
*/
int Tree_DeleteItem();
int Tree_ExpandItem();
int Tree_SelectSetFirstVisible();
int Tree_EditLabel();
int Tree_SortChildren();
int Tree_EnsureVisible();
int Tree_GetFirstChildItem();
int Tree_CancelEditLabel();
