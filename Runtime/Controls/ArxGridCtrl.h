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
	virtual const CArxControlServices* GetArxServices() const { return &mArxServices; }
	virtual bool Create( CWnd* pParentWnd, UINT nID );
	virtual bool OnApplyProperty( TPropertyPtr pProp );

public:
	//void DoEditCellNow(int nStyle, UINT nChar = 0);
	//void ShowComboBox(int nRow, int nCol, int nStyle, CStringArray &sStrings);
	//void ShowImageComboBox(int nRow, int nCol, CStringArray &sStrings, CArray<int, int> &nItems);

protected:
	void DoFileDlg(CellStyle nStyle);
	//void DrawLineWeights(CDC* pDC, CRect rc, AcDb::LineWeight LW);
	//void DrawArrowHeads(CDC* pDC, CRect rc, int &nImage, CString &sText);
	//void ShowTextBox(int nRow, int nCol, int nStyle, UINT nChar = 0);

protected:
	virtual void OnSelectionChanged();
	virtual void OnEditCurCell();
	virtual CGridCellEditCtrl* CreateEditControl( int nRow, int nCol );
	virtual void OnEndEditCurCell();
	virtual void DrawCell( int nRow, int nCol, const CRect& rectCell, CDC& cdc );
	virtual void DrawColor( CDC& cdc, const CRect& rcIcon, int nColor, const CString& sText );

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
public:
	afx_msg void OnKillFocus(CWnd* pNewWnd);
};
