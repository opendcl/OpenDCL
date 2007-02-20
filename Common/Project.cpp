// CProject.cpp : implementation file
//

#include "stdafx.h"
#include "Project.h"
#include "PropertyIds.h"
#include "SharedRes.h"
#include "DclControlObject.h"
#include "OleControlObject.h"
#include "PropertyObject.h"
#include "VarUtils.h"
#include "ArchiveEx.h"
#include "AxContainer.h"
#include "StgFile.h"
#include "Filing.h"
#include "DclFormObject.h"
#include "PictureObject.h"
#include "ControlTypes.h"
#include "AxPropertyDescriptor.h"
#include "AxEventDescriptor.h"
#include "AxMethodDescriptor.h"
#include "PurchaseMode.h"
#include "ProjectCollection.h"
#include <fstream>
#include <stdio.h>


#define RELEASE(lpUnk) do \
	{ if ((lpUnk) != NULL) { (lpUnk)->Release(); (lpUnk) = NULL; } } while (0)

#define DELETE_EXCEPTION(e) do { e->Delete(); } while (0)


static CString LTOA(int nVal)
{
  CString sLong;
	sLong.Format(_T("%d"), nVal);
  return sLong;
}


static CString StripPathFromFileName( LPCTSTR pszFilePath )
{
	CString sShortName = pszFilePath;
	sShortName.MakeReverse();
	sShortName = sShortName.SpanExcluding(_T("\\/:"));
	sShortName.MakeReverse();
	return sShortName;
}


static CString CreateUniqueName() 
{
	CString sUniqueName;
	UUID uuid;
#ifdef _UNICODE
	RPC_WSTR pUUID;
#else
	RPC_CSTR pUUID;
#endif
	UuidCreate(&uuid);
	UuidToString(&uuid, &pUUID);
	sUniqueName = (LPCTSTR)pUUID;
	return sUniqueName;
}

static void AddControlProperty(CDclControlObject *pControl, PropertyId nID, LPCTSTR strValue, PropertyType ValueType)
{
	// find the insert position for this new property
	POSITION InsertPos = pControl->FindPropertyInsertPos(nID, false);

	// create new property object pointer to pass to AddTail method
	RefCountedPtr< CPropertyObject > pPropertyObect = new CPropertyObject( ValueType, 0, nID );

	// assign values
	pPropertyObect->SetStringValue(strValue);
	pPropertyObect->SetHidden(false);

	// reset the name to the new value
	if (InsertPos == NULL)
		pControl->GetPropertyList().AddTail(pPropertyObect);
	else
		pControl->GetPropertyList().InsertAfter(InsertPos, pPropertyObect);
}


/////////////////////////////////////////////////////////////////////////////
// CProject
IMPLEMENT_SERIAL(CProject, CObject, 1)

CProject::CProject()
{
	Initialize();
}

CProject::CProject( LPCTSTR pszKeyName )
: msKeyName( pszKeyName )
{
	Initialize();
}

void CProject::Initialize()
{
  mrsActiveXFiles.RemoveAll();
  mrsActiveXFiles.SetSize(0,1);
  msPassword.Empty();
  mbHasPassword = false;
  msLispFileName.Empty();	
  msDistFileName.Empty();
  mnAutoCADVersion = theWorkspace.GetMinSupportedAcadVersion();
  mbFreeVersion = false;
	mnPurchaseState = nCurrentPurchaseMode;

  CString sSection = theWorkspace.LoadResourceString(IDR_MAINFRAME);
	CWinApp* pApp = AfxGetApp();
  m_sDefaultFontName = pApp->GetProfileString( sSection, theWorkspace.LoadResourceString(IDS_DefaultFontName), NULL);
  m_nDefaultFontSize = pApp->GetProfileInt( sSection, theWorkspace.LoadResourceString(IDS_DefaultFontSize), 0);
  m_bDefaultFontItalic = pApp->GetProfileInt( sSection, theWorkspace.LoadResourceString(IDS_DefaultFontItalic), 0);
  m_bDefaultFontUnderLine = pApp->GetProfileInt( sSection, theWorkspace.LoadResourceString(IDS_DefaultFontUnderLine), 0);
  m_bDefaultFontBold = pApp->GetProfileInt( sSection, theWorkspace.LoadResourceString(IDS_DefaultFontBold), 0);
  m_bDefaultFontSizeStyle = pApp->GetProfileInt( sSection, theWorkspace.LoadResourceString(IDS_DefaultFontSizeStyle), 1);
  if (m_sDefaultFontName.IsEmpty())
    m_sDefaultFontName = theWorkspace.LoadResourceString(IDS_DEFAULTFONT);
  if (m_nDefaultFontSize == 0)
    m_nDefaultFontSize = 8;	
}


CProject::~CProject()
{
  // clear the project
  ClearProject();

  // create a position variable to hold the counter increment
	POSITION pos = mClipBoard.GetHeadPosition();	
  while(pos)
  {
		POSITION posAt = pos;
    CDclControlObject* pControl = mClipBoard.GetNext(pos);
    pControl->ClearProperties(); // clear properties in this control
    mClipBoard.RemoveAt(posAt);
    delete pControl;
  }
}

void CProject::SetKeyName( LPCTSTR pszKeyName )
{
	if( !pszKeyName )
		return;
	CString sKey = pszKeyName;
	sKey = sKey.MakeReverse().SpanExcluding( _T("\\/:") ).MakeReverse().SpanExcluding( _T(".") );
	sKey.Replace(_T(' '), _T('|'));
	msKeyName = sKey;
}

void CProject::DeleteForm( CDclFormObject*& pDclForm )
{
	POSITION pos = mDclForms.GetHeadPosition();
	while( pos )
	{
		POSITION posAt = pos;
		CDclFormObject* pThisForm = mDclForms.GetNext( pos );
		assert( pThisForm != NULL );
		if( pThisForm == pDclForm )
			mDclForms.RemoveAt( posAt ); // and remove the form from the list
		CDclFormObject* pParentForm = pThisForm->GetParentForm();
		if( pParentForm == pDclForm )
		{
			if( pThisForm->m_pMdiChildWnd ) // if the child form has a view open
				pThisForm->m_pMdiChildWnd->DestroyWindow(); // close the view
			delete pThisForm; // delete the object
			mDclForms.RemoveAt( posAt ); // and remove it from the list
		}
	}
	if( pDclForm->m_pMdiChildWnd ) // if the form has a view open
		pDclForm->m_pMdiChildWnd->DestroyWindow(); // close the view
	delete pDclForm; // delete the object
	pDclForm = NULL;
}

CDclFormObject* CProject::AddForm( DclFormType nType )
{
	CDclFormObject* pNewDclForm = new CDclFormObject( this, nType );
	pNewDclForm->SetUniqueName( CreateUniqueName() );
	mDclForms.AddTail( pNewDclForm );
	return pNewDclForm;
}

CDclFormObject* CProject::AddForm( DclFormType nType, CDclFormObject* pParentForm )
{
	assert( pParentForm != NULL );
	assert( pParentForm->GetProject() == this );
	CDclFormObject* pNewDclForm = new CDclFormObject( this, nType );
	pNewDclForm->SetUniqueName( pParentForm->GetUniqueName() );
	pNewDclForm->SetParentForm( pParentForm );
	mDclForms.AddTail( pNewDclForm );
	return pNewDclForm;
}

void CProject::SetGlobalVariableNames( LPCTSTR pszRootName /*= NULL*/ )
{
	if( pszRootName )
		SetKeyName( pszRootName );
	POSITION pos = mDclForms.GetHeadPosition();
	while( pos )
	{
		POSITION posAt = pos;
		CDclFormObject* pDclForm = mDclForms.GetNext( pos );
		assert( pDclForm != NULL );
		pDclForm->SetGlobalVariableName( pszRootName, true );
	}
}

void CProject::ClearGlobalVariableNames()
{
	POSITION pos = mDclForms.GetHeadPosition();
	while( pos )
	{
		POSITION posAt = pos;
		CDclFormObject* pDclForm = mDclForms.GetNext( pos );
		assert( pDclForm != NULL );
		pDclForm->ClearGlobalVariableName( true );
	}
}

bool CProject::AddActiveXFile( LPCTSTR pszFileName )
{
	if( HasActiveXFile( pszFileName ) )
		return false; //can't add (case insensitive) duplicate filename
	mrsActiveXFiles.Add( pszFileName );
	return true;
}

bool CProject::RemoveActiveXFile( LPCTSTR pszFileName )
{
	INT_PTR idx = mrsActiveXFiles.GetCount();
	while( idx-- > 0 )
	{
		if( mrsActiveXFiles.GetAt( idx ).CompareNoCase( pszFileName ) == 0 )
		{
			mrsActiveXFiles.RemoveAt( idx );
			return true;
		}
	}
	return false;
}

bool CProject::HasActiveXFile( LPCTSTR pszFileName ) const
{
	INT_PTR idx = mrsActiveXFiles.GetCount();
	while( idx-- > 0 )
	{
		if( mrsActiveXFiles.GetAt( idx ).CompareNoCase( pszFileName ) == 0 )
			return true;
	}
	return false;
}

CString CProject::QueryForLispFileName() 
{
  // create the open dialog box
  CFileDialog BrowseWnd(
    FALSE,
    _T(".lsp"), 
    msLispFileName, 
    OFN_HIDEREADONLY | OFN_ENABLESIZING | OFN_EXPLORER,
    theWorkspace.LoadResourceString(IDS_AUTOLISPFILE),
    CWnd::GetActiveWindow());

	// setup the file buffer
  CString sResult;
  BrowseWnd.m_ofn.nMaxFile = MAX_PATH;
  BrowseWnd.m_ofn.lpstrFile = sResult.GetBuffer( MAX_PATH );

  // invoke the file dialog box
  if(BrowseWnd.DoModal() != IDOK)
		return CString();
	sResult.ReleaseBuffer();

  // update the lsp file name
  msLispFileName = sResult;

  return sResult;	
}

CString CProject::QueryForOdsFileName() 
{
  // create the open dialog box
  CFileDialog BrowseWnd(
    FALSE,
    _T(".ods"), 
    msLispFileName, 
    OFN_HIDEREADONLY | OFN_ENABLESIZING | OFN_EXPLORER,
    theWorkspace.LoadResourceString(IDS_ODSFILTER),
    CWnd::GetActiveWindow());

	// setup the file buffer
  CString sResult;
  BrowseWnd.m_ofn.nMaxFile = MAX_PATH;
  BrowseWnd.m_ofn.lpstrFile = sResult.GetBuffer( MAX_PATH );

  // invoke the file dialog box
  if(BrowseWnd.DoModal() != IDOK)
		return CString();
	sResult.ReleaseBuffer();

  // update the lsp file name
  msDistFileName = sResult;

  return sResult;	
}

void CProject::ClearProject()
{
  //msKeyName.Empty(); //this should remain unchanged for the life of the project unless explicitly changed!
	msDistFileName.Empty();
	msLispFileName.Empty();
	msBaseFileName.Empty();
	msPassword.Empty();
	mbHasPassword = false;
	mbFreeVersion = false;
	mnAutoCADVersion = theWorkspace.GetMinSupportedAcadVersion();

	// clear dcl forms
  POSITION posDclForm = mDclForms.GetHeadPosition();	
	while (posDclForm)
	{
    CDclFormObject* pDclFormObject = mDclForms.GetNext(posDclForm);
		if (pDclFormObject)
			pDclFormObject->ClearControls();
    delete pDclFormObject;
	}
	mDclForms.RemoveAll();

  // clear pictures
  POSITION posPicture = mPictures.GetHeadPosition();	
	while (posPicture)
		try{ delete mPictures.GetNext(posPicture); } catch(...){}
	mPictures.RemoveAll();

  // clear OLE controls
	mOleControls.clear();

	mClipBoard.RemoveAll();
  mrsActiveXFiles.RemoveAll();
}

void CProject::ClearR14Events()
{
  POSITION pos = mDclForms.GetHeadPosition();	
	while (pos)
	{
    CDclFormObject* pDclFormObject = mDclForms.GetNext(pos);
		if (pDclFormObject)
			pDclFormObject->ClearR14Events();
    delete pDclFormObject;
	}
}

INT_PTR CProject::AddPicture(CPictureObject* pPicture) 
{
	HDC hdc = ::GetDC(GetDesktopWindow());
	CDC * cdc = CDC::FromHandle(hdc);

	// assign picture object values	
	long lPicWidth;
	long lPicHeight;

	// get dimensions of bitmap
	pPicture->GetPicture().m_pPict->get_Width(&lPicWidth);
	pPicture->GetPicture().m_pPict->get_Height(&lPicHeight);

	CSize sizePic;
	sizePic.cx = (int)lPicWidth;
	sizePic.cy = (int)lPicHeight;

	// convert coordinates from units to logical units
	cdc->HIMETRICtoLP(&sizePic);
	cdc->Detach();
	pPicture->SetWidth(sizePic.cx);
	pPicture->SetHeight(sizePic.cy);

	// add the new picture object
	mPictures.AddTail(pPicture);
	
	// return the index that this new picture object was inserted at
	return mPictures.GetCount() - 1;
}

bool CProject::LoadPictureFile(LPCTSTR szFile, int nID)
{
	CPictureObject* pNewPic = new CPictureObject;
	pNewPic->LoadFile( szFile, nID );
	return (AddPicture( pNewPic ) >= 0);
}

void CProject::AddOleObject(const CLSID& clsid, CAxContainer *pAxCont)
{
	if (HasOleObject (clsid))
		return;
  COleControlObject *pObject = new COleControlObject( this, clsid );

  if (clsid == IID_IPictureDisp)
    pObject->SetAxTypeName( theWorkspace.LoadResourceString(IDS_PROP_PICTURE) );
  else if (pObject->m_clsid == IID_IFontDisp)
    pObject->SetAxTypeName( theWorkspace.LoadResourceString(IDS_PROP_LABEL_NAME) );
  else if (pObject->m_clsid == GUID_COLOR)
    pObject->SetAxTypeName( theWorkspace.LoadResourceString(IDS_COLOR) );
  mOleControls.push_back(pObject);	
  pAxCont->ExtractComponentsFromTLB(pObject, clsid);
}

bool CProject::HasOleObject(const CLSID& clsid)
{
	std::vector< RefCountedPtr< COleControlObject > >::const_iterator pos = mOleControls.begin();
  while (pos != mOleControls.end())
  {
    RefCountedPtr< COleControlObject > pObject = *pos++;
		assert( pObject != NULL );
    if (pObject != NULL && pObject->m_clsid == clsid)
      return true;
  }
  return false;
}

RefCountedPtr< COleControlObject > CProject::GetOleObject(const CLSID& clsid)
{
	std::vector< RefCountedPtr< COleControlObject > >::const_iterator pos = mOleControls.begin();
  while (pos != mOleControls.end())
  {
    RefCountedPtr< COleControlObject > pObject = *pos++;
		assert( pObject != NULL );
    if (pObject != NULL && pObject->m_clsid == clsid)
      return pObject;
  }
  return NULL;
}

CString CProject::GetOleObjectName(const AxPropertyDescriptor *pProperty)
{
	RefCountedPtr< COleControlObject > pOleControl = GetOleObject( pProperty );
	if( pOleControl )
		pOleControl->GetAxTypeName();
  return theWorkspace.LoadResourceString(IDS_OleObject);
}


RefCountedPtr< COleControlObject > CProject::GetOleObject(const AxEventDescriptor *pEvent)
{	
  if (pEvent == NULL)
    return NULL;

	std::vector< RefCountedPtr< COleControlObject > >::iterator pos = mOleControls.begin();
  while (pos != mOleControls.end())
  {
    RefCountedPtr< COleControlObject > pObject = *pos++;
		assert( pObject != NULL );
    if (pObject != NULL)
    {
      for (size_t idx = 0; idx < pEvent->GetArgs().size(); ++idx)
      {
        if (pEvent->GetArgs().at(idx).vt == VT_DISPATCH || pEvent->GetArgs().at(idx).vt == VT_UNKNOWN)
        {
          if (pObject->m_clsid == pEvent->GetArgs().at(idx).clsid )
            return pObject;
        }
      }	
    }
  }
  return NULL;
}

RefCountedPtr< COleControlObject > CProject::GetOleObject(const AxPropertyDescriptor *pProperty)
{	
  if (pProperty == NULL)
    return NULL;

	std::vector< RefCountedPtr< COleControlObject > >::iterator pos = mOleControls.begin();
  while (pos != mOleControls.end())
  {
    RefCountedPtr< COleControlObject > pObject = *pos++;
		assert( pObject != NULL );
    if (pObject != NULL)
    {
      if (pObject->m_clsid == pProperty->GetGuid())
        return pObject;
			size_t idx = pProperty->GetArgs().size();
      while (idx-- > 0)
      {
				if (pObject->m_clsid == pProperty->GetArgs().at(idx).clsid)
          return pObject;
      }	
    }
  }
  return NULL;
}

RefCountedPtr< COleControlObject > CProject::GetOleObject(const AxMethodDescriptor *pMethod)
{	
  if (pMethod == NULL)
    return NULL;

	std::vector< RefCountedPtr< COleControlObject > >::iterator pos = mOleControls.begin();
  while (pos != mOleControls.end())
  {
    RefCountedPtr< COleControlObject > pObject = *pos++;
		assert( pObject != NULL );
    if (pObject != NULL)
    {
      if (pObject->m_clsid == pMethod->GetReturnGuid())
        return pObject;
      for (size_t idx = 0; idx < pMethod->GetArgs().size(); ++idx)
      {
				if (pObject->m_clsid == pMethod->GetArgs().at(idx).clsid)
          return pObject;
      }	
    }
  }
  return NULL;
}

CDclFormObject* CProject::GetDclForm(CString DclFormName)
{
  POSITION pos;
  int nCount = 0;

  while (nCount < mDclForms.GetCount())
  {
    // always get the first index when clearing a list
    pos = mDclForms.FindIndex(nCount);

    // get the object
    CDclFormObject *pDclForm = mDclForms.GetAt(pos);

    // if this dcl form's name matches the one queried for
    if (pDclForm->GetKeyName() == DclFormName)
      // return the dcl form pointer
      return pDclForm;

    // increment the counter to include the next object
    nCount++;
  }
  return NULL;
}


CDclFormObject* CProject::GetParentDclForm(CString sParentName)
{
	POSITION pos = mDclForms.GetHeadPosition();
	while (pos)
	{
		CDclFormObject* pDclForm = mDclForms.GetNext(pos);
    if (pDclForm->GetUniqueName() == sParentName)
      return pDclForm;
	}
  return NULL;
}


CDclFormObject* CProject::GetDclTabChildForm(CString sDclParentUniqueName, int nTabIndex)
{
	POSITION pos = mDclForms.GetHeadPosition();
	while (pos)
	{
		CDclFormObject* pDclForm = mDclForms.GetNext(pos);
    if (pDclForm->GetParentName() == sDclParentUniqueName && pDclForm->GetTabIndex() == nTabIndex)
      return pDclForm;
	}
  return NULL;
  return NULL;
}

int CProject::CountDeletedForms()
{
  // set counter for mDclForms
  int nCount = mDclForms.GetCount();
  int nDeleted = 0;

  // set start position for navigating mDclForms
  POSITION pos = mDclForms.GetHeadPosition();

  // do loop to navigate mDclForms
  while (pos != NULL)
  {
    // get current mDclForms
    CDclFormObject *pDclForm = mDclForms.GetNext(pos);

    if (pDclForm->m_bDeleted)
      nDeleted++;

    // increment counter
    nCount--;
  }

  return nDeleted;
}


HBITMAP CProject::GetBitmap(UINT nID, CSize &sz) const
{
	HBITMAP hBmp = NULL;
	POSITION pos = mPictures.GetHeadPosition();
	while (pos != NULL)
	{
		// get current Picture in list
		CPictureObject* pPicture = mPictures.GetNext(pos);
		if (pPicture != NULL && pPicture->GetID() == nID)
		{
			if (pPicture->GetPicture().m_pPict != NULL)
			{
				// get the bitmap
				if (pPicture->GetPicType() == PICTYPE_BITMAP)
				{
					pPicture->GetPicture().m_pPict->get_Handle((OLE_HANDLE FAR *) &hBmp);										
					sz.cx = pPicture->GetWidth();
					sz.cy = pPicture->GetHeight();
				}
				if (pPicture->GetBitmap())
					return pPicture->GetBitmap();
			}
		}
	}
	return hBmp;
}


HICON CProject::GetIcon(UINT nID) const
{
	HICON hIcon = NULL;
	POSITION pos = mPictures.GetHeadPosition();
	while (pos != NULL)
	{
		// get current Picture in list
		CPictureObject* pPicture = mPictures.GetNext(pos);
		if (pPicture != NULL && pPicture->GetID() == nID)
		{
			if (pPicture->GetPicture().m_pPict != NULL)
			{
				// get the bitmap
				if (pPicture->GetPicType() == PICTYPE_ICON)
					pPicture->GetPicture().m_pPict->get_Handle((OLE_HANDLE FAR *) &hIcon);		
			}
			if (pPicture->GetIcon() != NULL)
				return pPicture->GetIcon();					
			if (pPicture->m_hLoadedIcon != NULL)
				return pPicture->m_hLoadedIcon;					
			return hIcon;
		}
	}
	return hIcon;
}


CPictureObject* CProject::FindPicture( UINT nID ) const
{
	POSITION pos = mPictures.GetHeadPosition();
	while (pos)
	{
		CPictureObject* pPicture = mPictures.GetNext(pos);
		assert (pPicture != NULL);
		if (pPicture->GetID() == nID)
			return pPicture;
	}
	return NULL;
}


bool CProject::GetPictureSize( UINT nID, CSize& size ) const
{
	POSITION pos = mPictures.GetHeadPosition();
	while (pos)
	{
		CPictureObject* pPicture = mPictures.GetNext(pos);
		assert (pPicture != NULL);
		if (!pPicture || pPicture->GetID() != nID)
			continue;
		if (!pPicture->GetPicture().m_pPict)
			return false;
		size.cx = pPicture->GetWidth();
		size.cy = pPicture->GetHeight();
		return true;
	}
	return false;
}


bool CProject::IsInUse() const
{
	POSITION pos = mDclForms.GetHeadPosition();
	while( pos )
	{
		CDclFormObject* pForm = mDclForms.GetNext( pos );
		assert( pForm != NULL );
		if( !pForm )
			continue;
		if( pForm->GetFormInstance() )
			return true; //there is an active dialog using this form
	}
	return false;
}


IOStatus CProject::ReadFromFile( LPCTSTR pszFilePath )
{
	CFile SrcFile;
	try
	{
		SrcFile.Open( pszFilePath, CFile::modeRead | CFile::shareDenyWrite | CFile::osSequentialScan );
		if (!SrcFile)
			return statFail;

		msProjectFilePath = SrcFile.GetFilePath();
		msBaseFileName = StripPathFromFileName( msProjectFilePath );

		//Check the first line -- if it is "ObjectDCL Project", then it is a text file.
		char szTextFileSignature[] = "ObjectDCL Project";
		char szLine1[sizeof(szTextFileSignature)];
		UINT_PTR cbRead = SrcFile.Read(szLine1, sizeof(szLine1) - 1);
		szLine1[cbRead] = '\0';

		if( lstrcmpA( szTextFileSignature, szLine1 ) == 0 )
		{ //this file is in plain text format
			SrcFile.Close();
			IOStatus stat = ReadFromTextFile( pszFilePath );
			return stat;
		}

		SrcFile.SeekToBegin();
		try
		{
			CArchiveEx archSource(&SrcFile, CArchive::load | CArchive::bNoFlushOnDelete, NULL, _T("ObjectDCL"), TRUE);
			archSource.m_pDocument = theWorkspace.GetActiveDocument();
			archSource.m_bForceFlat = FALSE;
			if( SrcFile.GetLength() != 0 )
				Serialize(archSource);     // load me
		}
		catch(...)
		{
			SrcFile.Close();
			return statInvalidFormat;
		}
	}
	catch(...)
	{
		SrcFile.Close();
		return statFileNotFound;
	}
	SrcFile.Close();
	return statOK;
}


IOStatus CProject::WriteToFile( LPCTSTR pszFilePath )
{
#ifdef SAVE_IN_TEXT_FORMAT
  FILE* pTextFile = fopen(pszPathName, "w");
  if (errno != 0)
    return FALSE;
	CString sFilename = pszFilePath;
  WriteToTextFile(pTextFile, sFileName);
  fclose(pTextFile);
#else //SAVE_IN_TEXT_FORMAT

	TRY
	{
		CFile DestFile( pszFilePath, CFile::modeWrite | CFile::shareExclusive | CFile::modeCreate );
		if (!DestFile)
			return statFileNotFound;
		{
			CArchiveEx archSave(&DestFile, CArchive::store, NULL, _T("ObjectDCL"), TRUE);
			Serialize(archSave);
			archSave.Close();
			msProjectFilePath = DestFile.GetFilePath();
		}
		DestFile.Flush();
	}
	CATCH_ALL(e)
	{
		DELETE_EXCEPTION(e);
		return statFail;
	}
	END_CATCH_ALL;
#endif //SAVE_IN_TEXT_FORMAT
	return statOK;
}


// This class is used for deserializing OLE controls from versions 8 and 9 project archives
// (the class must have the same name as the original, since the original code used MFC's
// typesafe serialization mechanism, which writes the class name to the archive) [ORW]
class CArxControlObject : public COleControlObject
{ CArxControlObject() : COleControlObject( NULL ) {} DECLARE_SERIAL(CArxControlObject); };
	IMPLEMENT_SERIAL(CArxControlObject, CObject, 1);


void CProject::Serialize(CArchive& ar)
{
  ULONG nThisVersion = GetCurrentSaveVersion();

  CObject::Serialize(ar);
  // create a position variable to hold the counter increment
  POSITION pos;	
  short nCount;

  if (ar.IsStoring())
  {
    ar << GetCurrentSaveVersion();
    ar << BOOL(mbHasPassword);
    ar << msPassword;
    ar << msLispFileName;
    ar << msKeyName; //project key
    ar << msDistFileName;

    ar << unsigned long(mDclForms.GetCount() - CountDeletedForms());
    ar << mnPurchaseState;
    ar << mnAutoCADVersion;

    pos = mDclForms.GetHeadPosition();
    while (pos != NULL)
    {
      CDclFormObject* pDclForm = mDclForms.GetNext(pos);
      if (!pDclForm->m_bDeleted)
			{
        pDclForm->Serialize(ar);
			#ifdef _DEBUG
				pDclForm->dumpDebugger( false );
			#endif
			}
    }

		unsigned long ctAxFiles = unsigned long(mrsActiveXFiles.GetCount());
		ar << ctAxFiles;
		for( unsigned long idx = 0; idx < ctAxFiles; ++idx )
			ar << mrsActiveXFiles.GetAt( idx );

		unsigned long ctOleControls = unsigned long(mOleControls.size());
		ar << ctOleControls;
		for( unsigned long idx = 0; idx < ctOleControls; ++idx )
			mOleControls.at( idx )->Serialize( ar );

    ar << unsigned long(mPictures.GetCount());		
    pos = mPictures.GetHeadPosition();
    while (pos != NULL)
      mPictures.GetNext(pos)->Serialize(ar);
  }
  else
  {		
    ar >> nThisVersion;
		if (nThisVersion > GetCurrentSaveVersion())
			AfxThrowArchiveException(CArchiveException::badSchema, ar.m_strFileName );

		BOOL bHasPassword;
    ar >> bHasPassword;
		mbHasPassword = (bHasPassword != FALSE);
    ar >> msPassword;		
    ar >> msLispFileName;

    // lets check here if we need to add the lisp file extension
    CString sNone = theWorkspace.LoadResourceString(IDS_NONE);
    if (msLispFileName.Right(4).CompareNoCase(_T(".lsp")) != 0 /*.lsp*/ && msLispFileName != sNone /*"<None>"*/)
      msLispFileName += _T(".lsp");
		else if (msLispFileName == _T(".lsp") || msLispFileName == (sNone + _T(".lsp")))
			msLispFileName = sNone; //drop the extraneous extension

		CString sKeyName;
    if (nThisVersion >= 10)
      ar >> sKeyName; //this is the project key name
    else
		{
			if (nThisVersion >= 5)
				ar >> sKeyName; //this is the full project file path as saved
			else
				sKeyName = msLispFileName;
			sKeyName = StripPathFromFileName(sKeyName).SpanExcluding(_T("."));
			sKeyName.Replace( _T(' '), _T('_') );
		}
		if( msKeyName.IsEmpty() ) //if it's already set, don't change it
			SetKeyName( sKeyName );

    if (nThisVersion >= 6)
    {
      // this is used for a distribution file with multiple projects in it.
      ar >> msDistFileName;

      // lets check here if we need to add the ods file extension
      if (msDistFileName.Right(4).CompareNoCase(_T(".ods")) != 0 /*.ods*/ && msDistFileName != sNone /* "<None>"*/)
        msDistFileName += _T(".ods");
			else if (msDistFileName == _T(".ods") || msDistFileName == (sNone + _T(".ods")))
				msDistFileName = sNone; //drop the extraneous extension
    }
    else
      msDistFileName = sNone;

    // this is a fix for an older password protect scheme.
    if (nThisVersion != 2 && nThisVersion != 3)
    {
      mbHasPassword = false;
      msPassword.Empty();
    }

    // get counter for dcl forms
		unsigned long ctForms = 0;
		if( nThisVersion < 11 )
		{
			short nCount;
			ar >> nCount;
			ctForms = (unsigned long)nCount;
		}
		else
			ar >> ctForms;
		
		ar >> mnPurchaseState;

    if (nThisVersion >= 4)
      ar >> mnAutoCADVersion;
    else
      // otherwise set the default to release nDeAcadVersion
      mnAutoCADVersion = theWorkspace.GetMinSupportedAcadVersion();

    // do loop to navigate dcl forms
    while (ctForms-- > 0)
    {
      CDclFormObject* pDclForm = new CDclFormObject( this );
      pDclForm->Serialize(ar);

			//Some projects for whatever reason have parentless tab page forms. This code removes those 
			//forms so they don't confuse the project list in the editor. [ORW]
			if( pDclForm->GetType() == VdclTabForm && !pDclForm->GetParentForm() )
			{
				delete pDclForm;
				continue;
			}

			//pDclForm->UpdateGlobalVariable(GetKeyName());
      mDclForms.AddTail(pDclForm);
		#ifdef _DEBUG
			pDclForm->dumpDebugger( false );
		#endif
    }

		mrsActiveXFiles.RemoveAll();
    if (nThisVersion >= 7)
		{
			if( nThisVersion <= 9 )
				mrsActiveXFiles.Serialize(ar);
			else
			{
				unsigned long ctAxFiles;
				ar >> ctAxFiles;
				for( unsigned long idx = 0; idx < ctAxFiles; ++idx )
				{
					CString sFile;
					ar >> sFile;
					mrsActiveXFiles.Add( sFile );
				}
			}
		}

		mOleControls.clear();
    if (nThisVersion >= 8)
		{
			if (nThisVersion <= 9)
			{
				CTypedPtrList< CObList, CArxControlObject* > listControls;
				listControls.Serialize(ar);
				pos = listControls.GetHeadPosition();
				while (pos)
					mOleControls.push_back(listControls.GetNext(pos));
			}
			else
			{
				unsigned long ctOleControls;
				ar >> ctOleControls;
				for( unsigned long idx = 0; idx < ctOleControls; ++idx )
				{
					COleControlObject* pOleControl = new COleControlObject( this );
					pOleControl->Serialize( ar );
					mOleControls.push_back( pOleControl );
				}
			}
		}


    if (nThisVersion < 9)
		{
			try
			{
				mPictures.Serialize(ar);
			}
			catch(...) {}
		}
		else
    {
			unsigned long nCount = 0;
			if( nThisVersion < 10 )
			{
				short sCount;
				ar >> sCount;
				nCount = (unsigned long)sCount;
			}
			else
				ar >> nCount;

      mPictures.RemoveAll();
      while (nCount-- > 0)
      {
        CPictureObject* pPictureObj = new CPictureObject;
        pPictureObj->Serialize(ar);
        mPictures.AddTail(pPictureObj);							
      }
    }

		// this has been added to cleanup picture objects with a blank picture.
		nCount = mPictures.GetCount() - 1;
		for (int i=nCount; i>= 0; i--)
		{
			pos = mPictures.FindIndex(i);
			if (pos != NULL)
			{
				CPictureObject *pPict = mPictures.GetAt(pos);

				if (pPict == NULL)
					mPictures.RemoveAt(pos);
				if (pPict != NULL)
				{
					if (pPict->GetID() <= -1 || pPict->GetID() >= 3000) //was nPicIdToHight, moved to common
					{
						delete pPict;
						mPictures.RemoveAt(pos);
					}
				}
			}
		}
  }

}

void CProject::SaveDistFile()
{
  CString sNone = theWorkspace.LoadResourceString(IDS_NONE);
  // now save the distribution file if required
  if (!msDistFileName.IsEmpty() &&
			msDistFileName != sNone &&
			msDistFileName != _T(".ods") &&
			msDistFileName != sNone + _T(".ods"))
  {
    CProjectCollection phProjects;

    phProjects.AddProject(this);

    // declare file variables
    CFileException Exception;
    CFile ThisFile;

    // open the file
    if ( !ThisFile.Open(msDistFileName, CFile::modeCreate | CFile::shareExclusive | CFile::modeWrite, &Exception) )
    {
      CString sTheFile;
      sTheFile = theWorkspace.LoadResourceString(IDS_THEFILE);
      CString sText;
      sText = theWorkspace.LoadResourceString(IDS_COULDNOTOPEN);

      MessageBox (::GetActiveWindow(), sTheFile + msDistFileName + sText, theWorkspace.LoadResourceString(IDR_MAINFRAME), NULL);
      return;
    }

    CArchiveEx ar(&ThisFile, CArchive::store | CArchive::bNoFlushOnDelete, NULL, _T("ObjectDCL"), TRUE);

    // Serialize the control
    phProjects.Serialize(ar);
    ar.Close();

    ThisFile.Close();
  }
}


IOStatus CProject::ReadFromTextFile( LPCTSTR lpszFilePath ) 
{
	IOStatus stat = statOK;
	std::ifstream sFile(lpszFilePath, std::ios::in);
	InitFilerGlobals(); //Init the globals before reading anything from the file.
	if (readLine(sFile) != "ObjectDCL Project")
		stat = statInvalidFormat;
	else
	{
		int nThisVersion;
		if (!readInt(sFile, nThisVersion))
			stat = statInvalidFormat;
		else
		{
			switch (nThisVersion)
			{
				case 9 : stat = ReadFromTextFile9(sFile, lpszFilePath);
					break;
				default : stat = statInvalidFormat;
			}
		}
	}
	sFile.close();
	return stat;
}

IOStatus CProject::ReadFromTextFile(std::ifstream &sFile, const CString &fileName)
{
  //Init the globals before reading anything from the file.
  InitFilerGlobals();

  if (readLine(sFile) != "ObjectDCL Project") return statInvalidFormat;

  int nThisVersion;
  if (!readInt(sFile, nThisVersion)) return statInvalidFormat;

  switch (nThisVersion) {
    case 9 : return ReadFromTextFile9(sFile, fileName);
  }
  return statInvalidFormat;
}

IOStatus CProject::ReadFromTextFile9(std::ifstream &sFile, const CString &fileName)
{
  int nCount;
  POSITION pos;

	BOOL bHasPassword;
  if (!readBOOL(sFile, bHasPassword)) return statInvalidFormat;
	mbHasPassword = (bHasPassword != FALSE);
  if (!readString(sFile, msPassword)) return statInvalidFormat;
  if (!readString(sFile, msLispFileName)) return statInvalidFormat;
  // lets check here if we need to add the ods file extension
  CString strResult = msLispFileName;
  strResult.MakeLower();

  CString sNone = theWorkspace.LoadResourceString(IDS_NONE);
  if (strResult != sNone && strResult.Right(4).CompareNoCase(_T(".lsp")) != 0)
    msLispFileName += _T(".lsp");

  // this is the original project file path; use it to construct the project key
	CString sKeyName;
  if (!readString(sFile, sKeyName)) return statInvalidFormat;
	if (!sKeyName.IsEmpty())
	{
		sKeyName = StripPathFromFileName(sKeyName).SpanExcluding(_T("."));
		sKeyName.Replace( _T(' '), _T('_') );
		SetKeyName( sKeyName );
	}

  // this is used for a distribution file with multiple projects in it.
  if (!readString(sFile, msDistFileName)) return statInvalidFormat;

	// lets check here if we need to add the ods file extension
  if (msDistFileName != sNone &&
			msDistFileName.Right(4).CompareNoCase(theWorkspace.GetDistributionFileExtension()) != 0)
    msDistFileName += theWorkspace.GetDistributionFileExtension();

  // get counter for dcl forms
  if (!readInt(sFile, nCount)) return statInvalidFormat;
  if (!readInt(sFile, (int&)mnPurchaseState)) return statInvalidFormat;

  if (!readInt(sFile, (int&)mnAutoCADVersion)) return statInvalidFormat;

  // do loop to navigate dcl forms
  while (nCount-- > 0)
  {
    // get current Dcl form
    CDclFormObject* pDclForm = new CDclFormObject( this );

    // get dcl form into archive
		IOStatus stat = pDclForm->ReadFromTextFile(sFile, fileName);
    if (stat != statOK) return stat;

    // add this dcl form to the list object
    mDclForms.AddTail(pDclForm);

  }	

	mrsActiveXFiles.RemoveAll();
  int iCount;
  if (!readInt(sFile, iCount)) return statInvalidFormat;
  for (int i = 0; i < iCount; i++) {
    CString str;
    if (!readString(sFile, str)) return statInvalidFormat;
    mrsActiveXFiles.Add(str);
  }

  if (!readInt(sFile, iCount)) return statInvalidFormat;
  for (int i = 0; i < iCount; i++) {
    COleControlObject* pOleControl = new COleControlObject( this );
		IOStatus stat = pOleControl->ReadFromTextFile(sFile, fileName);
    if (stat != statOK) return stat;
    mOleControls.push_back(pOleControl);
  }

  // set counter
  if (!readInt(sFile, nCount)) return statInvalidFormat;

  mPictures.RemoveAll();
  // do loop to navigate images
  while (nCount-- > 0)
  {
    // get current images
    CPictureObject* pPictureObj = new CPictureObject;

    try
    {	
      // get image into archive
			IOStatus stat = pPictureObj->ReadFromTextFile(sFile, fileName);
      if (stat != statOK) return stat;

      // add this image to the list object
      mPictures.AddTail(pPictureObj);							
    }
    catch(...)
    {
      // do nothing
    }
  }

  // this has been added to cleanup picture objects with a blank picture.
  nCount = mPictures.GetCount() - 1;
  for (int i=nCount; i>=0; i--)
  {
    pos = mPictures.FindIndex(i);
    if (pos != NULL)
    {
      CPictureObject *pPict = mPictures.GetAt(pos);

      if (pPict == NULL)
        mPictures.RemoveAt(pos);
      if (pPict != NULL)
      {
        if (pPict->GetID() <= -1 || pPict->GetID() >= 3000)
        {
          delete pPict;
          mPictures.RemoveAt(pos);
        }
      }
    }
  }
  return statOK;
}


#ifdef _DIAGNOSTIC
LPCTSTR CProject::toString() const
{
	static TCHAR buf[1024];
	_sntprintf( buf, _elements(buf), _T("CProject [%s] (%s forms, %s images, %s ActiveX controls)"),
																	 GetKeyName(),
																	 asString( mDclForms.GetCount() ),
																	 asString( mPictures.GetCount() ),
																	 asString( mOleControls.size() ) );
	return buf;
}

void CProject::dump( bool bDeep /*= true*/ ) const
{
	CString sOut;
	sOut.Format( _T("############################################################\r\n%s\r\n"), toString() );
	theWorkspace.DisplayStatus( sOut );
	if( !bDeep )
		return;
	POSITION pos = mDclForms.GetHeadPosition();
	while( pos )
		mDclForms.GetNext( pos )->dump( true );
	sOut.Format( _T("#####    End of project [%s]    #####\r\n"), GetKeyName() );
	theWorkspace.DisplayStatus( sOut );
}
#endif


#ifdef _DEBUG
void CProject::dumpDebugger( bool bDeep /*= true*/ ) const
{
	TraceFmt( _T("############################################################\r\n%s\r\n"), toString() );
	if( !bDeep )
		return;
	POSITION pos = mDclForms.GetHeadPosition();
	while( pos )
		mDclForms.GetNext( pos )->dumpDebugger( true );
	TraceFmt( _T("#####    End of project [%s]    #####\r\n"), GetKeyName() );
}
#endif
