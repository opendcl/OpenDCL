// Methods_TextBox.h : header file
//

#pragma once


namespace TextBox
{
ADSRESULT GetFilter();
ADSRESULT SetFilter();
ADSRESULT GetLineCount();
ADSRESULT IsModified();
ADSRESULT GetSel();
ADSRESULT GetLine();
ADSRESULT GetFirstVisibleLine();
ADSRESULT GetLineLength();
ADSRESULT LineScroll();
ADSRESULT ReplaceSel();
ADSRESULT SetSel();
ADSRESULT SetTabStops();
ADSRESULT Undo();
};
