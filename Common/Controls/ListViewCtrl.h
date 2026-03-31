// ListViewCtrl.h : header file
//

#pragma once

#include "DialogControl.h"
#include "AcadColorService.h"
#include "ListCtrlHdr.h"


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

class ListviewDropSource : public COleDropSource
{
public:
	ListviewDropSource()
		: COleDropSource()
		{}
	BOOL OnBeginDrag(CWnd* pWnd) override
	{
		ASSERT_VALID(this);

		m_bDragStarted = TRUE; //listview initiates dragging internally, so just carry on

		// opposite button cancels drag operation
		m_dwButtonCancel = 0;
		m_dwButtonDrop = 0;
		if (GetKeyState(VK_LBUTTON) < 0)
		{
			m_dwButtonDrop |= MK_LBUTTON;
			m_dwButtonCancel |= MK_RBUTTON;
		}
		else if (GetKeyState(VK_RBUTTON) < 0)
		{
			m_dwButtonDrop |= MK_RBUTTON;
			m_dwButtonCancel |= MK_LBUTTON;
		}

		return TRUE;
	}
};


/////////////////////////////////////////////////////////////////////////////
// CListViewCtrl window

class CListViewCtrl : public CListCtrl, public CDialogControl
{
	CListCtrlHdr mHeaderWnd;
	CAcadColorService mColorService;
	int mnEditSubItem;
	int mnDragSource;
	CLVEdit mLVEdit;
	ListviewDropSource mDropSource;

// Construction
public:
	CListViewCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CListViewCtrl();

// DialogControl Interface
public:
	operator TDialogControlPtr () { return TDialogControlLockedPtr( this ); } //to ensure it doesn't get auto deleted
	bool Create( CWnd* pParentWnd, UINT nID ) override;
	DWORD GetWndStyle() const override;
	void HandleDpiChanged() override; //handle relayed WM_DPICHANGED_AFTERPARENT message
	bool ApplyProperty( TPropertyPtr pProp ) override;
	bool OnApplyCaption( TPropertyPtr pProp ) override { return true; }
	bool OnApplyForegroundColor( TPropertyPtr pProp ) override;
	bool OnApplyBackgroundColor( TPropertyPtr pProp ) override;
	CAcadColorService* GetColorService() override { return &mColorService; }
	COleDropSource* GetDropSource() override { return &mDropSource; }
	DROPEFFECT OnBeginDrag( const CPoint& point, COleDataSource& SourceData ) override; //called to get drag data from this control
	bool OnDrop( const CPoint& point, COleDataObject* pSourceData, DROPEFFECT& dropEffect ) override;

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
	afx_msg void OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	void PostNcDestroy() override;
	BOOL PreTranslateMessage(MSG* pMsg) override;
	afx_msg HBRUSH CtlColor(CDC* /*pDC*/, UINT /*nCtlColor*/);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLvnInsertitem(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnDeleteitem(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnDeleteallitems(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg LRESULT OnDpiChanged(WPARAM wParam, LPARAM lParam);
};
