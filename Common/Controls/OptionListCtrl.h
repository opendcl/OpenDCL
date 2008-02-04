// OptionListCtrl.h : header file
//

#pragma once

#include "ListBoxCtrl.h"


/////////////////////////////////////////////////////////////////////////////
// COptionListCtrl window

class COptionListCtrl : public CListBoxCtrl
{
	int mnRowHeight;
	CImageList mImageList;

// Construction
public:
	COptionListCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~COptionListCtrl();

// DialogControl Interface
public:
	virtual bool Create( CWnd* pParentWnd, UINT nID );
	virtual DWORD GetWndStyle() const;
	virtual bool OnApplyProperty( TPropertyPtr pProp );

// Operations
public:
	void ResetTooltips();

// Overrides
protected:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);	
};
