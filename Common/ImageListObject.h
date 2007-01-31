#pragma once

enum IOStatus;


class CImageListObject : public CObject
{
public:
	CImageListObject();

// Attributes
public:
	CImageList m_ImageList;
	CSize m_ImageSize;
	BOOL m_Delete;
// Operations
public:
	void Copy(CImageListObject* other);

// Implementation
public:
	virtual ~CImageListObject();

	//File I/O
	virtual void Serialize(CArchive& ar);
  IOStatus WriteToTextFile(FILE* pFile, const CString &fileName) const;
  IOStatus ReadFromTextFile(std::ifstream &sFile, const CString &fileName);
  IOStatus ReadFromTextFile1(std::ifstream &sFile, const CString &fileName);

protected:
	DECLARE_SERIAL(CImageListObject)
};
