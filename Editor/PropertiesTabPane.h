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
	CPropertyListCtrl	mPropListCtrl;

	enum { IDD = IDD_TABPAGE_PROPERTIES };

public:
	CStatic				m_PropertyTitle;
	CEdit				m_PropertyDesc;
	CStatic				m_ControlDesc;
	CFont				m_font;
	bool m_bInitialized;

// Construction
public:
	CPropertiesTabPane(CWnd* pParent = NULL);   // standard constructor

public:
	const CPropertyListCtrl& GetPropertiesCtrl() const { return mPropListCtrl; }
	CPropertyListCtrl& GetPropertiesCtrl() { return mPropListCtrl; }

// Overrides
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();

protected:
	DECLARE_MESSAGE_MAP()
};
