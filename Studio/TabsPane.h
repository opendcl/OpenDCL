// TabsPane.h : header file
//

#pragma once

#include "Resource.h"
#include "PictureBox.h"
#include "PropertyObject.h"

class CDclFormObject;
class CDclControlObject;
class COpenDCLView;

class CTabInfo
{
public:
	CString msCaption;
	int mnImageIndex;
	CString msToolTipTitle;
	int mnOriginalIndex;
	CDclFormObject* mpChildForm;
	CTabInfo(int nIndex = -1) : mnOriginalIndex( nIndex ), mpChildForm( NULL ) {}
};

typedef CList<CTabInfo*> CTabInfoList;


/////////////////////////////////////////////////////////////////////////////
// CTabsPane dialog

class CTabsPane : public CPropertyPage
{
	COpenDCLView* mpView;
	CDclControlObject* mpDclControl;
	RefCountedPtr< CImageList >& mpImageList; //address of image list pointer used by image list property page
	int mnTabIndex;

	CStatic	m_Frame;
	CEdit	m_Caption;
	CComboBoxEx	m_Image;
	CSpinButtonCtrl	m_SpinBtn;
	CEdit	m_ToolTipTitle;
	RefCountedPtr< CPropertyObject > m_pTabCaptions;	
	RefCountedPtr< CPropertyObject > m_pTabTTT;	
	RefCountedPtr< CPropertyObject > m_pTabImages;	
	CTabInfoList m_TabList;
	CTabInfoList m_DeletedTabList;

// Dialog Data
	enum { IDD = IDD_TABS };
	
	// Construction
public:
	CTabsPane( COpenDCLView* pView, CDclControlObject* pControl, RefCountedPtr< CImageList >& pImageList );   // standard constructor
	~CTabsPane();   // standard constructor

public:
	const CTabInfoList& GetTabList() const { return m_TabList; }
	CTabInfoList& GetTabList() { return m_TabList; }
	void Setup();

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
	DECLARE_MESSAGE_MAP()

protected:
	virtual BOOL OnInitDialog();
	virtual BOOL OnSetActive();
	afx_msg void OnDeltaposSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnAdd();
	afx_msg void OnDelete();
	afx_msg void OnChangeCaption();
	afx_msg void OnChangeImage();
	afx_msg void OnChangeTtt();
};
