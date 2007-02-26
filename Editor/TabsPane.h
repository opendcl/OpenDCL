// TabsPane.h : header file
//

#pragma once

#include "Resource.h"
#include "PictureBox.h"
#include "PropertyObject.h"

class CDclFormObject;
class CDclControlObject;
class CObjectDCLView;

class CTabInfo
{
public:

	CString msCaption;
	CString msToolTipText;
	int mnOriginalIndex;
	CDclFormObject* mpChildForm;
	CTabInfo(int nIndex = -1) : mnOriginalIndex( nIndex ), mpChildForm( NULL ) {}
};

typedef CList<CTabInfo*> CTabInfoList;


/////////////////////////////////////////////////////////////////////////////
// CTabsPane dialog

class CTabsPane : public CPropertyPage
{
	CObjectDCLView* mpView;
	CDclControlObject* mpDclControl;
	int mnTabIndex;

	CStatic	m_Frame;
	CEdit	m_Caption;
	CPictureBox	m_Image;
	CSpinButtonCtrl	m_SpinBtn;
	CEdit	m_ToolTipText;
	RefCountedPtr< CPropertyObject > m_pTabCaptions;	
	RefCountedPtr< CPropertyObject > m_pTabTTT;	
	RefCountedPtr< CPropertyObject > m_pTabImages;	
	CTabInfoList m_TabList;
	CTabInfoList m_DeletedTabList;

// Dialog Data
	enum { IDD = IDD_TABS };
	
	// Construction
public:
	CTabsPane( CObjectDCLView* pView, CDclControlObject* pControl );   // standard constructor
	~CTabsPane();   // standard constructor

public:
	const CTabInfoList& GetTabList() const { return m_TabList; }
	CTabInfoList& GetTabList() { return m_TabList; }
	void Setup() ;

protected:
	void UpdateTabInfo();
	void UpdateFrame();

// Overrides
public:
	virtual BOOL OnApply();	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	virtual BOOL OnInitDialog();
	afx_msg void OnDeltaposSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnAdd();
	afx_msg void OnChangeCaption();
	afx_msg void OnChangeTtt();
	afx_msg void OnDelete();

protected:
	DECLARE_MESSAGE_MAP()
};
