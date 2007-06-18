// ColumnsPage.h : header file
//

#pragma once


class CImageListPage;

#include "Resource.h"
#include "ListHeader.h"
#include "PropertyObject.h"

class CImageListPage;
class CDclFormObject;
class CDclControlObject;
class COpenDCLView;


class CColumnData : public CObject
{
// Construction
public:
	CColumnData();
	~CColumnData();
	CColumnData(const CColumnData &Other);
	CColumnData& operator=(const CColumnData& other);
public:
	CString m_Caption;	
	int		m_Width;	
	int		m_Image;	
	int		m_Style;	
	int		m_Alignment;	
	int		m_Default;	
	int		m_Alternate;
	CStringArray m_ListItems;
	CArray<int, int> m_ImageItems;	
};


/////////////////////////////////////////////////////////////////////////////
// CColumnsPage dialog

class CColumnsPage : public CPropertyPage
{
	CStatic	m_StyleTitle;
	CEdit	m_IndexEdit;
	CEdit	m_FileExt;
	CComboBox	m_Times;
	CComboBox	m_Dates;
	CButton	m_DropListBtn;
	CStatic	m_AltLabel;
	CStatic	m_DefLabel;
	CComboBox	m_Disabled;
	CComboBoxEx	m_Default;
	CComboBoxEx	m_Alternate;
	CEdit	m_WidthTextBox;
	CEdit	m_WidthEdit;
	CComboBoxEx	m_Image;
	CEdit	m_Text;
	CComboBox	m_Style;
	CSpinButtonCtrl	m_Spin;
	CListCtrl	m_List;
	CStatic	m_Index;
	CComboBox	m_Alignment;

// Dialog Data
	enum { IDD = IDD_COLUMNS };

// members
public:
	bool m_bShowStyles;
	int  m_nIndex;
	CListHeader    m_HeaderCtrl;
	bool m_bChangingIndex;
	CImageListPage *m_pImageListPage;
	bool bUsesRowHeader;
	
	RefCountedPtr< CPropertyObject > m_pColCaptions;	
	RefCountedPtr< CPropertyObject > m_pColWidths;	
	RefCountedPtr< CPropertyObject > m_pColImages;	
	RefCountedPtr< CPropertyObject > m_pColStyles;	
	RefCountedPtr< CPropertyObject > m_pColAlignment;	
	RefCountedPtr< CPropertyObject > m_pColDefault;	
	RefCountedPtr< CPropertyObject > m_pColAlternate;	
	RefCountedPtr< CPropertyObject > m_pColListItems;	
	RefCountedPtr< CPropertyObject > m_pColImageItems;	

	CArray<CColumnData, CColumnData> m_ColData;										 
	COpenDCLView *m_pView;
	CDclFormObject *m_pDclForm;
	CDclControlObject *m_pControl;

// Construction
public:
	CColumnsPage();
	~CColumnsPage();

// Operators
	bool IsImageListValid();
	void SetColumn(int nIndex);
	void SetControls(int nIndex);
	void CommitLists();
	void ResetWidths();
	void OnColumnHeaderClicked(int nIndex);

	// Overrides
	public:
	virtual BOOL OnApply();
	virtual void OnOK();
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	virtual BOOL OnInitDialog();
	afx_msg void OnInsert();
	afx_msg void OnChangeText();
	afx_msg void OnChangeWidth();
	afx_msg void OnSelchangeAlignment();
	afx_msg void OnSelchangeStyle();
	afx_msg void OnSelchangeImage();
	afx_msg void OnDeltaposSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDelete();
	afx_msg void OnSelchangeDefault();
	afx_msg void OnSelchangeAlternate();
	afx_msg void OnDroplistbtn();
	afx_msg void OnDestroy();
	afx_msg void OnSelchangeDate();
	afx_msg void OnSelchangeTime();
	afx_msg void OnChangeFileext();
	afx_msg void OnSetfocusIndexEdit();

protected:
	DECLARE_MESSAGE_MAP()
};
