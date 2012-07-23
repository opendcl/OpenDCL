// GridCtrl.h : header file
//

#pragma once

#include "DialogControl.h"
#include "AcadColorService.h"
#include "TipWnd.h"
#include <string>
#include <locale>

class CGridCellEditCtrl;

#if defined(_UNICODE)
typedef std::wstring tstring;
#else
typedef std::string tstring;
#endif //_UNICODE

#define ID_CELLBUTTON 191

namespace Grid
{
	enum CellStyle
	{
		Undefined       = -1,
		Runtime         = 0,
		CheckBoxes      = 1,
		OptionButtons   = 2,
		SwitchableIcons = 3,
		EllipsesButtons = 4,
		PickButtons     = 5,
		Strings         = 6,
		AngleUnits      = 7,
		Integers        = 8,
		Units           = 9,
		UpperCase       = 10,
		LowerCase       = 11,
		Password        = 12,
		MultiLine       = 13,
		Currency        = 14,
		Date            = 15,
		Time            = 16,
		Percentage      = 17,
		DropDown        = 18,
		ArrowHead       = 19,
		AcadColors      = 20,
		TextStyleList   = 21,
		PlotStyleNames  = 22,
		PlotStyleTables = 23,
		PlotterList     = 24,
		Fonts           = 25,
		DriveList       = 26,
		LayerList       = 27,
		DimStyleList    = 28,
		ImageDropList   = 29,
		AcadColorCell   = 30,
		TrueColorCell   = 31,
		LineWeightCell  = 32,
		LinetypeCell    = 33,
		DirectoryCell   = 34,
		DwgFilesCell    = 35,
		Strings_Combo   = 36,
		AngleUnits_Combo = 37,
		Integers_Combo  = 38,
		Units_Combo     = 39,
		UpperCase_Combo = 40,
		LowerCase_Combo = 41,
		Symbols         = 42,
		Symbols_Combo   = 43,
	};
};

struct _CellData
{
	Grid::CellStyle mType;
	int midxImage;
	int midxAltImage;
	std::vector< tstring > mrsComboList;
	std::vector< int > mrnComboImage;
	_CellData()
		: mType( Grid::Undefined )
		, midxImage( -1 )
		, midxAltImage( -1 )
	{
	} 
	_CellData( const _CellData& _Right )
		: mType( _Right.mType )
		, midxImage( _Right.midxImage )
		, midxAltImage( _Right.midxAltImage )
		, mrsComboList( _Right.mrsComboList )
		, mrnComboImage( _Right.mrnComboImage )
	{
	} 
	_CellData( Grid::CellStyle type, int image = -1, int altImage = -1 )
		: mType( type )
		, midxImage( image )
		, midxAltImage( altImage )
	{
	} 
	virtual ~_CellData() {}
	_CellData& operator =( const _CellData& _Right )
	{
		mType = _Right.mType;
		midxImage = _Right.midxImage;
		midxAltImage = _Right.midxAltImage;
		mrsComboList = _Right.mrsComboList;
		mrnComboImage = _Right.mrnComboImage;
		return *this;
	} 
};

typedef std::vector< _CellData > _ColumnData;
struct _RowData
{
	DWORD_PTR mnItemData; // holds the item data
	_ColumnData mCellData;

	_RowData( DWORD_PTR nItemData = 0 ) : mnItemData( nItemData ) {}
	_RowData( const _RowData& _Right ) : mnItemData( _Right.mnItemData ), mCellData( _Right.mCellData ) {}
	virtual ~_RowData() {}
	_RowData& operator =( const _RowData& _Right )
	{
		mnItemData = _Right.mnItemData;
		mCellData = _Right.mCellData;
		return *this;
	} 
};


/////////////////////////////////////////////////////////////////////////////
// CGridCtrl window

class CGridCtrl : public CListCtrl, public CDialogControl
{
	CAcadColorService mColorService;
	class CGridTipWnd : public CTipWnd
	{
		CGridCtrl* mpGridCtl;
		int mnRow;
		int mnCol;
	public:
		CGridTipWnd( CGridCtrl* pGridCtl ) : mpGridCtl( pGridCtl ), mnRow( -1 ), mnCol( -1 ) {}
		void Track(const CPoint& point)
		{
			if( IsWindowVisible() )
			{
				int nRow = -1;
				int nCol = -1;
				if( !mpGridCtl->CellHitTest( point, nRow, nCol ) )
					Hide();
				else if( nRow != mnRow || nCol != mnCol )
					Hide();
			}
		}
		void Show(int nRow, int nCol)
		{
			if( IsWindowVisible() && mnRow == nRow && mnCol == nCol )
				return;
			mnRow = nRow;
			mnCol = nCol;
			CRect rcCell = mpGridCtl->GetCellRect( nRow, nCol, LVIR_BOUNDS );
			CDC* pDC = mpGridCtl->GetDC();
			CFont* pFont = mpGridCtl->GetFont();
			CFont* pOldFont = pDC->SelectObject( pFont );
			CSize sizCell = rcCell.Size();
			mpGridCtl->DrawCell( mnRow, mnCol, *pDC, sizCell, true );
			pDC->SelectObject( pOldFont );
			mpGridCtl->ReleaseDC( pDC );
			if( rcCell.Width() < sizCell.cx || rcCell.Height() < sizCell.cy )
			{
				if( !m_hWnd )
					Create( NULL );
				SetFont( pFont );
				CString sLabel = mpGridCtl->GetCellText( nRow, nCol );
				SetWindowText( sLabel );
				mpGridCtl->ClientToScreen( &rcCell );
				if( rcCell.Width() < sizCell.cx )
					sizCell.cx += 4;
				else
					sizCell.cy += 4;
				CTipWnd::Show( rcCell.TopLeft(), sizCell );
			}
		}
	protected:
		void Paint(CDC* pDC)
		{
			CRect rcTip;
			GetClientRect( &rcTip );
			CRect rcCell = mpGridCtl->GetCellRect( mnRow, mnCol );
			CPoint ptOldOrg = pDC->GetWindowOrg();
			pDC->SetWindowOrg( rcCell.TopLeft() );
			CFont* pOldFont = pDC->SelectObject( GetFont() );
			mpGridCtl->DrawCell( mnRow, mnCol, *pDC, rcTip.Size() );
			pDC->SelectObject( pOldFont );
			pDC->SetWindowOrg( ptOldOrg );
		}
	} mTipWnd;
	bool mbTrackingMouse;
	ULONG mcColumns;
	bool mbHasRowHeader;
	bool mbHasGridLines;
	bool mbAlternateColumnColors;
	CGridCellEditCtrl* mpCellEditCtrl;
	CImageList mDefaultImageList;
	CImageList mOptionButtonImageList;
	CImageList mArrowImageList;
	CImageList mFontImageList;
	COLORREF mclrAlternate;
	int mnRowHeight;
	bool mbIgnoreChange;

protected:
	class _Cell
	{
		int mRow;
		int mCol;
	public:
		_Cell( int row = -1, int col = -1 ) : mRow( row ), mCol( col ) {}
		operator bool() const { return (mRow >= 0); }
		int row() const { return mRow; }
		int col() const { return mCol; }
		void set( int row, int col )
			{ mRow = row; if( row < 0 ) mCol = -1; else mCol = col; }
	} mCurrentCell;
	std::vector< _RowData > mRowData;

public:
	static const UINT& refWM_CHECKFOCUS();

// Construction
public:
	CGridCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CGridCtrl();

// DialogControl Interface
public:
	operator TDialogControlPtr () { return TDialogControlLockedPtr( this ); } //to ensure it doesn't get auto deleted
	virtual bool Create( CWnd* pParentWnd, UINT nID );
	virtual CAcadColorService* GetColorService() { return &mColorService; }
	virtual DWORD GetWndStyle() const;
	virtual bool ApplyProperty( TPropertyPtr pProp );

public:
	void SetCurCell( int nRow, int nCol );
	int GetCurRow() const { return mCurrentCell.row(); }
	int GetCurColumn() const { return mCurrentCell.col(); }
	ULONG GetColumnCount() const { return mcColumns; }
	Grid::CellStyle GetCellStyle( int nRow, int nCol );
	Grid::CellStyle GetEffectiveCellStyle( int nRow, int nCol );
	Grid::CellStyle GetCurCellStyle();
	void SetCellStyle( int nRow, int nCol, Grid::CellStyle nStyle, int image = -1, int altImage = -1,
										 LPCTSTR pszListText = NULL );
	CString GetCellText( int nRow, int nCol ) const;
	CString GetCurCellText() const;
	void SetCurCellText( LPCTSTR pszText );
	bool SetCellText( int nRow, int nCol, LPCTSTR pszText );
	CRect GetCellRect( int nRow, int nCol, int area = LVIR_BOUNDS ) const;
	void SetCellImages( int nRow, int nCol, int nImage, int nAltImage = -2 );
	void SetCellTextImage( int nRow, int nCol, LPCTSTR pszText, int nImage );
	int GetCellImage( int nRow, int nCol );
	int GetCellAltImage( int nRow, int nCol );
	int GetCellUncheckedImage( int nRow, int nCol );
	int GetCellCheckedImage( int nRow, int nCol );
	bool IsCellChecked( int nRow, int nCol );
	bool SetCellChecked( int nRow, int nCol, bool bChecked );
	bool SetCellListData( int nRow, int nCol, const CArray<int, int>& rnImage, const CStringArray& rsList );
	bool GetCellListData( int nRow, int nCol, CArray<int, int>& rnImage, CStringArray& rsList );
	bool GetCellComboListItems( int nRow, int nCol, std::vector< tstring >& rsList, std::vector< int >& ridxImage );
	UINT GetCellState( int nRow, int nCol );
	bool ToggleCellState( int nRow, int nCol );
	bool CellHitTest( const CPoint& point, int& nRow, int& nCol ) const;
	void HideEditControls();
	bool EditCurCell() { OnEditCurCell(); return true; }
	bool CancelEditing() { HideEditControls(); return true; }

protected:
	const _RowData* GetRowData( size_t nRow ) const;
	const _CellData* GetCellData( size_t nRow, size_t nCol ) const;
	_RowData& GetRowDataRef( size_t nRow );
	_CellData& GetCellDataRef( size_t nRow, size_t nCol );
	int GetCellListImage( int nRow, int nCol, LPCTSTR pszListText );
	bool SetCellState( int nRow, int nCol, UINT nState );
	CRect GetCurSelRect();
	CRect GetCurCellRect( int area = LVIR_BOUNDS );
	void InvalidateCell( int nRow, int nCol );
	void SetupColumns();
	void MoveUp();
	void MoveDown();
	void MoveLeft();
	void MoveRight();
	bool IsEditing() const { return (mpCellEditCtrl != NULL); }

	// CListCtrl overrides to ensure that base class functions don't get called directly
public:
	BOOL SetItemData( int nRow, DWORD_PTR dwData );
	DWORD_PTR GetItemData( int nRow ) const;
	int InsertItem( int nRow, LPCTSTR lpszText, int nImageIndex = -1 );
	int InsertColumn( int nCol, LPCTSTR lpszColumnHeading, int nFormat = LVCFMT_LEFT, int nWidth = -1,
										int nImageIndex = -1 );
	CString GetItemText( int nRow, int nCol ) { return GetCellText( nRow, nCol ); }
	BOOL SetItemText( int nRow, int nCol, LPCTSTR pszText ) { return SetCellText( nRow, nCol, pszText ); }
	BOOL DeleteItem( int nRow );
	BOOL DeleteAllItems();
	BOOL DeleteColumn( int nCol );


// Overridables
protected:
	virtual void OnSelectionChanged() {}
	virtual void OnEditCurCell();
	virtual CGridCellEditCtrl* CreateEditControl( int nRow, int nCol );
	virtual void OnEndEditCurCell();
	virtual void DrawCell( int nRow, int nCol, CDC& cdc, CSize& sizCell = CSize(0, 0), bool bCalcOnly = false );

// Operations
protected:
	virtual void DrawOptionButton( CDC& cdc, const CRect& rcIcon, bool bPressed, bool bHighlight );
	virtual void DrawCheckBox( CDC& cdc, const CRect& rcIcon, bool bPressed, bool bHighlight );
	virtual void DrawArrow( CDC& cdc, const CRect& rcIcon, int nArrow, const CString& sText );
	virtual void DrawColor( CDC& cdc, const CRect& rcIcon, int nColor, const CString& sText );
	virtual void DrawFontIcons( CDC& cdc, const CRect& rcIcon, int nImage, const CString& sText );
	virtual void DrawLineWeight( CDC& cdc, const CRect& rcIcon, int LW, const CString& sText );

public:
	bool SortTextItems( int nCol, bool bAscending );
	bool SortNumericItems( int nCol, bool bAscending );

protected:
	DECLARE_MESSAGE_MAP()


// Generated message map functions
protected:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual afx_msg void PostNcDestroy();
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void HScroll(UINT nSBCode, UINT nPos);
	afx_msg void VScroll(UINT nSBCode, UINT nPos);
	afx_msg void OnLvnBeginScroll(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg LRESULT OnCheckFocus( WPARAM wParam, LPARAM lParam );
	afx_msg UINT OnGetDlgCode();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnDestroy();
	afx_msg void OnCancelMode();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);	
	afx_msg LRESULT OnMouseHover(WPARAM wParam, LPARAM lParam);	
};
