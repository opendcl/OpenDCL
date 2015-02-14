// ListCtrlHdr.h : header file
//

#pragma once

#include "TipWnd.h"


/////////////////////////////////////////////////////////////////////////////
// CListCtrlHdr window

class CListCtrlHdr : public CHeaderCtrl
{
	class CHdrTipWnd : public CTipWnd
	{
		CListCtrlHdr* mpHdrCtrl;
		int mnCol;
	public:
		CHdrTipWnd(CListCtrlHdr* pHdrCtrl) : mpHdrCtrl(pHdrCtrl), mnCol(-1){}
	public:
		void Track(const CPoint& point);
		void Show(int nCol);
	} mTipWnd;
	bool mbTrackingMouse;

// Construction
public:
	CListCtrlHdr();
	virtual ~CListCtrlHdr();

public:
	bool TooltipHitTest(const CPoint& point, int& nCol) const;

protected:
	void PreSubclassWindow();
	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

protected:
	DECLARE_MESSAGE_MAP()

// Generated message map functions
protected:
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMouseHover(WPARAM wParam, LPARAM lParam);
};
