// EventsTabPane.h : header file
//
#pragma once

#include "Resource.h"
#include "DclControlObject.h"

class CDclFormObject;
class CStudioDialogObject;


/////////////////////////////////////////////////////////////////////////////
// CEventsTabPane dialog

class CEventsTabPane : public CDialog
{
	TDclControlPtr mpDclControl;
	CCheckListBox mEventsList;
	CEdit mEventDesc;
	CEdit mDefunPreview;
	CEdit mDefunEdit;
	CSize mszDlg;

	enum { IDD = IDD_EVENTS };

public:
	CEventsTabPane( CWnd* pParent = NULL );   // standard constructor
	
// operations
public:
	void UpdateEvents(TDclControlPtr pControl);
	void AddEvents();
	void ClearEvents();
	void SetDefunPreview();
	void SetEvent( size_t idxProp, LPCTSTR pszEventDefun );
	CString GetEvent( size_t idxProp );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEventsTabPane)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEventsTabPane)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnChangeDefunedit();
	afx_msg void OnAddcancel();
	afx_msg void OnAddtolisp();
	afx_msg void OnCopytoclipboard();
	afx_msg void OnSelchangeEventstree();
	//}}AFX_MSG
	afx_msg void OnCheckChanged();
	DECLARE_MESSAGE_MAP()
};
