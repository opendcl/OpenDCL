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
	CDclFormObject* mpSourceForm;
	CArxControlPane mControlPane;

	enum { IDD = IDD_TABPAGE2 };

// Construction
public:
	CTabPage( CDclFormObject* pSourceForm, CTabCtrl* pTabCtrl, CRect rectPane, UINT& nId );
	~CTabPage();

	//Attributes
	const CDclFormObject* GetSourceForm() const { return mpSourceForm; }
	CDclFormObject* GetSourceForm() { return mpSourceForm; }
	const CControlPane& GetControlPane() const { return mControlPane; }
	CControlPane& GetControlPane() { return mControlPane; }

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
};
