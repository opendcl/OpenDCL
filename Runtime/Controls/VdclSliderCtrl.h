// VdclSliderCtrl.h : header file
//

#pragma once

#include "PPToolTip.h"

class CPropertyObject;
class CDclControlObject;


/////////////////////////////////////////////////////////////////////////////
// VdclSliderCtrl window

class VdclSliderCtrl : public CSliderCtrl
{
private:
	CDclControlObject *m_ArxControl;
	
// Construction
public:
	VdclSliderCtrl();

// Attributes
public:
	bool m_bInvokeWithSendString;
	TPropertyPtr m_pValueProp;
// Operations
public:
	CPPToolTip m_ToolTip;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VdclSliderCtrl)
	public:
	virtual BOOL Create(CDclControlObject* pControl, CWnd* pParentWnd, UINT nID );
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~VdclSliderCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(VdclSliderCtrl)
	afx_msg void OnOutofmemory(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnReleasedcapture(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnDestroy();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
