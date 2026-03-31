// EventsTabPane.h : header file
//
#pragma once

#include "Resource.h"
#include "DclControlTemplate.h"

class CDclFormObject;
class CStudioDialogObject;


/////////////////////////////////////////////////////////////////////////////
// CEventsTabPane dialog

class CEventsTabPane : public CDialog
{
	TDclControlPtr mpDclControl;
	CCheckListBox mEventsList;
	CStatic mStatusMsg;
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
	void ClearEvents( LPCTSTR pszErrMsg = NULL );
	void SetDefunPreview();
	void SetEvent( size_t idxProp, LPCTSTR pszEventDefun );
	CString GetEvent( size_t idxProp );

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

protected:
	BOOL PreTranslateMessage(MSG* pMsg) override;
	void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	BOOL OnInitDialog() override;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnChangeDefunedit();
	afx_msg void OnAddcancel();
	afx_msg void OnAddtolisp();
	afx_msg void OnCopytoclipboard();
	afx_msg void OnDblClkEventstree();
	afx_msg void OnSelchangeEventstree();
	afx_msg void OnCheckChanged();
};
