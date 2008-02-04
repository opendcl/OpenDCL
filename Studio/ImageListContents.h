// ImageListContents.h : header file
//

#pragma once

#include "Resource.h"

class CImageListPage;


/////////////////////////////////////////////////////////////////////////////
// CImageListContents dialog

class CImageListContents : public CDialog
{
// Construction
public:
	CImageListContents(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CImageListContents)
	enum { IDD = IDD_IMAGELISTCONTENTS };
	CComboBoxEx	m_Image;
	CEdit		m_Edit;
	CListCtrl	m_TheList;
	//}}AFX_DATA

	CImageListPage *m_pImageListPage;
	CStringArray	sStrings;
	CArray<int,int> sImages;

	void Changed(int nItem);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImageListContents)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CImageListContents)
	afx_msg void OnAdd();
	afx_msg void OnDelete();
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnClickThelist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkThelist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeleditThelist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBeginlabeleditThelist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedThelist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydownThelist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeEdit1();
	afx_msg void OnSelchangeComboboxex();
	afx_msg void OnUpdate();
	virtual void OnOK();
	afx_msg void OnOdstatechangedThelist(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
