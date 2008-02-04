// ColumnsPage.h : header file
//

#pragma once


#include "Resource.h"
#include "ListHeader.h"
#include "PropertyObject.h"
#include "DclControlObject.h"
#include "PtrTypes.h"

class CImageListPage;
class CDclFormObject;
class CDclControlObject;


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
	TDclControlPtr mpDclControl;
	CImageListPage* mpImageListPage;

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

	bool m_bShowStyles;
	INT_PTR m_nIndex;
	CListHeader    m_HeaderCtrl;
	bool m_bChangingIndex;
	bool bUsesRowHeader;
	
	TPropertyPtr m_pColCaptions;	
	TPropertyPtr m_pColWidths;	
	TPropertyPtr m_pColImages;	
	TPropertyPtr m_pColStyles;	
	TPropertyPtr m_pColAlignment;	
	TPropertyPtr m_pColDefault;	
	TPropertyPtr m_pColAlternate;	
	TPropertyPtr m_pColListItems;	
	TPropertyPtr m_pColImageItems;	

	CArray<CColumnData, CColumnData> m_ColData;										 

	enum { IDD = IDD_COLUMNS };

// Construction
public:
	CColumnsPage( TDclControlPtr pDclControl, CImageListPage* pImageListPage );
	~CColumnsPage();

// Operators
	bool IsImageListValid();
	void SetColumn(INT_PTR nIndex);
	void SetControls(INT_PTR nIndex);
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
