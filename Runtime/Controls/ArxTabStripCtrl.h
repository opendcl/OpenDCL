// ArxTabStripCtrl.h : header file
//

#pragma once

#include "TabStripCtrl.h"
#include "PPToolTip.h"
#include "TabPage.h"
#include "ArxControlServices.h"

class CDclControlObject;
class CDclFormObject;
class VdclTab;


/////////////////////////////////////////////////////////////////////////////
// CArxTabStripCtrl window

class CArxTabStripCtrl : public CTabStripCtrl
{
public:
	typedef std::vector< TTabPagePtr > TTabPages;

// Attributes
private:
	CArxControlServices	mArxServices;
	TTabPages mTabPages;
	TTabPagePtr mpActiveTabPage;
	bool mbInvokeWithSendString;

// Construction
public:
	CArxTabStripCtrl( CDclControlObject* pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CArxTabStripCtrl();

// DialogControl Interface
public:
	operator TDialogControlPtr () { return TDialogControlLockedPtr( *this ); } //to ensure it doesn't get auto deleted
	virtual bool Create( CWnd* pParentWnd, UINT nID );
	virtual bool GetChildPanes( std::list< const CControlPane* >& listChildren ) const;

// Operations
public:
	virtual void ShowTab( size_t nPageIndex );
	virtual void HideTab( size_t nPageIndex );
	virtual bool SetCurrentTab( size_t nPageIndex );
	int GetTabItemIndex( size_t nPageIndex ) const;
	size_t GetCurTabPage() const { return GetTabPageIndex( GetCurSel() ); }
	size_t GetTabPageCount() const { return mTabPages.size(); }
	size_t GetTabPageIndex( int nItemIndex ) const;
	TTabPagePtr GetTabPageAt( size_t nPageIndex ) const;
	const CDclFormObject* GetTabSourceFormAt( size_t nPageIndex ) const;
	const CControlPane* GetTabControlPaneAt( size_t nPageIndex ) const;

public:

// Implementation
protected:
	bool CreateTabPages( UINT& nId );
	void DestroyTabPages();
	void ActivateTabPage( TTabPagePtr pTabPage, bool bFireEvent = false );
	void SetFirstControlFocus( CTabPage* pActualTabPage );

protected:
	DECLARE_MESSAGE_MAP()

protected:
	afx_msg void OnSelchange( NMHDR* pNMHDR, LRESULT* pResult );
	afx_msg void OnSelchanging( NMHDR* pNMHDR, LRESULT* pResult );	
	afx_msg void OnSize( UINT nType, int cx, int cy );
	afx_msg void OnKillFocus( CWnd* pNewWnd );
	afx_msg void OnSetFocus( CWnd* pOldWnd );
};
