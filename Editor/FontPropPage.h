// FontPropPage.h : header file
//

#pragma once

#include "Resource.h"

class CPropertyObject;


/////////////////////////////////////////////////////////////////////////////
// CFontPropertyPage dialog
class CFontPropertyPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CFontPropertyPage)

// Construction
public:
	CFontPropertyPage();
	~CFontPropertyPage();
	void GetCurrentFont(LPLOGFONT lplf);

// Dialog Data
	//{{AFX_DATA(CFontPropertyPage)
	enum { IDD = IDD_FONTPAGE };
	CButton	m_Frame;
	CButton	m_staticSample;
	CComboBox	m_comboStyle;
	CComboBox	m_comboSize;
	CComboBox	m_comboFont;
	CButton		m_ScaledOpt;
	CButton		m_PixelOpt;
	BOOL	m_bStrikeOut;
	BOOL	m_bUnderline;
	CString	m_sFont;
	CString	m_sSize;
	CString	m_sStyle;
	//}}AFX_DATA
	
	CString sObjectDCLProf;
	CPropertyObject *m_pFontName;
	CPropertyObject *m_pFontSize;
	CPropertyObject *m_pFontStrikeOut;
	CPropertyObject *m_pFontUnderline;
	CPropertyObject *m_pFontBold;
	CPropertyObject *m_pFontItalic;
	CPropertyObject *m_pFontSizeStyle;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CFontPropertyPage)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CFontPropertyPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelChange();
	afx_msg void OnDestroy();
	afx_msg void OnSelendokStyle();
	afx_msg void OnSelchangeFontsize();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CFont m_fontSample;
	int m_cyPixelsPerInch;
	static int CALLBACK FontEnumProc(ENUMLOGFONTEX *lpelfe, NEWTEXTMETRICEX *lpntme, 
		int FontType, CFontPropertyPage* pFontPage );
};
