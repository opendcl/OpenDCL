#pragma once

#include "RefCountedPtr.h"


class CDclImageList
{
	CImageList mImageList;
	CSize msizeImage;

public:
	CDclImageList();
	CDclImageList( const CDclImageList& Src );
	CDclImageList( const CImageList* pSrc );
	virtual ~CDclImageList();

protected:
	//2014-02-03 [ORW]: save version set to 3 (Split old CObject-derived class into separate file)
	ULONG GetCurrentSaveVersion() const { return 3; }

// Operations
public:
	CDclImageList& operator =( const CDclImageList& Src );
	bool SetSize( const CSize& sizeImage );
	int AddPicture( CDC* pDC, LPPICTUREDISP iPic );
	bool ReplacePicture( int idxPic, CDC* pDC, LPPICTUREDISP iPic );
	bool DeletePicture( int idxPic );

// Attributes
public:
	bool IsNull() const { return (!mImageList.m_hImageList); }
	int GetCount() const { return (mImageList.m_hImageList? mImageList.GetImageCount() : 0); }
	const CSize& GetSize() const { return msizeImage; }
	const CImageList& GetImageList() const { return mImageList; }
	CImageList& GetImageList() { return mImageList; }

// Implementation

	//File I/O
public:
	virtual void Serialize(CArchive& ar);
};

typedef RefCountedPtr< CDclImageList > TImageListPtr;
typedef RefCountedPtrAsIUnknown< TImageListPtr > TImageListPtrIUnknown;