// OptionListCtrl.h : header file
//

#pragma once

#include "DialogControl.h"
#include "AcadColorService.h"


/////////////////////////////////////////////////////////////////////////////
// COptionListCtrl window

class COptionListCtrl : public CListBox, public CDialogControl
{
	CAcadColorService mColorService;
	int mnRowHeight;
	CImageList mImageList;
	bool mbTrackingMouse;

// Construction
public:
	COptionListCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~COptionListCtrl();

// DialogControl Interface
public:
	operator TDialogControlPtr () { return TDialogControlLockedPtr( this ); } //to ensure it doesn't get auto deleted
	virtual bool Create( CWnd* pParentWnd, UINT nID );
	virtual DWORD GetWndStyle() const;
	virtual bool ApplyPropertiesEnum();
	virtual bool ApplyProperty( TPropertyPtr pProp );
	virtual CAcadColorService* GetColorService() { return &mColorService; }

// Operations
public:
	void ResetTooltips();

protected:
	DECLARE_MESSAGE_MAP()

	virtual void PostNcDestroy();
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);	
	afx_msg void OnLbnSelchange();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};
