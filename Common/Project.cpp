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
#include "AxContainerCtrl.h"
#include "StgFile.h"
#include "Filing.h"
#include "DclFormObject.h"
#include "DialogControl.h"
#include "PictureObject.h"
#include "ControlTypes.h"
#include "AxPropertyDescriptor.h"
#include "AxEventDescriptor.h"
#include "AxMethodDescriptor.h"
#include "ProjectCollection.h"
#include "Base64.h"
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
  msLispFileName.Empty();	
  mnAutoCADVersion = theWorkspace.GetMinSupportedAcadVersion();

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

void CProject::ClearProject()
{
  //msKeyName.Empty(); //this should remain unchanged for the life of the project unless explicitly changed!
	msLispFileName.Empty();
	msBaseFileName.Empty();
	msPassword.Empty();

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

bool CProject::AddPicture( CPictureObject* pPicture ) 
{
	assert( pPicture != NULL );
	if( !pPicture )
		return false;
	int nID = pPicture->GetID();
	POSITION posPic = mPictures.GetHeadPosition();
	while( posPic )
	{
		POSITION posAt = posPic;
		CPictureObject* pPictureAt = mPictures.GetNext( posPic );
		assert( pPictureAt != NULL );
		if( pPictureAt->GetID() == nID )
		{
			delete pPictureAt;
			mPictures.SetAt( posAt, pPicture );
			//update any instances of controls or forms referencing the updated picture
			POSITION posForm = mDclForms.GetHeadPosition();
			while( posForm )
			{
				CDclFormObject* pDclForm = mDclForms.GetNext( posForm );
				POSITION posControl = pDclForm->GetControlList().GetHeadPosition();
				while( posControl )
				{
					CDclControlObject* pDclControl = pDclForm->GetControlList().GetNext( posControl );
					if( !pDclControl->GetControlInstance() )
						continue; //no instance of this control, so skip it
					POSITION posProp = pDclControl->GetPropertyList().GetHeadPosition();
					while( posProp )
					{
						RefCountedPtr< CPropertyObject > pProp = pDclControl->GetPropertyList().GetNext( posProp );
						if( pProp->GetType() == PropPicture && pProp->GetLongValue() == nID )
							pDclControl->GetControlInstance()->OnApplyProperty( pProp ); //apply the updated picture
					}
				}
			}
			return true;
		}
	}
	mPictures.AddTail(pPicture);
	return true;
}

void CProject::DeletePicture( int nID )
{
	POSITION posPicToDelete = NULL;
	POSITION posPic = mPictures.GetHeadPosition();
	while( posPic )
	{
		POSITION posAt = posPic;
		CPictureObject* pPicture = mPictures.GetNext( posPic );
		assert( pPicture != NULL );
		if( pPicture->GetID() == nID )
		{
			posPicToDelete = posAt;
			break;
		}
	}
	if( !posPicToDelete )
		return;

	//remove all references to this picture before deleting it
	POSITION posForm = mDclForms.GetHeadPosition();
	while( posForm )
	{
		CDclFormObject* pDclForm = mDclForms.GetNext( posForm );
		POSITION posControl = pDclForm->GetControlList().GetHeadPosition();
		while( posControl )
		{
			CDclControlObject* pDclControl = pDclForm->GetControlList().GetNext( posControl );
			POSITION posProp = pDclControl->GetPropertyList().GetHeadPosition();
			while( posProp )
			{
				RefCountedPtr< CPropertyObject > pProp = pDclControl->GetPropertyList().GetNext( posProp );
				if( pProp->GetType() == PropPicture && pProp->GetLongValue() == nID )
				{
					pProp->SetShortValue( -1 ); //reset the picture ID
					if( pDclControl->GetControlInstance() )
						pDclControl->GetControlInstance()->OnApplyProperty( pProp ); //apply the deleted picture
				}
			}
		}
	}
	delete mPictures.GetAt( posPicToDelete );
	mPictures.RemoveAt( posPicToDelete );
}

bool CProject::LoadPictureFile( LPCTSTR szFile, int nID, bool bApplyMask /*= false*/ )
{
	CPictureObject* pPic = FindPicture( nID );
	if( !pPic )
	{
		pPic = new CPictureObject( nID );
		AddPicture( pPic );
	}
	pPic->LoadFile( szFile, bApplyMask );
	return true;
}

void CProject::AddOleObject(const CLSID& clsid, CAxContainerCtrl *pAxCont)
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

CDclFormObject* CProject::FindDclForm( LPCTSTR pszDclFormName ) const
{
	POSITION pos = mDclForms.GetHeadPosition();
	while( pos )
	{
    CDclFormObject* pDclForm = mDclForms.GetNext( pos );
    if( pDclForm->GetKeyName().CompareNoCase( pszDclFormName ) == 0 )
      return pDclForm;
	}
  return NULL;
}

CDclFormObject* CProject::FindDclFormWithVarName( LPCTSTR pszVarName ) const
{
	POSITION pos = mDclForms.GetHeadPosition();
	while( pos )
	{
    CDclFormObject* pDclForm = mDclForms.GetNext( pos );
    if( pDclForm->GetControlProperties()->GetStrProperty( nGlobalVarName ).CompareNoCase( pszVarName ) == 0 )
      return pDclForm;
	}
  return NULL;
}

CDclControlObject* CProject::FindControlWithVarName( LPCTSTR pszVarName ) const
{
	POSITION pos = mDclForms.GetHeadPosition();
	while( pos )
	{
    CDclFormObject* pDclForm = mDclForms.GetNext( pos );
		CDclControlObject* pControl = pDclForm->FindControlWithVarName( pszVarName );
    if( pControl )
      return pControl;
	}
  return NULL;
}

CDclFormObject* CProject::FindParentDclForm( LPCTSTR pszParentFormName ) const
{
	POSITION pos = mDclForms.GetHeadPosition();
	while( pos )
	{
		CDclFormObject* pDclForm = mDclForms.GetNext( pos );
    if( pDclForm->GetUniqueName() == pszParentFormName )
      return pDclForm;
	}
  return NULL;
}

CDclFormObject* CProject::FindDclTabChildForm( LPCTSTR pszParentFormName, int nTabIndex ) const
{
	POSITION pos = mDclForms.GetHeadPosition();
	while (pos)
	{
		CDclFormObject* pDclForm = mDclForms.GetNext(pos);
    if (pDclForm->GetParentName() == pszParentFormName && pDclForm->GetTabIndex() == nTabIndex)
      return pDclForm;
	}
  return NULL;
  return NULL;
}

size_t CProject::CountDeletedForms() const
{
  size_t ctDeleted = 0;
  POSITION pos = mDclForms.GetHeadPosition();
  while( pos )
  {
    CDclFormObject* pDclForm = mDclForms.GetNext( pos );
    if( pDclForm->m_bDeleted )
      ++ctDeleted;
  }
  return ctDeleted;
}

HBITMAP CProject::CloneBitmap(UINT nID, CSize &sz) const
{
	CPictureObject* pPicture = FindPicture( nID );
	if( !pPicture )
		return NULL;
	sz.SetSize( pPicture->GetWidth(), pPicture->GetHeight() );
	return pPicture->CloneBitmap();
}

HICON CProject::CloneIcon(UINT nID) const
{
	CPictureObject* pPicture = FindPicture( nID );
	if( !pPicture )
		return NULL;
	return pPicture->CloneIcon();
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
	CPictureObject* pPicture = FindPicture( nID );
	if( !pPicture )
		return false;
	if (!pPicture->IsValid())
		return false;
	size.cx = pPicture->GetWidth();
	size.cy = pPicture->GetHeight();
	return true;
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
	CString sExt = CString( pszFilePath ).Right( 4 );
	if( sExt.CompareNoCase( _T(".lsp") ) == 0 )
	{
		try
		{
			CStdioFile SourceFile( pszFilePath, CFile::modeRead );
			if (!SourceFile)
				return statFileNotFound;
			CStringA sRawData;
			UINT cchData = SourceFile.Read( sRawData.GetBuffer( (int)SourceFile.GetLength() ), (UINT)SourceFile.GetLength() );
			sRawData.ReleaseBufferSetLength( cchData );
			if( cchData == 0 )
				return statReadFailed;
			std::string sData = base64_decode( (LPCSTR)sRawData );
			if( sData.length() == 0 )
				return statReadFailed;
			CMemFile Data( sData.length() );
			Data.Write( sData.c_str(), sData.length() );
			Data.SeekToBegin();
			try
			{
				CArchiveEx ar( &Data, CArchive::load, NULL, _T("ObjectDCL"), TRUE);
				Serialize( ar );
				ar.Close();
			}
			catch(...)
			{
				return statInvalidFormat;
			}
		}
		catch(...)
		{
			return statFileNotFound;
		}
		return statOK;
	}

	CFile SrcFile;
	try
	{
		SrcFile.Open( pszFilePath, CFile::modeRead | CFile::shareDenyWrite | CFile::osSequentialScan );
		if (!SrcFile)
			return statFileNotFound;

		msProjectFilePath = SrcFile.GetFilePath();
		msBaseFileName = StripPathFromFileName( msProjectFilePath );

		//Check the first line -- if it is "OpenDCL Project", then it is a text file.
		char szTextFileSignature[] = "OpenDCL Project";
		char szLine1[sizeof(szTextFileSignature)];
		UINT_PTR cbRead = SrcFile.Read(szLine1, sizeof(szLine1) - 1);
		szLine1[cbRead] = '\0';

		if( lstrcmpA( szTextFileSignature, szLine1 ) == 0 )
		{ //this file is in plain text format
			SrcFile.Close();
			IOStatus stat = ReadFromTextFile( pszFilePath );
			if( msKeyName.IsEmpty() )
				SetKeyName( msBaseFileName.SpanExcluding( _T(".") ) );
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
			if( msKeyName.IsEmpty() )
				SetKeyName( msBaseFileName.SpanExcluding( _T(".") ) );
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
	try
	{
		CString sExt = CString( pszFilePath ).Right( 4 );
		if( sExt.CompareNoCase( _T(".lsp") ) == 0 )
		{
			CStdioFile DestFile( pszFilePath, CFile::modeWrite | CFile::shareExclusive | CFile::modeCreate );
			if (!DestFile)
				return statFileNotFound;
			CMemFile Data( 0x10000 );
			CArchiveEx ar( &Data, CArchive::store | CArchive::bNoFlushOnDelete, NULL, _T("ObjectDCL"), TRUE);
			Serialize( ar );
			ar.Close();
			UINT cbData = (UINT)Data.GetLength();
			if( cbData == 0 )
				return statWriteFailed;
			BYTE* pbData = Data.Detach();
			CString sRawData = base64_encode( pbData, cbData ).c_str();
			Data.Attach( pbData, cbData );
			Data.Close();
			sRawData.Replace( _T("\r\n"), _T("\"\n\"") );
			CString sFormattedData;
			sFormattedData.Format( _T("'(\"%s\")"), (LPCTSTR)sRawData );
			DestFile.Write( sFormattedData, sFormattedData.GetLength() );
			DestFile.Flush();
		}
		else
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
	}
	catch(...)
	{
		return statFail;
	}
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

  if (ar.IsStoring())
  {
    ar << GetCurrentSaveVersion();
    ar << msPassword;
    ar << msLispFileName;
    ar << msKeyName; //project key

    ar << unsigned long(mDclForms.GetCount() - CountDeletedForms());
    ar << mnAutoCADVersion;

    POSITION pos = mDclForms.GetHeadPosition();
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

		if( nThisVersion < 12 )
		{
			BOOL bHasPassword;
			ar >> bHasPassword; //discard
			// this is a fix for an older password protect scheme.
			if (nThisVersion != 2 && nThisVersion != 3)
				msPassword.Empty();
		}
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

		if( nThisVersion < 12 && nThisVersion >= 6 )
		{
			CString sDistFileName;
			ar >> sDistFileName; //discard
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
		
		if( nThisVersion < 12 )
		{
			int nPurchaseState;
			ar >> nPurchaseState; //discard
		}

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
				POSITION pos = listControls.GetHeadPosition();
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

		POSITION posPic = mPictures.GetHeadPosition();
		while( posPic )
			delete mPictures.GetNext( posPic );
    mPictures.RemoveAll();

    if (nThisVersion < 9)
		{
			try
			{
				mPictures.Serialize(ar);

			// this has been added to cleanup picture objects with a blank picture.
			POSITION pos = mPictures.GetHeadPosition();
			while( pos )
			{
				POSITION posAt = pos;
				CPictureObject* pPict = mPictures.GetNext(pos);
				if (pPict == NULL)
					mPictures.RemoveAt( posAt );
				else if( pPict->GetID() <= -1 || pPict->GetID() >= 3000 )
				{
					delete pPict;
					mPictures.RemoveAt( posAt );
				}
			}
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

      while (nCount-- > 0)
      {
        CPictureObject* pPictureObj = new CPictureObject( -1 );
        pPictureObj->Serialize(ar);
        mPictures.AddTail(pPictureObj);							
      }
    }
  }
}

IOStatus CProject::ReadFromTextFile( LPCTSTR lpszFilePath ) 
{
	IOStatus stat = statOK;
	std::ifstream sFile(lpszFilePath, std::ios::in);
	InitFilerGlobals(); //Init the globals before reading anything from the file.
	if (readLine(sFile) != "OpenDCL Project")
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

  if (readLine(sFile) != "OpenDCL Project") return statInvalidFormat;

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
  if (!readBOOL(sFile, bHasPassword)) return statInvalidFormat; //discard
  if (!readString(sFile, msPassword)) return statInvalidFormat;
  if (!readString(sFile, msLispFileName)) return statInvalidFormat;
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
	CString sDistFileName;
  if (!readString(sFile, sDistFileName)) return statInvalidFormat; //discard

  // get counter for dcl forms
  if (!readInt(sFile, nCount)) return statInvalidFormat;
	int nPurchaseState;
  if (!readInt(sFile, nPurchaseState)) return statInvalidFormat; //discard

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

	POSITION posPic = mPictures.GetHeadPosition();
	while( posPic )
		delete mPictures.GetNext( posPic );
  mPictures.RemoveAll();
  // do loop to navigate images
  while (nCount-- > 0)
  {
    // get current images
    CPictureObject* pPictureObj = new CPictureObject( -1 );
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
