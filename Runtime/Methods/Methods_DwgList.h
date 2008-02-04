// Methods_DwgList.h : header file
//

#pragma once


namespace DwgList
{
ADSRESULT Dir();
ADSRESULT GetDir();
ADSRESULT GetFileName();
ADSRESULT GetType();
ADSRESULT GetText();
ADSRESULT GetSelectedItems();
ADSRESULT GetSelectedNths();
ADSRESULT GetCurSel();
ADSRESULT SetCurSel();
ADSRESULT GetCount();
ADSRESULT GetTopIndex();
ADSRESULT SetTopIndex();
ADSRESULT DeleteString();
ADSRESULT SetSel();
ADSRESULT GetSel();
ADSRESULT SetFocusIndex(); // SetCaretIndex
ADSRESULT GetFocusIndex(); // GetCaretIndex
ADSRESULT GetSelCount();
ADSRESULT SelItemRange();
ADSRESULT SetAnchorIndex();
ADSRESULT GetAnchorIndex();
};
