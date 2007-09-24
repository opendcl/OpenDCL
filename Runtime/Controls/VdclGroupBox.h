// VdclGroupBox.h : header file
//

#pragma once

#include "PPToolTip.h"
#include "PtrTypes.h"


/////////////////////////////////////////////////////////////////////////////
// VdclGroupBox window

class VdclGroupBox : public CStatic
{
// Construction
public:
	VdclGroupBox();

// Attributes
public:
	CButton m_Frame;
// Operations
public:
	CPPToolTip m_ToolTip;
	

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VdclGroupBox)
	public:
	virtual BOOL Create(TDclControlPtr pControl, CWnd* pParentWnd, UINT nID );
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~VdclGroupBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(VdclGroupBox)
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	TDclControlPtr m_ArxControl;
};
