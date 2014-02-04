#pragma once

#include "RefCountedPtr.h"


class CDclImageList
{
	CImageList mImageList;
	CSize mszImage;

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

// Attributes
public:
	bool IsNull() const { return (!mImageList.m_hImageList); }
	const CSize& GetSize() const { return mszImage; }
	const CImageList& GetImageList() const { return mImageList; }
	CImageList& GetImageList() { return mImageList; }

// Implementation

	//File I/O
public:
	virtual void Serialize(CArchive& ar);
};

typedef RefCountedPtr< CDclImageList > TImageListPtr;
typedef RefCountedPtrAsIUnknown< TImageListPtr > TImageListPtrIUnknown;