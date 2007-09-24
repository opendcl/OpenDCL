// EventsTabPane.h : header file
//
#pragma once

#include "Resource.h"
#include "DclControlObject.h"

class CDclFormObject;
class COpenDCLView;
enum Prop::Id;


/////////////////////////////////////////////////////////////////////////////
// CEventsTabPane dialog

class CEventsTabPane : public CDialog
{
// Construction
public:
	CEventsTabPane(CWnd* pParent = NULL);   // standard constructor

private:
// Dialog Data
	enum { IDD = IDD_EVENTS };

	CEdit				m_EventDesc;
	CEdit				m_DefunPreview;
	CEdit				m_DefunEdit;
	CCheckListBox		m_EventsTree;
	bool m_bInitialized;
	TDclControlPtr m_pControl;
	CSize mszDlg;
public:
	COpenDCLView		*m_pView;
	
// operations
public:
	void UpdateEvents(TDclControlPtr pControl);
	void AddAnyActiveXEvents();
	void TryToAddEvent(Prop::Id nEvent);
	void ClearEvents();
	void SetDefunPreview();
	CString GetDefunArguments();
	void SetEvent(Prop::Id nEventId, CString sEventDefun);
	CString GetEvent(Prop::Id nEventId);

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
