#include "StdAfx.h"
#include "ImageListObject.h"
#include "ArchiveEx.h"
#include "Filing.h"
#include "Project.h"
#include "SafeImageListWrite.h"


IMPLEMENT_SERIAL(CImageListObject, CObject, 1)

CImageListObject::CImageListObject()
: mbDeleted( false )
{
	mImageList.m_hImageList = NULL;
	mszImage.SetSize( -1, -1 );
}

CImageListObject::CImageListObject( const CImageListObject& Src )
: mbDeleted( false )
{
	mImageList.m_hImageList = ImageList_Duplicate( Src.mImageList );
	mszImage = Src.GetSize();
}

CImageListObject::CImageListObject( const CImageList* pSrc )
: mbDeleted( false )
{
	mImageList.m_hImageList = ImageList_Duplicate( pSrc->m_hImageList );
	int cx = -1;
	int cy = -1;
	ImageList_GetIconSize( mImageList.m_hImageList, &cx, &cy );
	mszImage.SetSize( cx, cy );
}

CImageListObject::~CImageListObject()
{	
	mImageList.DeleteImageList();
}

CImageListObject& CImageListObject::operator =( const CImageListObject& Src )
{
	mbDeleted = Src.mbDeleted;
	mszImage = Src.mszImage;
	mImageList.m_hImageList = ImageList_Duplicate( Src.mImageList );
	return *this;
}

IOStatus CImageListObject::ReadFromTextFile(std::ifstream &sFile, const CString &fileName)
{
	CString sObject;
	if (readLine(sFile) != "CImageListObject") return statInvalidFormat;
	int iVersion;
	if (!readInt(sFile, iVersion)) return statInvalidFormat;

	switch (iVersion) {
		case 1 : 
			return ReadFromTextFile1(sFile, fileName);
			break;
	}

	return statInvalidFormat;
}

void CImageListObject::Serialize(CArchive& ar)
{
	CObject::Serialize( ar );
	
	DWORD nThisVersion = GetCurrentSaveVersion();
	if (ar.IsStoring())
	{
		ar << nThisVersion;		

		// if the image list is deleted or null, just flag it as null
		if (mbDeleted || mImageList.m_hImageList == NULL)
			ar << true;
		else
		{
			ar << false;
			CArchiveStream is( &ar );
			SafeImageListWrite( mImageList.m_hImageList, &is );
		}
	}
	else
	{
		ar >> nThisVersion;
		if (nThisVersion > GetCurrentSaveVersion())
			AfxThrowArchiveException(CArchiveException::badSchema, ar.m_strFileName );

		mbDeleted = false;

		bool bNull = true;
		if( nThisVersion < 2 )
		{
			CSize szDiscard;
			ar >> szDiscard;
			BOOL bNullTemp;
			ar >> bNullTemp;
			bNull = (bNullTemp != FALSE);
		}
		else
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

IOStatus CImageListObject::ReadFromTextFile1(std::ifstream &sFile, const CString &fileName)
{
	if (!readLong(sFile, mszImage.cx)) return statInvalidFormat;		
	if (!readLong(sFile, mszImage.cy)) return statInvalidFormat;		

	// set the delete flag as false
	mbDeleted = false;

	BOOL bNull;
	if (!readBOOL(sFile, bNull)) return statInvalidFormat;
	// if the flag indicates that the image list in not null, then read the image list
	if (bNull == FALSE)
	{
		if (!readImageList(sFile, fileName, mImageList)) return statInvalidFormat;
	}
	else
		mImageList.DeleteImageList();
	return statOK;
}

//IOStatus CImageListObject::WriteToTextFile(FILE* pFile, const CString &fileName) const
//{
//  fprintf(pFile, "\nCImageListObject");
//  writeInt(pFile, CURRENTVERSION);
//  writeLong(pFile, m_ImageSize.cx);
//  writeLong(pFile, m_ImageSize.cy);
//
//  // if the image list is null then save a flag that indicates image list is null
//  if (mImageList.m_hImageList == NULL)
//    writeBOOL(pFile, true);
//  else
//  {
//    // this indicates the imagelist in not null and we are to write the image list
//    writeBOOL(pFile, false);
//    writeImageList(pFile, fileName, mImageList);
//  }
//	return statOK;
//}
