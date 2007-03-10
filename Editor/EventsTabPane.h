// EventsTabPane.h : header file
//
#pragma once

#include "Resource.h"

class CDclControlObject;
class CDclFormObject;
class CObjectDCLView;
enum PropertyId;


/////////////////////////////////////////////////////////////////////////////
// CEventsTabPane dialog

class CEventsTabPane : public CDialog
{
// Construction
public:
	CEventsTabPane(CWnd* pParent = NULL);   // standard constructor

	CObjectDCLView		*m_pView;
// Dialog Data
	//{{AFX_DATA(CEventsTabPane)
	enum { IDD = IDD_EVENTS };
	CStatic				m_Label;
	CEdit				m_EventDesc;
	CEdit				m_DefunPreview;
	CEdit				m_DefunEdit;
	CCheckListBox		m_EventsTree;
	CButton				m_AddToLisp;
	CButton				m_AddCancel;
	//}}AFX_DATA
	bool m_bInitialized;
	CDclControlObject	*m_pControl;
	CDclFormObject		*m_pDclForm;
	
// operations
public:
	void CopyToClipboard();
	void UpdateEvents(CDclFormObject *pDclForm, CDclControlObject *pControl);
	void AddAnyActiveXEvents();
	void TryToAddEvent(PropertyId nEvent);
	void ClearEvents();
	void SetDefunPreview();
	CString GetDefunArguments();
	void SetEvent(PropertyId nEventId, CString sEventDefun);
	CString GetEvent(PropertyId nEventId);

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
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnChangeDefunedit();
	afx_msg void OnAddcancel();
	afx_msg void OnAddtolisp();
	afx_msg void OnSelchangeEventstree();
	//}}AFX_MSG
	afx_msg void OnCheckChanged();
	DECLARE_MESSAGE_MAP()
};
