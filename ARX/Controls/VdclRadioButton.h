// VdclRadioButton.h : header file
//

#pragma once

#include "ColorButton.h"	// Added by ClassView
#include "PPToolTip.h"

class CDclControlObject;
class CPropertyObject;


/////////////////////////////////////////////////////////////////////////////
// VdclRadioButton window

class VdclRadioButton : public CClrButton
{
// Construction
public:
	VdclRadioButton();

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
	//{{AFX_VIRTUAL(VdclRadioButton)
	public:
	virtual BOOL Create(CDclControlObject* pControl, CWnd* pParentWnd, UINT nID );
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~VdclRadioButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(VdclRadioButton)
	afx_msg void OnDestroy();
	afx_msg void OnClicked();
	afx_msg void OnDoubleclicked();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	CDclControlObject *m_ArxControl;
	RefCountedPtr< CPropertyObject > m_pValue;
};
