#include "StdAfx.h"
#include "ImageListObject.h"
#include "ArchiveEx.h"
#include "Filing.h"
#include "Project.h"


static const int CURRENTVERSION = 1;


IMPLEMENT_SERIAL(CImageListObject, CObject, 1)

CImageListObject::CImageListObject()
{
	m_ImageList.m_hImageList = NULL;
	m_Delete = FALSE;
}

CImageListObject::CImageListObject( const CImageListObject& Src )
: m_Delete( FALSE )
{
	m_ImageList.m_hImageList = ImageList_Duplicate( Src.m_ImageList );
}

CImageListObject::~CImageListObject()
{	
	m_ImageList.DeleteImageList();
}

CImageListObject& CImageListObject::operator =( const CImageListObject& Src )
{
	m_ImageSize = Src.m_ImageSize;
	m_ImageList.m_hImageList = ImageList_Duplicate( Src.m_ImageList );
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
	
	if (ar.IsStoring())
	{
		ar << CURRENTVERSION;		
		ar << m_ImageSize;

		// if the image list is null than save a flag that indicates image list is null
		if (m_ImageList.m_hImageList == NULL)
		{
			ar << TRUE;
		}
		else
		{
			// this indicates the imagelist in not null and we are to write the image list
			ar << FALSE;
			m_ImageList.Write(&ar);
		}
		
	}
	else
	{
		int nThisVersion;
		BOOL bNull;
		ar >> nThisVersion;		
		ar >> m_ImageSize;		
		ar >> bNull;
		
		// set the delete flag as false
		m_Delete = FALSE;

		// if the flag indicates that the image list in not null, then read the image list
		if (bNull == FALSE)
		{
			m_ImageList.Read(&ar);
		}
		else
		{
			m_ImageList.DeleteImageList();
		}
	}

}

IOStatus CImageListObject::ReadFromTextFile1(std::ifstream &sFile, const CString &fileName)
{
  if (!readLong(sFile, m_ImageSize.cx)) return statInvalidFormat;		
  if (!readLong(sFile, m_ImageSize.cy)) return statInvalidFormat;		

  // set the delete flag as false
  m_Delete = FALSE;

  BOOL bNull;
  if (!readBOOL(sFile, bNull)) return statInvalidFormat;
  // if the flag indicates that the image list in not null, then read the image list
  if (bNull == FALSE)
	{
    if (!readImageList(sFile, fileName, m_ImageList)) return statInvalidFormat;
	}
  else
    m_ImageList.DeleteImageList();
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
//  if (m_ImageList.m_hImageList == NULL)
//    writeBOOL(pFile, true);
//  else
//  {
//    // this indicates the imagelist in not null and we are to write the image list
//    writeBOOL(pFile, false);
//    writeImageList(pFile, fileName, m_ImageList);
//  }
//	return statOK;
//}
