// Methods_Tree.h : header file
//

#pragma once


namespace Tree
{
ADSRESULT IsItemExpanded();
ADSRESULT SetByList();
ADSRESULT AddParent();
ADSRESULT AddChild();
ADSRESULT InsertAfter();
ADSRESULT GetParent();
ADSRESULT Clear();
ADSRESULT CountItems();
ADSRESULT SelectItem();
ADSRESULT ItemHasChildren();
ADSRESULT GetNextSiblingItem();
ADSRESULT GetPrevSiblingItem();
ADSRESULT GetFirstVisibleItem();
ADSRESULT GetNextVisibleItem();
ADSRESULT GetPrevVisibleItem();
ADSRESULT GetSelectedItem();
ADSRESULT GetRootItem();
ADSRESULT GetItem();
ADSRESULT SetItemImages();
ADSRESULT GetItemImages();
ADSRESULT GetItemText();
ADSRESULT SetItemText();
ADSRESULT GetItemData();
ADSRESULT SetItemData();
ADSRESULT GetVisibleCount();

ADSRESULT SetExpandedImage();
ADSRESULT GetExpandedImage();
/*
ADSRESULT GetCheck();
ADSRESULT SetCheck();
*/
ADSRESULT DeleteItem();
ADSRESULT ExpandItem();
ADSRESULT SelectSetFirstVisible();
ADSRESULT EditLabel();
ADSRESULT SortChildren();
ADSRESULT EnsureVisible();
ADSRESULT GetFirstChildItem();
ADSRESULT CancelEditLabel();
};
