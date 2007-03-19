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
	bool			m_bInvokeWithSendString;

	bool m_bShowHighlight;
	bool m_bEditCells;
	CDynamicButtonCtrl *m_pEllipsesButton;
	CDynamicButtonCtrl *m_pPickButton;
	CDynamicButtonCtrl *m_pFolderButton;
	CWnd *m_pTextBox[nNumOfTextBoxes];
	CWnd *m_pComboBox[nNumOfComboBoxes];
	int	m_nEditSubItem;

public:
	CArxGridCtrl( CDclControlObject* pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CArxGridCtrl();

// DialogControl Interface
public:
	virtual const CArxControlServices* GetArxServices() const { return &mArxServices; }
	virtual bool Create( CWnd* pParentWnd, UINT nID );
	virtual bool OnApplyProperty( RefCountedPtr< CPropertyObject > pProp );

public:
	virtual void HideEditControls();
	void DoEditCellNow(int nStyle, UINT nChar = 0);
	void ShowComboBox(int nRow, int nCol, int nStyle, CStringArray &sStrings);
	void ShowImageComboBox(int nRow, int nCol, CStringArray &sStrings, CArray<int, int> &nItems);
	void EndEditControls(CWnd *pWnd);
	void MoveUp();
	void MoveDown();
	void SetCurSel(int nRow, int nCol);

protected:
	void DoFileDlg(int nStyle);
	void CheckLayer(CString &sLayer, int &nImage);
	bool acad_truecolordlg(COLORREF color, int curColor, int nIndex, CString sPantone);
	void DrawLineWeights(CDC* pDC, CRect rc, AcDb::LineWeight LW);
	void DrawArrowHeads(CDC* pDC, CRect rc, int &nImage, CString &sText);
	void DrawColor(CDC* pDC, CRect rc, int &nColor, CString &sText);
	void DrawFontIcons(CDC* pDC, CRect rc, int &nImage, CString &sText);
	void EditCellNow(UINT nChar = 0);
	void ShowEllipsesButton(int nRow, int nCol, int nAsPick);
	void ShowCurSel();
	void ShowTextBox(int nRow, int nCol, int nStyle, UINT nChar = 0);
	void CallBeginLabelEdit(CPoint point);

protected:
	DECLARE_MESSAGE_MAP()

// Generated message map functions
protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnCellButtonClicked(void);
	afx_msg void OnDir2CellButtonClicked(void);
	afx_msg void OnDir3CellButtonClicked(void);
	afx_msg void OnKillfocus(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);	
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);	
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSetfocus(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeledit(CString sText);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
};
