// GripRect.h : header file
//

#pragma once

#include "SelectedControl.h"


/////////////////////////////////////////////////////////////////////////////
// CGripRect window

class CGripRect : public CWnd
{
// Construction
public:
	CGripRect();

// Attributes
public:
	CSelectedControl m_SelectedControl;
	HCURSOR m_hCursor;
	bool m_bShowAsSelected;
	bool m_bResizing;
	int m_nQuadrant;
// Operations
public:
	void SetGripCursor(LPCTSTR lpszCursorName);
	void Refresh();
	void RePaint(HDC hdc);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGripRect)
	public:
	virtual BOOL Create(const RECT& rect, CWnd* pParentWnd, UINT nID);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CGripRect();

	// Generated message map functions
protected:
	//{{AFX_MSG(CGripRect)
	afx_msg void OnPaint();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:	
	CString m_ClassName;
};
