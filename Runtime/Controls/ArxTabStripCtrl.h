// ArxTabStripCtrl.h : header file
//

#pragma once

#include "TabStripCtrl.h"
#include "PPToolTip.h"
#include "TabPageDlg.h"
#include "ArxControlServices.h"

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
	CArxTabStripCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CArxTabStripCtrl();

// DialogControl Interface
public:
	const CArxControlServices* GetArxServices() const override { return &mArxServices; }
	bool Create( CWnd* pParentWnd, UINT nID ) override;
	bool GetChildPanes( std::list< const CControlPane* >& listChildren ) const override;

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
	const TDclFormPtr GetTabSourceFormAt( size_t nPageIndex ) const;

public:

// Implementation
protected:
	bool CreateTabPages( UINT& nId );
	void DestroyTabPages();
	void ActivateTabPage( TTabPagePtr pTabPage, bool bFireEvent = false );
	void SetFirstControlFocus( CTabPageDlg* pActualTabPage );

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnCbnSelchange( NMHDR* pNMHDR, LRESULT* pResult );
	afx_msg void OnSelchanging( NMHDR* pNMHDR, LRESULT* pResult );	
	afx_msg void OnKillFocus( CWnd* pNewWnd );
	afx_msg void OnSetFocus( CWnd* pOldWnd );
	LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) override;
};
