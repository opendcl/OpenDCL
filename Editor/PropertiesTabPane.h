// PropertiesTabPane.h : header file
//

#pragma once

#include "Resource.h"
#include "PropertyListCtrl.h"

#define nPropertyListID 1103
#define nPropertyListHeight 72
#define nPropertyDescWidth 55
#define nTitleHeight 15
#define nNotSet -1
#define nDeFontSize 8
#define nDePixels 72

/////////////////////////////////////////////////////////////////////////////
// CPropertiesTabPane dialog

class CPropertiesTabPane : public CDialog
{
// Construction
public:
	CPropertiesTabPane(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPropertiesTabPane)
	enum { IDD = IDD_TABPAGE_PROPERTIES };
	CStatic				m_PropertyTitle;
	CPropertyListCtrl	m_PropertyList;
	CEdit				m_PropertyDesc;
	CStatic				m_ControlDesc;
	CFont				m_font;
	//}}AFX_DATA
	bool m_bInitialized;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropertiesTabPane)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPropertiesTabPane)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
