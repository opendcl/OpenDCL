// ArxDwgListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ArxDwgListCtrl.h"
#include "Resource.h"
#include "Resource.h"
#include "Workspace.h"
#include "InvokeMethod.h"
#include "ControlPane.h"
#include "ArxFolderComboCtrl.h"
#include "SADirRead.h"
#include "PropertyIds.h"
#include "DclControlObject.h"
#include "PropertyObject.h"
#include "ToolTips.h"


BOOL ThumbnailFile::LoadDwgThumbnail()
{
	if (m_bLoaded == true)
		return FALSE;

	// reset old values
	m_sPath.TrimRight( _T("\\/") );
	CString sFileName = m_sPath + _T("\\") + m_sFileName;
	BOOL m_IsValid = FALSE;
	long m_nCount  = 0;
	CString m_szTitle = CString();
	CSize m_size;
	m_size.cx = 0;
	m_size.cy = 0;
	const int DWG_OFFSET = 13;
	const int SENITEL_LENGTH = 16;
	const int TITLE_THUMB = 1;
	const int BMP_THUMB = 2;
	// open DWG-File
	CFile          f;
	CFileException e;
	TRACE (_T("Open DWG: %s\n"), (const TCHAR *) sFileName);
	if (f.Open (sFileName, CFile::modeRead | CFile::typeBinary| CFile::shareDenyNone , &e))
	{ 
			TRY
			{
				 LONG  lOffset = DWG_OFFSET;
				 DWORD  lSize;
				 char  cbSenitel[SENITEL_LENGTH];
				 char  nCount;
				 char  nType;
 
	 // offset to start of Thumbnail-Data
				 f.Seek (lOffset, CFile::begin);
				 f.Read (&lOffset, sizeof(LONG));         
				 f.Seek (lOffset, CFile::begin);
 
	 // read senitel and test it
				 f.Read (cbSenitel, SENITEL_LENGTH);
				 
	 // OK we have a R13 Thumbnail - look for the different types
				 m_IsValid = TRUE;
				 f.Read (&lSize, 4);   // not used

	 // look for number of thumbnail entries
				 f.Read (&nCount, 1);
	 // iterate the entries
				 for (int num = 0;num < nCount;num++)
				 {
	 // Get type of thumbnail
						f.Read (&nType, 1);
	 // Get Offset and Size in DWG
						f.Read (&lOffset, sizeof(LONG));
						f.Read (&lSize, 4);
	 // prepare different thumbnailtypes
	 // Text with Title
						if (nType == TITLE_THUMB)
						{
							 char szTitle[256];
							 LONGLONG lOld = (LONGLONG)f.GetPosition();
							 f.Seek (lOffset, CFile::begin);
							 f.Read(szTitle, lSize);
							 szTitle[lSize] = '\0';
							 m_szTitle = szTitle;
							 f.Seek (lOld, CFile::begin);
						}
	 // Bitmap (copied from some DIB examples)
						else if (nType == BMP_THUMB)
						{
				LONGLONG lOld = (LONGLONG)f.GetPosition();
				f.Seek (lOffset, CFile::begin);
				
				HGLOBAL hDIB = ::GlobalAlloc(GMEM_FIXED, lSize);
				
				if (f.Read(hDIB, lSize) != lSize)
				{
					::GlobalFree((HGLOBAL) hDIB);
					return FALSE;
				}
				
				BITMAPINFOHEADER &bmiHeader = *(LPBITMAPINFOHEADER)hDIB;
				BITMAPINFO &bmInfo = *(LPBITMAPINFO)hDIB;

				// If bmiHeader.biClrUsed is zero we have to infer the number
				// of colors from the number of bits used to specify it.
				int nColors = bmiHeader.biClrUsed ? bmiHeader.biClrUsed : 
									1 << bmiHeader.biBitCount;

				LPVOID lpDIBBits;
				if( bmInfo.bmiHeader.biBitCount > 8 )
					lpDIBBits = (LPVOID)((LPDWORD)(bmInfo.bmiColors + bmInfo.bmiHeader.biClrUsed) + 
						((bmInfo.bmiHeader.biCompression == BI_BITFIELDS) ? 3 : 0));
				else
					lpDIBBits = (LPVOID)(bmInfo.bmiColors + nColors);

				
				RGBQUAD *rgbStd = new RGBQUAD[nColors] ;
				
				// Create the palette
				if( nColors <= 256 )
				{
					for( int i=0; i < nColors; i++)
					{
						// here we switch the background color from white to black
						if (bmInfo.bmiColors[i].rgbBlue == 255 &&
							bmInfo.bmiColors[i].rgbGreen == 255 &&
							bmInfo.bmiColors[i].rgbRed == 255)
						{	
							rgbStd[i].rgbBlue = 0;
							rgbStd[i].rgbGreen = 0;
							rgbStd[i].rgbRed = 0;
							rgbStd[i].rgbReserved = 0;
						}
						// here we switch the foreground color from black to white
						else if (bmInfo.bmiColors[i].rgbBlue == 0 &&
							bmInfo.bmiColors[i].rgbGreen == 0 &&
							bmInfo.bmiColors[i].rgbRed == 0)							
						{	
							rgbStd[i].rgbBlue = 255;
							rgbStd[i].rgbGreen = 255;
							rgbStd[i].rgbRed = 255;
							rgbStd[i].rgbReserved = 0;
						}
						else
						{
							rgbStd[i].rgbBlue = bmInfo.bmiColors[i].rgbBlue;
							rgbStd[i].rgbGreen = bmInfo.bmiColors[i].rgbGreen;
							rgbStd[i].rgbRed = bmInfo.bmiColors[i].rgbRed;
							rgbStd[i].rgbReserved = 0;
						}
					}
				}
			
				CClientDC dc(NULL);

				memcpy( bmInfo.bmiColors, rgbStd, sizeof(RGBQUAD) * nColors ); 
				delete rgbStd;

				m_hBitmap = CreateDIBitmap( dc.m_hDC,		// handle to device context 
							&bmiHeader,	// pointer to bitmap size and format data 
							CBM_INIT,	// initialization flag 
							lpDIBBits,	// pointer to initialization data 
							&bmInfo,	// pointer to bitmap color-format data 
							DIB_RGB_COLORS);		// color-data usage 
				m_hDIB = hDIB;
				
				// store the height and width of the bmp
				nBmpHeight = bmiHeader.biHeight;
				nBmpWidth = bmiHeader.biWidth;

				//m_size = GetSize();
				f.Seek (lOld, CFile::begin);
			}
		 }
			}
			CATCH (CFileException, e)
			{
				 TRACE ("Read error in LoadDwgThumbnail()\n");
			}
			END_CATCH
			f.Close();
		m_bLoaded = true;	
		return TRUE;
		
	 }
	 return FALSE;
}

CSize ThumbnailFile::GetBitmapSize()
{
	if (m_bLoaded && nBmpHeight > 0 && nBmpWidth > 0)
		return CSize(nBmpWidth, nBmpHeight);
	
	if (!m_bLoaded && m_sFileName.GetLength() > 0)
		// load the Thumbnail now.
		LoadDwgThumbnail();

	if (nBmpHeight > 0 && nBmpWidth > 0)
		return CSize(nBmpWidth, nBmpHeight);
	
	return CSize(0,0);
}


void ThumbnailFile::DrawBitmap(CDC* pDC, CPoint pt, int nPaintableWidth, int nPaintableHeight, HBITMAP &hR14LogoLarge, HBITMAP &hR14LogoSmall)
{
	CPoint ptOrignal = pt;
	// if not already loaded, 
	if (!m_bLoaded)
		// load the Thumbnail now.
		LoadDwgThumbnail();

	CDC dcMem;
	VERIFY( dcMem.CreateCompatibleDC(pDC));

	// if a valid hbitmap 
	if (m_hBitmap == NULL)
	{
	
		HDC hdc = pDC->m_hDC;
		CPoint point;
		
		CBrush StaticBrush(pDC->GetBkColor());
		CRect rc(pt.x, pt.y, pt.x + nPaintableWidth, pt.y + nPaintableHeight);
		pDC->FillRect(rc, &StaticBrush);
		StaticBrush.DeleteObject();

		COLORREF rgb = RGB(255, 0, 0);
		HGDIOBJ pen = ::CreatePen(PS_SOLID, 1, rgb);
		HGDIOBJ OldPen = SelectObject(hdc, pen);
		/*
		MoveToEx(hdc, pt.x, pt.y, &point);
		// |
		LineTo(hdc, pt.x, pt.y + nPaintableHeight);		
		// _
		LineTo(hdc, pt.x + nPaintableWidth-2, pt.y + nPaintableHeight);		
		//  |
		LineTo(hdc, pt.x + nPaintableWidth-2, pt.y);		
		// -
		LineTo(hdc, pt.x, pt.y);		
		*/
		/*
		// \				
		LineTo(hdc, pt.x + nPaintableWidth-2, pt.y + nPaintableHeight);		
		// move
		MoveToEx(hdc, pt.x, pt.y + nPaintableHeight, &point);		
		// /
		LineTo(hdc, pt.x + nPaintableWidth-2, pt.y);		
		*/
		SelectObject(hdc, OldPen);			
		DeleteObject(pen);

		
		// Select bitmap into memory DC.
		HBITMAP* pBmpOld = NULL;
		
		if (nPaintableHeight < 100)
		{
			pBmpOld = (HBITMAP*) ::SelectObject(dcMem.m_hDC, hR14LogoSmall);
			
			int nX = (nPaintableWidth - 53) / 2;
			int nY = (nPaintableHeight - 51) / 2;
			
			pDC->StretchBlt(nX, pt.y+nY, 53, 51, 
										 &dcMem, 0, 0, 53, 51,
										 SRCCOPY);

		}
		else
		{
			pBmpOld = (HBITMAP*) ::SelectObject(dcMem.m_hDC, hR14LogoLarge);
			
			int nX = 20;
			int nY = (nPaintableHeight - 98) / 2;
			
			pDC->StretchBlt(nX+5, pt.y+nY+5, 100, 98, 
										 &dcMem, 0, 0, 100, 98,
										 SRCCOPY);

		}

		
		::SelectObject(dcMem.m_hDC, pBmpOld);
		return;
	}
	
	// if a valid hbitmap 
	if (m_hBitmap != NULL)
	{
		double dFactor;
		double dH;
		double dW;
		CSize paintSize(nPaintableWidth, nPaintableHeight);
		CSize drawSize(nBmpWidth, nBmpHeight);

		if (drawSize.cx < paintSize.cx && drawSize.cy < paintSize.cy)
			paintSize = drawSize;

		/*if (drawSize.cx < paintSize.cx && drawSize.cy == paintSize.cy)
		{
			pt.x = pt.x + (paintSize.cx - drawSize.cx) / 2;
		}		
		else*/
		if (drawSize.cy < paintSize.cy && drawSize.cx == paintSize.cx)
		{
			pt.y = pt.y + (paintSize.cy - drawSize.cy) / 2;
		}	
		else
		{
			// if the calc height is too large
			if (drawSize.cy > paintSize.cy)
			{
				dFactor = (double)drawSize.cx / (double)drawSize.cy;
				dW = dFactor;

				drawSize.cy = paintSize.cy;
				drawSize.cx = long(dW * drawSize.cy);

				pt.x = (paintSize.cx - drawSize.cx) / 2;
			}
			else if (drawSize.cy < paintSize.cy)
			{
				dFactor = (double)drawSize.cy / (double)drawSize.cx;
				dH = dFactor;
				
				if (dFactor <= 1.0)
				{
					drawSize.cx = paintSize.cx;
					drawSize.cy = long((double)drawSize.cx * dH);
				}
				else
				{
					drawSize.cx = long((double)drawSize.cy * dH);
					drawSize.cy = paintSize.cy;
				}

				pt.x = pt.x + (paintSize.cx - drawSize.cx) / 2;
				pt.y = pt.y + (paintSize.cy - drawSize.cy) / 2;
			}
			
			// if the calc width is too large
			if (drawSize.cx > paintSize.cx)
			{
				dFactor = (double)drawSize.cy / (double)drawSize.cx;
				dH = dFactor;
				
				drawSize.cx = paintSize.cx;
				drawSize.cy = long((double)drawSize.cx * dH);
				
				pt.x = 1;
				pt.y = pt.y + (paintSize.cy - drawSize.cy) / 2;
			}
			// if the calc height is too large
			if (drawSize.cy > paintSize.cy)
			{
				drawSize.cx = long(((double)paintSize.cx) * drawSize.cy / ((double)paintSize.cy));
				drawSize.cy = paintSize.cy;
				
			}

			if (pt.y < 18)
				pt.y = 18;
		}
		
		// Select bitmap into memory DC.
		HBITMAP* pBmpOld = (HBITMAP*) ::SelectObject(dcMem.m_hDC, m_hBitmap);

		pDC->StretchBlt(pt.x, pt.y, drawSize.cx, drawSize.cy, 
										 &dcMem, 0, 0, nBmpWidth, nBmpHeight,
										 SRCCOPY);

		::SelectObject(dcMem.m_hDC, pBmpOld);

		
		CRect rcTop;
			rcTop.left = ptOrignal.x;
			rcTop.top = ptOrignal.y;
			rcTop.right = ptOrignal.x + nPaintableWidth;
			rcTop.bottom = pt.y;
		CRect rcBottom;
			rcBottom.left = ptOrignal.x,
			rcBottom.top = drawSize.cy + pt.y,
			rcBottom.right = ptOrignal.x + nPaintableWidth,
			rcBottom.bottom = ptOrignal.y + nPaintableHeight;

		CRect rcLeft;
			rcLeft.left = ptOrignal.x;
			rcLeft.top = ptOrignal.y;
			rcLeft.right = pt.x;
			rcLeft.bottom = ptOrignal.y + nPaintableHeight;
		CRect rcRight;
			rcRight.left = pt.x + drawSize.cx;
			rcRight.top = ptOrignal.y;
			rcRight.right = ptOrignal.x + nPaintableWidth;
			rcRight.bottom = ptOrignal.y + nPaintableHeight;
			
			CBrush StaticBrush(pDC->GetBkColor());
		if (rcTop.Height() > 0)
			pDC->FillRect(rcTop, &StaticBrush);
		if (rcBottom.Height() > 0)
			pDC->FillRect(rcBottom, &StaticBrush);
		if (rcLeft.Width() > 0)
			pDC->FillRect(rcLeft, &StaticBrush);
		if (rcRight.Width() > 0)
			pDC->FillRect(rcRight, &StaticBrush);
		StaticBrush.DeleteObject();
	}
	dcMem.DeleteDC();
}


/////////////////////////////////////////////////////////////////////////////
// CArxDwgListCtrl

CArxDwgListCtrl::CArxDwgListCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CListBoxCtrl( pTemplate, pPane, nID, false )
, mArxServices( this )
, mDragDropService( this )
, mBlockInsertDropTarget( this )
, mnRowHeight( -1 )
{
	//m_RowHeight = 120;
	m_pDirComboBox = NULL;
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CArxDwgListCtrl::~CArxDwgListCtrl()
{
}

LPCTSTR CArxDwgListCtrl::GetDragTextPrefix() const
{
	static CString sDragPrefix;
	sDragPrefix = msPath;
	sDragPrefix.TrimRight( _T("\\/") );
	sDragPrefix += _T('\\');
	return sDragPrefix;
}

bool CArxDwgListCtrl::Create( CWnd* pParentWnd, UINT nID ) 
{
	bool bSuccess =
		__super::Create( pParentWnd, nID );

	CreateImageList();
	mhR14LogoLarge = (HBITMAP)::LoadImage(theWorkspace.GetLocalResourceModule(), MAKEINTRESOURCE(IDB_R14LARGE), IMAGE_BITMAP, 0, 0, 0);
	mhR14LogoSmall = (HBITMAP)::LoadImage(theWorkspace.GetLocalResourceModule(), MAKEINTRESOURCE(IDB_R14SMALL), IMAGE_BITMAP, 0, 0, 0);

	return bSuccess;
}

DWORD CArxDwgListCtrl::GetWndStyle() const
{
	DWORD dwStyle = __super::GetWndStyle();
	dwStyle &= ~(DWORD)(LBS_USETABSTOPS);
	dwStyle |= LBS_OWNERDRAWVARIABLE;
	return dwStyle;
}

bool CArxDwgListCtrl::ApplyProperty( TPropertyPtr pProp )
{
	if( !__super::ApplyProperty( pProp ) )
		return false;
	bool bFailed = false;
	switch( pProp->GetID() )
	{
	case Prop::RowHeight:
		{
			mnRowHeight = pProp->GetLongValue();
			int nCurSel = GetCurSel();
			Dir( msPath );
			SetCurSel( nCurSel );
			OnNeedRepaint();
			break;
		}
	}
	return !bFailed;
}

void CArxDwgListCtrl::CreateImageList()
{
	CBitmap bitmap;
	bitmap.LoadBitmap( IDB_FOLDER );
	m_imageList.Create( 16, 16, ILC_COLOR24, 10, 5 );
	m_imageList.Add( &bitmap, RGB(0,0,0) );
}

void CArxDwgListCtrl::ClearThumbnailList()
{
	POSITION pos = m_ThumbnailFileList.GetHeadPosition();
	while (pos != NULL)
	{
		ThumbnailFile *pThumbFile = m_ThumbnailFileList.GetAt(pos);
		delete pThumbFile;
		m_ThumbnailFileList.RemoveAt(pos);
		pos = m_ThumbnailFileList.GetHeadPosition();
	}
}

void CArxDwgListCtrl::Dir(CString sDir) 
{
	CSADirRead dr;
	CString csTemp;
	CString sFileName;
	CStringArray saFolderNames;
	bool bInserted;
	int nCount;

	// empty the list box
	ResetContent();
	
	// call the method to clear the thumbnail file list
	ClearThumbnailList();
	saFolderNames.RemoveAll();
	saFolderNames.SetSize(0,1);

	sDir = sDir.TrimRight( _T("\\/") );
	msPath = sDir;
	dr.ClearDirs();         // start clean
	if (!dr.GetDirs(sDir, true)) // get all folders 
		return; // exit if nothing found

	// get the dir array
	CSADirRead::SADirVector &dirs = dr.Dirs();
	
	// loop over it
	for (CSADirRead::SADirVector::const_iterator dit = dirs.begin(); dit!=dirs.end(); dit++)
	{
		TRACE("%s\n", (*dit).m_sName);		
		CString sFileName = (*dit).m_sName;
		
		if (sDir == sFileName)
			continue;
		int nSlash = sFileName.ReverseFind(_T('\\'));
		sFileName = sFileName.Mid(nSlash+1);
		if (sFileName.GetLength() == 0)
			continue;
		saFolderNames.Add(sFileName);	
		bInserted = false;	
		
		nCount = GetCount();			
		if (nCount > 0)
		{
			CString sThisText;
			for (int j=0; j<nCount; j++)
			{				
				GetText(j, sThisText);
				CString s1 = sFileName;
				s1.MakeLower();
				sThisText.MakeLower();
				// if this text is less than the current file name
				if (sThisText > s1)
				{
					int nIndex = InsertString(j, sFileName);
					SetItemData(nIndex, 0);
					SetItemHeight(nIndex, 17);
					ThumbnailFile *BlankBitmap = new ThumbnailFile;
					m_ThumbnailFileList.AddTail(BlankBitmap);
					bInserted = true;
					j = nCount;
				}
			}
		}
		if (!bInserted)
		{
			int nIndex = AddString(sFileName);
			SetItemData(nIndex, 0);
			SetItemHeight(nIndex, 17);		
			ThumbnailFile *BlankBitmap = new ThumbnailFile;
			m_ThumbnailFileList.AddTail(BlankBitmap);
		}
	}

	dr.ClearFiles();        // start clean

	dr.GetFiles(_T("*.dwg"));   // get all *.DWG files 

	// get the file array
	CSADirRead::SAFileVector &files = dr.Files();   

	int nDirCount = GetCount();

	// dump it...
	for (CSADirRead::SAFileVector::const_iterator fit = files.begin(); fit!=files.end(); fit++)
	{
		TRACE("%s\n", (*fit).m_sName);
		CString sFileName = (*fit).m_sName;
		int nSlash = sFileName.ReverseFind(*_T("\\"));
		sFileName = sFileName.Mid(nSlash+1);
		saFolderNames.Add(sFileName);	
		bInserted = false;
		nCount = GetCount();

		
		if (nCount - nDirCount > 0)
		{
			CString sThisText;
			for (int j=nDirCount; j<nCount; j++)
			{				
				GetText(j, sThisText);
				CString s1 = sFileName;
				s1.MakeLower();
				sThisText.MakeLower();
				// if this text is less than the current file name
				if (sThisText > s1)
				{
					int nIndex = InsertString(j, sFileName);
					SetItemData(nIndex, 1);
					ThumbnailFile *BlankBitmap = new ThumbnailFile( msPath, sFileName );
					BlankBitmap->nIndex = nIndex;
					m_ThumbnailFileList.AddTail(BlankBitmap);
					BlankBitmap->LoadDwgThumbnail();
					int nHeight = mnRowHeight;
					if( nHeight <= 0 )
					{
						nHeight = BlankBitmap->GetBitmapSize().cy;
						if( nHeight <= 0 )
							nHeight = 100;
						nHeight += 25; //add space for filename text
					}
					SetItemHeight(nIndex, nHeight);
					bInserted = true;
					j = nCount;
				}
			}
		}
		if (!bInserted)
		{
			int nIndex = AddString(sFileName);
			SetItemData(nIndex, 1);
			ThumbnailFile *BlankBitmap = new ThumbnailFile( msPath, sFileName );
			BlankBitmap->nIndex = nIndex;
			m_ThumbnailFileList.AddTail(BlankBitmap);
			BlankBitmap->LoadDwgThumbnail();
			SetItemData(nIndex, 1);
			int nHeight = mnRowHeight;
			if( nHeight <= 0 )
			{
				nHeight = BlankBitmap->GetBitmapSize().cy;
				if( nHeight <= 0 )
					nHeight = 100;
				nHeight += 25; //add space for filename text
			}
			SetItemHeight(nIndex, nHeight);
		}
	}	
	dr.ClearFiles();		// start clean, again
	return;
}


BEGIN_MESSAGE_MAP(CArxDwgListCtrl, CListBoxCtrl)
	ON_CONTROL_REFLECT(LBN_DBLCLK, OnDblclk)
	ON_CONTROL_REFLECT(LBN_SELCHANGE, OnCbnSelchange)
	ON_WM_DESTROY()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CArxDwgListCtrl message handlers

void CArxDwgListCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	ASSERT(lpDrawItemStruct->CtlType == ODT_LISTBOX);

	// Lets make a CDC for ease of use
	CDC *pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	COLORREF backGround;
	COLORREF textColor;
	COLORREF insideColor = ::GetSysColor(COLOR_WINDOW);

	if (lpDrawItemStruct->itemID < (UINT)GetTopIndex())
		return;

	int nCurSel = GetCurSel();
	if (nCurSel == lpDrawItemStruct->itemID)
	{
		backGround = ::GetSysColor(COLOR_HIGHLIGHT);
		textColor = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
	}
	else if (lpDrawItemStruct->itemState & ODS_SELECTED && GetFocus() == this)
	{
		backGround = ::GetSysColor(COLOR_HIGHLIGHT);
		textColor = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
	}
	else
	{
		backGround = ::GetSysColor(COLOR_WINDOW);
		textColor = ::GetSysColor(COLOR_BTNTEXT);
	}
	pDC->SetBkColor(backGround);
	
	ASSERT(pDC); // Attached failed
	
	CRect rc(lpDrawItemStruct->rcItem);
	
	
	// Save off context attributes
	int nIndexDC = pDC->SaveDC();

	pDC->SetBkMode(TRANSPARENT);

	CRect rcInside = rc;
	CRect rcOutside = rc;

	rcInside.right = rcInside.left + 17;
	rcOutside.left = rcOutside.left + 18;

	CBrush *pStaticBrush = new CBrush;
	pStaticBrush->CreateSolidBrush(insideColor);

	pDC->FillRect(rcInside, pStaticBrush);
	pDC->FillRect(rcOutside, pStaticBrush);

	rcOutside.top = rcInside.top;
	rcOutside.bottom =  rcInside.top + 19;

	delete pStaticBrush;

	pStaticBrush = new CBrush;
	pStaticBrush->CreateSolidBrush(backGround);

	pDC->FillRect(rcOutside, pStaticBrush);

	delete pStaticBrush;


	CString strCurFont,strNextFont;
	if (GetCount() == 0)
	{
		// Restore State of context
		pDC->RestoreDC(nIndexDC);
		return;
	}
	GetText(lpDrawItemStruct->itemID,strCurFont);

	int nData = GetItemData(lpDrawItemStruct->itemID);
	
	int nX = rc.left; // Save for lines
	int nY = rc.top + 1; // Save for lines

	CRect rcText = rc;
	CSize szText;
	
	if (nData == 0)
	{
		// calc for folder text placement
		rcText.left += 19;
		rcText.top = rcText.top + 2;
	}
	else
	{
		::GetTextExtentPoint32(pDC->m_hDC, strCurFont, strCurFont.GetLength(), &szText);
	
		// calc for dwg file text placement
		rcText.left += 18;
		rcText.top = rcText.top + 2;
		//rcText.top += m_RowHeight - szText.cy;		
	}
	
	CFont *pOldHeadingFont = pDC->SelectObject(GetFont());
	
	pDC->SetTextColor(textColor);
	
	// draw the text
	LPCTSTR lpszString;
	lpszString = strCurFont;
	
	int m_NextHeight = ::DrawText(pDC->m_hDC, lpszString, strCurFont.GetLength(), &rcText, DT_TOP|DT_LEFT);
	
	if (nData == 0)
	{
		// paint the folder icon
		POINT pt;
		pt.x = 1;
		pt.y = rcText.top - 1;
		int n = m_imageList.GetImageCount();
		m_imageList.Draw(pDC, 0, pt, ILD_NORMAL);
	}
	else
	{
		pDC->SetBkColor(backGround);
		CPoint pt(18, rcText.top + szText.cy + 1);
	
		POSITION pos = m_ThumbnailFileList.GetHeadPosition();
		while (pos != NULL)
		{
			ThumbnailFile *pTFile = m_ThumbnailFileList.GetNext(pos);
			if (pTFile->m_sFileName == strCurFont)
			{
				pTFile->DrawBitmap(pDC, pt, rc.Width() - pt.x, rc.bottom - pt.y - 2, mhR14LogoLarge, mhR14LogoSmall);
				break;
			}

		}		
	}

	pDC->SelectObject(pOldHeadingFont);
	
	// Restore State of context
	pDC->RestoreDC(nIndexDC);
	
}

BOOL CArxDwgListCtrl::PreTranslateMessage(MSG* pMsg) 
{
	GetToolTipCtrl().RelayEvent(pMsg);
	return __super::PreTranslateMessage(pMsg);
}

void CArxDwgListCtrl::OnDblclk() 
{
	CString sSelText;
	
	int nSel = GetCurSel();
	GetText(nSel, sSelText);
	if (GetItemData(nSel) == 0)
	{
		msPath.TrimRight( _T("\\/") );
		msPath	+= _T("\\");
		msPath += sSelText;
		Dir(msPath);
		if (m_pDirComboBox != NULL)
		{
			m_pDirComboBox->AddPath(msPath);
		}	
		GetArxServices()->HandleEvent( Prop::EventFolderChanged, false, args_S( msPath ) );
	}
	GetArxServices()->HandleEvent( Prop::EventDblClicked );
}


void CArxDwgListCtrl::OnDestroy() 
{
	__super::OnDestroy();
	ClearThumbnailList();
}

int CArxDwgListCtrl::HitTest(CPoint point) 
{
	CRect rc;

	for (int i=0; i<GetCount(); i++)
	{
		GetItemRect(i, &rc);

		if (rc.PtInRect(point) == TRUE)
			return i;
	}

	return -1;
}


void CArxDwgListCtrl::OnSetFocus(CWnd* pOldWnd) 
{
	__super::OnSetFocus(pOldWnd);
	GetArxServices()->HandleEvent( Prop::EventSetFocus );
}


void CArxDwgListCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	GetArxServices()->HandleEvent( Prop::EventMouseMove, args_NNN( nFlags, point.x, point.y ) );
	__super::OnMouseMove(nFlags, point);
}

void CArxDwgListCtrl::OnKillFocus(CWnd* pNewWnd) 
{
	__super::OnKillFocus(pNewWnd);
	GetArxServices()->HandleEvent( Prop::EventKillFocus );
}

void CArxDwgListCtrl::OnCbnSelchange() 
{
	__super::OnCbnSelchange();

	int nSelCount = GetSelCount();
	
	if (nSelCount > -1)
		GetArxServices()->HandleEvent( Prop::EventSelChanged, args_NS( nSelCount, _T("") ) );
	else
	{
		CString sText;
		int nCurSel = GetCurSel();
		if( nCurSel >= 0 )
			GetText( nCurSel, sText );
		GetArxServices()->HandleEvent( Prop::EventSelChanged, args_NS( 1, sText ) );
	}   	
}
