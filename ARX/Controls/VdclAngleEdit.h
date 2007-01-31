// VdclAngleEdit.h : header file
//

#pragma once

#include "ColorEdit.h"
#include "OleOdcDropTarget.h"
#include "PPToolTip.h"

class CPropertyObject;
class CDclControlObject;


/////////////////////////////////////////////////////////////////////////////
// VdclAngleEdit window

class VdclAngleEdit : public CColorEdit
{
// Construction
public:
	VdclAngleEdit();

// Attributes
public:
	bool			m_bFocusClick;
	CString m_sFilter;
	void SetTooltipText(CString* spText, BOOL bActivate = TRUE);
	void InitToolTip();
	CPPToolTip m_ToolTip;
	
	bool m_bInvokeWithSendString;
	CPropertyObject *m_pTextProp;
	void SetDragnDrop(BOOL bRegister);
	CComboBox		*m_pGridDropList;
// Implementation
public:
	//*****
    // COleOdcDropTarget, derived from COleDropTarget gives us
    // the functionality to be an OLE drop target.
    //*****
    COleOdcDropTarget m_DropTarget;

// Operations
public:
	void OnBadInput();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VdclAngleEdit)
	public:
	virtual BOOL Create(CDclControlObject* pControl, CWnd* pParentWnd, UINT nID );
	virtual BOOL Create(int nStyle, CRect rc, CWnd* pParentWnd, UINT nID);	
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	CString m_strOldValue;
	virtual ~VdclAngleEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(VdclAngleEdit)
	afx_msg void OnDestroy();
	afx_msg void OnChange();
	afx_msg void OnErrspace();
	afx_msg void OnKillfocus();
	afx_msg void OnMaxtext();
	afx_msg void OnSetfocus();
	afx_msg void OnUpdate();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	CDclControlObject *m_ArxControl;
};
