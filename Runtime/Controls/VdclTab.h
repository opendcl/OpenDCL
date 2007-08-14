// VdclTab.h : header file
//

#pragma once

#include "PPToolTip.h"
#include "TabPage.h"
#include "ArxDialogControl.h"

class CDclControlObject;
class CDclFormObject;
class VdclTab;


/////////////////////////////////////////////////////////////////////////////
// VdclTab window

class VdclTab : public CTabCtrl, public CArxDialogControl
{
public:
	typedef std::vector< TTabPagePtr > TTabPages;

// Attributes
private:
	TTabPages mTabPages;
	CBrush mbrushBackground;

public:
	bool m_bInvokeWithSendString;
	int m_nCurrentSelectedTab;

// Construction
public:
	VdclTab( CControlPane& Pane, CDclControlObject* pTemplate, UINT nID );
	virtual ~VdclTab();

// DialogControl Interface
public:
	operator TDialogControlPtr () { return TDialogControlLockedPtr( *this ); } //to ensure it doesn't get auto deleted
	virtual bool Create( CWnd* pParentWnd, UINT nID );
	virtual DWORD GetWndStyle() const;
	virtual bool OnApplyProperty( RefCountedPtr< CPropertyObject > pProp );
	virtual bool OnApplyCaption( RefCountedPtr< CPropertyObject > pProp ) { return true; }
	virtual bool OnApplyToolTip( RefCountedPtr< CPropertyObject > pProp );
	virtual bool GetChildPanes( std::list< const CControlPane* >& listChildren ) const;

// Interface
public:
	CTabCtrl& GetTabCtrl() { return *this; }
	const CTabCtrl& GetTabCtrl() const { return *this; }
	CRect GetUsedArea() const;

// Operations
public:
	virtual void ShowTab(int nPageIndex);
	virtual void HideTab(int nPageIndex);
	virtual bool CreateTabPages( UINT& nId );
	size_t GetCurTabPage() const { return GetTabPageIndex( GetCurSel() ); }
	size_t GetTabPageCount() const { return mTabPages.size(); }
	int GetTabItemIndex( size_t nPageIndex ) const;
	size_t GetTabPageIndex( int nItemIndex ) const;
	TTabPagePtr GetTabPageAt( size_t nPageIndex ) const;
	const CDclFormObject* GetTabSourceFormAt( size_t nPageIndex ) const;
	const CControlPane* GetTabControlPaneAt( size_t nPageIndex ) const;

public:
	void ActivateTabPage( int nTabPageToActivate, bool bShow, bool bFireEvent = false );
	void SetFirstControlFocus(CTabPage  *pActualTabPage);

// Implementation
protected:
	void ZOrderFrontAllTabs();
	void DestroyTabPages();
	void ResetTooltips();
	void SetupTabs();

protected:
	DECLARE_MESSAGE_MAP()

// Overrides
public:
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
	afx_msg HBRUSH CtlColor(CDC* /*pDC*/, UINT /*nCtlColor*/);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};
