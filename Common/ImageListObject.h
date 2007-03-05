#pragma once

enum IOStatus;


class CImageListObject : public CObject
{
public:
	CImageListObject( const CImageListObject& Src );
	CImageListObject();
	virtual ~CImageListObject();

// Attributes
public:
	CImageList m_ImageList;
	CSize m_ImageSize;
	BOOL m_Delete;
// Operations
public:
	CImageListObject& operator =( const CImageListObject& Src );

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
