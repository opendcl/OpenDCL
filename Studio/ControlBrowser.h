#pragma once

#include "ResizableDialog.h"
#include "AutoRichEditCtrl.h"
#include "Resource.h"

class CTreeNode;


// CControlBrowser dialog

class CControlBrowser : public CResizableDialog
{
	TDclControlPtr mpDclControl;
	CImageList mImageList;
	CTreeCtrl mObjectTree;
	CSize mszPrevious;
	CAutoRichEditCtrl mDescription;
	CString msCopy1Lisp;
	CString msCopy2Lisp;

// Dialog Data
	enum { IDD = IDD_CONTROLBROWSER };

public:
	CControlBrowser(TDclControlPtr pDclControl, CWnd* pParent = NULL);   // standard constructor
	virtual ~CControlBrowser();

public:
	HTREEITEM InsertItem( HTREEITEM hParent, CTreeNode* pItem );
	void RemoveItem( HTREEITEM hTarget );
	TDclControlPtr GetMainControl() const { return mpDclControl; }
	void SetDescription( LPCTSTR pszDescription ) { mDescription.SetRTF( pszDescription ); }
	void SetCopy1Lisp( LPCTSTR pszLisp ) { msCopy1Lisp = pszLisp; }
	void SetCopy2Lisp( LPCTSTR pszLisp ) { msCopy2Lisp = pszLisp; }

protected:
	bool OnBeginClipboardCopy();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual INT_PTR DoModal();
protected:
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCopy1();
	afx_msg void OnCopy2();
	afx_msg void OnCopy3();
};
