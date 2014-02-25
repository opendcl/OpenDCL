// ArxDwgListCtrl.h : header file
//

#pragma once

#include "RefCountedPtr.h"


class CDwgThumbnail
{
private:
	HBITMAP mhR14LogoLarge;
	HBITMAP mhR14LogoSmall;
protected:
	CString msDwgFilePath;
	bool mbLoaded;
	HBITMAP mhBitmap;
	int mnHeight;
	int mnWidth;

public:
	CDwgThumbnail()
		: mbLoaded( true )
		, mhBitmap( NULL )
		, mnHeight( 0 )
		, mnWidth( 0 )
	{
	}
	CDwgThumbnail( LPCTSTR pszDwgFilePath )
		: msDwgFilePath( pszDwgFilePath )
		, mbLoaded( false )
		, mhBitmap( NULL )
		, mnHeight( 0 )
		, mnWidth( 0 )
	{
	}
	~CDwgThumbnail()
	{
		if (mhBitmap)
			DeleteObject(mhBitmap);
	}

public:
	void Render(CDC* pDC, const CRect& rcCanvas);
	CSize GetBitmapSize();

private:
	bool Load();
};

typedef RefCountedPtr< CDwgThumbnail > CDwgThumbnailPtr;
