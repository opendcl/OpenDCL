// VdclComboBox.h : header file
//

#pragma once

#include "PPToolTip.h"

class CDclControlObject;


/////////////////////////////////////////////////////////////////////////////
// VdclComboBox window

class VdclComboBox : public CComboBox
{
// Construction
public:
	VdclComboBox();

// Attributes
public:
	bool m_bInvokeWithSendString;
	
// ListCtrl edit data members
	bool  m_bESC;
	CEdit *m_pGridEdit;
	bool m_bAutoComplete;
// Operations
public:
	CPPToolTip m_ToolTip;
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VdclComboBox)
	public:
	virtual BOOL Create(CDclControlObject* pControl, CWnd* pParentWnd, UINT nID );
	virtual BOOL Create(CRect rc, CWnd* pParentWnd, UINT nID);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~VdclComboBox();

      
	// Generated message map functions
protected:
	//{{AFX_MSG(VdclComboBox)
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnDestroy();	
	afx_msg void OnEditchange();
	afx_msg void OnKillfocus();
	afx_msg void OnSetfocus();
	afx_msg void OnSelchange();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnDropdown();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	CDclControlObject *m_ArxControl;
};
