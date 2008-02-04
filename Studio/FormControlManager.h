// FormControlManager.h : header file
//

#pragma once

#include "ControlManager.h"
#include "ControlDropSource.h"
#include "ControlDropTarget.h"

class CStudioDialogObject;


class CFormControlManager : public CControlManager, public COleDataSource, public CRectTracker
{
	CStudioDialogObject* mpDlgObject;
	CControlDropSource mDropSource;
	CControlDropTarget mDropTarget;
	CPoint mptDragStart;
	bool mbSnapTracker;
	CControlManager* mpResizeTarget;

	// Attributes
protected:

public:
	CFormControlManager( CStudioDialogObject* pDlgObject );
	virtual ~CFormControlManager();

public:
	bool ActivateControlsInRect( const CRect& rc );

protected:
	//virtual void OnSelected( bool bSelected ) {}
	virtual void DrawTrackerRect( LPCRECT lpRect, CWnd* pWndClipTo, CDC* pDC, CWnd* pWnd );

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg __UINT_LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnCancelMode();
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg void OnNcPaint();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnNcRButtonUp(UINT nHitTest, CPoint point);
	afx_msg void OnNcLButtonDblClk(UINT nHitTest, CPoint point);
};
