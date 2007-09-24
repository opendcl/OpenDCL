// TabPage.h : header file
//

#pragma once

#include "Resource.h"
#include "ArxControlPane.h"

class CDclFormObject;

typedef RefCountedPtr< class CTabPage > TTabPagePtr;


/////////////////////////////////////////////////////////////////////////////
// CTabPage dialog

class CTabPage : public CDialog
{
protected:
	TDclFormPtr mpSourceForm;
	CArxControlPane mControlPane;

	enum { IDD = IDD_TABPAGE };

// Construction
public:
	CTabPage( TDclFormPtr pSourceForm, CTabCtrl* pTabCtrl, CRect rectPane, UINT& nId );
	~CTabPage();

	//Attributes
	const TDclFormPtr GetSourceForm() const { return mpSourceForm; }
	TDclFormPtr GetSourceForm() { return mpSourceForm; }
	const CControlPane& GetControlPane() const { return mControlPane; }
	CControlPane& GetControlPane() { return mControlPane; }

// Implementation
protected:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

protected:
	DECLARE_MESSAGE_MAP()
};
