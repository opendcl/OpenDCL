// VdclTab.h : header file
//

#pragma once

#include "PPToolTip.h"

class CDclControlObject;
class CDclFormObject;
class CTabPage;


class CTabTracker : public CObject
{
public:
	CTabTracker()
	{ 
		m_pTabPage = NULL;
		m_pDclForm = NULL;
		m_nIndex   = 0;
	};
	CDclFormObject	*m_pDclForm;
	CTabPage		*m_pTabPage;
	int				m_nIndex;
};


typedef CList<CTabTracker*> CTabsList;


/////////////////////////////////////////////////////////////////////////////
// VdclTab window

class VdclTab : public CStatic //CTabCtrl
{
// Construction
public:
	VdclTab();

// Attributes
public:
	bool m_bInvokeWithSendString;
	int m_nCurrentSelectedTab;
	CRect m_rcPos;
	CTabsList m_ArxTabsList;
	POSITION m_SelectedTabPos;
	CTabCtrl m_Child;
	bool m_ToolTipsUpdated;
// Operations
public:
	void SetTooltipText(CString* spText, BOOL bActivate);
	void SetFirstControlFocus(CTabPage  *pActualTabPage);
	void InitToolTip();
	CToolTipCtrl m_ToolTip;
	void ShowTab(int nIndex);
	void HideTab(int nIndex);
	
	void ZOrderFrontAddTabControls(CDclFormObject *pDclObject);
	void ZOrderFrontAllTabs();
	void DestroyChildren();
	CDclFormObject * GetTabPane(int nIndex);
	CTabPage * GetActualTabPage(int nIndex);
	void SetPaneVisibility(int nCurrentSelectedTab, BOOL bShow, BOOL bFireEvent = FALSE);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VdclTab)
	public:
	virtual BOOL Create(CDclControlObject* pControl, CWnd* pParentWnd, UINT nID );
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetupTabs();
	virtual ~VdclTab();

	// Generated message map functions
protected:
	//{{AFX_MSG(VdclTab)
	afx_msg void OnSelchange(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchanging(NMHDR* pNMHDR, LRESULT* pResult);	
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	CDclControlObject *m_ArxControl;
};
