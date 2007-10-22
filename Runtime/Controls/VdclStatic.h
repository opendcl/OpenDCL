// VdclStatic.h : header file
//

#pragma once

#include "OleOdcDropTarget.h"
#include "PPToolTip.h"

#if (_MFC_VER < 0x0800)
#define __LRESULT UINT
#else
#define __LRESULT LRESULT
#endif


/////////////////////////////////////////////////////////////////////////////
// VdclStatic window

class VdclStatic : public CStatic
{
	CBrush mbrushBackground;
	TDclControlPtr m_ArxControl;

// Attributes
public:
	COLORREF m_ForeColor;
	COLORREF m_BkColor;
	bool m_bTransparent;

// Construction
public:
	VdclStatic();
	~VdclStatic();

// Operations
public:
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
	

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL Create(TDclControlPtr pControl, CWnd* pParentWnd, UINT nID );
protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);	
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg HBRUSH CtlColor ( CDC* pDC, UINT nCtlColor );
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg __LRESULT OnNcHitTest(CPoint point);
};
