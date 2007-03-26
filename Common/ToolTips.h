// ToolTips.h : header file
//

#pragma once

class CPPToolTip;
class CDclControlObject;


CString ConstructTooltipHtml( LPCTSTR pszTitle, COLORREF crTitleColor = RGB(0,0,0),
															bool bSeparator = false, LPCTSTR pszMainText = NULL );

void SetToolTipEx(CWnd *pWnd, CPPToolTip &m_tooltip, CDclControlObject *pControl);
void SetToolTipEx(CWnd *pWnd, CPPToolTip &m_tooltip, CString sTitle, CString sMain, int nLine, int nColor, int nPicture, CString sAvi, CDclControlObject *pControl);
