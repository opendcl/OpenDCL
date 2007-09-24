// ZOrderListCtrl.h : header file
//

#pragma once

#include "DclControlObject.h"

class COpenDCLView;

#define nValueSize 256
#define nImage29 29


/////////////////////////////////////////////////////////////////////////////
// CZOrderListCtrl window

class CZOrderListCtrl : public CListCtrl
{
// Construction
public:
	CZOrderListCtrl();
	virtual ~CZOrderListCtrl();

// Attributes
public:
	COpenDCLView *m_pView;
	CImageList		m_ImageList;	

// Operations
public:
	void AddControlToList( TDclControlPtr pDclControl, bool bSelected = true, bool bEnsureVisible = false );
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

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEditPaste();
	afx_msg void OnCustomdraw(NMHDR* pNMHDR, LRESULT* pResult);
};
