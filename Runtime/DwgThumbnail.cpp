// ArxDwgListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "DwgThumbnail.h"
#include "Resource.h"
#include "Workspace.h"
#include "png.h"


static HBITMAP GetDefaultPreviewLogoLarge()
{
	static class ResBmp
	{
		HBITMAP mhBmp;
	public:
		ResBmp( UINT idRes ) : mhBmp( (HBITMAP)::LoadImage(theWorkspace.GetLocalResourceModule(), MAKEINTRESOURCE(idRes), IMAGE_BITMAP, 0, 0, 0) ) {}
		~ResBmp() { DeleteObject( mhBmp ); }
		operator HBITMAP() const { return mhBmp; }
	} hR14LogoLarge( IDB_R14LARGE );
	return hR14LogoLarge;
}

static HBITMAP GetDefaultPreviewLogoSmall()
{
	static class ResBmp
	{
		HBITMAP mhBmp;
	public:
		ResBmp( UINT idRes ) : mhBmp( (HBITMAP)::LoadImage(theWorkspace.GetLocalResourceModule(), MAKEINTRESOURCE(idRes), IMAGE_BITMAP, 0, 0, 0) ) {}
		~ResBmp() { DeleteObject( mhBmp ); }
		operator HBITMAP() const { return mhBmp; }
	} hR14LogoSmall( IDB_R14SMALL );
	return hR14LogoSmall;
}

bool CDwgThumbnail::Load()
{
	if( mbLoaded )
		return true;

	static const int DWG_OFFSET = 13;
	static const int SENTINEL_LENGTH = 16;
	static const int TITLE_THUMB = 1;
	static const int BMP_THUMB = 2;
	static const int PNG_THUMB = 6;

	CFile fileDwg;
	CFileException e;
	if( !fileDwg.Open( msDwgFilePath, CFile::modeRead | CFile::typeBinary| CFile::shareDenyNone , &e ) )
		return false;

	mbLoaded = true;
	TRY
	{
		// seek to start of Thumbnail-Data
		fileDwg.Seek( DWG_OFFSET, CFile::begin );
		LONG lOffset = 0;
		fileDwg.Read( &lOffset, sizeof(lOffset) );         
		fileDwg.Seek( lOffset, CFile::begin );

		// read sentinel, but ignore it
		BYTE cbSentinel[SENTINEL_LENGTH];
		fileDwg.Read (cbSentinel, SENTINEL_LENGTH);

		ULONG lSize;
		fileDwg.Read (&lSize, sizeof(lSize)); //not used

		// check each thumbnail entry
		BYTE ctThumbs;
		fileDwg.Read (&ctThumbs, 1);
		// iterate the entries
		for (unsigned char idxThumb = 0; idxThumb < ctThumbs; ++idxThumb)
		{
			BYTE nThumbType;
			fileDwg.Read (&nThumbType, 1); //thumbnail type
			// Get Offset and Size in DWG
			fileDwg.Read( &lOffset, sizeof(lOffset) );
			fileDwg.Read( &lSize, sizeof(lSize) );

			switch (nThumbType)
			{
			case TITLE_THUMB:
				break;
			case BMP_THUMB:
				{
					LONGLONG lOld = (LONGLONG)fileDwg.GetPosition();
					fileDwg.Seek( lOffset, CFile::begin );
					HGLOBAL hDIB = GlobalAlloc( GMEM_FIXED, lSize );
					void* pDIB = (void*)hDIB;
					if( fileDwg.Read( pDIB, lSize ) != lSize )
					{
						GlobalFree( hDIB );
						return false;
					}
					fileDwg.Seek( lOld, CFile::begin );

					BITMAPINFO& bmInfo = *(LPBITMAPINFO)pDIB;
					BITMAPINFOHEADER& bmiHeader = bmInfo.bmiHeader;

					// If bmiHeader.biClrUsed is zero we have to infer the number
					// of colors from the number of bits used to specify it.
					int nColors = bmiHeader.biClrUsed ? bmiHeader.biClrUsed : (1 << bmiHeader.biBitCount);

					LPVOID lpDIBBits;
					if( bmInfo.bmiHeader.biBitCount > 8 )
						lpDIBBits = (LPVOID)((LPDWORD)(bmInfo.bmiColors + bmInfo.bmiHeader.biClrUsed) + 
						((bmInfo.bmiHeader.biCompression == BI_BITFIELDS) ? 3 : 0));
					else
						lpDIBBits = (LPVOID)(bmInfo.bmiColors + nColors);

					// Create the palette
					if( nColors <= 256 )
					{
						RGBQUAD *rgbStd = new RGBQUAD[nColors] ;
						for( int idx = 0; idx < nColors; ++idx )
						{
							// here we switch the background color from white to black
							if (bmInfo.bmiColors[idx].rgbBlue == 255 &&
								bmInfo.bmiColors[idx].rgbGreen == 255 &&
								bmInfo.bmiColors[idx].rgbRed == 255)
							{	
								rgbStd[idx].rgbBlue = 0;
								rgbStd[idx].rgbGreen = 0;
								rgbStd[idx].rgbRed = 0;
								rgbStd[idx].rgbReserved = 0;
							}
							// here we switch the foreground color from black to white
							else if (bmInfo.bmiColors[idx].rgbBlue == 0 &&
								bmInfo.bmiColors[idx].rgbGreen == 0 &&
								bmInfo.bmiColors[idx].rgbRed == 0)							
							{	
								rgbStd[idx].rgbBlue = 255;
								rgbStd[idx].rgbGreen = 255;
								rgbStd[idx].rgbRed = 255;
								rgbStd[idx].rgbReserved = 0;
							}
							else
							{
								rgbStd[idx].rgbBlue = bmInfo.bmiColors[idx].rgbBlue;
								rgbStd[idx].rgbGreen = bmInfo.bmiColors[idx].rgbGreen;
								rgbStd[idx].rgbRed = bmInfo.bmiColors[idx].rgbRed;
								rgbStd[idx].rgbReserved = 0;
							}
						}
						memcpy( bmInfo.bmiColors, rgbStd, sizeof(RGBQUAD) * nColors ); 
						delete rgbStd;
					}

					CClientDC dc(NULL);
					mhBitmap = CreateDIBitmap( dc.m_hDC,		// handle to device context 
						&bmiHeader,	// pointer to bitmap size and format data 
						CBM_INIT,	// initialization flag 
						lpDIBBits,	// pointer to initialization data 
						&bmInfo,	// pointer to bitmap color-format data 
						DIB_RGB_COLORS );		// color-data usage 

					// store the height and width of the bmp
					mnWidth = bmiHeader.biWidth;
					mnHeight = bmiHeader.biHeight;

					GlobalFree( hDIB );
				}
				break;
			case PNG_THUMB:
				{
					LONGLONG lOld = (LONGLONG)fileDwg.GetPosition();
					fileDwg.Seek( lOffset, CFile::begin );
					HGLOBAL hPNG = GlobalAlloc( GMEM_FIXED, lSize );
					void* pPNG = (void*)hPNG;
					if( fileDwg.Read( pPNG, lSize ) != lSize )
					{
						GlobalFree( hPNG );
						break;
					}
					fileDwg.Seek( lOld, CFile::begin );

					png_image pngInfo =
					{
						NULL,
						PNG_IMAGE_VERSION,
					};
					int nResult = png_image_begin_read_from_memory( &pngInfo, pPNG, lSize );
					if( !nResult )
					{
						GlobalFree( hPNG );
						break;
					}

					pngInfo.format = PNG_FORMAT_BGRA_COLORMAP;
					size_t nStride = PNG_IMAGE_ROW_STRIDE( pngInfo );
					size_t cbImage = PNG_IMAGE_SIZE(pngInfo);
					size_t cbColorMap = PNG_IMAGE_COLORMAP_SIZE(pngInfo);
					BYTE* pDIB = (BYTE*)malloc( sizeof(BITMAPINFOHEADER) + cbColorMap + cbImage );
					memset( pDIB, 0, sizeof(BITMAPINFOHEADER) + cbColorMap + cbImage );
					png_bytep pImageData = pDIB + sizeof(BITMAPINFOHEADER) + cbColorMap;
					png_bytep pColorMap = (cbColorMap > 0)? (pDIB + sizeof(BITMAPINFOHEADER)) : NULL;
					nResult = png_image_finish_read( &pngInfo, NULL, pImageData, nStride, pColorMap );
					if( nResult )
					{
						BITMAPINFO& bmiPngSource = *(BITMAPINFO*)pDIB;
						BITMAPINFOHEADER& bmihPngSource = bmiPngSource.bmiHeader;
						bmihPngSource.biSize = sizeof(BITMAPINFOHEADER);
						bmihPngSource.biWidth = pngInfo.width;
						bmihPngSource.biHeight = -pngInfo.height;
						bmihPngSource.biPlanes = 1;
						bmihPngSource.biBitCount = 8;
						bmihPngSource.biCompression = BI_RGB;
						bmihPngSource.biSizeImage = 0;
						bmihPngSource.biXPelsPerMeter = 0;
						bmihPngSource.biYPelsPerMeter = 0;
						bmihPngSource.biClrUsed = pngInfo.colormap_entries;
						bmihPngSource.biClrImportant = 0;

						CClientDC dc(NULL);
						mhBitmap = CreateDIBitmap( dc.m_hDC,		// handle to device context 
							&bmihPngSource,	// pointer to bitmap size and format data 
							CBM_INIT,	// initialization flag 
							pImageData,	// pointer to initialization data 
							&bmiPngSource,	// pointer to bitmap color-format data 
							DIB_RGB_COLORS );		// color-data usage 

						// store the height and width of the bmp
						mnWidth = bmihPngSource.biWidth;
						mnHeight = abs(bmihPngSource.biHeight);
					}

					free( pDIB );
					png_image_free( &pngInfo );
					GlobalFree( hPNG );
				}
				break;
			};
		}
	}
	CATCH (CFileException, e)
	{
		return false;
	}
	END_CATCH

	fileDwg.Close();
	return true;
}

CSize CDwgThumbnail::GetBitmapSize()
{
	Load();
	return CSize(mnWidth, mnHeight);
}

void CDwgThumbnail::Render(CDC* pDC, const CRect& rcCanvas)
{
	Load();

	CDC dcMem;
	VERIFY(dcMem.CreateCompatibleDC(pDC));

	CBrush StaticBrush(pDC->GetBkColor());
	pDC->FillRect(rcCanvas, &StaticBrush);
	StaticBrush.DeleteObject();

	if (!mhBitmap)
	{
		if (rcCanvas.Height() < 100)
		{
			HBITMAP* pBmpOld = (HBITMAP*)SelectObject(dcMem.m_hDC, GetDefaultPreviewLogoSmall());
			int nX = (rcCanvas.Width() - 53) / 2;
			int nY = (rcCanvas.Height() - 51) / 2;
			pDC->StretchBlt(rcCanvas.left + nX, rcCanvas.top + nY, 53, 51, &dcMem, 0, 0, 53, 51, SRCCOPY);
			SelectObject(dcMem.m_hDC, pBmpOld);
		}
		else
		{
			HBITMAP* pBmpOld = (HBITMAP*)SelectObject(dcMem.m_hDC, GetDefaultPreviewLogoLarge());
			int nX = (rcCanvas.Width() - 100) / 2;
			int nY = (rcCanvas.Height() - 98) / 2;
			pDC->StretchBlt(rcCanvas.left + nX, rcCanvas.top + nY, 100, 98, &dcMem, 0, 0, 100, 98, SRCCOPY);
			SelectObject(dcMem.m_hDC, pBmpOld);
		}
	}
	else 
	{
		CSize paintSize(rcCanvas.Width(), rcCanvas.Height());
		CSize drawSize(mnWidth, mnHeight);

		if( drawSize.cx > paintSize.cx )
		{
			LONG lNewY = LONG((double(drawSize.cy) * double(paintSize.cx) / double(drawSize.cx)) + 0.5);
			drawSize.SetSize( paintSize.cx, lNewY );
		}
		if( drawSize.cy > paintSize.cy )
		{
			LONG lNewX = LONG((double(drawSize.cx) * double(paintSize.cy) / double(drawSize.cy)) + 0.5);
			drawSize.SetSize( lNewX, paintSize.cy );
		}
		CPoint ptUL( rcCanvas.left + ((paintSize.cx - drawSize.cx) / 2), rcCanvas.top + ((paintSize.cy - drawSize.cy) / 2) );

		HBITMAP* pBmpOld = (HBITMAP*)::SelectObject( dcMem.m_hDC, mhBitmap );
		pDC->StretchBlt( ptUL.x, ptUL.y, drawSize.cx, drawSize.cy, &dcMem, 0, 0, mnWidth, mnHeight, SRCCOPY );
		::SelectObject( dcMem.m_hDC, pBmpOld );
	}
	dcMem.DeleteDC();
}
