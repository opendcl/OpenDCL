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
	HPALETTE            hpalT = NULL;
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
, mBlockInsertDropTarget( this )
, mbNoRefresh( false )
, mpLoadedDwg( NULL )
, mDocReactor( this )
, mEdReactor( this )
{
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

DROPEFFECT CArxBlockListCtrl::OnBeginDrag( const CPoint& point, COleDataSource& SourceData )
{
	DROPEFFECT dwEffect = __super::OnBeginDrag( point, SourceData );
	AcDbDatabase* pDb = mpLoadedDwg;
	if( !pDb )
		pDb = acdbHostApplicationServices()->workingDatabase();
	if( !pDb )
		return dwEffect;
	UINT nFlags = 0;
	int idxItem = HitTest( point, &nFlags );
	if( idxItem < 0 )
		return dwEffect;
	CString sText = GetItemText( idxItem, 0 );
	if( sText.IsEmpty() )
		return dwEffect;
	AcDbBlockTable* pBlockTable = NULL;
	if( Acad::eOk != pDb->getBlockTable( pBlockTable, AcDb::kForRead ) )
		return dwEffect;
	AcDbObjectId idBlock;
	Acad::ErrorStatus es = pBlockTable->getAt( sText, idBlock );
	pBlockTable->close();
	if( es != Acad::eOk )
		return dwEffect;
	UINT CF_DclBlockRecId = CAcadBlockInsertDropTarget::GetAcadBlockClipboardFormat();
	HGLOBAL hDclBlockRecIdPtr = GlobalAlloc( GHND, sizeof(AcDbObjectId) );
	if( !hDclBlockRecIdPtr )
		return dwEffect;
	*(AcDbObjectId*)GlobalLock( hDclBlockRecIdPtr ) = idBlock;
	GlobalUnlock( hDclBlockRecIdPtr );
	SourceData.CacheGlobalData( CF_DclBlockRecId, hDclBlockRecIdPtr );
	return DROPEFFECT_COPY;
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

bool CArxBlockListCtrl::LoadDwg( LPCTSTR pszFilename )
{
	if( mpLoadedDwg )
	{
		try
		{
			delete mpLoadedDwg;
		}
		catch( ... ) {}
		mpLoadedDwg = NULL;
		OnNeedRepaint( true );
	}

	if( pszFilename && *pszFilename )
	{
		CString sPath = theWorkspace.FindFile( pszFilename );
		if( sPath.IsEmpty() )
		{
			CString sMsg;
			sMsg.Format( theWorkspace.LoadResourceString( IDS_DWGNOTLOADING ), pszFilename );
			theWorkspace.DisplayAlert( sMsg );
			return false;
		}

		mpLoadedDwg = new AcDbDatabase( false, true );
		Acad::ErrorStatus es = mpLoadedDwg->readDwgFile( sPath );
		if( es != Acad::eOk )
		{
			CString sMsg;
			sMsg.Format( theWorkspace.LoadResourceString( IDS_DWGNOTLOADING ), pszFilename );
			theWorkspace.DisplayAlert( sMsg );
			try
			{
				delete mpLoadedDwg;
			}
			catch( ... ) {}
			mpLoadedDwg = NULL;
			return false;
		}
		mpLoadedDwg->closeInput( true );
	}

	RefreshBlockList();
	return true;
}

LPCTSTR CArxBlockListCtrl::GetLoadedDwg() const
{
	if( !mpLoadedDwg )
		return NULL;
	const ACHAR* pszFilename;
	if( Acad::eOk == mpLoadedDwg->getFilename( pszFilename ) )
		return pszFilename;
	return NULL;
}

void CArxBlockListCtrl::RefreshBlockList()
{
	if( mbNoRefresh )
		return;
	if( !m_hWnd )
		return;

	CString sCurSel;
	POSITION posSel = GetFirstSelectedItemPosition();
	if( posSel )
		sCurSel = GetItemText( GetNextSelectedItem( posSel ), 0 );
	DeleteAllItems();
	GetImageList(LVSIL_SMALL)->SetImageCount( 1 );
	GetImageList(LVSIL_NORMAL)->SetImageCount( 1 );
	CHeaderCtrl* pHeader = GetHeaderCtrl();
	if( pHeader )
	{
		for( int ctColumn = pHeader->GetItemCount(); ctColumn > 0 ; --ctColumn )
			DeleteColumn( 0 );
	}

	AcDbBlockTable* pBlockTable = NULL;
	if( mpLoadedDwg )
		mpLoadedDwg->getSymbolTable( pBlockTable, AcDb::kForRead );
	else
	{
		AcDbDatabase* pDb = acdbHostApplicationServices()->workingDatabase();
		if( pDb )
			pDb->getSymbolTable( pBlockTable, AcDb::kForRead );
	}
	if( !pBlockTable )
		return;

	bool bVisible = (IsWindowVisible() != FALSE);
	if( bVisible )
		SetRedraw( FALSE ); // turn drawing off regardless of list mode

	// Iterate through the block table and display the names in the ListCtrl.
	AcDbBlockTableIterator* pBTItr;
	Acad::ErrorStatus es = pBlockTable->newIterator( pBTItr );
	pBlockTable->close();
	if( es == Acad::eOk )
	{
		while( !pBTItr->done() )
		{
			AcDbBlockTableRecord* pRecord;
			if( pBTItr->getRecord( pRecord, AcDb::kForRead ) == Acad::eOk )
			{
				if( !pRecord->isAnonymous() &&
						!pRecord->isLayout() &&
						!pRecord->isFromExternalReference() )
				{
					const ACHAR* pszName;
					pRecord->getName( pszName );
					int nPreviewIndex = extractPreview( pRecord );
					CString sName( pszName );
					LVITEM lvItem =
					{ LVIF_TEXT | LVIF_IMAGE | LVIF_INDENT,
						GetItemCount(),
						0,
						0,
						0,
						sName.LockBuffer(),
						-1,
						nPreviewIndex,
						0,
						-1,
					};
					int idxItem = InsertItem( &lvItem );
					if( sCurSel == pszName )
					{
						SetItemState( idxItem, LVIS_SELECTED, LVIS_SELECTED );
						EnsureVisible( idxItem, FALSE );
					}
				}
				pRecord->close();
			}
			pBTItr->step();
		}
		delete pBTItr;
	}

	if( bVisible )
		SetRedraw( TRUE ); // turn drawing back on and update the window
	OnNeedRepaint( true, true );
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
	mbNoRefresh = true;
	__super::OnLButtonDown(nFlags, point);
	mbNoRefresh = false;
	GetArxServices()->HandleEvent( Prop::EventMouseDown, args_NNNN( 1, nFlags, point.x, point.y ) );
}

void CArxBlockListCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	GetArxServices()->HandleEvent( Prop::EventMouseUp, args_NNNN( 1, nFlags, point.x, point.y ) );
	__super::OnLButtonUp(nFlags, point);
}

void CArxBlockListCtrl::OnClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMITEMACTIVATE* plvia = (NMITEMACTIVATE*)pNMHDR;
	int nItem = plvia->iItem;
	if( nItem < GetItemCount() && nItem >= 0 )
		GetArxServices()->HandleEvent( Prop::EventClicked, args_N( nItem ) );
	*pResult = 0;
}

void CArxBlockListCtrl::OnKillfocus(NMHDR* pNMHDR, LRESULT* pResult)
{
	GetArxServices()->HandleEvent( Prop::EventKillFocus );
	*pResult = 0;
}

void CArxBlockListCtrl::OnRclick(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMITEMACTIVATE* plvia = (NMITEMACTIVATE*)pNMHDR;
	int nItem = plvia->iItem;
	if( nItem < GetItemCount() && nItem >= 0 )
		GetArxServices()->HandleEvent( Prop::EventRightClick, args_N( nItem ) );
	*pResult = 0;
}

void CArxBlockListCtrl::OnRdblclk(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMITEMACTIVATE* plvia = (NMITEMACTIVATE*)pNMHDR;
	int nItem = plvia->iItem;
	if( nItem < GetItemCount() && nItem >= 0 )
		GetArxServices()->HandleEvent( Prop::EventRightDblClick, args_N( nItem ) );
	*pResult = 0;
}

void CArxBlockListCtrl::OnReturn(NMHDR* pNMHDR, LRESULT* pResult)
{
	GetArxServices()->HandleEvent( Prop::EventReturnPressed );
	*pResult = 0;
}

void CArxBlockListCtrl::OnSetfocus(NMHDR* pNMHDR, LRESULT* pResult)
{
	GetArxServices()->HandleEvent( Prop::EventSetFocus );
	*pResult = 0;
}

void CArxBlockListCtrl::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult)
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	LV_ITEM* plvItem = &pDispInfo->item;

	if( pDispInfo->item.iItem >= 0 )
	{
		CString sLabel = plvItem->pszText? plvItem->pszText : GetItemText( plvItem->iItem, 0 );
		GetArxServices()->HandleEvent( Prop::EventEndLabelEdit, args_SN( sLabel, plvItem->iItem ) );
	}
	*pResult = TRUE;
}

void CArxBlockListCtrl::OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult)
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	GetArxServices()->HandleEvent( Prop::EventBeginLabelEdit, args_N( pDispInfo->item.iItem ) );
	*pResult = 0;
}

void CArxBlockListCtrl::OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	GetArxServices()->HandleEvent( Prop::EventColumnClick, args_N( pNMListView->iSubItem ) );
	*pResult = 0;
}

void CArxBlockListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	__super::OnLButtonDblClk(nFlags, point);

	int nRow = -1;
	POSITION pos = GetFirstSelectedItemPosition();
	if( pos )
		nRow = GetNextSelectedItem( pos );
	GetArxServices()->HandleEvent( Prop::EventDblClicked, args_N( nRow ) );
}

void CArxBlockListCtrl::OnMButtonDown(UINT nFlags, CPoint point)
{
	GetArxServices()->HandleEvent( Prop::EventMouseDown, args_NNNN( 4, nFlags, point.x, point.y ) );
	__super::OnMButtonDown(nFlags, point);
}

void CArxBlockListCtrl::OnMButtonUp(UINT nFlags, CPoint point)
{
	GetArxServices()->HandleEvent( Prop::EventMouseUp, args_NNNN( 4, nFlags, point.x, point.y ) );
	__super::OnMButtonUp(nFlags, point);
}

void CArxBlockListCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
	GetArxServices()->HandleEvent( Prop::EventMouseDown, args_NNNN( 2, nFlags, point.x, point.y ) );
	__super::OnRButtonDown(nFlags, point);
}

void CArxBlockListCtrl::OnRButtonUp(UINT nFlags, CPoint point)
{
	GetArxServices()->HandleEvent( Prop::EventMouseUp, args_NNNN( 2, nFlags, point.x, point.y ) );
	__super::OnRButtonUp(nFlags, point);
}

void CArxBlockListCtrl::OnContextMenu( CWnd* pTarget, CPoint point )
{
	GetArxServices()->HandleEvent( Prop::EventMouseDown, args_NNNN( 2, MK_RBUTTON, point.x, point.y ) );
	__super::OnContextMenu(pTarget, point);
}

void CArxBlockListCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	GetArxServices()->HandleEvent( Prop::EventKeyDown, args_CNN( (TCHAR)nChar, nRepCnt, nFlags ) );
	__super::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CArxBlockListCtrl::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	GetArxServices()->HandleEvent( Prop::EventKeyUp, args_CNN( (TCHAR)nChar, nRepCnt, nFlags ) );
	__super::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CArxBlockListCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	GetArxServices()->HandleEvent( Prop::EventMouseMove, args_NNN( nFlags, point.x, point.y ) );
	__super::OnMouseMove(nFlags, point);
}
