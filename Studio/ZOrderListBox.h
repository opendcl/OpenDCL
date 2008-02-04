// CZOrderListBox.h : header file
//

#pragma once

#include "ControlTypes.h"
#include "DclFormObject.h"
#include "PtrTypes.h"
#include <map>
#include <list>

class CStudioDialogObject;


/////////////////////////////////////////////////////////////////////////////
// CZOrderListBox window

class CZOrderListBox : public CListBox
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
	CZOrderListBox();
	virtual ~CZOrderListBox();

public:
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

// Attributes
public:

// Operations
public:
	void OnActivateDlgObject( CStudioDialogObject* pDlgObject );
	void OnActivateDclControl( TDclControlPtr pDclControl );

protected:
	void OnZOrderChanged();
	void OnMovezToIndex( int idxInsertAt );

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT /*lpMeasureItemStruct*/);
	afx_msg void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
	afx_msg void OnSendtofront();
	afx_msg void OnSendtoback();
	afx_msg void OnMovezbackby1();
	afx_msg void OnMovezfrontby1();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
};
