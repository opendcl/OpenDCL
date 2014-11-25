#include "StdAfx.h"
#include "DclImageList.h"
#include "ArchiveEx.h"
#include "Filing.h"
#include "Project.h"
#include "SafeImageListWrite.h"


CDclImageList::CDclImageList() : msizeImage( -1, -1 )
{
	mImageList.m_hImageList = NULL;
}

CDclImageList::CDclImageList( const CDclImageList& Src )
{
	mImageList.m_hImageList = ImageList_Duplicate( Src.mImageList );
	msizeImage = Src.GetSize();
}

CDclImageList::CDclImageList( const CImageList* pSrc )
{
	mImageList.m_hImageList = ImageList_Duplicate( pSrc->m_hImageList );
	int cx = -1;
	int cy = -1;
	ImageList_GetIconSize( mImageList.m_hImageList, &cx, &cy );
	msizeImage.SetSize( cx, cy );
}

CDclImageList::~CDclImageList()
{	
	mImageList.DeleteImageList();
}

CDclImageList& CDclImageList::operator =( const CDclImageList& Src )
{
	msizeImage = Src.msizeImage;
	mImageList.m_hImageList = ImageList_Duplicate( Src.mImageList );
	return *this;
}

bool CDclImageList::SetSize( const CSize& sizeImage )
{
	if( GetCount() > 0 )
		return false;
	msizeImage = sizeImage;
	return true;
}

int CDclImageList::AddPicture( CDC* pDC, LPPICTUREDISP iPic )
{
	if( !pDC )
		return -1;
	CPictureHolder NewPicture;
	NewPicture.SetPictureDispatch( iPic );
	if( !NewPicture.m_pPict )
		return -1;

	long lPicWidth;
	long lPicHeight;
	NewPicture.m_pPict->get_Width( &lPicWidth );
	NewPicture.m_pPict->get_Height( &lPicHeight );
	CSize sizePic( (int)lPicWidth, (int)lPicHeight );
	pDC->HIMETRICtoLP(&sizePic); //convert coordinates from units to logical units

	if( !mImageList.m_hImageList )
	{ // create the image list
		if( msizeImage.cx < 0 )
			msizeImage.cx = sizePic.cx;
		if( msizeImage.cy < 0 )
			msizeImage.cy = sizePic.cy;
		if( !mImageList.Create( msizeImage.cx, msizeImage.cy, ILC_COLOR32 | ILC_MASK, 1, 1 ) )
			return -1;
	}

	int idxPic = -1;
	if( msizeImage.cx == sizePic.cx && msizeImage.cy == sizePic.cy )
	{
		switch( NewPicture.GetType() )
		{
		case PICTYPE_BITMAP:
			{
				HBITMAP hBitmap = NULL;
				NewPicture.m_pPict->get_Handle( (OLE_HANDLE FAR *) &hBitmap );
				idxPic = mImageList.Add( CBitmap::FromHandle(hBitmap), RGB(192, 192, 192) ) ;
			}
			break;
		case PICTYPE_ICON:
			{
				HICON hIcon;
				NewPicture.m_pPict->get_Handle( (OLE_HANDLE FAR *) &hIcon );
				idxPic = mImageList.Add( hIcon ) ;
			}
			break;
		}
	}
	if (idxPic < 0 )
	{
		CDC dcImage;
		dcImage.CreateCompatibleDC( pDC );
		CBitmap bitmapImage;
		bitmapImage.CreateBitmap( msizeImage.cx, msizeImage.cy, 1, 32, NULL );
		CBitmap* pOldBitmap = dcImage.SelectObject( &bitmapImage );
		dcImage.FillSolidRect(0, 0, msizeImage.cx, msizeImage.cy, RGB(192, 192, 192) );
		dcImage.SetBkColor( RGB(192, 192, 192) );
		CRect rcImage( 0, 0, msizeImage.cx, msizeImage.cy );
		NewPicture.Render( &dcImage, &rcImage, &rcImage );
		dcImage.SelectObject( pOldBitmap );
		idxPic = mImageList.Add( &bitmapImage, RGB(192, 192, 192) ) ;
	}
	return idxPic;
}

bool CDclImageList::ReplacePicture( int idxPic, CDC* pDC, LPPICTUREDISP iPic )
{
	if( idxPic < 0 )
		return false;
	if( !pDC )
		return false;
	if( IsNull() )
		return false;
	CPictureHolder NewPicture;
	NewPicture.SetPictureDispatch( iPic );
	if( !NewPicture.m_pPict )
		return false;

	long lPicWidth;
	long lPicHeight;
	NewPicture.m_pPict->get_Width( &lPicWidth );
	NewPicture.m_pPict->get_Height( &lPicHeight );
	CSize sizePic( (int)lPicWidth, (int)lPicHeight );
	pDC->HIMETRICtoLP(&sizePic); //convert coordinates from units to logical units

	int idxReplacedPic = -1;
	if( msizeImage.cx == sizePic.cx && msizeImage.cy == sizePic.cy )
	{
		switch( NewPicture.GetType() )
		{
		case PICTYPE_BITMAP:
			{
				HBITMAP hBitmap = NULL;
				NewPicture.m_pPict->get_Handle( (OLE_HANDLE FAR *) &hBitmap );
				idxReplacedPic = mImageList.Replace( idxPic, CBitmap::FromHandle(hBitmap), NULL ) ;
			}
			break;
		case PICTYPE_ICON:
			{
				HICON hIcon;
				NewPicture.m_pPict->get_Handle( (OLE_HANDLE FAR *) &hIcon );
				idxReplacedPic = mImageList.Replace( idxPic, hIcon ) ;
			}
			break;
		}
	}
	if (idxReplacedPic < 0 )
	{
		CDC dcImage;
		dcImage.CreateCompatibleDC( pDC );
		CBitmap bitmapImage;
		bitmapImage.CreateBitmap( msizeImage.cx, msizeImage.cy, 1, 32, NULL );
		CBitmap* pOldBitmap = dcImage.SelectObject( &bitmapImage );
		dcImage.FillSolidRect(0, 0, msizeImage.cx, msizeImage.cy, RGB(192, 192, 192) );
		dcImage.SetBkColor( RGB(192, 192, 192) );
		CRect rcImage( 0, 0, msizeImage.cx, msizeImage.cy );
		NewPicture.Render( &dcImage, &rcImage, &rcImage );
		dcImage.SelectObject( pOldBitmap );
		idxReplacedPic = mImageList.Replace( idxPic, &bitmapImage, NULL ) ;
	}
	if( idxReplacedPic < 0 )
		return false;
	return true;
}

bool CDclImageList::DeletePicture( int idxPic )
{
	if( idxPic < 0 )
		return false;
	if( IsNull() )
		return false;
	if( !mImageList.Remove( idxPic ) )
		return false;

	return true;
}

void CDclImageList::Serialize(CArchive& ar)
{
	DWORD nThisVersion = GetCurrentSaveVersion();
	if (ar.IsStoring())
	{
		ar << nThisVersion;		

		// if the image list is deleted or null, just flag it as null
		if (IsNull())
			ar << true;
		else
		{
			ar << false;
			SafeImageListWrite( mImageList.m_hImageList, &CArchiveStream( &ar ) );
		}
	}
	else
	{
		ar >> nThisVersion;
		if (nThisVersion < 3)
			AfxThrowArchiveException(CArchiveException::badSchema, ar.m_strFileName );
		if (nThisVersion > GetCurrentSaveVersion())
			AfxThrowArchiveException(CArchiveException::badSchema, ar.m_strFileName );

		bool bNull = true;
		ar >> bNull;
		
		// if the flag indicates that the image list is not null, then read the image list
		if( !bNull )
		{
			mImageList.Read(&ar);
			int cx = -1;
			int cy = -1;
			ImageList_GetIconSize( mImageList.m_hImageList, &cx, &cy );
			msizeImage.SetSize( cx, cy );
		}
		else
			mImageList.DeleteImageList();
	}
}
