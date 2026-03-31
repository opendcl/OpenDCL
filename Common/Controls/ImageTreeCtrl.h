// ImageTreeCtrl.h : header file
//

#pragma once

#include "DialogControl.h"
#include "AcadColorService.h"
#include <list>


class CTreeItem
{
public:
	//CTreeItem() : mhItem( NULL ), mnImage( -1 ), mnSelImage( -1 ), mnExpImage( -1 ) {}
	CTreeItem( HTREEITEM hItem, LPCTSTR pszKey = NULL, int nImage = -1, int nSelImage = -1, int nExpImage = -1 )
		: mhItem( hItem )
		, msKey( pszKey )
		, mnImage( nImage )
		, mnSelImage( nSelImage )
		, mnExpImage( nExpImage )
		{
			assert( hItem != NULL );
		}
	virtual ~CTreeItem() {}

public:
	HTREEITEM mhItem;
	CString msKey;
	int mnImage;
	int mnSelImage;
	int mnExpImage;
};


/////////////////////////////////////////////////////////////////////////////
// CImageTreeCtrl window

class CImageTreeCtrl : public CTreeCtrl, public CDialogControl
{
	CAcadColorService mColorService;
	CImageList mImageList;
	std::list< CTreeItem > mTreeItems;
	HTREEITEM mhtiDragSource;
	bool mbDeleting;

	// Construction
public:
	CImageTreeCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CImageTreeCtrl();

// DialogControl Interface
public:
	operator TDialogControlPtr () { return TDialogControlLockedPtr( this ); } //to ensure it doesn't get auto deleted
	bool Create( CWnd* pParentWnd, UINT nID ) override;
	DWORD GetWndStyle() const override;
	bool ApplyProperty( TPropertyPtr pProp ) override;
	CAcadColorService* GetColorService() override { return &mColorService; }
	DROPEFFECT OnBeginDrag( const CPoint& point, COleDataSource& SourceData ) override;
	DROPEFFECT OnDragOver( const CPoint& point, COleDataObject* pSourceData, DWORD dwKeyState ) override;
	bool OnDrop( const CPoint& point, COleDataObject* pSourceData, DROPEFFECT& dropEffect ) override;
	void OnDragLeave() override;

// Image Tree interface
public:
	CString GetItemKey( HTREEITEM hItem ) const;
	HTREEITEM FindItem( LPCTSTR pszKey ) const;
	HTREEITEM AddParent( LPCTSTR pszCaption, LPCTSTR pszKey = NULL, int nImage = -1,
											 int nSelImage = -1, int nExpImage = -1 );
	HTREEITEM AddChild( HTREEITEM hParent, LPCTSTR pszCaption, LPCTSTR pszKey = NULL, int nImage = -1,
											int nSelImage = -1, int nExpImage = -1, HTREEITEM hAddAfter = TVI_LAST );
	BOOL GetItemImage( HTREEITEM hItem, int& nImage, int& nSelImage ) const;
	BOOL SetItemImage( HTREEITEM hItem, int nImage, int nSelImage );
	void Clear();
	int GetExpandedImage( HTREEITEM hItem ) const;
	bool SetExpandedImage( HTREEITEM hItem, int nExpImage );

protected:
	CTreeItem* GetTreeItem( HTREEITEM hItem );
	const CTreeItem* GetTreeItem( HTREEITEM hItem ) const;
	HTREEITEM CopyTreeItem( HTREEITEM hSource, HTREEITEM hNewParent, HTREEITEM hInsertAfter, bool bCopyKey = false );

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

	BOOL PreTranslateMessage(MSG* pMsg) override;
	void PostNcDestroy() override;
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTvnDeleteitem(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnGetdispinfo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg HBRUSH CtlColor(CDC* /*pDC*/, UINT /*nCtlColor*/);
	afx_msg UINT OnGetDlgCode();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnDpiChanged(WPARAM wParam, LPARAM lParam);
};
