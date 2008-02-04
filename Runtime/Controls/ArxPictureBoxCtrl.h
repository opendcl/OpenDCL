// ArxPictureBoxCtrl.h : header file
//

#pragma once

#include "PictureBoxCtrl.h"
#include "ArxControlServices.h"
#include "OleOdcDropTarget.h"


/////////////////////////////////////////////////////////////////////////////
// CArxPictureBoxCtrl window

class CArxPictureBoxCtrl : public CPictureBoxCtrl
{
	CArxControlServices	mArxServices;
	COleOdcDropTarget mDropTarget;	

// Construction
public:
	CArxPictureBoxCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CArxPictureBoxCtrl();

// DialogControl Interface
public:

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);	
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnClicked();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
};
