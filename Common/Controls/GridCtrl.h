// GridCtrl.h : header file
//

#pragma once

#include "DialogControl.h"
#include "GridCtrlHdr.h"
#include "ThemeHelperST.h"
#include "PPToolTip.h"

class CThemeHelperST;
class CPropertyObject;
class CDclControlObject;
class CDynamicButtonCtrl;

const int Grid_CheckBoxes		= 1;
const int Grid_OptionButtons	= 2;
const int Grid_SwitchableIcons	= 3;
const int Grid_EllipsesButtons	= 4;
const int Grid_PickButtons		= 5;
const int Grid_Strings			= 6;
const int Grid_AngleUnits		= 7;
const int Grid_Integers			= 8;
const int Grid_Units			= 9;
const int Grid_UpperCase		= 10;
const int Grid_LowerCase		= 11;
const int Grid_Password			= 12;
const int Grid_MultiLine		= 13;
const int Grid_Currency			= 14;
const int Grid_Date				= 15;
const int Grid_Time				= 16;
const int Grid_Percentage		= 17;
const int Grid_DropDown			= 18;
const int Grid_ArrowHead		= 19;
const int Grid_AcadColors		= 20;
const int Grid_TextStyleList	= 21;
const int Grid_PlotStyleNames	= 22;
const int Grid_PlotStyleTables	= 23;
const int Grid_PlotterList		= 24;
const int Grid_Fonts			= 25;
const int Grid_DriveList		= 26;
const int Grid_LayerList		= 27;
const int Grid_DimStyleList		= 28;
const int Grid_ImageDropList	= 29;
const int Grid_AcadColorCell	= 30;
const int Grid_TrueColorCell	= 31;
const int Grid_LineWeightCell	= 32;
const int Grid_LinetypeCell		= 33;
const int Grid_DirectoryCell	= 34;
const int Grid_DwgFilesCell		= 35;
const int Grid_Strings_Combo	= 36;
const int Grid_AngleUnits_Combo	= 37;
const int Grid_Integers_Combo	= 38;
const int Grid_Units_Combo		= 39;
const int Grid_UpperCase_Combo	= 40;
const int Grid_LowerCase_Combo	= 41;

const int nNumOfTextBoxes = 13; 
const int nNumOfComboBoxes = 14; 

struct _Style
{
	int nStyle;
	int nData1; // holds the default image value for style 3 and holds the date and time styles for 15 and 16
	int nData2; // holds the alternate image value for style 3.
	CStringArray sStrings; // holds the data that will fill a combo box list drop down.
	CArray<int, int> nInts; // holds the data that will fill an image combo box list drop down.

	_Style() 
	{
		nStyle = 0;
		nData1 = 0;
		nData2 = 0;
	} 

};

struct _RowData
{
	long nItemData; // holds the item data because the row item data is now used.
	CArray<_Style*, _Style*> m_Cells;

	_RowData()
	{
		nItemData = 0;
	}
	~_RowData()
	{
		for (int i=0; i<m_Cells.GetSize(); i++)
		{
			if (m_Cells[i] != NULL)
				delete m_Cells[i];
		}
	}
};


/////////////////////////////////////////////////////////////////////////////
// CGridCtrl window

class CGridCtrl : public CListCtrl, public CDialogControl
{

public:
	int HighlightType; // One of EHighligh enums
	COLORREF ForegroundColor;
	CGridCtrlHdr m_HeaderCtrl;
	CThemeHelperST*		m_pTheme;
	HTHEME m_hTheme;
	
	int m_nRowHeight;
	bool m_bHasRowHeader;

	RefCountedPtr< CPropertyObject > m_pColCaptions;	
	RefCountedPtr< CPropertyObject > m_pColWidths;	
	RefCountedPtr< CPropertyObject > m_pColImages;	
	RefCountedPtr< CPropertyObject > m_pColStyles;	
	RefCountedPtr< CPropertyObject > m_pColAlignment;	
	RefCountedPtr< CPropertyObject > m_pColDefault;	
	RefCountedPtr< CPropertyObject > m_pColAlternate;	

	CImageList		*m_pArrowImages;
	CImageList		*m_pFontImages;
	CImageList		*m_pImageList;

	bool				m_bHasGridLines;

	enum EHighlight { HIGHLIGHT_NORMAL, HIGHLIGHT_ALLCOLUMNS, HIGHLIGHT_ROW };

	CImageList			m_DefaultImageList;

	COLORREF alertnateColor;
	bool	 m_bOrientationVer;

	int m_nRowSelected;
	int m_nColSelected;

// Construction
public:
	CGridCtrl( CDclControlObject* pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CGridCtrl();

// DialogControl Interface
public:
	operator TDialogControlPtr () { return TDialogControlLockedPtr( *this ); } //to ensure it doesn't get auto deleted
	virtual bool Create( CWnd* pParentWnd, UINT nID );
	virtual DWORD GetWndStyle() const;
	virtual bool OnApplyProperty( RefCountedPtr< CPropertyObject > pProp );

public:
	CRect GetCurSelRect();
	int GetCurCellStyle();
	void GetImageDropListItems(int nRow, int nCol, int &nImage, CString &sText);
	_RowData * GetRowData(int nRow);
	_Style * GetCellData(int nRow, int nCol);
	void SetCellStyle(int nRow, int nCol, int nStyle);
	int GetCellStyle(int nRow, int nCol);
	void UpdateCell(CString sText);
	void InitToolTip();
	
// Operations
public:
	virtual void HideEditControls() { return; }
	BOOL SortTextItems( int nCol, BOOL bAscending, int low = 0, int high = -1);
	bool SortNumericItems( int nCol, BOOL bAscending, int low = 0, int high = -1);
	void SetItemTextImage( int nRow,int nCol, CString sText, int nImage);
	void SetupColumns();
	void SetItemImage( int nRow, int nCol, int nImage);
	int GetItemImage(int nRow, int nCol);
	void SetColumn(int nIndex);
	void SetThemeHelper(CThemeHelperST* pTheme);
	void CellHitTest(CPoint point, int &nRow, int &nCol);
	void DrawTrueColor(CDC* pDC, CRect rc, int &nColor, CString &sText);
	void DrawLineWeights(CDC* pDC, CRect rc, int nImage);
	void DrawCheckBox(CDC* pDC, CRect rc, bool bPressed, bool bSelected);
	void DrawOptionButton(CDC* pDC, CRect rc, bool bPressed, COLORREF backColor, BOOL bSelected);
	void ShowCurSel();
	void RefreshCell(int nRow, int nCol);

protected:
	DECLARE_MESSAGE_MAP()

// Generated message map functions
protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void PostNcDestroy();
};
