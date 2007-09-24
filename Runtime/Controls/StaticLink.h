// StaticLink.h : header file
//

#pragma once

#include "PPToolTip.h"
#include "PtrTypes.h"

#if (_MFC_VER < 0x0800)
#define __LRESULT UINT
#else
#define __LRESULT LRESULT
#endif


//////////////////
// Simple text hyperlink derived from CString
//
class CHyperlink : public CString {
public:
	CHyperlink(LPCTSTR lpLink = NULL) : CString(lpLink) { }
	~CHyperlink() { }
	const CHyperlink& operator=(LPCTSTR lpsz) {
		CString::operator=(lpsz);
		return *this;
	}
	operator LPCTSTR() {
		return CString::operator LPCTSTR(); 
	}
	HINSTANCE Navigate(CString sURL) {
		return  IsEmpty() ? NULL :
			ShellExecute(0, _T("open"), sURL, 0, 0, SW_SHOWNORMAL);
	}
};


/////////////////////////////////////////////////////////////////////////////
// CStaticLink window

class CStaticLink : public CStatic
{
	DECLARE_DYNAMIC(CStaticLink)

	// Construction
public:
	CStaticLink();

	// Operations
public:
	CPPToolTip m_ToolTip;
	
	void SetLinkText(CString sNewText);
	void Refresh();
	void SetAcadColor(long nColor);
	void SetForeColor(long nColor);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStaticLink)
	public:
	virtual BOOL Create(TDclControlPtr pControl, CWnd* pParentWnd, UINT nID );
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

	// Hyperlink contains URL/filename. If NULL, I will use the window text.
	// (GetWindowText) to get the target.
	CHyperlink		m_link;
	COLORREF		m_BkColor;
	COLORREF		m_color;

	// Default colors you can change
	// These are global, so they're the same for all links.
	static COLORREF g_colorUnvisited;
	static COLORREF g_colorVisited;

	// Cursor used when mouse is on a link--you can set, or
	// it will default to the standard hand with pointing finger.
	// This is global, so it's the same for all links.
	static HCURSOR	 g_hCursorLink;
// Implementation
public:
	virtual ~CStaticLink();

	// Generated message map functions
protected:
	//{{AFX_MSG(CStaticLink)
  afx_msg __LRESULT OnNcHitTest(CPoint point);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	void CStaticLink::Refresh(CDC *pdc);

	TDclControlPtr m_ArxControl;
	UINT m_hPos;
};
