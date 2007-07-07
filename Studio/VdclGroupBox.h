// VdclGroupBox.h : header file
//

#pragma once


/////////////////////////////////////////////////////////////////////////////
// VdclGroupBox window

class VdclGroupBox : public CStatic
{
// Construction
public:
	VdclGroupBox();

// Attributes
public:
	CButton m_Frame;
	BOOL m_bCreated;
	void SetCaption(CString sCaption);
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VdclGroupBox)
	public:
	virtual BOOL Create(CString sCaption, CRect rcRect, CWnd* pParentWnd, UINT nID );
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~VdclGroupBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(VdclGroupBox)
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
