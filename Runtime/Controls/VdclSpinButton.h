// VdclSpinButton.h : header file
//

#pragma once

#include "PPToolTip.h"

class CPropertyObject;
class CDclControlObject;


/////////////////////////////////////////////////////////////////////////////
// VdclSpinButton window

class VdclSpinButton : public CSpinButtonCtrl
{
// Construction
public:
	VdclSpinButton();

// Attributes
public:
	bool m_bInvokeWithSendString;
	TPropertyPtr m_pValueProp;
	TPropertyPtr m_pWrapProp;
	TPropertyPtr m_pMinProp;
	TPropertyPtr m_pMaxProp;
	int m_Pos;
// Operations
public:
	CPPToolTip m_ToolTip;
	

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VdclSpinButton)
	public:
	virtual BOOL Create(CDclControlObject* pControl, CWnd* pParentWnd, UINT nID );
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~VdclSpinButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(VdclSpinButton)
	afx_msg void OnDeltapos(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	CDclControlObject *m_ArxControl;
};
