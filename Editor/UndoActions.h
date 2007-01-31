// UndoActions.h : header file
//

#pragma once

class CDclControlObject;


/////////////////////////////////////////////////////////////////////////////
// CUndoActions window

#define uaDeleted  1
#define uaMoved	 2
#define uaZOrder	 3
#define uaInsert	 4
#define uaFontName  5
#define uaFontSize  6
#define uaFontBold  7
#define uaFontItalic  8
#define uaFontUnderline  9

class CUndoActions : public CObject
{
// Construction
public:
	CUndoActions(int nAction, CDclControlObject *pArxObject, CWnd *pControl);
	~CUndoActions();
// Attributes
public:
	int m_nAction;
	CDclControlObject *m_pArxObject;
	CWnd *m_pControl;
	CRect rcPos;
	CString sString;
	long lLong;
	BOOL bBool;
};
