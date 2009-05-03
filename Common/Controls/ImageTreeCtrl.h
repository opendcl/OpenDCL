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
	virtual bool Create( CWnd* pParentWnd, UINT nID );
	virtual DWORD GetWndStyle() const;
	virtual bool OnApplyProperty( TPropertyPtr pProp );
	virtual CAcadColorService* GetColorService() { return &mColorService; }
	virtual DROPEFFECT OnBeginDrag( const CPoint& point, COleDataSource& SourceData );
	virtual DROPEFFECT OnDragOver( const CPoint& point, COleDataObject* pSourceData, DWORD dwKeyState );
	virtual bool OnDrop( const CPoint& point, COleDataObject* pSourceData, DROPEFFECT dropEffect );
	virtual void OnDragLeave();

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

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual afx_msg void PostNcDestroy();
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTvnDeleteitem(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnGetdispinfo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg HBRUSH CtlColor(CDC* /*pDC*/, UINT /*nCtlColor*/);
	afx_msg UINT OnGetDlgCode();
};
