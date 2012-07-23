// ListViewCtrl.h : header file
//

#pragma once

#include "DialogControl.h"
#include "AcadColorService.h"


/////////////////////////////////////////////////////////////////////////////
// CLVEdit 

class CLVEdit : public CEdit
{
public:
	int m_x;
	int m_y;
	CRect m_rcOldPos;
	CWnd *m_pParent;

public:
	CLVEdit() : m_x(0), m_y(0) {}
	virtual ~CLVEdit() {};

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnWindowPosChanging(WINDOWPOS FAR* lpwndpos);
	afx_msg void OnSetfocus();	
};


/////////////////////////////////////////////////////////////////////////////
// CListViewCtrl window

class CListViewCtrl : public CListCtrl, public CDialogControl
{
	CAcadColorService mColorService;
	int mnEditSubItem;
	int mnDragSource;
	CLVEdit mLVEdit;

// Construction
public:
	CListViewCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CListViewCtrl();

// DialogControl Interface
public:
	operator TDialogControlPtr () { return TDialogControlLockedPtr( this ); } //to ensure it doesn't get auto deleted
	virtual bool Create( CWnd* pParentWnd, UINT nID );
	virtual DWORD GetWndStyle() const;
	virtual bool ApplyProperty( TPropertyPtr pProp );
	virtual bool OnApplyCaption( TPropertyPtr pProp ) { return true; }
	virtual bool OnApplyForegroundColor( TPropertyPtr pProp );
	virtual bool OnApplyBackgroundColor( TPropertyPtr pProp );
	virtual CAcadColorService* GetColorService() { return &mColorService; }
	virtual DROPEFFECT OnBeginDrag( const CPoint& point, COleDataSource& SourceData ); //called to get drag data from this control
	virtual bool OnDrop( const CPoint& point, COleDataObject* pSourceData, DROPEFFECT dropEffect );

protected:
	int GetCurrentSubItem() const { return mnEditSubItem; }
	virtual bool HasSubItemImages() const { return true; }
	bool CellHitTest( const CPoint& point, int& nRow, int& nCol ) const;
	CRect GetCellRect( int nRow, int nCol, int area = LVIR_BOUNDS ) const;

// Implementation
public:
	CEdit* EditLabel( int nItem );
	bool SortTextItems( int nCol, bool bAscending );
	bool SortNumericItems( int nCol, bool bAscending );
	int GetItemImage( int nRow, int nCol );
	void SetItemImage( int nRow, int nCol, int nImage );
	void SetItemTextImage( int nRow, int nCol, LPCTSTR pszText, int nImage );

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void PostNcDestroy();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg HBRUSH CtlColor(CDC* /*pDC*/, UINT /*nCtlColor*/);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLvnInsertitem(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnDeleteitem(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnDeleteallitems(NMHDR *pNMHDR, LRESULT *pResult);
};
