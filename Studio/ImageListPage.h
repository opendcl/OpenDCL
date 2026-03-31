// ImageListPage.h : header file
//

#pragma once

#include "Resource.h"
#include "PropertyObject.h"
#include "DclControlTemplate.h"
#include "DclImageList.h"


/////////////////////////////////////////////////////////////////////////////
// CImageListPage dialog

class CImageListPage : public CPropertyPage
{
	friend class CPropertyListCtrl;

	TDclControlPtr mpDclControl;
	TImageListPtr mpImageList;

	enum { IDD = IDD_IMAGELIST };
	int	mnHeight;
	int	mnWidth;
	CListCtrl	mPicList;

public:
	CImageListPage( TDclControlPtr pDclControl );
	~CImageListPage();

	const TImageListPtr GetDclImageList() const { return mpImageList; }
	TImageListPtr GetDclImageList() { return mpImageList; }

protected:
	BOOL ImageListAddPicture( LPPICTUREDISP iPic );
	BOOL ImageListReplacePicture( int idxPic, LPPICTUREDISP iPic );

public:
	BOOL OnApply() override;
protected:
	void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnSelchange();
	afx_msg void OnAddimage();
	afx_msg void OnRemoveimage();
	afx_msg void OnChangeimage();
	BOOL OnInitDialog() override;
	BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) override;
	afx_msg void OnDropFiles(HDROP hDropInfo);
};
