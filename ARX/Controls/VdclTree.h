// VdclTree.h : header file
//

#pragma once

#include "TreeCtrlEx.h"

class CDclControlObject;


/////////////////////////////////////////////////////////////////////////////
// VdclTree window

class OdclTreeItem : public CObject
{ 
public:
	OdclTreeItem();
	~OdclTreeItem();
	HTREEITEM hItem;
	CString sKey;
	int m_ImageIndex;
	int m_SelectedImageIndex;
	int m_ExpandedImageIndex;
};


typedef CList<OdclTreeItem*> CTreeItemList;

class VdclTree : public CStatic//CTreeCtrl
{
// Construction
public:
	VdclTree();

// Attributes
public:
	bool m_bDeleting;
	CTreeCtrlEx m_ChildTree;
	bool m_bInvokeWithSendString;
	CTreeItemList m_TreeItems;
	CDclControlObject *m_ArxControl;
	bool m_bSingleExpand;
	HTREEITEM m_hOldItem;
	bool m_bAutoChangeSelection;
// Operations
public:
	
	void SetDragnDrop(BOOL bRegister);
	CString GetNextAvailableKey();
	OdclTreeItem *GetTreeItem(int nIndex);
	HTREEITEM Get_hItem(CString sKey);
	int FindItemIndex(CString sKey);
	CString Get_hItemKey(HTREEITEM hItem);

	void SelectItem(CString sKey);
	void Clear();
	HTREEITEM AddParent(CString sParentText, CString sKey, int nImage, int nSelectedImage, int nExpandedImage);
	HTREEITEM AddChild(CString sChildText, CString sParentKey, CString sKey, int nImage, int nSelectedImage, int nExpandedImage);
	HTREEITEM AddChild(CString sChildText, ULONG lParent, int nImage, int nSelectedImage, int nExpandedImage);
	
	bool GetParentInfo(CString sKey);
	bool GetParentInfo(HTREEITEM hItem);
	HTREEITEM AddAfter(CString sChildText, CString sRefKey, CString sKey, int nImage, int nSelectedImage, int nExpandedImage);
	HTREEITEM AddAfter(CString sChildText, ULONG lRefKey, int nImage, int nSelectedImage, int nExpandedImage);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VdclTree)
	public:
	virtual BOOL Create(CDclControlObject* pControl, CWnd* pParentWnd, UINT nID );
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~VdclTree();

	// Generated message map functions
protected:
	//{{AFX_MSG(VdclTree)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillfocus(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChildSetfocus(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRdblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnReturn(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeleteitem(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemexpanded(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemexpanding(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
