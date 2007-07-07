// AcadColorEdit.h : header file
//

#pragma once

#define nBackSpace 8
#define nReturnChar 13


/////////////////////////////////////////////////////////////////////////////
// CAcadColorEdit window

class CAcadColorEdit : public CEdit
{
// Construction
public:
	CAcadColorEdit();

// Attributes
public:
	CString m_sFilter;
	
// Operations
public:
	void OnBadInput();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAcadColorEdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAcadColorEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CAcadColorEdit)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
