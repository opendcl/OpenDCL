#if !defined(AFX_OdclDirStylesCombo_H__E7C5C370_F683_4720_9C61_7547F8613936__INCLUDED_)
#define AFX_OdclDirStylesCombo_H__E7C5C370_F683_4720_9C61_7547F8613936__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OdclDirStylesCombo.h : header file
//
#include "PPToolTip.h"

/////////////////////////////////////////////////////////////////////////////
// OdclDirStylesCombo window

class OdclDirStylesCombo : public CComboBox
{
// Construction
public:
	OdclDirStylesCombo();

// Attributes
public:
	bool	m_bTextStyle;
// Operations
public:
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(OdclDirStylesCombo)
	public:
	//}}AFX_VIRTUAL
	BOOL Create(int nStyle, CRect rc, CWnd* pParentWnd, UINT nID);
// Implementation
public:
	virtual ~OdclDirStylesCombo();

	// Generated message map functions
protected:
	//{{AFX_MSG(OdclDirStylesCombo)
	afx_msg void OnSelchange();
	afx_msg void OnDropdown();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OdclDirStylesCombo_H__E7C5C370_F683_4720_9C61_7547F8613936__INCLUDED_)
