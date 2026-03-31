// TabsPane.h : header file
//

#pragma once

#include "Resource.h"
#include "PictureBox.h"
#include "PropertyObject.h"
#include "DclControlTemplate.h"
#include "DclFormTemplate.h"
#include "DclImageList.h"


class CTabInfo
{
public:
	CString msCaption;
	int mnImageIndex;
	CString msToolTipTitle;
	size_t mnOriginalIndex;
	TDclFormPtr mpChildForm;
	CTabInfo(int nIndex = -1) : mnOriginalIndex( nIndex ), mnImageIndex( -1 ), mpChildForm( NULL ) {}
	CTabInfo(TDclFormPtr pTabPage) : mnOriginalIndex( pTabPage? pTabPage->GetTabIndex() : -1 ), mnImageIndex( -1 ), mpChildForm( pTabPage ) {}
};

typedef CList<CTabInfo*> CTabInfoList;


/////////////////////////////////////////////////////////////////////////////
// CTabsPane dialog

class CTabsPane : public CPropertyPage
{
	TDclControlPtr mpDclControl;
	TImageListPtr mpImageList; //image list used by image list property page
	int mnTabIndex;

	CStatic	m_Frame;
	CEdit	m_Caption;
	CComboBoxEx	m_Image;
	CSpinButtonCtrl	m_SpinBtn;
	CEdit	m_ToolTipTitle;
	TPropertyPtr m_pTabCaptions;	
	TPropertyPtr m_pTabTTT;	
	TPropertyPtr m_pTabImages;	
	CTabInfoList m_TabList;
	CTabInfoList m_DeletedTabList;

// Dialog Data
	enum { IDD = IDD_TABS };
	
	// Construction
public:
	CTabsPane( TDclControlPtr pControl, TImageListPtr pImageList );   // standard constructor
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
	BOOL OnApply() override;
protected:
	void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()

protected:
	BOOL OnInitDialog() override;
	BOOL OnSetActive() override;
	afx_msg void OnDeltaposSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnAdd();
	afx_msg void OnDelete();
	afx_msg void OnChangeCaption();
	afx_msg void OnChangeImage();
	afx_msg void OnChangeTtt();
};
