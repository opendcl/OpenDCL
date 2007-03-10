// StaticLink.h : header file
//

#pragma once

#include "PPToolTip.h"

class CDclControlObject;


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
// Construction
public:
	CStaticLink();
	DECLARE_DYNAMIC(CStaticLink)

// Attributes
public:
	bool m_bInvokeWithSendString;
	CBrush *m_pStaticBrush;		

	// Operations
public:
	void SetTooltipText(CString* spText, BOOL bActivate = TRUE);
	void InitToolTip();
	CPPToolTip m_ToolTip;
	
	void SetLinkText(CString sNewText);
	void Refresh();
	void SetAcadColor(long nColor);
	void SetForeColor(long nColor);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStaticLink)
	public:
	virtual BOOL Create(CDclControlObject* pControl, CWnd* pParentWnd, UINT nID );
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

	// Hyperlink contains URL/filename. If NULL, I will use the window text.
	// (GetWindowText) to get the target.
	CHyperlink		m_link;
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

protected:
	//CFont			m_font;					// underline font for text control
	BOOL			m_bDeleteOnDestroy;	// delete object when window destroyed?

	virtual void PostNcDestroy();

	// Generated message map functions
protected:
	//{{AFX_MSG(CStaticLink)
#if (_ACADTARGET == 16)
  afx_msg UINT OnNcHitTest(CPoint point);
#else
  afx_msg LRESULT OnNcHitTest(CPoint point);
#endif
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

	CDclControlObject *m_ArxControl;
	UINT m_hPos;
};
