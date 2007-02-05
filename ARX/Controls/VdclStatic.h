// VdclStatic.h : header file
//

#pragma once

#include "OleOdcDropTarget.h"
#include "PPToolTip.h"

class CDclControlObject;


/////////////////////////////////////////////////////////////////////////////
// VdclStatic window

class VdclStatic : public CStatic
{
// Attributes
public:
	CBrush *m_pStaticBrush;		
	COLORREF m_ForeColor;
	COLORREF m_BkColor;
	bool m_bTransparent;
	bool m_bInvokeWithSendString;

// Construction
public:
	VdclStatic();
	~VdclStatic();

// Operations
public:
	void SetTooltipText(CString* spText, BOOL bActivate = TRUE);
	void InitToolTip();
	CPPToolTip m_ToolTip;
	void SetDragnDrop(BOOL bRegister);
	void SetAcadColor(long nColor);
	void SetForeColor(long nColor);

// Implementation
public:
	//*****
    // COleOdcDropTarget, derived from COleDropTarget gives us
    // the functionality to be an OLE drop target.
    //*****
    COleOdcDropTarget m_DropTarget;
	

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VdclStatic)
	public:
	virtual BOOL Create(CDclControlObject* pControl, CWnd* pParentWnd, UINT nID );
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);	
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_VIRTUAL

// Implementation
public:

	// Generated message map functions
protected:
	//{{AFX_MSG(VdclStatic)
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg HBRUSH CtlColor ( CDC* pDC, UINT nCtlColor );
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	CDclControlObject *m_ArxControl;
};
