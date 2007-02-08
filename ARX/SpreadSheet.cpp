// SpreadSheet.cpp : implementation file
//

#include "stdafx.h"
#include "SpreadSheet.h"
#include "DclControlObject.h"
#include "PropertyObject.h"
#include "PropertyIds.h"
#include "ToolTips.h"
#include "Workspace.h"
#include "Resource.h"
#include "VdclColorComboBox.h"
#include "ComboBoxHolder.h"
#include "AcadColorTable.h"
#include "FontCombo.h"
#include "VdclArrowHeadComboBox.h"
#include "InvokeMethod.h"
#include "VdclAngleEdit.h"
#include "VdclComboBox.h"
#include "VdclNumericEdit.h"
#include "OdclEdit.h"
#include "DirDialog.h"
#include "VdclSymbolEdit.h"
#include "CurrencyEdit.h"
#include "DateTimeHolder.h"
#include "PercentageEdit.h"
#include "VdclComboBoxEx.h"
#include "OdclDirStylesCombo.h"
#include "VdclPlotStyleNamesComboBox.h"
#include "VdclPlotStyleTablesComboBox.h"
#include "PrinterComboBox.h"
#include "ComboBoxFolder.h"
#include "OdclLayerCombo.h"
#include "LineWeightDlg.h"
#include "LinetypeDlg.h"

extern void SetPlotterPaperSizesCB(CComboBox *pComboBox);

#define HP_HEADERITEM			0x00000001

#define BP_PUSHBUTTON			0x00000001
#define BP_RADIOBUTTON			0x00000002
#define BP_CHECKBOX				0x00000003

#define PBS_NORMAL				0x00000001
#define PBS_HOT					0x00000002
#define PBS_PRESSED				0x00000003
#define PBS_DISABLED			0x00000004
#define PBS_DEFAULTED			0x00000005

#define RBS_UNCHECKEDNORMAL		0x00000001
#define RBS_UNCHECKEDHOT		0x00000002
#define RBS_UNCHECKEDPRESSED	0x00000003
#define RBS_UNCHECKEDDISABLED	0x00000004
#define RBS_CHECKEDNORMAL		0x00000005
#define RBS_CHECKEDHOT			0x00000006
#define RBS_CHECKEDPRESSED		0x00000007
#define RBS_CHECKEDDISABLED		0x00000008


//extern CAcExtensionModule InterfaceArxDLL;

const TCHAR sDotDotDot[] = _T("...");
const TCHAR sPassword[] = _T("*");					
LPCWSTR sBtn = L"HEADER";
const TCHAR sComma[] = _T(",");
const TCHAR sDollarSign[] = _T("$");
const TCHAR sAcad_TrueColorDlg[] = _T("acad_truecolordlg");
const TCHAR sSpace[] = _T(" ");
const TCHAR sRow[] = _T("Row ");
LPCWSTR sButton = L"BUTTON";
const TCHAR sDollar[] = _T("$");
const TCHAR sDateFormat[] = _T("DD/MM/YYYY");
const TCHAR sTimeFormat[] = _T("hh:mm:ss");
const TCHAR sDot2[] = _T("%.2f mm");
const TCHAR sDot3[] = _T("%.3f\"");
const TCHAR sDwg[] = _T("dwg");
const TCHAR sReturn[] = _T("\r\n");

static LPTSTR s_DateFormats[] =
{
	_T("d/M/yy"),
	_T("dd/MM/yy"),
	_T("dd/MMM/yy"),
	_T("ddd/MM/yy"),
	_T("ddd/MMM/yyyy"),
	_T("dddd/MMMM/yyyy"),
	_T("M/d/yy"),
	_T("MM/dd/yy"),
	_T("MM/ddd/yy"),
	_T("MMM/dd/yy"),
	_T("MMM/ddd/yyyy"),
	_T("MMMM/dddd/yyyy"),
	NULL
};
static LPTSTR s_TimeFormats[] =
{
	_T("h':'m':'s"),
	_T("hh':'mm':'ss"),
	_T("h':'m':'s' 'tt"),
	_T("hh':'mm':'ss' 'tt"),
	_T("H':'m':'s"),
	_T("HH':'mm':'ss"),

	NULL
	
};

CString _ltoa(long nValue)
{
	char value[80];
	_ltoa(nValue, value, 10);
	return value;
}	
/////////////////////////////////////////////////////////////////////////////
// CSpreadSheet

CSpreadSheet::CSpreadSheet()
{
	ForegroundColor = GetSysColor(COLOR_BTNTEXT);
	m_bHasRowHeader = true;
	m_bHeaderIsSubclassed = false;
	m_pTheme = NULL;

	m_nRowSelected = -1;
	m_nColSelected = -2;
	m_bShowHighlight = true;
	m_pArrowImages = NULL;
	m_pFontImages = NULL;
	m_bHasGridLines = false;
	m_pImageList = NULL;
}

CSpreadSheet::~CSpreadSheet()
{
	if (m_pFontImages != NULL)
	{
		m_pFontImages->DeleteImageList();
		delete m_pFontImages;
		m_pFontImages = NULL;
	}

	if (m_pArrowImages != NULL)
	{
		m_pArrowImages->DeleteImageList();
		delete m_pArrowImages;
		m_pArrowImages = NULL;
	}

	if (m_pImageList != NULL)
	{
		m_pImageList->DeleteImageList();
		delete m_pImageList;
	}
	m_HeaderCtrl.DestroyWindow();
}


BEGIN_MESSAGE_MAP(CSpreadSheet, CListCtrl)
	//{{AFX_MSG_MAP(CSpreadSheet)
	ON_WM_MEASUREITEM()
	ON_WM_DESTROY()
	ON_WM_NCCALCSIZE()	
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_MEASUREITEM_REFLECT()
	ON_WM_DRAWITEM_REFLECT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()	
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDBLCLK()	
	ON_WM_SIZE()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_NOTIFY_REFLECT(NM_SETFOCUS, OnSetfocus)
	ON_NOTIFY_REFLECT(NM_KILLFOCUS, OnKillfocus)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnclick)		
	ON_WM_MOUSEMOVE()
	ON_WM_KEYUP()
	//}}AFX_MSG_MAP	
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSpreadSheet message handlers

void CSpreadSheet::SetThemeHelper(CThemeHelperST* pTheme)
{	
	m_pTheme = pTheme;
}


void CSpreadSheet::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp) 
{
	UpdateWindow();	
	CListCtrl::OnNcCalcSize(bCalcValidRects, lpncsp);	
}

BOOL CSpreadSheet::Create(CDclControlObject* pControl, CWnd* pParentWnd, UINT nID) 
{
	BOOL RetVal;
	DWORD dwStyle;
	CRect ArxRect;

	// set the arx control pointer
    m_ArxControl = pControl;

	// get the rectangle of the new control
	ArxRect.top = pControl->m_pTop->GetLongValue();
	ArxRect.left = pControl->m_pLeft->GetLongValue();
	ArxRect.bottom = pControl->m_pHeight->GetLongValue() + ArxRect.top;
	ArxRect.right = pControl->m_pWidth->GetLongValue() + ArxRect.left;
	
	
	dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | LVS_OWNERDRAWFIXED | 
			  WS_CLIPCHILDREN | LVS_SINGLESEL;

	m_bEditCells = true;

	// init all the pointers for cell editing controls.
  int i;
	for (i=0; i<nNumOfTextBoxes; i++)
	{
		m_pTextBox[i] = NULL;
	}
	for (i=0; i<nNumOfComboBoxes; i++)
	{
		m_pComboBox[i] = NULL;
	}
	m_pFolderButton = NULL;
	m_pEllipsesButton = NULL;
	m_pPickButton = NULL;

	if (pControl->GetBoolProperty(nIsTabStop) != FALSE)
		dwStyle = dwStyle | WS_TABSTOP;
	else
		dwStyle = dwStyle | WS_GROUP;

	m_bHasRowHeader = m_ArxControl->GetBoolProperty(nRowHeader) == TRUE;
	
	dwStyle = dwStyle | LVS_REPORT;
	if (m_ArxControl->GetBoolProperty(nColHeader) == FALSE)
		dwStyle = dwStyle | LVS_NOCOLUMNHEADER;
		
	m_nRowHeight = m_ArxControl->GetLngProperty(nRowHeight);

	switch (pControl->GetLngProperty(nListViewSort))
	{
	case 1:
		dwStyle = dwStyle | LVS_SORTDESCENDING;
		break;
	case 2:
		dwStyle = dwStyle | LVS_SORTASCENDING;
		break;
	}
	
	if (m_ArxControl->GetBoolProperty(nLabelWrap) == FALSE)
		dwStyle = dwStyle | LVS_NOLABELWRAP;
	if (m_ArxControl->GetBoolProperty(nShowSelectAlways) == TRUE)
		dwStyle = dwStyle | LVS_SHOWSELALWAYS;
	if (m_ArxControl->GetBoolProperty(nMultiSelect) == FALSE)
		dwStyle = dwStyle | LVS_SINGLESEL;
	
	m_bEditCells = true;
		
	m_bOrientationVer = m_ArxControl->GetLngProperty(nAlternateOrient);
	alertnateColor = m_ArxControl->GetColorProperty(nAlternateColor);

	RetVal = CListCtrl::Create( dwStyle, ArxRect, pParentWnd, nID );

	VERIFY(CListCtrl::SubclassDlgItem(nID, pParentWnd));
	
	// set the extended list view styles
	SetExtendedStyle(GetExtendedStyle()|LVS_EX_SUBITEMIMAGES);

	m_bHasGridLines = pControl->GetBoolProperty(nGridLines) == TRUE;
		
	InitToolTip();
	SetToolTipEx(this, m_ToolTip, pControl);


	switch (m_ArxControl->GetLngProperty(nEventInvoke))
	{
	case 1:
		m_bInvokeWithSendString = true;
		break;
	default:
		m_bInvokeWithSendString = false;
		break;
	}

	m_ArxControl = pControl;
	
	m_pColCaptions = m_ArxControl->GetPropertyObject(nColumnCaptions);	
	m_pColWidths = m_ArxControl->GetPropertyObject(nColumnWidths);	
	m_pColImages = m_ArxControl->GetPropertyObject(nColumnImages);	
	m_pColStyles = m_ArxControl->GetPropertyObject(nColumnStyles);
	m_pColAlignment = m_ArxControl->GetPropertyObject(nColumnAlignments);
	m_pColDefault = m_ArxControl->GetPropertyObject(nColumnDefaultImages);	
	m_pColAlternate = m_ArxControl->GetPropertyObject(nColumnAlternateImages);	

	SetupColumns();

	SubclassHeaderControl();

	// init all the pointers for cell editing controls.
	for (i=0; i<nNumOfTextBoxes; i++)
	{
		m_pTextBox[i] = NULL;
	}
	for (i=0; i<nNumOfComboBoxes; i++)
	{
		m_pComboBox[i] = NULL;
	}
	m_pFolderButton = NULL;
	m_pEllipsesButton = NULL;
	m_pPickButton = NULL;

	return RetVal;
	
}


void CSpreadSheet::SetColumn(int nIndex)
{	
	HD_ITEM curItem;
	LPTSTR  pStrTemp;
	CHeaderCtrl* pHdrCtrl = NULL;

	// retrieve embedded header control			
	pHdrCtrl = GetHeaderCtrl();

	// add bmaps to each header item
	pHdrCtrl->GetItem(nIndex, &curItem);
	
	if (m_pColImages->GetIntArrayPtr()->at(nIndex) == -1)
	{
		curItem.mask= HDI_TEXT | HDI_FORMAT;
		curItem.iImage = -1;
	}
	else
	{
		curItem.mask= HDI_TEXT | HDI_IMAGE | HDI_FORMAT;
		curItem.iImage= m_pColImages->GetIntArrayPtr()->at(nIndex);
	}
	
	CString sCaption = m_pColCaptions->GetStringArrayPtr()->at(nIndex);
	pStrTemp = sCaption.GetBuffer(sCaption.GetLength());
	curItem.pszText = pStrTemp;
	
	switch(m_pColAlignment->GetIntArrayPtr()->at(nIndex))
	{					
	case 1:
		curItem.fmt= HDF_CENTER | HDF_STRING;
		break;
	case 2:
		curItem.fmt= HDF_RIGHT | HDF_STRING;
		break;				
	default:
		curItem.fmt= HDF_LEFT | HDF_STRING;
		break;
	}

	if (m_pColImages->GetIntArrayPtr()->at(nIndex) == -1)
	{
		// do nothing
	}
	else
	{
		curItem.fmt = curItem.fmt | HDF_IMAGE;
	}

	pHdrCtrl->SetItem(nIndex, &curItem);
}

void CSpreadSheet::SetupColumns()
{		
	for (int i=0; i<m_pColWidths->GetIntArrayPtr()->size(); i++)
	{
		InsertColumn(i, m_pColCaptions->GetStringItem(i), LVCFMT_LEFT, m_pColWidths->GetIntArrayPtr()->at(i));
		SetColumn(i);
	}
}


void CSpreadSheet::PreSubclassWindow() 
{
	//use our custom CHeaderCtrl
	CListCtrl::PreSubclassWindow();

	SubclassHeaderControl();
}


///////////////////////////////////////////////////////////////////////////////
// OnCreate
int CSpreadSheet::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CListCtrl::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}

	// When the CXListCtrl object is created via a call to Create(), instead
	// of via a dialog box template, we must subclass the header control
	// window here because it does not exist when the PreSubclassWindow()
	// function is called.

	
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// SubclassHeaderControl
void CSpreadSheet::SubclassHeaderControl()
{
	if (m_bHeaderIsSubclassed)
		return;

	if (m_ArxControl->GetBoolProperty(nColHeader) == FALSE)
		return;

	// if the list control has a header control window, then
	// subclass it

	// Thanks to Alberto Gattegno and Alon Peleg  and their article
	// "A Multiline Header Control Inside a CListCtrl" for easy way
	// to determine if the header control exists.
	
	CHeaderCtrl* pHeader = GetHeaderCtrl();
	
	if (pHeader != NULL)
	{
		m_HeaderCtrl.SubclassWindow(pHeader->m_hWnd);
		m_HeaderCtrl.m_pParent = this;
		m_bHeaderIsSubclassed = true;		
	}
}


bool getvar(CString varname)
{	
	struct resbuf rsVarVal;

	if (acedGetVar(varname, &rsVarVal) != RTNORM)
		return false;
	
	if (rsVarVal.restype == RTT)
		return true;
	if (rsVarVal.restype == RTNIL)
		return FALSE;
	
	return rsVarVal.resval.rint == 1;	
}

void CSpreadSheet::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	//its not meself
	if( GetFocus() != this) 
		SetFocus();
	
	HideEditControls();
	
	//Invalidate();

	CListCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CSpreadSheet::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if( GetFocus() != this) 
		SetFocus();

	HideEditControls();
	CListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CSpreadSheet::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if (lpDrawItemStruct->CtlType != ODT_LISTVIEW)
        return;

	if(lpDrawItemStruct->itemAction != ODA_DRAWENTIRE)
		return;

	COLORREF rowColor = alertnateColor;
	COLORREF backGround = m_ArxControl->GetColorProperty(nAcadColor);
	COLORREF back = backGround;
	COLORREF foreGround = ::GetSysColor(COLOR_WINDOWTEXT);
	
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	CRect rcItem(lpDrawItemStruct->rcItem); 
	
	CRect rcClipBox;
	pDC->GetClipBox(rcClipBox);

    //CMemDC pDC(CDC::FromHandle(lpDrawItemStruct->hDC), rcItem);
    int nItem = lpDrawItemStruct->itemID;
    CImageList* pImageList; 
    // Save dc state 
    //int nSavedDC = pDC->SaveDC();
    
	//CDC dcm;    
	CRect rc;
    GetClientRect(rc);    
    //pDC->CreateCompatibleDC(pDC);
    //CBitmap bmt;
    //bmt.CreateCompatibleBitmap(pDC,rc.Width(),rc.Height());
    //CBitmap *pBitmapOld = pDC->SelectObject(&bmt);
    HGDIOBJ pOldFont = SelectObject(pDC->m_hDC, GetFont()->m_hObject);

  //  pDC->Rectangle(rc);// make the work of the OnEraseBkgnd function
  

    // Get item image and state info 
    LV_ITEM lvi;
    lvi.mask = LVIF_IMAGE | LVIF_STATE; 
    lvi.iItem = nItem; 
    lvi.iSubItem = 0;
    lvi.stateMask = 0xFFFF; 
    // get all state flags 
    GetItem(&lvi);
    
    // Should the item be highlighted
    BOOL bHighlight =
        ((lvi.state & LVIS_DROPHILITED) ||
        ((lvi.state & LVIS_SELECTED)
        && ((GetFocus()
        == this) || (GetStyle() & LVS_SHOWSELALWAYS))));

	int nCellStyle = GetCellStyle(lvi.iItem, 0);

	if (bHighlight && nCellStyle == Grid_EllipsesButtons)
		bHighlight = false;
	if (bHighlight && nCellStyle == Grid_PickButtons)
		bHighlight = false;
	if (bHighlight && nCellStyle == Grid_SwitchableIcons)
		bHighlight = false;

    // Get rectangles for drawing 
    CRect rcBounds, rcLabel, rcIcon;
    GetItemRect(nItem, rcBounds, LVIR_BOUNDS);
    GetItemRect(nItem, rcLabel, LVIR_LABEL); 
    GetItemRect(nItem, rcIcon, LVIR_ICON);
    CRect rcCol(rcBounds);
    CString sLabel = GetItemText(nItem, 0);
  
	// Labels are offset by a certain amount 
    // This offset is related to the width of a space character
    int offset = 4;//pDC->GetTextExtent(_T(sSpace), 1).cx*2;
    
	bool bWordWrap = false;
	if (rcLabel.Height() >= (pDC->GetTextExtent(sSpace, 1).cy*2) + 1)
		bWordWrap = true;
	
    
    CRect rcHighlight;
    CRect rcWnd; 
    int nExt; 
    
    switch (HighlightType) 
	{ 
    case HIGHLIGHT_NORMAL: 
        nExt = pDC->GetOutputTextExtent(sLabel).cx + offset; 
        rcHighlight = rcLabel;
        if( rcLabel.left + nExt < rcLabel.right )
            rcHighlight.right = rcLabel.left + nExt; 
        break; 
    case HIGHLIGHT_ALLCOLUMNS:
        rcHighlight = rcBounds;
        rcHighlight.left = rcLabel.left;
        break; 
    case HIGHLIGHT_ROW:
        GetClientRect(&rcWnd);
        rcHighlight = rcBounds;
        rcHighlight.left = rcLabel.left;
        rcHighlight.right = rcWnd.right;
        
        break;
    default: 
        rcHighlight = rcLabel; 
    } 
    
	CRect rcBtn = rcBounds;
	//rcBtn.left = 0;
	//rcBtn.top++;
	rcBtn.bottom--;
	rcBtn.right = rcBtn.left + CListCtrl::GetColumnWidth(0);

	HTHEME	hTheme = NULL;
		
	
	if (m_bHasRowHeader == true)
	{		
		hTheme = m_pTheme->OpenThemeData(m_hWnd, sBtn);
		if (hTheme == NULL) // If the handle cannot be retrieved, 
							// go back to non-visual style drawing
							// code.
		{			
			if (nItem == GetTopIndex())
			{
				rcBtn.top--;
				rcBtn.top--;
			}
			
			rcBtn.bottom++;

			pDC->SetTextColor(::GetSysColor(COLOR_BTNTEXT));
			pDC->SetBkColor(::GetSysColor(COLOR_BTNFACE));
			pDC->FillRect(rcBtn,
				&CBrush(::GetSysColor(COLOR_BTNFACE)));

			// draw the solid rectangle
			::DrawEdge(pDC->m_hDC, &rcBtn, BDR_RAISEDINNER, BF_RECT);
		}    
		else
		{
			CRect rcTheme = rcBtn;

			rcTheme.right++;
			rcTheme.right++;
			rcTheme.bottom++;
			rcTheme.bottom++;
			rcTheme.bottom++;
			int iStateId = 0;

			m_pTheme->DrawThemeBackground(hTheme, m_hWnd, pDC->GetSafeHdc(), HP_HEADERITEM, iStateId, &rcTheme, NULL);

			m_pTheme->CloseThemeData(hTheme);

			hTheme = NULL;

			CPoint point;
		
			COLORREF rgb = ::GetSysColor(COLOR_BTNHIGHLIGHT);
			HGDIOBJ pen = ::CreatePen(PS_SOLID, 1, rgb);
			HGDIOBJ OldPen = SelectObject(pDC->m_hDC, pen);
			
			MoveToEx(pDC->m_hDC, rcTheme.left, rcTheme.top, &point);
			LineTo(pDC->m_hDC, rcTheme.right+1, rcTheme.top);	

			SelectObject(pDC->m_hDC, OldPen);			
			DeleteObject(pen);
		
				
			if (nItem == GetItemCount()-1)
			{
				HGDIOBJ pen = ::CreatePen(PS_SOLID, 1, backGround);
				HGDIOBJ OldPen = SelectObject(pDC->m_hDC, pen);
				
				pDC->MoveTo(CPoint(rcBtn.right,rcBtn.bottom+1));
				pDC->LineTo(CPoint(rcBtn.right + 3,rcBtn.bottom+1));
				pDC->MoveTo(CPoint(rcBtn.left,rcBtn.bottom+2));
				pDC->LineTo(CPoint(rcBtn.right + 3,rcBtn.bottom+2));
			
				SelectObject(pDC->m_hDC, OldPen);			
				DeleteObject(pen);
			}
		
		}
		
	}
	else
	{
		// Draw the background color
		if (bHighlight) {
			pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
			pDC->SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));
			pDC->FillRect(rcHighlight,
				&CBrush(::GetSysColor(COLOR_HIGHLIGHT))); 
	    
		} else {
			pDC->FillRect(rcHighlight,
				&CBrush(backGround)); 
			if (ForegroundColor != 0)
				pDC->SetTextColor(ForegroundColor); 
		}
	}

	
    // Set clip region 
    rcCol.right = rcCol.left + GetColumnWidth(0); 
    //CRgn rgn;
	//CRect rc2 = rcCol;
	//rc2.bottom++;
	//rc2.right++;
    //rgn.CreateRectRgnIndirect(&rc2); 
    //pDC->SelectClipRgn(&rgn);
    //rgn.DeleteObject(); 
    

    // Draw state icon 
    if (lvi.state & LVIS_STATEIMAGEMASK) 
	{
        int nImage = ((lvi.state & LVIS_STATEIMAGEMASK)>>12) - 1;
        pImageList = GetImageList(LVSIL_STATE); 
        if (pImageList) 
		{
            pImageList->Draw(pDC,
                nImage,CPoint(rcCol.left, rcCol.top+1), ILD_TRANSPARENT);            
        } 
    } 
    

    // Draw item label - Column 0    
    rcLabel.left = rcBounds.left + 4;//(offset/2)-1;
    rcLabel.right -= offset;

	rcLabel.top++;

	pDC->SetBkMode(TRANSPARENT);

	// Draw normal and overlay icon
	int nCellImage = GetItemImage(lvi.iItem, 0);
    pImageList = GetImageList(LVSIL_SMALL);
	if (lvi.iImage > -1 && pImageList != NULL)
	{
		IMAGEINFO inf;
		pImageList->GetImageInfo(nCellImage, &inf);
		rcLabel.left += inf.rcImage.right - inf.rcImage.left + 4;
	}

	rcLabel.top++;

	if (bWordWrap)
		pDC->DrawText(sLabel,-1,rcLabel,DT_LEFT |
			DT_NOPREFIX | DT_NOCLIP |
			DT_TOP | DT_WORDBREAK); 
	else
		pDC->DrawText(sLabel,-1,rcLabel,DT_LEFT |
			DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP |
			DT_TOP | DT_END_ELLIPSIS); 

	rcLabel.top--;

    // Draw labels for remaining columns
    LV_COLUMN lvc; 
    lvc.mask = LVCF_FMT | LVCF_WIDTH;
    if (HighlightType == 0) 
	{
        // Highlight only first column
        pDC->SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
        pDC->SetBkColor(backGround);
    } 

	rcIcon.top += 1;

	if (m_pColDefault->GetIntArrayPtr()->size() > 0)
	{
		if (nCellStyle == Grid_SwitchableIcons &&
			lvi.iImage != m_pColDefault->GetIntArrayPtr()->at(0) &&
			lvi.iImage != m_pColAlternate->GetIntArrayPtr()->at(0))
		{
			if (lvi.iImage < m_pColDefault->GetIntArrayPtr()->at(0))
				lvi.iImage = m_pColDefault->GetIntArrayPtr()->at(0);
			if (lvi.iImage > m_pColAlternate->GetIntArrayPtr()->at(0))
				lvi.iImage = m_pColAlternate->GetIntArrayPtr()->at(0);						
		}
	}

	if (pImageList && lvi.iImage > -1) 
	{
		UINT nOvlImageMask = lvi.state & LVIS_OVERLAYMASK;
        pImageList->Draw(pDC,
            lvi.iImage, CPoint(rcIcon.left, rcIcon.top+1),
            //(bHighlight ? ILD_BLEND50 : 0) |
            ILD_TRANSPARENT | nOvlImageMask ); 
    }

	rcBounds.right = rcHighlight.right > rcBounds.right ? rcHighlight.right : rcBounds.right;

	
	int nColumn = 1;
	if (!m_bHasRowHeader)
	{
		nColumn = 0;
		rcCol.right = rcBtn.left; 
	}

	// draw each cell item.
	for (; GetColumn(nColumn, &lvc) == TRUE; nColumn++) 
	{		
		int nCellStyle = GetCellStyle(lvi.iItem, nColumn);
		bool bHighlighted = false;
		rcCol.left = rcCol.right; 
        rcCol.right = rcCol.left + lvc.cx;

		
		if (m_bShowHighlight)
		{
			switch(nCellStyle)
			{
				case Grid_DirectoryCell:
				case Grid_DwgFilesCell:		
				case Grid_EllipsesButtons:
				case Grid_PickButtons:
				case Grid_SwitchableIcons:
					m_bShowHighlight = false;
					break;
			}
		}
		// if the entire column is selected.
		if (m_nColSelected == -1)
			m_bShowHighlight = true;
		
		if (m_nRowSelected == lvi.iItem &&
			m_nColSelected == nColumn &&
			m_bShowHighlight)
		{
			backGround = ::GetSysColor(COLOR_HIGHLIGHT);
			foreGround = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
			bHighlighted = true;
		}
		else if (m_nRowSelected == lvi.iItem &&
			     m_nColSelected == -1 &&
				 m_bShowHighlight)
		{
			backGround = ::GetSysColor(COLOR_HIGHLIGHT);
			foreGround = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
			bHighlighted = true;
		}
		else
		{
			backGround = back;
			foreGround = ::GetSysColor(COLOR_WINDOWTEXT);
		}

		if (!bHighlighted)
		{
			if (!m_bOrientationVer)
			{
				if ((lvi.iItem % 2) != 0)
					backGround = rowColor;
			}
			else
			{
				int n = 0;
				if (m_bHasRowHeader)
					n = 1;
				if (((nColumn+n) % 2) != 0)
					backGround = rowColor;
			}
		}
		pDC->SetBkColor(backGround);
		pDC->SetTextColor(foreGround);

		if (hTheme != NULL)
		{
			::SetBkColor(pDC->m_hDC, backGround);
			::ExtTextOut(pDC->m_hDC, 0, 0, ETO_OPAQUE, &rcCol, NULL, 0, NULL);
		}
		if (hTheme == NULL)
		{
			if (nItem == GetTopIndex())
			{
				rcCol.top--;
				rcCol.top--;
			}
			
			::SetBkColor(pDC->m_hDC, backGround);
			::ExtTextOut(pDC->m_hDC, 0, 0, ETO_OPAQUE, &rcCol, NULL, 0, NULL);
			
			if (nItem == GetTopIndex())
			{
				rcCol.top++;
				rcCol.top++;
			}
		}
		
        sLabel = GetItemText(nItem, nColumn); 

		
		if (m_bHasGridLines)
		{
			COLORREF rgb = ::GetSysColor(COLOR_BTNFACE);
			HGDIOBJ pen = ::CreatePen(PS_SOLID, 1, rgb);
			HGDIOBJ OldPen = SelectObject(pDC->m_hDC, pen);
			
			pDC->MoveTo(CPoint(rcCol.left, rcCol.bottom-1));
			pDC->LineTo(CPoint(rcCol.right-1, rcCol.bottom-1));
			pDC->LineTo(CPoint(rcCol.right-1, rcCol.top-1));
			
			SelectObject(pDC->m_hDC, OldPen);			
			DeleteObject(pen);
		}

		int nImageOffset = 0;
		int nCellImage = GetItemImage(lvi.iItem, nColumn);

		bool bDrawIcon = true;
			
		switch (nCellStyle)
		{
		case Grid_ImageDropList:
			{
			if (nCellImage == -1)
			{
				GetImageDropListItems(lvi.iItem, nColumn, nCellImage, sLabel);
				SetItemImage(lvi.iItem, nColumn, nCellImage);
				SetItemText(lvi.iItem, nColumn, sLabel);
			}
			break;
			}
		}
			

		// if this cell is to use switchable image icons...
		if (m_pColDefault->GetIntArrayPtr()->size() > nColumn)
		{
			// if switchable icons are to be drawn.
			if (nCellStyle == Grid_SwitchableIcons &&
				nCellImage != m_pColDefault->GetIntArrayPtr()->at(nColumn) &&
				nCellImage != m_pColAlternate->GetIntArrayPtr()->at(nColumn))
			{
				_Style *pData = GetCellData(lvi.iItem, nColumn);
				if (pData != NULL)
				{
					if (pData->nStyle == Grid_SwitchableIcons)
					{
						if (nCellImage < pData->nData1)
							nCellImage = pData->nData1;
						if (nCellImage > pData->nData2)
						{
							nCellImage = pData->nData2;
						}
					}					
					else
					{
						if (nCellImage < m_pColDefault->GetIntArrayPtr()->at(nColumn))
								nCellImage = m_pColDefault->GetIntArrayPtr()->at(nColumn);
							if (nCellImage > m_pColAlternate->GetIntArrayPtr()->at(nColumn))
							{
								nCellImage = m_pColAlternate->GetIntArrayPtr()->at(nColumn);
							}
					}
				}
				else
				{
					if (nCellImage < m_pColDefault->GetIntArrayPtr()->at(nColumn))
							nCellImage = m_pColDefault->GetIntArrayPtr()->at(nColumn);
						if (nCellImage > m_pColAlternate->GetIntArrayPtr()->at(nColumn))
						{
							nCellImage = m_pColAlternate->GetIntArrayPtr()->at(nColumn);
						}
				}
								
			}
		
			// if check boxes are to be drawn.
			switch (nCellStyle)
			{
				case Grid_CheckBoxes:
				case Grid_OptionButtons:
				case Grid_AcadColors:
				case Grid_ArrowHead:
				//case Grid_LineWeights:
				case Grid_LineWeightCell:
				case Grid_LayerList:
				case Grid_AcadColorCell:
				case Grid_TrueColorCell:
				case Grid_Fonts:
				{
					CRect rcBox;
					rcBox.left = rcCol.left + 2;
					rcBox.top = rcCol.top + 1;
					rcBox.right = rcBox.left + 13;
					rcBox.bottom = rcBox.top + 13;
					nImageOffset = 16;
					bDrawIcon = false;

					int nImage = GetItemImage(lvi.iItem, nColumn);					
				
					int nColStyle = nCellStyle;

					switch (nCellStyle)
					{
						case Grid_CheckBoxes:
						{
							if (GetItemText(lvi.iItem, nColumn).GetLength() == 0)
							{
								rcBox.top++;
								rcBox.left = rcCol.left + ((rcCol.Width()-13) / 2);
								rcBox.right = rcBox.left + 13;
								DrawCheckBox(pDC, rcBox, nImage == 1, bHighlighted);
							}
							else
							{
								rcBox.top++;
								rcBox.bottom++;
								DrawCheckBox(pDC, rcBox, nImage == 1, bHighlighted);
							}
							break;
						}
					
						case Grid_OptionButtons:
						{
							DrawOptionButton(pDC, rcBox, nImage == 1, backGround, bHighlighted);							
							break;
						}
					
						case Grid_AcadColors:
						case Grid_AcadColorCell:
						{
							if (nImage > 256)
								nImage = 0;

							if (nImage == -1)
							{
								DrawColor(pDC, rcBox, nImage, sLabel);
								SetItemText(lvi.iItem, nColumn, sLabel);
								SetItemImage(lvi.iItem, nColumn, nImage);
							}
							else
							{
								DrawColor(pDC, rcBox, nImage, sLabel);
							}
							break;
						}
						case Grid_TrueColorCell:
						{
							DrawTrueColor(pDC, rcBox, nImage, sLabel);
							/*
							COLORREF clr;
							if (nImage >= -1 && nImage <= 256)								
							{
								if (nImage == -1)
								{
									DrawColor(pDC, rcBox, nImage, sLabel);
									SetItemText(lvi.iItem, nColumn, sLabel);
									SetItemImage(lvi.iItem, nColumn, nImage);
								}
								else
								{
									DrawColor(pDC, rcBox, nImage, sLabel);
								}
							}
							if (nImage < 0)
							{	
								int nComma = sLabel.Find(sComma);
								int nComma2 = sLabel.Find(sComma, nComma+1);

								if (nComma > -1 && nComma2 > -1)
								{
									CString sRed = sLabel.Left(nComma);
									CString sGreen = sLabel.Mid(nComma+1, nComma2-nComma);
									CString sBlue = sLabel.Mid(nComma2+1);
									
									int nRed = atol(sRed);
									int nGreen = atol(sGreen);
									int nBlue = atol(sBlue);
									
									clr = RGB(nRed, nGreen, nBlue);									
									DrawTrueColor(pDC, rcBox, clr);
								}
							}
							if (nImage == 300) // if a color book color
							{
								AcCmColor clrBook;
								Acad::ErrorStatus es;

								CString sColorBook = sLabel;
								int n = sColorBook.Find(sDollarSign);
								sLabel = sColorBook.Mid(n+1);
								sColorBook = sColorBook.Left(n);

								es = accmGetColorFromColorBookName(clrBook,
									sColorBook,
									sLabel);								
								
								clr = RGB(clrBook.red(), clrBook.green(), clrBook.blue());	
								DrawTrueColor(pDC, rcBox, clr);
							}
							*/

							break;
						}
						case Grid_LayerList:
						{
							//if (nImage < 0 || nImage > 256)
							//	nImage = 0;

							CheckLayer(sLabel, nImage);
							DrawColor(pDC, rcBox, nImage, CString());
							break;
						}
					

						case Grid_ArrowHead:
						{
							if (nImage > 20)
								nImage = 20;

							if (nImage == -1)
							{
								DrawArrowHeads(pDC, rcBox, nImage, sLabel);
								SetItemText(lvi.iItem, nColumn, sLabel);
								SetItemImage(lvi.iItem, nColumn, nImage);
							}
							else
							{
								DrawArrowHeads(pDC, rcBox, nImage, sLabel);
							}
							break;
						}
					
						case Grid_Fonts:
						{
							if (nImage > 1)
								nImage = 0;

							if (nImage == -1)
							{
								DrawFontIcons(pDC, rcBox, nImage, sLabel);
								SetItemText(lvi.iItem, nColumn, sLabel);
								SetItemImage(lvi.iItem, nColumn, nImage);
							}
							else
							{
								DrawFontIcons(pDC, rcBox, nImage, sLabel);
							}
							break;
						}

						case Grid_LineWeightCell:
						{
							AcDb::LineWeight newLW = (AcDb::LineWeight)nImage;

							if (newLW == AcDb::kLnWtByLayer)
							{
								sLabel = theWorkspace.LoadResourceString(4221);
							}
							else if (newLW == AcDb::kLnWtByBlock)
							{
								sLabel = theWorkspace.LoadResourceString(4222);
							}
							else if (newLW == AcDb::kLnWtByLwDefault)
							{
								sLabel = theWorkspace.LoadResourceString(4223);
							}
							else if (getvar("LWUNITS"))
							{
								double dValue = ((double) newLW) / 100;
								sLabel.Format( sDot2, dValue);								
							}
							else
							{
								double dValue = ((double) newLW);								
								dValue = dValue / 2540.0;
								sLabel.Format( sDot3, dValue);
							}

							DrawLineWeights(pDC, rcBox, newLW);
						}
					}
				}
			}
			hTheme = NULL;

			lvi.iImage = nCellImage;
		}

		if (pImageList && nCellImage > -1 && bDrawIcon) 
		{
			rcIcon.left = rcCol.left + 2;
			IMAGEINFO inf;
			pImageList->GetImageInfo(nCellImage, &inf);
			nImageOffset = inf.rcImage.right - inf.rcImage.left + 2;
						
			//UINT nOvlImageMask = lvc.state & LVIS_OVERLAYMASK;
			pImageList->Draw(pDC,
				nCellImage, CPoint(rcIcon.left, rcIcon.top-1), 
				//(bHighlighted ? ILD_BLEND50 : 0) |
				ILD_TRANSPARENT);// | nOvlImageMask ); 			
		}

		if (sLabel.GetLength() > 0)
		{
	        
			// Get the text justification 
			UINT nJustify = DT_LEFT;
			switch (lvc.fmt & LVCFMT_JUSTIFYMASK) { 
			case LVCFMT_RIGHT:
				nJustify = DT_RIGHT; 
				break;
	            
			case LVCFMT_CENTER:
				nJustify = DT_CENTER;
				break;
	            
			default:
				break;
			}
	        
			rcLabel = rcCol;
	        
			rcLabel.left += offset + nImageOffset;
			rcLabel.right -= offset;
			rcLabel.top++;
			
			if (nCellStyle == Grid_Password)
			{
				int nCount = sLabel.GetLength();
				sLabel = CString();
				for (int k=0; k<nCount; k++)
				{
					sLabel += sPassword;
				}
			}

			if (nCellStyle == Grid_MultiLine)
				bWordWrap = true;

			if (bWordWrap)
				pDC->DrawText(sLabel,-1,rcLabel,DT_LEFT |
					DT_NOPREFIX | DT_EDITCONTROL | 
					DT_TOP | DT_WORDBREAK); 
			else
				pDC->DrawText(sLabel,-1,rcLabel,DT_LEFT |
					DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | 
					DT_TOP | DT_END_ELLIPSIS); 		
		}
		

    }

	// restore the old font ** a must
	SelectObject(pDC->m_hDC, pOldFont);
	
//	BitBlt(pDC->m_hDC,rcItem.left,rcItem.top,rcItem.Width()+1,m_nRowHeight+1,pDC->m_hDC,rcItem.left,rcItem.top,SRCCOPY);

 //   pDC->SelectObject(pBitmapOld);
    //pDC->RestoreDC( nSavedDC );
}


#define GLYPH_WIDTH 17 

void CSpreadSheet::DrawOptionButton(CDC* pDC, CRect rc, bool bPressed, COLORREF backColor, BOOL bSelected)
{	
	HTHEME hTheme = m_pTheme->OpenThemeData(GetSafeHwnd(), sButton);
		
	if (hTheme != NULL)
	{
		rc.top++;
		rc.bottom++;
		// draw the round option button
		if (bPressed)
			m_pTheme->DrawThemeBackground(hTheme, m_hWnd, pDC->GetSafeHdc(), 
				BP_RADIOBUTTON, 
				(bSelected ? RBS_CHECKEDNORMAL : RBS_CHECKEDNORMAL), &rc, NULL);
		else
			m_pTheme->DrawThemeBackground(hTheme, m_hWnd, pDC->GetSafeHdc(),
				BP_RADIOBUTTON, 
				(bSelected ? RBS_UNCHECKEDNORMAL : RBS_UNCHECKEDNORMAL), &rc, NULL);

		m_pTheme->CloseThemeData(hTheme);

		HGDIOBJ pen = ::CreatePen(PS_SOLID, 1, backColor);
		HGDIOBJ OldPen = SelectObject(pDC->m_hDC, pen);
		
		int nX = rc.left;
		int nY = rc.top;
		CPoint point;

		// draw top left corner, this is required to paint of the
		// greyish background because the option button is round but is 
		// drawn as a square.
		MoveToEx(pDC->m_hDC, nX, nY, &point);
		LineTo(pDC->m_hDC, nX+4+bSelected, nY);		

		MoveToEx(pDC->m_hDC, nX, nY, &point);
		LineTo(pDC->m_hDC, nX, nY+4+bSelected);		

		SetPixel(pDC->m_hDC, nX+1, nY+1, backColor);
		SetPixel(pDC->m_hDC, nX+1+bSelected, nY+1, backColor);
		SetPixel(pDC->m_hDC, nX+1, nY+1+bSelected, backColor);
		
		// draw the top right corner.
		MoveToEx(pDC->m_hDC, nX+9-bSelected, nY, &point);
		LineTo(pDC->m_hDC, nX+16, nY);		

		MoveToEx(pDC->m_hDC, nX+11-bSelected, nY+1, &point);
		LineTo(pDC->m_hDC, nX+16, nY+1);		

		MoveToEx(pDC->m_hDC, nX+13-bSelected, nY+2, &point);
		LineTo(pDC->m_hDC, nX+16, nY+2);		
		
		MoveToEx(pDC->m_hDC, nX+13, nY+2, &point);
		LineTo(pDC->m_hDC, nX+13, nY+5+bSelected);		
		
		MoveToEx(pDC->m_hDC, nX+12, nY+2-bSelected, &point);
		LineTo(pDC->m_hDC, nX+12, nY+4+bSelected);		
		
		if (bSelected)
			SetPixel(pDC->m_hDC, nX+11, nY+2, backColor);
		
		// draw the bottom left corner.
		MoveToEx(pDC->m_hDC, nX, nY+11, &point);
		LineTo(pDC->m_hDC, nX, nY+8-bSelected);		

		nY += 13;

		MoveToEx(pDC->m_hDC, nX, nY, &point);
		LineTo(pDC->m_hDC, nX+5+bSelected, nY);		

		MoveToEx(pDC->m_hDC, nX, nY-1, &point);
		LineTo(pDC->m_hDC, nX+4+bSelected, nY-1);		

		MoveToEx(pDC->m_hDC, nX, nY-2, &point);
		LineTo(pDC->m_hDC, nX+2+bSelected, nY-2);	

		if (bSelected)
			SetPixel(pDC->m_hDC, nX+1, nY-3, backColor);
		
		nX += 13;
		
		// draw the bottom right corner.
		MoveToEx(pDC->m_hDC, nX, nY, &point);
		LineTo(pDC->m_hDC, nX-10-bSelected, nY);		

		MoveToEx(pDC->m_hDC, nX, nY-1, &point);
		LineTo(pDC->m_hDC, nX-5-bSelected, nY-1);		

		MoveToEx(pDC->m_hDC, nX, nY-2, &point);
		LineTo(pDC->m_hDC, nX-3-bSelected, nY-2);	

		MoveToEx(pDC->m_hDC, nX, nY-3, &point);
		LineTo(pDC->m_hDC, nX, nY-10-bSelected);	

		MoveToEx(pDC->m_hDC, nX-1, nY-3, &point);
		LineTo(pDC->m_hDC, nX-1, nY-5-bSelected);	
	
		if (bSelected)
			SetPixel(pDC->m_hDC, nX-2, nY-3, backColor);
		
	}
	else
	{
		if (m_pImageList == NULL)
		{
			m_pImageList = new CImageList;
			m_pImageList->Create(13, 13, ILC_COLOR8|ILC_MASK, 0, 1);
	
			CBitmap bitmap1;
			bitmap1.LoadBitmap(IDB_OPBTNNON);
			m_pImageList->Add(&bitmap1, RGB(255,0,255));
			
			CBitmap bitmap2;
			bitmap2.LoadBitmap(IDB_OPBTNNONH);
			m_pImageList->Add(&bitmap2, RGB(255,0,255));
			
			CBitmap bitmap3;
			bitmap3.LoadBitmap(IDB_OPBTNSEL);
			m_pImageList->Add(&bitmap3, RGB(255,0,255));
			
			CBitmap bitmap4;
			bitmap4.LoadBitmap(IDB_OPBTNSELH);
			m_pImageList->Add(&bitmap4, RGB(255,0,255)); 
		}
		if (bPressed)
			m_pImageList->Draw(pDC, 2, rc.TopLeft(), ILD_TRANSPARENT);
		else
			m_pImageList->Draw(pDC, 0, rc.TopLeft(), ILD_TRANSPARENT);
				
	}
}

void CSpreadSheet::CheckLayer(CString &sLayer, int &nImage)
{
	if (sLayer.GetLength() > 0 && nImage > -1)
		return;

	AcDbLayerTable *pLayerTable;
    acdbHostApplicationServices()->workingDatabase()->getSymbolTable(pLayerTable, AcDb::kForRead);

    AcDbLayerTableIterator* pIterator = NULL;
    
	pLayerTable->newIterator(pIterator, true, true);

	//pIterator->step();

	AcDbLayerTableRecord *pLayerTableRecord = NULL;

	// open the entity for read
	if (pIterator->getRecord(pLayerTableRecord, AcDb::kForRead) != Acad::eOk)
		return;

	// get the layer name
	const TCHAR* pName;
	pLayerTableRecord->getName(pName);
	sLayer = pName;

	// get the layer color
	AcCmColor clr = pLayerTableRecord->color();
	nImage = clr.colorIndex();

	// of course we must close it
	pLayerTableRecord->close();
	
	delete pIterator;

    pLayerTable->close();
}

void CSpreadSheet::GetImageDropListItems(int nRow, int nCol, int &nImage, CString &sText)
{
	if (GetCellData(nRow, nCol) != NULL)
	{
		for (int i=0; i<GetCellData(nRow, nCol)->sStrings.GetSize(); i++)
		{
			if (_tcsicmp(sText, GetCellData(nRow, nCol)->sStrings[i]) == 0)
			{
				sText = GetCellData(nRow, nCol)->sStrings[i];
				if (i < GetCellData(nRow, nCol)->nInts.GetSize())
				{
					nImage = GetCellData(nRow, nCol)->nInts[i];
					return;
				}
			}
		}
		if (GetCellData(nRow, nCol)->nInts.GetSize() > 0)
			nImage = GetCellData(nRow, nCol)->nInts[0];
		sText = GetCellData(nRow, nCol)->sStrings[0];
	}
	else
	{
		RefCountedPtr< CPropertyObject > pProp = m_ArxControl->GetPropertyObject(nColumnListItems);	
		RefCountedPtr< CPropertyObject > pPropImages = m_ArxControl->GetPropertyObject(nColumnListImages);
		if (nCol < pProp->GetStringArrayListPtr()->size())
		{
			PropVal::TCStringArray& rStrings = pProp->GetStringArrayListPtr()->at(nCol);
			PropVal::TIntArray& rInts = pPropImages->GetIntArrayListPtr()->at(nCol);
			for (int i=0; i<rStrings.size(); i++)
			{
				if (_tcsicmp(sText, rStrings[i]) == 0)
				{
					sText = rStrings[i];
					if (i < rInts.size())
					{
						nImage = rInts[i];
						return;
					}
				}
			}
			nImage = rInts[0];
			sText = rStrings[0];
		}
	}
}

void CSpreadSheet::DrawColor(CDC* pDC, CRect rc, int &nColor, CString &sText)
{

	if (nColor == -1)
	{
		VdclColorComboBox *pComboBox = NULL;
		CComboBoxHolder *pHolder;
		
		// get or create the control.
		if (m_pComboBox[4] == NULL)
		{
			pComboBox = new VdclColorComboBox;
			pHolder = new CComboBoxHolder;
			
			pHolder->Create(CString(), WS_CHILD, CRect(1,1,10,10), this, 124);
			
			pHolder->pComboBox = pComboBox;
			pComboBox->Create(4, CRect(1,1,10,10), pHolder, 410);
			pComboBox->SetFont(GetFont());

			pHolder->m_nStyle = 4;
			pHolder->SetFont(GetFont());				
			m_pComboBox[4] = pHolder;
			
		}
		else
		{
			pHolder = (CComboBoxHolder*)m_pComboBox[4];
			pComboBox = (VdclColorComboBox*)pHolder->pComboBox;
		}

		int n = -1;
		if (sText.GetLength() == 0)
		{
			pComboBox->GetLBText(8, sText);
		}

		n = pComboBox->FindStringExact(0, sText);
		if (n == -1)
		{
			n = pComboBox->FindString(0, sText);
			if (n != -1)
			{					
				pComboBox->GetLBText(n, sText);
			}
		}
		if (n == -1)
		{	
			nColor = _ttoi(sText);

			n = pComboBox->FindItemByColorIndex(nColor);
			
			if (n == -101)
				sText = _ltoa(nColor);
			else
				pComboBox->GetLBText(n, sText);
		}
		else
		{
			nColor = pComboBox->GetItemColorIndex(n);
		}
	}
	
	// setup the CRect for FillRect	
	CPoint point;
	
	// move the color splotch down by one.
	rc.top++;
	rc.bottom++;

	HGDIOBJ pen = ::CreatePen(PS_SOLID, 1, RGB(0,0,0));
	HGDIOBJ OldPen = SelectObject(pDC->m_hDC, pen);
		
	::Rectangle(pDC->m_hDC, rc.left, rc.top, rc.right, rc.bottom);

	SelectObject(pDC->m_hDC, OldPen);			
	DeleteObject(pen);

	rc.DeflateRect(1,1);

	// draw the solid rectangle
	COLORREF rgb;
	
	if (nColor == 256 || nColor == 0)
	{
		rgb = RGB(255,255,255);
		::SetBkColor(pDC->m_hDC, rgb);
	}
	else
	{
		rgb = GetRGBColor(nColor);
		::SetBkColor(pDC->m_hDC, rgb);
	}

	::ExtTextOut(pDC->m_hDC, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);
	::SetBkColor(pDC->m_hDC, rgb);
}

void CSpreadSheet::DrawTrueColor(CDC* pDC, CRect rc, int &nColor, CString &sText)
{
	// setup the CRect for FillRect	
	CPoint point;
	
	// move the color splotch down by one.
	rc.top++;
	rc.bottom++;

	HGDIOBJ pen = ::CreatePen(PS_SOLID, 1, RGB(0,0,0));
	HGDIOBJ OldPen = SelectObject(pDC->m_hDC, pen);
		
	::Rectangle(pDC->m_hDC, rc.left, rc.top, rc.right, rc.bottom);

	SelectObject(pDC->m_hDC, OldPen);			
	DeleteObject(pen);

	rc.DeflateRect(1,1);

	// draw the solid rectangle
	COLORREF rgb;
	
	if (nColor == 256 || nColor == 0)
	{
		rgb = RGB(255,255,255);
		::SetBkColor(pDC->m_hDC, rgb);
	}
	else if (sText.GetLength() == 0)
	{
		rgb = RGB(255,255,255);
		::SetBkColor(pDC->m_hDC, rgb);
	}
	else if (nColor == -1)					
	{
		int nComma = sText.Find(sComma);
		int nComma2 = sText.Find(sComma, nComma+1);
		if (nComma > -1 && nComma2 > -1)
		{		
			CString sRed = sText.Left(nComma);
			CString sGreen = sText.Mid(nComma+1, nComma2-nComma);
			CString sBlue = sText.Mid(nComma2+1);
			
			long nRed = _ttol(sRed);
			long nGreen = _ttol(sGreen);
			long nBlue = _ttol(sBlue);
			
			rgb = RGB(nRed, nGreen, nBlue);	
		}
		else
		{
			rgb = RGB(255,255,255);
			::SetBkColor(pDC->m_hDC, rgb);
		}
	}
	else
	{
		rgb = GetRGBColor(nColor);
		::SetBkColor(pDC->m_hDC, rgb);
	}

	::SetBkColor(pDC->m_hDC, rgb);
	::ExtTextOut(pDC->m_hDC, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);
}

void CSpreadSheet::DrawLineWeights(CDC* pDC, CRect rc, AcDb::LineWeight LW)
{
	CPoint point;
	if (LW < AcDb::kLnWt030)
	{
		COLORREF rgb =RGB(0,0,0);
		HGDIOBJ pen = ::CreatePen(PS_SOLID, 1, rgb);
		HGDIOBJ OldPen = SelectObject(pDC->m_hDC, pen);
		
		MoveToEx(pDC->m_hDC, rc.left, rc.top + 6, &point);
		LineTo(pDC->m_hDC, rc.right, rc.top + 6);		
		
		SelectObject(pDC->m_hDC, OldPen);			
		DeleteObject(pen);
	}
	else
	{
		rc.right = rc.left + 12;

		switch (LW)
		{
			case AcDb::kLnWt030:
			case AcDb::kLnWt035:
			{
				rc.top +=5;
				rc.bottom = rc.top + 3;
				break;
			}
			case AcDb::kLnWt040:
			{
				rc.top +=5;
				rc.bottom = rc.top + 4;	
				break;
			}
			case AcDb::kLnWt050:
			case AcDb::kLnWt053:
			{
				rc.top +=4;
				rc.bottom = rc.top + 5;
				break;
			}
			case AcDb::kLnWt060:
			{
				rc.top +=4;
				rc.bottom = rc.top + 6;
				break;
			}
			case AcDb::kLnWt070:
			{
				rc.top +=3;
				rc.bottom = rc.top + 7;
				break;
			}
			case AcDb::kLnWt080:
			{
				rc.top +=3;
				rc.bottom = rc.top + 8;
				break;
			}
			case AcDb::kLnWt090:
			{
				rc.top +=3;
				rc.bottom = rc.top + 9;
				break;
			}
			case AcDb::kLnWt100:
			case AcDb::kLnWt106:
			{
				rc.top +=3;
				rc.bottom = rc.top + 10;
				break;
			}
			case AcDb::kLnWt120:
			{
				rc.top +=2;
				rc.bottom = rc.top + 12;
				break;
			}
			case AcDb::kLnWt140:
			case AcDb::kLnWt158:
			case AcDb::kLnWt200:
			case AcDb::kLnWt211:
			{
				rc.bottom = rc.top + 14;
				break;
			}
		}
		
		// draw the solid rectangle
		COLORREF rgb = RGB(0,0,0);
		rgb = ::SetBkColor(pDC->m_hDC, rgb);
		
		::ExtTextOut(pDC->m_hDC, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);
		::SetBkColor(pDC->m_hDC, rgb);
	}
}
void CSpreadSheet::DrawLineWeights(CDC* pDC, CRect rc, int nImage)
{
	CPoint point;
		
	if (nImage < 10)
	{
		COLORREF rgb =RGB(0,0,0);
		HGDIOBJ pen = ::CreatePen(PS_SOLID, 1, rgb);
		HGDIOBJ OldPen = SelectObject(pDC->m_hDC, pen);
		
		MoveToEx(pDC->m_hDC, rc.left, rc.top + 6, &point);
		LineTo(pDC->m_hDC, rc.right, rc.top + 6);		
		
		SelectObject(pDC->m_hDC, OldPen);			
		DeleteObject(pen);
	}
	else
	{
		rc.right = rc.left + 12;

		if (nImage == 10 || nImage == 11)
		{
			rc.top +=5;
			rc.bottom = rc.top + 3;
		}
		else if (nImage == 12)
		{
			rc.top +=5;
			rc.bottom = rc.top + 4;		
		}
		else if (nImage == 13)
		{
			rc.top +=4;
			rc.bottom = rc.top + 5;
		}
		else if (nImage == 14 || nImage == 15 || nImage == 16)
		{
			rc.top +=4;
			rc.bottom = rc.top + 6;
		}
		else if (nImage == 17)
		{
			rc.top +=3;
			rc.bottom = rc.top + 7;
		}
		else if (nImage == 18)
		{
			rc.top +=3;
			rc.bottom = rc.top + 8;
		}
		else if (nImage == 18 || nImage == 19)
		{
			rc.top +=3;
			rc.bottom = rc.top + nImage - 10;
		}
		else if (nImage == 20 || nImage == 21)
		{
			rc.top +=2;
			rc.bottom = rc.top + 10;
		}
		else
		{
			rc.bottom = rc.top + 12;
		}
		
		// draw the solid rectangle
		COLORREF rgb = RGB(0,0,0);
		rgb = ::SetBkColor(pDC->m_hDC, rgb);
		
		::ExtTextOut(pDC->m_hDC, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);
		::SetBkColor(pDC->m_hDC, rgb);
	}
}
void CSpreadSheet::DrawFontIcons(CDC* pDC, CRect rc, int &nImage, CString &sText)
{
	if (nImage == -1)
	{
		BeginWaitCursor();

		CFontCombo *pComboBox = NULL;
		CComboBoxHolder *pHolder;
		
		// get or create the control.
		if (m_pComboBox[9] == NULL)
		{
			pComboBox = new CFontCombo;
			pHolder = new CComboBoxHolder;

			pHolder->Create(CString(), WS_CHILD, CRect(1,1,10,10), this, 129);
			
			pHolder->pComboBox = pComboBox;
			pComboBox->Create(9, CRect(1,1,10,10), pHolder, 410);
			pComboBox->SetFont(GetFont());
			
			pHolder->m_nStyle = 9;
			pHolder->SetFont(GetFont());				
			m_pComboBox[9] = pHolder;			
		}
		else
		{
			pHolder = (CComboBoxHolder*)m_pComboBox[9];
			pComboBox = (CFontCombo*)pHolder->pComboBox;
		}

		int n = pComboBox->FindStringExact(0, sText);
		if (n == -1)
		{
			n = pComboBox->FindString(0, sText);
			if (n == -1)
			{	
				n = _ttoi(sText);
				pComboBox->GetLBText(n, sText);
			}
		}
		
		DWORD dwData = pComboBox->GetFontTypeId(sText);
	
		if (dwData == 4)
			nImage = 0;
		else if (dwData == 6)
			nImage = 1;

		EndWaitCursor();

		if (nImage == -1)
			return;
	}

	rc.top++;

	if (m_pFontImages == NULL)
	{
		m_pFontImages = new CImageList();
		m_pFontImages->Create(15,13,ILC_COLOR4 | ILC_MASK, 3, 1);
		
		HMODULE hRes = _hdllInstance;

		HICON hIcon = (HICON)::LoadImage(hRes, MAKEINTRESOURCE(IDI_TRUEFONT), IMAGE_ICON, 0, 0, 0);	
		m_pFontImages->Add(hIcon);
		DestroyIcon(hIcon);

		hIcon = (HICON)::LoadImage(hRes, MAKEINTRESOURCE(IDI_ACADFONT), IMAGE_ICON, 0, 0, 0);
		m_pFontImages->Add(hIcon);
		DestroyIcon(hIcon);
	}

	if (nImage > -1)
		m_pFontImages->Draw(pDC, nImage, rc.TopLeft(), ILD_TRANSPARENT);
}

	
    
	
	


void CSpreadSheet::DrawArrowHeads(CDC* pDC, CRect rc, int &nImage, CString &sText)
{
	if (nImage == -1)
	{
		VdclArrowHeadComboBox *pComboBox;
		CComboBoxHolder *pHolder;
		// get or create the control.
		if (m_pComboBox[3] == NULL)
		{
			pHolder = new CComboBoxHolder;
			pComboBox = new VdclArrowHeadComboBox;
			
			pHolder->Create(CString(), WS_CHILD, CRect(1,1,10,10), this, 123);
			
			pHolder->pComboBox = pComboBox;
			pComboBox->Create(3, CRect(1,1,10,10), pHolder, 410);
			pComboBox->SetFont(GetFont());

			pHolder->m_nStyle = 3;
			pHolder->SetFont(GetFont());				
			pHolder->ShowWindow(FALSE);
			m_pComboBox[3] = pHolder;			
		}
		else
		{
			pHolder = (CComboBoxHolder*)m_pComboBox[3];
			pComboBox = (VdclArrowHeadComboBox*)pHolder->pComboBox;				
		}
		int n = pComboBox->FindStringExact(0, sText);
		if (n == -1)
		{
			n = pComboBox->FindString(0, sText);
			if (n == -1)
			{	
				n = _ttoi(sText);
				pComboBox->GetLBText(n, sText);
			}
		}
		
		nImage = n;

		if (nImage == -1)
			return;
	}

	if (nImage == 20)
		return;

	rc.top++;
	rc.top++;

	if (m_pArrowImages == NULL)
	{
		m_pArrowImages = new CImageList();
		m_pArrowImages->Create(IDB_ARROWHEADS, 11, 1, CLR_NONE);
		m_pArrowImages->SetBkColor(RGB(255,255,255));
	}

	m_pArrowImages->Draw(pDC, nImage, rc.TopLeft(), ILD_NORMAL);
}

void CSpreadSheet::DrawCheckBox(CDC* pDC, CRect rc, bool bPressed, bool bSelected)
{
	HTHEME hTheme = m_pTheme->OpenThemeData(GetSafeHwnd(), sButton);
		
	if (hTheme)
	{
		if (bPressed)
			m_pTheme->DrawThemeBackground(hTheme, m_hWnd, pDC->GetSafeHdc(), 
				BP_CHECKBOX, 
				(bSelected ? RBS_CHECKEDNORMAL : RBS_CHECKEDNORMAL), &rc, NULL);
		else
			m_pTheme->DrawThemeBackground(hTheme, m_hWnd, pDC->GetSafeHdc(), 
				BP_CHECKBOX, 
				(bSelected ? RBS_UNCHECKEDNORMAL : RBS_UNCHECKEDNORMAL), &rc, NULL);

		m_pTheme->CloseThemeData(hTheme);
	}
	else
	{
		UINT unEdge = EDGE_SUNKEN;
		// draw the solid rectangle
		::DrawEdge(pDC->m_hDC, &rc, unEdge, BF_RECT);
	
		rc.DeflateRect(2,2);

		// draw the solid rectangle
		::SetBkColor(pDC->m_hDC, RGB(255,255,255));
		::ExtTextOut(pDC->m_hDC, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);

		rc.DeflateRect(-4,-4);

		if (bPressed)
		{
			CPoint point;		
			COLORREF rgb = RGB(0,0,0);
			HGDIOBJ pen = ::CreatePen(PS_SOLID, 1, rgb);
			HGDIOBJ OldPen = SelectObject(pDC->m_hDC, pen);
			
			MoveToEx(pDC->m_hDC, rc.left + 5, rc.top + 7, &point);
			LineTo(pDC->m_hDC, rc.left + 5, rc.top + 10);		
			
			MoveToEx(pDC->m_hDC, rc.left + 6, rc.top + 8, &point);
			LineTo(pDC->m_hDC, rc.left + 6, rc.top + 11);		
		
			MoveToEx(pDC->m_hDC, rc.left + 7, rc.top + 9, &point);
			LineTo(pDC->m_hDC, rc.left + 7, rc.top + 12);		
		
			MoveToEx(pDC->m_hDC, rc.left + 8, rc.top + 8, &point);
			LineTo(pDC->m_hDC, rc.left + 8, rc.top + 11);		
		
			MoveToEx(pDC->m_hDC, rc.left + 9, rc.top + 7, &point);
			LineTo(pDC->m_hDC, rc.left + 9, rc.top + 10);		
		
			MoveToEx(pDC->m_hDC, rc.left + 10, rc.top + 6, &point);
			LineTo(pDC->m_hDC, rc.left + 10, rc.top + 9);		
		
			MoveToEx(pDC->m_hDC, rc.left + 11, rc.top + 5, &point);
			LineTo(pDC->m_hDC, rc.left + 11, rc.top + 8);		
		
			SelectObject(pDC->m_hDC, OldPen);			
			DeleteObject(pen);
		}
	}
}

void CSpreadSheet::CellHitTest(CPoint point, int &nRow, int &nCol)
{
	CHeaderCtrl* pHeader = GetHeaderCtrl();
	nRow = -1;
	nCol = -1;

	int nHpos = GetScrollPos(SB_HORZ);
	
	point.x += nHpos;

	// do a loop to get the column index
  int i;
	for (i=0; i<pHeader->GetItemCount(); i++)
	{
		CRect rc;
		pHeader->GetItemRect(i, rc);
		
		if (point.x >= rc.left && point.x <= rc.right)
		{
			nCol = i;			
			break;
		}
	}

	for (i=0; i<GetItemCount(); i++)
	{
		CRect rc;
		GetItemRect(i, rc, LVIR_BOUNDS);
		if (rc.top <= point.y && rc.bottom >= point.y)
		{
			nRow = i;
			break;
		}
	}

}

void CSpreadSheet::OnKillfocus(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// call methods to invoke the event
	InvokeMethod(m_ArxControl->GetStrProperty(nEventKillFocus), m_bInvokeWithSendString);
	
	*pResult = 0;
}

CRect CSpreadSheet::GetCurSelRect()
{
	CRect rc;
	GetSubItemRect(m_nRowSelected, m_nColSelected, LVIR_BOUNDS, rc);
		
	if (m_nColSelected == -1)
	{
		CRect rcThis;
		GetWindowRect(&rcThis);

		rc.left = 0;
		rc.right = rcThis.Width();
	}
	return rc;
}


void CSpreadSheet::OnLButtonDown(UINT nFlags, CPoint point) 
{
	int nRow, nCol;
	
	InvokeMethodIntIntIntInt(
		m_ArxControl->GetStrProperty(nEventMouseDown),
		1,
		nFlags,
		point.x,
		point.y,
		m_bInvokeWithSendString);

	CellHitTest(point, nRow, nCol);

	if (m_nRowSelected != nRow || m_nColSelected != nCol)
		HideEditControls();

	if (nCol == 0 && m_bHasRowHeader)
	{
		nCol = -1;
	}

	CHeaderCtrl *pHeader = GetHeaderCtrl();

	if (pHeader != NULL)
		if (nCol > pHeader->GetItemCount())
			return;

	if (nRow == -1)
		return;
    
	InvokeMethodIntInt(
		m_ArxControl->GetStrProperty(nEventClicked),
		nRow,
		nCol,
		m_bInvokeWithSendString);


	m_bShowHighlight = true;

	if (nCol >= 0)
	{
		// test to see how to edit the cell each different kind of edit style
		// according to the column specification.
		if (GetCellStyle(nRow, nCol) == Grid_CheckBoxes ||
			GetCellStyle(nRow, nCol) == Grid_OptionButtons)
		{
			int nImage = GetItemImage(nRow, nCol);
			
			// if this column is an option button
			if (GetCellStyle(nRow, nCol) == Grid_OptionButtons)
			{
				if (nImage <= 0)
				{
					// reset all the other option buttons to unchecked.
					for (int i=0; i<GetItemCount(); i++)
					{
						if (GetCellStyle(i, nCol) == Grid_OptionButtons)
							SetItemImage(i, nCol, 0);
					}
					if (nImage != 1)
						SetItemImage(nRow, nCol, 1);
					else
						SetItemImage(nRow, nCol, 0);
				}
				else
					SetItemImage(nRow, nCol, 1);
			}
			else
			{			
				if (nImage != 1)
					SetItemImage(nRow, nCol, 1);
				else
					SetItemImage(nRow, nCol, 0);
			}
			
			// fire the on Grid edit cell event.
			EndEditControls(this);
		}
		

		// test to see how to edit the cell each different kind of edit style
		// according to the column specification.
		if (GetCellStyle(nRow, nCol) == Grid_SwitchableIcons)
		{
			int nImage = GetItemImage(nRow, nCol);
			int nDefImage;
			int nAltImage;
			
			if (GetCellData(nRow, nCol) == NULL)
			{
				nDefImage = m_pColDefault->GetIntArrayPtr()->at(nCol);	
				nAltImage = m_pColAlternate->GetIntArrayPtr()->at(nCol);	
			}
			else
			{
				nDefImage = GetCellData(nRow, nCol)->nData1;
				nAltImage = GetCellData(nRow, nCol)->nData2;
			}

			if (nImage == nDefImage || nImage == -1)
			{
				SetItemImage(nRow, nCol, nAltImage);
			}
			else
			{
				SetItemImage(nRow, nCol, nDefImage);
			}
			// fire the on Grid edit cell event.
			EndEditControls(this);
		}

		if (GetCellStyle(nRow, nCol) == Grid_EllipsesButtons ||
			GetCellStyle(nRow, nCol) == Grid_PickButtons)
		{
			int nOldRow = m_nRowSelected;
			int nOldCol = m_nColSelected;

			m_nColSelected = nCol;
			m_nRowSelected = nRow;
			
			RefreshCell(nOldRow, nOldCol);
			EditCellNow();
			return;
		}
		if (m_nRowSelected == nRow && m_nColSelected == nCol)
		{
			EditCellNow();
			return;
		}
	}

	
	CRect rcOld;
	bool bRefreshOldRect = false;
	if (m_nRowSelected >= 0)
	{
		rcOld = GetCurSelRect();
		bRefreshOldRect = true;
	}
	
	m_nRowSelected = nRow;
	m_nColSelected = nCol;

	ShowCurSel();

	if (bRefreshOldRect)
	{
		InvalidateRect(rcOld, TRUE);
	}
	
	
	if (nCol > 0)
	{
		if (GetCellStyle(nRow, nCol) != Grid_CheckBoxes &&
			GetCellStyle(nRow, nCol) != Grid_OptionButtons &&
			GetCellStyle(nRow, nCol) != Grid_SwitchableIcons)
		{
			CRect rc = GetCurSelRect();
			InvalidateRect(rc, TRUE);
		}
	}
	// if not in a cell exit here
	if (nRow == -1)
		return;

	// if no style property is set for this column
	// exit here.
	//if (m_pColStyles->m_intList.GetSize() < nCol)
	//	return;
}

void CSpreadSheet::OnLButtonUp(UINT nFlags, CPoint point) 
{
	InvokeMethodIntIntIntInt(
		m_ArxControl->GetStrProperty(nEventMouseUp),
		1,
		nFlags,
		point.x,
		point.y,
		m_bInvokeWithSendString);

}

void CSpreadSheet::OnMouseMove(UINT nFlags, CPoint point) 
{
	InvokeMethodIntIntInt(
		m_ArxControl->GetStrProperty(nEventMouseMove),
		nFlags,
		point.x,
		point.y,
		m_bInvokeWithSendString);
	
	CListCtrl::OnMouseMove(nFlags, point);
}

void CSpreadSheet::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
//	CListCtrl::OnLButtonDblClk(nFlags, point);
	int nRow, nCol;

	CellHitTest(point, nRow, nCol);

	InvokeMethodIntInt(
		m_ArxControl->GetStrProperty(nEventDblClicked),
		nRow,
		nCol,
		m_bInvokeWithSendString);

	if (nCol < 0)
		return;



	// test to see how to edit the cell each different kind of edit style
	// according to the column specification.
	if (GetCellStyle(nRow, nCol) == Grid_CheckBoxes ||
		GetCellStyle(nRow, nCol) == Grid_OptionButtons)
	{
		int nImage = GetItemImage(nRow, nCol);
		
		// if this column is an option button
		if (GetCellStyle(nRow, nCol) == Grid_OptionButtons)
		{
			if (nImage <= 0)
			{
				// reset all the other option buttons to unchecked.
				for (int i=0; i<GetItemCount(); i++)
				{
					SetItemImage(i, nCol, 0);
				}
				if (nImage != 1)
					SetItemImage(nRow, nCol, 1);
				else
					SetItemImage(nRow, nCol, 0);
			}
			else
				SetItemImage(nRow, nCol, 1);
		}
		else
		{			
			if (nImage != 1)
				SetItemImage(nRow, nCol, 1);
			else
				SetItemImage(nRow, nCol, 0);
		}
	}

	// test to see how to edit the cell each different kind of edit style
	// according to the column specification.
	if (GetCellStyle(nRow, nCol) == Grid_SwitchableIcons)
	{
		int nImage = GetItemImage(nRow, nCol);
		int nDefImage;
		int nAltImage;
		
		if (GetCellData(nRow, nCol) == NULL)
		{
			nDefImage = m_pColDefault->GetIntArrayPtr()->at(nCol);	
			nAltImage = m_pColAlternate->GetIntArrayPtr()->at(nCol);	
		}
		else
		{
			nDefImage = GetCellData(nRow, nCol)->nData1;
			nAltImage = GetCellData(nRow, nCol)->nData2;
		}

		if (nImage == nDefImage || nImage == -1)
		{
			SetItemImage(nRow, nCol, nAltImage);
		}
		else
		{
			SetItemImage(nRow, nCol, nDefImage);
		}
	}

	//
	if (m_bHasRowHeader && nCol == 0)
		return;

	ShowCurSel();
	EditCellNow();
	
}

int GetCurrentLayerColor() 
{
	CString sLayerName;
	Acad::ErrorStatus es;
	AcDbEntity *pEnt;
	AcDbObjectId objId = acdbHostApplicationServices()->workingDatabase()->clayer();

	if (Acad::eOk != (es = acdbOpenAcDbEntity(pEnt, objId, AcDb::kForRead)))
	{
		return 0;
	}
	
	AcDbLayerTableRecord *pLayerTableRecord = (AcDbLayerTableRecord*)pEnt;

	if (pLayerTableRecord == NULL)
	{
		return 0;
	}
	
	AcCmColor clr = pLayerTableRecord->color();
	
	int nColor = clr.colorIndex();

	// of course we must close it
	pLayerTableRecord->close();

	return nColor;
}
/*
setq col(jb:GetTrueColor))
(defun jb:GetTrueColor (/ ce ret)
  (setq ce (getvar "cecolor"))
  (initdia)
  (command "_.color")
  (while (= (logand (getvar "CMDACTIVE") 8) 1)
    (command pause))
  (if (/= (getvar "cecolor") ce)
    (setq ret (getvar "cecolor")))
  (if ce
    (setvar "cecolor" ce))
  ret
  )
*/


bool CSpreadSheet::acad_truecolordlg(COLORREF color, int curColor, int nIndex = -1, CString sPantone = CString())
{
	struct resbuf *arglist, *rslt=NULL; 
	int rc; 

	if (nIndex == -1)
	{
		arglist = acutBuildList(
					RTSTR, sAcad_TrueColorDlg, 
					420, (long)color, 
					RTT, 
					RTSHORT, curColor, 
					0); 
	}	
	else if (sPantone.GetLength() > 0)
	{
		arglist = acutBuildList(
					RTSTR, sAcad_TrueColorDlg, 
					430, sPantone, 
					RTT, 
					RTSHORT, curColor, 
					0); 
	}
	else // uses index value not true value
	{
		arglist = acutBuildList(
					RTSTR, sAcad_TrueColorDlg, 
					RTSHORT, nIndex, 
					RTT, 
					RTSHORT, curColor, 
					0); 
	}

	if (arglist == NULL) 
		return false;

	rc = acedInvoke(arglist, &rslt); 

	acutRelRb(arglist);

	if (rc != RTNORM) 
	{ 
		return false;
	} 
	else if (rslt == NULL) 
	{ 
		return false;
	} 
	
	struct resbuf *index, *trueColor, *colorBook;
	/* Save the user-selected color */ 
	index = rslt;
	
	trueColor = index->rbnext;
	if (trueColor != NULL)
	{
		colorBook = trueColor->rbnext;

		if (colorBook != NULL)
		{
			if (colorBook->restype == 430)
			{
				CString sColorBook = colorBook->resval.rstring;

				SetItemImage(m_nRowSelected, m_nColSelected, 300);
				SetItemText(m_nRowSelected, m_nColSelected, sColorBook);
				return true;
			}
		}

		if (trueColor->restype == 420)
		{
			COLORREF clr = trueColor->resval.rlong;
			CString sColor = _ltoa(GetBValue(clr)) + sComma +
							 _ltoa(GetGValue(clr)) + sComma +
							 _ltoa(GetRValue(clr));
			
			SetItemImage(m_nRowSelected, m_nColSelected, -1);
			SetItemText(m_nRowSelected, m_nColSelected, sColor);
		}
	}
	else
	{
		SetItemImage(m_nRowSelected, m_nColSelected, index->resval.rint);
		SetItemText(m_nRowSelected, m_nColSelected, _ltoa(index->resval.rint));
	}

	acutRelRb(rslt);

	return true;
}

 
int CSpreadSheet::GetCurCellStyle()
{
	return GetCellStyle(m_nRowSelected, m_nColSelected);
}

int CSpreadSheet::GetCellStyle(int nRow, int nCol)
{
	_RowData *pCells = GetRowData(nRow);
	
	if (pCells != NULL)
	{
		if (nCol < pCells->m_Cells.GetCount())
		{
			if (pCells->m_Cells.GetAt(nCol) != NULL)
			{
				if (pCells->m_Cells.GetAt(nCol)->nStyle > 0)
					return pCells->m_Cells.GetAt(nCol)->nStyle;
			}
		}
	}

	// now if the overriding cell style was not set,
	if (nCol < m_pColStyles->GetIntArrayPtr()->size())
		return m_pColStyles->GetIntArrayPtr()->at(nCol);

	return 0;
}


_Style * CSpreadSheet::GetCellData(int nRow, int nCol)
{
	// first we need to check the overriding cell styles
	_RowData *pCells = GetRowData(nRow);
	
	if (pCells != NULL)
	{
		if (nCol < pCells->m_Cells.GetCount())
		{
			if (pCells->m_Cells.GetAt(nCol) == NULL)
				return NULL;

			if (pCells->m_Cells.GetAt(nCol)->nStyle > 0)
				return pCells->m_Cells.GetAt(nCol);
		}
	}
	
	return NULL;
}


void CSpreadSheet::SetCellStyle(int nRow, int nCol, int nStyle)
{
	_RowData *pCells = GetRowData(nRow);
	
	// if not set and style is 0 
	if (pCells == NULL && nStyle == 0)
	{
		// exit here
		return;
	}

	// if not set yet and style is not 0
	if (pCells == NULL && nStyle != 0)
	{
		// set the row now
		pCells = new _RowData;
		SetItemData(nRow, (long)pCells);
	}

	int nCount = pCells->m_Cells.GetCount();

	if (nCol >= nCount)
	{	
		//int n = pCells->Add(newCellData);
		pCells->m_Cells.SetSize(nCol+1);
	}

	if (nCol < pCells->m_Cells.GetCount())
	{
		if (pCells->m_Cells[nCol] == NULL)
			pCells->m_Cells[nCol] = new _Style;

		pCells->m_Cells.GetAt(nCol)->nStyle = nStyle;			
	}
}
void CSpreadSheet::EditCellNow(UINT nChar/* = 0 */) 
{
	// call methods to invoke the event
	InvokeMethodIntInt(		
		m_ArxControl->GetStrProperty(nEventBeginLabelEdit),
		m_nRowSelected, m_nColSelected,
		m_bInvokeWithSendString);

	if (m_nColSelected >= m_pColStyles->GetIntArrayPtr()->size())
		return;	

	switch (GetCurCellStyle())
	{
		case Grid_CheckBoxes:
		case Grid_OptionButtons:
		case Grid_SwitchableIcons:
			return;
			break;
		case Grid_DirectoryCell:
		{
			ShowEllipsesButton(m_nRowSelected,	m_nColSelected, 2);
			return;
			break;
		}
		case Grid_DwgFilesCell:
		{
			ShowEllipsesButton(m_nRowSelected,	m_nColSelected, 3);
			return;
			break;
		}
		case Grid_EllipsesButtons:
		{
			ShowEllipsesButton(m_nRowSelected,	m_nColSelected, 0);
			return;
			break;
		}
		case Grid_PickButtons:
		{
			ShowEllipsesButton(m_nRowSelected,	m_nColSelected, 1);
			return;
			break;
		}
	}
	RefreshCell(m_nRowSelected, m_nColSelected);

	m_bShowHighlight = false;

	DoEditCellNow(GetCurCellStyle());

	
}

void CSpreadSheet::DoEditCellNow(int nStyle, UINT nChar/* = 0 */) 
{
	
	switch (nStyle)
	{		
	case Grid_AngleUnits_Combo:
		{
			ShowTextBox(m_nRowSelected,	m_nColSelected, nStyle, nChar);
			
			RefCountedPtr< CPropertyObject > pProp = m_ArxControl->GetPropertyObject(nColumnListItems);	
			
			if (GetCellData(m_nRowSelected,	m_nColSelected) != NULL)
				ShowComboBox(m_nRowSelected, m_nColSelected, 2, GetCellData(m_nRowSelected, m_nColSelected)->sStrings);
			else if (m_nColSelected < pProp->GetStringArrayListPtr()->size())
					ShowComboBox(m_nRowSelected, m_nColSelected, 2, PropVal::ConvertedCStringArray(pProp->GetStringArrayListPtr()->at(m_nColSelected)));
			VdclAngleEdit *pText = (VdclAngleEdit *)m_pTextBox[nStyle-36];
				
			CComboBoxHolder *pHolder = (CComboBoxHolder*)m_pComboBox[2];
			VdclComboBox *pComboBox = (VdclComboBox*)pHolder->pComboBox;
			
			pText->m_pGridDropList = pComboBox;
			pComboBox->m_pGridEdit = pText;
			pText->SetFocus();			
			return;
			break;
		}
	case Grid_Units_Combo:
		{
			ShowTextBox(m_nRowSelected,	m_nColSelected, nStyle, nChar);
			
			RefCountedPtr< CPropertyObject > pProp = m_ArxControl->GetPropertyObject(nColumnListItems);	

			if (GetCellData(m_nRowSelected,	m_nColSelected) != NULL)
				ShowComboBox(m_nRowSelected, m_nColSelected, 2, GetCellData(m_nRowSelected, m_nColSelected)->sStrings);
			else if (m_nColSelected < pProp->GetStringArrayListPtr()->size())
				ShowComboBox(m_nRowSelected, m_nColSelected, 2, PropVal::ConvertedCStringArray(pProp->GetStringArrayListPtr()->at(m_nColSelected)));

			VdclNumericEdit *pText = (VdclNumericEdit *)m_pTextBox[nStyle-36];
			
			CComboBoxHolder *pHolder = (CComboBoxHolder*)m_pComboBox[2];
			VdclComboBox *pComboBox = (VdclComboBox*)pHolder->pComboBox;
			
			pText->m_pGridDropList = pComboBox;
			pComboBox->m_pGridEdit = pText;
			pText->SetFocus();
			return;	
			break;
		}
	case Grid_Strings_Combo:
	case Grid_Integers_Combo:
		{
			ShowTextBox(m_nRowSelected,	m_nColSelected, nStyle, nChar);
			
			RefCountedPtr< CPropertyObject > pProp = m_ArxControl->GetPropertyObject(nColumnListItems);	

			if (GetCellData(m_nRowSelected,	m_nColSelected) != NULL)
				ShowComboBox(m_nRowSelected, m_nColSelected, 2, GetCellData(m_nRowSelected, m_nColSelected)->sStrings);
			else if (m_nColSelected < pProp->GetStringArrayListPtr()->size())
				ShowComboBox(m_nRowSelected, m_nColSelected, 2, PropVal::ConvertedCStringArray(pProp->GetStringArrayListPtr()->at(m_nColSelected)));
			
			OdclEdit *pText = (OdclEdit *)m_pTextBox[nStyle-36];
			
			CComboBoxHolder *pHolder = (CComboBoxHolder*)m_pComboBox[2];
			if (pHolder != NULL)
			{
				VdclComboBox *pComboBox = (VdclComboBox*)pHolder->pComboBox;					
				pText->m_pGridDropList = pComboBox;
				pComboBox->m_pGridEdit = pText;
				pText->SetFocus();
			}			
			return;
			break;
		}
	case Grid_UpperCase_Combo:
	case Grid_LowerCase_Combo:
		{
			ShowTextBox(m_nRowSelected,	m_nColSelected, nStyle, nChar);
			
			RefCountedPtr< CPropertyObject > pProp = m_ArxControl->GetPropertyObject(nColumnListItems);	

			if (GetCellData(m_nRowSelected,	m_nColSelected) != NULL)
				ShowComboBox(m_nRowSelected, m_nColSelected, 2, GetCellData(m_nRowSelected, m_nColSelected)->sStrings);
			else if (m_nColSelected < pProp->GetStringArrayListPtr()->size())
				ShowComboBox(m_nRowSelected, m_nColSelected, 2, PropVal::ConvertedCStringArray(pProp->GetStringArrayListPtr()->at(m_nColSelected)));
			
			OdclEdit *pText = NULL;
			switch(nStyle)
			{
			case Grid_UpperCase_Combo:
				nStyle = 5;
				break;
			case Grid_LowerCase_Combo:
				nStyle = 6;
				break;
			}
			pText = (OdclEdit *)m_pTextBox[nStyle];
			
			CComboBoxHolder *pHolder = (CComboBoxHolder*)m_pComboBox[2];
			VdclComboBox *pComboBox = (VdclComboBox*)pHolder->pComboBox;
			
			pText->m_pGridDropList = pComboBox;
			pComboBox->m_pGridEdit = pText;
			return;
			break;
		}
	case Grid_Strings:
			ShowTextBox(m_nRowSelected,	m_nColSelected, 0, nChar);
			break;
	case Grid_AngleUnits:
			ShowTextBox(m_nRowSelected,	m_nColSelected, 1, nChar);
			break;
	case Grid_Integers:
			ShowTextBox(m_nRowSelected,	m_nColSelected, 2, nChar);
			break;
	case Grid_Units:
			ShowTextBox(m_nRowSelected,	m_nColSelected, 3, nChar);
			break;
	case Grid_UpperCase:
			ShowTextBox(m_nRowSelected,	m_nColSelected, 5, nChar);
			break;
	case Grid_LowerCase:
			ShowTextBox(m_nRowSelected,	m_nColSelected, 6, nChar);
			break;
	case Grid_Password:
			ShowTextBox(m_nRowSelected,	m_nColSelected, 7, nChar);
			break;
	case Grid_MultiLine:
			ShowTextBox(m_nRowSelected,	m_nColSelected, 8, nChar);
			break;
	case Grid_Currency:
			ShowTextBox(m_nRowSelected,	m_nColSelected, 9, nChar);
			break;
	case Grid_Date:
			ShowTextBox(m_nRowSelected,	m_nColSelected, 10, nChar);
			break;
	case Grid_Time:
			ShowTextBox(m_nRowSelected,	m_nColSelected, 11, nChar);
			break;
	case Grid_Percentage:
			ShowTextBox(m_nRowSelected,	m_nColSelected, 12, nChar);
			break;			
	case Grid_DropDown:
		{
			RefCountedPtr< CPropertyObject > pProp = m_ArxControl->GetPropertyObject(nColumnListItems);	
			if (GetCellData(m_nRowSelected,	m_nColSelected) != NULL)
				ShowComboBox(m_nRowSelected, m_nColSelected, 2, GetCellData(m_nRowSelected, m_nColSelected)->sStrings);
			else if (m_nColSelected < pProp->GetStringArrayListPtr()->size())
				ShowComboBox(m_nRowSelected, m_nColSelected, 2, PropVal::ConvertedCStringArray(pProp->GetStringArrayListPtr()->at(m_nColSelected)));
		break;
		}
	case Grid_ImageDropList:
		{
			if (GetCellData(m_nRowSelected, m_nColSelected) != NULL &&
				GetCellData(m_nRowSelected, m_nColSelected)->sStrings.GetSize() > 0)
			{
				ShowImageComboBox(m_nRowSelected, 
						m_nColSelected,
						GetCellData(m_nRowSelected, m_nColSelected)->sStrings,
						GetCellData(m_nRowSelected, m_nColSelected)->nInts);
			}
			else
			{
				RefCountedPtr< CPropertyObject > pProp = m_ArxControl->GetPropertyObject(nColumnListItems);	
				RefCountedPtr< CPropertyObject > pPropImages = m_ArxControl->GetPropertyObject(nColumnListImages);
				if (m_nColSelected < pProp->GetStringArrayListPtr()->size())
				{
					PropVal::TIntArray& rInts = pPropImages->GetIntArrayListPtr()->at(m_nColSelected);
					CArray< int, int > rInt;
					for(int idx = 0; idx < rInts.size(); idx++)
						rInt.Add(rInts[idx]);
					ShowImageComboBox(m_nRowSelected, 
							m_nColSelected,
							PropVal::ConvertedCStringArray(pProp->GetStringArrayListPtr()->at(m_nColSelected)),
							rInt);
				}
			}
		break;
		}
	case Grid_ArrowHead:
	case Grid_AcadColors:
	case Grid_TextStyleList:
	case Grid_PlotStyleNames:
	case Grid_PlotStyleTables:
	case Grid_PlotterList:
	case Grid_Fonts:
	case Grid_DriveList:
	case Grid_LayerList:
	case Grid_DimStyleList:
		{
			CStringArray sStrings;
			ShowComboBox(m_nRowSelected, m_nColSelected, nStyle-16, sStrings);
		break;
		}	
		
	case Grid_AcadColorCell:
		{
			int nCurrentColor = GetItemImage(m_nRowSelected, m_nColSelected);
			if (acedSetColorDialog(
					nCurrentColor,
					TRUE,
					GetCurrentLayerColor()) == TRUE)
			{
				AcCmColor retColor;
				CString sDesc;

				retColor.setColorIndex(nCurrentColor);
				sDesc = retColor.colorNameForDisplay();
				SetItemText(m_nRowSelected, m_nColSelected, sDesc);
				SetItemImage(m_nRowSelected, m_nColSelected, nCurrentColor);
			}
			// fire the on Grid edit cell event.
			EndEditControls(this);
		break;
		}
	case Grid_TrueColorCell:
		{
				COLORREF clr = 0;
			int nImage = GetItemImage(m_nRowSelected, m_nColSelected);
		
			CString sLabel = GetItemText(m_nRowSelected, m_nColSelected);
			int nComma = sLabel.Find(sComma);
			int nComma2 = sLabel.Find(sComma, nComma+1);
			if (nComma > -1 && nComma2 > -1)
			{
				CString sRed = sLabel.Left(nComma);
				CString sGreen = sLabel.Mid(nComma+1, nComma2-nComma);
				CString sBlue = sLabel.Mid(nComma2+1);
				
				int nRed = _ttol(sRed);
				int nGreen = _ttol(sGreen);
				int nBlue = _ttol(sBlue);
				
				clr = RGB(nRed, nGreen, nBlue);									
			}
			else
			{
				clr = RGB(255, 255, 255);									
			}
			CColorDialog dlg(clr, CC_FULLOPEN, this);
			if (dlg.DoModal() == IDOK)
			{
			   COLORREF color = dlg.GetColor();
			   
			   CString sColor = _ltoa(GetRValue(color)) + sComma +
							 _ltoa(GetGValue(color)) + sComma +
							 _ltoa(GetBValue(color));
			
				SetItemImage(m_nRowSelected, m_nColSelected, -1);
				SetItemText(m_nRowSelected, m_nColSelected, sColor);
			}
			// fire the on Grid edit cell event.
			EndEditControls(this);
		break;
		}
		
	case Grid_LineWeightCell:
		{
			AcDb::LineWeight newLW = (AcDb::LineWeight)GetItemImage(m_nRowSelected, m_nColSelected);
			SelectLineWeightUI(newLW, true, this);
			SetItemImage(m_nRowSelected, m_nColSelected, (int)newLW);
			
			// fire the on Grid edit cell event.
			EndEditControls(this);
			break;
		}
	case Grid_LinetypeCell:
		{
			CString sLinetype;
			AcDbObjectId idLinetype;
			SelectLinetypeUI(idLinetype, sLinetype, true, this);
			SetItemText(m_nRowSelected, m_nColSelected, sLinetype);

			// fire the on Grid edit cell event.
			EndEditControls(this);
			break;
		}
	case Grid_DirectoryCell:
		{			
			CString sTitle;
			sTitle = theWorkspace.LoadResourceString(IDS_SELFOLDER);
			
			CDirDialog dlg;
			//dlg.m_strTitle = sTitle;
			dlg.m_strSelDir = GetItemText(m_nRowSelected, m_nColSelected);
			if (dlg.DoBrowse(this) == TRUE)
			{
				SetItemText(m_nRowSelected, m_nColSelected, dlg.m_strPath);
				SetItemImage(m_nRowSelected, m_nColSelected, -1);
			}
			// fire the on Grid edit cell event.
			EndEditControls(this);
			break;
		}
	case Grid_DwgFilesCell:
		{
			struct resbuf * result;
			CString sFile = GetItemText(m_nRowSelected, m_nColSelected);
			result = acutNewRb(RTSTR);

			if (acedGetFileD(sFile, sFile, sDwg, 8, result) == RTNORM)
			{
				SetItemText(m_nRowSelected, m_nColSelected, result->resval.rstring);
				SetItemImage(m_nRowSelected, m_nColSelected, -1);
			}
			acutRelRb(result);
			
			// fire the on Grid edit cell event.
			EndEditControls(this);
			
			return;
			break;
		}
	}	

	if (nStyle == 0)
	{
		m_bShowHighlight = true;
		ShowCurSel();
	}
}

const char sDwgExt[] = "dwg;dxf";

void CSpreadSheet::DoFileDlg(int nStyle) 
{
	switch (nStyle)
	{
		case 2:
		{			
			/*
			CString sTitle;
			sTitle = theWorkspace.LoadResourceString(IDS_SELFOLDER);
			
			CDirDialog dlg;
			dlg.m_strSelDir = GetItemText(m_nRowSelected, m_nColSelected);
			if (dlg.DoBrowse(this) == TRUE)
			{
				SetItemText(m_nRowSelected, m_nColSelected, dlg.m_strPath);
				SetItemImage(m_nRowSelected, m_nColSelected, -1);
			}
			*/
			CString sTitle;
			sTitle = theWorkspace.LoadResourceString(IDS_SELFOLDER);
			
			CDirDialog dlg;
			//dlg.m_strTitle = sTitle;
			dlg.m_strSelDir = GetItemText(m_nRowSelected, m_nColSelected);
			if (dlg.DoBrowse(this) == TRUE)
			{
				SetItemText(m_nRowSelected, m_nColSelected, dlg.m_strPath);
				SetItemImage(m_nRowSelected, m_nColSelected, -1);
			}

			// fire the on Grid edit cell event.
			EndEditControls(this);

			break;
		}
		case 3:
		{
			struct resbuf * result;
			CString sFileExt = sDwgExt;
			CString sFile = GetItemText(m_nRowSelected, m_nColSelected);
			result = acutNewRb(RTSTR);
		
			if (GetCellData(m_nRowSelected, m_nColSelected) != NULL &&
				GetCellData(m_nRowSelected, m_nColSelected)->sStrings.GetSize() > 0)
			{
				sFileExt = GetCellData(m_nRowSelected, m_nColSelected)->sStrings[0];
			}
			else
			{			
				RefCountedPtr< CPropertyObject > pProp = m_ArxControl->GetPropertyObject(nColumnListItems);	
				if (m_nColSelected < pProp->GetStringArrayListPtr()->size())
					sFileExt = pProp->GetStringArrayListPtr()->at(m_nColSelected)[0];
			}
			if (acedGetFileD(sFile, sFile, sFileExt, 8, result) == RTNORM)
			{
				SetItemText(m_nRowSelected, m_nColSelected, result->resval.rstring);
				SetItemImage(m_nRowSelected, m_nColSelected, -1);
			}
			acutRelRb(result);
			
			// fire the on Grid edit cell event.
			EndEditControls(this);

			break;
		}
	}
}

void CSpreadSheet::SetCurSel(int nRow, int nCol) 
{
	m_bShowHighlight = true;
	bool bRefreshOldRect = false;
	CRect rcOld;
			
	if (m_nRowSelected != nRow ||
		m_nColSelected != nCol)
	{
		if (m_nRowSelected != -1)
		{
			rcOld = GetCurSelRect();
			bRefreshOldRect = true;
		}
	}
	m_nRowSelected = nRow;
	m_nColSelected = nCol;

	if (bRefreshOldRect)
	{
		InvalidateRect(rcOld, TRUE);
	}
	
	CRect rc = GetCurSelRect();
	InvalidateRect(rc, TRUE);
	
	ShowCurSel();
}
void CSpreadSheet::RefreshCell(int nRow, int nCol) 
{
	CRect rc = GetCurSelRect();
	InvalidateRect(rc, TRUE);
}

void CSpreadSheet::ShowCurSel() 
{
	InvokeMethodIntInt(m_ArxControl->GetStrProperty(nEventSelChanged), m_nRowSelected, m_nColSelected, m_bInvokeWithSendString);
	
	CRect rc = GetCurSelRect();
	InvalidateRect(rc, TRUE);
	
}

void CSpreadSheet::MoveUp() 
{
	HideEditControls();
	m_bShowHighlight = true;
	
	if (m_nRowSelected > 0)
	{
		int nRow = m_nRowSelected;
		m_nRowSelected--;
		RefreshCell(nRow, m_nColSelected);
		ShowCurSel();
	}
}

void CSpreadSheet::MoveDown() 
{
	HideEditControls();
	m_bShowHighlight = true;
	
	if (m_nRowSelected < GetItemCount()-1)
	{
		int nRow = m_nRowSelected;
		m_nRowSelected++;
		RefreshCell(nRow, m_nColSelected);
		ShowCurSel();
	}
}


void CSpreadSheet::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	char sChar = nChar;
	if (m_ArxControl)		
		InvokeMethodStringIntInt(m_ArxControl->GetStrProperty(nEventKeyUp), CString() + sChar,  (int)nRepCnt,  (int)nFlags, m_bInvokeWithSendString);
	
	CListCtrl::OnKeyUp(nChar, nRepCnt, nFlags);

}

void CSpreadSheet::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{

	if (m_ArxControl)
	{
		char sChar = nChar;
		InvokeMethodStringIntInt(m_ArxControl->GetStrProperty(nEventKeyDown), CString() + sChar,  (int)nRepCnt, (int)nFlags, m_bInvokeWithSendString);
	}	
	
	m_bShowHighlight = true;

	switch (nChar) 
	{
		case VK_ESCAPE: 
			break;
		case VK_RETURN: 
			EditCellNow();
			return;
			break;			
		case VK_UP: 
		{
			MoveUp();
			return;
			break;
		}
		case VK_DOWN: 
		{
			MoveDown();
			return;
			break;
		}
		case VK_LEFT: 
		{
			
			if (m_nColSelected > (BOOL)m_bHasRowHeader)
			{
				int nCol = m_nColSelected;
				m_nColSelected--;
				RefreshCell(m_nRowSelected, nCol);
				ShowCurSel();
				return;
			}
			break;
		}
		case VK_RIGHT: 
		{
			CHeaderCtrl* pHdrCtrl = GetHeaderCtrl();

			if (m_nColSelected < pHdrCtrl->GetItemCount()-1)
			{
				int nCol = m_nColSelected;
				m_nColSelected++;
				RefreshCell(m_nRowSelected, nCol);
				ShowCurSel();
				return;
			}
			break;
		}
	}
	CListCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
	
}

void CSpreadSheet::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	m_bShowHighlight = true;
	switch (nChar) 
	{
		case VK_ESCAPE: 
			break;
		case VK_RETURN: 
			EditCellNow();
			return;
			break;	
		case VK_BACK: 
			EditCellNow();
			return;
			break;		
		default:
			EditCellNow(nChar);
			return;
			break;		
	}
	CListCtrl::OnChar(nChar, nRepCnt, nFlags);
	
}

BOOL CSpreadSheet::PreTranslateMessage(MSG* pMsg) 
{
	InitToolTip();
	m_ToolTip.RelayEvent(pMsg);
	
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		// get the Multiline control.
		if (m_pTextBox[8] != NULL)
		{		
			OdclEdit *pTextBox = (OdclEdit*)m_pTextBox[8];
			int nS, nE;
			CString sText;

			pTextBox->GetSel(nS, nE);
			pTextBox->GetWindowText(sText);

			sText = sText.Mid(nS, nE) + sReturn;
			
			pTextBox->ReplaceSel(sText, TRUE);
			return TRUE;
		}
		pMsg->wParam = NULL;
		pMsg->message = NULL;
		EditCellNow();
	}

	return CListCtrl::PreTranslateMessage(pMsg);
}


void CSpreadSheet::ShowTextBox(int nRow, int nCol, int nStyle, UINT nChar/* = 0 */)
{
	int nCount = GetItemCount();
	
	if (nRow > -1 && nCol > -1)
	{
		m_nRowSelected = nRow;
		m_nColSelected = nCol;
	}
	else
	{
		m_nRowSelected = -1;
		m_nColSelected = -1;
	}
	
	COLORREF backGround = GetBkColor();
	
	if (!m_bOrientationVer)
	{
		if ((m_nRowSelected % 2) != 0)
			backGround = alertnateColor; 
	}
	else
	{
		int n = 0;
		if (m_bHasRowHeader)
			n = 1;
		if (((m_nColSelected+n) % 2) != 0)
			backGround = alertnateColor;
	}

	CRect rc;
	if (nCol == 0)
		GetItemRect(nRow, rc, LVIR_LABEL);
	else
		GetSubItemRect(nRow, nCol, LVIR_LABEL, rc);
	
	rc.right = rc.left + GetColumnWidth(nCol)-(4/2)-1;
	rc.top++;

	switch (nStyle)
	{
	case 8:
		{
		CRect rcClient;
		GetClientRect(&rcClient);
		int nCalc = (m_nRowHeight * 5);
		rc.bottom = rc.top + nCalc;
		if (rc.bottom > rcClient.bottom)
			rc.bottom = rcClient.bottom-1;
		rc.left -= 2;
		rc.top -= 2;
		break;
		}
	case Grid_Strings_Combo:
	case Grid_AngleUnits_Combo:
	case Grid_Integers_Combo:
	case Grid_Units_Combo:
		rc.right = rc.left + GetColumnWidth(nCol)-(4/2)-16;
		nStyle -= 36;
		break;
	case Grid_UpperCase_Combo:
		rc.right = rc.left + GetColumnWidth(nCol)-(4/2)-16;
		nStyle = 5;
		break;
	case Grid_LowerCase_Combo:
		rc.right = rc.left + GetColumnWidth(nCol)-(4/2)-16;
		nStyle = 6;
		break;
	}
	
	if (!m_bHasRowHeader || nCol > 0)
	{
		int nImage = GetItemImage(nRow, nCol);

		if (nImage > -1)
		{
			IMAGEINFO Info;

			CImageList *pImage1 = GetImageList(LVSIL_NORMAL);
			CImageList *pImage2 = GetImageList(LVSIL_SMALL);
		
			if (pImage1 != NULL)
				pImage1->GetImageInfo(0, &Info);
			if (pImage2 != NULL)
				pImage2->GetImageInfo(0, &Info);

			rc.left += Info.rcImage.right - Info.rcImage.left + 4;
		}
	}

	rc.bottom--;
	rc.left++;
	if (GetItemImage(nRow, nCol) > 0)
		rc.left += 1;
	else if (nStyle != 10)
		rc.left += 3;
	
	if (nStyle == 10)
		rc.bottom+=1;


	//rc.left++;
	//rc.left++;

	CString sCellText;
	sCellText = GetItemText(nRow, nCol);
	
	
	switch (nStyle)
	{
	case 1:
		{
			VdclAngleEdit *pTextBox;

			// get or create the control.
			if (m_pTextBox[nStyle] == NULL)
			{
				pTextBox = new VdclAngleEdit;
				pTextBox->Create(nStyle, rc, this, 110+nStyle);
			}
			else
			{
				pTextBox = (VdclAngleEdit*)m_pTextBox[nStyle];
				pTextBox->MoveWindow(rc, TRUE);
			}
			pTextBox->m_pGridDropList = NULL;
			// setup the control
			pTextBox->m_strOldValue = sCellText;
			pTextBox->SetWindowText(sCellText);		
			pTextBox->SetFont(GetFont());
			pTextBox->SetBkColor(backGround);
			if (nChar > 0)
			{
				if (pTextBox->m_sFilter.FindOneOf(CString(char(nChar))) > -1)
					pTextBox->SetWindowText(CString(char(nChar)));
				pTextBox->GetWindowText(sCellText);
			}
			//pTextBox->EnableWindow(TRUE);
			pTextBox->ShowWindow(TRUE);

			if (nChar == 0)
				pTextBox->SetSel(sCellText.GetLength(),0, TRUE);
			else
				pTextBox->SetSel(sCellText.GetLength(), sCellText.GetLength(), TRUE);

			pTextBox->SetFocus();	
			// set the control array.
			m_pTextBox[nStyle] = pTextBox;
			break;
		}
	case 3:
		{
			VdclNumericEdit *pTextBox;
			// get or create the control.
			if (m_pTextBox[nStyle] == NULL)
			{
				pTextBox = new VdclNumericEdit;
				pTextBox->Create(nStyle, rc, this, 110+nStyle);
			}
			else
			{
				pTextBox = (VdclNumericEdit*)m_pTextBox[nStyle];
				pTextBox->MoveWindow(rc, TRUE);
			}

			pTextBox->m_pGridDropList = NULL;
			
			// setup the control
			pTextBox->m_strOldValue = sCellText;
			pTextBox->SetWindowText(sCellText);
			pTextBox->SetBkColor(backGround);
			if (nChar > 0)
			{
				if (pTextBox->m_sFilter.FindOneOf(CString(char(nChar))) > -1)
					pTextBox->SetWindowText(CString(char(nChar)));
				pTextBox->GetWindowText(sCellText);
			}
			pTextBox->SetFont(GetFont());
			//pTextBox->EnableWindow(TRUE);
			pTextBox->ShowWindow(TRUE);
			if (nChar == 0)
				pTextBox->SetSel(sCellText.GetLength(),0, TRUE);
			else
				pTextBox->SetSel(sCellText.GetLength(), sCellText.GetLength(), TRUE);
			pTextBox->SetFocus();			
			
			// set the control array.
			m_pTextBox[nStyle] = pTextBox;
			break;
		}
	case 4:
		{
			VdclSymbolEdit *pTextBox;
			// get or create the control.
			if (m_pTextBox[nStyle] == NULL)
			{
				pTextBox = new VdclSymbolEdit;
				pTextBox->Create(nStyle, rc, this, 110+nStyle);
			}
			else
			{
				pTextBox = (VdclSymbolEdit*)m_pTextBox[nStyle];
				pTextBox->MoveWindow(rc, TRUE);
			}


			// setup the control
			pTextBox->m_strOldValue = sCellText;
			pTextBox->SetWindowText(sCellText);		
			if (nChar > 0)
			{
				pTextBox->SetWindowText(CString(char(nChar)));
				pTextBox->GetWindowText(sCellText);
			}
			pTextBox->SetFont(GetFont());
			//pTextBox->EnableWindow(TRUE);
			pTextBox->ShowWindow(TRUE);
			if (nChar == 0)
				pTextBox->SetSel(sCellText.GetLength(),0, TRUE);
			else
				pTextBox->SetSel(sCellText.GetLength(), sCellText.GetLength(), TRUE);
			pTextBox->SetFocus();			

			// set the control array.
			m_pTextBox[nStyle] = pTextBox;
			break;
		}
	case 9:
		{
			CCurrencyEdit *pTextBox;
			// get or create the control.
			if (m_pTextBox[nStyle] == NULL)
			{
				pTextBox = new CCurrencyEdit;

				UINT nCreateStyle = WS_CHILD | WS_VISIBLE | ES_WANTRETURN | WS_CLIPSIBLINGS | WS_HSCROLL | ES_AUTOHSCROLL | ES_LEFT;
				pTextBox->Create(nCreateStyle, rc, this, 110+nStyle);
			}
			else
			{
				pTextBox = (CCurrencyEdit*)m_pTextBox[nStyle];
				pTextBox->MoveWindow(rc, TRUE);
			}

			// setup the control
			pTextBox->m_strOldValue = sCellText;
			sCellText.TrimLeft(sDollar);
			sCellText.Replace(sComma, CString());
			double rValue = _tstof(sCellText);
			pTextBox->Set_Number(rValue);
			
			if (nChar > 0)
			{
				pTextBox->Set_Number(_ttoi(CString(TCHAR(nChar))));
				pTextBox->GetWindowText(sCellText);
			}
			pTextBox->SetFont(GetFont());
			pTextBox->ShowWindow(TRUE);

			pTextBox->SetFocus();			
			
			if (nChar == 0)
				pTextBox->SetSel(sCellText.GetLength(),0, TRUE);
			else
				pTextBox->SetSel(1, 1, TRUE);
						
			// set the control array.
			m_pTextBox[nStyle] = pTextBox;
			break;
		}	
	case 10:
	case 11:
		{
			CDateTimeHolder *pTextBox;
			
			rc.top -= 1;
			rc.right += 2;

			// get or create the control.
			if (m_pTextBox[nStyle] == NULL)
			{
				pTextBox = new CDateTimeHolder;

				if (nStyle == 10)
				{
					pTextBox->CreateDate(rc, this, 310+nStyle);
					
				}
				else
				{
					pTextBox->CreateTime(rc, this, 410+nStyle);
				}
			}
			else
			{
				pTextBox = (CDateTimeHolder*)m_pTextBox[nStyle];
				pTextBox->MoveWindow(rc, TRUE);
			}
			
			int nDataStyle = 0;

			if (GetCellData(nRow, nCol) == NULL)
			{
				nDataStyle = m_pColImages->GetIntArrayPtr()->at(nCol);
			}
			else
			{
				nDataStyle = GetCellData(nRow, nCol)->nData1;
			}

			if (nStyle == 10)
			{
				pTextBox->m_Child.SetFormat(s_DateFormats[nDataStyle]);					
			}
			else
			{
				pTextBox->m_Child.SetFormat(s_TimeFormats[nDataStyle]);
			}

			COleDateTime date;
			date.ParseDateTime(sCellText);
			SYSTEMTIME st = { '\0' };
			date.GetAsSystemTime(st);
			pTextBox->m_Child.SetTime(date);
			
			// setup the control
			pTextBox->m_Child.m_strOldValue = sCellText;
			//pTextBox->m_Child.SetBkColor(RGB(255, 0,0));
			
			pTextBox->m_Child.SetFont(GetFont());
			pTextBox->ShowWindow(TRUE);

			pTextBox->m_Child.SetFocus();			

			// set the control array.
			m_pTextBox[nStyle] = pTextBox;
			break;
		}	
	case 12:
		{
			CPercentageEdit *pTextBox;
			// get or create the control.
			if (m_pTextBox[nStyle] == NULL)
			{
				pTextBox = new CPercentageEdit;

				UINT nCreateStyle = WS_CHILD | WS_VISIBLE | ES_WANTRETURN | WS_CLIPSIBLINGS | WS_HSCROLL | ES_AUTOHSCROLL | ES_LEFT;
				pTextBox->Create(nCreateStyle, rc, this, 110+nStyle);
			}
			else
			{
				pTextBox = (CPercentageEdit*)m_pTextBox[nStyle];
				pTextBox->MoveWindow(rc, TRUE);
			}

			// setup the control
			pTextBox->m_strOldValue = sCellText;
			sCellText.Replace(sComma, CString());
			double rValue = _tstof(sCellText);
			pTextBox->Set_Number(rValue);
			
			
			if (nChar > 0)
			{
				pTextBox->Set_Number(_ttoi(CString(TCHAR(nChar))));
				pTextBox->GetWindowText(sCellText);
			}
			pTextBox->SetFont(GetFont());
			pTextBox->ShowWindow(TRUE);

			pTextBox->SetFocus();			
			
			if (nChar == 0)
				pTextBox->SetSel(sCellText.GetLength(),0, TRUE);
			else
				pTextBox->SetSel(1, 1, TRUE);
						
			// set the control array.
			m_pTextBox[nStyle] = pTextBox;
			break;
		}	
	default:
		{
			OdclEdit *pTextBox;

			// get or create the control.
			if (m_pTextBox[nStyle] == NULL)
			{
				pTextBox = new OdclEdit;
				pTextBox->Create(nStyle, rc, this, 110+nStyle);
			}
			else
			{
				pTextBox = (OdclEdit*)m_pTextBox[nStyle];
				pTextBox->MoveWindow(rc, TRUE);
			}

			pTextBox->m_pGridDropList = NULL;
			
			// setup the control
			pTextBox->m_strOldValue = sCellText;
			pTextBox->SetWindowText(sCellText);		
			pTextBox->SetBkColor(backGround);
			if (nChar > 0)
			{
				pTextBox->SetWindowText(CString(TCHAR(nChar)));
				sCellText = TCHAR(nChar);
			}
			
			pTextBox->SetFont(GetFont());
			pTextBox->ShowWindow(TRUE);
			if (nChar == 0)
				pTextBox->SetSel(sCellText.GetLength(), 0, TRUE);
			else
				pTextBox->SetSel(sCellText.GetLength(), sCellText.GetLength(), TRUE);
			pTextBox->SetFocus();			
			
			// set the control array.
			m_pTextBox[nStyle] = pTextBox;
			
			break;
		}
	}
}

void CSpreadSheet::ShowEllipsesButton(int nRow, int nCol, int nAsPick)
{
	if (!m_bEditCells) 
		return;

	int nCount = GetItemCount();
	for (int i=0; i<nCount; i++)
	{
		SetItemState(i, NULL, NULL);

		CHeaderCtrl *pHdr = GetHeaderCtrl();
		int nCount = pHdr->GetItemCount();
		for (int j=0; j<nCount; j++)
		{
			SetItem(i, j, LVIF_STATE, NULL, 0, NULL, NULL, 0);				
		}
	}		
		
	if (nRow > -1 && nCol > -1)
	{
		//SetItem(nRow, 0, LVIF_STATE, NULL, 0, LVIS_SELECTED, LVIS_SELECTED, 0);				
		//SetItem(nRow, nCol, LVIF_STATE, NULL, 0, LVIS_SELECTED, LVIS_SELECTED, 0);				
		m_nRowSelected = nRow;
		m_nColSelected = nCol;
	}
	else
	{
		m_nRowSelected = -1;
		m_nColSelected = -1;
	}
	

	CRect rc;
	if (nCol == 0)
		GetItemRect(nRow, rc, LVIR_LABEL);
	else
		GetSubItemRect(nRow, nCol, LVIR_LABEL, rc);

	rc.left = rc.right - 17;
	rc.left--;
	//rc.right--;
	rc.top--;
	//rc.bottom--;
	
	

	if (nAsPick > 1)
	{			
		rc.left -= 5;
	
		if (m_pFolderButton == NULL)
		{
			m_pFolderButton = new CXPStyleButtonST;
			m_pFolderButton->Create(this, rc, 192);
			
			m_pFolderButton->SetIcon(IDI_FOLDER2);
			m_pFolderButton->SetThemeHelper(&m_ThemeHelper);
			m_pFolderButton->SetFlat(FALSE);

			m_pFolderButton->m_sOnClickedEvent = m_ArxControl->GetStrProperty(nEventBtnClicked);
			m_pFolderButton->m_bInvokeWithSendString = m_bInvokeWithSendString;					
		}
		m_pFolderButton->m_nDirectory = nAsPick;
		m_pFolderButton->MoveWindow(rc, TRUE);	
		m_pFolderButton->ShowWindow(TRUE);		
		
	}
	else if (nAsPick == 1)
	{			
		if (m_pPickButton == NULL)
		{
			m_pPickButton = new CXPStyleButtonST;
			m_pPickButton->Create(this, rc, 192);
			
			m_pPickButton->SetIcon(IDI_PICSM);
			m_pPickButton->SetThemeHelper(&m_ThemeHelper);
			m_pPickButton->SetFlat(FALSE);

			m_pPickButton->m_sOnClickedEvent = m_ArxControl->GetStrProperty(nEventBtnClicked);
			m_pPickButton->m_bInvokeWithSendString = m_bInvokeWithSendString;		
		}
		m_pFolderButton->m_nDirectory = nAsPick;
		m_pPickButton->MoveWindow(rc, TRUE);	
		m_pPickButton->ShowWindow(TRUE);		
		
	}
	else if (nAsPick == 0)
	{
		if (m_pEllipsesButton == NULL)
		{
			m_pEllipsesButton = new CXPStyleButtonST;
			m_pEllipsesButton->Create(this, rc, 193);

			m_pEllipsesButton->SetWindowText(sDotDotDot);
			m_pEllipsesButton->SetThemeHelper(&m_ThemeHelper);
			m_pEllipsesButton->SetFlat(FALSE);

			m_pEllipsesButton->m_sOnClickedEvent = m_ArxControl->GetStrProperty(nEventBtnClicked);
			m_pEllipsesButton->m_bInvokeWithSendString = m_bInvokeWithSendString;
		}
		m_pFolderButton->m_nDirectory = nAsPick;
		m_pEllipsesButton->MoveWindow(rc, TRUE);
		m_pEllipsesButton->ShowWindow(TRUE);
	}
}

void CSpreadSheet::ShowImageComboBox(int nRow, int nCol, CStringArray &sStrings, CArray<int, int> &nItems)
{
	if (!m_bEditCells) 
		return;

	int nCount = GetItemCount();
	for (int i=0; i<nCount; i++)
	{
		SetItemState(i, NULL, NULL);

		CHeaderCtrl *pHdr = GetHeaderCtrl();
		int nCount = pHdr->GetItemCount();
		for (int j=0; j<nCount; j++)
		{
			SetItem(i, j, LVIF_STATE, NULL, 0, NULL, NULL, 0);				
		}
	}		
		
	if (nRow > -1 && nCol > -1)
	{
		SetItem(nRow, 0, LVIF_STATE, NULL, 0, LVIS_SELECTED, LVIS_SELECTED, 0);				
		SetItem(nRow, nCol, LVIF_STATE, NULL, 0, LVIS_SELECTED, LVIS_SELECTED, 0);				
		m_nRowSelected = nRow;
		m_nColSelected = nCol;
	}
	else
	{
		m_nRowSelected = -1;
		m_nColSelected = -1;
	}
	

	CRect rc;
	if (nCol == 0)
		GetItemRect(nRow, rc, LVIR_LABEL);
	else
		GetSubItemRect(nRow, nCol, LVIR_LABEL, rc);
	
	int nItemHeight;

	if (!m_bHasRowHeader || nCol > 0)
	{
		int nImage = GetItemImage(nRow, nCol);

		if (nImage == -1)
			nImage = 0;

		IMAGEINFO Info;

		CImageList *pImage1 = GetImageList(LVSIL_NORMAL);
		CImageList *pImage2 = GetImageList(LVSIL_SMALL);
	
		if (pImage1 != NULL)
			pImage1->GetImageInfo(0, &Info);
		if (pImage2 != NULL)
			pImage2->GetImageInfo(0, &Info);

		nItemHeight = Info.rcImage.bottom - Info.rcImage.top;
		rc.left += Info.rcImage.right - Info.rcImage.left + 4;
	}


	rc.left = rc.right - 15;
	rc.left--;
	rc.right--;
	

	CString sCellText;
	sCellText = GetItemText(nRow, nCol);
	
	
	CComboBoxHolder *pHolder = NULL;
	VdclComboBoxEx *pComboBox = NULL;
	int nStyle = 13;
	// get or create the control.
	if (m_pComboBox[nStyle] == NULL)
	{
		int nColWidth = GetColumnWidth(nCol);
		
		pHolder = new CComboBoxHolder;
		pComboBox = new VdclComboBoxEx;
		pHolder->Create(CString(), WS_CHILD|WS_VISIBLE, rc, this, 140);
		pHolder->pComboBox = pComboBox;
		CImageList *pImage = GetImageList(LVSIL_SMALL);
		
		nItemHeight = nItemHeight * 12;
		nItemHeight = nItemHeight + 10;
		DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST;
    
		BOOL b = pComboBox->Create(dwStyle, CRect(15-nColWidth,-2, rc.Width()+2, nItemHeight), pHolder, IDC_CB_CHILD);
		pComboBox->SetFont(GetFont());		
		pComboBox->SetDroppedWidth(nColWidth);
		pComboBox->SetImageList(pImage);
			
		pHolder->m_nStyle = nStyle;
		pHolder->SetFont(GetFont());
	}
	else
	{
		pHolder = (CComboBoxHolder*)m_pComboBox[nStyle];
		pComboBox = (VdclComboBoxEx*)pHolder->pComboBox;
		pHolder->MoveWindow(rc, TRUE);
		pComboBox->ResetContent();
	}

	if (pComboBox->GetImageList() != NULL)
		pComboBox->GetImageList()->SetBkColor(RGB(255,255,255));

	_Style *pCell = GetCellData(nRow, nCol);

	if (pCell == NULL)
	{
		// add the strings.
		for (int i=0; i<sStrings.GetSize(); i++)
		{
			COMBOBOXEXITEM cbi;

			cbi.mask = CBEIF_IMAGE | CBEIF_INDENT | CBEIF_OVERLAY |
			CBEIF_SELECTEDIMAGE | CBEIF_TEXT;

			cbi.iItem = i;
			cbi.pszText = (LPTSTR)(LPCTSTR)sStrings[i];
			cbi.cchTextMax = sStrings[i].GetLength();
			cbi.iImage = nItems[i];
			cbi.iSelectedImage = nItems[i];
			cbi.iOverlay = 2;
			cbi.iIndent = 0;

			pComboBox->InsertItem(&cbi);
		}			
	}
	else
	{
		// add the strings.
		for (int i=0; i<pCell->sStrings.GetSize(); i++)
		{
			COMBOBOXEXITEM cbi;

			cbi.mask = CBEIF_IMAGE | CBEIF_INDENT | CBEIF_OVERLAY |
			CBEIF_SELECTEDIMAGE | CBEIF_TEXT;

			cbi.iItem = i;
			cbi.pszText = (LPTSTR)(LPCTSTR)sStrings[i];
			cbi.cchTextMax = pCell->sStrings[i].GetLength();
			cbi.iImage = pCell->nInts[i];
			cbi.iSelectedImage = pCell->nInts[i];
			cbi.iOverlay = 2;
			cbi.iIndent = 0;

			pComboBox->InsertItem(&cbi);
		}			
	}
	// setup the control
	//
	pHolder->ShowWindow(TRUE);
	
	int nIndex = pComboBox->FindStringExact(0, sCellText);
	if (nIndex > -1)
	{
		pComboBox->SetCurSel(nIndex);
	}
	else
	{
		nIndex = pComboBox->FindString(0, sCellText);
		if (nIndex > -1)
			pComboBox->SetCurSel(nIndex);			
	}	
	pComboBox->SetFocus();			

	// set the control array.
	m_pComboBox[nStyle] = pHolder;

}


void CSpreadSheet::ShowComboBox(int nRow, int nCol, int nStyle, CStringArray &sStrings)
{
	if (!m_bEditCells) 
		return;

	int nCount = GetItemCount();
	if (nRow > -1 && nCol > -1)
	{
		m_nRowSelected = nRow;
		m_nColSelected = nCol;
	}
	else
	{
		m_nRowSelected = -1;
		m_nColSelected = -1;
	}
	
	BeginWaitCursor();

	CRect rc;
	if (nCol == 0)
		GetItemRect(nRow, rc, LVIR_LABEL);
	else
		GetSubItemRect(nRow, nCol, LVIR_LABEL, rc);
	
	if (!m_bHasRowHeader || nCol > 0)
	{
		int nImage = GetItemImage(nRow, nCol);

		if (nImage > -1)
		{
			IMAGEINFO Info;

			CImageList *pImage1 = GetImageList(LVSIL_NORMAL);
			CImageList *pImage2 = GetImageList(LVSIL_SMALL);
		
			if (pImage1 != NULL)
				pImage1->GetImageInfo(0, &Info);
			if (pImage2 != NULL)
				pImage2->GetImageInfo(0, &Info);

			rc.left += Info.rcImage.right - Info.rcImage.left + 4;
		}
	}


	rc.left = rc.right - 15;
	rc.bottom = rc.top + 17;
	
	if (m_bHasGridLines)
	{
		rc.right--;
		rc.left--;
	}

	if (nStyle > 2)
	{
		rc.top--;
		rc.bottom = rc.top + 18;
	}


	CString sCellText;
	sCellText = GetItemText(nRow, nCol);
	
	CComboBoxHolder *pHolder = NULL;
	CComboBox *pTheComboBox = NULL;
			
	int nColWidth = GetColumnWidth(nCol);
				
	if (m_pComboBox[nStyle] == NULL)
	{
		pHolder = new CComboBoxHolder;
	}

	rc.top++;	
			
	switch (nStyle)
	{
	case 2:
		{
			VdclComboBox *pComboBox = NULL;
			rc.top--;	
			// get or create the control.
			if (m_pComboBox[nStyle] == NULL)
			{
				pComboBox = new VdclComboBox;
				
				pHolder->Create(sCellText, WS_CHILD, rc, this, 120+nStyle);
				
				pHolder->pComboBox = pComboBox;
				pComboBox->Create(CRect(15-nColWidth,-2, rc.Width()+2, 350), pHolder, 410);
				pComboBox->SetFont(GetFont());
			
				pHolder->m_nStyle = nStyle;
				pHolder->SetFont(GetFont());				
			}
			else
			{
				pHolder = (CComboBoxHolder*)m_pComboBox[nStyle];
				pComboBox = (VdclComboBox*)pHolder->pComboBox;
				pHolder->MoveWindow(rc, TRUE);
				pComboBox->MoveWindow(CRect(15-nColWidth,-2, rc.Width()+2, 350));
				pComboBox->ResetContent();
			}
			pComboBox->m_pGridEdit = NULL;
			pTheComboBox = pComboBox;
			break;
		}
	case 3:
		{
			VdclArrowHeadComboBox *pComboBox = NULL;
			rc.top--;
			rc.bottom++;
			if (m_nRowHeight == 19)
				rc.bottom++;
			if (m_nRowHeight >= 20)
				rc.bottom += 2;

			// get or create the control.
			if (m_pComboBox[nStyle] == NULL)
			{
				pComboBox = new VdclArrowHeadComboBox;
				
				pHolder->Create(sCellText, WS_CHILD, rc, this, 120+nStyle);
				
				pHolder->pComboBox = pComboBox;
				pComboBox->Create(nStyle, CRect(15-nColWidth,-2, rc.Width()+2, 350), pHolder, 410);
				pComboBox->SetFont(GetFont());

				pHolder->m_nStyle = nStyle;
				pHolder->SetFont(GetFont());				
			}
			else
			{
				pHolder = (CComboBoxHolder*)m_pComboBox[nStyle];
				pComboBox = (VdclArrowHeadComboBox*)pHolder->pComboBox;
				pComboBox->MoveWindow(CRect(15-nColWidth,-2, rc.Width()+2, 350));
				pHolder->MoveWindow(rc, TRUE);
			}
			pTheComboBox = pComboBox;
			break;
		}
	case 4:
		{
			VdclColorComboBox *pComboBox = NULL;
			rc.top--;
			rc.bottom++;
			if (m_nRowHeight == 19)
				rc.bottom++;
			if (m_nRowHeight >= 20)
				rc.bottom += 2;

			// get or create the control.
			if (m_pComboBox[nStyle] == NULL)
			{
				pComboBox = new VdclColorComboBox;
				
				pHolder->Create(sCellText, WS_CHILD, rc, this, 120+nStyle);
				
				pHolder->pComboBox = pComboBox;
				pComboBox->Create(nStyle, CRect(15-nColWidth,-2, rc.Width()+2, 350), pHolder, 410);
				pComboBox->SetFont(GetFont());

				pHolder->m_nStyle = nStyle;
				pHolder->SetFont(GetFont());				

			}
			else
			{
				pHolder = (CComboBoxHolder*)m_pComboBox[nStyle];
				pComboBox = (VdclColorComboBox*)pHolder->pComboBox;
				pComboBox->MoveWindow(CRect(15-nColWidth,-2, rc.Width()+2, 350));
				pHolder->MoveWindow(rc, TRUE);
			}
			
			pTheComboBox = pComboBox;
			break;
		}
	case 5:
		{
			OdclDirStylesCombo *pComboBox = NULL;
			//rc.top--;	
			// get or create the control.
			if (m_pComboBox[nStyle] == NULL)
			{
				pComboBox = new OdclDirStylesCombo;
				
				pComboBox->m_bTextStyle = true;
				pHolder->Create(sCellText, WS_CHILD, rc, this, 120+nStyle);
				
				pHolder->pComboBox = pComboBox;
				pComboBox->Create(nStyle, CRect(15-nColWidth,-2, rc.Width()+2, 350), pHolder, 410);
				pComboBox->SetFont(GetFont());
			
				pHolder->m_nStyle = nStyle;
				pHolder->SetFont(GetFont());				
			}
			else
			{
				pHolder = (CComboBoxHolder*)m_pComboBox[nStyle];
				pComboBox = (OdclDirStylesCombo*)pHolder->pComboBox;
				pComboBox->MoveWindow(CRect(15-nColWidth,-2, rc.Width()+2, 350));
				pHolder->MoveWindow(rc, TRUE);
				pComboBox->ResetContent();
			}
			pTheComboBox = pComboBox;
			break;
		}
	case 6:
		{
			VdclPlotStyleNamesComboBox *pComboBox = NULL;
			// get or create the control.
			if (m_pComboBox[nStyle] == NULL)
			{
				pComboBox = new VdclPlotStyleNamesComboBox;
				
				pHolder->Create(sCellText, WS_CHILD, rc, this, 120+nStyle);
				
				pHolder->pComboBox = pComboBox;
				pComboBox->Create(nStyle, CRect(15-nColWidth,-2, rc.Width()+2, 350), pHolder, 410);
				pComboBox->SetFont(GetFont());
				
				pHolder->m_nStyle = nStyle;
				pHolder->SetFont(GetFont());				
			}
			else
			{
				pHolder = (CComboBoxHolder*)m_pComboBox[nStyle];
				pComboBox = (VdclPlotStyleNamesComboBox*)pHolder->pComboBox;
				pComboBox->MoveWindow(CRect(15-nColWidth,-2, rc.Width()+2, 350));
				pHolder->MoveWindow(rc, TRUE);
			}
			pTheComboBox = pComboBox;
			break;
		}	
	case 7:
		{
			VdclPlotStyleTablesComboBox *pComboBox = NULL;
			// get or create the control.
			if (m_pComboBox[nStyle] == NULL)
			{
				pComboBox = new VdclPlotStyleTablesComboBox;
				
				pHolder->Create(sCellText, WS_CHILD, rc, this, 120+nStyle);
				
				pHolder->pComboBox = pComboBox;
				pComboBox->Create(nStyle, CRect(15-nColWidth,-2, rc.Width()+2, 350), pHolder, 410);
				pComboBox->SetFont(GetFont());
				
				pHolder->m_nStyle = nStyle;
				pHolder->SetFont(GetFont());				
			}
			else
			{
				pHolder = (CComboBoxHolder*)m_pComboBox[nStyle];
				pComboBox = (VdclPlotStyleTablesComboBox*)pHolder->pComboBox;
				pComboBox->MoveWindow(CRect(15-nColWidth,-2, rc.Width()+2, 350));
				pHolder->MoveWindow(rc, TRUE);
			}
			pTheComboBox = pComboBox;
			break;
		}	

	case 8:
		{
			CPrinterComboBox *pComboBox = NULL;
			// get or create the control.
			if (m_pComboBox[nStyle] == NULL)
			{
				pHolder->Create(sCellText, WS_CHILD, rc, this, 120+nStyle);
				CRect rcWnd(15-nColWidth,-2, rc.Width()+2, 350);
				pComboBox = new CPrinterComboBox( *m_pParentCtrlPane, NULL, 410, rcWnd );
				pHolder->pComboBox = pComboBox;
				pComboBox->SetFont(GetFont());
				
				pHolder->m_nStyle = nStyle;
				pHolder->SetFont(GetFont());				
			}
			else
			{
				pHolder = (CComboBoxHolder*)m_pComboBox[nStyle];
				pComboBox = (CPrinterComboBox*)pHolder->pComboBox;
				pComboBox->MoveWindow(CRect(15-nColWidth,-2, rc.Width()+2, 350));
				pHolder->MoveWindow(rc, TRUE);
			}
			pTheComboBox = pComboBox;
			break;
		}	
	case 9:
		{
			CFontCombo *pComboBox = NULL;
			rc.top--;
			rc.bottom++;
			// get or create the control.
			if (m_pComboBox[nStyle] == NULL)
			{
				pComboBox = new CFontCombo;
				
				pHolder->Create(sCellText, WS_CHILD, rc, this, 120+nStyle);
				
				pHolder->pComboBox = pComboBox;
				pComboBox->Create(nStyle, CRect(15-nColWidth,-2, rc.Width()+2, 350), pHolder, 410);
				pComboBox->SetFont(GetFont());
				
				pHolder->m_nStyle = nStyle;
				pHolder->SetFont(GetFont());				
			}
			else
			{
				pHolder = (CComboBoxHolder*)m_pComboBox[nStyle];
				pComboBox = (CFontCombo*)pHolder->pComboBox;
				pComboBox->MoveWindow(CRect(15-nColWidth,-2, rc.Width()+2, 350));
				pHolder->MoveWindow(rc, TRUE);
			}
			pTheComboBox = pComboBox;
			break;
		}
	case 10:
		{
			CComboBoxFolder *pComboBox = NULL;
			//rc.top--;	
			//rc.bottom++;

			// get or create the control.
			if (m_pComboBox[nStyle] == NULL)
			{
				pComboBox = new CComboBoxFolder;
				
				pHolder->Create(sCellText, WS_CHILD, rc, this, 120+nStyle);
				
				pHolder->pComboBox = pComboBox;
				pComboBox->Create(nStyle, CRect(15-nColWidth,-2, rc.Width()+2, 350), pHolder, 410);
				pComboBox->SetFont(GetFont());
				pComboBox->Init(0, 300); 
				pHolder->m_nStyle = nStyle;
				pHolder->SetFont(GetFont());				
			}
			else
			{
				pHolder = (CComboBoxHolder*)m_pComboBox[nStyle];
				pComboBox = (CComboBoxFolder*)pHolder->pComboBox;
				pComboBox->MoveWindow(CRect(15-nColWidth,-2, rc.Width()+2, 350));
				pHolder->MoveWindow(rc, TRUE);
			}
			pTheComboBox = pComboBox;
			break;
		}
	case 11:
		{
			OdclLayerCombo *pComboBox = NULL;
			rc.top--;	
			rc.bottom++;
			
			// get or create the control.
			if (m_pComboBox[nStyle] == NULL)
			{
				pComboBox = new OdclLayerCombo;
				
				pHolder->Create(sCellText, WS_CHILD, rc, this, 120+nStyle);
				
				pHolder->pComboBox = pComboBox;
				pComboBox->Create(nStyle, CRect(15-nColWidth,-2, rc.Width()+2, 350), pHolder, 410);
				pComboBox->SetFont(GetFont());
				
				pHolder->m_nStyle = nStyle;
				pHolder->SetFont(GetFont());				
			}
			else
			{
				pHolder = (CComboBoxHolder*)m_pComboBox[nStyle];
				pComboBox = (OdclLayerCombo*)pHolder->pComboBox;
				
				pHolder->MoveWindow(rc, TRUE);
			}
			pTheComboBox = pComboBox;
			break;
		}
	case 12:
		{
			OdclDirStylesCombo *pComboBox = NULL;
			//rc.top--;	
			// get or create the control.
			if (m_pComboBox[nStyle] == NULL)
			{
				pComboBox = new OdclDirStylesCombo;
				
				pHolder->Create(sCellText, WS_CHILD, rc, this, 120+nStyle);
				
				pHolder->pComboBox = pComboBox;
				pComboBox->Create(nStyle, CRect(15-nColWidth,-2, rc.Width()+2, 350), pHolder, 410);
				pComboBox->SetFont(GetFont());
			
				pHolder->m_nStyle = nStyle;
				pHolder->SetFont(GetFont());				
			}
			else
			{
				pHolder = (CComboBoxHolder*)m_pComboBox[nStyle];
				pComboBox = (OdclDirStylesCombo*)pHolder->pComboBox;
				pHolder->MoveWindow(rc, TRUE);
				pComboBox->ResetContent();
			}
			pTheComboBox = pComboBox;
			break;
		}
		
	}

	pTheComboBox->MoveWindow(CRect(15-nColWidth,-2, rc.Width()+2, 350));				
	pTheComboBox->ShowWindow(TRUE);

	if (nStyle == 2)
	{
		_Style *pCell = GetCellData(nRow, nCol);

		if (pCell != NULL)
		{
			// add the strings.
			for (int i=0; i<pCell->sStrings.GetSize(); i++)
			{
				pTheComboBox->AddString(pCell->sStrings[i]);
			}			
		}
		else
		{			
			// add the strings.
			for (int i=0; i<sStrings.GetSize(); i++)
			{
				pTheComboBox->AddString(sStrings[i]);
			}			
		}	
	}

	// setup the control
	//
	pHolder->ShowWindow(TRUE);
	
	int nIndex = pTheComboBox->FindStringExact(0, sCellText);
	if (nIndex > -1)
	{
		pTheComboBox->SetCurSel(nIndex);
	}
	else
	{
		nIndex = pTheComboBox->FindString(0, sCellText);
		if (nIndex > -1)
			pTheComboBox->SetCurSel(nIndex);			
		else
			pTheComboBox->SetCurSel(0);			
	}
		
	pTheComboBox->SetDroppedWidth(nColWidth);
				
	pTheComboBox->SetFocus();			

	// set the control array.
	m_pComboBox[nStyle] = pHolder;

	EndWaitCursor();
}

void CSpreadSheet::SetItemImage( int nRow, int nCol, int nImage)  
{
	
	CString sText = GetItemText(nRow, nCol);
	
	LVITEM lvItem;
	lvItem.mask = LVIF_TEXT|LVIF_IMAGE;
	lvItem.iItem = nRow;
	lvItem.iSubItem = nCol;	
	lvItem.iImage = nImage;	
	TCHAR sValue [256];
	_tcscpy(sValue, sText);		
	lvItem.pszText = sValue;

	SetItem(&lvItem);
}

void CSpreadSheet::OnSetfocus(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_bShowHighlight = true;
	HideEditControls();

	// call methods to invoke the event
	InvokeMethod(m_ArxControl->GetStrProperty(nEventSetFocus), m_bInvokeWithSendString);
	
	*pResult = 0;
}

void CSpreadSheet::UpdateCell(CString sText) 
{
	if (m_nRowSelected > -1)
	{
		SetItemText(m_nRowSelected, m_nColSelected, sText);
	}


}
void CSpreadSheet::OnEndlabeledit(CString sText) 
{	
	if (m_nRowSelected > -1)
	{
		InvokeMethodIntInt(
			m_ArxControl->GetStrProperty(nEventEndLabelEdit),
			m_nRowSelected,
			m_nColSelected,
			m_bInvokeWithSendString);

		SetItemText(m_nRowSelected, m_nColSelected, sText);

		int nCount = GetItemCount();
		for (int i=0; i<nCount; i++)
		{
			SetItemState(i, NULL, NULL);

			CHeaderCtrl *pHdr = GetHeaderCtrl();
			int nCount = pHdr->GetItemCount();
			for (int j=0; j<nCount; j++)
			{
				SetItem(i, j, LVIF_STATE, NULL, 0, NULL, NULL, 0);				
			}
		}	
		SetItem(m_nRowSelected, 0, LVIF_STATE, NULL, 0, LVIS_SELECTED, LVIS_SELECTED, 0);				
		SetItem(m_nRowSelected,	m_nColSelected, LVIF_STATE, NULL, 0, NULL, NULL, 0);				
	}


}
void CSpreadSheet::CallBeginLabelEdit(CPoint point) 
{
	LVHITTESTINFO lvhti;

	// Clear the subitem text the user clicked on.
	lvhti.pt = point;
	SubItemHitTest(&lvhti);
	
	if (lvhti.flags & LVHT_ONITEMLABEL)
	{
		// call methods to invoke the event
		InvokeMethodIntInt(		
			m_ArxControl->GetStrProperty(nEventBeginLabelEdit),
			lvhti.iItem,
			lvhti.iSubItem,
			m_bInvokeWithSendString);
	}
	
}

int CSpreadSheet::GetItemImage( int nRow, int nCol)  
{
	LV_ITEM lvitem;
				
	lvitem.mask = LVIF_IMAGE | LVIF_PARAM | LVIF_STATE;
	lvitem.iItem = nRow;
	lvitem.iSubItem = nCol;				
	lvitem.stateMask = LVIS_CUT | LVIS_DROPHILITED | 
			LVIS_FOCUSED |  LVIS_SELECTED | 
			LVIS_OVERLAYMASK | LVIS_STATEIMAGEMASK;

	GetItem( &lvitem );

	return lvitem.iImage;

}

void CSpreadSheet::EndEditControls(CWnd *pWnd)
{
	if (pWnd->IsWindowVisible() == TRUE)
	{
		InvokeMethodIntInt(		
			m_ArxControl->GetStrProperty(nEventEndLabelEdit),
			m_nRowSelected, m_nColSelected,
			m_bInvokeWithSendString);
	}
}
void CSpreadSheet::HideEditControls()
{
	CRect rc;
  int i;
	for (i=0; i<nNumOfTextBoxes; i++)
	{
		if (m_pTextBox[i] != NULL && m_pTextBox[i]->IsWindowVisible() == TRUE)
		{
			EndEditControls(m_pTextBox[i]);
			m_pTextBox[i]->ShowWindow(FALSE);
		}
	}

	for (i=0; i<nNumOfComboBoxes; i++)
	{
		if (m_pComboBox[i] != NULL && m_pComboBox[i]->IsWindowVisible() == TRUE)
		{
			//EndEditControls(m_pComboBox[i]);
			m_pComboBox[i]->ShowWindow(FALSE);
		}
	}

	
	if (m_pPickButton != NULL && m_pPickButton->IsWindowVisible() == TRUE)
	{
		m_pPickButton->ShowWindow(FALSE);
	}
	if (m_pEllipsesButton != NULL && m_pEllipsesButton->IsWindowVisible() == TRUE)
	{
		m_pEllipsesButton->ShowWindow(FALSE);
	}
	if (m_pFolderButton != NULL && m_pFolderButton->IsWindowVisible() == TRUE)
	{
		m_pFolderButton->ShowWindow(FALSE);
	}
	
}

void CSpreadSheet::OnDestroy() 
{
	// init all the pointers for cell editing controls.
  int i;
	for (i=0; i<nNumOfTextBoxes; i++)
	{
		if (m_pTextBox[i] != NULL)
		{
			m_pTextBox[i]->DestroyWindow();
			delete m_pTextBox[i];
			m_pTextBox[i] = NULL;
		}
	}
	for (i=0; i<nNumOfComboBoxes; i++)
	{
		if (m_pComboBox[i] != NULL)
		{
			CComboBoxHolder *pHolder = (CComboBoxHolder*)m_pComboBox[i];
			
			pHolder->pComboBox->DestroyWindow();
			delete pHolder->pComboBox;
			pHolder->pComboBox = NULL;
			
			m_pComboBox[i]->DestroyWindow();
			delete m_pComboBox[i];
			m_pComboBox[i] = NULL;
		}
	}
	
	if (m_pPickButton != NULL)
	{
		m_pPickButton->DestroyWindow();
		delete m_pPickButton;
		m_pPickButton = NULL;
	}
	if (m_pEllipsesButton != NULL)
	{
		m_pEllipsesButton->DestroyWindow();
		delete m_pEllipsesButton;
		m_pEllipsesButton = NULL;
	}
	
	if (m_pFolderButton != NULL)
	{
		m_pFolderButton->DestroyWindow();
		delete m_pFolderButton;
		m_pFolderButton = NULL;
	}
	for (i=0; i<GetItemCount(); i++)
	{
		if (GetItemData(i) > 0)
		{
			delete (_RowData*)GetItemData(i);	
		}
	}
	
	CListCtrl::OnDestroy();
		
}

void CSpreadSheet::SetTooltipText(CString* spText, BOOL bActivate)
{
} // End of SetTooltipText

void CSpreadSheet::InitToolTip()
{
	if (m_ToolTip.m_hWnd == NULL)
	{
		// Create ToolTip control
		m_ToolTip.Create(this);
		// Create inactive
		m_ToolTip.Activate(FALSE);
	}
} // End of InitToolTip

// SortTextItems - Sort the list based on column text
// Returns		 - Returns true for success
// nCol			 - column that contains the text to be sorted
// bAscending	 - indicate sort order
// low			 - row to start scanning from - default row is 0
// high			 - row to end scan. -1 indicates last row
BOOL CSpreadSheet::SortTextItems( int nCol, BOOL bAscending, int low, int high)
{
	if (GetDlgItem(0) != NULL)
	{
 		if( nCol >= ((CHeaderCtrl*)GetDlgItem(0))->GetItemCount() )
			return FALSE;
	}
	if( high == -1 ) high = GetItemCount() - 1;

	int lo = low;
	int hi = high;
	CString midItem;

	if( hi <= lo ) return FALSE;

	midItem = GetItemText( (lo+hi)/2, nCol );

	// loop through the list until indices cross
	while( lo <= hi )
	{
		// rowText will hold all column text for one row
		CStringArray rowText;

		// find the first element that is greater than or equal to 
		// the partition element starting from the left Index.
		if( bAscending )
			while( ( lo < high ) && ( GetItemText(lo, nCol) < midItem ) )
				++lo;
		else
			while( ( lo < high ) && ( GetItemText(lo, nCol) > midItem ) )
				++lo;

		// find an element that is smaller than or equal to 
		// the partition element starting from the right Index.
		if( bAscending )
			while( ( hi > low ) && ( GetItemText(hi, nCol) > midItem ) )
				--hi;
		else
			while( ( hi > low ) && ( GetItemText(hi, nCol) < midItem ) )
				--hi;

		// if the indexes have not crossed, swap
		// and if the items are not equal
		if( lo <= hi )
		{
			// swap only if the items are not equal
			if( GetItemText(lo, nCol) != GetItemText(hi, nCol))
			{
				// swap the rows
				LV_ITEM lvitemlo, lvitemhi;
				
				int nColCount = 0;
				if (GetDlgItem(0) != NULL)
				{
					nColCount = ((CHeaderCtrl*)GetDlgItem(0))->GetItemCount();
				}
				else
				{
					nColCount = GetItemCount();
				}
				rowText.SetSize( nColCount );
				int i;
				for( i=0; i<nColCount; i++)
					rowText[i] = GetItemText(lo, i);
				lvitemlo.mask = LVIF_IMAGE | LVIF_PARAM | LVIF_STATE;
				lvitemlo.iItem = lo;
				lvitemlo.iSubItem = 0;				
				lvitemlo.stateMask = LVIS_CUT | LVIS_DROPHILITED | 
						LVIS_FOCUSED |  LVIS_SELECTED | 
						LVIS_OVERLAYMASK | LVIS_STATEIMAGEMASK;

				lvitemhi = lvitemlo;
				lvitemhi.iItem = hi;
				
				GetItem( &lvitemlo );
				GetItem( &lvitemhi );

				// here we are going to swap the text of all the items and sub items
				for( i=0; i<nColCount; i++)
				{ 
					CString sText = GetItemText(hi, i);
					int nImageHi = GetItemImage(hi, i);
					int nImageLo = GetItemImage(lo, i);
					SetItemTextImage(lo, i, sText, nImageHi);
					SetItemTextImage(hi, i, rowText[i], nImageLo);										
					SetItemText(lo, i, sText);
				}

				lvitemhi.iItem = lo;
				SetItem( &lvitemhi );

				for( i=0; i<nColCount; i++)
					SetItemText(hi, i, rowText[i]);

				lvitemlo.iItem = hi;
				SetItem( &lvitemlo );
			}

			++lo;
			--hi;
		}
	}

	// If the right index has not reached the left side of array
	// must now sort the left partition.
	if( low < hi )
		SortTextItems( nCol, bAscending , low, hi);

	// If the left index has not reached the right side of array
	// must now sort the right partition.
	if( lo < high )
		SortTextItems( nCol, bAscending , lo, high );

	return TRUE;
}
bool CSpreadSheet::SortNumericItems( int nCol, BOOL bAscending,int low, int high)
{
	if (GetDlgItem(0) != NULL)
	{
		if( nCol >= ((CHeaderCtrl*)GetDlgItem(0))->GetItemCount() )
			return FALSE;
	}
	
	if( high == -1 ) high = GetItemCount() - 1;
	int lo = low;   
	int hi = high;
	
	int midItem;
	
	if( hi <= lo ) return FALSE;
	
	midItem = _ttoi(GetItemText( (lo+hi)/2, nCol ));
	
	// loop through the list until indices cross
	while( lo <= hi )
	{
		// rowText will hold all column text for one row
		CStringArray rowText;
		
		// find the first element that is greater than or equal to 
		// the partition element starting from the left Index.
		if( bAscending )
			while( ( lo < high ) && (_ttoi(GetItemText(lo, nCol)) < midItem ) )
				++lo;           
		else
			while( ( lo < high ) && (_ttoi(GetItemText(lo, nCol)) > midItem ) )
				++lo;
                
		// find an element that is smaller than or equal to 
		// the partition element starting from the right Index.
		if( bAscending )
			while( ( hi > low ) && (_ttoi(GetItemText(hi, nCol)) > midItem ) )
				--hi;           
		else
			while( ( hi > low ) && (_ttoi(GetItemText(hi, nCol)) < midItem ) )
				--hi;
				
		// if the indexes have not crossed, swap                
		// and if the items are not equal
		if( lo <= hi )
		{
			// swap only if the items are not equal
			if(_ttoi(GetItemText(lo, nCol)) != _ttoi(GetItemText(hi, nCol)) )
			{                               
				// swap the rows
				LV_ITEM lvitemlo, lvitemhi;
                
				int nColCount = 0;
				if (GetDlgItem(0) != NULL)
				{
					nColCount = ((CHeaderCtrl*)GetDlgItem(0))->GetItemCount();
				}
				else
				{
					nColCount = GetItemCount();
				}

				rowText.SetSize( nColCount );
                
				int i;
				for( i=0; i < nColCount; i++)
							rowText[i] = GetItemText(lo, i);
                
				lvitemlo.mask = LVIF_IMAGE | LVIF_PARAM | LVIF_STATE;
				lvitemlo.iItem = lo;
				lvitemlo.iSubItem = 0;
				lvitemlo.stateMask = LVIS_CUT | LVIS_DROPHILITED |
							LVIS_FOCUSED |  LVIS_SELECTED |
							LVIS_OVERLAYMASK | LVIS_STATEIMAGEMASK;
				lvitemhi = lvitemlo;
				lvitemhi.iItem = hi;
                
				GetItem( &lvitemlo );
				GetItem( &lvitemhi );
                
				for( i=0; i< nColCount; i++)
				{
					CString sText = GetItemText(hi, i);
					int nImageHi = GetItemImage(hi, i);
					int nImageLo = GetItemImage(lo, i);
					SetItemTextImage(lo, i, sText, nImageHi);
					SetItemTextImage(hi, i, rowText[i], nImageLo);										
					SetItemText(lo, i, sText);
				}
                
				lvitemhi.iItem = lo;
				SetItem( &lvitemhi );
                
				for( i=0; i< nColCount; i++)
							SetItemText(hi, i, rowText[i]);
                
				lvitemlo.iItem = hi;
				SetItem( &lvitemlo );
			}
			
			++lo;
			--hi;
		}
	}
	
	// If the right index has not reached the left side of array
	// must now sort the left partition.
	if( low < hi )
		SortNumericItems( nCol, bAscending , low, hi);
	
	// If the left index has not reached the right side of array
	// must now sort the right partition.
	if( lo < high )
		SortNumericItems( nCol, bAscending , lo, high );
	
	return TRUE;
}
void CSpreadSheet::SetItemTextImage( int nRow,int nCol, CString sText, int nImage)  
{
	sText = GetItemText(nRow, nCol);
	// we do not update the image item in the first column
	if (nCol == 0) return;

	LVITEM lvItem;
	if (nImage == -1)
		lvItem.mask = LVIF_TEXT|LVIF_IMAGE;
	else
		lvItem.mask = LVIF_TEXT|LVIF_IMAGE;
	lvItem.iItem = nRow;
	lvItem.iSubItem = nCol;	
	lvItem.iImage = nImage;	
	TCHAR sValue [256];
	_tcscpy(sValue, sText);		
	lvItem.pszText = sValue;

	SetItem(&lvItem);


}
void CSpreadSheet::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
    lpMeasureItemStruct->itemHeight = m_nRowHeight;
}
void CSpreadSheet::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	lpMeasureItemStruct->itemHeight = m_nRowHeight;
		
	CListCtrl::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}


void CSpreadSheet::OnSize(UINT nType, int cx, int cy) 
{
	CListCtrl::OnSize(nType, cx, cy);

	if (IsWindow(m_hWnd) == TRUE)
	{
		HideEditControls();
	}
}

_RowData * CSpreadSheet::GetRowData(int nRow) 
{
	if (GetItemData(nRow) == 0)
		return NULL;
	return (_RowData *)GetItemData(nRow); 
}

void CSpreadSheet::OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	InvokeMethodInt(m_ArxControl->GetStrProperty(nEventColumnClick), pNMListView->iSubItem, m_bInvokeWithSendString);
	
	*pResult = 0;
}
