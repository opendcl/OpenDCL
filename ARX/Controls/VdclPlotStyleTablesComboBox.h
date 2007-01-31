// VdclPlotStyleTablesComboBox.h : header file
//

#pragma once

#include "PPToolTip.h"

class CDclControlObject;


/////////////////////////////////////////////////////////////////////////////
// VdclPlotStyleTablesComboBox window

class VdclPlotStyleTablesComboBox : public CAcUiPlotStyleTablesComboBox
{
// Construction
public:
	VdclPlotStyleTablesComboBox();

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
	//{{AFX_VIRTUAL(VdclPlotStyleTablesComboBox)
	public:
	virtual BOOL Create(CDclControlObject* pControl, CWnd* pParentWnd, UINT nID );
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~VdclPlotStyleTablesComboBox();

      
	// Generated message map functions
protected:
	//{{AFX_MSG(VdclPlotStyleTablesComboBox)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnDestroy();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSelchange();
	afx_msg void OnDropdown();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	CDclControlObject *m_ArxControl;
};
