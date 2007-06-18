// ZOrderListCtrl.h : header file
//

#pragma once

class COpenDCLView;

#define nValueSize 256
#define nType100 100
#define nImage29 29
#define nNotSet -1


/////////////////////////////////////////////////////////////////////////////
// CZOrderListCtrl window

class CZOrderListCtrl : public CListCtrl
{
// Construction
public:
	CZOrderListCtrl();

// Attributes
public:
	COpenDCLView *m_pView;
	CImageList		m_ImageList;	

// Operations
public:
	void AddControlToList(CString sName, int nType);
	void ClearList(COpenDCLView *pView);
	void RemoveControlFromList(CString sName);
	void RenameControlInList(CString sOldName, CString sNewName);
	void SelectItem(CString sName, bool bEnsureVisible);
	void ClearSelection();
	void Bringtofront();
	void SendtoBack();
	void BringtofrontBy1();
	void SentToBackBy1();
	int GetSelectedCount();
	void DoZOrderUpdate();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CZOrderListCtrl)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CZOrderListCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CZOrderListCtrl)
	afx_msg void OnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEditPaste();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
