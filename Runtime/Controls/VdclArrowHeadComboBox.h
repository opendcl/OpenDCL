// VdclArrowHeadComboBox.h : header file
//

#pragma once

#include "PPToolTip.h"

class CDclControlObject;


/////////////////////////////////////////////////////////////////////////////
// VdclArrowHeadComboBox window

class VdclArrowHeadComboBox : public CAcUiArrowHeadComboBox
{
// Construction
public:
	VdclArrowHeadComboBox();

// Attributes
public:
	bool m_bInvokeWithSendString;
// ListCtrl edit data members
	bool m_bESC;
	virtual BOOL Create(int nStyle, CRect rc, CWnd* pParentWnd, UINT nID);

// Operations
public:
	CPPToolTip m_ToolTip;
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VdclArrowHeadComboBox)
	public:
	virtual BOOL Create(CDclControlObject* pControl, CWnd* pParentWnd, UINT nID );
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~VdclArrowHeadComboBox();

      
	// Generated message map functions
protected:
	//{{AFX_MSG(VdclArrowHeadComboBox)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnDestroy();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSelchange();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnDropdown();
	//afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);		
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	CDclControlObject *m_ArxControl;
};
