// ToolTipsPage.h : header file
//

#pragma once


#include "Resource.h"
#include "PPToolTip.h"
#include "ButtonST.h"
#include "ColourPicker.h"
#include "AutoRichEditCtrl.h"

class CPropertyObject;


/////////////////////////////////////////////////////////////////////////////
// CToolTipsPage dialog

class CToolTipsPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CToolTipsPage)

// Construction
public:
	CToolTipsPage();
	~CToolTipsPage();

// Dialog Data
	//{{AFX_DATA(CToolTipsPage)
	enum { IDD = IDD_TOOLTIPS };
	CButtonST	m_Color2;
	CButtonST	m_UnderLine;
	CButtonST	m_Bold;
	CEdit	m_MainText;
	CButtonST	m_Italic;
	CListBox	m_Pictures;
	CEdit	m_Avi;
	CEdit	m_Title;
	CButton	m_Line;
	CColourPicker	m_Color;
	//}}AFX_DATA

	CPPToolTip m_tooltip;
	int m_SelectedPic;
	int m_nHighestId;
	
	RefCountedPtr< CPropertyObject > m_pToolTipTitle;	
	RefCountedPtr< CPropertyObject > m_pToolTipLine;	
	RefCountedPtr< CPropertyObject > m_pToolTipBody;	
	RefCountedPtr< CPropertyObject > m_pToolTipPicture;	
	RefCountedPtr< CPropertyObject > m_pToolTipAvi;	
	RefCountedPtr< CPropertyObject > m_pToolTipTitleColor;

	void Commit();
	void ModifySelection( LPCTSTR pszPrefix, LPCTSTR pszSuffix );

protected:
	DECLARE_MESSAGE_MAP()

// Overrides
protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnApply();
	virtual void OnOK();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	virtual BOOL OnInitDialog();
	afx_msg void OnPreview();
	afx_msg void OnBold();
	afx_msg void OnItalic();
	afx_msg void OnUnderline();
	afx_msg void OnColor2();
	afx_msg void OnChangeAvi();
	afx_msg void OnColor();
	afx_msg void OnLine();
	afx_msg void OnChangeMaintext();
	afx_msg void OnSelchangePiclist();
};
