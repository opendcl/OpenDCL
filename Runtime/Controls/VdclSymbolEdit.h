// VdclSymbolEdit.h : header file
//

#pragma once

#include "OleOdcDropTarget.h"
#include "PPToolTip.h"

class CDclControlObject;
class CPropertyObject;


/////////////////////////////////////////////////////////////////////////////
// VdclSymbolEdit window

class VdclSymbolEdit : public CAcUiSymbolEdit
{
// Construction
public:
	VdclSymbolEdit();

// Attributes
public:
	bool m_bInvokeWithSendString;
	TPropertyPtr m_pTextProp;
	CBrush *m_pStaticBrush;		
	COLORREF m_ForeColor;
	void OnBadInput();
	bool m_bFocusClick;
	CComboBox *m_pGridDropList;
// Operations
public:
	CPPToolTip m_ToolTip;
	void SetDragnDrop(BOOL bRegister);
	void SetAcadColor(long nColor);
	void SetForeColor(long nColor);

// ListCtrl edit data members
	bool m_bESC;

// Implementation
public:
	//*****
    // COleOdcDropTarget, derived from COleDropTarget gives us
    // the functionality to be an OLE drop target.
    //*****
    COleOdcDropTarget m_DropTarget;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VdclSymbolEdit)
public:
	virtual BOOL Create(CDclControlObject* pControl, CWnd* pParentWnd, UINT nID );
	virtual BOOL Create(int nStyle, CRect rc, CWnd* pParentWnd, UINT nID);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	CString m_strOldValue;
	virtual ~VdclSymbolEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(VdclSymbolEdit)
	afx_msg void OnDestroy();
	afx_msg void OnChange();
	afx_msg void OnErrspace();
	afx_msg void OnKillfocus();
	afx_msg void OnMaxtext();
	afx_msg void OnSetfocus();
	afx_msg void OnUpdate();
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	CDclControlObject *m_ArxControl;
};
