// GridCtrl.h : header file
//

#pragma once

#include "DialogControl.h"
#include "GridCtrlHdr.h"
#include "AcadColorService.h"
#include <string>
#include <locale>

class CGridCellEditCtrl;

#if defined(_UNICODE)
typedef std::wstring tstring;
#else
typedef std::string tstring;
#endif //_UNICODE

#define ID_CELLBUTTON 191

enum CellStyle
{
	Grid_Undefined = -1,
	Grid_Runtime = 0,
	Grid_CheckBoxes		= 1,
	Grid_OptionButtons	= 2,
	Grid_SwitchableIcons	= 3,
	Grid_EllipsesButtons	= 4,
	Grid_PickButtons		= 5,
	Grid_Strings			= 6,
	Grid_AngleUnits		= 7,
	Grid_Integers			= 8,
	Grid_Units			= 9,
	Grid_UpperCase		= 10,
	Grid_LowerCase		= 11,
	Grid_Password			= 12,
	Grid_MultiLine		= 13,
	Grid_Currency			= 14,
	Grid_Date				= 15,
	Grid_Time				= 16,
	Grid_Percentage		= 17,
	Grid_DropDown			= 18,
	Grid_ArrowHead		= 19,
	Grid_AcadColors		= 20,
	Grid_TextStyleList	= 21,
	Grid_PlotStyleNames	= 22,
	Grid_PlotStyleTables	= 23,
	Grid_PlotterList		= 24,
	Grid_Fonts			= 25,
	Grid_DriveList		= 26,
	Grid_LayerList		= 27,
	Grid_DimStyleList		= 28,
	Grid_ImageDropList	= 29,
	Grid_AcadColorCell	= 30,
	Grid_TrueColorCell	= 31,
	Grid_LineWeightCell	= 32,
	Grid_LinetypeCell		= 33,
	Grid_DirectoryCell	= 34,
	Grid_DwgFilesCell		= 35,
	Grid_Strings_Combo	= 36,
	Grid_AngleUnits_Combo	= 37,
	Grid_Integers_Combo	= 38,
	Grid_Units_Combo		= 39,
	Grid_UpperCase_Combo	= 40,
	Grid_LowerCase_Combo	= 41,
};

struct _CellData
{
	CellStyle mType;
	int midxImage;
	int midxAltImage;
	int mnDateTimeStyle;
	std::vector< tstring > mrsComboList;
	std::vector< int > mrnComboImage;
	_CellData()
		: mType( Grid_Undefined )
		, midxImage( -1 )
		, midxAltImage( -1 )
		, mnDateTimeStyle( -1 )
	{
	} 
	_CellData( CellStyle type, int image = -1, int altImage = -1, int dateTimeStyle = -1 )
		: mType( type )
		, midxImage( image )
		, midxAltImage( altImage )
		, mnDateTimeStyle( dateTimeStyle )
	{
	} 
	virtual ~_CellData() {}
};

typedef std::vector< _CellData > _ColumnData;
struct _RowData
{
	DWORD_PTR mnItemData; // holds the item data
	_ColumnData mCellData;

	_RowData( DWORD_PTR nItemData = 0 ) : mnItemData( nItemData ) {}
	virtual ~_RowData() {}
};


/////////////////////////////////////////////////////////////////////////////
// CGridCtrl window

class CGridCtrl : public CListCtrl, public CDialogControl
{
	CAcadColorService mColorService;
	CGridCtrlHdr mHeaderCtrl;
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

// Construction
public:
	CGridCtrl( CDclControlObject* pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CGridCtrl();

// DialogControl Interface
public:
	operator TDialogControlPtr () { return TDialogControlLockedPtr( *this ); } //to ensure it doesn't get auto deleted
	virtual bool Create( CWnd* pParentWnd, UINT nID );
	virtual CAcadColorService* GetColorService() { return &mColorService; }
	virtual DWORD GetWndStyle() const;
	virtual bool OnApplyProperty( TPropertyPtr pProp );

public:
	void SetCurCell( int nRow, int nCol );
	int GetCurRow() const { return mCurrentCell.row(); }
	int GetCurColumn() const { return mCurrentCell.col(); }
	CellStyle GetCellStyle( int nRow, int nCol );
	CellStyle GetCurCellStyle();
	void SetCellStyle( int nRow, int nCol, CellStyle nStyle, int image = -1, int altImage = -1,
										 int dateTimeStyle = -1, LPCTSTR pszListText = NULL );
	CString GetCellText( int nRow, int nCol ) const;
	CString GetCurCellText() const;
	void SetCurCellText( LPCTSTR pszText );
	bool SetCellText( int nRow, int nCol, LPCTSTR pszText );
	CRect GetCellRect( int nRow, int nCol, int area = LVIR_BOUNDS );
	void SetCellImage( int nRow, int nCol, int nImage );
	void SetCellTextImage( int nRow, int nCol, LPCTSTR pszText, int nImage );
	int GetCellImage( int nRow, int nCol );
	int GetCellUncheckedImage( int nRow, int nCol );
	int GetCellCheckedImage( int nRow, int nCol );
	bool IsCellChecked( int nRow, int nCol );
	bool SetCellListData( int nRow, int nCol, const CArray<int, int>& rnImage, const CStringArray& rsList );
	bool GetCellComboListItems( int nRow, int nCol, std::vector< tstring >& rsList, std::vector< int >& ridxImage );
	UINT GetCellState( int nRow, int nCol );
	bool ToggleCellState( int nRow, int nCol );
	bool CellHitTest( const CPoint& point, int& nRow, int& nCol );
	void HideEditControls();
	bool EditCurCell() { OnEditCurCell(); return true; }
	bool CancelEditing() { HideEditControls(); return true; }

protected:
	const _RowData* GetRowData( size_t nRow ) const;
	const _CellData* GetCellData( size_t nRow, size_t nCol ) const;
	_RowData& GetRowDataRef( size_t nRow );
	_CellData& GetCellDataRef( size_t nRow, size_t nCol );
	int GetCellListImage( int nRow, int nCol, LPCTSTR pszListText );
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
	int InsertColumn( int nCol, LPCTSTR lpszColumnHeading, int nFormat = LVCFMT_LEFT, int nWidth = -1,
										int nImageIndex = -1 );
	CString GetItemText( int nRow, int nCol ) { return GetCellText( nRow, nCol ); }
	BOOL SetItemText( int nRow, int nCol, LPCTSTR pszText ) { return SetCellText( nRow, nCol, pszText ); }
	BOOL DeleteItem( int nRow );
	BOOL DeleteColumn( int nCol );


// Overridables
protected:
	virtual void OnSelectionChanged() {}
	virtual void OnEditCurCell();
	virtual CGridCellEditCtrl* CreateEditControl( int nRow, int nCol );
	virtual void OnEndEditCurCell();
	virtual void DrawCell( int nRow, int nCol, const CRect& rectCell, CDC& cdc );

// Operations
protected:
	void DrawOptionButton( CDC& cdc, const CRect& rcIcon, bool bPressed, bool bHighlight );
	void DrawCheckBox( CDC& cdc, const CRect& rcIcon, bool bPressed, bool bHighlight );
	void DrawTrueColor( CDC& cdc, const CRect& rcIcon, int nColor, const CString& sText );
	void DrawColor( CDC& cdc, const CRect& rcIcon, int nColor, const CString& sText );
	void DrawFontIcons( CDC& cdc, const CRect& rcIcon, int nImage, const CString& sText );

public:
	BOOL SortTextItems( int nCol, BOOL bAscending, int low = 0, int high = -1 );
	bool SortNumericItems( int nCol, BOOL bAscending, int low = 0, int high = -1 );

protected:
	DECLARE_MESSAGE_MAP()

// Generated message map functions
protected:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void PostNcDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};
