// Methods_Tree.h : header file
//

#pragma once


namespace Tree
{
ADSRESULT IsItemExpanded();
ADSRESULT SetByList();
ADSRESULT AddParent();
ADSRESULT AddChild();
ADSRESULT AddSibling();
ADSRESULT GetParentItem();
ADSRESULT Clear();
ADSRESULT GetCount();
ADSRESULT SelectItem();
ADSRESULT ItemHasChildren();
ADSRESULT GetNextSiblingItem();
ADSRESULT GetPrevSiblingItem();
ADSRESULT GetFirstVisibleItem();
ADSRESULT GetNextVisibleItem();
ADSRESULT GetPrevVisibleItem();
ADSRESULT GetSelectedItem();
ADSRESULT GetRootItem();
ADSRESULT GetItemHandle();
ADSRESULT GetItemKey();
ADSRESULT GetItem();
ADSRESULT SetItemImages();
ADSRESULT GetItemImages();
ADSRESULT GetItemLabel();
ADSRESULT SetItemLabel();
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
ADSRESULT StartLabelEdit();
ADSRESULT SortChildren();
ADSRESULT EnsureVisible();
ADSRESULT GetFirstChildItem();
ADSRESULT CancelLabelEdit();
};
