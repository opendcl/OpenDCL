// OdclMonth.h : header file
//

#pragma once

#include "PPToolTip.h"
#include "PtrTypes.h"


/////////////////////////////////////////////////////////////////////////////
// OdclMonth window

class OdclMonth : public CMonthCalCtrl
{
	TDclControlPtr m_ArxControl;

public:
	CPPToolTip m_ToolTip;

public:
	OdclMonth();
	virtual ~OdclMonth();
	
// Overrides
	// ClassWizard generated virtual function overrides
public:
	virtual BOOL Create(TDclControlPtr pControl, CWnd* pParentWnd, UINT nID);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	// Generated message map functions
protected:
	afx_msg void OnDestroy();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnGetdaystate(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchange(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelect(NMHDR* pNMHDR, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()
};
