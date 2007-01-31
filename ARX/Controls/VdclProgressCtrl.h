// VdclProgressCtrl.h : header file
//

#pragma once

#include "PPToolTip.h"

class CDclControlObject;


/////////////////////////////////////////////////////////////////////////////
// VdclProgressCtrl window

class VdclProgressCtrl : public CProgressCtrl
{
// Construction
public:
	VdclProgressCtrl();

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
	//{{AFX_VIRTUAL(VdclProgressCtrl)
	public:
	virtual BOOL Create(CDclControlObject* pControl, CWnd* pParentWnd, UINT nID );
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~VdclProgressCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(VdclProgressCtrl)
	afx_msg void OnDestroy();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	CDclControlObject *m_ArxControl;
};
