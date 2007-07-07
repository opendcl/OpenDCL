// ImageListPage.h : header file
//

#pragma once

class CImageListObject;

#include "Resource.h"
#include "PropertyObject.h"

class CDclControlObject;
class CImageListObject;

/////////////////////////////////////////////////////////////////////////////
// CImageListPage dialog

class CImageListPage : public CPropertyPage
{
	//DECLARE_DYNCREATE(CImageListPage)

	CDclControlObject* mpDclControl;
	RefCountedPtr< CImageList > mpImageList;

// Construction
//protected:
//	CImageListPage();
public:
	CImageListPage( CDclControlObject* pDclControl );
	~CImageListPage();

// Dialog Data
	//{{AFX_DATA(CImageListPage)
	enum { IDD = IDD_IMAGELIST };
	CEdit	m_DispHeight;
	CEdit	m_DispWidth;
	CListCtrl	m_PicList;
	//}}AFX_DATA

	CImageList& GetImageList() { return *mpImageList; }

	//RefCountedPtr< CPropertyObject > m_pImageListIndex;
	//CDclFormObject *m_pDclForm;
	//CImageListObject *m_pImageListObj;
	//bool m_bImageListCreated;
	void LoadPictureFile(LPCTSTR szFile);
	BOOL ImageListAddPicture(LPPICTUREDISP iPic);
	int nCurrentWidth;
	int nCurrentHeight;
// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CImageListPage)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CImageListPage)
	afx_msg void OnAddimage();
	afx_msg void OnRemoveimage();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
