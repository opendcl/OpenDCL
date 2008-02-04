// SlideCtrl.h : header file
//

#pragma once

#include "DialogControl.h"
#include "AcadColorService.h"


/////////////////////////////////////////////////////////////////////////////
// CSlideCtrl window

class CSlideCtrl : public CSliderCtrl, public CDialogControl
{
	CAcadColorService mAcadColorService;

public:
	CSlideCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CSlideCtrl();

// DialogControl Interface
public:
	operator TDialogControlPtr () { return TDialogControlLockedPtr( this ); } //to ensure it doesn't get auto deleted
	virtual bool Create( CWnd* pParentWnd, UINT nID );
	virtual DWORD GetWndStyle() const;
	virtual bool OnApplyProperty( TPropertyPtr pProp );
	virtual CAcadColorService* GetColorService() { return &mAcadColorService; }

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

	virtual afx_msg void PostNcDestroy();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnReleasedcapture(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
};
