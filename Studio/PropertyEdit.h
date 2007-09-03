// PropertyEdit.h : header file
//

#pragma once

#include "FormVarNameUpdate.h"
#include "PropertyObject.h"

class COpenDCLView;
class CControlHolder;
class CDclControlObject;

#define nCharReturn 13
#define nCharEsc	27
#define nCharBackSp	8	
#define nAnsweredYes 6

#pragma warning(disable:4150)

/////////////////////////////////////////////////////////////////////////////
// CPropertyEdit window

class CPropertyEdit : public CEdit
{
// Construction
public:
	CPropertyEdit();

// Attributes
public:
	CString m_OriginalValue;
	CString m_sFilter;
	bool m_bAllowReturn;
	TPropertyPtr m_pProp;
	CControlHolder *m_pAxContainer;
	CDclControlObject	*m_pControl;
	COpenDCLView		*m_pView;
// Operations
public:
	
	void CommitValue();
	void CommitList();
	void SetNameProp();
	void UpdateSizes();
	void UpdateVarName();


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropertyEdit)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPropertyEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPropertyEdit)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnChange();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
