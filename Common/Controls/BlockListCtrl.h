// BlockListCtrl.h : header file
//

#pragma once

#include "ListViewCtrl.h"


/////////////////////////////////////////////////////////////////////////////
// CBlockListCtrl window

class CBlockListCtrl : public CListViewCtrl
{
	CImageList mBlockViewImageList;

// Construction
public:
	CBlockListCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CBlockListCtrl();

// DialogControl Interface
public:
	operator TDialogControlPtr () { return TDialogControlLockedPtr( this ); } //to ensure it doesn't get auto deleted
	bool Create( CWnd* pParentWnd, UINT nID ) override;
	DWORD GetWndStyle() const override;

// CBlockListCtrl Interface
protected:
	bool HasSubItemImages() const override { return false; }
};
