// ToolTipsPage.h : header file
//

#pragma once


#include "Resource.h"
#include "PPToolTip.h"
#include "ButtonST.h"
#include "ColourPicker.h"
//#include "FontCombo.h"
//#include "FontSizes.h"
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
	CAutoRichEditCtrl	m_MainText;
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
	
	CPropertyObject *m_pToolTipText;	
	CPropertyObject *m_pToolTipLine;	
	CPropertyObject *m_pToolTipBody;	
	CPropertyObject *m_pToolTipPicture;	
	CPropertyObject *m_pToolTipAvi;	
	CPropertyObject *m_pToolTipTitleColor;

	void Commit();
	CString GetHtmlText();

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CToolTipsPage)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnApply();
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CToolTipsPage)
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
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
