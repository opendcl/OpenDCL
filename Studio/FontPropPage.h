// FontPropPage.h : header file
//

#pragma once

#include "Resource.h"
#include "DclControlTemplate.h"
#include <string>
#include <set>

class CPropertyObject;

#if defined(_UNICODE)
typedef std::wstring tstring;
#else
typedef std::string tstring;
#endif //_UNICODE


/////////////////////////////////////////////////////////////////////////////
// CFontPropertyPage dialog
class CFontPropertyPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CFontPropertyPage)

	TDclControlPtr mpDclControl;
	std::set< tstring > msetHiddenFonts;

// Construction
public:
	CFontPropertyPage(TDclControlPtr pDclControl = NULL);
	~CFontPropertyPage();

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
	

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CFontPropertyPage)
public:
	BOOL OnApply() override;
protected:
	void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CFontPropertyPage)
	BOOL OnInitDialog() override;
	afx_msg void OnSelChange();
	afx_msg void OnDestroy();
	afx_msg void OnSelendokStyle();
	afx_msg void OnSelchangeFontsize();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CFont m_fontSample;
	static int CALLBACK FontEnumProc(ENUMLOGFONTEX *lpelfe, NEWTEXTMETRICEX *lpntme, 
		int FontType, CFontPropertyPage* pFontPage );
	bool IsHidden( LPCTSTR pszFont );
};
