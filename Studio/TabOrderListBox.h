// CTabOrderListBox.h : header file
//

#pragma once

#include "ControlTypes.h"
#include "DclFormTemplate.h"
#include "PtrTypes.h"
#include <map>
#include <list>

class CStudioDialogObject;


/////////////////////////////////////////////////////////////////////////////
// CTabOrderListBox window

class CTabOrderListBox : public CListBox
{
	CStudioDialogObject* mpDlgObject;
	CImageList mImageList;
	CFont mFont;
	TDclControlList mControls;
	bool mbNotifying;
	int mnItemUnselectPending;
	bool mbDragging;
	CPoint mptDragStart;
	int midxInsertAt;
	CRect mrcInsertCaret;

// Construction
public:
	CTabOrderListBox();
	virtual ~CTabOrderListBox();

public:
	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID) override;

// Attributes
public:

// Operations
public:
	void OnActivateDlgObject( CStudioDialogObject* pDlgObject );
	void OnActivateDclControl( TDclControlPtr pDclControl );

protected:
	void OnTabOrderChanged();
	void OnMovezToIndex( int idxInsertAt );

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

	BOOL PreTranslateMessage(MSG* pMsg) override;
	void MeasureItem(LPMEASUREITEMSTRUCT /*lpMeasureItemStruct*/) override;
	void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/) override;
	afx_msg void OnSendtofront();
	afx_msg void OnSendtoback();
	afx_msg void OnMovebackby1();
	afx_msg void OnMovefrontby1();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
};
