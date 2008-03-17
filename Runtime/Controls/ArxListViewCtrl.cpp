// ArxListViewCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ArxListViewCtrl.h"
#include "DclControlObject.h"
#include "PropertyObject.h"
#include "PropertyIds.h"
#include "Resource.h"
#include "AcadBlockReactor.h"
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
// CArxListViewCtrl

CArxListViewCtrl::CArxListViewCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CListViewCtrl( pTemplate, pPane, nID, false )
, mArxServices( this )
, mDragDropService( this )
, mpDocReactor( NULL )
, mpBlockReactor( NULL )
{
	m_pLoadedDwg = NULL;
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CArxListViewCtrl::~CArxListViewCtrl()
{
	delete mpBlockReactor;
	delete mpDocReactor;
}

bool CArxListViewCtrl::Create( CWnd* pParentWnd, UINT nID )
{
	bool bSuccess = __super::Create( pParentWnd, nID );

	if( bSuccess && IsBlockList() )
	{
		mpBlockReactor = new CAcadBlockReactor( this );
		acedEditor->addReactor( mpBlockReactor );
		mpDocReactor = new CDocReactor( this );
		RefreshBlockList();
	}

	return bSuccess;
}

void CArxListViewCtrl::RefreshBlockList() 
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
	
int CArxListViewCtrl::extractPreview(const AcDbBlockTableRecord* pBTR)
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

bool CArxListViewCtrl::LoadDwg(CString sFileName)
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
			m_FileName = CString();
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
		// set the file name for future use
		m_FileName = sPath;
		
		if (m_pLoadedDwg==NULL)
			return false;			
	}
	catch(...)
	{
	}
	
	// now call the method to display the blocks in this loaded dwg
	RefreshBlockList();
	return true;			
}


BEGIN_MESSAGE_MAP(CArxListViewCtrl, CListCtrl)
	ON_WM_DESTROY()
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
// CArxListViewCtrl message handlers

void CArxListViewCtrl::OnDestroy() 
{
	if( mpBlockReactor )
	{
		acedEditor->removeReactor( mpBlockReactor );		
		delete mpBlockReactor;
		mpBlockReactor = NULL;
	}
	__super::OnDestroy();
}

void CArxListViewCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
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

void CArxListViewCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
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

void CArxListViewCtrl::OnClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* plvdi = (LV_DISPINFO*)pNMHDR;
	
	int nStyle = mpTemplate->GetLongProperty(Prop::ListViewStyle);

	if (plvdi->item.mask < GetItemCount() && plvdi->item.mask >= 0)
	{
		InvokeMethodIntInt(
			mpTemplate->GetStringProperty(Prop::EventClicked),  
			plvdi->item.mask, 
			plvdi->item.iItem, 
			IsAsyncEvents());			
	}
	*pResult = 0;
}

void CArxListViewCtrl::OnKillfocus(NMHDR* pNMHDR, LRESULT* pResult) 
{
	InvokeMethod(mpTemplate->GetStringProperty(Prop::EventKillFocus), IsAsyncEvents());
	*pResult = 0;
}

void CArxListViewCtrl::OnRclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* plvdi = (LV_DISPINFO*)pNMHDR;
	if (plvdi->item.mask < GetItemCount() && plvdi->item.mask >= 0)
	{
		InvokeMethodInt(
			mpTemplate->GetStringProperty(Prop::EventRClick),  
			plvdi->item.mask, 
			IsAsyncEvents());
	}
	*pResult = 0;
}

void CArxListViewCtrl::OnRdblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* plvdi = (LV_DISPINFO*)pNMHDR;

	if (plvdi->item.mask < GetItemCount() && plvdi->item.mask >= 0)
	{
		InvokeMethodInt(
			mpTemplate->GetStringProperty(Prop::EventRDblClick),  
			plvdi->item.mask, 
			IsAsyncEvents());
	}
	*pResult = 0;
}

void CArxListViewCtrl::OnReturn(NMHDR* pNMHDR, LRESULT* pResult) 
{
	InvokeMethod(mpTemplate->GetStringProperty(Prop::EventReturnPressed), IsAsyncEvents());
	*pResult = 0;
}

void CArxListViewCtrl::OnSetfocus(NMHDR* pNMHDR, LRESULT* pResult) 
{
	InvokeMethod(mpTemplate->GetStringProperty(Prop::EventSetFocus), IsAsyncEvents());
	*pResult = 0;
}

void CArxListViewCtrl::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	LV_ITEM		*plvItem = &pDispInfo->item;

	if (pDispInfo->item.mask > 0)
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

void CArxListViewCtrl::OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	InvokeMethodIntInt(		
		mpTemplate->GetStringProperty(Prop::EventBeginLabelEdit),
		pDispInfo->item.iItem,
		m_nEditSubItem,
		IsAsyncEvents());
	*pResult = 0;
}

void CArxListViewCtrl::OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	InvokeMethodInt(mpTemplate->GetStringProperty(Prop::EventColumnClick), pNMListView->iSubItem, IsAsyncEvents());
	*pResult = 0;
}

void CArxListViewCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	__super::OnLButtonDblClk(nFlags, point);

	int nRow = -1, nCol = -1;
	POSITION pos = GetFirstSelectedItemPosition();
	if (pos == NULL)
	{
		if (mpTemplate->GetLongProperty(Prop::ListViewStyle) > -1)
		{
			LVHITTESTINFO lvhti;
			lvhti.pt = point;
			SubItemHitTest(&lvhti);
			nRow = lvhti.iItem;
			if (nRow > -1)
				nCol = lvhti.iSubItem;
		}
	}
	else
	{
		nRow = GetNextSelectedItem(pos);			
		nCol = 0;
	}
	
	if (mpTemplate->GetLongProperty(Prop::ListViewStyle) > -1)
		InvokeMethodIntInt(
			mpTemplate->GetStringProperty(Prop::EventDblClicked),  
			nRow, 
			nCol, 
			IsAsyncEvents());
	else				
		InvokeMethodInt(
			mpTemplate->GetStringProperty(Prop::EventDblClicked),  
			nRow, 
			IsAsyncEvents());
}

void CArxListViewCtrl::OnMButtonDown(UINT nFlags, CPoint point) 
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

void CArxListViewCtrl::OnMButtonUp(UINT nFlags, CPoint point) 
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

void CArxListViewCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
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

void CArxListViewCtrl::OnRButtonUp(UINT nFlags, CPoint point) 
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

void CArxListViewCtrl::OnContextMenu( CWnd* pTarget, CPoint point )
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

void CArxListViewCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	TCHAR sChar = nChar;
	InvokeMethodStringIntInt(mpTemplate->GetStringProperty(Prop::EventKeyDown), sChar, nRepCnt, nFlags, IsAsyncEvents());
	__super::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CArxListViewCtrl::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	TCHAR sChar = nChar;
	InvokeMethodStringIntInt(mpTemplate->GetStringProperty(Prop::EventKeyUp), sChar, nRepCnt, nFlags, IsAsyncEvents());
	__super::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CArxListViewCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	InvokeMethodIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventMouseMove),
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());
	__super::OnMouseMove(nFlags, point);
}
