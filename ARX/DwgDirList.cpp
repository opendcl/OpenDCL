// DwgDirList.cpp : implementation file
//

#include "stdafx.h"
#include "DwgDirList.h"
#include "Resource.h"
#include "InvokeMethod.h"
#include "ComboBoxFolder.h"
#include "SADirRead.h"
#include "PropertyIds.h"
#include "DclControlObject.h"
#include "PropertyObject.h"

const TCHAR *sBackSlash2 = _T("\\");
const TCHAR sDwg[] = _T("*.dwg");


BOOL ThumbnailFile::LoadDwgThumbnail()
{
	if (m_bLoaded == true)
		return FALSE;

	// reset old values
	CString sFileName;
	if (m_pPath->Right(1) == sBackSlash2)
		sFileName = CString(*m_pPath) + m_sFileName;
	else
		sFileName = CString(*m_pPath) + sBackSlash2 + m_sFileName;
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
               DWORD lOld = (DWORD)f.GetPosition();
               f.Seek (lOffset, CFile::begin);
               f.Read(szTitle, lSize);
               szTitle[lSize] = '\0';
               m_szTitle = szTitle;
               f.Seek (lOld, CFile::begin);
            }
   // Bitmap (copied from some DIB examples)
            else if (nType == BMP_THUMB)
            {
				DWORD lOld = (DWORD)f.GetPosition();
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
	
	return CSize(0,19);
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
		
		CBrush *pStaticBrush = new CBrush;
		pStaticBrush->CreateSolidBrush(::GetSysColor(COLOR_WINDOW));
		CRect rc(pt.x, pt.y, pt.x + nPaintableWidth, pt.y + nPaintableHeight);

		pDC->FillRect(rc, pStaticBrush);
		
		delete pStaticBrush;

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
		
		if (nPaintableHeight < 110)
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
		
		//pDC->BitBlt(10,pt.y+10, 53, 51, &dcMem, 53, 51, SRCCOPY);

		/*
		CBitmap *poldbmp; 
		CDC memdc;
    
	    // Create a compatible memory DC
        memdc.CreateCompatibleDC( pDC );
        // Select the bitmap into the DC
        poldbmp = (CBitmap*)memdc.SelectObject( hR14Logo);
		
		// Copy (BitBlt) bitmap from memory DC to screen DC
        pDC->BitBlt( 10, pt.y+10,
			GetDeviceCaps(pDC->m_hDC, HORZRES), 
			GetDeviceCaps(pDC->m_hDC, VERTRES),                 
			&memdc, 0, 0, SRCCOPY );

        memdc.SelectObject( poldbmp );
		*/
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

		CBrush *pStaticBrush = new CBrush;
		pStaticBrush->CreateSolidBrush(::GetSysColor(COLOR_WINDOW));
		
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
			
		if (rcTop.Height() > 0)
			pDC->FillRect(rcTop, pStaticBrush);
		if (rcBottom.Height() > 0)
			pDC->FillRect(rcBottom, pStaticBrush);
		if (rcLeft.Width() > 0)
			pDC->FillRect(rcLeft, pStaticBrush);
		if (rcRight.Width() > 0)
			pDC->FillRect(rcRight, pStaticBrush);
		
		delete pStaticBrush;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDwgDirList

CDwgDirList::CDwgDirList()
{
	m_RowHeight = 120;
	m_pDirComboBox = NULL;
}

CDwgDirList::~CDwgDirList()
{
}


BEGIN_MESSAGE_MAP(CDwgDirList, CListBox)
	//{{AFX_MSG_MAP(CDwgDirList)
	ON_CONTROL_REFLECT(LBN_DBLCLK, OnDblclk)
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_CONTROL_REFLECT(LBN_SELCHANGE, OnSelchange)
	ON_WM_SIZE()
	ON_WM_MEASUREITEM_REFLECT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDwgDirList message handlers


BOOL CDwgDirList::Create(CDclControlObject* pControl, CWnd* pParentWnd, UINT nID ) 
{
	CRect ArxRect;

	// set the arx control pointer
    m_ArxControl = pControl;
	
	// get the rectangle of the new control
	ArxRect.top = pControl->m_pTop->GetLongValue();
	ArxRect.left = pControl->m_pLeft->GetLongValue();
	ArxRect.bottom = pControl->m_pHeight->GetLongValue() + ArxRect.top;
	ArxRect.right = pControl->m_pWidth->GetLongValue() + ArxRect.left;
		
	m_RowHeight = pControl->GetLngProperty(nRowHeight);
	if (m_RowHeight == -1) 
		m_RowHeight = 120;

	DWORD dwStyle = WS_CHILD | WS_VISIBLE | LBS_OWNERDRAWVARIABLE
		| LBS_HASSTRINGS | LBS_NOTIFY |WS_CLIPSIBLINGS;
	
	if (pControl->GetBoolProperty(nNoIntegralHeight) == TRUE)
		dwStyle = dwStyle | LBS_NOINTEGRALHEIGHT;
	

	if (pControl->GetBoolProperty(nVScrollBar) == TRUE)
		dwStyle = dwStyle | WS_VSCROLL;	

	if (pControl->GetBoolProperty(nIsTabStop) != FALSE)
		dwStyle = dwStyle | WS_TABSTOP;
	else
		dwStyle = dwStyle | WS_GROUP;

	// get the selection style and apply the appropriate setting
	switch (pControl->GetLngProperty(nSelectStyle))
	{
	case 1:
		dwStyle = dwStyle | LBS_EXTENDEDSEL;
		break;
	case 2:
		dwStyle = dwStyle | LBS_MULTIPLESEL;
		break;
	}

	BOOL RetVal = CClrListBox::Create(dwStyle,ArxRect, pParentWnd, nID);
	VERIFY(CClrListBox::SubclassDlgItem(nID, pParentWnd));

	switch (m_ArxControl->GetLngProperty(nEventInvoke))
	{
	case 1:
		m_bInvokeWithSendString = true;
		break;
	default:
		m_bInvokeWithSendString = false;
		break;
	}
	// call the method to create the image list
	CreateImageList();

	
	m_hR14LogoLarge = (HBITMAP)::LoadImage(_hdllInstance, MAKEINTRESOURCE(IDB_R14LARGE), IMAGE_BITMAP, 0, 0, 0);
	m_hR14LogoSmall = (HBITMAP)::LoadImage(_hdllInstance, MAKEINTRESOURCE(IDB_R14SMALL), IMAGE_BITMAP, 0, 0, 0);

	return RetVal;
}


void CDwgDirList::CreateImageList()
{
	CBitmap bitmap;
	bitmap.LoadBitmap(IDB_FOLDER);
	m_imageList.Create(16, 16, ILC_COLOR24, 10, 5);
	m_imageList.Add(&bitmap, RGB(0,0,0));
	
}

void CDwgDirList::ClearThumbnailList()
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
void CDwgDirList::SetRowHeight(int nTheRowHeight) 
{
	if (nTheRowHeight > 250)
		nTheRowHeight = 250;

	POSITION pos = m_ThumbnailFileList.GetHeadPosition();
	int nCount = 0;
	int nBiggestBmp = 0;
	while (pos != NULL)
	{
		ThumbnailFile *pFileObj = m_ThumbnailFileList.GetNext(pos);
		if (pFileObj->m_sFileName.GetLength() == 0)
			continue;

		if (nCount < 10)
		{
			if (nBiggestBmp < pFileObj->GetBitmapSize().cy)
			{
				nBiggestBmp = pFileObj->GetBitmapSize().cy;
			}
			nCount++;
		}
		else
			break;
	}

	if (nBiggestBmp > 0 && nBiggestBmp+19 < nTheRowHeight)
		m_RowHeight = nBiggestBmp+19;
	else
		m_RowHeight = nTheRowHeight;

	for (int i=0; i<GetCount(); i++)
	{
		int nData = GetItemData(i);
		if (nData >= 1)
			SetItemHeight(i, m_RowHeight);
	}
	m_ArxControl->SetLongProperty(nRowHeight, m_RowHeight);
	Invalidate();
}

	
	
void CDwgDirList::Dir(CString sDir) 
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

	//dr.Recurse() = false;    // scan subdirs ?

	m_sPath = sDir;

	dr.ClearDirs();         // start clean
	if (!dr.GetDirs(sDir, true)) // get all folders 
		return; // exit if nothing found

	// dump the directory list to the debug trace window:

	// get the dir array
	CSADirRead::SADirVector &dirs = dr.Dirs();
	
	// loop over it
	for (CSADirRead::SADirVector::const_iterator dit = dirs.begin(); dit!=dirs.end(); dit++)
	{
		TRACE("%s\n", (*dit).m_sName);		
		CString sFileName = (*dit).m_sName;
		
		if (sDir == sFileName)
			continue;
		int nSlash = sFileName.ReverseFind(*sBackSlash2);
		sFileName = sFileName.Mid(nSlash+1);
		
		if (sDir == (sFileName + sBackSlash2))
			continue;
		
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

	dr.GetFiles(sDwg);   // get all *.DWG files 

	// get the file array
	CSADirRead::SAFileVector &files = dr.Files();   

	int nDirCount = GetCount();
	bool bLoadedFirst = false;

	// dump it...
	for (CSADirRead::SAFileVector::const_iterator fit = files.begin(); fit!=files.end(); fit++)
	{
		TRACE("%s\n", (*fit).m_sName);
		CString sFileName = (*fit).m_sName;
		int nSlash = sFileName.ReverseFind(*sBackSlash2);
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
					SetItemHeight(nIndex, m_RowHeight);
					ThumbnailFile *BlankBitmap = new ThumbnailFile(&m_sPath, sFileName);
					BlankBitmap->nIndex = nIndex;
					m_ThumbnailFileList.AddTail(BlankBitmap);
					bInserted = true;
					j = nCount;
				}
			}
		}
		if (!bInserted)
		{
			int nIndex = AddString(sFileName);
			SetItemData(nIndex, 1);
			ThumbnailFile *BlankBitmap = new ThumbnailFile(&m_sPath, sFileName);
			BlankBitmap->nIndex = nIndex;
			m_ThumbnailFileList.AddTail(BlankBitmap);
			if (!bLoadedFirst)
			{
				bLoadedFirst = true;
				BlankBitmap->LoadDwgThumbnail();
				//CalcRowHeight(BlankBitmap);
			}			
			SetItemData(nIndex, 1);
			SetItemHeight(nIndex, m_RowHeight);
			
		}
	}	
	int nIndex = AddString(CString());
	SetItemData(nIndex, m_RowHeight);
	DeleteString(nIndex);

	dr.ClearFiles();		// start clean, again

	// we need to reset the row height now.
	SetRowHeight(m_RowHeight);

	return;
}


void CDwgDirList::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	ASSERT(lpDrawItemStruct->CtlType == ODT_LISTBOX); // We've gotta be a combo

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
		CPoint pt(1, rcText.top + szText.cy + 1);
	
		POSITION pos = m_ThumbnailFileList.GetHeadPosition();
		while (pos != NULL)
		{
			ThumbnailFile *pTFile = m_ThumbnailFileList.GetNext(pos);
			if (pTFile->m_sFileName == strCurFont)
			{
				pTFile->DrawBitmap(pDC, pt, rc.Width(), m_RowHeight-19, m_hR14LogoLarge, m_hR14LogoSmall);
				break;
			}

		}		
	}

	pDC->SelectObject(pOldHeadingFont);
	
	// Restore State of context
	pDC->RestoreDC(nIndexDC);
	
}


void CDwgDirList::SetTooltipText(CString* spText, BOOL bActivate)
{

} // End of SetTooltipText

void CDwgDirList::InitToolTip()
{
	if (m_ToolTip.m_hWnd == NULL)
	{
		// Create ToolTip control
		m_ToolTip.Create(this);
		// Create inactive
		m_ToolTip.Activate(FALSE);
	}
} // End of InitToolTip


BOOL CDwgDirList::PreTranslateMessage(MSG* pMsg) 
{
	InitToolTip();
	m_ToolTip.RelayEvent(pMsg);

	
	return CListBox::PreTranslateMessage(pMsg);
}

void CDwgDirList::OnDblclk() 
{
	CString sSelText;
	
	int nSel = GetCurSel();
	GetText(nSel, sSelText);
	if (GetItemData(nSel) == 0)
	{	
		if (m_sPath.Right(1) == sBackSlash2)
			m_sPath	+= sSelText;
		else
		{
			m_sPath	+= sBackSlash2;
			m_sPath += sSelText;
		}
		Dir(m_sPath);
		if (m_pDirComboBox != NULL)
		{
			m_pDirComboBox->AddPath(m_sPath);
		}	
		InvokeMethodString(m_ArxControl->GetStrProperty(nEventFolderChanged), m_sPath, false);		
	}
	InvokeMethod(m_ArxControl->GetStrProperty(nEventDblClicked), m_bInvokeWithSendString);
	
}


void CDwgDirList::OnDestroy() 
{
	m_DropTarget.Revoke();

	CListBox::OnDestroy();
	
	ClearThumbnailList();
}


void CDwgDirList::SetDragnDrop(BOOL bRegister)
{
	if (bRegister == TRUE)
	{
		BOOL success = m_DropTarget.Register(this);
		m_DropTarget.m_pThisArxControl = m_ArxControl;
		m_DropTarget.m_pParent = this;
    }
	else
		m_DropTarget.Revoke();
}
int CDwgDirList::HitTest(CPoint point) 
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


void CDwgDirList::OnLButtonDown(UINT nFlags, CPoint point) 
{	
	//CListBox::OnLButtonDown(nFlags, point);
	int nIndex = HitTest(point);
	SetCurSel(nIndex);
	OnSelchange();

	Invalidate();    

	// Get the currently selected Item
    if(GetCurSel() == LB_ERR)
        return;
    
	if (m_ArxControl->GetBoolProperty(nDragnDropAllowBegin) == TRUE)
	{
		int nCount = GetSelCount();
		int nIndex = -1;
		
		CStringArray saBlockNames;
		saBlockNames.SetSize(0,1);
		// add the path
		saBlockNames.Add(m_sPath);
		
		CString sText;
		
		if (nCount == -1)
		{
			GetText(GetCurSel(), sText);
			
			saBlockNames.Add(sText);
		}
		else
		{
			CArray<int,int> aryListBoxSel;

			// get an array of all the selected line numbers
			aryListBoxSel.SetSize(nCount);
			GetSelItems(nCount, aryListBoxSel.GetData());

			for (int i=0; i<nCount; i++)
			{
				// get the text name of the selected line number
				CString sTextItem;
				GetText(aryListBoxSel[i], sTextItem);
				saBlockNames.Add(sTextItem);
			}
		}

		BeginDragnDropToInsertBlocks(m_ArxControl, point, m_bInvokeWithSendString, saBlockNames);
		
		// We need to send WM_LBUTTONUP to control or else the selection rectangle 
		// will "follow" the mouse (like when you hold the left mouse down and 
		// scroll through a regular listbox). WM_LBUTTONUP was sent to window that 
		// recieved the drag/drop, not the one that initiated it, so we simulate
		// an WM_LBUTTONUP to the initiating window.
		LPARAM lparam;
    
		// "Pack" lparam with x and y coordinates where lbuttondown originated
		lparam=point.y; 
		lparam=lparam<<16;
		lparam &= point.x;                         
    
		SendMessage(WM_LBUTTONUP,0,lparam);	
	}	
}

void CDwgDirList::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
//	CListBox::OnLButtonUp(nFlags, point);
}



void CDwgDirList::OnSetFocus(CWnd* pOldWnd) 
{
	CListBox::OnSetFocus(pOldWnd);
	
	// call methods to invoke the event
	InvokeMethod(m_ArxControl->GetStrProperty(nEventSetFocus), m_bInvokeWithSendString);
	
}


void CDwgDirList::OnMouseMove(UINT nFlags, CPoint point) 
{


	InvokeMethodIntIntInt(
		m_ArxControl->GetStrProperty(nEventMouseMove),
		nFlags,
		point.x,
		point.y,
		m_bInvokeWithSendString);
	
	
	CListBox::OnMouseMove(nFlags, point);
}
void CDwgDirList::OnKillFocus(CWnd* pNewWnd) 
{
	CListBox::OnKillFocus(pNewWnd);
	
	// call methods to invoke the event
	InvokeMethod(m_ArxControl->GetStrProperty(nEventKillFocus), m_bInvokeWithSendString);
	
}

void CDwgDirList::OnSelchange() 
{
	int nSelCount = GetSelCount();
	
	if (nSelCount > -1)
	{
		// call methods to invoke the event
		InvokeMethodIntString(m_ArxControl->GetStrProperty(nEventSelChanged), nSelCount, "", m_bInvokeWithSendString);
	}   

	if (nSelCount == -1)
	{
		int nCurSel = GetCurSel();
	
		CString sSelText = CString();
		
		if (nCurSel > -1)
		{
			int n = GetTextLen(nCurSel);      
			GetText(nCurSel, sSelText.GetBuffer(n));
			sSelText.ReleaseBuffer();
		}

		// call methods to invoke the event
		InvokeMethodIntString(m_ArxControl->GetStrProperty(nEventSelChanged), 1, sSelText, m_bInvokeWithSendString);
	}   	
}

void CDwgDirList::OnSize(UINT nType, int cx, int cy) 
{
	CListBox::OnSize(nType, cx, cy);
	CString str;
	CSize sz(0,0);

	CalcRowHeight(NULL, cx);
	/*
	POSITION pos = m_ThumbnailFileList.GetHeadPosition();
	while (pos != NULL)
	{
		ThumbnailFile *pTFile = m_ThumbnailFileList.GetNext(pos);
				
		GetText(pTFile->nIndex, str);

		if (pTFile->m_sFileName == str)
		{
			
			break;
		}
	}
	*/
}
void CDwgDirList::CalcRowHeight(ThumbnailFile *pTFile, int cx)
{
	if (cx == 0)
	{
		CRect rc;
		GetClientRect(&rc);
		cx = rc.Width();
	}

	int nNewHeight = 126 * cx;
	nNewHeight = nNewHeight / 180;
	nNewHeight += 19;

	SetRowHeight(nNewHeight);

	return;
	/*
	CSize sz = pTFile->GetBitmapSize();
			
	if (sz.cx == 0 || sz.cy < 19)
		SetRowHeight(25);
	else
	{
		int nNewHeight = sz.cy * cx;
		nNewHeight = nNewHeight / sz.cx;
		nNewHeight += 19;

		if (sz.cy+19 < nNewHeight)				
			SetRowHeight(sz.cy+19);
		else
			SetRowHeight(nNewHeight);
	}
	*/
}


void CDwgDirList::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	int n = GetItemData(lpMeasureItemStruct->itemID);

	CString sString;
	
	int n2 = GetTextLen(lpMeasureItemStruct->itemID);      
	GetText(lpMeasureItemStruct->itemID, sString.GetBuffer(n2));
	sString.ReleaseBuffer();

	if (_tcsicmp(sString.Right(4), _T(".dwg")) == 0)
		n = 1;

	if (n == 0)
		lpMeasureItemStruct->itemHeight = 17;
	else
		lpMeasureItemStruct->itemHeight = m_RowHeight;
	
}
