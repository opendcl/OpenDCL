// GridCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "GridCtrl.h"
#include "ControlPane.h"
#include "DclControlObject.h"
#include "PropertyObject.h"
#include "PropertyIds.h"
#include "ToolTips.h"
#include "Workspace.h"
#include "AcadColorTable.h"
#include "SharedRes.h"

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

#define GLYPH_WIDTH 17 


/////////////////////////////////////////////////////////////////////////////
// CGridCtrl

CGridCtrl::CGridCtrl( CDclControlObject* pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CDialogControl( pTemplate, pPane, this )
, m_HeaderCtrl( this )
{
	ForegroundColor = GetSysColor(COLOR_BTNTEXT);
	m_pTheme = NULL;
	m_nRowSelected = -1;
	m_nColSelected = -2;
	m_pArrowImages = NULL;
	m_pFontImages = NULL;
	m_pImageList = NULL;
	m_bHasRowHeader = pTemplate->GetBoolProperty( nRowHeader );
	m_nRowHeight = pTemplate->GetLongProperty( nRowHeight );
	m_bOrientationVer = pTemplate->GetBoolProperty( nAlternateOrient );
	alertnateColor = pTemplate->GetColorProperty( nAlternateColor );
	m_bHasGridLines = pTemplate->GetBoolProperty( nGridLines );
	
	m_pColCaptions = mpTemplate->GetPropertyObject( nColumnCaptions );	
	m_pColWidths = mpTemplate->GetPropertyObject( nColumnWidths );	
	m_pColImages = mpTemplate->GetPropertyObject( nColumnImages );	
	m_pColStyles = mpTemplate->GetPropertyObject( nColumnStyles );
	m_pColAlignment = mpTemplate->GetPropertyObject( nColumnAlignments );
	m_pColDefault = mpTemplate->GetPropertyObject( nColumnDefaultImages );	
	m_pColAlternate = mpTemplate->GetPropertyObject( nColumnAlternateImages );	

	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CGridCtrl::~CGridCtrl()
{
	m_HeaderCtrl.DestroyWindow();
	if (m_pFontImages != NULL)
	{
		m_pFontImages->DeleteImageList();
		delete m_pFontImages;
	}
	if (m_pArrowImages != NULL)
	{
		m_pArrowImages->DeleteImageList();
		delete m_pArrowImages;
	}
	if (m_pImageList != NULL)
	{
		m_pImageList->DeleteImageList();
		delete m_pImageList;
	}
}

#undef SubclassWindow
bool CGridCtrl::Create( CWnd* pParentWnd, UINT nID )
{
	bool bSuccess = (__super::Create( GetWndStyle(), GetWndRect(), pParentWnd, nID ) != FALSE);

	if( bSuccess && !ApplyPropertiesEnum() )
		bSuccess = false;
	
	SetExtendedStyle( GetExtendedStyle() | LVS_EX_SUBITEMIMAGES );

	SetupColumns();
	if( mpTemplate->GetBoolProperty( nColHeader ) )
	{
		HWND hHdrCtrl = GetHeaderCtrl()->m_hWnd;
		m_HeaderCtrl.SubclassWindow( hHdrCtrl );
	}

	return bSuccess;
}

DWORD CGridCtrl::GetWndStyle() const
{
	DWORD dwStyle = CDialogControl::GetWndStyle();
	dwStyle |= WS_CLIPCHILDREN | LVS_REPORT | LVS_OWNERDRAWFIXED | LVS_SINGLESEL | LVS_SHAREIMAGELISTS;
	if( !mpTemplate->GetBoolProperty( nColHeader ) )
		dwStyle |= LVS_NOCOLUMNHEADER;
	switch( mpTemplate->GetLongProperty( nListViewSort ) )
	{
	case 1:
		dwStyle |= LVS_SORTDESCENDING;
		break;
	case 2:
		dwStyle |= LVS_SORTASCENDING;
		break;
	}
	if( !mpTemplate->GetBoolProperty( nLabelWrap ) )
		dwStyle |= LVS_NOLABELWRAP;
	if( mpTemplate->GetBoolProperty( nShowSelectAlways ) )
		dwStyle |= LVS_SHOWSELALWAYS;
	if( !mpTemplate->GetBoolProperty( nMultiSelect ) )
		dwStyle |= LVS_SINGLESEL;
	return dwStyle;
}

bool CGridCtrl::OnApplyProperty( RefCountedPtr< CPropertyObject > pProp )
{
	if( !__super::OnApplyProperty( pProp ) )
		return false;
	bool bFailed = false;
	switch( pProp->GetID() )
	{
	case nImageList:
		{
			RefCountedPtr< CImageListObject > pImageList = mpTemplate->GetImageList();
			if (pImageList)
			{
				pImageList->m_ImageList.SetBkColor( ::GetSysColor( COLOR_BTNFACE ) );
				SetImageList( &pImageList->m_ImageList, TVSIL_NORMAL );
				SetImageList( &pImageList->m_ImageList, LVSIL_SMALL );
			}
			else
			{
				if( !m_DefaultImageList.m_hImageList )
					m_DefaultImageList.Create( 1, mpTemplate->GetLongProperty( nRowHeight ), ILC_COLOR, 1, 1 );
				m_DefaultImageList.SetBkColor( ::GetSysColor(COLOR_WINDOW ) );
				SetImageList( &m_DefaultImageList, TVSIL_NORMAL );
				SetImageList( &m_DefaultImageList, LVSIL_SMALL );
			}
		}
		break;
	//case nFullRowSelect:
	//	{
	//		DWORD dwExStyle = GetExtendedStyle();
	//		if( pProp->GetBooleanValue() )
	//			dwExStyle |= LVS_EX_FULLROWSELECT;
	//		else
	//			dwExStyle &= ~LVS_EX_FULLROWSELECT;
	//		SetExtendedStyle( dwExStyle );
	//	}
	//	break;
	case nGridLines:
		{
			DWORD dwExStyle = GetExtendedStyle();
			if( pProp->GetBooleanValue() )
				dwExStyle |= LVS_EX_GRIDLINES;
			else
				dwExStyle &= ~LVS_EX_GRIDLINES;
			SetExtendedStyle( dwExStyle );
		}
		break;
	}
	return !bFailed;
}

void CGridCtrl::SetThemeHelper(CThemeHelperST* pTheme)
{	
	m_pTheme = pTheme;
}

int CGridCtrl::GetItemImage(int nRow, int nCol)  
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

void CGridCtrl::SetColumn(UINT nIndex)
{	
	CString sCaption = m_pColCaptions->GetStringArrayPtr()->at(nIndex);
	HD_ITEM curItem =
		{ HDI_TEXT | HDI_FORMAT, 0, sCaption.LockBuffer(), NULL, sCaption.GetLength() + 1, HDF_STRING, 0, -1, -1 };
	if (m_pColImages->GetIntArrayPtr()->at(nIndex) >= 0)
	{
		curItem.mask |= HDI_IMAGE;
		curItem.iImage = m_pColImages->GetIntArrayPtr()->at(nIndex);
		curItem.fmt |= HDF_IMAGE;
	}
	
	switch(m_pColAlignment->GetIntArrayPtr()->at(nIndex))
	{					
	case 1:
		curItem.fmt |= HDF_CENTER;
		break;
	case 2:
		curItem.fmt |= HDF_RIGHT;
		break;				
	default:
		curItem.fmt |= HDF_LEFT;
		break;
	}
	GetHeaderCtrl()->SetItem(nIndex, &curItem);
}

void CGridCtrl::SetupColumns()
{
	for (size_t idxColumn = 0; idxColumn < m_pColWidths->GetIntArrayPtr()->size(); ++idxColumn)
	{
		InsertColumn(idxColumn,
								 m_pColCaptions->GetStringItem(idxColumn),
								 LVCFMT_LEFT,
								 m_pColWidths->GetIntArrayPtr()->at(idxColumn));
		SetColumn(idxColumn);
	}
}


BEGIN_MESSAGE_MAP(CGridCtrl, CListCtrl)
	ON_WM_MEASUREITEM()
	ON_WM_NCCALCSIZE()	
	ON_WM_MEASUREITEM_REFLECT()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGridCtrl message handlers

void CGridCtrl::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp) 
{
	UpdateWindow();	
	__super::OnNcCalcSize(bCalcValidRects, lpncsp);	
}

BOOL CGridCtrl::PreTranslateMessage(MSG* pMsg) 
{
	GetToolTipCtrl().RelayEvent(pMsg);
	return __super::PreTranslateMessage(pMsg);
}

void CGridCtrl::DrawOptionButton(CDC* pDC, CRect rc, bool bPressed, COLORREF backColor, BOOL bSelected)
{	
	HTHEME hTheme = m_pTheme? m_pTheme->OpenThemeData(GetSafeHwnd(), L"BUTTON") : NULL;
		
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
		
		::DeleteObject(SelectObject(pDC->m_hDC, OldPen));
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

void CGridCtrl::GetImageDropListItems(int nRow, int nCol, int &nImage, CString &sText)
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
		RefCountedPtr< CPropertyObject > pProp = mpTemplate->GetPropertyObject(nColumnListItems);	
		RefCountedPtr< CPropertyObject > pPropImages = mpTemplate->GetPropertyObject(nColumnListImages);
		if (nCol < (int)pProp->GetStringArrayListPtr()->size())
		{
			PropVal::TCStringArray& rStrings = pProp->GetStringArrayListPtr()->at(nCol);
			PropVal::TIntArray& rInts = pPropImages->GetIntArrayListPtr()->at(nCol);
			for (size_t i=0; i<rStrings.size(); i++)
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

void CGridCtrl::DrawTrueColor(CDC* pDC, CRect rc, int &nColor, CString &sText)
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
		int nComma = sText.Find(_T(","));
		int nComma2 = sText.Find(_T(","), nComma+1);
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

void CGridCtrl::DrawCheckBox(CDC* pDC, CRect rc, bool bPressed, bool bSelected)
{
	HTHEME hTheme = m_pTheme? m_pTheme->OpenThemeData(GetSafeHwnd(), L"BUTTON") : NULL;
		
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

void CGridCtrl::CellHitTest(CPoint point, int &nRow, int &nCol)
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

CRect CGridCtrl::GetCurSelRect()
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
 
int CGridCtrl::GetCurCellStyle()
{
	return GetCellStyle(m_nRowSelected, m_nColSelected);
}

int CGridCtrl::GetCellStyle(int nRow, int nCol)
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
	if (nCol < (int)m_pColStyles->GetIntArrayPtr()->size())
		return m_pColStyles->GetIntArrayPtr()->at(nCol);

	return 0;
}


_Style * CGridCtrl::GetCellData(int nRow, int nCol)
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


void CGridCtrl::SetCellStyle(int nRow, int nCol, int nStyle)
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
		SetItemData(nRow, (DWORD_PTR)pCells);
	}

	if (nCol >= static_cast< int >(pCells->m_Cells.GetCount()))
		pCells->m_Cells.SetSize( nCol + 1 );

	if (nCol < static_cast< int >(pCells->m_Cells.GetCount()))
	{
		if (pCells->m_Cells[nCol] == NULL)
			pCells->m_Cells[nCol] = new _Style;
		pCells->m_Cells.GetAt(nCol)->nStyle = nStyle;			
	}
}

void CGridCtrl::RefreshCell(int nRow, int nCol) 
{
	CRect rc = GetCurSelRect();
	InvalidateRect(rc, TRUE);
}

void CGridCtrl::ShowCurSel() 
{
	CRect rc = GetCurSelRect();
	InvalidateRect(rc, TRUE);
}

void CGridCtrl::SetItemImage( int nRow, int nCol, int nImage)  
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

void CGridCtrl::UpdateCell(CString sText) 
{
	if (m_nRowSelected > -1)
		SetItemText(m_nRowSelected, m_nColSelected, sText);
}

// SortTextItems - Sort the list based on column text
// Returns		 - Returns true for success
// nCol			 - column that contains the text to be sorted
// bAscending	 - indicate sort order
// low			 - row to start scanning from - default row is 0
// high			 - row to end scan. -1 indicates last row
BOOL CGridCtrl::SortTextItems( int nCol, BOOL bAscending, int low, int high)
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

bool CGridCtrl::SortNumericItems( int nCol, BOOL bAscending,int low, int high)
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

void CGridCtrl::SetItemTextImage( int nRow,int nCol, CString sText, int nImage)  
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

void CGridCtrl::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	lpMeasureItemStruct->itemHeight = m_nRowHeight;
}

void CGridCtrl::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	lpMeasureItemStruct->itemHeight = m_nRowHeight;
	__super::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}

_RowData * CGridCtrl::GetRowData(int nRow) 
{
	if (GetItemData(nRow) == 0)
		return NULL;
	return (_RowData *)GetItemData(nRow); 
}

void CGridCtrl::PostNcDestroy() 
{
	__super::PostNcDestroy();
	delete this;
}
