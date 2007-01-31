// VdclScrollBar.h : header file
//

#pragma once

#include "PPToolTip.h"

class CPropertyObject;
class CDclControlObject;


/////////////////////////////////////////////////////////////////////////////
// VdclScrollBar window

class VdclScrollBar : public CScrollBar
{
// Construction
public:
	VdclScrollBar();

// Attributes
public:
	int m_nLargeChange;
	int m_nSmallChange;
	int m_hPos;
	CPropertyObject *m_pValueProp;
	bool m_bInvokeWithSendString;
// Operations
public:
	void SetTooltipText(CString* spText, BOOL bActivate = TRUE);
	void InitToolTip();
	CPPToolTip m_ToolTip;
	

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VdclScrollBar)
	public:
	virtual BOOL Create(CDclControlObject* pControl, CWnd* pParentWnd, UINT nID );
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~VdclScrollBar();

	// Generated message map functions
protected:
	
	//{{AFX_MSG(VdclScrollBar)
	afx_msg void HScroll(UINT nSBCode, UINT nPos);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void VScroll(UINT nSBCode, UINT nPos);
	afx_msg void OnDestroy();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	void OnScroll(UINT nSBCode, UINT nPos);
	CDclControlObject *m_ArxControl;
};
