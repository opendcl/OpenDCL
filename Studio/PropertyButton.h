// PropertyButton.h : header file
//

#pragma once

#define nDropDownImage  0
#define nDotsImage  1
#define nOffWindow -1


/////////////////////////////////////////////////////////////////////////////
// CPropertyButton window

class CPropertyButton : public CWnd
{
// Construction
public:
	CPropertyButton();	
// Attributes
public:
	int m_ButtonImage;
// Operations
public:
	void Refresh();
	void DrawOuterRectUp(CRect *rect, CDC *pdc);
	void DrawInnerRectUp(CRect *rect, CDC *pdc);
	void DrawCenterUp(CRect *rect, CDC *pdc);
	void DrawButtonUp(CRect *rect, CDC *pdc);
	void DrawButtonImage(bool bButtonUp, CRect *rect, CDC *pdc);
	void SetButtonStyle(int nStyle);
	int  GetButtonStyle();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropertyButton)
	public:
	virtual BOOL Create(const RECT& rect, CWnd* pParentWnd, UINT nID);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPropertyButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPropertyButton)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CString m_ClassName;
};
