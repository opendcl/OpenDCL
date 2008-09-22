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
	virtual bool Create( CWnd* pParentWnd, UINT nID );
	virtual DWORD GetWndStyle() const;

// CBlockListCtrl Interface
protected:
	virtual bool HasSubItemImages() const { return false; }
};
