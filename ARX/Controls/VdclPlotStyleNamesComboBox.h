// VdclPlotStyleNamesComboBox.h : header file
//

#pragma once

#include "PPToolTip.h"

class CDclControlObject;


/////////////////////////////////////////////////////////////////////////////
// VdclPlotStyleNamesComboBox window

class VdclPlotStyleNamesComboBox : public CAcUiPlotStyleNamesComboBox
{
// Construction
public:
	VdclPlotStyleNamesComboBox();

// Attributes
public:
	bool m_bInvokeWithSendString;
// ListCtrl edit data members
	bool m_bESC;
	virtual BOOL Create(int nStyle, CRect rc, CWnd* pParentWnd, UINT nID);

// Operations
public:
	void SetTooltipText(CString* spText, BOOL bActivate = TRUE);
	void InitToolTip();
	CPPToolTip m_ToolTip;
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VdclPlotStyleNamesComboBox)
	public:
	virtual BOOL Create(CDclControlObject* pControl, CWnd* pParentWnd, UINT nID );
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~VdclPlotStyleNamesComboBox();

      
	// Generated message map functions
protected:
	//{{AFX_MSG(VdclPlotStyleNamesComboBox)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnDestroy();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSelchange();
	afx_msg void OnDropdown();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	CDclControlObject *m_ArxControl;
};
