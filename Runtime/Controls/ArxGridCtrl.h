// ArxGridCtrl.h : header file
//
// ARX specific functionality for Grid control

#pragma once

#include "GridCtrl.h"
#include "ArxControlServices.h"


/////////////////////////////////////////////////////////////////////////////
// CArxGridCtrl window

class CArxGridCtrl : public CGridCtrl
{
	CArxControlServices	mArxServices;

public:
	CArxGridCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CArxGridCtrl();

// DialogControl Interface
public:
	const CArxControlServices* GetArxServices() const override { return &mArxServices; }
	bool Create( CWnd* pParentWnd, UINT nID ) override;
	bool ApplyProperty( TPropertyPtr pProp ) override;

protected:
	void OnSelectionChanged() override;
	void OnEditCurCell() override;
	CGridCellEditCtrl* CreateEditControl( int nRow, int nCol ) override;
	void OnEndEditCurCell() override;
	void DrawCell( int nRow, int nCol, CDC& cdc, CSize sizCell = CSize(0, 0), bool bCalcOnly = false ) override;
	void DrawColor( CDC& cdc, const CRect& rcIcon, int nColor, const CString& sText ) override;

protected:
	DECLARE_MESSAGE_MAP()

// Generated message map functions
protected:
	afx_msg void OnCellButtonClicked(void);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu( CWnd* pTarget, CPoint point );
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);	
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSetfocus(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
};
