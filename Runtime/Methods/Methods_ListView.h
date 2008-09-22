// Methods_ListView.h : header file
//

#pragma once


namespace ListView
{
ADSRESULT AddColumns();
ADSRESULT AddString();
ADSRESULT GetColumnImage();
ADSRESULT SetColumnImage();
ADSRESULT GetCount();
ADSRESULT GetColumnCount();
ADSRESULT AddItem();
ADSRESULT Arrange();
ADSRESULT CalcColWidth();
ADSRESULT Clear();
ADSRESULT DeleteItem();
ADSRESULT DeleteItems();
ADSRESULT DeleteColumn();
ADSRESULT DeleteColumns();
ADSRESULT FillList();
ADSRESULT GetColWidth();
ADSRESULT GetItemData();
ADSRESULT GetItemImage();
ADSRESULT GetItemText();
ADSRESULT GetSelCount();
ADSRESULT GetSelectedItems();
ADSRESULT GetSelectedNths();
ADSRESULT InsertItem();
ADSRESULT SetColWidth();
ADSRESULT GetCurSel();
ADSRESULT SetCurSel();
ADSRESULT SetItemData();
ADSRESULT SetItemText();
ADSRESULT SetItemImage();
ADSRESULT StartLabelEdit();
ADSRESULT CancelLabelEdit();
ADSRESULT SortTextItems();
ADSRESULT SortNumericItems();
ADSRESULT GetRowItems();
ADSRESULT GetColumnItems();
ADSRESULT HitPointTest();
};
