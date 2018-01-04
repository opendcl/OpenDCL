// SplitterCtrl.h : header file
//

#pragma once

#include "DialogControl.h"
#include "AcadColorService.h"

#if (_MFC_VER < 0x0800)
#define __UINT_LRESULT UINT
#else
#define __UINT_LRESULT LRESULT
#endif


/////////////////////////////////////////////////////////////////////////////
// CSplitterCtrlCtrl window

class CSplitterCtrl : public CStatic, public CDialogControl
{
	CAcadColorService mColorService;
	bool mbVertical;
	CPoint mptDragStart;
	bool mbIgnoreSizing;

// Construction
public:
	CSplitterCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CSplitterCtrl();
	bool IsVertical() const { return mbVertical; }

// DialogControl Interface
public:
	operator TDialogControlPtr () { return TDialogControlLockedPtr( this ); } //to ensure it doesn't get auto deleted
	virtual bool Create( CWnd* pParentWnd, UINT nID );
	virtual CRect ValidatePosition( const CRect& rcProposed ) const;
	virtual bool ApplyProperty( TPropertyPtr pProp );
	virtual bool OnApplyLeft( TPropertyPtr pProp ); //Prop::Left
	virtual bool OnApplyTop( TPropertyPtr pProp ); //Prop::Top
	virtual bool OnApplyWidth( TPropertyPtr pProp ); //Prop::Width
	virtual bool OnApplyHeight( TPropertyPtr pProp ); //Prop::Height
	virtual CAcadColorService* GetColorService() { return &mColorService; }

protected:
	DECLARE_MESSAGE_MAP()

// Generated message map functions
protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual afx_msg void PostNcDestroy();
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg __UINT_LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnDpiChanged(WPARAM wParam, LPARAM lParam);
};
