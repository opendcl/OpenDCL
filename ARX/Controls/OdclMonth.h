// OdclMonth.h : header file
//

#pragma once

#include "PPToolTip.h"

class CDclControlObject;


/////////////////////////////////////////////////////////////////////////////
// OdclMonth window

class OdclMonth : public CMonthCalCtrl
{
// Construction
public:
	OdclMonth();

// Attributes
public:
	bool m_bInvokeWithSendString;
	
// Operations
public:
	void SetTooltipText(CString* spText, BOOL bActivate = TRUE);
	void InitToolTip();
	CPPToolTip m_ToolTip;
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(OdclMonth)
	public:
	virtual BOOL Create(CDclControlObject* pControl, CWnd* pParentWnd, UINT nID);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~OdclMonth();

	// Generated message map functions
protected:
	//{{AFX_MSG(OdclMonth)
	afx_msg void OnDestroy();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnGetdaystate(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchange(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelect(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	CDclControlObject *m_ArxControl;
};
