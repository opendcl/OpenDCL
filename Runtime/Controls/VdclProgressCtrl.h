// VdclProgressCtrl.h : header file
//

#pragma once

#include "PPToolTip.h"
#include "PtrTypes.h"


/////////////////////////////////////////////////////////////////////////////
// VdclProgressCtrl window

class VdclProgressCtrl : public CProgressCtrl
{
// Construction
public:
	VdclProgressCtrl();

// Operations
public:
	CPPToolTip m_ToolTip;
	

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VdclProgressCtrl)
	public:
	virtual BOOL Create(TDclControlPtr pControl, CWnd* pParentWnd, UINT nID );
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
	TDclControlPtr m_ArxControl;
};
