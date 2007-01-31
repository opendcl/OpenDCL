// TabPage.h : header file
//

#pragma once

#include "Resource.h"
#include "ControlPane.h"

class CDclFormObject;


/////////////////////////////////////////////////////////////////////////////
// CTabPage dialog

class CTabPage : public CDialog
{
protected:
	CDclFormObject* mpSourceForm;
	CControlPane mControlPane;

// Construction
public:
	CTabPage( CDclFormObject* pSourceForm );
	~CTabPage();

	//Attributes
	const CDclFormObject* GetSourceForm() const { return mpSourceForm; }
	CDclFormObject* GetSourceForm() { return mpSourceForm; }
	const CControlPane& GetControlPane() const { return mControlPane; }
	CControlPane& GetControlPane() { return mControlPane; }


// Dialog Data
	//{{AFX_DATA(CTabPage)
	enum { IDD = IDD_TABPAGE2 };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTabPage)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual void OnOK();
	virtual void OnCancel();	
	// Generated message map functions
	//{{AFX_MSG(CTabPage)
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
