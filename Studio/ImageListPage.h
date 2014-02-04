// ImageListPage.h : header file
//

#pragma once

#include "Resource.h"
#include "PropertyObject.h"
#include "DclControlTemplate.h"


/////////////////////////////////////////////////////////////////////////////
// CImageListPage dialog

class CImageListPage : public CPropertyPage
{
	friend class CPropertyListCtrl;

	TDclControlPtr mpDclControl;
	RefCountedPtr< CImageList > mpImageList;

	enum { IDD = IDD_IMAGELIST };
	int	mnHeight;
	int	mnWidth;
	CListCtrl	mPicList;

public:
	CImageListPage( TDclControlPtr pDclControl );
	~CImageListPage();

	RefCountedPtr< CImageList >& GetImageList() { return mpImageList; }

protected:
	BOOL ImageListAddPicture( LPPICTUREDISP iPic );
	BOOL ImageListReplacePicture( int idxPic, LPPICTUREDISP iPic );

public:
	virtual BOOL OnApply();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnSelchange();
	afx_msg void OnAddimage();
	afx_msg void OnRemoveimage();
	afx_msg void OnChangeimage();
	virtual BOOL OnInitDialog();
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
};
