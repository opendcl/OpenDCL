// ParentDlg.h : header file
//

#pragma once

#include "Resource.h"
#include "ControlPane.h"

class CDialogControl;
class CFontCollection;
class CDclControlObject;


/////////////////////////////////////////////////////////////////////////////
// CParentDlg dialog

class CParentDlg : public CCommonDialog
{
protected:
	CDclFormObject* mpSourceForm;
	CControlPane mControlPane;
	CDclControlObject* mpControl;

public:
	CList<CArxDialogControl*>			m_ControlCol;
	CFontCollection		*m_pFontCollection;	
	CWnd				*m_pMainChild;

	BOOL m_bShowGrip;	
	BOOL m_bInitDone;			// if all internal vars initialized
	POINT m_ptMinTrackSize;		// min tracking size
	SIZE m_szGripSize;			// set at construction time	
	CRect m_rcGripRect;			// current pos of grip

// Construction
public:
	CParentDlg(CDclFormObject* pSourceForm, CWnd* pParent = NULL);   // standard constructor

	//Attributes
	const CDclFormObject* GetSourceForm() const { return mpSourceForm; }
	CDclFormObject* GetSourceForm() { return mpSourceForm; }
	const CControlPane& GetControlPane() const { return mControlPane; }
	CControlPane& GetControlPane() { return mControlPane; }
	const CDclControlObject* GetControl() const { return mpControl; }
	CDclControlObject* GetControl() { return mpControl; }

// Dialog Data
	enum { IDD = IDD_CUSTOM_FILE_DIALOG };

	
	void UpdateGripPos();
	void ShowSizeGrip(BOOL bShow);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CParentDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	UINT OnNcHitTest(CPoint point);
	// Generated message map functions
	//{{AFX_MSG(CParentDlg)
	afx_msg void OnCancel();
	afx_msg void OnOK();
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
