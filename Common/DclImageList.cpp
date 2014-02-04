#include "StdAfx.h"
#include "DclImageList.h"
#include "ArchiveEx.h"
#include "Filing.h"
#include "Project.h"
#include "SafeImageListWrite.h"


CDclImageList::CDclImageList()
{
	mImageList.m_hImageList = NULL;
	mszImage.SetSize( -1, -1 );
}

CDclImageList::CDclImageList( const CDclImageList& Src )
{
	mImageList.m_hImageList = ImageList_Duplicate( Src.mImageList );
	mszImage = Src.GetSize();
}

CDclImageList::CDclImageList( const CImageList* pSrc )
{
	mImageList.m_hImageList = ImageList_Duplicate( pSrc->m_hImageList );
	int cx = -1;
	int cy = -1;
	ImageList_GetIconSize( mImageList.m_hImageList, &cx, &cy );
	mszImage.SetSize( cx, cy );
}

CDclImageList::~CDclImageList()
{	
	mImageList.DeleteImageList();
}

CDclImageList& CDclImageList::operator =( const CDclImageList& Src )
{
	mszImage = Src.mszImage;
	mImageList.m_hImageList = ImageList_Duplicate( Src.mImageList );
	return *this;
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
			mszImage.SetSize( cx, cy );
		}
		else
			mImageList.DeleteImageList();
	}
}
