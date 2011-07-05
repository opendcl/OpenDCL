#pragma once

#include "TipWnd.h"
#include <string>
#include <set>

#if defined(_UNICODE)
typedef std::wstring tstring;
#else
typedef std::string tstring;
#endif //_UNICODE


#define TRUETYPE_FONT		0x0001
#define PRINTER_FONT		0x0002
#define DEVICE_FONT			0x0004


class CFontObj : public CObject
{
public:
	BOOL	m_bInUse;	// Fonts in use 
	DWORD	m_nFlags;	// Font flags
public:
	CFontObj(DWORD nFlags)
	{	
		m_nFlags = nFlags;
		m_bInUse = FALSE;
	}

	CFontObj(CFontObj* pFontObj)
	{	
		m_nFlags = pFontObj->GetFlags();
		m_bInUse = pFontObj->GetFontInUse();
	}

	int GetFlags() const { return m_nFlags; }
	BOOL GetFontInUse() { return m_bInUse; }
	void SetFontInUse(BOOL bInUse) { m_bInUse = bInUse; }
};


/////////////////////////////////////////////////////////////////////////////
// CFontCombo window
class CFontCombo : public CComboBox
{
	std::set< tstring > msetHiddenFonts;

// Construction
public:
	CFontCombo();
protected:	
	void Initialize();
	DWORD GetFontTypeId( LPCTSTR pszFont );
	BOOL EnumerateFonts();
	void AddFont( LPCTSTR pszFontName, DWORD dwFlags );
	void SetCurrentFont();
	void SetFontInUse(const CString& strFont);
private:
	static BOOL CALLBACK AFX_EXPORT EnumFamScreenCallBackEx(
		ENUMLOGFONTEX* pelf, NEWTEXTMETRICEX* /*lpntm*/, int FontType, CFontCombo* pThis);
	bool IsHidden( LPCTSTR pszFont );

// Attributes
public:

protected:
	CTipWnd			m_wndTip;
	CString			m_strFontSave;
	CString			m_strDefault;
	CImageList		m_img;

	CTypedPtrMap<CMapStringToPtr, CString, CFontObj*> m_mapFonts;
	CTypedPtrMap<CMapStringToPtr, CString, CFontObj*> m_mruFonts;
 
	

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFontCombo)
	public:
	virtual void DeleteItem(LPDELETEITEMSTRUCT lpDeleteItemStruct);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CFontCombo();

	// Generated message map functions
protected:
	//{{AFX_MSG(CFontCombo)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnKillfocus();
	afx_msg void OnSetfocus();
	afx_msg void OnCloseUp();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
