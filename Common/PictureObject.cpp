#include "StdAfx.h"
#include "PictureObject.h"
#include "StgFile.h"
#include "ArchiveEx.h"
#include "Filing.h"
#include "Project.h"


static const CLSID _afx_CLSID_StdPicture2_V1 =
	{ 0xfb8f0824,0x0164,0x101b, { 0x84,0xed,0x08,0x00,0x2b,0x2e,0xc7,0x13 } };


static LPCTSTR gszPicture = _T(".pic");
static LPCTSTR gszPassword = _T("d32afd3aw3aq3fdaw3");

const int nPictureObjectVersion1 = 1;
const int nPictureObjectVersion2 = 2;
const int nPictureObjectVersion3 = 3;
const int nPictureObjectVersion4 = 4;
const int nPictureObjectVersion5 = 5;
const COLORREF rgbLightGrey = RGB(192, 192, 192);

#define HIMETRIC_INCH	2540

#define RELEASE(lpUnk) do \
	{ if ((lpUnk) != NULL) { (lpUnk)->Release(); (lpUnk) = NULL; } } while (0)

#if defined(_AFXDLL)
static LPSTREAM AFXAPI _AfxGetArchiveStream(CArchive& ar, CArchiveStream& stm)
{
	// Obtain direct access to the archive's LPSTREAM.
	ar.Flush();
	CFile* pFile = ar.GetFile();
	ASSERT(pFile != NULL);
	LPSTREAM pstm;
	if (pFile->IsKindOf(RUNTIME_CLASS(COleStreamFile)))
	{
		pstm = ((COleStreamFile*)pFile)->m_lpStream;
		ASSERT(pstm != NULL);
	}
	else
	{
		ASSERT(stm.m_pArchive == NULL || stm.m_pArchive == &ar);
		stm.m_pArchive = &ar;
		pstm = &stm;
	}
	return pstm;
}

static BOOL AFXAPI _AfxIsSameUnknownObject(REFIID iid, LPUNKNOWN pUnk1, LPUNKNOWN pUnk2)
{
	if (pUnk1 == pUnk2)
		return TRUE;

	if (pUnk1 == NULL || pUnk2 == NULL)
		return FALSE;

	LPUNKNOWN pI1 = NULL;
	LPUNKNOWN pI2 = NULL;
	BOOL bResult = FALSE;
	if (SUCCEEDED(pUnk1->QueryInterface(iid, (void**)&pI1)))
	{
		ASSERT_POINTER(pI1, IUnknown);
		if (SUCCEEDED(pUnk2->QueryInterface(iid, (void**)&pI2)))
		{
			ASSERT_POINTER(pI2, IUnknown);
			bResult = (pI1 == pI2);
			pI2->Release();
		}
		pI1->Release();
	}
	return bResult;
}
#else
LPSTREAM AFXAPI _AfxGetArchiveStream(CArchive& ar, CArchiveStream& stm);
BOOL AFXAPI _AfxIsSameUnknownObject(REFIID iid, LPUNKNOWN pUnk1, LPUNKNOWN pUnk2);
#endif //_AFXDLL


//Utility functions

static void LoadPicture(CArchive& ar, COleVariant& varSrc) {
  LPVARIANT pSrc = &varSrc;
  ar >> pSrc->vt;

  switch (pSrc->vt) {
  case VT_DISPATCH:
  case VT_UNKNOWN:
    {
      LPPERSISTSTREAM pPersistStream = NULL;
      CArchiveStream stm(&ar);

      // Retrieve the CLSID (GUID) and create an instance
      CLSID clsid;
      ar >> clsid.Data1;
      ar >> clsid.Data2;
      ar >> clsid.Data3;
      ar.Read(&clsid.Data4[0], sizeof clsid.Data4);

      CPictureHolder hPicture;
      hPicture.CreateEmpty();
      pSrc->punkVal = hPicture.GetPictureDispatch();
      TRY
      {
        // QI for IPersistStream or IPeristStreamInit
        SCODE sc = pSrc->punkVal->QueryInterface(
          IID_IPersistStream, (void**)&pPersistStream);
#ifndef _AFX_NO_OCC_SUPPORT
        if (FAILED(sc))
          sc = pSrc->punkVal->QueryInterface(
          IID_IPersistStreamInit, (void**)&pPersistStream);
#endif

        AfxCheckError(sc);

        // Always assumes object is dirty
        AfxCheckError(pPersistStream->Load(&stm));
      }
      CATCH_ALL(e)
      {
        // Clean up
        if (pPersistStream != NULL)
          pPersistStream->Release();

        pSrc->punkVal->Release();
        THROW_LAST();
      }
      END_CATCH_ALL;

			pPersistStream->Release();
    }
  }
}

static CRect CalcFitRect(int nPicWidth, int nPicHeight, int nCtrlWidth, int nCtrlHeight)
{
	CRect rcCell(0, 0, nCtrlWidth, nCtrlHeight);
	double dFactor;
	double dH;
	double dW;

	dFactor = (double)nPicHeight / (double)nPicWidth;
	dH = dFactor;
	dW = 1.0;

	int nDrawWidth = int(dW * nCtrlWidth);
	int nDrawHeight = int(dH * nDrawWidth);
	
	// if the calc height is too large
	if (nDrawHeight > nCtrlHeight)
	{
		dFactor = (double)nPicWidth / (double)nPicHeight;
		dH = 1.0;
		dW = dFactor;

		nDrawHeight = int(dH * nCtrlHeight);
		nDrawWidth = int(dW * nDrawHeight);

		rcCell.left = (nCtrlWidth - nDrawWidth) / 2;
		rcCell.right = nCtrlWidth - rcCell.left;
	}
	else if (nDrawHeight < nCtrlHeight)
	{
		rcCell.top = (nCtrlHeight - nDrawHeight) / 2;
		rcCell.bottom = nCtrlHeight - rcCell.top;
	}
	
	// if the calc width is too large
	if (nDrawWidth > nCtrlWidth)
	{
		dFactor = (double)nPicHeight / (double)nPicWidth;
		dH = dFactor;
		dW = 1.0;

		nDrawWidth = int(dW * nCtrlWidth);
		nDrawHeight = int((double)nDrawWidth * dH);
		
		rcCell.left = 1;
		rcCell.right = nCtrlWidth;
		rcCell.top = (nCtrlHeight - nDrawHeight) / 2;
		rcCell.bottom = nCtrlHeight - rcCell.top;
	}

	return rcCell;
}


/////////////////////////////////////////////////////////////////////////////
// CArchivePropExchange - for persistence in an archive.

class CArchivePropExchange : public CPropExchange
{
// Constructors
public:
	CArchivePropExchange(CArchive& ar);

// Operations
	virtual BOOL ExchangeProp(LPCTSTR pszPropName, VARTYPE vtProp,
				void* pvProp, const void* pvDefault = NULL);
	virtual BOOL ExchangeBlobProp(LPCTSTR pszPropName, HGLOBAL* phBlob,
				HGLOBAL hBlobDefault = NULL);
	virtual BOOL ExchangeFontProp(LPCTSTR pszPropName, CFontHolder& font,
				const FONTDESC* pFontDesc, LPFONTDISP pFontDispAmbient);
	virtual BOOL ExchangePersistentProp(LPCTSTR pszPropName,
				LPUNKNOWN* ppUnk, REFIID iid, LPUNKNOWN pUnkDefault);

// Implementation
protected:
	CArchive& m_ar;
};



IMPLEMENT_SERIAL(CPictureObject, CObject, 1)

CPictureObject::CPictureObject()
{
	m_hPicture.CreateEmpty();
	m_hIcon = NULL;
	m_ToBeAdded = false;
	m_ToBeDeleted = false;

	m_hBitmap = NULL;
	m_hLoadedIcon = NULL;
	m_bLoaded = false;
}

CPictureObject::~CPictureObject()
{
	if (m_hBitmap)
		DeleteObject(m_hBitmap);
	if (m_hLoadedIcon)
		DestroyIcon(m_hLoadedIcon);
	if (m_hIcon)
		DestroyIcon(m_hIcon);
}

void CPictureObject::Clear()
{
	try
	{
		m_hPicture.m_pPict->Release();
	}
	catch(...)
	{
	}
	if (m_hIcon != NULL)
	{
		DestroyIcon(m_hIcon);
		m_hIcon = NULL;
	}

}

HICON CPictureObject::GetIcon() const
{
	switch (GetPicType() )
	{
	case PICTYPE_ICON:
		{
			HICON hIcon;
			m_hPicture.m_pPict->get_Handle((OLE_HANDLE*)&hIcon);
			return hIcon;			
		}
	case PICTYPE_BITMAP:
		{
			HBITMAP hBmpPic = NULL;
			m_hPicture.m_pPict->get_Handle((OLE_HANDLE*)&hBmpPic);
			if (hBmpPic != NULL)
			{
				// add the bitmap to a image list for extraction
				CImageList ImageList;
				ImageList.Create(m_Width, m_Height, ILC_COLOR4 | ILC_MASK, 1, 1);
				ImageList.Add(CBitmap::FromHandle(hBmpPic), rgbLightGrey);
				HICON hIcon = ImageList.ExtractIcon(0);
				ImageList.DeleteImageList();
				return hIcon;
			}
		}
	}
	return NULL;
}

HBITMAP CPictureObject::GetBitmap() const
{
	switch (GetPicType() )
	{
	case PICTYPE_BITMAP:
		{
			HBITMAP m_hBitmap = NULL;
			m_hPicture.m_pPict->get_Handle((OLE_HANDLE*)&m_hBitmap);
			return m_hBitmap;
		}
	}
	return NULL;
}

short CPictureObject::GetPicType() const
{
	if (m_hIcon != NULL)
		return PICTYPE_ICON;
	return const_cast<CPictureObject*>(this)->m_hPicture.GetType();
}

void CPictureObject::Update(short nID, LPPICTUREDISP NewPicture) 
{
	m_nID = nID;
	m_hPicture.SetPictureDispatch(NewPicture);
	
	HDC hdc = ::GetDC(GetDesktopWindow());
	CDC * cdc = CDC::FromHandle(hdc);

	// assign picture object values	
	CSize sizePic;
	long lPicWidth;
	long lPicHeight;

	// get dimensions of bitmap
	m_hPicture.m_pPict->get_Width(&lPicWidth);
	m_hPicture.m_pPict->get_Height(&lPicHeight);

	sizePic.cx = (int)lPicWidth;
	sizePic.cy = (int)lPicHeight;

	// convert coordinates from units to logical units
	cdc->HIMETRICtoLP(&sizePic);
	cdc->Detach();
	m_Width = sizePic.cx;
	m_Height = sizePic.cy;
}

void CPictureObject::LoadFile(LPCTSTR szFile, int nID)
{
	LPPICTURE		lpPicture;
	lpPicture		= NULL;
	CPictureHolder	phPicture;
	
	// open file
	HANDLE hFile = CreateFile(szFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	_ASSERTE(INVALID_HANDLE_VALUE != hFile);

	// get file size
	DWORD dwFileSize = GetFileSize(hFile, NULL);
	_ASSERTE(-1 != dwFileSize);

	LPVOID pvData = NULL;
	// alloc memory based on file size
	HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, dwFileSize);
	_ASSERTE(NULL != hGlobal);

	pvData = GlobalLock(hGlobal);
	_ASSERTE(NULL != pvData);

	DWORD dwBytesRead = 0;
	// read file and store in global memory
	BOOL bRead = ReadFile(hFile, pvData, dwFileSize, &dwBytesRead, NULL);
	_ASSERTE(FALSE != bRead);
	GlobalUnlock(hGlobal);
	CloseHandle(hFile);

	LPSTREAM pstm = NULL;
	// create IStream* from global memory
	HRESULT hr = CreateStreamOnHGlobal(hGlobal, TRUE, &pstm);
	_ASSERTE(SUCCEEDED(hr) && pstm);
	
	// Create IPicture from image file
	if (lpPicture)
		lpPicture->Release();
	hr = ::OleLoadPicture(pstm, dwFileSize, FALSE, IID_IPicture, (LPVOID *)&lpPicture);
	_ASSERTE(SUCCEEDED(hr) && lpPicture);	

	IPicture *ipOld = phPicture.m_pPict;
	phPicture.m_pPict = lpPicture;

	Update(nID, phPicture.GetPictureDispatch());
	phPicture.m_pPict = ipOld;
	pstm->Release();
}

//static
CPictureObject* CPictureObject::CreatePictureObject(short nID, LPPICTUREDISP NewPicture)
{
	//create new picture object
	CPictureObject *pPicture = new CPictureObject;
	
	pPicture->m_nID = nID;
	pPicture->m_hPicture.SetPictureDispatch(NewPicture);
	
	HDC hdc = ::GetDC(GetDesktopWindow());
	CDC * cdc = CDC::FromHandle(hdc);

	// assign picture object values	
	CSize sizePic;
	long lPicWidth;
	long lPicHeight;

	// get dimensions of bitmap
	pPicture->m_hPicture.m_pPict->get_Width(&lPicWidth);
	pPicture->m_hPicture.m_pPict->get_Height(&lPicHeight);

	sizePic.cx = (int)lPicWidth;
	sizePic.cy = (int)lPicHeight;

	// convert coordinates from units to logical units
	cdc->HIMETRICtoLP(&sizePic);
	cdc->Detach();
	pPicture->m_Width = sizePic.cx;
	pPicture->m_Height = sizePic.cy;
	return pPicture;
}

/*
void CPictureObject::SaveSS(CStgFile &FileStg, CDocument *pDoc) const
{
	CString sValue;
	sValue.Format(_T("%d"), m_nID);
	FileStg.Open(sValue + gszPicture, CFile::modeCreate | CFile::modeWrite); 

	CArchiveEx ar(&FileStg, CArchive::store | CArchive::bNoFlushOnDelete, NULL, gszPassword, TRUE);
	ar.m_pDocument = pDoc;
	ar.m_bForceFlat = FALSE;

	// put dcl form into archive
	const_cast<CPictureObject*>(this)->Serialize(ar);

	ar.Close();			

	FileStg.Close();	// close the stream
}

CPictureObject* CPictureObject::ReadSS(int nID, CStgFile &FileStg, CDocument *pDoc)
{
	CString sValue;
	sValue.Format(_T("%d"), nID);
	FileStg.Open(sValue + gszPicture, CFile::modeRead | CFile::shareDenyWrite); 

	CArchiveEx ar(&FileStg, CArchive::load | CArchive::bNoFlushOnDelete, NULL, gszPassword, TRUE);
	
	// get current Dcl form
	CPictureObject* pPicture = new CPictureObject;
		
	// get dcl form into archive
	pPicture->Serialize(ar);
		
	ar.Close();			

	FileStg.Close();	// close the stream

	return pPicture;
}
*/

IOStatus CPictureObject::WriteToTextFile(FILE* pFile, const CString &fileName) const
{
  //savebug
  fprintf(pFile, "\nCPictureObject");
	writeInt(pFile, nPictureObjectVersion3);
	
	writeInt(pFile, m_nID);
	writeInt(pFile, m_Height);
	writeInt(pFile, m_Width);

	int nPictureType = const_cast<CPictureObject*>(this)->m_hPicture.GetType();
	writeInt(pFile, nPictureType);

	if (nPictureType == PICTYPE_BITMAP) {
    HBITMAP hBitmap = NULL;
    m_hPicture.m_pPict->get_Handle((OLE_HANDLE*)&hBitmap);

    CImage img;
    img.Attach(hBitmap);
    writeImage(pFile, fileName, img);

    hBitmap = img.Detach();
    const_cast<CPictureObject*>(this)->m_hPicture.CreateFromBitmap(hBitmap, NULL, TRUE);
	} else if (nPictureType == PICTYPE_METAFILE ||
             nPictureType == PICTYPE_ENHMETAFILE) {
    //Currently not quite sure how to handle these
  } else if (PICTYPE_ICON == nPictureType) {
    // else if picture is an icon
		HICON hIconPic = NULL;
		// get handle of the icon
		m_hPicture.m_pPict->get_Handle((OLE_HANDLE FAR *) &hIconPic);
		const_cast<CPictureObject*>(this)->m_hIcon = hIconPic;
		//m_bImageCreated = true;

		CImageList il;
		il.Create(m_Width, m_Height, ILC_COLOR8 | ILC_MASK, 1, 1);
		il.Add(hIconPic);
    writeImageList(pFile, fileName, il);
		il.DeleteImageList();
	}
	return statOK;
}

void CPictureObject::Serialize(CArchive& ar)
{
  CObject::Serialize( ar );

  if (ar.IsStoring())
  {
    ar << nPictureObjectVersion3;

    ar << m_nID;
    ar << m_Height;
    ar << m_Width;

    int nPictureType = m_hPicture.GetType();
    ar << nPictureType;

    if (m_hPicture.GetType() == PICTYPE_BITMAP || 
      m_hPicture.GetType() == PICTYPE_METAFILE ||
      m_hPicture.GetType() == PICTYPE_ENHMETAFILE)
    {
      short nPicType = m_hPicture.GetType();
      ar << nPicType;

      //PX_Picture(ar, m_hPicture);

      VARIANT var;
      var.vt = VT_DISPATCH;
      IPictureDisp *pPicDisp = m_hPicture.GetPictureDispatch();
      var.pdispVal = pPicDisp;
      COleVariant OleVar;
      OleVar.Attach(var);
      ar << OleVar;
      var.pdispVal = NULL;
      OleVar.Clear();
      HRESULT hr = VariantClear(&var);			

    }
    // else if picture is an icon
    else if (PICTYPE_ICON == m_hPicture.GetType())
    {
      short nPicType = PICTYPE_ICON;
      ar << nPicType;

      HICON hIconPic = NULL;
      // get handle of the icon
      m_hPicture.m_pPict->get_Handle((OLE_HANDLE FAR *) &hIconPic);
      m_hIcon = hIconPic;
      //m_bImageCreated = true;

      CImageList ImageList;
      ImageList.Create(
        m_Width,
        m_Height,
        ILC_COLOR8 | ILC_MASK, 1, 1);
      ImageList.Add(hIconPic);
      ImageList.Write(&ar);
      ImageList.DeleteImageList();
    }
  }
  else	
  {
    try
    {
      int nThisVersion;
      ar >> nThisVersion;

      ar >> m_nID;
      if (nThisVersion == nPictureObjectVersion1)
      {
        CImageList tempImage;
        tempImage.Read(&ar);
        if (tempImage.m_hImageList != NULL)
        {				
          m_hIcon = tempImage.ExtractIcon(0);
          m_hPicture.CreateFromIcon(m_hIcon, TRUE);			
          tempImage.DeleteImageList();
        }
      }
      ar >> m_Height;
      ar >> m_Width;
      int nPictureType = 0;
      if (nThisVersion == nPictureObjectVersion3 || nThisVersion == nPictureObjectVersion4)
        ar >> nPictureType;

      if ((nPictureType != -1 && nThisVersion == nPictureObjectVersion2) ||
        (nPictureType >= 0 && nThisVersion == nPictureObjectVersion3))
      {
        short nPicType;
        ar >> nPicType;
        if (nPicType != nPictureType)
        {
          nPicType = nPictureType;				
        }
        if (nPicType <= 0)
        {
          m_nID = -1;
        }

        if (nPicType == PICTYPE_BITMAP || 
          nPicType == PICTYPE_METAFILE ||
          nPicType == PICTYPE_ENHMETAFILE)
        {	

          VARIANT var;
          COleVariant OleVar;
          OleVar.Attach(var);

          try
          {		
            LoadPicture(ar, OleVar);
            var = OleVar.Detach();	
            m_hPicture.SetPictureDispatch((IPictureDisp*)var.pdispVal);
            OleVar.Clear();
            HRESULT hr = VariantClear(&var);
          }
          catch(...)
          {
            var = OleVar.Detach();	
            OleVar.Clear();
            HRESULT hr = VariantClear(&var);
          }

        }
        else if (PICTYPE_ICON == nPicType)
        {
          CImageList tempImage;
          tempImage.Read(&ar);
          if (tempImage.m_hImageList != NULL)
          {
            HICON hIcon = tempImage.ExtractIcon(0);
            m_hPicture.CreateFromIcon(hIcon, TRUE);			
            tempImage.DeleteImageList();
          }
        }
      }
      if (nPictureType > 0 && nThisVersion == nPictureObjectVersion4)
      {
        short nPicType;
        ar >> nPicType;
        if (nPicType == PICTYPE_BITMAP || 
          nPicType == PICTYPE_METAFILE ||
          nPicType == PICTYPE_ENHMETAFILE)
        {		
          CArchivePropExchange px(ar);
					CString sPropName;
					sPropName.Format(_T("%d"), m_nID);
          PX_Picture(&px, sPropName, m_hPicture);
        }
        else if (PICTYPE_ICON == nPicType)
        {
          CImageList tempImage;
          tempImage.Read(&ar);
          if (tempImage.m_hImageList != NULL)
          {
            HICON hIcon = tempImage.ExtractIcon(0);
            m_hPicture.CreateFromIcon(hIcon, TRUE);			
            tempImage.DeleteImageList();
          }
        }
      }
      if (nPictureType > 0 && nThisVersion == nPictureObjectVersion5)
      {
        short nPicType;
        ar >> nPicType;
        if (nPicType == PICTYPE_BITMAP || 
          nPicType == PICTYPE_METAFILE ||
          nPicType == PICTYPE_ENHMETAFILE)
        {		
          CArchivePropExchange px(ar);
					CString sPropName;
					sPropName.Format(_T("%d"), m_nID);
          PX_Picture(&px, sPropName, m_hPicture);
        }
        else if (PICTYPE_ICON == nPicType)
        {
          CImageList tempImage;
          tempImage.Read(&ar);
          if (tempImage.m_hImageList != NULL)
          {
            HICON hIcon = tempImage.ExtractIcon(0);
            m_hPicture.CreateFromIcon(hIcon, TRUE);			
            tempImage.DeleteImageList();
          }
        }
      }
    }
    catch(...)
    {
      // do nothing
    }
  }

}

IOStatus CPictureObject::ReadFromTextFile(std::ifstream &sFile, const CString &fileName)
{
  CString sObject;
  if (readLine(sFile) != "CPictureObject") return statInvalidFormat;
  int iVersion;
  if (!readInt(sFile, iVersion)) return statInvalidFormat;

  switch (iVersion) {
    case 3 : 
      return ReadFromTextFile3(sFile, fileName);
      break;
  }
  return statInvalidFormat;
}

IOStatus CPictureObject::ReadFromTextFile3(std::ifstream &sFile, const CString &fileName)
{
  try
  {
    if (!readInt(sFile, m_nID)) return statInvalidFormat;
    if (!readInt(sFile, m_Height)) return statInvalidFormat;
    if (!readInt(sFile, m_Width)) return statInvalidFormat;

    int iPicType;
    if (!readInt(sFile, iPicType)) return statInvalidFormat;
#ifndef EDITOR
    m_PicType = iPicType;
#endif

    if (iPicType == PICTYPE_BITMAP) {
      CImage img;
      if (!readImage(sFile, fileName, img)) return statInvalidFormat;
      HBITMAP hBitmap = img.Detach();
      m_hPicture.CreateFromBitmap(hBitmap);
    } else if (iPicType == PICTYPE_METAFILE 
               || iPicType == PICTYPE_ENHMETAFILE) {
      //Currently not handled
    } else if (iPicType == PICTYPE_ICON) {
      CImageList il;
      if (!readImageList(sFile, fileName, il)) return statInvalidFormat;
      if (il.m_hImageList != NULL)
      {
        HICON hIcon = il.ExtractIcon(0);
        m_hPicture.CreateFromIcon(hIcon, TRUE);			
        il.DeleteImageList();
      }
    }
  }
  catch(...)
  {
    // do nothing
  }

  return statOK;
}

BOOL AFX_CDECL CPictureObject::PX_IUnknown2(CPropExchange* pPX, LPCTSTR pszPropName, LPUNKNOWN& pUnk,
	REFIID iid, LPUNKNOWN pUnkDefault)
{
	ASSERT_POINTER(pPX, CPropExchange);
	ASSERT(AfxIsValidString(pszPropName));
	ASSERT_POINTER(&pUnk, LPUNKNOWN);
	ASSERT_NULL_OR_POINTER(pUnk, IUnknown);
	ASSERT_NULL_OR_POINTER(pUnkDefault, IUnknown);

	if (pPX->IsAsynchronous())
		return TRUE;
	return pPX->ExchangePersistentProp(pszPropName, &pUnk, iid, pUnkDefault);
}

BOOL AFX_CDECL CPictureObject::PX_Picture(CPropExchange* pPX, LPCTSTR pszPropName, CPictureHolder& pict)
{
	ASSERT_POINTER(pPX, CPropExchange);
	ASSERT(AfxIsValidString(pszPropName));
	ASSERT_POINTER(&pict, CPictureHolder);

	if (pPX->IsAsynchronous())
		return TRUE;
	LPUNKNOWN& pUnk = (LPUNKNOWN&)pict.m_pPict;
	return PX_IUnknown2(pPX, pszPropName, pUnk, IID_IPicture);
}

BOOL CPictureObject::PX_IUnknown(CArchive& ar, LPUNKNOWN& pUnk,
	REFIID iid, LPUNKNOWN pUnkDefault)
{
	ASSERT_POINTER(&pUnk, LPUNKNOWN);
	ASSERT_NULL_OR_POINTER(pUnk, IUnknown);
	ASSERT_NULL_OR_POINTER(pUnkDefault, IUnknown);

	return ExchangePersistentProp(ar, &pUnk, iid, pUnkDefault);
}

BOOL CPictureObject::PX_Picture(CArchive& ar, CPictureHolder& pict)
{
	ASSERT_POINTER(&pict, CPictureHolder);

	LPUNKNOWN& pUnk = (LPUNKNOWN&)pict.m_pPict;
	return PX_IUnknown(ar, pUnk, IID_IPicture);
}

BOOL CPictureObject::ExchangePersistentProp(CArchive& ar, 
		LPUNKNOWN* ppUnk, REFIID iid, LPUNKNOWN pUnkDefault)
{
	ASSERT_POINTER(ppUnk, LPUNKNOWN);
	ASSERT_NULL_OR_POINTER(pUnkDefault, IUnknown);

	BOOL bResult = FALSE;
	CArchiveStream stm(&ar);

	if (!ar.IsStoring())
	{
		RELEASE(*ppUnk);
		*ppUnk = NULL;

		BYTE bFlag;
		ar >> bFlag;
		if (bFlag != 0xFF)
		{
			// read the CLSID
			CLSID clsid;
			ar >> clsid.Data1;
			ar >> clsid.Data2;
			ar >> clsid.Data3;
			ar.Read(&clsid.Data4[0], sizeof clsid.Data4);

			// check for GUID_NULL first and skip if found
			if (IsEqualCLSID(clsid, GUID_NULL))
				bResult = TRUE;
			else
			{
				// otherwise will need a stream
				LPSTREAM pstm = _AfxGetArchiveStream(ar, stm);
				if (IsEqualCLSID(clsid, CLSID_StdPicture) ||
					IsEqualCLSID(clsid, _afx_CLSID_StdPicture2_V1))
				{
					// special case for pictures
					bResult = SUCCEEDED(::OleLoadPicture(pstm, 0, FALSE, iid,
						(void**)ppUnk));
				}
				else
				{
					// otherwise, seek back to the CLSID
					LARGE_INTEGER li;
					li.LowPart = (DWORD)(-(long)sizeof(CLSID));
					li.HighPart = -1;
					VERIFY(SUCCEEDED(pstm->Seek(li, STREAM_SEEK_CUR, NULL)));

					// and load the object normally

					CLSID clsid;
					if (SUCCEEDED(::ReadClassStm(pstm, &clsid)) &&
							(SUCCEEDED(::CoCreateInstance(clsid, NULL,
								CLSCTX_SERVER | CLSCTX_REMOTE_SERVER,
								iid, (void**)ppUnk)) ||
							SUCCEEDED(::CoCreateInstance(clsid, NULL,
								CLSCTX_SERVER & ~CLSCTX_REMOTE_SERVER,
								iid, (void**)ppUnk))))
					{
						LPPERSISTSTREAM pps = NULL;
						if (SUCCEEDED((*ppUnk)->QueryInterface(
								IID_IPersistStream, (void**)&pps)) ||
							SUCCEEDED((*ppUnk)->QueryInterface(
								IID_IPersistStreamInit, (void**)&pps)))
						{
							ASSERT_POINTER(pps, IPersistStream);
							bResult = SUCCEEDED(pps->Load(pstm));
							pps->Release();
						}

						if (!bResult)
						{
							(*ppUnk)->Release();
							*ppUnk = NULL;
						}
					}
				}
			}
		}
		else
		{
			// Use default value.
			bResult = pUnkDefault == NULL ||
				SUCCEEDED(pUnkDefault->QueryInterface(iid, (LPVOID*)ppUnk));
		}
	}
	else
	{
		ASSERT_NULL_OR_POINTER(*ppUnk, IUnknown);

		// Check if *ppUnk and pUnkDefault are the same thing.  If so, don't
		// bother saving the object; just write a special flag instead.

		if (_AfxIsSameUnknownObject(iid, *ppUnk, pUnkDefault))
		{
			ar << (BYTE)0xFF;
			bResult = TRUE;
		}
		else
		{
			ar << (BYTE)0x00;
			if (*ppUnk != NULL)
			{
				LPPERSISTSTREAM pps = NULL;
				if (SUCCEEDED((*ppUnk)->QueryInterface(
						IID_IPersistStream, (void**)&pps)) ||
					SUCCEEDED((*ppUnk)->QueryInterface(
						IID_IPersistStreamInit, (void**)&pps)))
				{
					ASSERT_POINTER(pps, IPersistStream);
					LPSTREAM pstm = _AfxGetArchiveStream(ar, stm);
					bResult = SUCCEEDED(::OleSaveToStream(pps, pstm));
					pps->Release();
				}
			}
			else
			{
				// If no object, write null class ID.
				ar.Write(&GUID_NULL, sizeof(GUID));
			}
		}
	}

	// throw exception in case of unthrown errors
	if (!bResult)
    AfxThrowArchiveException(CArchiveException::badClass);

	return TRUE;
}

void CPictureObject::Render(CDC *pdc, int nPicLeft, int nPicTop, CRect &rcThis, bool bAutoSize)
{
	// get width and height of picture
	long hmWidth;
	long hmHeight;
	m_hPicture.m_pPict->get_Width(&hmWidth);
	m_hPicture.m_pPict->get_Height(&hmHeight);
	
	// convert himetric to pixels
	int nPicWidth	= MulDiv(hmWidth, pdc->GetDeviceCaps(LOGPIXELSX), HIMETRIC_INCH);
	int nPicHeight	= MulDiv(hmHeight, pdc->GetDeviceCaps(LOGPIXELSY), HIMETRIC_INCH);		
	int cxIcon = nPicWidth;
	int cyIcon = nPicHeight;
	if (bAutoSize == TRUE)
	{
		// Center the icon horizontally
		nPicLeft = ((rcThis.Width() - nPicWidth)/2);
	
		// Center the icon vertically
		nPicTop = ((rcThis.Height() - nPicHeight)/2);           
	
		nPicTop = 0;
		nPicLeft = 0;
	}

	if (m_hPicture.GetType() == PICTYPE_METAFILE ||
		m_hPicture.GetType() == PICTYPE_ENHMETAFILE)
	{
		CRect rcCell = CalcFitRect(nPicWidth, nPicHeight,rcThis.Width(), rcThis.Height());

		// display picture using IPicture::Render
		m_hPicture.m_pPict->Render(pdc->m_hDC, rcCell.left, rcCell.top, rcCell.Width(), rcCell.Height(), 0, hmHeight, hmWidth, -hmHeight, &rcThis);
		
	}
	else
	{
		// display picture using IPicture::Render
		m_hPicture.m_pPict->Render(pdc->m_hDC, nPicLeft, nPicTop, nPicWidth, nPicHeight, 0, hmHeight, hmWidth, -hmHeight, &rcThis);
	}
}

void CPictureObject::EnsurePictureIsLoaded()
{
	if (m_bLoaded)
		return;

	CStgFile FileStg;

	FileStg.OpenStg(m_sFileName);

	CString sID;
	sID.Format(_T("%d"), m_nID);
	FileStg.Open(sID + gszPicture, CFile::modeRead | CFile::shareDenyWrite); 

	CArchiveEx ar(&FileStg, CArchive::load | CArchive::bNoFlushOnDelete, NULL, gszPassword, TRUE);
	
	// get dcl form into archive
	Serialize(ar);
		
	ar.Close();			

	FileStg.Close();	// close the stream

	FileStg.CloseStg(); // close the storage file

	m_bLoaded = true;
}

/*
CPictureObject* CPictureObject::ReadSS(int nID, CStgFile &FileStg)
{
	CString sID;
	sID.Format(_T("%d"), nID);
	FileStg.Open(sID + gszPicture, CFile::modeRead | CFile::shareDenyWrite); 

	CArchiveEx ar(&FileStg, CArchive::load | CArchive::bNoFlushOnDelete, NULL, gszPassword, TRUE);
	
	// get current Dcl form
	CPictureObject* pPicture = new CPictureObject;
		
	// get dcl form into archive
	pPicture->Serialize(ar);
		
	ar.Close();			

	FileStg.Close();	// close the stream

	return pPicture;
}

void CPictureObject::SaveSS(CStgFile &FileStg) const
{
	CString sID;
	sID.Format(_T("%d"), m_nID);
	FileStg.Open(sID + gszPicture, CFile::modeCreate | CFile::modeWrite); 

	CArchiveEx ar(&FileStg, CArchive::store | CArchive::bNoFlushOnDelete, NULL, gszPassword, TRUE);
	ar.m_pDocument = NULL;
	ar.m_bForceFlat = FALSE;

	// put dcl form into archive
	const_cast<CPictureObject*>(this)->Serialize(ar);

	ar.Close();			

	FileStg.Close();	// close the stream
}
*/
