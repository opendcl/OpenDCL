// ArxBlockListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ArxBlockListCtrl.h"
#include "Resource.h"
#include "InvokeMethod.h"
#include "Workspace.h"
#include "ControlPane.h"


//utility functions
static WORD DibNumColors (VOID FAR * pv)
{
    INT                 bits;
    LPBITMAPINFOHEADER  lpbi;

    lpbi = ((LPBITMAPINFOHEADER)pv);

	if (lpbi->biClrUsed != 0)
		return (WORD)lpbi->biClrUsed;
    bits = lpbi->biBitCount;

    switch (bits){
        case 1:
                return 2;
        case 4:
                return 16;
        case 8:
                return 256;
        default:
                /* A 24 bitcount DIB has no color table */
                return 0;
    }
}

static WORD PaletteSize (VOID FAR * pv)
{
    LPBITMAPINFOHEADER lpbi;
    WORD               NumColors;

    lpbi      = (LPBITMAPINFOHEADER)pv;
    NumColors = DibNumColors(lpbi);

    return (WORD)(NumColors * sizeof(RGBQUAD));
}

static HBITMAP BitmapFromDib (
    HANDLE         hdib,
    HPALETTE   hpal)
{
    LPBITMAPINFOHEADER  lpbi;
    HPALETTE            hpalT;
    HDC                 hdc;
    HBITMAP             hbm;


    if (!hdib)
        return NULL;

    lpbi = (LPBITMAPINFOHEADER)GlobalLock(hdib);

    if (!lpbi)
        return NULL;

    hdc = GetDC(NULL);

    if (hpal){
        hpalT = SelectPalette(hdc,hpal,FALSE);
        RealizePalette(hdc);     // GDI Bug...????
    }

    hbm = CreateDIBitmap(hdc,
                (LPBITMAPINFOHEADER)lpbi,
                (LONG)CBM_INIT,
                (LPSTR)lpbi + lpbi->biSize + PaletteSize(lpbi),
                (LPBITMAPINFO)lpbi,
                DIB_RGB_COLORS );

    if (hpal)
        SelectPalette(hdc,hpalT,FALSE);

    ReleaseDC(NULL,hdc);
    GlobalUnlock(hdib);
    return hbm;
}

static HANDLE readDibFromMemory(LPVOID pDibSrc)
{
    LPBITMAPINFOHEADER pbmih = (LPBITMAPINFOHEADER)pDibSrc;

    DWORD totalSize = pbmih->biSize 
                        + pbmih->biSizeImage
                            + (DWORD)PaletteSize(pbmih);

    HANDLE hdib = GlobalAlloc(GHND, totalSize);
    LPVOID pDibDst = GlobalLock(hdib);

    CopyMemory(pDibDst, pDibSrc, totalSize);

    GlobalUnlock(hdib);
    return hdib;
}


/////////////////////////////////////////////////////////////////////////////
// CArxBlockListCtrl

CArxBlockListCtrl::CArxBlockListCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CBlockListCtrl( pTemplate, pPane, nID, false )
, mArxServices( this )
, mDragDropService( this )
, mDocReactor( this )
, mEdReactor( this )
{
	m_pLoadedDwg = NULL;
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CArxBlockListCtrl::~CArxBlockListCtrl()
{
}

bool CArxBlockListCtrl::Create( CWnd* pParentWnd, UINT nID )
{
	bool bSuccess = __super::Create( pParentWnd, nID );

	if( bSuccess )
		RefreshBlockList();

	return bSuccess;
}

int CArxBlockListCtrl::extractPreview(const AcDbBlockTableRecord* pBTR)
{
	if (!pBTR->hasPreviewIcon())
		return 0; 

	int ret = 0;

	if (pBTR->hasPreviewIcon())
	{
		AcDbBlockTableRecord::PreviewIcon previewIcon;
		pBTR->getPreviewIcon(previewIcon);
		HANDLE hDib = readDibFromMemory((LPVOID)previewIcon.asArrayPtr());
		HBITMAP hBitmap = BitmapFromDib(hDib,NULL);
		if(hBitmap) 
		{
			CBitmap* pbmpImage = CBitmap::FromHandle(hBitmap);
			GetImageList(LVSIL_SMALL)->Add(pbmpImage, RGB(255,0,255));
			ret = GetImageList(LVSIL_NORMAL)->Add(pbmpImage, RGB(255,0,255));
			::DeleteObject(hBitmap);
		}
		if(hDib)
			GlobalFree(hDib);
	}
	return ret;
}

bool CArxBlockListCtrl::LoadDwg(CString sFileName)
{
	try
	{
		CString sPath = theWorkspace.FindFile( sFileName ); 
		if( sPath.IsEmpty() )
		{				
			CString sLoadString;
			sLoadString = theWorkspace.LoadResourceString(IDS_DWGNOTLOADING);
			acedAlert(sLoadString + sFileName);
			return FALSE;
		}

		// delete all the items
		DeleteAllItems();
		
		if (GetHeaderCtrl() != NULL)
		{
			// get the column count
			int nColumnCount = GetHeaderCtrl()->GetItemCount();

			// Delete all of the columns.
			for (int i=0;i < nColumnCount;i++)
			{
			   DeleteColumn(0);
			}
		}

		// if a drawing has been previously loaded
		if (m_pLoadedDwg)
		{
			// delete it.
			delete m_pLoadedDwg;
			m_pLoadedDwg = NULL;
		}

		Acad::ErrorStatus es;


		// This database is used to open the user specified file into if it exists
		m_pLoadedDwg = new AcDbDatabase(false, true);
		
		// Try to open the user specified file
		if (Acad::eOk != (es = m_pLoadedDwg->readDwgFile(sPath,_SH_DENYNO,false)))
		{
			CString sLoadString;
			sLoadString = theWorkspace.LoadResourceString(IDS_DWGNOTLOADING);
			acedAlert(sLoadString + sFileName);
			delete m_pLoadedDwg;					
			m_pLoadedDwg = NULL;
			return false;
		}
		
		if (m_pLoadedDwg==NULL)
			return false;			
		m_pLoadedDwg->closeInput( true );
	}
	catch(...)
	{
	}
	
	// now call the method to display the blocks in this loaded dwg
	RefreshBlockList();
	return true;			
}

void CArxBlockListCtrl::RefreshBlockList() 
{
	if( !m_hWnd )
		return;

	DeleteAllItems();
	
	if (acdbHostApplicationServices()->workingDatabase() == NULL)
		return;

	AcDbBlockTable *pBlockTable;

	if (m_pLoadedDwg == NULL)
		acdbHostApplicationServices()->workingDatabase()->getSymbolTable(pBlockTable, AcDb::kForRead);
	else
		m_pLoadedDwg->getSymbolTable(pBlockTable, AcDb::kForRead);

	bool bVisible = (IsWindowVisible() != FALSE);
	if( bVisible )
		SetRedraw(FALSE); // turn drawing off regardless of list mode

	// Iterate through the block table and disaply the names in the ListCtrl.
	const TCHAR *pName;
	int nRet = -1;
	AcDbBlockTableIterator *pBTItr;
	if (pBlockTable->newIterator(pBTItr) == Acad::eOk) 
	{
		while (!pBTItr->done()) 
		{
			AcDbBlockTableRecord *pRecord;
			if (pBTItr->getRecord(pRecord, AcDb::kForRead) == Acad::eOk)
			{
				pRecord->getName(pName);
				CString sBlockName = pName;
				if (sBlockName.Left(1) != _T("*") &&
					sBlockName.Left(1) != _T("~") &&
					sBlockName != CString() &&
					!pRecord->isAnonymous())
				{		
					int nCount = GetItemCount();
					int nPreviewIndex = extractPreview(pRecord);
					
					LVITEM lvItem;
					lvItem.mask = LVIF_TEXT|LVIF_IMAGE|LVIF_INDENT;
					lvItem.iItem = nCount;
					lvItem.iSubItem = 0;
					TCHAR sValue[256];
					_tcscpy(sValue, sBlockName);
					lvItem.pszText = sValue;
					lvItem.iImage = nPreviewIndex;
					lvItem.iIndent = -1;

					nRet = InsertItem(&lvItem);
				}
				pRecord->close();
			}
			pBTItr->step();
		}
	}
	pBlockTable->close();

	if( bVisible )
		SetRedraw(TRUE); // turn drawing back on and update the window
	OnNeedRepaint();
	UpdateWindow(); 
}


BEGIN_MESSAGE_MAP(CArxBlockListCtrl, CBlockListCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_NOTIFY_REFLECT(NM_CLICK, OnClick)
	ON_NOTIFY_REFLECT(NM_KILLFOCUS, OnKillfocus)
	ON_NOTIFY_REFLECT(NM_RCLICK, OnRclick)
	ON_NOTIFY_REFLECT(NM_RDBLCLK, OnRdblclk)
	ON_NOTIFY_REFLECT(NM_RETURN, OnReturn)
	ON_NOTIFY_REFLECT(NM_SETFOCUS, OnSetfocus)
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnEndlabeledit)
	ON_NOTIFY_REFLECT(LVN_BEGINLABELEDIT, OnBeginlabeledit)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnclick)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_CONTEXTMENU()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CArxBlockListCtrl message handlers

void CArxBlockListCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	__super::OnLButtonDown(nFlags, point);
	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventMouseDown),
		1,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());
/*
	bool bDrag = mpTemplate->GetBooleanProperty(Prop::DragnDropAllowBegin);
	if (!bDrag) //dragging not allowed
		return;

	// a -1 will be returned if not found
	if (mpTemplate->GetLongProperty(Prop::ListViewStyle) > -1)
		BeginDragnDrop(mpTemplate, point, IsAsyncEvents());
	else
	{
		CStringArray saBlockNames;
		saBlockNames.SetSize(1,1);
		POSITION pos = GetFirstSelectedItemPosition();
		if (pos)
		{
			while (pos)
				saBlockNames.Add(GetItemText(GetNextSelectedItem(pos), 0));
			//!CHANGED! 10-5-04 SRM
			//didnt allow user to drag and drop blocks from external dwgs
			//if (m_pLoadedDwg == NULL)
			BeginDragnDropToInsertBlocks(mpTemplate, point, IsAsyncEvents(), saBlockNames);
		}
	}
*/
}

void CArxBlockListCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventMouseUp),
		1,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());
	__super::OnLButtonUp(nFlags, point);
}

void CArxBlockListCtrl::OnClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* plvdi = (LV_DISPINFO*)pNMHDR;
	int nItem = plvdi->item.iItem;
	if( nItem < GetItemCount() && nItem >= 0 )
	{
		InvokeMethodInt(
			mpTemplate->GetStringProperty( Prop::EventClicked ),  
			nItem, 
			IsAsyncEvents());			
	}
	*pResult = 0;
}

void CArxBlockListCtrl::OnKillfocus(NMHDR* pNMHDR, LRESULT* pResult) 
{
	InvokeMethod(mpTemplate->GetStringProperty(Prop::EventKillFocus), IsAsyncEvents());
	*pResult = 0;
}

void CArxBlockListCtrl::OnRclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* plvdi = (LV_DISPINFO*)pNMHDR;
	int nItem = plvdi->item.iItem;
	if( nItem < GetItemCount() && nItem >= 0 )
	{
		InvokeMethodInt(
			mpTemplate->GetStringProperty( Prop::EventRClick ),  
			nItem, 
			IsAsyncEvents());			
	}
	*pResult = 0;
}

void CArxBlockListCtrl::OnRdblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* plvdi = (LV_DISPINFO*)pNMHDR;
	int nItem = plvdi->item.iItem;
	if( nItem < GetItemCount() && nItem >= 0 )
	{
		InvokeMethodInt(
			mpTemplate->GetStringProperty( Prop::EventRDblClick ),  
			nItem, 
			IsAsyncEvents());			
	}
	*pResult = 0;
}

void CArxBlockListCtrl::OnReturn(NMHDR* pNMHDR, LRESULT* pResult) 
{
	InvokeMethod(mpTemplate->GetStringProperty(Prop::EventReturnPressed), IsAsyncEvents());
	*pResult = 0;
}

void CArxBlockListCtrl::OnSetfocus(NMHDR* pNMHDR, LRESULT* pResult) 
{
	InvokeMethod(mpTemplate->GetStringProperty(Prop::EventSetFocus), IsAsyncEvents());
	*pResult = 0;
}

void CArxBlockListCtrl::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	LV_ITEM		*plvItem = &pDispInfo->item;

	if (pDispInfo->item.iItem >= 0)
	{
		InvokeMethodStringIntInt(
			mpTemplate->GetStringProperty(Prop::EventEndLabelEdit),
			plvItem->pszText,
			plvItem->iItem,
			plvItem->iSubItem,
			IsAsyncEvents());
	}
	__super::OnEndlabeledit( pNMHDR, pResult );
	*pResult = 0;
}

void CArxBlockListCtrl::OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	InvokeMethodIntInt(		
		mpTemplate->GetStringProperty(Prop::EventBeginLabelEdit),
		pDispInfo->item.iItem,
		GetCurrentSubItem(),
		IsAsyncEvents());
	*pResult = 0;
}

void CArxBlockListCtrl::OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	InvokeMethodInt(mpTemplate->GetStringProperty(Prop::EventColumnClick), pNMListView->iSubItem, IsAsyncEvents());
	*pResult = 0;
}

void CArxBlockListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	__super::OnLButtonDblClk(nFlags, point);

	int nRow = -1;
	POSITION pos = GetFirstSelectedItemPosition();
	if( pos )
		nRow = GetNextSelectedItem( pos );			
	
	InvokeMethodInt(
		mpTemplate->GetStringProperty( Prop::EventDblClicked ),  
		nRow, 
		IsAsyncEvents());
}

void CArxBlockListCtrl::OnMButtonDown(UINT nFlags, CPoint point) 
{
	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventMouseDown),
		4,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());
	__super::OnMButtonDown(nFlags, point);
}

void CArxBlockListCtrl::OnMButtonUp(UINT nFlags, CPoint point) 
{
	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventMouseUp),
		4,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());
	__super::OnMButtonUp(nFlags, point);
}

void CArxBlockListCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{
	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventMouseDown),
		2,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());
	__super::OnRButtonDown(nFlags, point);
}

void CArxBlockListCtrl::OnRButtonUp(UINT nFlags, CPoint point) 
{
	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventMouseUp),
		2,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());
	__super::OnRButtonUp(nFlags, point);
}

void CArxBlockListCtrl::OnContextMenu( CWnd* pTarget, CPoint point )
{
	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventMouseDown),
		2,
		MK_RBUTTON,
		point.x,
		point.y,
		IsAsyncEvents());
	__super::OnContextMenu(pTarget, point);
}

void CArxBlockListCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	TCHAR sChar = nChar;
	InvokeMethodStringIntInt(mpTemplate->GetStringProperty(Prop::EventKeyDown), sChar, nRepCnt, nFlags, IsAsyncEvents());
	__super::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CArxBlockListCtrl::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	TCHAR sChar = nChar;
	InvokeMethodStringIntInt(mpTemplate->GetStringProperty(Prop::EventKeyUp), sChar, nRepCnt, nFlags, IsAsyncEvents());
	__super::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CArxBlockListCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	InvokeMethodIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventMouseMove),
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());
	__super::OnMouseMove(nFlags, point);
}
