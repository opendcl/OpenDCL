// PropertiesTabPane.h : header file
//

#pragma once

#include "Resource.h"
#include "PropertyGridCtrl.h"
//#include "PropertyListCtrl.h"

#define nPropertyListID 1103
#define nPropertyListHeight 72
#define nPropertyDescWidth 55
#define nDeFontSize 8
#define nDePixels 72

/////////////////////////////////////////////////////////////////////////////
// CPropertiesTabPane dialog

class CPropertiesTabPane : public CDialog
{
	CPropertyGridCtrl	mPropGridCtrl;
	CFont mFont;
	bool mbInitialized;

	enum { IDD = IDD_TABPAGE_PROPERTIES };

// Construction
public:
	CPropertiesTabPane( const std::vector< TDclControlPtr >& ActiveControls, CWnd* pParent = NULL );   // standard constructor

public:
	const CPropertyGridCtrl& GetPropertiesCtrl() const { return mPropGridCtrl; }
	CPropertyGridCtrl& GetPropertiesCtrl() { return mPropGridCtrl; }
	void ActivateProperty( TPropertyPtr pProp );

// Overrides
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();

protected:
	DECLARE_MESSAGE_MAP()
};
