// TabsPane.h : header file
//

#pragma once

#include "Resource.h"
#include "PictureBox.h"
#include "PropertyObject.h"

class CDclFormObject;
class CDclControlObject;
class CObjectDCLView;

// class for holding information about ActiveX methods so we do not
// have to keep going back to typeinfo
class CTabInfo : public CObject
{
public:

	CString m_sCaption;
	CString m_sToolTipText;
	int m_OriginalIndex;
	CDclFormObject *m_pChildForm;
	CTabInfo(int nIndex = -1) 
	{	
		m_OriginalIndex = nIndex;
		m_pChildForm = NULL;
	}	
};

typedef CList<CTabInfo*> CTabInfoList;


/////////////////////////////////////////////////////////////////////////////
// CTabsPane dialog

class CTabsPane : public CPropertyPage
{
	DECLARE_DYNCREATE(CTabsPane)
// Construction
public:
	CTabsPane();   // standard constructor
	~CTabsPane();   // standard constructor

	int m_nTabIndex;

	void Setup() ;

// Dialog Data
	//{{AFX_DATA(CTabsPane)
	enum { IDD = IDD_TABS };
	CStatic	m_Frame;
	CEdit	m_Caption;
	CPictureBox	m_Image;
	CSpinButtonCtrl	m_SpinBtn;
	CEdit	m_ToolTipText;
	//}}AFX_DATA

	CDclFormObject *m_pDclForm;
	CDclControlObject *m_pControl;
	RefCountedPtr< CPropertyObject > m_pTabCaptions;	
	RefCountedPtr< CPropertyObject > m_pTabTTT;	
	RefCountedPtr< CPropertyObject > m_pTabImages;	
	CTabInfoList m_TabList;
	CTabInfoList m_DeletedTabList;
	CObjectDCLView *m_pView;

	void UpdateTabInfo();
	void UpdateFrame();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTabsPane)
	public:
	virtual BOOL OnApply();	
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTabsPane)
	virtual BOOL OnInitDialog();
	afx_msg void OnDeltaposSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnAdd();
	afx_msg void OnChangeCaption();
	afx_msg void OnChangeTtt();
	afx_msg void OnDelete();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
