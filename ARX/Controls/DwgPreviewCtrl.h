// DwgPreviewCtrl.h : header file
//

#pragma once

#include "OleOdcDropTarget.h"
#include "PPToolTip.h"


/////////////////////////////////////////////////////////////////////////////
// CDwgPreviewCtrl window

class CDwgPreviewCtrl : public CButton
{
// Construction
public:
	CDwgPreviewCtrl();

// Attributes
public:
	CBrush *m_pStaticBrush;		
	COLORREF m_BackColor;
	CPPToolTip m_ToolTip;
	
	bool				m_bSelectedRect;
	COLORREF			m_HighlightColor;
	CString				m_filename;
	CDclControlObject	*m_ArxControl;
	COLORREF			m_BkColor;
	bool				m_bInvokeWithSendString;		
	CRect				m_rcFocus;
	
// Operations
public:
	void PaintCtrl(CDC *pdc);
	void SetHighLight(int nColorIndex);
	void RemoveHighLight();
	void Refresh(CDC *pdc);
	void LoadDwg(CString sFileName);
	void SetAcadColor(long nColor);
	void SetDragnDrop(BOOL bRegister);

// Implementation
public:
	//*****
    // COleOdcDropTarget, derived from COleDropTarget gives us
    // the functionality to be an OLE drop target.
    //*****
    COleOdcDropTarget m_DropTarget;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDwgPreviewCtrl)
	public:
	virtual BOOL Create(CDclControlObject* pControl, CWnd* pParentWnd, UINT nID );
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDwgPreviewCtrl();

	// Generated message map functions
protected:
	//afx_msg void OnPaint();
	//{{AFX_MSG(CDwgPreviewCtrl)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClicked();
	afx_msg void OnDoubleclicked();	
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);	
	afx_msg void OnSetFocus(CWnd* pOldWnd);	
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	
	DECLARE_MESSAGE_MAP()
};
