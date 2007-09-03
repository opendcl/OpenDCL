// OdclEdit.h : header file
//

#pragma once

#include "OleOdcDropTarget.h"
#include "ColorEdit.h"
#include "PPToolTip.h"

class CDclControlObject;
class CPropertyObject;


/////////////////////////////////////////////////////////////////////////////
// OdclEdit window

class OdclEdit : public CColorEdit
{
// Construction
public:
	OdclEdit();
	CPPToolTip m_ToolTip;
	
// Attributes
public:
	bool			m_bFocusClick;
	bool			m_bAllowReturn;
	bool			m_bInvokeWithSendString;
	TPropertyPtr m_pTextProp;
	CString			m_sFilter;

	CComboBox		*m_pGridDropList;
	
// Operations
public:
	virtual void OnBadInput();
	void SetDragnDrop(BOOL bRegister);

// Implementation
public:
	//*****
    // COleOdcDropTarget, derived from COleDropTarget gives us
    // the functionality to be an OLE drop target.
    //*****
    COleOdcDropTarget m_DropTarget;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(OdclEdit)
	public:
	virtual BOOL Create(CDclControlObject* pControl, CWnd* pParentWnd, UINT nID );
	virtual BOOL Create(int nStyle, CRect rc, CWnd* pParentWnd, UINT nID);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:	
	CString m_strOldValue;
	virtual ~OdclEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(OdclEdit)
	afx_msg void OnDestroy();	
	afx_msg void OnChange();
	afx_msg void OnErrspace();
	afx_msg void OnKillFocus(/* CWnd* pFocus */);
	afx_msg void OnMaxtext();
	afx_msg void OnSetFocus(/* CWnd* pFocus */);
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
