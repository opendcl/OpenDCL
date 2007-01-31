// VdclComboBoxEx.h : header file
//

#pragma once

#include "PPToolTip.h"

class CDclControlObject;


/////////////////////////////////////////////////////////////////////////////
// VdclComboBoxEx window

class VdclComboBoxEx : public CComboBoxEx
{
// Construction
public:
	VdclComboBoxEx();

// Attributes
public:
	bool m_bInvokeWithSendString;

// Operations
public:
	void SetTooltipText(CString* spText, BOOL bActivate = TRUE);
	void InitToolTip();
	CPPToolTip m_ToolTip;
	
// ListCtrl edit data members
	bool m_bESC;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VdclComboBoxEx)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~VdclComboBoxEx();

	BOOL Create2(CDclControlObject* pControl, CWnd* pParentWnd, UINT nID );

	// Generated message map functions
protected:
	//{{AFX_MSG(VdclComboBoxEx)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnDestroy();	
	afx_msg void OnEditchange();
	afx_msg void OnKillfocus();
	afx_msg void OnSetfocus();
	afx_msg void OnSelchange();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnEditupdate();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnDropdown();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	CDclControlObject *m_ArxControl;
};
