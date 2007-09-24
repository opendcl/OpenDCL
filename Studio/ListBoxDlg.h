// ListBoxDlg.h : header file
//

#pragma once

#include "Resource.h"
#include "DclControlObject.h"

class COpenDCLView;
class CPropertyObject;
class CControlHolder;


/////////////////////////////////////////////////////////////////////////////
// CListBoxDlg dialog

class CListBoxDlg : public CDialog
{
// Construction
public:
	CListBoxDlg(CWnd* pParent = NULL);   // standard constructor
	void AddString(CString NewString);
	void MoveWindow(CRect *pRect);
	void SetPropertyPointer(TPropertyPtr pPropObject);
	CControlHolder *m_pAxContainer;
// Dialog Data
	//{{AFX_DATA(CListBoxDlg)
	enum { IDD = IDD_LISTDIALOG };
	CListBox	m_ListBox;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListBoxDlg)
	public:
	virtual BOOL Create();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
	CWnd* m_pParent;
	int m_nID;

	// Generated message map functions
	//{{AFX_MSG(CListBoxDlg)
	afx_msg void OnSelchangeListbox();
	afx_msg void OnKillfocusListbox();
	afx_msg void OnSetfocusListbox();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	BOOL m_ListBoxCreated;
	TPropertyPtr m_pPropObject;
	TDclControlPtr m_pControl;
	COpenDCLView *m_pView;
};
