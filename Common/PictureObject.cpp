#include "StdAfx.h"
#include "PictureObject.h"
#include "StgFile.h"
#include "ArchiveEx.h"
#include "Filing.h"
#include "Workspace.h"
#include "SafeImageListWrite.h"

//#include <AtlImage.h> //for CImage


static const CLSID _afx_CLSID_StdPicture2_V1 =
	{ 0xfb8f0824,0x0164,0x101b, { 0x84,0xed,0x08,0x00,0x2b,0x2e,0xc7,0x13 } };


static LPCTSTR gszPicture = _T(".pic");
static LPCTSTR gszPassword = _T("d32afd3aw3aq3fdaw3");

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


static bool ImageListAddPicture(CPictureHolder* pPicture, CImageList& ImageList, CSize& ImageSize, bool bApplyMask)
{
	if (NULL == pPicture->m_pPict)
		return false;
	bool bSuccess = true;

	// if picture is a bitmap
	if (PICTYPE_BITMAP == pPicture->GetType())
	{
		// get handle of the bitmap
		HBITMAP hBitmap = NULL;
		pPicture->m_pPict->get_Handle((OLE_HANDLE FAR *) &hBitmap);
		CBitmap bmpPic;
		bmpPic.Attach( hBitmap );

		// get dimensions of bitmap
		long lPicWidth = 0;
		pPicture->m_pPict->get_Width(&lPicWidth);
		long lPicHeight = 0;
		pPicture->m_pPict->get_Height(&lPicHeight);

		// convert coordinates from units to logical units
		CSize sizePic( lPicWidth, lPicHeight );
		CDC* pDC = CDC::FromHandle( ::GetDC( NULL ) );
		pDC->HIMETRICtoLP( &sizePic );

		// if image list has not been created
		if (!ImageList.m_hImageList)
		{			
			if (ImageSize.cx == 0)
			{
				ImageSize.cx = sizePic.cx;
				ImageSize.cy = sizePic.cy;
			}

			// create the image list
			if( !ImageList.Create(sizePic.cx, sizePic.cy, ILC_COLOR8 | (bApplyMask? ILC_MASK : 0), 0, 1) )
				bSuccess = false;

			// set the background color of the image list
			//ImageList.SetBkColor(RGB(nWhite,nWhite,nWhite));		
		}
		// add bitmap to imagelist; mask is ignored in this sample
		if( ImageList.Add( CBitmap::FromHandle(hBitmap), RGB(192, 192, 192) ) == -1 )
			bSuccess = false;
		bmpPic.DeleteObject();
	}
	// else if picture is an icon
	else if (PICTYPE_ICON == pPicture->GetType())
	{
		HICON hIcon;

		// get handle of the icon
		pPicture->m_pPict->get_Handle((OLE_HANDLE FAR *) &hIcon);

		// get dimensions of icon
		long lPicWidth = 0;
		pPicture->m_pPict->get_Width(&lPicWidth);
		long lPicHeight = 0;
		pPicture->m_pPict->get_Height(&lPicHeight);

		// convert coordinates from units to logical units
		CSize sizePic( lPicWidth, lPicHeight );
		CDC* pDC = CDC::FromHandle( ::GetDC( NULL ) );
		pDC->HIMETRICtoLP( &sizePic );

		// if image list has not been created
		if (!ImageList.m_hImageList)
		{			
			if (ImageSize.cx == 0)
			{
				ImageSize.cx = sizePic.cx;
				ImageSize.cy = sizePic.cy;
			}

			// create the image list
			if( !ImageList.Create(sizePic.cx, sizePic.cy, ILC_COLOR8 | (bApplyMask? ILC_MASK : 0), 1, 1) )
				bSuccess = false;

			// set the background color of the image list
			ImageList.SetBkColor(RGB(255,255,255));		
		}
		// add icon to image list
		if( ImageList.Add(hIcon) == -1 )
			bSuccess = false;
	}
	else
		bSuccess = false;
	
	return bSuccess;
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
: mnID( -1 )
{
	m_hPicture.CreateEmpty();
	CalcLogicalSize();
}

CPictureObject::CPictureObject( UINT nID )
: mnID( nID )
{
	m_hPicture.CreateEmpty();
	CalcLogicalSize();
}

CPictureObject::CPictureObject( UINT nID, LPCTSTR szFile, bool bApplyMask /*= false*/ )
: mnID( nID )
{
	LoadFile( szFile, bApplyMask );
}

CPictureObject::~CPictureObject()
{
}

//static
CPictureObject* CPictureObject::CreatePictureObject(short nID, LPPICTUREDISP NewPicture)
{
	//create new picture object
	CPictureObject *pPicture = new CPictureObject( nID );
	pPicture->m_hPicture.SetPictureDispatch(NewPicture);
	pPicture->CalcLogicalSize();
	return pPicture;
}

void CPictureObject::Clear()
{
	if( m_hPicture.m_pPict )
		m_hPicture.m_pPict->Release();
}

LPDISPATCH CPictureObject::GetPictureDisp() const
{
	if( !m_hPicture.m_pPict )
		return NULL;
	return const_cast< CPictureObject* >(this)->m_hPicture.GetPictureDispatch();
}

const HBITMAP CPictureObject::GetBitmap() const
{
	if( GetPicType() != PICTYPE_BITMAP )
		return NULL;
	HBITMAP hbmpPic = NULL;
	m_hPicture.m_pPict->get_Handle( (OLE_HANDLE*)&hbmpPic );
	return hbmpPic;
}

const HICON CPictureObject::GetIcon() const
{
	if( GetPicType() != PICTYPE_ICON )
		return NULL;
	HICON hIcon;
	m_hPicture.m_pPict->get_Handle( (OLE_HANDLE*)&hIcon );
	return hIcon;			
}

HICON CPictureObject::CloneIcon() const
{
	switch (GetPicType() )
	{
	case PICTYPE_ICON:
		{
			HICON hIcon;
			m_hPicture.m_pPict->get_Handle( (OLE_HANDLE*)&hIcon );
			return CopyIcon( hIcon );
			//return (HICON)CopyImage( hIcon, PICTYPE_ICON, 0, 0, 0 );
		}
	case PICTYPE_BITMAP:
		{
			HBITMAP hbmpClone = CloneBitmap();
			if (hbmpClone != NULL)
			{
				// add the bitmap to a image list for extraction
				CImageList ImageList;
				ImageList.Create(msizePic.cx, msizePic.cy, ILC_COLOR | ILC_MASK, 1, 1);
				ImageList.Add(CBitmap::FromHandle(hbmpClone), rgbLightGrey);
				HICON hIcon = ImageList.ExtractIcon(0);
				ImageList.DeleteImageList();
				DeleteObject(hbmpClone);
				return hIcon;
			}
		}
	}
	return NULL;
}

HBITMAP CPictureObject::CloneBitmap() const
{
	switch (GetPicType() )
	{
	case PICTYPE_BITMAP:
		{
			HBITMAP hbmpPic = NULL;
			m_hPicture.m_pPict->get_Handle((OLE_HANDLE*)&hbmpPic);
			return (HBITMAP)CopyImage( hbmpPic, IMAGE_BITMAP, 0, 0, 0 );
		}
	}
	return NULL;
}

short CPictureObject::GetPicType() const
{
	return const_cast<CPictureObject*>(this)->m_hPicture.GetType();
}

void CPictureObject::Update(LPPICTUREDISP NewPicture) 
{
	m_hPicture.SetPictureDispatch(NewPicture);
	CalcLogicalSize();
}

void CPictureObject::LoadResourceIcon( UINT nIconResId, HMODULE hResMod /*= NULL*/ )
{
	if( !hResMod )
		hResMod = theWorkspace.GetLocalResourceModule();
	HICON hIcon = LoadIcon( hResMod, MAKEINTRESOURCE(nIconResId) );
	if( hIcon )
		m_hPicture.CreateFromIcon( hIcon );
	else
		m_hPicture.CreateEmpty();
	CalcLogicalSize();
}

void CPictureObject::LoadFile( LPCTSTR szFile, bool bApplyMask /*= false*/ )
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

	if( bApplyMask )
	{
		CImageList imglTemp;
		CSize sizeImage( 0, 0 );
		if( ImageListAddPicture( &phPicture, imglTemp, sizeImage, true ) )
			phPicture.CreateFromIcon( imglTemp.ExtractIcon(0), TRUE );
	}

	Update(phPicture.GetPictureDispatch());
	phPicture.m_pPict = ipOld;
	pstm->Release();
	CalcLogicalSize();
}

//IOStatus CPictureObject::WriteToTextFile(FILE* pFile, const CString &fileName) const
//{
//  //savebug
//  fprintf(pFile, "\nCPictureObject");
//	writeInt(pFile, int(GetCurrentSaveVersion()));
//	
//	writeInt(pFile, mnID);
//	writeInt(pFile, msizePic.cx);
//	writeInt(pFile, msizePic.cy);
//
//	int nPictureType = const_cast<CPictureObject*>(this)->m_hPicture.GetType();
//	writeInt(pFile, nPictureType);
//
//	if (nPictureType == PICTYPE_BITMAP) {
//    HBITMAP hBitmap = NULL;
//    m_hPicture.m_pPict->get_Handle((OLE_HANDLE*)&hBitmap);
//
//    CImage img;
//    img.Attach(hBitmap);
//    writeImage(pFile, fileName, img);
//
//    hBitmap = img.Detach();
//    const_cast<CPictureObject*>(this)->m_hPicture.CreateFromBitmap(hBitmap, NULL, TRUE);
//	} else if (nPictureType == PICTYPE_METAFILE ||
//             nPictureType == PICTYPE_ENHMETAFILE) {
//    //Currently not quite sure how to handle these
//  } else if (PICTYPE_ICON == nPictureType) {
//    // else if picture is an icon
//		HICON hIconPic = NULL;
//		// get handle of the icon
//		m_hPicture.m_pPict->get_Handle((OLE_HANDLE FAR *) &hIconPic);
//
//		CImageList il;
//		il.Create(msizePic.cx, msizePic.cy, ILC_COLOR8 | ILC_MASK, 1, 1);
//		il.Add(hIconPic);
//    writeImageList(pFile, fileName, il);
//		il.DeleteImageList();
//	}
//	return statOK;
//}


void CPictureObject::Serialize(CArchive& ar)
{
  CObject::Serialize( ar );

  if (ar.IsStoring())
  {
    ar << GetCurrentSaveVersion();

    ar << unsigned long(mnID);

    short nPictureType = m_hPicture.GetType();
    ar << nPictureType;

		switch( nPictureType )
		{
		case PICTYPE_BITMAP:
		case PICTYPE_METAFILE:
		case PICTYPE_ENHMETAFILE:
			{
				_variant_t var( m_hPicture.GetPictureDispatch() );
				ar << COleVariant( var );
				break;
			}
		case PICTYPE_ICON:
			{
				HICON hIconPic = NULL;
				m_hPicture.m_pPict->get_Handle((OLE_HANDLE FAR *) &hIconPic); // get handle of the icon
				CImageList ImageList;
				ImageList.Create(msizePic.cx, msizePic.cy, ILC_COLOR8 | ILC_MASK, 1, 1);
				ImageList.Add(hIconPic);
				ar.Flush();
				ULONGLONG lPos1 = ar.GetFile()->GetPosition();
				SafeImageListWrite( ImageList.m_hImageList, &CArchiveStream( &ar ) );
				ar.Flush();
				ULONGLONG lPos2 = ar.GetFile()->GetPosition();
				ULONGLONG lSize = lPos2 - lPos1;
				ImageList.DeleteImageList();
				break;
			}
		}
  }
  else	
  {
    unsigned long nThisVersion;
    ar >> nThisVersion;

		if( nThisVersion > GetCurrentSaveVersion() )
			AfxThrowArchiveException(CArchiveException::badSchema, ar.m_strFileName );

		unsigned long ulID;
    ar >> ulID;
		mnID = ulID;
    if (nThisVersion == 1)
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
		if( nThisVersion < 8 )
		{ //discarding saved width and height, and recalculating with current device metrics
			int iHeight;
			ar >> iHeight;
			int iWidth;
			ar >> iWidth;
		}
		if( nThisVersion == 1 )
			return;

    short nPictureType = 0;
    if (nThisVersion == 3 || nThisVersion == 4 || nThisVersion == 6 )
		{
			int nDuplicatePictureType;
      ar >> nDuplicatePictureType;
			ar >> nPictureType;
			if( nPictureType != nDuplicatePictureType )
				nPictureType = nDuplicatePictureType;
		}
		else
			ar >> nPictureType;

		if( nPictureType < 0 )
		{
      mnID = -1;
			return;
		}
		
    if( nThisVersion == 2 || nThisVersion == 3 || nThisVersion >= 6 )
    {
      switch( nPictureType )
			{
			case PICTYPE_BITMAP:
			case PICTYPE_METAFILE:
			case PICTYPE_ENHMETAFILE:
				{
					try
					{		
						COleVariant var;
						LoadPicture(ar, var);
						CComQIPtr< IPictureDisp > pPicDisp( (var.vt == VT_DISPATCH)? var.pdispVal : NULL );	
						m_hPicture.SetPictureDispatch(pPicDisp);
					}
					catch(...) {}
					break;
				}
			case PICTYPE_ICON:
				{
					CImageList tempImage;
					tempImage.Read(&ar);
					if (tempImage.m_hImageList != NULL)
					{
						HICON hIcon = tempImage.ExtractIcon(0);
						m_hPicture.CreateFromIcon(hIcon, TRUE);			
						tempImage.DeleteImageList();
					}
					break;
				}
			}
		}
    else if( nThisVersion == 4 || nThisVersion == 5 )
    {
      switch( nPictureType )
			{
			case PICTYPE_BITMAP:
			case PICTYPE_METAFILE:
			case PICTYPE_ENHMETAFILE:
				{
					CArchivePropExchange px(ar);
					CString sPropName;
					sPropName.Format(_T("%d"), mnID);
					PX_Picture(&px, sPropName, m_hPicture);
					break;
				}
			case PICTYPE_ICON:
				{
					CImageList tempImage;
					tempImage.Read(&ar);
					if (tempImage.m_hImageList != NULL)
					{
						HICON hIcon = tempImage.ExtractIcon(0);
						m_hPicture.CreateFromIcon(hIcon, TRUE);			
						tempImage.DeleteImageList();
					}
					break;
				}
			}
		}
		CalcLogicalSize();
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
	CalcLogicalSize();
  return statInvalidFormat;
}

IOStatus CPictureObject::ReadFromTextFile3(std::ifstream &sFile, const CString &fileName)
{
  try
  {
		int nID;
    if (!readInt(sFile, nID)) return statInvalidFormat;
		mnID = nID;
		int iHeight;
    if (!readInt(sFile, iHeight)) return statInvalidFormat;
		int iWidth;
    if (!readInt(sFile, iWidth)) return statInvalidFormat;

    int iPicType;
    if (!readInt(sFile, iPicType)) return statInvalidFormat;

    //if (iPicType == PICTYPE_BITMAP) {
    //  CImage img;
    //  if (!readImage(sFile, fileName, img)) return statInvalidFormat;
    //  HBITMAP hBitmap = img.Detach();
    //  m_hPicture.CreateFromBitmap(hBitmap);
    //} else if (iPicType == PICTYPE_METAFILE 
    //           || iPicType == PICTYPE_ENHMETAFILE) {
    //  //Currently not handled
    //} else if (iPicType == PICTYPE_ICON) {
    //  CImageList il;
    //  if (!readImageList(sFile, fileName, il)) return statInvalidFormat;
    //  if (il.m_hImageList != NULL)
    //  {
    //    HICON hIcon = il.ExtractIcon(0);
    //    m_hPicture.CreateFromIcon(hIcon, TRUE);			
    //    il.DeleteImageList();
    //  }
    //}
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

void CPictureObject::CalcLogicalSize()
{
	if( !m_hPicture.m_pPict )
	{
		msizePic.SetSize( -1, -1 );
		return;
	}

	OLE_XSIZE_HIMETRIC lPicWidth;
	OLE_YSIZE_HIMETRIC lPicHeight;
	m_hPicture.m_pPict->get_Width( &lPicWidth );
	m_hPicture.m_pPict->get_Height( &lPicHeight );
	msizePic.SetSize( lPicWidth, lPicHeight );

	// convert coordinates from units to logical units
	CDC* pDC = CDC::FromHandle( ::GetDC( NULL ) );
	pDC->HIMETRICtoLP( &msizePic );
}
