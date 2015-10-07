// TipWnd.h : header file
//

#pragma once

#if (_MFC_VER < 0x0800)
#define __UINT_LRESULT UINT
#else
#define __UINT_LRESULT LRESULT
#endif


/////////////////////////////////////////////////////////////////////////////
// CTipWnd window

class CTipWnd : public CWnd
{
	CFont mFont;
	COLORREF mclrForeground;
	COLORREF mclrBackground;

// Construction
public:
	CTipWnd();
	CTipWnd(COLORREF clrForeground, COLORREF clrBackground);
	virtual ~CTipWnd();

// Interface
public:
	BOOL Create();
	void Show(const CPoint& ptTip, const CSize& szTip);
	void Show();
	void Hide();

protected:
	virtual void Paint(CDC* pDC);

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

	afx_msg LRESULT OnGetFont(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetFont(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg __UINT_LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnDestroy();
};
