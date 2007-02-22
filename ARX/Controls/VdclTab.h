// VdclTab.h : header file
//

#pragma once

#include "PPToolTip.h"
#include "TabPage.h"
#include "ArxDialogControl.h"

class CDclControlObject;
class CDclFormObject;
class VdclTab;


class CTabControlX : public CArxDialogControl
{
public:
	CTabControlX( CDclControlObject* pTemplate, CControlPane* pPane, CWnd* pWnd )
		: CArxDialogControl( pTemplate, pPane, pWnd ) {}
	virtual ~CTabControlX() {}

	const VdclTab* GetControl() const { return (VdclTab*)mpControl; }
	VdclTab* GetControl() { return (VdclTab*)mpControl; }

	// services
	virtual TDialogControlPtr FindControl( HWND hwndControl ) const; //find nested control
	virtual TDialogControlPtr FindControl( LPCTSTR pszControlName, ControlType type = CtlInvalid ) const; //find nested control

	// attributes
	virtual DWORD GetWndStyle() const; //get window style from properties

	// operations
	virtual bool OnApplyProperty( RefCountedPtr< CPropertyObject > pProp );
	virtual bool OnApplyEnabled( RefCountedPtr< CPropertyObject > pProp );
	virtual bool OnApplyCaption( RefCountedPtr< CPropertyObject > pProp ) { return true; }
	virtual bool OnApplyCaptionFont( RefCountedPtr< CPropertyObject > pProp );
};


/////////////////////////////////////////////////////////////////////////////
// VdclTab window

class VdclTab : public CTabCtrl
{
public:
	typedef std::vector< TTabPagePtr > TTabPages;

// Attributes
private:
	CTabControlX mControlX;
	CDclControlObject* mpSourceControl;	
	CControlPane* mpControlPane;
	//CTabCtrl mTabCtrl;
	TTabPages mTabPages;
	//CRect mrectTabCtrl;

public:
	CToolTipCtrl m_ToolTip;
	bool m_bInvokeWithSendString;
	int m_nCurrentSelectedTab;
	bool m_ToolTipsUpdated;

// Construction
public:
	VdclTab( CControlPane& Pane, CDclControlObject* pTemplate, UINT nID );
	virtual ~VdclTab();

// Interface
public:
	CArxDialogControl& GetDialogControl() { return mControlX; }
	const CArxDialogControl& GetDialogControl() const { return mControlX; }
	CTabCtrl& GetTabCtrl() { return *this; }
	const CTabCtrl& GetTabCtrl() const { return *this; }
	CRect GetUsedArea() const;
	TDialogControlPtr FindControl( HWND hwndControl ) const;
	TDialogControlPtr FindControl( LPCTSTR pszControlName, ControlType type = CtlInvalid ) const;

// Operations
public:
	virtual void ShowTab(int nIndex);
	virtual void HideTab(int nIndex);
	virtual bool Create( CWnd* pParentWnd, UINT nID );
	virtual bool CreateTabPages( UINT& nId );
	TTabPagePtr GetTabPageAt( size_t nIndex ) const;
	const CDclFormObject* GetTabSourceFormAt( size_t nIndex ) const;
	const CControlPane* GetTabControlPaneAt( size_t nIndex ) const;

public:
	void ActivateTabPage( int nTabPageToActivate, bool bShow, bool bFireEvent = false );
	void SetTooltipText(CString* spText, BOOL bActivate);
	void SetFirstControlFocus(CTabPage  *pActualTabPage);
	void InitToolTip();

// Implementation
protected:
	void ZOrderFrontAllTabs();
	void DestroyTabPages();
	void SetupTabs();

// Overrides
public:
	//virtual BOOL Create(CDclControlObject* pControl, CWnd* pParentWnd, UINT nID );
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	// Generated message map functions
protected:
	afx_msg void OnSelchange(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchanging(NMHDR* pNMHDR, LRESULT* pResult);	
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void PostNcDestroy();
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);

protected:
	DECLARE_MESSAGE_MAP()
};
