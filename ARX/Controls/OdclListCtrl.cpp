// OdclListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "OdclListCtrl.h"
#include "DclControlObject.h"
#include "PropertyObject.h"
#include "PropertyIds.h"
#include "Resource.h"
#include "AcadDocReactor.h"
#include "AcadBlockReactor.h"
#include "AcadColorTable.h"
#include "InvokeMethod.h"
#include "ToolTips.h"
#include "Workspace.h"
#include "ControlPane.h"


/////////////////////////////////////////////////////////////////////////////
// CLVEdit

BEGIN_MESSAGE_MAP(CLVEdit, CEdit)
	//{{AFX_MSG_MAP(CLVEdit)
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_SHOWWINDOW()
	ON_WM_MOVE()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	ON_CONTROL_REFLECT(EN_SETFOCUS, OnSetfocus)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CLVEdit::OnWindowPosChanging(WINDOWPOS FAR* lpwndpos) 
{
	m_rcOldPos.SetRect(lpwndpos->x, lpwndpos->y, lpwndpos->cx + lpwndpos->x, lpwndpos->cy + lpwndpos->y);
	lpwndpos->x=m_x;
	lpwndpos->y=m_y;
	CEdit::OnWindowPosChanging(lpwndpos);
	m_pParent->InvalidateRect(m_rcOldPos);
}
void CLVEdit::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CEdit::OnShowWindow(bShow, nStatus);
}

void CLVEdit::OnMove(int x, int y) 
{
	CEdit::OnMove(x, y);
}
void CLVEdit::OnSize(UINT nType, int cx, int cy) 
{
	CEdit::OnSize(nType, cx, cy);
}

void CLVEdit::OnSetfocus() 
{
	CRect rcThis(m_x, m_y, m_x+ m_rcOldPos.Width(), m_y + m_rcOldPos.Height());
	MoveWindow(m_rcOldPos, TRUE);
	MoveWindow(rcThis, TRUE);
	GetParent()->InvalidateRect(m_rcOldPos);
	m_pParent->InvalidateRect(m_rcOldPos);
}


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
// OdclListCtrl

OdclListCtrl::OdclListCtrl( CControlPane& Pane, CDclControlObject* pTemplate, UINT nID )
: CArxDialogControl( pTemplate, &Pane, this )
, mbBlockList( false )
{
	m_pDocToModReactor = NULL;
	m_pBlockReactor = NULL;
	
	// No tooltip created
	mToolTip.m_hWnd = NULL;

	m_pLoadedDwg = NULL;
	m_FileName = CString();

	Create( Pane.GetHostDialog(), nID );
}

OdclListCtrl::~OdclListCtrl()
{
	if (m_pDocToModReactor != NULL)
		delete m_pDocToModReactor;
	if (m_pBlockReactor != NULL)
		delete m_pBlockReactor;
}

bool OdclListCtrl::Create( CWnd* pParentWnd, UINT nID )
{
	bool bSuccess = (CListCtrl::Create( GetWndStyle(), GetWndRect(), pParentWnd, nID ) != FALSE);

	RefCountedPtr< CImageListObject > pImageList = mpTemplate->GetImageList();
	if( pImageList )
	{
		pImageList->m_ImageList.SetBkColor( GetBkColor() );
		SetImageList( &pImageList->m_ImageList, TVSIL_NORMAL );
		SetImageList( &pImageList->m_ImageList, LVSIL_SMALL );
	}
	else
	{ //create a default image list
		if( !mDefaultImageList.m_hImageList )
			mDefaultImageList.Create( 1, mpTemplate->GetLngProperty( nRowHeight ), ILC_COLOR, 1, 1 );
		mDefaultImageList.SetBkColor( GetBkColor() );
		SetImageList( &mDefaultImageList, TVSIL_NORMAL );
		SetImageList( &mDefaultImageList, LVSIL_SMALL );
	}

	if( bSuccess && !ApplyPropertiesEnum() )
		bSuccess = false;

	if( IsBlockList() )
	{
		// create the new reactors
		m_pDocToModReactor = new CAcadDocReactor( this );
		m_pBlockReactor = new CAcadBlockReactor( this );

		// activate the reactors
		acDocManager->addReactor( m_pDocToModReactor );
		acedEditor->addReactor( m_pBlockReactor );

		// call the method to populate the block list control
		RefreshBlockList();
	}
	else
		SetExtendedStyle( GetExtendedStyle() | LVS_EX_SUBITEMIMAGES );

	SetAcadColor( mpTemplate->GetLngProperty( nAcadColor ) );

	InitToolTip();
	SetToolTipEx(this, mToolTip, mpTemplate);

	if( mpTemplate->GetLngProperty(nEventInvoke) == 1 )
		m_bInvokeWithSendString = true;
	else
		m_bInvokeWithSendString = false;

	return bSuccess;
}

bool OdclListCtrl::OnApplyProperty( RefCountedPtr< CPropertyObject > pProp )
{
	if( !__super::OnApplyProperty( pProp ) )
		return false;
	bool bFailed = false;
	switch( pProp->GetID() )
	{
	case nBlockListStyle:
		{
			mbBlockList = true;

			//create a default icon for blocks without a preview image
			mBlockViewImageList.Create( 32, 32, ILC_COLOR4, 1, 1 );
			mBlockViewImageList.SetBkColor( GetBkColor() );

			HICON hIcon = (HICON)::LoadImage( _hdllInstance, MAKEINTRESOURCE(IDI_LARGEBLOCK), IMAGE_ICON, 0, 0, 0 );
			mBlockViewImageList.Add( hIcon );
			DestroyIcon( hIcon );
			SetImageList( &mBlockViewImageList, LVSIL_NORMAL );
			SetImageList( &mBlockViewImageList, LVSIL_SMALL );
			break;
		}
	case nListViewStyle:
		{
			break;
		}
	case nIconXSpacing:
		{
			int nImageListIconSizeX = 0;
			if( mpTemplate->GetPropertyObject( nBlockListStyle ) )
				nImageListIconSizeX = 32;
			else
			{
				RefCountedPtr< CImageListObject > pImageList = mpTemplate->GetImageList();
				if( pImageList )
					nImageListIconSizeX = pImageList->m_ImageSize.cx;
			}
			CSize sizeIconSpacing( mpControl->SendMessage( LVM_GETITEMSPACING ) );
			sizeIconSpacing.cx = pProp->GetLongValue() + nImageListIconSizeX;
			SetIconSpacing( sizeIconSpacing );
			break;
		}
	case nIconYSpacing:
		{
			int nImageListIconSizeY = 0;
			if( mpTemplate->GetPropertyObject( nBlockListStyle ) )
				nImageListIconSizeY = 32;
			else
			{
				RefCountedPtr< CImageListObject > pImageList = mpTemplate->GetImageList();
				if( pImageList )
					nImageListIconSizeY = pImageList->m_ImageSize.cy;
			}
			CSize sizeIconSpacing( mpControl->SendMessage( LVM_GETITEMSPACING ) );
			sizeIconSpacing.cy = pProp->GetLongValue() + nImageListIconSizeY;
			SetIconSpacing( sizeIconSpacing );
			break;
		}
	case nGridLines:
		{
			if( pProp->GetBooleanValue() )
				SetExtendedStyle( GetExtendedStyle() | LVS_EX_GRIDLINES );
			else
				SetExtendedStyle( GetExtendedStyle() & ~LVS_EX_GRIDLINES );
			break;
		}
	case nFullRowSelect:
		{
			if( pProp->GetBooleanValue() )
				SetExtendedStyle( GetExtendedStyle() | LVS_EX_FULLROWSELECT );
			else
				SetExtendedStyle( GetExtendedStyle() & ~LVS_EX_FULLROWSELECT );
			break;
		}
	case nMultiSelect:
		{
			if( pProp->GetBooleanValue() )
				ModifyStyle( LVS_SINGLESEL, 0 );
			else
				ModifyStyle( 0, LVS_SINGLESEL );
			break;
		}
	case nColumnCaptions:
		{
			CHeaderCtrl* pHdrCtrl = GetHeaderCtrl();
			PropVal::TCStringArray* prsCaption = pProp->GetStringArrayPtr();
			size_t idx = prsCaption->size();
			while( idx > 0 )
			{
				CString sCaption = prsCaption->at(--idx);
				HDITEM hdi;
				hdi.mask = HDI_TEXT;
				hdi.pszText = sCaption.LockBuffer();
				if( !pHdrCtrl->SetItem( idx, &hdi ) )
					bFailed = true;
			}
			break;
		}
	case nColumnWidths:
		{
			CHeaderCtrl* pHdrCtrl = GetHeaderCtrl();
			PropVal::TIntArray* prInt = pProp->GetIntArrayPtr();
			size_t idx = prInt->size();
			while( idx > 0 )
			{
				HDITEM hdi;
				hdi.mask = HDI_WIDTH;
				hdi.cxy = prInt->at(--idx);
				if( !pHdrCtrl->SetItem( idx, &hdi ) )
					bFailed = true;
			}
			break;
		}
	case nColumnAlignments:
		{
			CHeaderCtrl* pHdrCtrl = GetHeaderCtrl();
			PropVal::TIntArray* prInt = pProp->GetIntArrayPtr();
			size_t idx = prInt->size();
			while( idx > 0 )
			{
				HDITEM hdi;
				hdi.mask = HDI_FORMAT;
				hdi.fmt = prInt->at(--idx);
				if( !pHdrCtrl->SetItem( idx, &hdi ) )
					bFailed = true;
			}
			break;
		}
	case nColumnImages:
		{
			CHeaderCtrl* pHdrCtrl = GetHeaderCtrl();
			PropVal::TIntArray* prInt = pProp->GetIntArrayPtr();
			size_t idx = prInt->size();
			while( idx > 0 )
			{
				HDITEM hdi;
				hdi.mask = HDI_IMAGE;
				hdi.iImage = prInt->at(--idx);
				if( !pHdrCtrl->SetItem( idx, &hdi ) )
					bFailed = true;
			}
			break;
		}
	}
	return !bFailed;
}

DWORD OdclListCtrl::GetWndStyle() const
{
	DWORD dwStyle = CArxDialogControl::GetWndStyle();

	switch( mpTemplate->GetLngProperty( nBlockListStyle ) ) //returns -1 if not found
	{
	case 0:
		dwStyle |= (LVS_ICON | LVS_NOCOLUMNHEADER);
		break;
	case 1:
		dwStyle |= (LVS_LIST| LVS_NOCOLUMNHEADER);
		break;
	case -1:
		{ //block list style property not found, so it is not a blockview list
			dwStyle |= LVS_SHAREIMAGELISTS; //normal listview always uses a shared image list
			LONG fListViewStyle = mpTemplate->GetLngProperty( nListViewStyle ); //returns -1 if not found
			switch( fListViewStyle )
			{
			case 0:
				dwStyle |= (LVS_ICON | LVS_NOCOLUMNHEADER);
				break;
			case 1:
				dwStyle |= (LVS_SMALLICON | LVS_NOCOLUMNHEADER);
				break;
			case 2:
				dwStyle |= (LVS_LIST | LVS_NOCOLUMNHEADER);
				break;
			default:
				dwStyle |= (LVS_REPORT);
				if( !mpTemplate->GetBoolProperty( nColHeader ) )
					dwStyle |= (LVS_NOCOLUMNHEADER);
				break;
			}
			if( fListViewStyle < 4 && mpTemplate->GetBoolProperty( nEditLabels ) )
				dwStyle |= (LVS_EDITLABELS);			
			break;
		}
	}

	switch( mpTemplate->GetLngProperty( nListViewSort ) )
	{
	case 1:
		dwStyle |= (LVS_SORTDESCENDING);
		break;
	case 2:
		dwStyle |= (LVS_SORTASCENDING);
		break;
	}
	
	switch( mpTemplate->GetLngProperty( nListViewIconAlign ) )
	{
	case 0:
		// I know that 0 = left in the property list box in the editor, but the top must be set so
		// the list scrolls top to bottom, this would make better sense to the user
		dwStyle |= (LVS_ALIGNTOP);
		break;
	case 1:
		// I know that 1 = top in the property list box in the editor, but the top must be set so
		// the list scrolls left to right, this would make better sense to the user
		dwStyle |= (LVS_ALIGNLEFT);
		break;
	}

	if( mpTemplate->GetBoolProperty( nAutoArrange ) )
		dwStyle |= (LVS_AUTOARRANGE);	
	if( !mpTemplate->GetBoolProperty( nLabelWrap ) )
		dwStyle |= (LVS_NOLABELWRAP);
	if( mpTemplate->GetBoolProperty( nShowSelectAlways ) )
		dwStyle |= (LVS_SHOWSELALWAYS);
	if( !mpTemplate->GetBoolProperty( nMultiSelect ) )
		dwStyle |= (LVS_SINGLESEL);

	return dwStyle;
}


BEGIN_MESSAGE_MAP(OdclListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(OdclListCtrl)
	ON_WM_DESTROY()
	ON_WM_SIZE()
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
	ON_NOTIFY_REFLECT(LVN_KEYDOWN, OnKeydown)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnclick)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_MOUSEMOVE()
	ON_NOTIFY_REFLECT(LVN_ODSTATECHANGED, OnOdstatechanged)
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// OdclListCtrl message handlers

CString _ltoa(int nVal)
{
	char value[80];
	_ltoa(nVal, value, 10);
	return value;
}

void OdclListCtrl::SetAcadColor(long nColor)
{
	COLORREF crBkColor = GetRGBColor(nColor);
	SetBkColor(crBkColor);
	SetTextBkColor(crBkColor);
	Invalidate();
}

void OdclListCtrl::OnDestroy() 
{
	// delete the tool tip text control object
	mToolTip.DelTool(this, 1);
	
	// delete all the items
	DeleteAllItems();
	
	SetImageList(NULL, TVSIL_NORMAL);
	SetImageList(NULL, LVSIL_SMALL);
	
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

	m_DropTarget.Revoke();
    
	if (m_pDocToModReactor != NULL)
	{
		acDocManager->removeReactor(m_pDocToModReactor);
	    delete m_pDocToModReactor;
		m_pDocToModReactor = NULL;
	}	

	if (m_pBlockReactor != NULL)
	{
		acedEditor->removeReactor(m_pBlockReactor);		
		delete m_pBlockReactor;
		m_pBlockReactor = NULL;
	}
	CListCtrl::OnDestroy();
}



const TCHAR sStar[] = _T("*");
const TCHAR sTidle[] = _T("~");

void OdclListCtrl::RefreshBlockList() 
{
	DeleteAllItems();
	
	if (acdbHostApplicationServices()->workingDatabase() == NULL)
		return;
	
	
	AcDbBlockTable *pBlockTable;

	if (m_pLoadedDwg == NULL)
		acdbHostApplicationServices()->workingDatabase()->getSymbolTable(pBlockTable, AcDb::kForRead);
	else
		m_pLoadedDwg->getSymbolTable(pBlockTable, AcDb::kForRead);

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
				if (sBlockName.Left(1) != sStar &&
					sBlockName.Left(1) != sTidle &&
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

	SetRedraw(TRUE); // turn drawing back on and update the window
	Invalidate();
	UpdateWindow(); 
}
	
int OdclListCtrl::extractPreview(const AcDbBlockTableRecord* pBTR)
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

void OdclListCtrl::OnSize(UINT nType, int cx, int cy) 
{
	CListCtrl::OnSize(nType, cx, cy);
	//Arrange(LVA_DEFAULT);
	//return;
	int nPropIconStyle = mpTemplate->GetLngProperty(nListViewStyle);
	if (nPropIconStyle == 0 || nPropIconStyle == 1 || mpTemplate->GetLngProperty(nBlockListStyle) > -1)
	{	
		GetWorkAreas(0, NULL);
		CRect rcThis;
		GetClientRect(&rcThis);
		CRect rcWorkArea[1];
		rcWorkArea[0].SetRect(0,0,rcThis.Width(), 36000);
		// set the work area
		SetWorkAreas(1, rcWorkArea);
	}
}

void OdclListCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
		/*
	BOOL bDrag = mpTemplate->GetBoolProperty(nDragnDropAllowBegin);

	if (bDrag == FALSE)
		CListCtrl::OnLButtonDown(nFlags, point);

	if (bDrag == TRUE && nFlags == 1)
	*/
	int nStyle = mpTemplate->GetLngProperty(nListViewStyle);
	LVHITTESTINFO lvhti;
	lvhti.pt = point;
	SubItemHitTest(&lvhti);
	
	if (GetItemCount() > 0)
	{	
		
		if (nStyle == 3)
		{
			SetItem(lvhti.iItem, lvhti.iSubItem, LVIF_STATE, NULL, 0, LVIS_SELECTED, LVIS_SELECTED, 0);		

			// call methods to invoke the event
			InvokeMethodIntInt(
				mpTemplate->GetStrProperty(nEventClicked),  
				lvhti.iItem,
				lvhti.iSubItem,
				m_bInvokeWithSendString);
		}
		else
		{
			SetItem(lvhti.iItem, 0, LVIF_STATE, NULL, 0, LVIS_SELECTED, LVIS_SELECTED, 0);		

			// call methods to invoke the event
			InvokeMethodInt(
				mpTemplate->GetStrProperty(nEventClicked),  
				lvhti.iItem,
				m_bInvokeWithSendString);
		}
	}
	
	//HideEditControls();
	/*
	if (nStyle >= 3)
	{
		if (
			(lvhti.iSubItem == 0 && nStyle == 3) ||
			nStyle == 4
		   )
		{
		// call methods to invoke the event
		InvokeMethodIntInt(		
			mpTemplate->GetStrProperty(nEventBeginLabelEdit),
			lvhti.iItem,
			lvhti.iSubItem,
			m_bInvokeWithSendString);
		}
	}
	*/
	if (mpTemplate->GetBoolProperty(nDragnDropAllowBegin) == TRUE && nFlags == 1)
	{
		// Select the item the user clicked on.
		UINT uFlags;
		int nItem = HitTest(point, &uFlags);

		if (nItem > -1)
		{
			// force the selected item to show focus
			SetFocus();
			UINT flag = LVIS_SELECTED | LVIS_FOCUSED;
			SetItemState(nItem, flag, flag);
           
		}


		LVHITTESTINFO lvhti;

		// Clear the subitem text the user clicked on.
		lvhti.pt = point;
		SubItemHitTest(&lvhti);

		// a -1 will be returned if not found
		if (mpTemplate->GetLngProperty(nListViewStyle) > -1)
		{			
			if (lvhti.iItem > -1)
			{		
				if (mpTemplate->GetLngProperty(nListViewStyle) > -1)
					// call methods to invoke the event
					InvokeMethodIntInt(
						mpTemplate->GetStrProperty(nEventClicked),  
						lvhti.iItem, 
						lvhti.iSubItem, 
						m_bInvokeWithSendString);
				else				
					// call methods to invoke the event
					InvokeMethodInt(
						mpTemplate->GetStrProperty(nEventClicked),  
						lvhti.iItem, 
						m_bInvokeWithSendString);
			}
			BeginDragnDrop(mpTemplate, point, m_bInvokeWithSendString);
		}
		else
		{
			CStringArray saBlockNames;
			saBlockNames.SetSize(1,1);
			POSITION pos = GetFirstSelectedItemPosition();
			if (pos == NULL)
			{
				CListCtrl::OnLButtonDown(nFlags, point);
				return;
			}
			else
			{
			   while (pos)
			   {
					int nItem = GetNextSelectedItem(pos);
					CString sItemText = GetItemText(nItem, 0);
					
					saBlockNames.Add(sItemText);
			   }
			}

			//!CHANGED! 10-5-04 SRM
			//didnt allow user to drag and drop blocks from external dwgs
			//if (m_pLoadedDwg == NULL)
				BeginDragnDropToInsertBlocks(mpTemplate, point, m_bInvokeWithSendString, saBlockNames);
		}
	}

	InvokeMethodIntIntIntInt(
		mpTemplate->GetStrProperty(nEventMouseDown),
		1,
		nFlags,
		point.x,
		point.y,
		m_bInvokeWithSendString);

	//if (bDrag == TRUE)
	CListCtrl::OnLButtonDown(nFlags, point);
}

void OdclListCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{

	InvokeMethodIntIntIntInt(
		mpTemplate->GetStrProperty(nEventMouseUp),
		1,
		nFlags,
		point.x,
		point.y,
		m_bInvokeWithSendString);

	
	CListCtrl::OnLButtonUp(nFlags, point);
}



void OdclListCtrl::SetDragnDrop(BOOL bRegister)
{
	if (bRegister == TRUE)
	{
		BOOL success = m_DropTarget.Register(this);
		m_DropTarget.m_pThisArxControl = mpTemplate;
		m_DropTarget.m_pParent = this;
    }
	else
		m_DropTarget.Revoke();
}
/////////////////////////////////////////////////////////////////////////////

void OdclListCtrl::OnClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* plvdi = (LV_DISPINFO*)pNMHDR;
	
	int nStyle = mpTemplate->GetLngProperty(nListViewStyle);

	if (plvdi->item.mask < GetItemCount() && 
		plvdi->item.mask >= 0)
	{
		// call methods to invoke the event
		InvokeMethodIntInt(		
			mpTemplate->GetStrProperty(nEventBeginLabelEdit),
			plvdi->item.mask, plvdi->item.iItem,
			m_bInvokeWithSendString);

		
		if (nStyle == 4)
		{
			// call methods to invoke the event
			InvokeMethodIntInt(
				mpTemplate->GetStrProperty(nEventClicked),  
				plvdi->item.mask, 
				plvdi->item.iItem, 
				m_bInvokeWithSendString);			
		}
		/*else				
		{
			// call methods to invoke the event
			InvokeMethodInt(
				mpTemplate->GetStrProperty(nEventClicked),  
				plvdi->item.iItem, 
				m_bInvokeWithSendString);
		}
		*/
	}
/*	else
	{
		HideEditControls();
	}*/
	*pResult = 0;
}


void OdclListCtrl::OnKillfocus(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// call methods to invoke the event
	InvokeMethod(mpTemplate->GetStrProperty(nEventKillFocus), m_bInvokeWithSendString);
	
	*pResult = 0;
}


void OdclListCtrl::OnRclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* plvdi = (LV_DISPINFO*)pNMHDR;

	if (plvdi->item.mask < GetItemCount() && 
		plvdi->item.mask >= 0)
	{
		if (mpTemplate->GetLngProperty(nListViewStyle) > -1)
			// call methods to invoke the event
			InvokeMethodIntInt(
				mpTemplate->GetStrProperty(nEventDblClicked),  
				plvdi->item.mask, 
				plvdi->item.iItem, 
				m_bInvokeWithSendString);
		else				
			// call methods to invoke the event
			InvokeMethodInt(
				mpTemplate->GetStrProperty(nEventDblClicked),  
				plvdi->item.mask, 
				m_bInvokeWithSendString);
	}
	
	*pResult = 0;
}

void OdclListCtrl::OnRdblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* plvdi = (LV_DISPINFO*)pNMHDR;

	if (plvdi->item.mask < GetItemCount() && 
		plvdi->item.mask >= 0)
	{
		if (mpTemplate->GetLngProperty(nListViewStyle) > -1)
			// call methods to invoke the event
			InvokeMethodIntInt(
				mpTemplate->GetStrProperty(nEventDblClicked),  
				plvdi->item.mask, 
				plvdi->item.iItem, 
				m_bInvokeWithSendString);
		else				
			// call methods to invoke the event
			InvokeMethodInt(
				mpTemplate->GetStrProperty(nEventDblClicked),  
				plvdi->item.mask, 
				m_bInvokeWithSendString);
	}
	
	*pResult = 0;
}

void OdclListCtrl::OnReturn(NMHDR* pNMHDR, LRESULT* pResult) 
{
	InvokeMethod(mpTemplate->GetStrProperty(nEventReturnPressed), m_bInvokeWithSendString);
	
	*pResult = 0;
}

void OdclListCtrl::OnSetfocus(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// call methods to invoke the event
	InvokeMethod(mpTemplate->GetStrProperty(nEventSetFocus), m_bInvokeWithSendString);
	
	
	*pResult = 0;
}

void OdclListCtrl::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	LV_ITEM		*plvItem = &pDispInfo->item;

	if (pDispInfo->item.mask > 0)
	{
		InvokeMethodStringIntInt(
			mpTemplate->GetStrProperty(nEventEndLabelEdit),
			plvItem->pszText,
			plvItem->iItem,
			plvItem->iSubItem,
			m_bInvokeWithSendString);

		SetItemText(plvItem->iItem, plvItem->iSubItem, plvItem->pszText);
		//SetItemText(nItem, m_nEditSubItem, sNewText);
	}
	*pResult = 0;
}


void OdclListCtrl::OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;

	int nItem = pDispInfo->item.iItem;

	if (m_nEditSubItem > -1)
	{
		CRect rcItem;
		GetSubItemRect(nItem,m_nEditSubItem, LVIR_LABEL, rcItem);

		HWND hWnd=(HWND)SendMessage(LVM_GETEDITCONTROL);
		ASSERT(hWnd!=NULL);
		VERIFY(m_LVEdit.SubclassWindow(hWnd));

		m_LVEdit.m_x=rcItem.left;
		m_LVEdit.m_y=rcItem.top-1;
		m_LVEdit.m_pParent = this;
		m_LVEdit.SetWindowText(GetItemText(nItem,m_nEditSubItem));

		/*
		CRect rcNewPos(rcItem.left, rcItem.top-1, rcItem.Width(), rcItem.Height());
		m_LVEdit.MoveWindow(rcNewPos);
		*/
	}

	// call methods to invoke the event
	InvokeMethodIntInt(		
		mpTemplate->GetStrProperty(nEventBeginLabelEdit),
		nItem,
		m_nEditSubItem,
		m_bInvokeWithSendString);

	
	*pResult = 0;
}

void OdclListCtrl::OnKeydown(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;
	
//	CString sChar = pLVKeyDow->wVKey;
//	InvokeMethodString(mpTemplate->GetStrProperty(nEventKeyDown), sChar, m_bInvokeWithSendString);
	
	
	*pResult = 0;
}

void OdclListCtrl::OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	InvokeMethodInt(mpTemplate->GetStrProperty(nEventColumnClick), pNMListView->iSubItem, m_bInvokeWithSendString);
	
	*pResult = 0;
}

void OdclListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	CListCtrl::OnLButtonDblClk(nFlags, point);

	int nRow, nCol = -1;
	POSITION pos = GetFirstSelectedItemPosition();
	if (pos == NULL)
	{
		if (mpTemplate->GetLngProperty(nListViewStyle) > -1)
		{
			LVHITTESTINFO lvhti;

			// Clear the subitem text the user clicked on.
			lvhti.pt = point;
			SubItemHitTest(&lvhti);

			nRow = lvhti.iItem;
			if (nRow > -1)
			{			
				nCol = lvhti.iSubItem;

				// force the selected item to show focus
				SetFocus();
				UINT flag = LVIS_SELECTED | LVIS_FOCUSED;
				SetItemState(nRow, flag, flag);
			}
		}
		else
		{
			nRow = -1;
			nCol = -1;		
		}
	}
	else
	{
		nRow = GetNextSelectedItem(pos);			
		nCol = 0;
	}
	
	if (mpTemplate->GetLngProperty(nListViewStyle) > -1)
		// call methods to invoke the event
		InvokeMethodIntInt(
			mpTemplate->GetStrProperty(nEventDblClicked),  
			nRow, 
			nCol, 
			m_bInvokeWithSendString);
	else				
		// call methods to invoke the event
		InvokeMethodInt(
			mpTemplate->GetStrProperty(nEventDblClicked),  
			nRow, 
			m_bInvokeWithSendString);
	
}

// SortTextItems - Sort the list based on column text
// Returns		 - Returns true for success
// nCol			 - column that contains the text to be sorted
// bAscending	 - indicate sort order
// low			 - row to start scanning from - default row is 0
// high			 - row to end scan. -1 indicates last row
BOOL OdclListCtrl::SortTextItems( int nCol, BOOL bAscending, int low, int high)
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
void OdclListCtrl::SetItemImage( int nRow, int nCol, int nImage)  
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

int OdclListCtrl::GetItemImage( int nRow, int nCol)  
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

void OdclListCtrl::SetItemTextImage( int nRow,int nCol, CString sText, int nImage)  
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
bool OdclListCtrl::SortNumericItems( int nCol, BOOL bAscending,int low, int high)
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

void OdclListCtrl::SetTooltipText(CString* spText, BOOL bActivate)
{
} // End of SetTooltipText

void OdclListCtrl::InitToolTip()
{
	if (mToolTip.m_hWnd == NULL)
		mToolTip.Create(this);
} // End of InitToolTip


BOOL OdclListCtrl::PreTranslateMessage(MSG* pMsg) 
{
	InitToolTip();
	mToolTip.RelayEvent(pMsg);
		
	return CListCtrl::PreTranslateMessage(pMsg);
}

bool OdclListCtrl::LoadDwg(CString sFileName)
{
	TCHAR fullpath[511]; 

	try
	{
		if (acedFindFile(sFileName, fullpath) != RTNORM) 			
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
		if (Acad::eOk != (es = m_pLoadedDwg->readDwgFile(fullpath,_SH_DENYNO,false)))
		{
			CString sLoadString;
			sLoadString = theWorkspace.LoadResourceString(IDS_DWGNOTLOADING);
			acedAlert(sLoadString + sFileName);
			delete m_pLoadedDwg;					
			m_pLoadedDwg = NULL;
			return false;
		}
		// set the file name for future use
		m_FileName = fullpath;
		
		if (m_pLoadedDwg==NULL)
		{
			return false;			
		}
	}
	catch(...)
	{
	}
	
	// now call the method to display the blocks in this loaded dwg
	RefreshBlockList();
	return true;			
		
}


void OdclListCtrl::OnMButtonDown(UINT nFlags, CPoint point) 
{
	//HideEditControls();

	InvokeMethodIntIntIntInt(
		mpTemplate->GetStrProperty(nEventMouseDown),
		4,
		nFlags,
		point.x,
		point.y,
		m_bInvokeWithSendString);
	
	
	CListCtrl::OnMButtonDown(nFlags, point);
}

void OdclListCtrl::OnMButtonUp(UINT nFlags, CPoint point) 
{
	//HideEditControls();

	InvokeMethodIntIntIntInt(
		mpTemplate->GetStrProperty(nEventMouseUp),
		4,
		nFlags,
		point.x,
		point.y,
		m_bInvokeWithSendString);
	
	
	CListCtrl::OnMButtonUp(nFlags, point);
}

void OdclListCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{
	//HideEditControls();

	InvokeMethodIntIntIntInt(
		mpTemplate->GetStrProperty(nEventMouseDown),
		2,
		nFlags,
		point.x,
		point.y,
		m_bInvokeWithSendString);
	
	
	CListCtrl::OnRButtonDown(nFlags, point);
}

void OdclListCtrl::OnRButtonUp(UINT nFlags, CPoint point) 
{
	//HideEditControls();

	InvokeMethodIntIntIntInt(
		mpTemplate->GetStrProperty(nEventMouseUp),
		2,
		nFlags,
		point.x,
		point.y,
		m_bInvokeWithSendString);

	
	CListCtrl::OnRButtonUp(nFlags, point);
}

void OdclListCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	char sChar = nChar;
	InvokeMethodStringIntInt(mpTemplate->GetStrProperty(nEventKeyDown), sChar, nRepCnt, nFlags, m_bInvokeWithSendString);
		
	CListCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

void OdclListCtrl::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	char sChar = nChar;
	InvokeMethodStringIntInt(mpTemplate->GetStrProperty(nEventKeyUp), sChar, nRepCnt, nFlags, m_bInvokeWithSendString);
		
	CListCtrl::OnKeyUp(nChar, nRepCnt, nFlags);
}

void OdclListCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	InvokeMethodIntIntInt(
		mpTemplate->GetStrProperty(nEventMouseMove),
		nFlags,
		point.x,
		point.y,
		m_bInvokeWithSendString);
	
	CListCtrl::OnMouseMove(nFlags, point);
}


void OdclListCtrl::OnOdstatechanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NMLVODSTATECHANGE* pStateChanged = (NMLVODSTATECHANGE*)pNMHDR;

	*pResult = 0;
}

void OdclListCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if( GetFocus() != this ) SetFocus();
	CListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}

void OdclListCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if( GetFocus() != this ) SetFocus();
	CListCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
}

void OdclListCtrl::PostNcDestroy() 
{
	CListCtrl::PostNcDestroy();
	delete this;
}
