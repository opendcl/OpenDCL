// ToolTipsPage.h : header file
//

#pragma once


#include "Resource.h"
#include "PPToolTip.h"
#include "ButtonST.h"
#include "ColourPicker.h"

class CPropertyObject;


/////////////////////////////////////////////////////////////////////////////
// CToolTipsPage dialog

class CToolTipsPage : public CPropertyPage
{
	TDclControlPtr mpDclControl;

// Dialog Data
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
	CButton m_Balloon;

	CPPToolTip m_tooltip;
	int m_SelectedPic;
	UINT m_nHighestId;
	
	TPropertyPtr m_pToolTipBalloon;	
	TPropertyPtr m_pToolTipTitle;	
	TPropertyPtr m_pToolTipLine;	
	TPropertyPtr m_pToolTipBody;	
	TPropertyPtr m_pToolTipPicture;	
	TPropertyPtr m_pToolTipAvi;	
	TPropertyPtr m_pToolTipTitleColor;

// Construction
public:
	CToolTipsPage( TDclControlPtr pDclControl );
	~CToolTipsPage();

protected:
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
	afx_msg void OnChangeTitle();
	afx_msg void OnSelchangePiclist();
	afx_msg void OnBalloon();
};
