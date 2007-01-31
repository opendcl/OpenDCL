// ToolTips.h : header file
//

#pragma once

class CPPToolTip;
class CDclControlObject;


void SetToolTipEx(CWnd *pWnd, CPPToolTip &m_tooltip, CDclControlObject *pControl);
void SetToolTipEx(CWnd *pWnd, CPPToolTip &m_tooltip, CString sTitle, CString sMain, int nLine, int nColor, int nPicture, CString sAvi, CDclControlObject *pControl);
