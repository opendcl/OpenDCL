// Methods_ListBox.h : header file
//

#pragma once


namespace ListBox
{
ADSRESULT Dir();
ADSRESULT AddString();
ADSRESULT AddList();
ADSRESULT GetItemText();
ADSRESULT GetSelectedItems();
ADSRESULT GetSelectedNths();
ADSRESULT GetCurSel();
ADSRESULT SetCurSel();
ADSRESULT GetCount();
ADSRESULT Clear();
ADSRESULT GetTopIndex();
ADSRESULT SetTopIndex();
ADSRESULT SetItemData();
ADSRESULT GetItemData();
ADSRESULT DeleteItem();
ADSRESULT InsertString();
ADSRESULT FindString();
ADSRESULT SelectString();
ADSRESULT FindStringExact();
ADSRESULT SelectItem();
ADSRESULT IsItemSelected();
ADSRESULT SetFocusIndex(); // SetCaretIndex
ADSRESULT GetFocusIndex(); // GetCaretIndex
ADSRESULT GetSelCount();
ADSRESULT SelItemRange();
ADSRESULT SetAnchorIndex();
ADSRESULT GetAnchorIndex();
//ADSRESULT SetTabStops();
};
