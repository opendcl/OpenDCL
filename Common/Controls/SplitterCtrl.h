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
	bool Create( CWnd* pParentWnd, UINT nID ) override;
	CRect ValidatePosition( const CRect& rcProposed ) const override;
	bool ApplyProperty( TPropertyPtr pProp ) override;
	bool OnApplyLeft( TPropertyPtr pProp ) override; //Prop::Left
	bool OnApplyTop( TPropertyPtr pProp ) override; //Prop::Top
	bool OnApplyWidth( TPropertyPtr pProp ) override; //Prop::Width
	bool OnApplyHeight( TPropertyPtr pProp ) override; //Prop::Height
	CAcadColorService* GetColorService() override { return &mColorService; }

protected:
	DECLARE_MESSAGE_MAP()

// Generated message map functions
protected:
	BOOL PreTranslateMessage(MSG* pMsg) override;
	afx_msg void PostNcDestroy() override;
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
