#pragma once

enum IOStatus;


class CImageListObject : public CObject
{
	CImageList mImageList;
	bool mbDeleted;
	CSize mszImage;

public:
	CImageListObject();
	CImageListObject( const CImageListObject& Src );
	CImageListObject( const CImageList* pSrc );
	virtual ~CImageListObject();

// Operations
public:
	CImageListObject& operator =( const CImageListObject& Src );
	operator bool() const { return (!mbDeleted && mImageList.m_hImageList); }

// Attributes
public:
	bool IsDeleted() const { return mbDeleted; }
	void SetDeleted( bool bDelete = true ) { mbDeleted = bDelete; }
	const CSize& GetSize() const { return mszImage; }
	const CImageList& GetImageList() const { return mImageList; }
	CImageList& GetImageList() { return mImageList; }

// Implementation

	//File I/O
public:
	virtual void Serialize(CArchive& ar);
  IOStatus ReadFromTextFile(std::ifstream &sFile, const CString &fileName);
  IOStatus ReadFromTextFile1(std::ifstream &sFile, const CString &fileName);
  //IOStatus WriteToTextFile(FILE* pFile, const CString &fileName) const;

protected:
	DECLARE_SERIAL(CImageListObject)
};
