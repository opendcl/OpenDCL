// VdclCheckBox.h : header file
//

#pragma once

#include "ColorButton.h"
#include "PPToolTip.h"

class CDclControlObject;
class CPropertyObject;


/////////////////////////////////////////////////////////////////////////////
// VdclCheckBox window

class VdclCheckBox : public CClrButton
{
// Construction
public:
	VdclCheckBox();

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
	//{{AFX_VIRTUAL(VdclCheckBox)
	public:
	virtual BOOL Create(CDclControlObject* pControl, CWnd* pParentWnd, UINT nID );
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~VdclCheckBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(VdclCheckBox)
	afx_msg void OnDestroy();
	afx_msg void OnClicked();
	afx_msg void OnDoubleclicked();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	CDclControlObject *m_ArxControl;
	RefCountedPtr< CPropertyObject > m_pValue;
};
