// GripWnd.h : header file
//

#pragma once

extern const UINT WM_GRIP_STARTDRAG;
extern const UINT WM_GRIP_DRAGGING;


/////////////////////////////////////////////////////////////////////////////
// CGripWnd window

class CGripWnd : public CWnd
{
	CWnd* mpParentWnd;
	bool mbHot;
	HCURSOR mhCursor;
	bool mbResizing;
	int mnQuadrant;

public:
	enum { Size = 6 };

// Construction
public:
	CGripWnd( CWnd* pParentWnd, int nQuadrant, LPCTSTR pszCursor, bool bHot = false );
	virtual ~CGripWnd();

// Operations
public:
	void CancelSizing() { mbResizing = false; }
	HCURSOR GetGripCursor() const { return mhCursor; }
	bool EnsureCreated() { return CreateGripWindow(); }
	bool IsPointInside( const CPoint& ptTest /*window coordinates*/ );
	void SetHot( bool bHot = true ) { mbHot = bHot; Invalidate(); }

protected:
	bool CreateGripWindow();

protected:
	DECLARE_MESSAGE_MAP()

// Generated message map functions
protected:
	afx_msg void OnPaint();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
};
