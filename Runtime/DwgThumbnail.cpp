// ArxDwgListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "DwgThumbnail.h"
#include "Resource.h"
#include "Workspace.h"
#include "png.h"


static HBITMAP CreateDIBSection( BITMAPINFO& bmInfo )
{
	BITMAPINFOHEADER& bmiHeader = bmInfo.bmiHeader;
	if( bmiHeader.biCompression != BI_RGB )
		return NULL; //only uncompressed bitmaps are supported

	// If bmiHeader.biClrUsed is zero we have to infer the number
	// of colors from the number of bits used to specify it.
	int nColors = bmiHeader.biClrUsed ? bmiHeader.biClrUsed : (1 << bmiHeader.biBitCount);

	LPVOID lpDIBBits;
	if( bmInfo.bmiHeader.biBitCount > 8 )
		lpDIBBits = (LPVOID)((LPDWORD)(bmInfo.bmiColors + bmInfo.bmiHeader.biClrUsed) + 
		((bmInfo.bmiHeader.biCompression == BI_BITFIELDS) ? 3 : 0));
	else
		lpDIBBits = (LPVOID)(bmInfo.bmiColors + nColors);

	BITMAPINFO bmiARGB = 
	{
		sizeof(BITMAPINFOHEADER),
		bmiHeader.biWidth,
		bmiHeader.biHeight,
		1,
		32,
		BI_RGB,
	};
	COLORREF* rcrDest = NULL;
	HBITMAP hBitmap = CreateDIBSection( NULL, &bmiARGB, DIB_RGB_COLORS, (LPVOID*)&rcrDest, NULL, 0 );
	if( !hBitmap )
		return NULL;
	if( bmiHeader.biBitCount <= 8 )
	{
		BYTE* pSrcPixel = (LPBYTE)lpDIBBits;
		RGBQUAD* rcrSrc = bmInfo.bmiColors;
		COLORREF* pDestPixel = &rcrDest[0];
		BYTE idxBackground = *pSrcPixel;
		for( size_t row = abs(bmiHeader.biHeight); row > 0; --row )
		{
			pSrcPixel = (LPBYTE)(((DWORD_PTR)pSrcPixel - 1) & ~(DWORD_PTR)3) + 4; //align on 4-byte boundary
			for( size_t col = bmiHeader.biWidth; col > 0; --col )
			{
				BYTE idxSrc = *pSrcPixel;
				DWORD dwAlpha = (idxSrc == idxBackground)? 0 : 0xFF000000;
				*pDestPixel = *(COLORREF*)(&rcrSrc[idxSrc]) | dwAlpha;
				++pSrcPixel;
				++pDestPixel;
			}
		}
	}
	else if( bmiHeader.biBitCount == 32 )
	{
		COLORREF* pSrcPixel = (COLORREF*)lpDIBBits;
		COLORREF* pDestPixel = &rcrDest[0];
		COLORREF crBackground = RGB(GetRValue(*pSrcPixel), GetGValue(*pSrcPixel), GetBValue(*pSrcPixel));
		for( size_t row = abs(bmiHeader.biHeight); row > 0; --row )
		{
			for( size_t col = bmiHeader.biWidth; col > 0; --col )
			{
				if( (*pSrcPixel & 0xFF000000) != 0xFF000000 )
					*pDestPixel = (*pSrcPixel & 0xFFFFFF);
				else if( (*pSrcPixel & 0xFFFFFF) == crBackground )
					*pDestPixel = crBackground;
				else
					*pDestPixel = *pSrcPixel;
				++pSrcPixel;
				++pDestPixel;
			}
		}
	}
	return hBitmap;
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
		fileDwg.Read( cbSentinel, SENTINEL_LENGTH );

		ULONG ulSize;
		fileDwg.Read( &ulSize, sizeof(ulSize) ); //not used

		// check each thumbnail entry
		BYTE ctThumbs;
		fileDwg.Read (&ctThumbs, 1);
		// iterate the entries
		for( unsigned char idxThumb = 0; idxThumb < ctThumbs; ++idxThumb )
		{
			BYTE nThumbType;
			fileDwg.Read (&nThumbType, 1); //thumbnail type

			// get thumbnail file offset and size
			fileDwg.Read( &lOffset, sizeof(lOffset) );
			fileDwg.Read( &ulSize, sizeof(ulSize) );

			switch (nThumbType)
			{
			case TITLE_THUMB:
				break;
			case BMP_THUMB:
				{
					LONGLONG lOld = (LONGLONG)fileDwg.GetPosition();
					fileDwg.Seek( lOffset, CFile::begin );
					HGLOBAL hDIB = LocalAlloc( GMEM_FIXED, ulSize );
					void* pDIB = (void*)hDIB;
					if( fileDwg.Read( pDIB, ulSize ) != ulSize )
					{
						LocalFree( hDIB );
						return false;
					}
					fileDwg.Seek( lOld, CFile::begin );

					BITMAPINFO& bmInfo = *(LPBITMAPINFO)pDIB;
					mhBitmap = CreateDIBSection( bmInfo );

					// store the height and width of the bmp
					mnWidth = bmInfo.bmiHeader.biWidth;
					mnHeight = abs(bmInfo.bmiHeader.biHeight);

					LocalFree( hDIB );
				}
				break;
			case PNG_THUMB:
				{
					LONGLONG lOld = (LONGLONG)fileDwg.GetPosition();
					fileDwg.Seek( lOffset, CFile::begin );
					HGLOBAL hPNG = GlobalAlloc( GMEM_FIXED, ulSize );
					void* pPNG = (void*)hPNG;
					if( fileDwg.Read( pPNG, ulSize ) != ulSize )
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
					int nResult = png_image_begin_read_from_memory( &pngInfo, pPNG, ulSize );
					if( !nResult )
					{
						GlobalFree( hPNG );
						break;
					}

					pngInfo.format = PNG_FORMAT_BGRA;
					png_int_32 nStride = PNG_IMAGE_ROW_STRIDE( pngInfo );
					size_t cbImage = PNG_IMAGE_SIZE(pngInfo);
					BYTE* pDIB = (BYTE*)malloc( sizeof(BITMAPINFOHEADER) + cbImage );
					memset( pDIB, 0, sizeof(BITMAPINFOHEADER) + cbImage );
					png_bytep pImageData = pDIB + sizeof(BITMAPINFOHEADER);
					nResult = png_image_finish_read( &pngInfo, NULL, pImageData, -nStride, NULL );
					if( nResult )
					{
						BITMAPINFO& bmiPngSource = *(BITMAPINFO*)pDIB;
						BITMAPINFOHEADER& bmihPngSource = bmiPngSource.bmiHeader;
						bmihPngSource.biSize = sizeof(BITMAPINFOHEADER);
						bmihPngSource.biWidth = (LONG)pngInfo.width;
						bmihPngSource.biHeight = (LONG)pngInfo.height;
						bmihPngSource.biPlanes = 1;
						bmihPngSource.biBitCount = 32;
						bmihPngSource.biCompression = BI_RGB;
						bmihPngSource.biSizeImage = cbImage;
						bmihPngSource.biXPelsPerMeter = 0;
						bmihPngSource.biYPelsPerMeter = 0;
						bmihPngSource.biClrUsed = 0;
						bmihPngSource.biClrImportant = 0;

						mhBitmap = CreateDIBSection( bmiPngSource );

						// store the height and width of the bmp
						mnWidth = bmiPngSource.bmiHeader.biWidth;
						mnHeight = abs(bmiPngSource.bmiHeader.biHeight);
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

void CDwgThumbnail::Render(CDC* pDC, const CRect& rcCanvas, COLORREF clrBackground)
{
	Load();

	CDC dcMem;
	VERIFY(dcMem.CreateCompatibleDC(pDC));

	pDC->FillSolidRect( rcCanvas, clrBackground );
	dcMem.SetBkColor( clrBackground );

	if (mhBitmap)
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

		//This block of code enlarges the bitmap up if there is room. The results of enlarging are not
		//very visually appealing, so this is here only as historical test code for posterity. [ORW 2014-03-30]
		//
		//if( drawSize.cx < paintSize.cx && drawSize.cy < paintSize.cy )
		//{
		//	int dAspect = (drawSize.cx * paintSize.cy) / (drawSize.cy * paintSize.cx);
		//	if( dAspect > 0 )
		//	{
		//		LONG lNewY = LONG((double(drawSize.cy) * double(paintSize.cx) / double(drawSize.cx)) + 0.5);
		//		drawSize.SetSize( paintSize.cx, lNewY );
		//	}
		//	else if( dAspect < 0 )
		//	{
		//		LONG lNewX = LONG((double(drawSize.cx) * double(paintSize.cy) / double(drawSize.cy)) + 0.5);
		//		drawSize.SetSize( lNewX, paintSize.cy );
		//	}
		//}

		CPoint ptUL( rcCanvas.left + ((paintSize.cx - drawSize.cx) / 2), rcCanvas.top + ((paintSize.cy - drawSize.cy) / 2) );

		DIBSECTION bmpARGB = { 0 };
		if( sizeof(bmpARGB) == GetObject( mhBitmap, sizeof(bmpARGB), &bmpARGB ) )
		{
			BITMAPINFO bmiBlended = 
			{
				sizeof(BITMAPINFOHEADER),
				mnWidth,
				mnHeight,
				1,
				32,
				BI_RGB,
			};

			RGBQUAD* rcrDest = NULL;
			HBITMAP hBlendedBmp = CreateDIBSection( dcMem.m_hDC, &bmiBlended, DIB_RGB_COLORS, (LPVOID*)&rcrDest, NULL, 0 );

			RGBQUAD crBack = { GetBValue(clrBackground), GetGValue(clrBackground), GetRValue(clrBackground), 0xFF };
			RGBQUAD* rcrSrc = (RGBQUAD*)bmpARGB.dsBm.bmBits;
			RGBQUAD* pSrcPixel = &rcrSrc[0];
			RGBQUAD* pDestPixel = &rcrDest[0];
			for( size_t row = abs(mnHeight); row > 0; --row )
			{
				for( size_t col = mnWidth; col > 0; --col )
				{
					RGBQUAD crSrc = *pSrcPixel;
					if( *(DWORD*)&crSrc == *(DWORD*)&crBack )
						*(DWORD*)pDestPixel = (0xFF000000 | ~*(DWORD*)pDestPixel); //invert pixels that match background color
					else if( crSrc.rgbReserved != 0 )
						*pDestPixel = crSrc; //all non-zero alpha values are treated as opaque
					else
						*(DWORD*)pDestPixel = *(DWORD*)&crBack;
					++pSrcPixel;
					++pDestPixel;
				}
			}

			HBITMAP hOldBmp = (HBITMAP)dcMem.SelectObject( hBlendedBmp );
			int nOldStretchMode = pDC->SetStretchBltMode( HALFTONE );
			POINT ptOldBrushOrg;
			BOOL bBrushOrgSet = SetBrushOrgEx( pDC->m_hDC, 0, 0, &ptOldBrushOrg );
			pDC->StretchBlt( ptUL.x, ptUL.y, drawSize.cx, drawSize.cy, &dcMem, 0, 0, mnWidth, mnHeight, SRCCOPY );
			pDC->SetStretchBltMode( nOldStretchMode );
			if( bBrushOrgSet )
				SetBrushOrgEx( pDC->m_hDC, ptOldBrushOrg.x, ptOldBrushOrg.y, &ptOldBrushOrg );
			dcMem.SelectObject( hOldBmp );
			DeleteObject( hBlendedBmp );
		}
		else
		{
			HBITMAP hOldBmp = (HBITMAP)dcMem.SelectObject( mhBitmap );
			int nOldStretchMode = pDC->SetStretchBltMode( HALFTONE );
			POINT ptOldBrushOrg;
			BOOL bBrushOrgSet = SetBrushOrgEx( pDC->m_hDC, 0, 0, &ptOldBrushOrg );
			pDC->StretchBlt( ptUL.x, ptUL.y, drawSize.cx, drawSize.cy, &dcMem, 0, 0, mnWidth, mnHeight, SRCCOPY );
			pDC->SetStretchBltMode( nOldStretchMode );
			if( bBrushOrgSet )
				SetBrushOrgEx( pDC->m_hDC, ptOldBrushOrg.x, ptOldBrushOrg.y, &ptOldBrushOrg );
			dcMem.SelectObject( hOldBmp );
		}
	}
	dcMem.DeleteDC();
}
