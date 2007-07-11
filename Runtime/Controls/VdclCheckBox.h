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
	CDclControlObject *m_ArxControl;
	RefCountedPtr< CPropertyObject > m_pValue;
	bool m_bInvokeWithSendString;
public:
	CPPToolTip m_ToolTip;

// Construction
public:
	VdclCheckBox();
	virtual ~VdclCheckBox();

// Overrides
public:
	virtual BOOL Create(CDclControlObject* pControl, CWnd* pParentWnd, UINT nID );
protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

protected:
	afx_msg void OnDestroy();
	afx_msg void OnClicked();
	afx_msg void OnDoubleclicked();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};
