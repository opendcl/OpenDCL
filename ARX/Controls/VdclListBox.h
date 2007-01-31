// VdclListBox.h : header file
//

#pragma once

//#include "DropTarget.h"
//#include "DropSource.h"
#include "OleOdcDropTarget.h"
#include "ClrListBox.h"
#include "PPToolTip.h"


/////////////////////////////////////////////////////////////////////////////
// VdclListBox window

class VdclListBox : public CClrListBox
{
// Construction
public:
	VdclListBox();
	void GetCurrentSelection();

// Attributes
public:
	bool m_bInvokeWithSendString;

// Operations
public:
	void SetTooltipText(CString* spText, BOOL bActivate = TRUE);
	void InitToolTip();
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
	//{{AFX_VIRTUAL(VdclListBox)
	public:
	virtual BOOL Create(CDclControlObject* pControl, CWnd* pParentWnd, UINT nID );
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~VdclListBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(VdclListBox)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();	
	afx_msg void OnSelchange();
	afx_msg void OnDblclk();
	afx_msg void OnKillfocus();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSetfocus();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
  afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);	
  //}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
