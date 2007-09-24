// VdclTextButton.h : header file
//

#pragma once

#include "OleOdcDropTarget.h"
#include "PPToolTip.h"


/////////////////////////////////////////////////////////////////////////////
// VdclTextButton window

class VdclTextButton : public CButton
{
// Construction
public:
	VdclTextButton();
	
// Operations
public:
	CPPToolTip m_ToolTip;
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
	//{{AFX_VIRTUAL(VdclTextButton)
	public:
	virtual BOOL Create(TDclControlPtr pControl, CWnd* pParentWnd, UINT nID );
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~VdclTextButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(VdclTextButton)
	afx_msg void OnDestroy();
	afx_msg void OnClicked();
	afx_msg void OnDoubleclicked();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg void OnNcPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	TDclControlPtr m_ArxControl;
};
