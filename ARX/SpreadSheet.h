// SpreadSheet.h : header file
//

#pragma once

#include "OdclListCtrl.h"
#include "SpreadSheetHdr.h"
#include "ThemeHelperST.h"
#include "XPStyleButtonST.h"
#include "PPToolTip.h"

class CThemeHelperST;
class CPropertyObject;
class CDclControlObject;

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
// CSpreadSheet window

class CSpreadSheet : public CListCtrl
{
// Construction
public:
	CSpreadSheet();

	enum EHighlight {
        HIGHLIGHT_NORMAL, 
        HIGHLIGHT_ALLCOLUMNS, 
        HIGHLIGHT_ROW
    };
    

// Attributes
public:
	int HighlightType; // One of EHighligh enums
	COLORREF ForegroundColor;
	CSpreadSheetHdr m_HeaderCtrl;
	bool			m_bHeaderIsSubclassed;
	void SetThemeHelper(CThemeHelperST* pTheme);
	CThemeHelperST*		m_pTheme;
	HTHEME m_hTheme;
	
	int m_nRowHeight;
	bool m_bHasRowHeader;
	bool m_bShowHighlight;
	
	
	CPropertyObject *m_pColCaptions;	
	CPropertyObject *m_pColWidths;	
	CPropertyObject *m_pColImages;	
	CPropertyObject *m_pColStyles;	
	CPropertyObject *m_pColAlignment;	
	CPropertyObject *m_pColDefault;	
	CPropertyObject *m_pColAlternate;	

	CImageList		*m_pArrowImages;
	CImageList		*m_pFontImages;
	CImageList		*m_pImageList;

	CDclControlObject	*m_ArxControl;
	CControlPane		*m_pParentCtrlPane;
	bool				m_bHasGridLines;

	// Cell Editing.
public:
	CRect GetCurSelRect();
	void DoFileDlg(int nStyle);
	void EndEditControls(CWnd *pWnd);
	int GetCurCellStyle();
	void GetImageDropListItems(int nRow, int nCol, int &nImage, CString &sText);
	_RowData * GetRowData(int nRow);
	_Style * GetCellData(int nRow, int nCol);
	void SetCellStyle(int nRow, int nCol, int nStyle);
	int GetCellStyle(int nRow, int nCol);
	void HideEditControls();
	void CallBeginLabelEdit(CPoint point);
	void OnEndlabeledit(CString sText);
	void UpdateCell(CString sText);
	bool m_bEditCells;
	bool m_bInvokeWithSendString;
	CImageList			m_DefaultImageList;
	
	CThemeHelperST		m_ThemeHelper;
	
	CXPStyleButtonST *m_pEllipsesButton;
	CXPStyleButtonST *m_pPickButton;
	CXPStyleButtonST *m_pFolderButton;

	COLORREF alertnateColor;
	bool	 m_bOrientationVer;
	
	CWnd *m_pTextBox[nNumOfTextBoxes];
	CWnd *m_pComboBox[nNumOfComboBoxes];
		
	int	m_nEditSubItem;
	int m_nRowSelected;
	int m_nColSelected;

	void SetTooltipText(CString* spText, BOOL bActivate = TRUE);
	void InitToolTip();
	CPPToolTip m_ToolTip;
	
// Operations
public:
	BOOL SortTextItems( int nCol, BOOL bAscending, int low = 0, int high = -1);
	bool SortNumericItems( int nCol, BOOL bAscending, int low = 0, int high = -1);
	void SetItemTextImage( int nRow,int nCol, CString sText, int nImage);
	bool acad_truecolordlg(COLORREF color, int curColor, int nIndex, CString sPantone);
	void SetupColumns();
	void SetItemImage( int nRow, int nCol, int nImage);
	int GetItemImage(int nRow, int nCol);
	void SetColumn(int nIndex);
	void CellHitTest(CPoint point, int &nRow, int &nCol);
	void DrawColor(CDC* pDC, CRect rc, int &nColor, CString &sText);
	void DrawTrueColor(CDC* pDC, CRect rc, int &nColor, CString &sText);
	void DrawFontIcons(CDC* pDC, CRect rc, int &nImage, CString &sText);
	void DrawArrowHeads(CDC* pDC, CRect rc, int &nImage, CString &sText);
	void DrawLineWeights(CDC* pDC, CRect rc, AcDb::LineWeight LW);
	void DrawLineWeights(CDC* pDC, CRect rc, int nImage);
	void DrawCheckBox(CDC* pDC, CRect rc, bool bPressed, bool bSelected);
	void DrawOptionButton(CDC* pDC, CRect rc, bool bPressed, COLORREF backColor, BOOL bSelected);
	void SetCurSel(int nRow, int nCol);
	void ShowCurSel();
	void RefreshCell(int nRow, int nCol);
	void CheckLayer(CString &sLayer, int &nImage);

	void MoveUp();
	void MoveDown();
	
// celling editing operations
	void EditCellNow(UINT nChar = 0);
	void DoEditCellNow(int nStyle, UINT nChar = 0);
	void ShowTextBox(int nRow, int nCol, int nStyle, UINT nChar = 0);
	void ShowImageComboBox(int nRow, int nCol, CStringArray &sStrings, CArray<int, int> &nItems);
	void ShowComboBox(int nRow, int nCol, int nStyle, CStringArray &sStrings);
	void ShowEllipsesButton(int nRow, int nCol, int nAsPick);
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSpreadSheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSpreadSheet();

	BOOL Create(CDclControlObject* pControl, CWnd* pParentWnd, UINT nID);

	// Generated message map functions
protected:
	void SubclassHeaderControl();
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void PreSubclassWindow();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	
	//{{AFX_MSG(CSpreadSheet)	
	afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);	
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);	
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSetfocus(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnKillfocus(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG	
	DECLARE_MESSAGE_MAP()
};
